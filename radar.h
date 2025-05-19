#ifndef RADAR_H
#define RADAR_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "common.h" // For Tile, Bot (if needed directly), map dimensions, colors
#include "bot.h"    // For Bot struct definition (used in BotRadarInfo if we keep it, or for passing currentBots)

class Radar {
public:
    Radar(sf::Vector2u windowResolution, unsigned int mapWidthInTiles, unsigned int mapHeightInTiles);
    ~Radar();

    // Call this every game frame; it will internally decide when to redraw the texture
    void updateAndPrepareState(const std::vector<std::vector<Tile>>& gameMapState, 
                               const sf::Vector2i& playerGridPos, 
                               const sf::Color& playerCurrentHeadColor,
                               const std::vector<Bot>& currentBots); // Pass const ref to vector of Bots

    void render(sf::RenderWindow& window);
    void setPosition(float x, float y); // <<< Declaration added here

private:
    void redrawRadarTexture(); // Internal method to update the render texture

    sf::RenderTexture radarRenderTexture; // Texture to draw the radar content onto
    sf::Sprite radarSprite;             // Sprite to display the radar texture
    sf::RectangleShape radarBackground;   // Background shape of the radar area
    sf::RectangleShape radarBorder;       // Optional border for the radar

    sf::Vector2f radarScreenPosition;   // Top-left position on the screen
    sf::Vector2f radarSize;             // Width and height of the radar view

    // unsigned int mapPixelWidth;      // Full map width in game pixels (tiles * TILE_SIZE) - Not strictly needed as member
    // unsigned int mapPixelHeight;     // Full map height in game pixels - Not strictly needed as member
    unsigned int mapTilesWidth;         // Map width in tiles (from common.h)
    unsigned int mapTilesHeight;        // Map height in tiles (from common.h)

    float tileScaleX;                   // How much one game tile is scaled on the radar X
    float tileScaleY;                   // How much one game tile is scaled on the radar Y

    sf::Clock updateClock;
    sf::Time updateInterval;

    // Cached state for redrawing. When updateAndPrepareState decides it's time to redraw,
    // it copies the relevant current game state into these members, then calls redrawRadarTexture.
    std::vector<std::vector<Tile>> cachedGameMapState;
    sf::Vector2i cachedPlayerGridPos;
    sf::Color cachedPlayerHeadColor;
    
    // Store essential info for bots on the radar to avoid copying full Bot objects frequently
    struct BotRadarInfo {
        sf::Vector2i gridPos;
        sf::Color headColor; // Current head color (safe or active)
        bool alive;
        // We don't need baseOwner/entityID here if we pass territory colors directly or get them via gameMap
    };
    std::vector<BotRadarInfo> cachedBotsRadarInfo;
    bool needsRedraw; // Flag to force a redraw (e.g., on initialization or game state change)
};

#endif // RADAR_H


