#ifndef BOT_H
#define BOT_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string> 
#include "common.h" // For Direction, TileBaseOwner, TILE_SIZE, gameMap (extern), etc.

struct Bot {
    int id; 
    sf::Vector2i gridPos;
    sf::Vector2f visualPos;
    sf::Sprite headSprite; 

    Direction currentDirection;

    bool isMovingToTarget;
    sf::Vector2f targetVisualPos;

    std::vector<sf::Vector2i> tail;
    bool isClaiming; 
    
    // TileBaseOwner ownerBaseType; // Not strictly needed if id is used with Tile.entityID
    sf::Color territoryColor; 
    sf::Color activeColor;    // Primary color for tail and active head
    sf::Color safeHeadColor;    
    
    sf::Texture activeHeadTexture; 
    sf::Texture safeHeadTexture;   

    bool alive;
    float decisionTimer;        
    float timeSinceLastTurn;    

    enum class AIState { EXPLORING, MAKING_LOOP_LEG1, MAKING_LOOP_LEG2, MAKING_LOOP_LEG3, RETURNING_HOME };
    AIState currentAIState;
    int stepsInCurrentDirection;
    int loopLegCounter; 
    sf::Vector2i initialSpawnPoint; // Stores the center of its last/current 5x5 spawn
    sf::Vector2i loopStartPoint;    // Stores where a claiming loop began

    // --- RESPAWN TIMER MEMBERS REMOVED for instant respawn ---
    // bool respawnTimerActive; // REMOVED
    // float timeUntilRespawn;    // REMOVED
    // static constexpr float RESPAWN_DELAY = 5.0f; // REMOVED 
    // --- END REMOVAL ---

    // Constructor
    Bot(int bot_id, 
        sf::Color primary_active_c, 
        sf::Color safe_head_c,    
        sf::Vector2i start_pos);

    void initializeTextures(); 
    
    void update(float deltaTime, 
                std::vector<std::vector<Tile>>& gameMapRef, 
                const std::vector<sf::Vector2i>& playerTailRef, 
                bool isPlayerClaimingRef,
                sf::Vector2i playerGridPosRef, 
                std::vector<Bot>& allBots); 

    void renderTail(sf::RenderWindow& window, int cStartX, int cEndX, int cStartY, int cSEndY) const; 
    void renderHead(sf::RenderWindow& window) const; 
    
    void die(std::vector<std::vector<Tile>>& gameMapRef); 
    void respawn(std::vector<std::vector<Tile>>& gameMapRef, sf::Vector2i newStartPos);
};

#endif // BOT_H


