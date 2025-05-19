#ifndef COMMON_H
#define COMMON_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string> 

// --- Game Configuration Struct ---
struct GameConfig {
    bool debugMode = false;
    int numBots = 8; 
    int mapWidthTiles = 500;
    int mapHeightTiles = 500;
    float playerSpeed = 10.0f; // <<< NEW: Default player speed
    float botSpeed = 10.0f;    // <<< NEW: Default bot speed
};

// --- Grid & Tile ---
const float TILE_SIZE = 30.0f; 
const float GRID_LINE_THICKNESS = 1.0f; 
const sf::Color GRID_COLOR = sf::Color(15, 15, 15); 

// --- Head Sizing & Margins ---
const float HEAD_MARGIN = 2.0f;
const int HEAD_SIZE_INT = static_cast<int>(TILE_SIZE - (2.0f * HEAD_MARGIN)); 
const float HEAD_SIZE_FLOAT = static_cast<float>(HEAD_SIZE_INT); 

// --- Map Configuration ---
const int MAX_BOTS_ALLOWED = 63; 

// --- Base Colors ---
const sf::Color NEUTRAL_TERRITORY_COLOR = sf::Color(25, 25, 30); 
const sf::Color PLAYER_ACTIVE_COLOR = sf::Color(0, 180, 0); 
const sf::Color PLAYER_TERRITORY_COLOR = sf::Color(PLAYER_ACTIVE_COLOR.r / 2, PLAYER_ACTIVE_COLOR.g / 2, PLAYER_ACTIVE_COLOR.b / 2); 
extern const sf::Color BOT_PRIMARY_ACTIVE_COLORS[MAX_BOTS_ALLOWED];
extern sf::Color BOT_TERRITORY_COLORS[MAX_BOTS_ALLOWED]; 
const int MAX_BOTS = MAX_BOTS_ALLOWED; 

// --- Tail Configuration (Visuals) ---
const float SNAKE_TAIL_THICKNESS = TILE_SIZE * 0.75f; 

// --- Movement Speeds (REMOVED - now in GameConfig as playerSpeed and botSpeed) ---
// const float PLAYER_TILES_PER_SECOND = 10.0f; 
// const float BOT_DEFAULT_TILES_PER_SECOND = 10.0f; 

// --- Enums ---
enum class Direction { NONE, UP, DOWN, LEFT, RIGHT };
enum class TileBaseOwner { NEUTRAL, PLAYER, BOT };
enum class GameState { PLAYING, GAME_OVER };

// --- Tile Structure ---
struct Tile {
    TileBaseOwner baseOwner = TileBaseOwner::NEUTRAL;
    int entityID = -1; 
};
const int PLAYER_ENTITY_ID = 0; 

extern std::vector<std::vector<Tile>> gameMap;
extern GameConfig G_CONFIG; 

// --- Common Utility Function Declarations ---
sf::Vector2f gridToPixelForHead(sf::Vector2i gridPos);
sf::Vector2i visualHeadPosToGrid(sf::Vector2f visualPos);
bool isOppositeDirection(Direction dir1, Direction dir2); 
sf::Texture createGenericHeadTexture(unsigned int size, float outlineThicknessFloat,
                                    const sf::Color& fillColor, const sf::Color& outlineColor,
                                    const sf::Color& cornerPixelColor);
void initializeBotTerritoryColors(); 

#endif // COMMON_H


