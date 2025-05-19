#include <SFML/Graphics.hpp>
#include <iostream> // Still needed for std::cerr and potentially non-debug std::cout
#include <string>
#include <cmath> 
#include <vector>
#include <algorithm> 
#include <deque> 
#include <cstdlib> 
#include <ctime>   

#include "common.h" 
#include "bot.h"    
#include "radar.h"  
#include "util.h"   

// --- main.cpp Specific Global Constants ---
const sf::Color NIGHT_MODE_BACKGROUND = sf::Color(20, 20, 30);
const sf::Color PLAYER_SAFE_HEAD_FILL_COLOR = sf::Color(100, 100, 100); 
const sf::Color PLAYER_HEAD_OUTLINE_COLOR = sf::Color(150, 150, 150); 
const float PLAYER_HEAD_OUTLINE_THICKNESS = 1.0f; 
const sf::Color PLAYER_HEAD_CORNER_COLOR = sf::Color::Black; 
const sf::Color PLAYER_TAIL_COLOR = PLAYER_ACTIVE_COLOR; 

const sf::Color BOT_SAFE_HEAD_COLORS_MAIN[MAX_BOTS_ALLOWED] = { 
    sf::Color(150,150,150), sf::Color(155,155,155), sf::Color(160,160,160), sf::Color(165,165,165),
    sf::Color(170,170,170), sf::Color(175,175,175), sf::Color(180,180,180), sf::Color(185,185,185),
    sf::Color(190,190,190), sf::Color(195,195,195), sf::Color(140,140,140), sf::Color(145,145,145),
    sf::Color(130,130,130), sf::Color(135,135,135), sf::Color(120,120,120), sf::Color(125,125,125),
    sf::Color(150,150,150), sf::Color(155,155,155), sf::Color(160,160,160), sf::Color(165,165,165),
    sf::Color(170,170,170), sf::Color(175,175,175), sf::Color(180,180,180), sf::Color(185,185,185),
    sf::Color(190,190,190), sf::Color(195,195,195), sf::Color(140,140,140), sf::Color(145,145,145),
    sf::Color(130,130,130), sf::Color(135,135,135), sf::Color(120,120,120), sf::Color(125,125,125),
    sf::Color(150,150,150), sf::Color(155,155,155), sf::Color(160,160,160), sf::Color(165,165,165),
    sf::Color(170,170,170), sf::Color(175,175,175), sf::Color(180,180,180), sf::Color(185,185,185),
    sf::Color(190,190,190), sf::Color(195,195,195), sf::Color(140,140,140), sf::Color(145,145,145),
    sf::Color(130,130,130), sf::Color(135,135,135), sf::Color(120,120,120), sf::Color(125,125,125),
    sf::Color(150,150,150), sf::Color(155,155,155), sf::Color(160,160,160), sf::Color(165,165,165),
    sf::Color(170,170,170), sf::Color(175,175,175), sf::Color(180,180,180), sf::Color(185,185,185),
    sf::Color(190,190,190), sf::Color(195,195,195), sf::Color(140,140,140), sf::Color(145,145,145),
    sf::Color(130,130,130) 
};

const sf::Color TEXT_COLOR = sf::Color::White;
const unsigned int TEXT_SIZE = 30;
const std::string FONT_PATH = "assets/arial.ttf"; 
const std::string SAVE_FILENAME = "tsnakes.sav"; 

sf::Texture createPlayerHeadTexture(unsigned int size, float outlineThicknessFloat,
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
            image.setPixel(0, 0, cornerPixelColor); image.setPixel(1, 0, cornerPixelColor); image.setPixel(0, 1, cornerPixelColor);
            image.setPixel(size - 1, 0, cornerPixelColor); image.setPixel(size - 2, 0, cornerPixelColor); image.setPixel(size - 1, 1, cornerPixelColor);
            image.setPixel(0, size - 1, cornerPixelColor); image.setPixel(1, size - 1, cornerPixelColor); image.setPixel(0, size - 2, cornerPixelColor);
            image.setPixel(size - 1, size - 1, cornerPixelColor); image.setPixel(size - 2, size - 1, cornerPixelColor); image.setPixel(size - 1, size - 2, cornerPixelColor);
        } else if (size > 0) { 
             for(unsigned int y_img=0; y_img<size; ++y_img) for(unsigned int x_img=0; x_img<size; ++x_img) image.setPixel(x_img,y_img, cornerPixelColor);
        }
    }
    sf::Texture texture; if (!texture.loadFromImage(image)) { /*if (G_CONFIG.debugMode)*/ std::cerr << "Error: Could not create player head texture." << std::endl; } // Keep cerr for critical errors
    return texture;
}

sf::Color getTileColor(const Tile& tile) { 
    switch (tile.baseOwner) {
        case TileBaseOwner::PLAYER: 
            if (tile.entityID == PLAYER_ENTITY_ID) return PLAYER_TERRITORY_COLOR; 
            /*if (G_CONFIG.debugMode)*/ std::cerr << "Error: Player tile with wrong entityID: " << tile.entityID << std::endl; // Keep cerr
            return sf::Color::White; 
        case TileBaseOwner::BOT:
            if (tile.entityID >= 0 && tile.entityID < MAX_BOTS_ALLOWED) return BOT_TERRITORY_COLORS[tile.entityID]; 
            /*if (G_CONFIG.debugMode)*/ std::cerr << "Error: Bot entityID " << tile.entityID << " out of range for BOT_TERRITORY_COLORS (max is " << MAX_BOTS_ALLOWED -1 <<")." << std::endl; // Keep cerr
            return sf::Color::White; 
        case TileBaseOwner::NEUTRAL: 
        default:
            return NEUTRAL_TERRITORY_COLOR; 
    }
}

void floodFill(sf::Vector2i seedPos, TileBaseOwner targetOwner, int targetEntityID_unused, 
               TileBaseOwner replacementOwner, int replacementEntityID) { 
    if (seedPos.x < 0 || seedPos.x >= G_CONFIG.mapWidthTiles || seedPos.y < 0 || seedPos.y >= G_CONFIG.mapHeightTiles) { 
        // if (G_CONFIG.debugMode) std::cout << "Flood fill seed out of bounds: (" << seedPos.x << "," << seedPos.y << ")" << std::endl; 
        return; 
    }
    if (gameMap[seedPos.x][seedPos.y].baseOwner != targetOwner) { 
        // if (G_CONFIG.debugMode) std::cout << "Flood fill seed (" << seedPos.x << "," << seedPos.y << ") not on target baseOwner. Seed baseOwner: " 
        //           << static_cast<int>(gameMap[seedPos.x][seedPos.y].baseOwner) 
        //           << " Target baseOwner: " << static_cast<int>(targetOwner) << std::endl; 
        return; 
    }
    std::deque<sf::Vector2i> q; q.push_back(seedPos); 
    gameMap[seedPos.x][seedPos.y].baseOwner = replacementOwner; 
    gameMap[seedPos.x][seedPos.y].entityID = replacementEntityID;
    int filledCount = 1; int ddx[] = {0,0,1,-1}; int ddy[] = {-1,1,0,0}; 
    while(!q.empty()){
        sf::Vector2i current = q.front(); q.pop_front();
        for(int i_ff=0; i_ff<4; ++i_ff){ 
            sf::Vector2i n(current.x+ddx[i_ff], current.y+ddy[i_ff]); 
            if (n.x>=0 && n.x<G_CONFIG.mapWidthTiles && n.y>=0 && n.y<G_CONFIG.mapHeightTiles && 
                gameMap[n.x][n.y].baseOwner == targetOwner) { 
                gameMap[n.x][n.y].baseOwner = replacementOwner; 
                gameMap[n.x][n.y].entityID = replacementEntityID;
                q.push_back(n); filledCount++; 
            } 
        } 
    }
    // if (G_CONFIG.debugMode) std::cout << "Flood fill from ("<<seedPos.x<<","<<seedPos.y<<"). Tiles: "<<filledCount << std::endl;
}

bool isSeedEnclosed(sf::Vector2i seedPos, TileBaseOwner boundaryBaseOwner, int boundaryEntityID, 
                    int maxSearchDepth) { 
    if (seedPos.x < 0 || seedPos.x >= G_CONFIG.mapWidthTiles || seedPos.y < 0 || seedPos.y >= G_CONFIG.mapHeightTiles) { 
        // if (G_CONFIG.debugMode) std::cout << "isSeedEnclosed: Seed (" << seedPos.x << "," << seedPos.y << ") out of bounds." << std::endl; 
        return false; 
    }
    if (gameMap[seedPos.x][seedPos.y].baseOwner != TileBaseOwner::NEUTRAL) {
        // if (G_CONFIG.debugMode) std::cout << "isSeedEnclosed: Seed (" << seedPos.x << "," << seedPos.y << ") not neutral. Owner: " 
        //           << static_cast<int>(gameMap[seedPos.x][seedPos.y].baseOwner) << std::endl; 
        return false; 
    }
    if (maxSearchDepth == 0) maxSearchDepth = G_CONFIG.mapWidthTiles * G_CONFIG.mapHeightTiles / 2; 
    std::deque<sf::Vector2i> q; q.push_back(seedPos);
    std::vector<std::vector<bool>> visited(G_CONFIG.mapWidthTiles, std::vector<bool>(G_CONFIG.mapHeightTiles, false)); 
    visited[seedPos.x][seedPos.y] = true; int count = 0; int ddx[] = {0,0,1,-1}; int ddy[] = {-1,1,0,0}; 
    while(!q.empty()){
        sf::Vector2i c = q.front(); q.pop_front(); count++;
        if (count > maxSearchDepth) { 
            // if (G_CONFIG.debugMode) std::cout << "isSeedEnclosed: Max search depth for seed (" << seedPos.x << "," << seedPos.y << "). Assuming outside." << std::endl; 
            return false; 
        }
        for(int i_ise=0; i_ise<4; ++i_ise){ 
            sf::Vector2i n(c.x + ddx[i_ise], c.y + ddy[i_ise]); 
            if (n.x < 0 || n.x >= G_CONFIG.mapWidthTiles || n.y < 0 || n.y >= G_CONFIG.mapHeightTiles) {
                // if (G_CONFIG.debugMode) std::cout << "isSeedEnclosed: Hit map boundary from seed (" << seedPos.x << "," << seedPos.y << "). Assuming outside." << std::endl; 
                return false; 
            }
            if (!visited[n.x][n.y]) {
                visited[n.x][n.y] = true; 
                if (gameMap[n.x][n.y].baseOwner == TileBaseOwner::NEUTRAL) { q.push_back(n);
                } else if (gameMap[n.x][n.y].baseOwner == boundaryBaseOwner && gameMap[n.x][n.y].entityID == boundaryEntityID) { /* Hit specified boundary */ }
            }
        }
    }
    // if (G_CONFIG.debugMode) std::cout << "isSeedEnclosed: Seed (" << seedPos.x << "," << seedPos.y << ") appears enclosed by boundaryOwner/map edge. Searched " << count << " tiles." << std::endl; 
    return true; 
}

void findAndFillEnclosedAreas(const std::vector<sf::Vector2i>& trail, sf::Vector2i headPosWhenEnteringTerritory, 
                              Direction entryDirection, TileBaseOwner fillingBaseOwner, int fillingEntityID) { 
    if (trail.size() < 3) { 
        // if (G_CONFIG.debugMode) std::cout << "Trail too short for flood fill. Length: " << trail.size() << std::endl; 
        return; 
    }
    sf::Vector2i lastTailPoint = trail.back(); sf::Vector2i seed_offsets[2]; 
    switch (entryDirection) {
        case Direction::UP:    seed_offsets[0] = sf::Vector2i(-1, 0); seed_offsets[1] = sf::Vector2i(1, 0);  break; 
        case Direction::DOWN:  seed_offsets[0] = sf::Vector2i(1, 0);  seed_offsets[1] = sf::Vector2i(-1, 0); break; 
        case Direction::LEFT:  seed_offsets[0] = sf::Vector2i(0, 1);  seed_offsets[1] = sf::Vector2i(0, -1); break; 
        case Direction::RIGHT: seed_offsets[0] = sf::Vector2i(0, -1); seed_offsets[1] = sf::Vector2i(0, 1);  break; 
        default: 
            // if (G_CONFIG.debugMode) std::cout << "Cannot determine seeds: No valid entry direction." << std::endl; 
            return;
    }
    // if (G_CONFIG.debugMode) std::cout << "FFA for EID " << fillingEntityID << ": LastTail:(" << lastTailPoint.x << "," << lastTailPoint.y << "). Entry:(" << headPosWhenEnteringTerritory.x << "," << headPosWhenEnteringTerritory.y << "). Dir:" << static_cast<int>(entryDirection) << std::endl;
    bool filledSomething = false;
    for (int i_ffa=0; i_ffa<2; ++i_ffa){ 
        sf::Vector2i pS = lastTailPoint + seed_offsets[i_ffa]; 
        // if (G_CONFIG.debugMode) std::cout << "FFA: ChkSeed("<<i_ffa<<"):("<<pS.x<<","<<pS.y<<")"<<std::endl;
        if (pS.x>=0 && pS.x<G_CONFIG.mapWidthTiles && pS.y>=0 && pS.y<G_CONFIG.mapHeightTiles && gameMap[pS.x][pS.y].baseOwner==TileBaseOwner::NEUTRAL){ 
            // if (G_CONFIG.debugMode) std::cout<<"FFA: Seed("<<pS.x<<","<<pS.y<<") is NEUTRAL. ChkEnclose..."<<std::endl; 
            if(isSeedEnclosed(pS, fillingBaseOwner, fillingEntityID, G_CONFIG.mapWidthTiles * G_CONFIG.mapHeightTiles / 2)){ 
                // if (G_CONFIG.debugMode) std::cout<<"FFA: Seed("<<pS.x<<","<<pS.y<<") IS ENCLOSED. Flooding for EID "<<fillingEntityID<<"."<<std::endl; 
                floodFill(pS, TileBaseOwner::NEUTRAL, -1, fillingBaseOwner, fillingEntityID);
                filledSomething=true; 
            } // else {if (G_CONFIG.debugMode) std::cout<<"FFA: Seed("<<pS.x<<","<<pS.y<<") NOT enclosed."<<std::endl;} // Commented out "not enclosed" for less spam
        } // else { // Commented out for less spam
             // if(pS.x>=0 && pS.x<G_CONFIG.mapWidthTiles && pS.y>=0 && pS.y<G_CONFIG.mapHeightTiles)  
                // if (G_CONFIG.debugMode) std::cout<<"FFA: Seed("<<pS.x<<","<<pS.y<<") not NEUTRAL. Own:"<<static_cast<int>(gameMap[pS.x][pS.y].baseOwner)<<" ID:"<<gameMap[pS.x][pS.y].entityID<<std::endl;
             // else if (G_CONFIG.debugMode) std::cout<<"FFA: Seed("<<pS.x<<","<<pS.y<<") out of bounds."<<std::endl;
        // }
    }
    // if(!filledSomething && G_CONFIG.debugMode) std::cout<<"FFA: No valid, enclosed neutral seed for EID "<<fillingEntityID<<"."<<std::endl;
    // if (G_CONFIG.debugMode) std::cout<<"End FFA for EID "<<fillingEntityID<<"."<<std::endl;
}


int main(int argc, char* argv[]) {
    // --- Argument Parsing & Config Setup ---
    // G_CONFIG is global, parseArguments will modify it.
    // Initialize G_CONFIG with defaults first (happens at its definition in common.cpp)
    if (!parseArguments(argc, argv, G_CONFIG)) { 
        return 0; // Exit if help was shown or error in parsing
    }

    // Resize gameMap based on G_CONFIG *before* it's used by load or new game setup.
    if (gameMap.size() != static_cast<size_t>(G_CONFIG.mapWidthTiles) || 
        (!gameMap.empty() && gameMap[0].size() != static_cast<size_t>(G_CONFIG.mapHeightTiles))) {
        try {
            gameMap.assign(G_CONFIG.mapWidthTiles, std::vector<Tile>(G_CONFIG.mapHeightTiles));
            if (G_CONFIG.debugMode) {
                std::cout << "Initial game map resized to " << G_CONFIG.mapWidthTiles << "x" << G_CONFIG.mapHeightTiles << " based on config." << std::endl;
            }
        } catch (const std::bad_alloc& e) {
            std::cerr << "Error: Failed to allocate memory for gameMap of size " 
                      << G_CONFIG.mapWidthTiles << "x" << G_CONFIG.mapHeightTiles 
                      << ". " << e.what() << std::endl;
            return EXIT_FAILURE;
        }
    }

    srand(static_cast<unsigned int>(time(0))); 
    initializeBotTerritoryColors(); // Initialize derived bot colors

    // --- Game State Variables ---
    std::vector<Bot> bots;
    sf::Vector2i playerGridPos;
    sf::Vector2f playerVisualPos; 
    Direction currentDirection = Direction::NONE;       
    Direction previousCurrentDirection = Direction::NONE; 
    Direction nextDirection = Direction::NONE;          
    bool isMovingToTarget = false;      
    sf::Vector2f targetVisualPos;       
    std::vector<sf::Vector2i> playerTail; 
    bool isPlayerClaiming = false;        
    GameState currentGameState = GameState::PLAYING; 
    int playerScore = 0; 
    sf::Time timeSurvived = sf::Time::Zero;
    sf::Clock gameRunClock; 
    bool playerWantsToPause = true;     
    bool gameIsEffectivelyPaused = true; 
    bool oldPlayerWantsToPauseState; 
    bool wantsToQuit = false; 
    bool confirmingQuit = false; 
    bool wasPausedBeforeEsc = false;        
    Direction directionBeforeEsc = Direction::NONE; 
    Direction nextDirectionBeforeEsc = Direction::NONE; 
    
    GameSaveData loadedSaveData;
    bool gameLoadedSuccessfully = false;
    bool startNewGame = true; // Assume new game unless load is successful and compatible

    if (loadGameStateFromFile(SAVE_FILENAME, loadedSaveData, G_CONFIG)) { 
        // Check if current G_CONFIG (from args or defaults) forces a new game due to map size mismatch
        if (loadedSaveData.config.mapWidthTiles != G_CONFIG.mapWidthTiles ||
            loadedSaveData.config.mapHeightTiles != G_CONFIG.mapHeightTiles) {
            if (G_CONFIG.debugMode) {
                std::cout << "Save file map dimensions (" << loadedSaveData.config.mapWidthTiles << "x" << loadedSaveData.config.mapHeightTiles
                          << ") mismatch current command-line/default dimensions (" 
                          << G_CONFIG.mapWidthTiles << "x" << G_CONFIG.mapHeightTiles 
                          << "). Starting new game with current config dimensions." << std::endl;
            }
            // gameMap is already sized to G_CONFIG dimensions, just ensure it's neutral
            for(auto& row : gameMap) for(auto& tile : row) {tile.baseOwner = TileBaseOwner::NEUTRAL; tile.entityID = -1;}
            startNewGame = true; 
        } else { // Dimensions match, or no command-line override, so use save file's config
            if (G_CONFIG.debugMode || loadedSaveData.config.debugMode) { // Prioritize loaded debug if it was on
                 std::cout << "Save game found. Applying loaded state." << std::endl;
            }
            // Apply ALL loaded config settings to the global G_CONFIG
            G_CONFIG = loadedSaveData.config; 
            // gameMap was already sized to these dimensions (as they matched), now copy tile data.
            gameMap = loadedSaveData.mapState; 

            playerGridPos = loadedSaveData.playerData.gridPos;
            currentDirection = loadedSaveData.playerData.currentDirection;
            previousCurrentDirection = loadedSaveData.playerData.previousCurrentDirection;
            nextDirection = loadedSaveData.playerData.nextDirection;
            isPlayerClaiming = loadedSaveData.playerData.isClaiming;
            playerTail = loadedSaveData.playerData.tail;
            isMovingToTarget = false; 

            bots.clear(); 
            for (const auto& botData : loadedSaveData.botsData) {
                if (botData.id < 0 || botData.id >= MAX_BOTS_ALLOWED) { 
                    std::cerr << "Warning: Loaded bot with invalid ID " << botData.id << ". Skipping." << std::endl; continue; 
                }
                int colorIndex = botData.id % MAX_BOTS_ALLOWED; 
                bots.emplace_back(
                    botData.id,
                    BOT_PRIMARY_ACTIVE_COLORS[colorIndex], 
                    BOT_SAFE_HEAD_COLORS_MAIN[colorIndex], 
                    botData.gridPos 
                );
                Bot& currentBot = bots.back();
                currentBot.currentDirection = botData.currentDirection; 
                currentBot.isMovingToTarget = botData.isMovingToTarget; 
                currentBot.targetVisualPos = botData.targetVisualPos; 
                currentBot.isClaiming = botData.isClaiming;
                currentBot.alive = botData.alive; 
                currentBot.tail = botData.tail; 
                currentBot.currentAIState = static_cast<Bot::AIState>(botData.currentAIState_int); 
                currentBot.stepsInCurrentDirection = botData.stepsInCurrentDirection; 
                currentBot.loopLegCounter = botData.loopLegCounter;
                currentBot.initialSpawnPoint = botData.initialSpawnPoint; 
                currentBot.loopStartPoint = botData.loopStartPoint;
                // visualPos and headSprite will be set after textures are initialized for all bots
            }
            
            currentGameState = loadedSaveData.currentGameState; 
            playerScore = loadedSaveData.playerScore;
            timeSurvived = sf::seconds(loadedSaveData.timeSurvivedSeconds);
            
            if (currentGameState == GameState::PLAYING) {
                playerWantsToPause = true; // Start paused after loading a playing game
            }
            gameLoadedSuccessfully = true; // Mark that we used the save data
            startNewGame = false;          // Do not initialize a new game
        }
    } else { // No save file or it was invalid/unparsable
        // G_CONFIG already holds defaults or command-line arguments.
        if (G_CONFIG.debugMode) std::cout << "No valid save file. Starting new game with current/default config." << std::endl;
        for(auto& row : gameMap) for(auto& tile : row) { tile.baseOwner = TileBaseOwner::NEUTRAL; tile.entityID = -1; }
        startNewGame = true;
    }
    
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktopMode, "tSnakes", sf::Style::Fullscreen);
    window.setVerticalSyncEnabled(true);

    sf::Font font;
    if (!font.loadFromFile(FONT_PATH)) { std::cerr << "Error: Could not load font from " << FONT_PATH << std::endl; return EXIT_FAILURE; }
    sf::Text quitConfirmText("Quit? (Y/N)", font, TEXT_SIZE); quitConfirmText.setFillColor(TEXT_COLOR);
    sf::FloatRect localTextRect = quitConfirmText.getLocalBounds(); 
    quitConfirmText.setOrigin(localTextRect.left + localTextRect.width / 2.0f, localTextRect.top + localTextRect.height / 2.0f);
    quitConfirmText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);

    sf::Texture playerActiveHeadTexture = createPlayerHeadTexture( HEAD_SIZE_INT, PLAYER_HEAD_OUTLINE_THICKNESS, PLAYER_ACTIVE_COLOR, PLAYER_HEAD_OUTLINE_COLOR, PLAYER_HEAD_CORNER_COLOR);
    sf::Texture playerSafeHeadTexture = createPlayerHeadTexture( HEAD_SIZE_INT, PLAYER_HEAD_OUTLINE_THICKNESS, PLAYER_SAFE_HEAD_FILL_COLOR, PLAYER_HEAD_OUTLINE_COLOR, PLAYER_HEAD_CORNER_COLOR);
    sf::Sprite playerHeadSprite; 
    
    if (startNewGame) { 
        if (G_CONFIG.debugMode) std::cout << "Setting up new game state." << std::endl;
        int playerInitialTerritorySize_local = 5; 
        playerGridPos = sf::Vector2i(G_CONFIG.mapWidthTiles / 3, G_CONFIG.mapHeightTiles / 2); 
        for (int r = -playerInitialTerritorySize_local / 2; r <= playerInitialTerritorySize_local / 2; ++r) {
            for (int c = -playerInitialTerritorySize_local / 2; c <= playerInitialTerritorySize_local / 2; ++c) {
                int tileX = playerGridPos.x + c; int tileY = playerGridPos.y + r;
                if (tileX >= 0 && tileX < G_CONFIG.mapWidthTiles && tileY >= 0 && tileY < G_CONFIG.mapHeightTiles) {
                    gameMap[tileX][tileY].baseOwner = TileBaseOwner::PLAYER;
                    gameMap[tileX][tileY].entityID = PLAYER_ENTITY_ID; 
                }
            }
        }
        currentDirection = Direction::NONE; previousCurrentDirection = Direction::NONE; nextDirection = Direction::NONE;
        isMovingToTarget = false; playerTail.clear(); isPlayerClaiming = false;
        playerScore = 0; timeSurvived = sf::Time::Zero; 
        playerWantsToPause = true; // Start new game paused
        currentGameState = GameState::PLAYING; // Ensure new game starts in PLAYING state
    }
    playerVisualPos = gridToPixelForHead(playerGridPos); 
    playerHeadSprite.setPosition(playerVisualPos);

    if (startNewGame || bots.empty()) { // Initialize Bots if new game or if loaded game had no bots (e.g. corrupted save)
        bots.clear(); 
        const int numberOfBotsToCreate = G_CONFIG.numBots; 
        if (G_CONFIG.debugMode) std::cout << "Initializing " << numberOfBotsToCreate << " new bots." << std::endl;
        for (int i = 0; i < numberOfBotsToCreate && i < MAX_BOTS_ALLOWED; ++i) { 
            sf::Vector2i botStartPos_local; 
            bool spotFound = false; int attempts = 0; const int maxSpawnAttempts = 150; 
            const int botTerritorySizeForSpawnCheck = 5; 
            while (!spotFound && attempts < maxSpawnAttempts) {
                attempts++;
                botStartPos_local = sf::Vector2i(
                    (rand() % (G_CONFIG.mapWidthTiles - (botTerritorySizeForSpawnCheck + 4))) + (botTerritorySizeForSpawnCheck / 2) + 2, 
                    (rand() % (G_CONFIG.mapHeightTiles - (botTerritorySizeForSpawnCheck + 4))) + (botTerritorySizeForSpawnCheck / 2) + 2  
                );
                bool areaIsClear = true;
                for (int r_check = -botTerritorySizeForSpawnCheck/2; r_check <= botTerritorySizeForSpawnCheck/2; ++r_check) { 
                    for (int c_check = -botTerritorySizeForSpawnCheck/2; c_check <= botTerritorySizeForSpawnCheck/2; ++c_check) {
                        int checkX = botStartPos_local.x + c_check; int checkY = botStartPos_local.y + r_check;
                        if (checkX<0||checkX>=G_CONFIG.mapWidthTiles||checkY<0||checkY>=G_CONFIG.mapHeightTiles|| gameMap[checkX][checkY].baseOwner != TileBaseOwner::NEUTRAL) 
                            { areaIsClear = false; break; }
                    } if (!areaIsClear) break;
                } if (areaIsClear) spotFound = true;
            }
            if (!spotFound) {
                std::cerr << "Warning: Could not find clear 5x5 spot for bot " << i << ". Placing randomly." << std::endl;
                 botStartPos_local = sf::Vector2i( (rand()%(G_CONFIG.mapWidthTiles-6))+3, (rand()%(G_CONFIG.mapHeightTiles-6))+3 );
            }
            bots.emplace_back(i, BOT_PRIMARY_ACTIVE_COLORS[i % MAX_BOTS_ALLOWED], 
                                 BOT_SAFE_HEAD_COLORS_MAIN[i % MAX_BOTS_ALLOWED], 
                                 botStartPos_local); 
        }
        int botInitialTerritorySize_local = 5; 
        for (Bot& bot : bots) { 
            for (int r = -botInitialTerritorySize_local / 2; r <= botInitialTerritorySize_local / 2; ++r) {
                for (int c = -botInitialTerritorySize_local / 2; c <= botInitialTerritorySize_local / 2; ++c) {
                    int tileX = bot.gridPos.x + c; int tileY = bot.gridPos.y + r;
                    if (tileX >= 0 && tileX < G_CONFIG.mapWidthTiles && tileY >= 0 && tileY < G_CONFIG.mapHeightTiles) {
                        if (gameMap[tileX][tileY].baseOwner == TileBaseOwner::NEUTRAL) { 
                            gameMap[tileX][tileY].baseOwner = TileBaseOwner::BOT;
                            gameMap[tileX][tileY].entityID = bot.id; 
                        }
                    }
                }
            }
        }
    } 
    // Ensure all bots (new or loaded) have their textures and visual positions set
    for (Bot& bot : bots) { 
        bot.initializeTextures(); // Safe to call even if already initialized
        bot.visualPos = gridToPixelForHead(bot.gridPos); 
        bot.headSprite.setPosition(bot.visualPos);
    }
    
    Radar gameRadar(window.getSize(), G_CONFIG.mapWidthTiles, G_CONFIG.mapHeightTiles); 

    sf::View gameView(sf::FloatRect(0, 0, static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));
    gameView.setCenter(playerVisualPos.x + HEAD_SIZE_FLOAT / 2.0f, 
                       playerVisualPos.y + HEAD_SIZE_FLOAT / 2.0f);

    sf::Clock deltaClock;
    // oldPlayerWantsToPauseState is declared above

    // Restart gameRunClock for the current session
    gameRunClock.restart(); 
    // timeSurvived holds the value from save file if loaded, or Zero if new game.
    // When saving, current session time (gameRunClock.getElapsedTime()) will be added to timeSurvived.
    if (G_CONFIG.debugMode) {
        if (gameLoadedSuccessfully) {
            std::cout << "Game loaded. Initial timeSurvived: " << timeSurvived.asSeconds() << "s. Current session clock restarted." << std::endl;
        } else {
            std::cout << "New game started. timeSurvived: " << timeSurvived.asSeconds() << "s. Current session clock restarted." << std::endl;
        }
    }


    while (window.isOpen()) {
        sf::Time dt = deltaClock.restart();
        float deltaTime = dt.asSeconds();
        
        if (currentGameState == GameState::PLAYING) { 
            oldPlayerWantsToPauseState = playerWantsToPause; 
            gameIsEffectivelyPaused = (playerWantsToPause && !isMovingToTarget) || confirmingQuit;
        } else {
            gameIsEffectivelyPaused = true; 
        }

        bool isPlayerOnOwnTerritory = false; 
        if (playerGridPos.x >= 0 && playerGridPos.x < G_CONFIG.mapWidthTiles &&
            playerGridPos.y >= 0 && playerGridPos.y < G_CONFIG.mapHeightTiles) { 
            isPlayerOnOwnTerritory = (gameMap[playerGridPos.x][playerGridPos.y].baseOwner == TileBaseOwner::PLAYER &&
                                      gameMap[playerGridPos.x][playerGridPos.y].entityID == PLAYER_ENTITY_ID );
        }
        
        if(currentGameState == GameState::PLAYING) { 
             playerHeadSprite.setTexture(isPlayerOnOwnTerritory ? playerSafeHeadTexture : playerActiveHeadTexture);
        }

        sf::Event gameEvent; 
        while (window.pollEvent(gameEvent)) {
            if (gameEvent.type == sf::Event::Closed) {
                if (currentGameState == GameState::PLAYING) { 
                    GameSaveData currentSaveData; currentSaveData.config = G_CONFIG; 
                    currentSaveData.mapState = gameMap;
                    currentSaveData.playerData = {playerGridPos, currentDirection, previousCurrentDirection, nextDirection, isPlayerClaiming, playerTail};
                    currentSaveData.botsData.clear(); 
                    for(const auto& b : bots) { BotSaveData bsd; bsd.id=b.id; bsd.gridPos=b.gridPos; bsd.currentDirection=b.currentDirection; bsd.isMovingToTarget=b.isMovingToTarget; bsd.targetVisualPos=b.targetVisualPos; bsd.isClaiming=b.isClaiming; bsd.alive=b.alive; bsd.currentAIState_int=static_cast<int>(b.currentAIState); bsd.stepsInCurrentDirection=b.stepsInCurrentDirection; bsd.loopLegCounter=b.loopLegCounter; bsd.initialSpawnPoint=b.initialSpawnPoint; bsd.loopStartPoint=b.loopStartPoint; bsd.tail=b.tail; currentSaveData.botsData.push_back(bsd); }
                    currentSaveData.currentGameState = currentGameState; currentSaveData.playerScore = playerScore;
                    currentSaveData.timeSurvivedSeconds = timeSurvived.asSeconds() + gameRunClock.getElapsedTime().asSeconds(); // Add current session time
                    saveGameStateToFile(SAVE_FILENAME, currentSaveData);
                } 
                window.close();
            }
            if (currentGameState == GameState::PLAYING) {
                if (gameEvent.type == sf::Event::KeyPressed) {
                    if (confirmingQuit) { 
                        if (gameEvent.key.code == sf::Keyboard::Y) wantsToQuit = true; 
                        else { confirmingQuit = false; playerWantsToPause = wasPausedBeforeEsc; currentDirection = directionBeforeEsc; nextDirection = nextDirectionBeforeEsc;}
                    } else { 
                        if (gameEvent.key.code == sf::Keyboard::Escape) { confirmingQuit = true; wasPausedBeforeEsc = playerWantsToPause; directionBeforeEsc = currentDirection; nextDirectionBeforeEsc = nextDirection;}
                        else if (gameEvent.key.code == sf::Keyboard::P) { playerWantsToPause = !playerWantsToPause; if (!playerWantsToPause && currentDirection == Direction::NONE && nextDirection == Direction::NONE) playerWantsToPause = true; if (playerWantsToPause && currentDirection != Direction::NONE) previousCurrentDirection = currentDirection;}
                        else if (gameEvent.key.code == sf::Keyboard::Left)  { nextDirection = Direction::LEFT;  if (playerWantsToPause && !isMovingToTarget) playerWantsToPause = false; }
                        else if (gameEvent.key.code == sf::Keyboard::Right) { nextDirection = Direction::RIGHT; if (playerWantsToPause && !isMovingToTarget) playerWantsToPause = false; }
                        else if (gameEvent.key.code == sf::Keyboard::Up)    { nextDirection = Direction::UP;    if (playerWantsToPause && !isMovingToTarget) playerWantsToPause = false; }
                        else if (gameEvent.key.code == sf::Keyboard::Down)  { nextDirection = Direction::DOWN;  if (playerWantsToPause && !isMovingToTarget) playerWantsToPause = false; }
                    }
                }
            } else if (currentGameState == GameState::GAME_OVER) {
                if (gameEvent.type == sf::Event::KeyPressed || gameEvent.type == sf::Event::MouseButtonPressed) {
                    window.close(); 
                }
            }
        }
        if (wantsToQuit && currentGameState == GameState::PLAYING) { 
            GameSaveData currentSaveData; currentSaveData.config = G_CONFIG;
            currentSaveData.mapState = gameMap;
            currentSaveData.playerData = {playerGridPos, currentDirection, previousCurrentDirection, nextDirection, isPlayerClaiming, playerTail};
            currentSaveData.botsData.clear(); 
            for(const auto& b : bots) { BotSaveData bsd; bsd.id=b.id; bsd.gridPos=b.gridPos; bsd.currentDirection=b.currentDirection; bsd.isMovingToTarget=b.isMovingToTarget; bsd.targetVisualPos=b.targetVisualPos; bsd.isClaiming=b.isClaiming; bsd.alive=b.alive; bsd.currentAIState_int=static_cast<int>(b.currentAIState); bsd.stepsInCurrentDirection=b.stepsInCurrentDirection; bsd.loopLegCounter=b.loopLegCounter; bsd.initialSpawnPoint=b.initialSpawnPoint; bsd.loopStartPoint=b.loopStartPoint; bsd.tail=b.tail; currentSaveData.botsData.push_back(bsd); }
            currentSaveData.currentGameState = currentGameState; 
            currentSaveData.playerScore = playerScore;
            currentSaveData.timeSurvivedSeconds = timeSurvived.asSeconds() + gameRunClock.getElapsedTime().asSeconds(); // Add current session time
            saveGameStateToFile(SAVE_FILENAME, currentSaveData);
            window.close();
        }
        if (currentGameState == GameState::GAME_OVER && confirmingQuit) { 
            confirmingQuit = false; 
        }


        // --- Game Logic Update ---
        if (currentGameState == GameState::PLAYING) { 
            // --- Bot Updates & Respawn / Claiming Logic ---
            for (Bot& bot : bots) { 
                if (bot.alive) { 
                    bot.update(deltaTime, gameMap, playerTail, isPlayerClaiming, playerGridPos, bots); 

                    if (!bot.alive) { // Bot died during its own update (e.g. self-collision, or AI suicide if stuck)
                        if (G_CONFIG.debugMode) std::cout << "Main: Bot " << bot.id << " died. Initiating instant respawn." << std::endl;
                        sf::Vector2i newBotSpawnPos;
                        bool spotFoundForRespawn = false;
                        int respawnAttempts = 0;
                        const int maxRespawnAttempts = 150; 
                        const int botTerritorySizeForRespawn = 5; 

                        while(!spotFoundForRespawn && respawnAttempts < maxRespawnAttempts) {
                            respawnAttempts++;
                            newBotSpawnPos = sf::Vector2i(
                                (rand() % (G_CONFIG.mapWidthTiles - (botTerritorySizeForRespawn + 4))) + (botTerritorySizeForRespawn / 2) + 2, 
                                (rand() % (G_CONFIG.mapHeightTiles - (botTerritorySizeForRespawn + 4))) + (botTerritorySizeForRespawn / 2) + 2  
                            );
                            bool areaClearForRespawn = true;
                            for (int r_check = -botTerritorySizeForRespawn/2; r_check <= botTerritorySizeForRespawn/2; ++r_check) { 
                                for (int c_check = -botTerritorySizeForRespawn/2; c_check <= botTerritorySizeForRespawn/2; ++c_check) {
                                    int checkX = newBotSpawnPos.x + c_check; int checkY = newBotSpawnPos.y + r_check;
                                    if (checkX<0||checkX>=G_CONFIG.mapWidthTiles||checkY<0||checkY>=G_CONFIG.mapHeightTiles|| gameMap[checkX][checkY].baseOwner != TileBaseOwner::NEUTRAL) 
                                        { areaClearForRespawn = false; break; }
                                }
                                if (!areaClearForRespawn) break;
                            }
                            if (areaClearForRespawn) spotFoundForRespawn = true;
                        }
                        if (spotFoundForRespawn) {
                            bot.respawn(gameMap, newBotSpawnPos); 
                        } else { 
                            if (G_CONFIG.debugMode) std::cout << "Main: Could not find clear spot for bot " << bot.id << " respawn. Bot remains dead for now." << std::endl; 
                        }
                    } else { // Bot is still alive after its update, process its potential claim
                        if (!bot.isClaiming && !bot.tail.empty()) { // Bot just completed a trail
                            if (G_CONFIG.debugMode) std::cout << "Main: Processing Bot " << bot.id << " territory claim. Tail size: " << bot.tail.size() << std::endl;
                            int botTrailLength = bot.tail.size();
                            
                            for (const auto& botTailPos : bot.tail) { // Convert tail to trail
                                if (botTailPos.x >= 0 && botTailPos.x < G_CONFIG.mapWidthTiles && botTailPos.y >= 0 && botTailPos.y < G_CONFIG.mapHeightTiles) {
                                    gameMap[botTailPos.x][botTailPos.y].baseOwner = TileBaseOwner::BOT;
                                    gameMap[botTailPos.x][botTailPos.y].entityID = bot.id;
                                }
                            }
                            
                            if (botTrailLength > 2 && botTrailLength < 200) { // Bot Flood Fill
                                if (G_CONFIG.debugMode) std::cout << "Bot " << bot.id << " attempting flood fill..." << std::endl;
                                findAndFillEnclosedAreas(bot.tail, bot.gridPos, bot.currentDirection, TileBaseOwner::BOT, bot.id); 
                            } else if (botTrailLength >= 200) {
                                if (G_CONFIG.debugMode) std::cout << "Bot " << bot.id << " trail too long (" << botTrailLength << "), no flood fill." << std::endl;
                            } else if (botTrailLength > 0) { 
                                 if (G_CONFIG.debugMode) std::cout << "Bot " << bot.id << " trail too short (" << botTrailLength << ") for fill." << std::endl;
                            }
                            bot.tail.clear(); 
                        }
                    }
                } 
            }
            // --- End Bot Update & Respawn / Claiming Logic --- 

            // --- Player Logic ---
            if (!confirmingQuit && (isMovingToTarget || !playerWantsToPause) ) {
                if (!isMovingToTarget) { 
                    if (playerGridPos.x >= 0 && playerGridPos.x < G_CONFIG.mapWidthTiles && 
                        playerGridPos.y >= 0 && playerGridPos.y < G_CONFIG.mapHeightTiles) { 
                        
                        bool nowOnOwnTerritory = (gameMap[playerGridPos.x][playerGridPos.y].baseOwner == TileBaseOwner::PLAYER &&
                                                  gameMap[playerGridPos.x][playerGridPos.y].entityID == PLAYER_ENTITY_ID);

                        if (isPlayerClaiming) { 
                            if (nowOnOwnTerritory) { 
                                isPlayerClaiming = false; int trailLength = playerTail.size(); 
                                if (G_CONFIG.debugMode) std::cout << "Player re-entered. Trail: " << trailLength << std::endl;
                                for (const auto& tP : playerTail) if (tP.x>=0&&tP.x<G_CONFIG.mapWidthTiles&&tP.y>=0&&tP.y<G_CONFIG.mapHeightTiles) {
                                    gameMap[tP.x][tP.y].baseOwner = TileBaseOwner::PLAYER; gameMap[tP.x][tP.y].entityID = PLAYER_ENTITY_ID;
                                }
                                if (trailLength>2 && trailLength<200) { if (G_CONFIG.debugMode) std::cout << "Player Flood Fill..." << std::endl; findAndFillEnclosedAreas(playerTail, playerGridPos, currentDirection, TileBaseOwner::PLAYER, PLAYER_ENTITY_ID); }
                                else if (trailLength>=200) { if (G_CONFIG.debugMode) std::cout << "Player trail too long." << std::endl; }
                                else if (trailLength>0) { if (G_CONFIG.debugMode) std::cout << "Player trail too short." << std::endl; }
                                playerTail.clear(); 
                            } else { 
                                bool pSelfCol=false; if(playerTail.size()>2) for(size_t i=0;i<playerTail.size()-1;++i) if(playerTail[i]==playerGridPos){pSelfCol=true;break;}
                                if(pSelfCol){ 
                                    if (G_CONFIG.debugMode) std::cout<<"PLAYER SELF COLLISION! GAME OVER."<<std::endl; 
                                    currentGameState=GameState::GAME_OVER; 
                                    timeSurvived = gameRunClock.getElapsedTime() + (gameLoadedSuccessfully ? sf::seconds(loadedSaveData.timeSurvivedSeconds) : sf::Time::Zero);
                                    playerScore = 0; for(int y_s=0;y_s<G_CONFIG.mapHeightTiles;++y_s)for(int x_s=0;x_s<G_CONFIG.mapWidthTiles;++x_s)if(gameMap[x_s][y_s].baseOwner==TileBaseOwner::PLAYER && gameMap[x_s][y_s].entityID == PLAYER_ENTITY_ID)playerScore++;
                                    GameSaveData currentSaveData; currentSaveData.config = G_CONFIG; currentSaveData.mapState = gameMap;
                                    currentSaveData.playerData = {playerGridPos, currentDirection, previousCurrentDirection, nextDirection, isPlayerClaiming, playerTail};
                                    currentSaveData.botsData.clear(); 
                                    for(const auto& b : bots) { BotSaveData bsd; bsd.id=b.id; bsd.gridPos=b.gridPos; bsd.currentDirection=b.currentDirection; bsd.isMovingToTarget=b.isMovingToTarget; bsd.targetVisualPos=b.targetVisualPos; bsd.isClaiming=b.isClaiming; bsd.alive=b.alive; bsd.currentAIState_int=static_cast<int>(b.currentAIState); bsd.stepsInCurrentDirection=b.stepsInCurrentDirection; bsd.loopLegCounter=b.loopLegCounter; bsd.initialSpawnPoint=b.initialSpawnPoint; bsd.loopStartPoint=b.loopStartPoint; bsd.tail=b.tail; currentSaveData.botsData.push_back(bsd); }
                                    currentSaveData.currentGameState = currentGameState; currentSaveData.playerScore = playerScore; currentSaveData.timeSurvivedSeconds = timeSurvived.asSeconds();
                                    saveGameStateToFile(SAVE_FILENAME, currentSaveData);
                                }
                                else if(playerTail.empty()||playerTail.back()!=playerGridPos)playerTail.push_back(playerGridPos);
                            }
                        } else { 
                            if (!nowOnOwnTerritory) { isPlayerClaiming=true; playerTail.clear(); playerTail.push_back(playerGridPos); if (G_CONFIG.debugMode) std::cout<<"Player left, starting tail."<<std::endl;}
                        }
                        
                        if (currentGameState == GameState::PLAYING) { 
                            for (Bot& bot : bots) {
                                if (bot.alive && bot.isClaiming) {
                                    bool pHitBTail=false; for(const auto&bTP:bot.tail)if(playerGridPos==bTP){pHitBTail=true;break;}
                                    if (pHitBTail) { 
                                        if (G_CONFIG.debugMode) std::cout<<"PLAYER HIT BOT "<<bot.id<<" TAIL! Bot dies."<<std::endl; 
                                        bot.die(gameMap); 
                                        sf::Vector2i newBotSpawnPos; bool spotFound=false; int attempts=0; const int maxAtt=100; const int bTSR=5; 
                                        while(!spotFound && attempts < maxAtt){ attempts++;
                                            newBotSpawnPos = sf::Vector2i((rand()%(G_CONFIG.mapWidthTiles-(bTSR+4)))+(bTSR/2)+2, (rand()%(G_CONFIG.mapHeightTiles-(bTSR+4)))+(bTSR/2)+2);
                                            bool areaClear=true; for(int rC=-bTSR/2;rC<=bTSR/2;++rC){for(int cC=-bTSR/2;cC<=bTSR/2;++cC){
                                            int cX=newBotSpawnPos.x+cC;int cY=newBotSpawnPos.y+rC; if(cX<0||cX>=G_CONFIG.mapWidthTiles||cY<0||cY>=G_CONFIG.mapHeightTiles||gameMap[cX][cY].baseOwner!=TileBaseOwner::NEUTRAL){areaClear=false;break;}}if(!areaClear)break;}
                                            if(areaClear)spotFound=true;
                                        }
                                        if(spotFound) bot.respawn(gameMap, newBotSpawnPos);
                                        else {if (G_CONFIG.debugMode) std::cout<<"Main: No spot for bot "<<bot.id<<" respawn after player kill. Bot remains dead."<<std::endl; }
                                    }
                                }
                            }
                        }
                    } 
                    
                    if (currentGameState == GameState::PLAYING) { 
                        if (playerWantsToPause) { if(currentDirection==Direction::NONE && previousCurrentDirection !=Direction::NONE) currentDirection=previousCurrentDirection; } 
                        else { 
                            if (nextDirection != Direction::NONE) {
                                bool canApply=false; if(isPlayerOnOwnTerritory)canApply=true; else if(currentDirection==Direction::NONE||!isOppositeDirection(currentDirection,nextDirection))canApply=true;
                                if(canApply)currentDirection=nextDirection;
                            }
                            if (currentDirection != Direction::NONE) {
                                sf::Vector2i tGP=playerGridPos; bool cM=true; 
                                if(currentDirection==Direction::LEFT)tGP.x--; else if(currentDirection==Direction::RIGHT)tGP.x++; else if(currentDirection==Direction::UP)tGP.y--; else if(currentDirection==Direction::DOWN)tGP.y++; else cM=false; 
                                if(tGP.x<0||tGP.x>=G_CONFIG.mapWidthTiles||tGP.y<0||tGP.y>=G_CONFIG.mapHeightTiles){cM=false;currentDirection=Direction::NONE;playerWantsToPause=true; if (G_CONFIG.debugMode) std::cout<<"Player hit boundary."<<std::endl;}
                                if(cM){targetVisualPos=gridToPixelForHead(tGP);isMovingToTarget=true;nextDirection=Direction::NONE;previousCurrentDirection=currentDirection;}
                            }
                        }
                    }
                } 
                if (isMovingToTarget && currentGameState == GameState::PLAYING) { 
                    float moveSpeedPixels = G_CONFIG.playerSpeed * TILE_SIZE; // Use G_CONFIG
                    sf::Vector2f dV=targetVisualPos-playerVisualPos; float dT=std::sqrt(dV.x*dV.x+dV.y*dV.y); float sT=moveSpeedPixels*deltaTime*0.5f; 
                    if(dT<sT||dT<0.1f){playerVisualPos=targetVisualPos;playerGridPos=visualHeadPosToGrid(playerVisualPos);isMovingToTarget=false;}
                    else{sf::Vector2f nD=dV/dT;playerVisualPos+=nD*moveSpeedPixels*deltaTime;}
                    playerHeadSprite.setPosition(playerVisualPos); 
                }
            } 


            // --- Bot hits Player's Tail (Player dies) ---
            if (currentGameState == GameState::PLAYING && isPlayerClaiming && !playerTail.empty()) {
                for (const Bot& bot : bots) { 
                    if (bot.alive) { 
                        for (const auto& playerTailPos : playerTail) {
                            if (bot.gridPos == playerTailPos) {
                                if (G_CONFIG.debugMode) std::cout << "BOT " << bot.id << " HIT PLAYER'S TAIL! GAME OVER." << std::endl;
                                currentGameState = GameState::GAME_OVER;
                                timeSurvived = gameRunClock.getElapsedTime() + (gameLoadedSuccessfully ? sf::seconds(loadedSaveData.timeSurvivedSeconds) : sf::Time::Zero);
                                playerScore = 0; 
                                for (int y_s=0;y_s<G_CONFIG.mapHeightTiles;++y_s) {
                                    for(int x_s=0;x_s<G_CONFIG.mapWidthTiles;++x_s) {
                                        if(gameMap[x_s][y_s].baseOwner==TileBaseOwner::PLAYER && gameMap[x_s][y_s].entityID == PLAYER_ENTITY_ID) playerScore++;
                                    }
                                }
                                GameSaveData currentSaveData; currentSaveData.config = G_CONFIG; currentSaveData.mapState = gameMap;
                                currentSaveData.playerData = {playerGridPos, currentDirection, previousCurrentDirection, nextDirection, isPlayerClaiming, playerTail};
                                currentSaveData.botsData.clear(); 
                                for(const auto& b : bots) { 
                                    BotSaveData bsd; bsd.id = b.id; bsd.gridPos = b.gridPos; bsd.currentDirection = b.currentDirection;
                                    bsd.isMovingToTarget = b.isMovingToTarget; bsd.targetVisualPos = b.targetVisualPos; 
                                    bsd.isClaiming = b.isClaiming; bsd.alive = b.alive; 
                                    bsd.currentAIState_int = static_cast<int>(b.currentAIState);
                                    bsd.stepsInCurrentDirection = b.stepsInCurrentDirection; bsd.loopLegCounter = b.loopLegCounter;
                                    bsd.initialSpawnPoint = b.initialSpawnPoint; bsd.loopStartPoint = b.loopStartPoint; bsd.tail = b.tail;
                                    currentSaveData.botsData.push_back(bsd);
                                }
                                currentSaveData.currentGameState = currentGameState; 
                                currentSaveData.playerScore = playerScore;
                                currentSaveData.timeSurvivedSeconds = timeSurvived.asSeconds();
                                saveGameStateToFile(SAVE_FILENAME, currentSaveData);
                                
                                goto end_game_logic_for_frame; 
                            }
                        }
                    }
                }
            }
            end_game_logic_for_frame:; 
            // --- End Bot hits Player's Tail ---

            // --- Update Radar State ---
            sf::Color currentPlayerHeadColorOnRadar = isPlayerOnOwnTerritory ? PLAYER_SAFE_HEAD_FILL_COLOR : PLAYER_ACTIVE_COLOR;
            gameRadar.updateAndPrepareState(gameMap, playerGridPos, currentPlayerHeadColorOnRadar, bots);
            // --- End Update Radar State ---

        } // End if (currentGameState == GameState::PLAYING) for all game logic

        if(currentGameState == GameState::PLAYING || currentGameState == GameState::GAME_OVER) { 
            gameView.setCenter(playerVisualPos.x + HEAD_SIZE_FLOAT / 2.0f, 
                               playerVisualPos.y + HEAD_SIZE_FLOAT / 2.0f);
        }
        
        // --- Rendering ---
        window.clear(NIGHT_MODE_BACKGROUND); 
        
        if (currentGameState == GameState::PLAYING || currentGameState == GameState::GAME_OVER) {
            window.setView(gameView); 

            sf::Vector2f viewCenter = gameView.getCenter();
            sf::Vector2f viewSize = gameView.getSize();
            int startX = std::max(0, static_cast<int>(std::floor((viewCenter.x - viewSize.x / 2.0f) / TILE_SIZE)));
            int endX = std::min(G_CONFIG.mapWidthTiles, static_cast<int>(std::ceil((viewCenter.x + viewSize.x / 2.0f) / TILE_SIZE)));
            int startY = std::max(0, static_cast<int>(std::floor((viewCenter.y - viewSize.y / 2.0f) / TILE_SIZE)));
            int endY = std::min(G_CONFIG.mapHeightTiles, static_cast<int>(std::ceil((viewCenter.y + viewSize.y / 2.0f) / TILE_SIZE)));

            sf::RectangleShape tileShape(sf::Vector2f(TILE_SIZE, TILE_SIZE)); 
            for (int y_draw = startY; y_draw < endY; ++y_draw) { 
                for (int x_draw = startX; x_draw < endX; ++x_draw) { 
                    tileShape.setFillColor(getTileColor(gameMap[x_draw][y_draw])); 
                    tileShape.setPosition(x_draw * TILE_SIZE, y_draw * TILE_SIZE);
                    window.draw(tileShape);
                }
            }
            
            sf::RectangleShape gridLineShape;
            gridLineShape.setFillColor(GRID_COLOR); 
            for (int x_grid = startX; x_grid <= endX; ++x_grid) { 
                gridLineShape.setSize(sf::Vector2f(GRID_LINE_THICKNESS, (endY - startY) * TILE_SIZE));
                gridLineShape.setPosition(x_grid * TILE_SIZE - (GRID_LINE_THICKNESS / 2.0f), startY * TILE_SIZE); 
                window.draw(gridLineShape);
            }
            for (int y_grid = startY; y_grid <= endY; ++y_grid) { 
                gridLineShape.setSize(sf::Vector2f((endX - startX) * TILE_SIZE, GRID_LINE_THICKNESS));
                gridLineShape.setPosition(startX * TILE_SIZE, y_grid * TILE_SIZE - (GRID_LINE_THICKNESS / 2.0f));
                window.draw(gridLineShape);
            }

            if (isPlayerClaiming && playerTail.size() >= 1 && currentGameState == GameState::PLAYING) {
                sf::VertexArray tailVertices(sf::TriangleStrip);
                float halfThickness = SNAKE_TAIL_THICKNESS / 2.0f; 
                if (playerTail.size() == 1) {
                    sf::RectangleShape capShape(sf::Vector2f(SNAKE_TAIL_THICKNESS, SNAKE_TAIL_THICKNESS));
                    capShape.setFillColor(PLAYER_TAIL_COLOR); 
                    capShape.setOrigin(halfThickness, halfThickness);
                    capShape.setPosition(playerTail[0].x*TILE_SIZE + TILE_SIZE/2.0f, playerTail[0].y*TILE_SIZE + TILE_SIZE/2.0f);
                    window.draw(capShape);
                } else { 
                    for (size_t i = 0; i < playerTail.size(); ++i) {
                        sf::Vector2f cP(playerTail[i].x*TILE_SIZE+TILE_SIZE/2.0f, playerTail[i].y*TILE_SIZE+TILE_SIZE/2.0f); 
                        sf::Vector2f dN; 
                        if(i==0){sf::Vector2f nP(playerTail[i+1].x*TILE_SIZE+TILE_SIZE/2.0f,playerTail[i+1].y*TILE_SIZE+TILE_SIZE/2.0f); sf::Vector2f sD=nP-cP; float l=std::sqrt(sD.x*sD.x+sD.y*sD.y); if(l>0)sD/=l; else sD=sf::Vector2f(1,0); dN=sf::Vector2f(-sD.y,sD.x);}
                        else if(i==playerTail.size()-1){sf::Vector2f pP(playerTail[i-1].x*TILE_SIZE+TILE_SIZE/2.0f,playerTail[i-1].y*TILE_SIZE+TILE_SIZE/2.0f); sf::Vector2f sD=cP-pP; float l=std::sqrt(sD.x*sD.x+sD.y*sD.y); if(l>0)sD/=l; else sD=sf::Vector2f(1,0); dN=sf::Vector2f(-sD.y,sD.x);}
                        else{sf::Vector2f nP(playerTail[i+1].x*TILE_SIZE+TILE_SIZE/2.0f,playerTail[i+1].y*TILE_SIZE+TILE_SIZE/2.0f); sf::Vector2f sD=nP-cP; float l=std::sqrt(sD.x*sD.x+sD.y*sD.y); if(l>0)sD/=l; else sD=sf::Vector2f(1,0); dN=sf::Vector2f(-sD.y,sD.x);}
                        sf::Vertex v1,v2; v1.position=cP+dN*halfThickness; v2.position=cP-dN*halfThickness; v1.color=PLAYER_TAIL_COLOR; v2.color=PLAYER_TAIL_COLOR;
                        tailVertices.append(v1); tailVertices.append(v2); 
                    } 
                    window.draw(tailVertices); 
                }
            }
            for (const Bot& bot : bots) { 
                if (bot.alive && bot.isClaiming && currentGameState == GameState::PLAYING) { 
                    bot.renderTail(window, startX, endX, startY, endY);
                }
            }
            
            window.draw(playerHeadSprite); 
            for (const Bot& bot : bots) { 
                if (bot.alive) { 
                    bot.renderHead(window);
                }
            }
        } 

        window.setView(window.getDefaultView());
        if (currentGameState == GameState::PLAYING || currentGameState == GameState::GAME_OVER) { 
            gameRadar.render(window);
        }

        if (currentGameState == GameState::PLAYING && confirmingQuit) { 
            window.draw(quitConfirmText); 
        } else if (currentGameState == GameState::GAME_OVER) {
            sf::Text gameOverText("GAME OVER", font, 80); 
            gameOverText.setFillColor(sf::Color::Red); sf::FloatRect goRect=gameOverText.getLocalBounds(); 
            gameOverText.setOrigin(goRect.left+goRect.width/2.0f,goRect.top+goRect.height/2.0f); 
            gameOverText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 3.0f); 
            window.draw(gameOverText);
            std::string scoreStr="Score: "+std::to_string(playerScore); sf::Text scoreDisplay(scoreStr,font,40); 
            scoreDisplay.setFillColor(TEXT_COLOR); sf::FloatRect scoreRect=scoreDisplay.getLocalBounds(); 
            scoreDisplay.setOrigin(scoreRect.left+scoreRect.width/2.0f,scoreRect.top+scoreRect.height/2.0f); 
            scoreDisplay.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f); 
            window.draw(scoreDisplay);
            std::string timeStr="Time: "+std::to_string(static_cast<int>(timeSurvived.asSeconds()))+"s"; sf::Text timeDisplay(timeStr,font,40); 
            timeDisplay.setFillColor(TEXT_COLOR); sf::FloatRect timeRect=timeDisplay.getLocalBounds(); 
            timeDisplay.setOrigin(timeRect.left+timeRect.width/2.0f,timeRect.top+timeRect.height/2.0f); 
            timeDisplay.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f+60.f); 
            window.draw(timeDisplay);
            sf::Text exitMsg("Press any key to exit",font,30); exitMsg.setFillColor(TEXT_COLOR); 
            sf::FloatRect exitRect=exitMsg.getLocalBounds(); 
            exitMsg.setOrigin(exitRect.left+exitRect.width/2.0f,exitRect.top+exitRect.height/2.0f); 
            exitMsg.setPosition(window.getSize().x / 2.0f, window.getSize().y*(2.0f/3.0f)+30.f); 
            window.draw(exitMsg);
        }
        
        window.display();
    }

    return 0;
}


