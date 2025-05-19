#include "util.h"
#include "common.h" 
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm> // For std::transform for lowercase string comparison
#include <sstream>   // For string stream parsing (not strictly needed for stoi/stof but can be useful)

// --- Command Line Argument Parsing ---
void displayHelp(const std::string& programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h, -?          Show this help message." << std::endl;
    /* std::cout << "  --debug <on|off>        Enable or disable debug output (default: " 
              << (GameConfig().debugMode ? "on" : "off") << ")." << std::endl; */
    std::cout << "  --bots <number>         Set the number of bots (0-" << MAX_BOTS_ALLOWED 
              << ", default: " << GameConfig().numBots << ")." << std::endl; 
    std::cout << "  --xsize <number>        Set arena width in tiles (50-2000, default: " 
              << GameConfig().mapWidthTiles << ")." << std::endl;
    std::cout << "  --ysize <number>        Set arena height in tiles (50-2000, default: " 
              << GameConfig().mapHeightTiles << ")." << std::endl;
    std::cout << "  --player_speed <number> Set player speed in tiles/sec (1.0-50.0, default: "
              << GameConfig().playerSpeed << ")." << std::endl;
    std::cout << "  --bot_speed <number>    Set bot speed in tiles/sec (1.0-50.0, default: "
              << GameConfig().botSpeed << ")." << std::endl;
}

bool parseArguments(int argc, char* argv[], GameConfig& config) { // config is G_CONFIG
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h" || arg == "-?") {
            displayHelp(argv[0]);
            return false; 
        } else if (arg == "--debug") {
            if (i + 1 < argc) {
                std::string val = argv[++i];
                std::transform(val.begin(), val.end(), val.begin(), 
                    [](unsigned char c){ return std::tolower(c); });
                if (val == "on") {
                    config.debugMode = true;
                } else if (val == "off") {
                    config.debugMode = false;
                } else {
                    std::cerr << "Error: Invalid value for --debug. Use 'on' or 'off'." << std::endl;
                    displayHelp(argv[0]); return false;
                }
            } else { std::cerr << "Error: --debug option requires a value (on/off)." << std::endl; displayHelp(argv[0]); return false; }
        } else if (arg == "--bots") {
            if (i + 1 < argc) {
                try {
                    int num = std::stoi(argv[++i]);
                    if (num >= 0 && num <= MAX_BOTS_ALLOWED) { 
                        config.numBots = num;
                    } else {
                        std::cerr << "Error: Number of bots must be between 0 and " << MAX_BOTS_ALLOWED << "." << std::endl;
                        displayHelp(argv[0]); return false;
                    }
                } catch (const std::exception& e) { std::cerr << "Error: Invalid number for --bots: " << argv[i] << " (" << e.what() << ")" << std::endl; displayHelp(argv[0]); return false;}
            } else { std::cerr << "Error: --bots option requires a number." << std::endl; displayHelp(argv[0]); return false; }
        } else if (arg == "--xsize") {
            if (i + 1 < argc) {
                try {
                    int size = std::stoi(argv[++i]);
                    if (size >= 50 && size <= 2000) { config.mapWidthTiles = size;
                    } else { std::cerr << "Error: Arena X size must be between 50 and 2000." << std::endl; displayHelp(argv[0]); return false; }
                } catch (const std::exception& e) { std::cerr << "Error: Invalid number for --xsize: " << argv[i] << " (" << e.what() << ")" << std::endl; displayHelp(argv[0]); return false; }
            } else { std::cerr << "Error: --xsize option requires a number." << std::endl; displayHelp(argv[0]); return false; }
        } else if (arg == "--ysize") {
            if (i + 1 < argc) {
                try {
                    int size = std::stoi(argv[++i]);
                    if (size >= 50 && size <= 2000) { config.mapHeightTiles = size;
                    } else { std::cerr << "Error: Arena Y size must be between 50 and 2000." << std::endl; displayHelp(argv[0]); return false; }
                } catch (const std::exception& e) { std::cerr << "Error: Invalid number for --ysize: " << argv[i] << " (" << e.what() << ")" << std::endl; displayHelp(argv[0]); return false; }
            } else { std::cerr << "Error: --ysize option requires a number." << std::endl; displayHelp(argv[0]); return false; }
        } else if (arg == "--player_speed") { // <<< NEW
            if (i + 1 < argc) {
                try {
                    float speed = std::stof(argv[++i]);
                    if (speed >= 1.0f && speed <= 50.0f) {
                        config.playerSpeed = speed;
                    } else {
                        std::cerr << "Error: Player speed must be between 1.0 and 50.0." << std::endl;
                        displayHelp(argv[0]); return false;
                    }
                } catch (const std::exception& e) { std::cerr << "Error: Invalid number for --player_speed: " << argv[i] << " (" << e.what() << ")" << std::endl; displayHelp(argv[0]); return false; }
            } else { std::cerr << "Error: --player_speed option requires a number." << std::endl; displayHelp(argv[0]); return false; }
        } else if (arg == "--bot_speed") { // <<< NEW
            if (i + 1 < argc) {
                try {
                    float speed = std::stof(argv[++i]);
                    if (speed >= 1.0f && speed <= 50.0f) {
                        config.botSpeed = speed;
                    } else {
                        std::cerr << "Error: Bot speed must be between 1.0 and 50.0." << std::endl;
                        displayHelp(argv[0]); return false;
                    }
                } catch (const std::exception& e) { std::cerr << "Error: Invalid number for --bot_speed: " << argv[i] << " (" << e.what() << ")" << std::endl; displayHelp(argv[0]); return false; }
            } else { std::cerr << "Error: --bot_speed option requires a number." << std::endl; displayHelp(argv[0]); return false; }
        }
         else {
            std::cerr << "Error: Unknown option '" << arg << "'." << std::endl;
            std::cout << "Use '" << argv[0] << " --help' for usage information." << std::endl;
            return false;
        }
    }
    return true; 
}


// --- Save Game State ---
bool saveGameStateToFile(const std::string& filename, const GameSaveData& saveData) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open save file for writing: " << filename << std::endl;
        return false;
    }

    // Save GameConfig first
    outFile << saveData.config.debugMode << std::endl;
    outFile << saveData.config.numBots << std::endl;
    outFile << saveData.config.mapWidthTiles << std::endl;
    outFile << saveData.config.mapHeightTiles << std::endl;
    outFile << saveData.config.playerSpeed << std::endl; // <<< NEW
    outFile << saveData.config.botSpeed << std::endl;    // <<< NEW

    // Save GameState, Score, Time
    outFile << static_cast<int>(saveData.currentGameState) << std::endl;
    outFile << saveData.playerScore << std::endl;
    outFile << saveData.timeSurvivedSeconds << std::endl;

    // Save Player Data
    outFile << saveData.playerData.gridPos.x << " " << saveData.playerData.gridPos.y << std::endl;
    outFile << static_cast<int>(saveData.playerData.currentDirection) << std::endl;
    outFile << static_cast<int>(saveData.playerData.previousCurrentDirection) << std::endl;
    outFile << static_cast<int>(saveData.playerData.nextDirection) << std::endl;
    outFile << saveData.playerData.isClaiming << std::endl;
    outFile << saveData.playerData.tail.size() << std::endl;
    for (const auto& p : saveData.playerData.tail) outFile << p.x << " " << p.y << " ";
    outFile << std::endl;

    // Save Number of Bots and Bot Data
    outFile << saveData.botsData.size() << std::endl;
    for (const auto& botData : saveData.botsData) {
        outFile << botData.id << std::endl;
        outFile << botData.gridPos.x << " " << botData.gridPos.y << std::endl;
        outFile << static_cast<int>(botData.currentDirection) << std::endl;
        outFile << botData.isMovingToTarget << std::endl; 
        outFile << botData.targetVisualPos.x << " " << botData.targetVisualPos.y << std::endl; 
        outFile << botData.isClaiming << std::endl; outFile << botData.alive << std::endl;
        outFile << botData.currentAIState_int << std::endl; 
        outFile << botData.stepsInCurrentDirection << std::endl; outFile << botData.loopLegCounter << std::endl;
        outFile << botData.initialSpawnPoint.x << " " << botData.initialSpawnPoint.y << std::endl;
        outFile << botData.loopStartPoint.x << " " << botData.loopStartPoint.y << std::endl;
        outFile << botData.tail.size() << std::endl;
        for (const auto& p : botData.tail) outFile << p.x << " " << p.y << " ";
        outFile << std::endl;
    }
    // Save Map Data (Owner and Entity ID for each tile)
    outFile << saveData.config.mapWidthTiles << " " << saveData.config.mapHeightTiles << std::endl; 
    for (int y = 0; y < saveData.config.mapHeightTiles; ++y) {
        for (int x = 0; x < saveData.config.mapWidthTiles; ++x) {
            outFile << static_cast<int>(saveData.mapState[x][y].baseOwner) << " " 
                    << saveData.mapState[x][y].entityID << " ";
        }
        outFile << std::endl;
    }
    outFile.close();
    if (G_CONFIG.debugMode) { 
        std::cout << "Game state saved to " << filename << std::endl;
    }
    return true;
}


// --- Load Game State ---
bool loadGameStateFromFile(const std::string& filename, GameSaveData& loadData, const GameConfig& currentCmdLineConfig) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        std::cout << "Info: Save file not found: " << filename << ". Starting new game." << std::endl;
        return false;
    }
    
    std::string line; int tempInt; size_t tempSize; float tempFloat;

    // Load GameConfig first
    if (!(inFile >> loadData.config.debugMode)) return false;
    if (!(inFile >> loadData.config.numBots)) return false;
    if (!(inFile >> loadData.config.mapWidthTiles)) return false;
    if (!(inFile >> loadData.config.mapHeightTiles)) return false;
    if (!(inFile >> loadData.config.playerSpeed)) { // <<< NEW
        // For backward compatibility with old save files, provide a default if not found
        std::cout << "Warning: playerSpeed not found in save file, using default." << std::endl;
        loadData.config.playerSpeed = GameConfig().playerSpeed; // Default from GameConfig struct
    }
    if (!(inFile >> loadData.config.botSpeed)) { // <<< NEW
        std::cout << "Warning: botSpeed not found in save file, using default." << std::endl;
        loadData.config.botSpeed = GameConfig().botSpeed; // Default from GameConfig struct
    }


    // Validate loaded map dimensions against current command-line/default G_CONFIG
    // If command-line args specified a map size, that takes precedence over save file's map size.
    // However, if no args were given, we use the save file's map size.
    // The currentCmdLineConfig reflects defaults OR command-line overrides.
    // We should load the map based on the save file's dimensions,
    // but if main.cpp later resizes gameMap due to cmd line args, that's a separate issue.
    // For now, trust save file dimensions if they are reasonable.
    if (loadData.config.mapWidthTiles > 2000 || loadData.config.mapHeightTiles > 2000 || 
        loadData.config.mapWidthTiles < 10 || loadData.config.mapHeightTiles < 10) { 
        std::cerr << "Error: Loaded map dimensions (" << loadData.config.mapWidthTiles << "x" << loadData.config.mapHeightTiles
                  << ") are unreasonable. Save file corrupted or invalid." << std::endl;
        return false;
    }
    if (loadData.config.numBots < 0 || loadData.config.numBots > MAX_BOTS_ALLOWED) { 
         std::cerr << "Error: Loaded number of bots (" << loadData.config.numBots 
                   << ") is invalid. Save file corrupted or invalid." << std::endl;
        return false;
    }
    // Validate loaded speeds
    if (loadData.config.playerSpeed < 1.0f || loadData.config.playerSpeed > 50.0f) {
        std::cout << "Warning: Loaded playerSpeed (" << loadData.config.playerSpeed << ") out of range. Resetting to default." << std::endl;
        loadData.config.playerSpeed = GameConfig().playerSpeed;
    }
    if (loadData.config.botSpeed < 1.0f || loadData.config.botSpeed > 50.0f) {
        std::cout << "Warning: Loaded botSpeed (" << loadData.config.botSpeed << ") out of range. Resetting to default." << std::endl;
        loadData.config.botSpeed = GameConfig().botSpeed;
    }


    if(!(inFile >> tempInt)) return false; loadData.currentGameState = static_cast<GameState>(tempInt);
    if(!(inFile >> loadData.playerScore)) return false; if(!(inFile >> loadData.timeSurvivedSeconds)) return false;
    if(!(inFile >> loadData.playerData.gridPos.x >> loadData.playerData.gridPos.y)) return false;
    if(!(inFile >> tempInt)) return false; loadData.playerData.currentDirection = static_cast<Direction>(tempInt);
    if(!(inFile >> tempInt)) return false; loadData.playerData.previousCurrentDirection = static_cast<Direction>(tempInt);
    if(!(inFile >> tempInt)) return false; loadData.playerData.nextDirection = static_cast<Direction>(tempInt);
    if(!(inFile >> loadData.playerData.isClaiming)) return false; if(!(inFile >> tempSize)) return false; 
    loadData.playerData.tail.resize(tempSize); for(size_t i=0; i<tempSize; ++i) if(!(inFile >> loadData.playerData.tail[i].x >> loadData.playerData.tail[i].y)) return false;
    
    size_t numBotsInSave; if(!(inFile >> numBotsInSave)) return false;
    if (numBotsInSave != static_cast<size_t>(loadData.config.numBots) && G_CONFIG.debugMode) { // Use G_CONFIG for debug print
         std::cerr << "Warning: Bot count in save file body (" << numBotsInSave << ") differs from count in save file config (" << loadData.config.numBots << ")." << std::endl;
    }
    loadData.botsData.resize(numBotsInSave); // Trust the list size in the file
    for(size_t i=0; i<numBotsInSave; ++i){
        if(!(inFile >> loadData.botsData[i].id)) return false;
        if(!(inFile >> loadData.botsData[i].gridPos.x >> loadData.botsData[i].gridPos.y)) return false;
        if(!(inFile >> tempInt)) return false; loadData.botsData[i].currentDirection = static_cast<Direction>(tempInt);
        if(!(inFile >> loadData.botsData[i].isMovingToTarget)) return false;
        if(!(inFile >> loadData.botsData[i].targetVisualPos.x >> loadData.botsData[i].targetVisualPos.y)) return false;
        if(!(inFile >> loadData.botsData[i].isClaiming)) return false; if(!(inFile >> loadData.botsData[i].alive)) return false;
        if(!(inFile >> loadData.botsData[i].currentAIState_int)) return false;
        if(!(inFile >> loadData.botsData[i].stepsInCurrentDirection)) return false; if(!(inFile >> loadData.botsData[i].loopLegCounter)) return false;
        if(!(inFile >> loadData.botsData[i].initialSpawnPoint.x >> loadData.botsData[i].initialSpawnPoint.y)) return false;
        if(!(inFile >> loadData.botsData[i].loopStartPoint.x >> loadData.botsData[i].loopStartPoint.y)) return false;
        if(!(inFile >> tempSize)) return false; loadData.botsData[i].tail.resize(tempSize);
        for(size_t j=0; j<tempSize; ++j) if(!(inFile >> loadData.botsData[i].tail[j].x >> loadData.botsData[i].tail[j].y)) return false;
    }
    int mapW_s, mapH_s; if(!(inFile >> mapW_s >> mapH_s)) return false;
    if(mapW_s != loadData.config.mapWidthTiles || mapH_s != loadData.config.mapHeightTiles){ 
        std::cerr<<"Error: Map data section dimensions mismatch config section in save file. Corrupted."<<std::endl; return false;
    }
    loadData.mapState.assign(loadData.config.mapWidthTiles, std::vector<Tile>(loadData.config.mapHeightTiles));
    for(int y=0; y<loadData.config.mapHeightTiles; ++y) for(int x=0; x<loadData.config.mapWidthTiles; ++x){
        if(!(inFile >> tempInt)) return false; loadData.mapState[x][y].baseOwner = static_cast<TileBaseOwner>(tempInt);
        if(!(inFile >> loadData.mapState[x][y].entityID)) return false;
    }
    inFile.close();
    if (loadData.config.debugMode) { // Use the debug mode from the loaded config for this message
        std::cout << "Game state successfully parsed from " << filename << std::endl;
    }
    return true;
}


