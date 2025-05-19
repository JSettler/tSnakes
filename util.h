#ifndef UTIL_H
#define UTIL_H

#include "common.h" // <<< MUST BE FIRST to define GameConfig, Tile, etc.
#include <string>
#include <vector>
#include <SFML/System/Vector2.hpp> 
#include "bot.h"    // For Bot::AIState (or use int as before)


// Player data to save/load
struct PlayerSaveData {
    sf::Vector2i gridPos;
    Direction currentDirection; // From common.h
    Direction previousCurrentDirection;
    Direction nextDirection;
    bool isClaiming;
    std::vector<sf::Vector2i> tail;
};

// Bot data to save/load
struct BotSaveData {
    int id;
    sf::Vector2i gridPos;
    Direction currentDirection; // From common.h
    bool isMovingToTarget;      
    sf::Vector2f targetVisualPos; 
    bool isClaiming;
    bool alive;
    // Bot::AIState currentAIState; // Using int to avoid direct Bot class dep here if possible
    int currentAIState_int; 
    int stepsInCurrentDirection;
    int loopLegCounter;
    sf::Vector2i initialSpawnPoint;
    sf::Vector2i loopStartPoint;
    std::vector<sf::Vector2i> tail;
};

// Overall game state to save/load
struct GameSaveData {
    GameConfig config; // GameConfig from common.h
    std::vector<std::vector<Tile>> mapState; // Tile from common.h
    PlayerSaveData playerData;
    std::vector<BotSaveData> botsData;
    GameState currentGameState; // GameState from common.h
    int playerScore;
    float timeSurvivedSeconds; 
};

bool saveGameStateToFile(const std::string& filename, const GameSaveData& saveData);
bool loadGameStateFromFile(const std::string& filename, GameSaveData& loadData, const GameConfig& defaultConfig); 
void displayHelp(const std::string& programName);
bool parseArguments(int argc, char* argv[], GameConfig& config); // GameConfig from common.h

#endif // UTIL_H


