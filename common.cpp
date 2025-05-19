#include "common.h"
#include <cmath>     
#include <iostream>  
#include <vector>    

// --- Definition for Global Game Configuration Instance ---
GameConfig G_CONFIG; 

// --- Definition for Global Game Map ---
std::vector<std::vector<Tile>> gameMap(G_CONFIG.mapWidthTiles, std::vector<Tile>(G_CONFIG.mapHeightTiles));

// --- Definitions for Global Bot Color Arrays ---
const sf::Color BOT_PRIMARY_ACTIVE_COLORS[MAX_BOTS_ALLOWED] = { // MAX_BOTS_ALLOWED should be 63 from common.h
    sf::Color(230, 50, 50),   // 0 Bright Red
    sf::Color(50, 50, 230),   // 1 Bright Blue
    sf::Color(50, 200, 50),   // 2 Bright Green
    sf::Color(230, 230, 50),  // 3 Bright Yellow
    sf::Color(50, 220, 220),  // 4 Bright Cyan
    sf::Color(230, 50, 230),  // 5 Bright Magenta
    sf::Color(255, 120, 0),  // 6 Bright Orange
    sf::Color(150, 50, 240),  // 7 Bright Purple
    sf::Color(255,105,180),   // 8 HotPink
    sf::Color(124,252,0),     // 9 LawnGreen
    sf::Color(70,130,180),    // 10 SteelBlue
    sf::Color(218,165,32),    // 11 Goldenrod
    sf::Color(255,20,147),    // 12 DeepPink
    sf::Color(0,255,127),     // 13 SpringGreen
    sf::Color(138,43,226),    // 14 BlueViolet
    sf::Color(210,105,30),    // 15 Chocolate
    sf::Color(100, 149, 237), // 16 CornflowerBlue
    sf::Color(255, 99, 71),   // 17 Tomato
    sf::Color(60, 179, 113),  // 18 MediumSeaGreen
    // sf::Color(255, 215, 0),   // Gold - (Similar to Goldenrod, removing for variety)
    sf::Color(176, 196, 222), // 19 LightSteelBlue
    sf::Color(240, 128, 128), // 20 LightCoral
    sf::Color(32, 178, 170),  // 21 LightSeaGreen
    // sf::Color(255, 250, 205), // LemonChiffon (Too light, removing)
    sf::Color(123, 104, 238), // 22 MediumSlateBlue
    sf::Color(205, 92, 92),   // 23 IndianRed
    sf::Color(0, 191, 255),   // 24 DeepSkyBlue
    sf::Color(244, 164, 96),  // 25 SandyBrown
    sf::Color(154, 205, 50),  // 26 YellowGreen
    sf::Color(72, 61, 139),   // 27 DarkSlateBlue
    sf::Color(200, 80, 80),   // 28 (Variation of Red)
    sf::Color(80, 80, 200),   // 29 (Variation of Blue)
    sf::Color(80, 180, 80),   // 30 (Variation of Green)
    sf::Color(200, 200, 80),  // 31 (Variation of Yellow)
    sf::Color(80, 200, 200),  // 32 (Variation of Cyan)
    sf::Color(200, 80, 200),  // 33 (Variation of Magenta)
    sf::Color(230, 100, 50),  // 34 (Variation of Orange)
    sf::Color(130, 80, 220),  // 35 (Variation of Purple)
    sf::Color(230,130,190),   // 36 (Variation of Pink)
    sf::Color(100,230,50),    // 37 (Variation of Lime)
    sf::Color(100,100,150),   // 38 (Darker Slate Blue)
    sf::Color(180,130,80),    // 39 (Brownish Orange)
    sf::Color(230,80,100),    // 40 (Reddish Pink)
    sf::Color(80,230,100),    // 41 (Greenish Lime)
    sf::Color(100,80,180),    // 42 (Darker Purple)
    sf::Color(180,100,80),    // 43 (Brownish Red)
    sf::Color(255,192,203),   // 44 Pink
    sf::Color(175,238,238),   // 45 PaleTurquoise
    sf::Color(210,180,140),   // 46 Tan
    sf::Color(176,224,230),   // 47 PowderBlue
    sf::Color(255,228,181),   // 48 Moccasin
    sf::Color(144,238,144),   // 49 LightGreen
    // sf::Color(255,182,193), // LightPink - (Similar to Pink, removing)
    sf::Color(255,222,173),   // 50 NavajoWhite
    sf::Color(240,230,140),   // 51 Khaki
    // sf::Color(224,255,255), // LightCyan - (Similar to PaleTurquoise/Cyan, removing)
    // sf::Color(250,235,215), // AntiqueWhite - (Too light, removing)
    sf::Color(255,235,205),   // 52 BlanchedAlmond
    sf::Color(255,228,196),   // 53 Bisque
    sf::Color(245,245,220),   // 54 Beige
    // sf::Color(255,240,245), // LavenderBlush - (Too light, removing)
    sf::Color(240,255,240),   // 55 Honeydew
    sf::Color(240,248,255),   // 56 AliceBlue
    // sf::Color(248,248,255), // GhostWhite - (Too light, removing)
    sf::Color(245,255,250),   // 57 MintCream
    sf::Color(255,250,240),   // 58 FloralWhite
    // sf::Color(245,245,245), // WhiteSmoke - (Too light, removing)
    sf::Color(255,255,240),   // 59 Ivory
    sf::Color(240,255,255),   // 60 Azure
    sf::Color(230,230,250),   // 61 Lavender
    sf::Color(253,245,230)    // 62 OldLace
};

sf::Color BOT_TERRITORY_COLORS[MAX_BOTS_ALLOWED]; 

void initializeBotTerritoryColors() {
    for (int i = 0; i < MAX_BOTS_ALLOWED; ++i) {
        BOT_TERRITORY_COLORS[i] = sf::Color(
            BOT_PRIMARY_ACTIVE_COLORS[i].r / 2,
            BOT_PRIMARY_ACTIVE_COLORS[i].g / 2,
            BOT_PRIMARY_ACTIVE_COLORS[i].b / 2
        );
    }
    if (G_CONFIG.debugMode) { 
        std::cout << "Bot territory colors initialized for " << MAX_BOTS_ALLOWED << " potential bots." << std::endl;
    }
}


// --- Definitions for Common Utility Functions ---

sf::Vector2f gridToPixelForHead(sf::Vector2i gridPos) { 
    // TILE_SIZE and HEAD_MARGIN are from common.h
    return sf::Vector2f(gridPos.x * TILE_SIZE + HEAD_MARGIN, 
                        gridPos.y * TILE_SIZE + HEAD_MARGIN);
}

sf::Vector2i visualHeadPosToGrid(sf::Vector2f visualPos) { 
    // TILE_SIZE and HEAD_SIZE_FLOAT are from common.h
    float headCenterX = visualPos.x + HEAD_SIZE_FLOAT / 2.0f; 
    float headCenterY = visualPos.y + HEAD_SIZE_FLOAT / 2.0f;
    return sf::Vector2i(static_cast<int>(std::floor(headCenterX / TILE_SIZE)), 
                        static_cast<int>(std::floor(headCenterY / TILE_SIZE)));
}

bool isOppositeDirection(Direction dir1, Direction dir2) { // Direction enum from common.h
    if (dir1 == Direction::NONE || dir2 == Direction::NONE) return false;
    if (dir1 == Direction::UP && dir2 == Direction::DOWN) return true; 
    if (dir1 == Direction::DOWN && dir2 == Direction::UP) return true;
    if (dir1 == Direction::LEFT && dir2 == Direction::RIGHT) return true; 
    if (dir1 == Direction::RIGHT && dir2 == Direction::LEFT) return true;
    return false;
}

// Definition for createGenericHeadTexture
sf::Texture createGenericHeadTexture(unsigned int size, float outlineThicknessFloat,
                                    const sf::Color& fillColor, const sf::Color& outlineColor,
                                    const sf::Color& cornerPixelColor) {
    sf::Image image;
    image.create(size, size, sf::Color::Transparent); 

    int outlineThickness = static_cast<int>(std::round(outlineThicknessFloat));
    if (outlineThickness < 1) outlineThickness = 1; 
    if (static_cast<unsigned int>(outlineThickness * 2) > size) {
        outlineThickness = size / 2;
        if (outlineThickness == 0 && size > 0) outlineThickness = 1;
        else if (size == 0) { 
             sf::Texture texture; 
             if (G_CONFIG.debugMode) std::cerr << "Warning: createGenericHeadTexture called with size 0." << std::endl;
             return texture;
        }
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
        } else if (size > 0 && outlineThickness == 0) { 
             for(unsigned int y_img=0; y_img<size; ++y_img) {
                for(unsigned int x_img=0; x_img<size; ++x_img) {
                    image.setPixel(x_img,y_img, cornerPixelColor);
                }
             }
        } else if (size > 0 && outlineThickness > 0 && cornerPixelColor != outlineColor) {
            if (size >= 1) { image.setPixel(0,0,cornerPixelColor);
                if (size >=2) { image.setPixel(1,0,cornerPixelColor); image.setPixel(0,1,cornerPixelColor); }
            }
            if (size >= 2) { image.setPixel(size-1,0,cornerPixelColor);
                if (size >=2) { image.setPixel(size-2,0,cornerPixelColor); image.setPixel(size-1,1,cornerPixelColor); }
            }
             if (size >= 2) { image.setPixel(0,size-1,cornerPixelColor);
                if (size >=2) { image.setPixel(1,size-1,cornerPixelColor); image.setPixel(0,size-2,cornerPixelColor); }
            }
            if (size >= 2) { image.setPixel(size-1,size-1,cornerPixelColor);
                if (size >=2) { image.setPixel(size-2,size-1,cornerPixelColor); image.setPixel(size-1,size-2,cornerPixelColor); }
            }
        }
    }
    sf::Texture texture; 
    if (!texture.loadFromImage(image)) {
        std::cerr << "Error: Could not create generic head texture from image." << std::endl;
    }
    return texture;
}


