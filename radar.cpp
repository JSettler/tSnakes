#include "radar.h"
#include "common.h" 
#include <iostream> 
#include <algorithm> // For std::max

// Constructor
Radar::Radar(sf::Vector2u windowResolution, unsigned int mapWidthInTiles, unsigned int mapHeightInTiles)
    : mapTilesWidth(mapWidthInTiles), mapTilesHeight(mapHeightInTiles), needsRedraw(true) { 
    
    float radarHeightPixels = static_cast<float>(windowResolution.y) * 0.20f; 
    radarSize = sf::Vector2f(radarHeightPixels, radarHeightPixels); 

    float radarMargin = 10.0f; 
    radarScreenPosition = sf::Vector2f(
        static_cast<float>(windowResolution.x) - radarSize.x - radarMargin,
        radarMargin
    );

    if (!radarRenderTexture.create(static_cast<unsigned int>(radarSize.x), static_cast<unsigned int>(radarSize.y))) {
        std::cerr << "Error: Could not create radar RenderTexture!" << std::endl;
    }
    radarRenderTexture.setSmooth(false); 

    radarSprite.setTexture(radarRenderTexture.getTexture());
    radarSprite.setPosition(radarScreenPosition);

    radarBackground.setSize(radarSize);
    radarBackground.setFillColor(sf::Color(10, 10, 15, 200)); 
    radarBackground.setPosition(0,0); 

    radarBorder.setSize(radarSize);
    radarBorder.setFillColor(sf::Color::Transparent);
    radarBorder.setOutlineColor(sf::Color(50, 50, 50, 200)); 
    radarBorder.setOutlineThickness(2.0f);
    radarBorder.setPosition(radarScreenPosition); // Border is drawn directly on window, so use screen pos

    // Calculate scaling factors for tiles on the radar
    tileScaleX = radarSize.x / static_cast<float>(mapTilesWidth);
    tileScaleY = radarSize.y / static_cast<float>(mapTilesHeight);

    updateInterval = sf::milliseconds(125); // Update radar texture every 125ms (8 times per second)
    updateClock.restart(); 
}

Radar::~Radar() {
    // Destructor
}

void Radar::updateAndPrepareState(const std::vector<std::vector<Tile>>& gameMapState, 
                                  const sf::Vector2i& playerGridPos, 
                                  const sf::Color& playerCurrentHeadColor,
                                  const std::vector<Bot>& currentBots) {
    if (updateClock.getElapsedTime() >= updateInterval || needsRedraw) {
        cachedGameMapState = gameMapState; // Use cached member name
        cachedPlayerGridPos = playerGridPos;
        cachedPlayerHeadColor = playerCurrentHeadColor;

        cachedBotsRadarInfo.clear(); // Use cached member name
        for (const auto& bot : currentBots) {
            if (bot.alive) { 
                BotRadarInfo info;
                info.gridPos = bot.gridPos;
                
                bool botOnOwnTerritory = false;
                if (bot.gridPos.x >= 0 && bot.gridPos.x < static_cast<int>(mapTilesWidth) &&
                    bot.gridPos.y >= 0 && bot.gridPos.y < static_cast<int>(mapTilesHeight)) {
                    // Check gameMapState directly, not cachedGameMapState, as this is current info
                    const Tile& botTile = gameMapState[bot.gridPos.x][bot.gridPos.y];
                    botOnOwnTerritory = (botTile.baseOwner == TileBaseOwner::BOT &&
                                         botTile.entityID == bot.id);
                }
                info.headColor = botOnOwnTerritory ? bot.safeHeadColor : bot.activeColor;
                info.alive = bot.alive; 
                // Removed baseOwner and entityID from BotRadarInfo as they are not needed
                // if territory color comes from iterating cachedGameMapState.
                cachedBotsRadarInfo.push_back(info); // Use cached member name
            }
        }
        
        redrawRadarTexture();
        updateClock.restart();
        needsRedraw = false;
    }
}

void Radar::redrawRadarTexture() {
    radarRenderTexture.clear(sf::Color::Transparent); 
    radarRenderTexture.draw(radarBackground); 

    sf::RectangleShape radarTileShape;

    // 1. Draw Territories
    for (unsigned int y = 0; y < mapTilesHeight; ++y) {
        for (unsigned int x = 0; x < mapTilesWidth; ++x) {
            const Tile& currentMapTile = cachedGameMapState[x][y]; // Use cached member name
            if (currentMapTile.baseOwner != TileBaseOwner::NEUTRAL) {
                sf::Color territoryColor;
                if (currentMapTile.baseOwner == TileBaseOwner::PLAYER) {
                    territoryColor = PLAYER_TERRITORY_COLOR; 
                } else if (currentMapTile.baseOwner == TileBaseOwner::BOT) {
                    if (currentMapTile.entityID >= 0 && currentMapTile.entityID < MAX_BOTS_ALLOWED) {
                        territoryColor = BOT_TERRITORY_COLORS[currentMapTile.entityID]; 
                    } else {
                        territoryColor = sf::Color::White; 
                    }
                } else { // Should not happen if NEUTRAL is handled
                    territoryColor = NEUTRAL_TERRITORY_COLOR;
                }

                radarTileShape.setFillColor(territoryColor);
                radarTileShape.setSize(sf::Vector2f(std::max(1.0f, tileScaleX), std::max(1.0f, tileScaleY)));
                radarTileShape.setPosition(x * tileScaleX, y * tileScaleY);
                radarRenderTexture.draw(radarTileShape);
            }
        }
    }

    // 2. Draw Heads (on top of territories)
    sf::RectangleShape headMarkerShape;
    
    // Player Head
    headMarkerShape.setFillColor(cachedPlayerHeadColor); // Use cached member name
    float headMarkerSizeX = std::max(1.0f, tileScaleX * 2.0f); // Make heads a bit more prominent
    float headMarkerSizeY = std::max(1.0f, tileScaleY * 2.0f);
    headMarkerShape.setSize(sf::Vector2f(headMarkerSizeX, headMarkerSizeY));
    headMarkerShape.setOrigin(headMarkerSizeX / 2.0f, headMarkerSizeY / 2.0f); 
    headMarkerShape.setPosition(
        (cachedPlayerGridPos.x + 0.5f) * tileScaleX, // Use cached member name
        (cachedPlayerGridPos.y + 0.5f) * tileScaleY  // Use cached member name
    );
    radarRenderTexture.draw(headMarkerShape);

    // Bot Heads
    for (const auto& botInfo : cachedBotsRadarInfo) { // Use cached member name
        if (botInfo.alive) {
            headMarkerShape.setFillColor(botInfo.headColor);
            headMarkerShape.setPosition(
                (botInfo.gridPos.x + 0.5f) * tileScaleX,
                (botInfo.gridPos.y + 0.5f) * tileScaleY
            );
            radarRenderTexture.draw(headMarkerShape);
        }
    }
    
    radarRenderTexture.display(); 
}

void Radar::render(sf::RenderWindow& window) {
    window.draw(radarSprite);
    window.draw(radarBorder); 
}

void Radar::setPosition(float x, float y) {
    radarScreenPosition = sf::Vector2f(x, y);
    radarSprite.setPosition(radarScreenPosition);
    radarBorder.setPosition(radarScreenPosition);
}


