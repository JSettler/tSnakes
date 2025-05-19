#include "bot.h"
#include "common.h" 
#include <iostream>      
#include <cmath>         
#include <cstdlib>       
#include <vector>        
#include <algorithm>     // For std::random_shuffle, std::min, std::max

// Definition for createGenericHeadTexture
/* sf::Texture createGenericHeadTexture(unsigned int size, float outlineThicknessFloat,
                                    const sf::Color& fillColor, const sf::Color& outlineColor,
                                    const sf::Color& cornerPixelColor) {
    sf::Image image; image.create(size, size, sf::Color::Transparent); 
    int outlineThickness = static_cast<int>(std::round(outlineThicknessFloat));
    if (outlineThickness < 1) outlineThickness = 1; 
    if (static_cast<unsigned int>(outlineThickness * 2) > size) {
        outlineThickness = size / 2;
        if (outlineThickness == 0 && size > 0) outlineThickness = 1; else if (size == 0) outlineThickness = 0;
    }
    if (size > 0) {
        if (size > static_cast<unsigned int>(outlineThickness * 2)) {
            for (unsigned int y = outlineThickness; y < size - outlineThickness; ++y) 
                for (unsigned int x = outlineThickness; x < size - outlineThickness; ++x) image.setPixel(x, y, fillColor);
        } else { 
            for (unsigned int y = 0; y < size; ++y) for (unsigned int x = 0; x < size; ++x) image.setPixel(x, y, fillColor);
        }
        if (outlineThickness > 0) {
            for (int i_ot = 0; i_ot < outlineThickness; ++i_ot) { 
                if (static_cast<unsigned int>(i_ot) >= size) break;
                for (unsigned int x = 0; x < size; ++x) {
                    image.setPixel(x, i_ot, outlineColor);
                    if (static_cast<unsigned int>(size - 1 - i_ot) < size) image.setPixel(x, size - 1 - i_ot, outlineColor);
                }
            }
            for (int i_ot = 0; i_ot < outlineThickness; ++i_ot) { 
                if (static_cast<unsigned int>(i_ot) >= size) break;
                for (unsigned int y = outlineThickness; y < size - outlineThickness; ++y) {
                    image.setPixel(i_ot, y, outlineColor);
                    if (static_cast<unsigned int>(size - 1 - i_ot) < size) image.setPixel(size - 1 - i_ot, y, outlineColor);
                }
            }
        }
        if (size >= 3) { 
            sf::Color cpc = cornerPixelColor; 
            image.setPixel(0, 0, cpc); image.setPixel(1, 0, cpc); image.setPixel(0, 1, cpc);
            image.setPixel(size-1, 0, cpc); image.setPixel(size-2, 0, cpc); image.setPixel(size-1, 1, cpc);
            image.setPixel(0, size-1, cpc); image.setPixel(1, size-1, cpc); image.setPixel(0, size-2, cpc);
            image.setPixel(size-1, size-1, cpc); image.setPixel(size-2, size-1, cpc); image.setPixel(size-1, size-2, cpc);
        } else if (size > 0) { 
             for(unsigned int y_img=0; y_img<size; ++y_img) for(unsigned int x_img=0; x_img<size; ++x_img) image.setPixel(x_img,y_img, cornerPixelColor);
        }
    }
    sf::Texture texture; 
    if (!texture.loadFromImage(image)) {
        if (G_CONFIG.debugMode) std::cerr << "Error: Could not create generic head texture from image for bot." << std::endl;
    }
    return texture;
} */

// Helper function for bot AI to check if a target tile is safe to move to
bool isSafeBotMove(sf::Vector2i targetPos, int botSelfID, bool isBotClaiming, const std::vector<sf::Vector2i>& botTail, const std::vector<std::vector<Tile>>& mapRef) {
    // Check map boundaries (uses G_CONFIG from common.h, defined in common.cpp)
    if (targetPos.x < 0 || targetPos.x >= G_CONFIG.mapWidthTiles ||
        targetPos.y < 0 || targetPos.y >= G_CONFIG.mapHeightTiles) {
        return false; 
    }

    // Check for collision with own tail
    if (isBotClaiming && !botTail.empty()) {
        for (const auto& tailSegment : botTail) {
            if (tailSegment == targetPos) {
                return false; // Target is part of the existing tail
            }
        }
    }
    return true;
}


Bot::Bot(int bot_id, sf::Color primary_active_c, sf::Color safe_head_c, sf::Vector2i start_pos)
    : id(bot_id), gridPos(start_pos), 
      activeColor(primary_active_c), 
      safeHeadColor(safe_head_c),
      currentDirection(Direction::NONE), 
      isMovingToTarget(false),
      isClaiming(false),
      alive(true),
      decisionTimer(0.0f), 
      timeSinceLastTurn(0.0f),
      currentAIState(AIState::EXPLORING), 
      stepsInCurrentDirection(0),
      loopLegCounter(0),
      initialSpawnPoint(start_pos), 
      loopStartPoint(start_pos) {
    
    territoryColor = sf::Color(activeColor.r / 2, activeColor.g / 2, activeColor.b / 2);
    visualPos = sf::Vector2f(gridPos.x * TILE_SIZE + HEAD_MARGIN, gridPos.y * TILE_SIZE + HEAD_MARGIN);
    targetVisualPos = visualPos; 
    headSprite.setPosition(visualPos);
    initializeTextures(); 
}

void Bot::initializeTextures() {
    sf::Color botOutlineColor = sf::Color(100,100,100); 
    sf::Color botCornerColor = sf::Color::Black;    
    activeHeadTexture = createGenericHeadTexture(HEAD_SIZE_INT, 1.0f, activeColor, botOutlineColor, botCornerColor);
    safeHeadTexture = createGenericHeadTexture(HEAD_SIZE_INT, 1.0f, safeHeadColor, botOutlineColor, botCornerColor);
    headSprite.setTexture(safeHeadTexture); 
}

void Bot::die(std::vector<std::vector<Tile>>& gameMapRef) {
    if (!alive) return; 

    if (G_CONFIG.debugMode) std::cout << "Bot " << id << " has died." << std::endl;
    alive = false; 
    isClaiming = false; 
    tail.clear();

    for (int y = 0; y < G_CONFIG.mapHeightTiles; ++y) { 
        for (int x = 0; x < G_CONFIG.mapWidthTiles; ++x) { 
            if (gameMapRef[x][y].baseOwner == TileBaseOwner::BOT && gameMapRef[x][y].entityID == id) {
                gameMapRef[x][y].baseOwner = TileBaseOwner::NEUTRAL;
                gameMapRef[x][y].entityID = -1; 
            }
        }
    }
    // For instant respawn, no timer logic needed here. Main.cpp will trigger respawn.
}

void Bot::respawn(std::vector<std::vector<Tile>>& gameMapRef, sf::Vector2i newStartPos) {
    if (G_CONFIG.debugMode) std::cout << "Bot " << id << " AGGRESSIVELY respawning at (" << newStartPos.x << "," << newStartPos.y << ")." << std::endl;
    
    gridPos = newStartPos;
    initialSpawnPoint = newStartPos; 
    loopStartPoint = newStartPos;    
    visualPos = sf::Vector2f(gridPos.x * TILE_SIZE + HEAD_MARGIN, gridPos.y * TILE_SIZE + HEAD_MARGIN);
    headSprite.setPosition(visualPos);

    alive = true; 
    isClaiming = false;
    tail.clear();
    currentDirection = Direction::NONE; 
    currentAIState = AIState::EXPLORING; 
    stepsInCurrentDirection = 0;
    loopLegCounter = 0;
    
    isMovingToTarget = false;         
    targetVisualPos = visualPos;      
    
    int botRespawnTerritorySize = 5; 
    for (int r = -botRespawnTerritorySize / 2; r <= botRespawnTerritorySize / 2; ++r) {
        for (int c = -botRespawnTerritorySize / 2; c <= botRespawnTerritorySize / 2; ++c) {
            int tileX = gridPos.x + c;
            int tileY = gridPos.y + r;
            if (tileX >= 0 && tileX < G_CONFIG.mapWidthTiles && 
                tileY >= 0 && tileY < G_CONFIG.mapHeightTiles) {
                
                if (gameMapRef[tileX][tileY].baseOwner == TileBaseOwner::PLAYER && gameMapRef[tileX][tileY].entityID == PLAYER_ENTITY_ID) {
                    if (G_CONFIG.debugMode) std::cout << "Bot " << id << " respawn overwriting player territory at (" << tileX << "," << tileY << ")" << std::endl;
                } else if (gameMapRef[tileX][tileY].baseOwner == TileBaseOwner::BOT && gameMapRef[tileX][tileY].entityID != id) {
                     if (G_CONFIG.debugMode) std::cout << "Bot " << id << " respawn overwriting territory of bot " << gameMapRef[tileX][tileY].entityID << " at (" << tileX << "," << tileY << ")" << std::endl;
                }
                gameMapRef[tileX][tileY].baseOwner = TileBaseOwner::BOT;
                gameMapRef[tileX][tileY].entityID = id;
            }
        }
    }
    if (G_CONFIG.debugMode) std::cout << "Bot " << id << " territory forcefully re-established." << std::endl;
}

void Bot::update(float deltaTime, 
                 std::vector<std::vector<Tile>>& gameMapRef, 
                 const std::vector<sf::Vector2i>& playerTailRef, 
                 bool isPlayerClaimingRef,
                 sf::Vector2i playerGridPosRef, 
                 std::vector<Bot>& allBots) {
    if (!alive) return; 

    timeSinceLastTurn += deltaTime;
    decisionTimer += deltaTime;

    bool onOwnTerritory = false;
    if (gridPos.x >=0 && gridPos.x < G_CONFIG.mapWidthTiles && gridPos.y >=0 && gridPos.y < G_CONFIG.mapHeightTiles) {
        onOwnTerritory = (gameMapRef[gridPos.x][gridPos.y].baseOwner == TileBaseOwner::BOT &&
                          gameMapRef[gridPos.x][gridPos.y].entityID == id);
    }
    headSprite.setTexture(onOwnTerritory ? safeHeadTexture : activeHeadTexture);

    if (!isMovingToTarget) {
        if (isClaiming && tail.size() > 2) { 
            for (size_t i = 0; i < tail.size() - 1; ++i) { 
                if (tail[i] == gridPos) {
                    if (G_CONFIG.debugMode) std::cout << "Bot " << id << " hit its own tail at (" << gridPos.x << "," << gridPos.y <<")." << std::endl;
                    die(gameMapRef); return; 
                }
            }
        }
        for (Bot& otherBot : allBots) { 
            if (!otherBot.alive || otherBot.id == id || !otherBot.isClaiming) continue; 
            bool collisionWithOtherBotTail = false;
            for (const auto& otherTailPos : otherBot.tail) {
                if (gridPos == otherTailPos) { collisionWithOtherBotTail = true; break; }
            }
            if (collisionWithOtherBotTail) {
                if (G_CONFIG.debugMode) std::cout << "Bot " << id << " hit tail of Bot " << otherBot.id << ". Bot " << otherBot.id << " dies." << std::endl;
                otherBot.die(gameMapRef); 
            }
        }
        if (!alive) return; 

        bool justArrivedOnOwnTerritory = (gameMapRef[gridPos.x][gridPos.y].baseOwner == TileBaseOwner::BOT &&
                                          gameMapRef[gridPos.x][gridPos.y].entityID == id);
        if (isClaiming) {
            if (justArrivedOnOwnTerritory) { 
                isClaiming = false; 
                currentAIState = AIState::EXPLORING; 
                stepsInCurrentDirection = 0; loopLegCounter = 0;
                if (G_CONFIG.debugMode) std::cout << "Bot " << id << " re-entered territory. Tail ready for main. Length: " << tail.size() << std::endl;
            } else { 
                if (tail.empty() || tail.back() != gridPos) tail.push_back(gridPos);
            }
        } else { 
            if (!justArrivedOnOwnTerritory) { 
                isClaiming = true; tail.clear(); tail.push_back(gridPos); 
                currentAIState = AIState::MAKING_LOOP_LEG1; 
                stepsInCurrentDirection = 0; loopLegCounter = 0;
                loopStartPoint = gridPos; 
                currentDirection = Direction::NONE; 
                isMovingToTarget = false;         
                
                bool initialDirFound = false;
                Direction potentialInitialDirs[] = {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT};
                std::random_shuffle(std::begin(potentialInitialDirs), std::end(potentialInitialDirs));
                for(Direction dir : potentialInitialDirs) {
                    sf::Vector2i nextTestPos = gridPos;
                    if (dir == Direction::UP) nextTestPos.y--; else if (dir == Direction::DOWN) nextTestPos.y++;
                    else if (dir == Direction::LEFT) nextTestPos.x--; else if (dir == Direction::RIGHT) nextTestPos.x++;
                    if (isSafeBotMove(nextTestPos, id, true, tail, gameMapRef)) { 
                        currentDirection = dir; initialDirFound = true; break;
                    }
                }
                if (!initialDirFound) { 
                    if (G_CONFIG.debugMode) std::cout << "Bot " << id << " cannot find safe start for loop. Suiciding." << std::endl;
                    die(gameMapRef); return;
                }
                if (G_CONFIG.debugMode) std::cout << "Bot " << id << " started tail. AI State: LOOP_LEG1, Dir: " << static_cast<int>(currentDirection) << std::endl;
            }
        }
        if (!alive) return; 

        Direction chosenDirThisFrame = Direction::NONE; 
        const int LOOP_X_MIN = 3, LOOP_X_MAX = 5; 
        const int LOOP_Y_MIN = 2, LOOP_Y_MAX = 4; 
        int currentLoopXLength = LOOP_X_MIN + (rand() % (LOOP_X_MAX - LOOP_X_MIN + 1));
        int currentLoopYLength = LOOP_Y_MIN + (rand() % (LOOP_Y_MAX - LOOP_Y_MIN + 1));

        if (currentAIState == AIState::EXPLORING) {
            if (currentDirection == Direction::NONE || stepsInCurrentDirection > (4 + rand() % 7) || decisionTimer > 0.6f) {
                Direction dirs[] = {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT};
                std::random_shuffle(std::begin(dirs), std::end(dirs)); 
                for(Direction d : dirs) {
                    if (!isOppositeDirection(currentDirection, d) || currentDirection == Direction::NONE) {
                        sf::Vector2i nextTestPos = gridPos;
                        if (d == Direction::UP) nextTestPos.y--; else if (d == Direction::DOWN) nextTestPos.y++;
                        else if (d == Direction::LEFT) nextTestPos.x--; else if (d == Direction::RIGHT) nextTestPos.x++;
                        if (isSafeBotMove(nextTestPos, id, isClaiming, tail, gameMapRef)) {chosenDirThisFrame = d; break;}
                    }
                }
                if (chosenDirThisFrame == Direction::NONE && currentDirection != Direction::NONE) { 
                    sf::Vector2i nextTestPos = gridPos;
                    if (currentDirection == Direction::UP) nextTestPos.y--; else if (currentDirection == Direction::DOWN) nextTestPos.y++;
                    else if (currentDirection == Direction::LEFT) nextTestPos.x--; else if (currentDirection == Direction::RIGHT) nextTestPos.x++;
                    if (isSafeBotMove(nextTestPos, id, isClaiming, tail, gameMapRef)) chosenDirThisFrame = currentDirection;
                }
                if (chosenDirThisFrame == Direction::NONE) { 
                     for(Direction d : dirs) { 
                        sf::Vector2i nextTestPos = gridPos;
                        if (d == Direction::UP) nextTestPos.y--; else if (d == Direction::DOWN) nextTestPos.y++;
                        else if (d == Direction::LEFT) nextTestPos.x--; else if (d == Direction::RIGHT) nextTestPos.x++;
                        if (isSafeBotMove(nextTestPos, id, isClaiming, tail, gameMapRef)) {chosenDirThisFrame = d; break;}
                     }
                }
                stepsInCurrentDirection = 0; decisionTimer = 0.0f;
            } else {
                chosenDirThisFrame = currentDirection;
            }
        } 
        else if (currentAIState == AIState::MAKING_LOOP_LEG1) {
            if (stepsInCurrentDirection < currentLoopXLength) chosenDirThisFrame = currentDirection;
            else { currentAIState = AIState::MAKING_LOOP_LEG2; stepsInCurrentDirection = 0; loopLegCounter = 1;
                   if (currentDirection==Direction::RIGHT) chosenDirThisFrame=Direction::DOWN; else if (currentDirection==Direction::LEFT) chosenDirThisFrame=Direction::UP; 
                   else if (currentDirection==Direction::UP) chosenDirThisFrame=Direction::RIGHT; else if (currentDirection==Direction::DOWN) chosenDirThisFrame=Direction::LEFT;
                   else chosenDirThisFrame = (rand()%2 == 0) ? Direction::UP : Direction::DOWN; 
            }
        } else if (currentAIState == AIState::MAKING_LOOP_LEG2) {
             if (stepsInCurrentDirection < currentLoopYLength) chosenDirThisFrame = currentDirection;
            else { currentAIState = AIState::MAKING_LOOP_LEG3; stepsInCurrentDirection = 0; loopLegCounter = 2;
                   if (currentDirection==Direction::DOWN) chosenDirThisFrame=Direction::LEFT; else if (currentDirection==Direction::UP) chosenDirThisFrame=Direction::RIGHT;
                   else if (currentDirection==Direction::RIGHT) chosenDirThisFrame=Direction::UP; else if (currentDirection==Direction::LEFT) chosenDirThisFrame=Direction::DOWN;
                   else chosenDirThisFrame = (rand()%2 == 0) ? Direction::LEFT : Direction::RIGHT;
            }
        } else if (currentAIState == AIState::MAKING_LOOP_LEG3) {
            if (stepsInCurrentDirection < currentLoopXLength) chosenDirThisFrame = currentDirection;
            else { currentAIState = AIState::RETURNING_HOME; stepsInCurrentDirection = 0; loopLegCounter = 3;
                   if (currentDirection==Direction::LEFT) chosenDirThisFrame=Direction::UP; else if (currentDirection==Direction::RIGHT) chosenDirThisFrame=Direction::DOWN;
                   else if (currentDirection==Direction::UP) chosenDirThisFrame=Direction::LEFT; else if (currentDirection==Direction::DOWN) chosenDirThisFrame=Direction::RIGHT;
                   else chosenDirThisFrame = (rand()%2 == 0) ? Direction::UP : Direction::DOWN;
            }
        } 
        else if (currentAIState == AIState::RETURNING_HOME) {
            Direction homeDirs[] = {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT};
            std::random_shuffle(std::begin(homeDirs), std::end(homeDirs)); 
            for(Direction d_home : homeDirs) {
                sf::Vector2i nextPos = gridPos;
                if (d_home == Direction::UP) nextPos.y--; else if (d_home == Direction::DOWN) nextPos.y++;
                else if (d_home == Direction::LEFT) nextPos.x--; else if (d_home == Direction::RIGHT) nextPos.x++;
                if (nextPos.x >= 0 && nextPos.x < G_CONFIG.mapWidthTiles && nextPos.y >= 0 && nextPos.y < G_CONFIG.mapHeightTiles &&
                    gameMapRef[nextPos.x][nextPos.y].baseOwner == TileBaseOwner::BOT && 
                    gameMapRef[nextPos.x][nextPos.y].entityID == id &&
                    isSafeBotMove(nextPos, id, isClaiming, tail, gameMapRef)) {
                    chosenDirThisFrame = d_home; 
                    if(G_CONFIG.debugMode) std::cout << "Bot " << id << " returning home, found owned tile target." << std::endl;
                    break;
                }
            }
            if (chosenDirThisFrame == Direction::NONE) { 
                int dx_home = initialSpawnPoint.x - gridPos.x; int dy_home = initialSpawnPoint.y - gridPos.y;
                std::vector<Direction> preferredDirs;
                if (dx_home > 0) preferredDirs.push_back(Direction::RIGHT); else if (dx_home < 0) preferredDirs.push_back(Direction::LEFT);
                if (dy_home > 0) preferredDirs.push_back(Direction::DOWN); else if (dy_home < 0) preferredDirs.push_back(Direction::UP);
                if (preferredDirs.empty() && (dx_home != 0 || dy_home != 0)) { 
                    if (dx_home != 0) preferredDirs.push_back((dx_home > 0) ? Direction::RIGHT : Direction::LEFT);
                    if (dy_home != 0) preferredDirs.push_back((dy_home > 0) ? Direction::DOWN : Direction::UP);
                }
                std::random_shuffle(preferredDirs.begin(), preferredDirs.end()); 
                for(Direction d : preferredDirs) { 
                    sf::Vector2i nextTestPos = gridPos;
                    if (d == Direction::UP) nextTestPos.y--; else if (d == Direction::DOWN) nextTestPos.y++;
                    else if (d == Direction::LEFT) nextTestPos.x--; else if (d == Direction::RIGHT) nextTestPos.x++;
                    if (isSafeBotMove(nextTestPos, id, isClaiming, tail, gameMapRef)) {chosenDirThisFrame = d; break;}
                }
                if (chosenDirThisFrame == Direction::NONE) { 
                    std::random_shuffle(std::begin(homeDirs), std::end(homeDirs)); 
                    for(Direction d : homeDirs) { 
                        sf::Vector2i nextTestPos = gridPos;
                        if (d == Direction::UP) nextTestPos.y--; else if (d == Direction::DOWN) nextTestPos.y++;
                        else if (d == Direction::LEFT) nextTestPos.x--; else if (d == Direction::RIGHT) nextTestPos.x++;
                        if (isSafeBotMove(nextTestPos, id, isClaiming, tail, gameMapRef)) {chosenDirThisFrame = d; break;}
                    }
                }
                if (chosenDirThisFrame == Direction::NONE) { 
                    currentAIState = AIState::EXPLORING; 
                    if (G_CONFIG.debugMode) std::cout << "Bot " << id << " stuck returning home, resetting to EXPLORING." << std::endl;
                }
            }
            if (isClaiming && currentDirection != Direction::NONE && chosenDirThisFrame != Direction::NONE && isOppositeDirection(currentDirection, chosenDirThisFrame) ) { 
                Direction originalChosenDir = chosenDirThisFrame; chosenDirThisFrame = Direction::NONE;
                Direction altDirs[] = {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT};
                std::random_shuffle(std::begin(altDirs), std::end(altDirs));
                for (Direction d_alt : altDirs) {
                    if (d_alt != originalChosenDir && (!isOppositeDirection(currentDirection, d_alt) || currentDirection == Direction::NONE)) {
                         sf::Vector2i nextTestPos = gridPos;
                         if (d_alt==Direction::UP)nextTestPos.y--; else if(d_alt==Direction::DOWN)nextTestPos.y++; else if(d_alt==Direction::LEFT)nextTestPos.x--; else if(d_alt==Direction::RIGHT)nextTestPos.x++;
                         if (isSafeBotMove(nextTestPos, id, isClaiming, tail, gameMapRef)) {chosenDirThisFrame = d_alt; break;}
                    }
                }
                if (chosenDirThisFrame == Direction::NONE) chosenDirThisFrame = originalChosenDir; 
            }
            if (tail.size() > (G_CONFIG.mapWidthTiles + G_CONFIG.mapHeightTiles) / 2 ) { 
                 currentAIState = AIState::EXPLORING;
                 if (G_CONFIG.debugMode) std::cout << "Bot " << id << " tail too long while returning, resetting to EXPLORING." << std::endl;
            }
        }

        if (chosenDirThisFrame != Direction::NONE) {
            sf::Vector2i targetGridPosCalc = gridPos;
            if (chosenDirThisFrame == Direction::UP) targetGridPosCalc.y--; 
            else if (chosenDirThisFrame == Direction::DOWN) targetGridPosCalc.y++;
            else if (chosenDirThisFrame == Direction::LEFT) targetGridPosCalc.x--; 
            else if (chosenDirThisFrame == Direction::RIGHT) targetGridPosCalc.x++;

            if (isSafeBotMove(targetGridPosCalc, id, isClaiming, tail, gameMapRef)) {
                currentDirection = chosenDirThisFrame;
                targetVisualPos = sf::Vector2f(targetGridPosCalc.x * TILE_SIZE + HEAD_MARGIN, 
                                               targetGridPosCalc.y * TILE_SIZE + HEAD_MARGIN);
                isMovingToTarget = true;
                timeSinceLastTurn = 0.0f;
                if (currentAIState == AIState::MAKING_LOOP_LEG1 || currentAIState == AIState::MAKING_LOOP_LEG2 || currentAIState == AIState::MAKING_LOOP_LEG3) {
                     stepsInCurrentDirection++;
                } else {
                    stepsInCurrentDirection = 0; 
                }
            } else { 
                if (G_CONFIG.debugMode) std::cout << "Bot " << id << " final chosen dir (" << static_cast<int>(chosenDirThisFrame) << ") was unsafe. Resetting AI." << std::endl;
                currentDirection = Direction::NONE; 
                currentAIState = AIState::EXPLORING; 
                stepsInCurrentDirection = 1000; 
            }
        } else { 
            if (G_CONFIG.debugMode) std::cout << "Bot " << id << " is STUCK (no safe move found after AI logic). Initiating self-destruct." << std::endl;
            die(gameMapRef); 
            return; 
        }
    } 

    if (isMovingToTarget) {
        float botSpeedPixels = G_CONFIG.botSpeed * TILE_SIZE; 
        sf::Vector2f dirVec = targetVisualPos - visualPos;
        float dist = std::sqrt(dirVec.x * dirVec.x + dirVec.y * dirVec.y);
        float snapThreshold = botSpeedPixels * deltaTime * 0.5f;
        if (dist < snapThreshold || dist < 0.1f) {
            visualPos = targetVisualPos;
            gridPos = visualHeadPosToGrid(visualPos); 
            isMovingToTarget = false; 
        } else {
            sf::Vector2f normDir = dirVec / dist;
            visualPos += normDir * botSpeedPixels * deltaTime;
        }
        headSprite.setPosition(visualPos);
    }
}

void Bot::renderTail(sf::RenderWindow& window, int cStartX, int cEndX, int cStartY, int cEndY) const { 
    if (!alive || !isClaiming || tail.empty()) return;
    if (tail.size() >= 1) {
        sf::VertexArray tailVertices(sf::TriangleStrip);
        float halfThickness = SNAKE_TAIL_THICKNESS / 2.0f; 
        if (tail.size() == 1) {
            sf::RectangleShape capShape(sf::Vector2f(SNAKE_TAIL_THICKNESS, SNAKE_TAIL_THICKNESS));
            capShape.setFillColor(activeColor); 
            capShape.setOrigin(halfThickness, halfThickness);
            capShape.setPosition(tail[0].x*TILE_SIZE + TILE_SIZE/2.0f, tail[0].y*TILE_SIZE + TILE_SIZE/2.0f);
            if (tail[0].x >= cStartX && tail[0].x < cEndX && tail[0].y >= cStartY && tail[0].y < cEndY) window.draw(capShape);
        } else { 
            for (size_t i = 0; i < tail.size(); ++i) {
                sf::Vector2f cP(tail[i].x*TILE_SIZE+TILE_SIZE/2.0f,tail[i].y*TILE_SIZE+TILE_SIZE/2.0f);
                sf::Vector2f dN; 
                if(i==0){sf::Vector2f nP(tail[i+1].x*TILE_SIZE+TILE_SIZE/2.0f,tail[i+1].y*TILE_SIZE+TILE_SIZE/2.0f); sf::Vector2f sD=nP-cP; float l=std::sqrt(sD.x*sD.x+sD.y*sD.y); if(l>0)sD/=l; else sD=sf::Vector2f(1,0); dN=sf::Vector2f(-sD.y,sD.x); }
                else if(i==tail.size()-1){sf::Vector2f pP(tail[i-1].x*TILE_SIZE+TILE_SIZE/2.0f,tail[i-1].y*TILE_SIZE+TILE_SIZE/2.0f); sf::Vector2f sD=cP-pP; float l=std::sqrt(sD.x*sD.x+sD.y*sD.y); if(l>0)sD/=l; else sD=sf::Vector2f(1,0); dN=sf::Vector2f(-sD.y,sD.x); }
                else{sf::Vector2f nP(tail[i+1].x*TILE_SIZE+TILE_SIZE/2.0f,tail[i+1].y*TILE_SIZE+TILE_SIZE/2.0f); sf::Vector2f sD=nP-cP; float l=std::sqrt(sD.x*sD.x+sD.y*sD.y); if(l>0)sD/=l; else sD=sf::Vector2f(1,0); dN=sf::Vector2f(-sD.y,sD.x);}
                sf::Vertex v1,v2; v1.position=cP+dN*halfThickness; v2.position=cP-dN*halfThickness; 
                v1.color=activeColor; v2.color=activeColor; 
                tailVertices.append(v1); tailVertices.append(v2);
            }
            if(tailVertices.getVertexCount()>0) window.draw(tailVertices);
        }
    }
}

void Bot::renderHead(sf::RenderWindow& window) const { 
    if (!alive) return;
    window.draw(headSprite);
}


