/*
 * ═══════════════════════════════════════════════════════════════════════
 *  PLANETARY DEFENSE COMMAND - Main Engine (Raylib Version)
 * ═══════════════════════════════════════════════════════════════════════
 *  Entry point for the game engine with GRAPHICAL rendering!
 *  NOTE: This file is part of the engine - students should NOT edit this.
 * 
 *  HOW IT WORKS:
 *  This engine CALLS the functions that the student implements in
 *  src/student/*.cpp files. The student writes the logic, the engine
 *  runs the game with real graphics!
 * 
 *  REQUIRES: Raylib 5.0 (run scripts/setup_raylib.ps1 first)
 * ═══════════════════════════════════════════════════════════════════════
 */

#include "raylib.h"
#include "GameEngine.h"
#include <string>
#include <cmath>
#include <vector>

// ═══════════════════════════════════════════════════════════════════════
// GAME CONSTANTS
// ═══════════════════════════════════════════════════════════════════════
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 660;  // Extended for button bar below grid
const int GRID_SIZE = 10;
const int CELL_SIZE = 50;
const int GRID_OFFSET_X = 50;
const int GRID_OFFSET_Y = 100;

const int TOWER_COST = 100;
const double TOWER_RANGE = 3.0;
const int TOWER_BASE_DAMAGE = 10;
const int TOWER_LEVEL = 1;

// Colors - Space Theme
const Color COLOR_BACKGROUND = { 8, 8, 24, 255 };         // Deep space
const Color COLOR_GRID = { 30, 40, 60, 255 };             // Subtle grid
const Color COLOR_GRID_HOVER = { 60, 80, 120, 200 };      // Hovered cell
const Color COLOR_BASE = { 80, 140, 255, 255 };           // Bright blue base
const Color COLOR_BASE_GLOW = { 80, 140, 255, 40 };       // Base glow
const Color COLOR_TOWER = { 0, 255, 150, 255 };           // Cyan-green tower
const Color COLOR_TOWER_DARK = { 0, 150, 90, 255 };       // Tower shadow
const Color COLOR_TOWER_RANGE = { 0, 255, 150, 25 };      // Tower range (subtle)
const Color COLOR_ENEMY = { 255, 60, 80, 255 };           // Red enemy
const Color COLOR_ENEMY_DARK = { 180, 30, 50, 255 };      // Enemy shadow
const Color COLOR_ENEMY_HEALTH = { 50, 255, 100, 255 };   // Bright green health
const Color COLOR_UI_TEXT = { 200, 210, 230, 255 };       // Soft white
const Color COLOR_MONEY = { 255, 220, 50, 255 };          // Gold
const Color COLOR_HEALTH = { 80, 220, 120, 255 };         // Green health
const Color COLOR_PROJECTILE = { 0, 255, 200, 255 };      // Cyan laser
const Color COLOR_PROJECTILE_GLOW = { 0, 255, 200, 100 }; // Laser glow

// Stars for background
struct Star { float x, y, brightness; };
const int NUM_STARS = 100;
Star stars[NUM_STARS];

// Custom Font
Font gameFont;
bool fontLoaded = false;

// ═══════════════════════════════════════════════════════════════════════
// GAME STATE
// ═══════════════════════════════════════════════════════════════════════
int playerMoney = 500;
int baseHealth = 100;
int score = 0;
int wave = 1;

// Tower data (support multiple towers)
struct Tower {
    int gridX;
    int gridY;
    float cooldown;
    bool active;
    int kills;       // Track kills for upgrade system
    int level;       // Tower level (1-5)
    std::string type; // "laser", "missile", "plasma"
};
const int MAX_TOWERS = 10;
Tower towers[MAX_TOWERS];
int towerCount = 0;
int totalPlayerKills = 0;  // Track total kills for player rank

// Enemy data (support multiple enemies)
struct GameEnemy {
    float x;       // Pixel position for smooth movement
    float y;
    int health;
    float speed;
    bool alive;
};
const int MAX_ENEMIES = 20;
GameEnemy enemies[MAX_ENEMIES];
int enemyCount = 0;

// Projectile data for visual effects
struct Projectile {
    float x, y;
    float targetX, targetY;
    float speed;
    bool active;
};
const int MAX_PROJECTILES = 50;
Projectile projectiles[MAX_PROJECTILES];

// Game timing
float enemySpawnTimer = 0;
float gameSpeed = 1.0f;
bool gamePaused = false;
bool gameOver = false;

// UI State
int hoveredCellX = -1;
int hoveredCellY = -1;
std::string statusMessage = "Click on the grid to place towers!";
float messageTimer = 0;

// Button/Selection State
int selectedTowerIndex = -1;  // -1 = no tower selected
bool showUpgradePanel = false;

// Button rectangles (defined here for click detection)
Rectangle btnRestart = {0, 0, 0, 0};
Rectangle btnAutoUpgrade = {0, 0, 0, 0};
Rectangle btnUpgradeTower = {0, 0, 0, 0};

// ═══════════════════════════════════════════════════════════════════════
// HELPER FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════

// Convert grid coordinates to screen pixels
Vector2 gridToScreen(int gridX, int gridY) {
    return { 
        (float)(GRID_OFFSET_X + gridX * CELL_SIZE + CELL_SIZE / 2),
        (float)(GRID_OFFSET_Y + gridY * CELL_SIZE + CELL_SIZE / 2)
    };
}

// Convert screen pixels to grid coordinates
void screenToGrid(int screenX, int screenY, int* gridX, int* gridY) {
    *gridX = (screenX - GRID_OFFSET_X) / CELL_SIZE;
    *gridY = (screenY - GRID_OFFSET_Y) / CELL_SIZE;
}

// Check if grid position is valid
bool isValidGridPos(int gridX, int gridY) {
    return gridX >= 0 && gridX < GRID_SIZE && gridY >= 0 && gridY < GRID_SIZE;
}

// Check if a tower exists at position
bool hasTowerAt(int gridX, int gridY) {
    for (int i = 0; i < towerCount; i++) {
        if (towers[i].active && towers[i].gridX == gridX && towers[i].gridY == gridY) {
            return true;
        }
    }
    return false;
}

// Check if position is the base
bool isBasePosition(int gridX, int gridY) {
    return gridX == GRID_SIZE - 1 && (gridY == 4 || gridY == 5);
}

// Calculate distance between two points
double calculateDistance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

// Set status message
void setMessage(const std::string& msg) {
    statusMessage = msg;
    messageTimer = 3.0f;
}

// ═══════════════════════════════════════════════════════════════════════
// CUSTOM FONT DRAWING HELPER
// ═══════════════════════════════════════════════════════════════════════
// Draws text using the custom font with proper spacing

void DrawTextCustom(const char* text, int x, int y, int fontSize, Color color) {
    if (fontLoaded) {
        Vector2 pos = { (float)x, (float)y };
        DrawTextEx(gameFont, text, pos, (float)fontSize, 1.0f, color);
    } else {
        DrawText(text, x, y, fontSize, color);
    }
}

// ═══════════════════════════════════════════════════════════════════════
// RADAR HELPER - Convert game enemies to Radar format
// ═══════════════════════════════════════════════════════════════════════
// This creates data that can be passed to student's 04_Radar.cpp functions

std::vector<RadarEnemy> getRadarEnemies() {
    std::vector<RadarEnemy> radarList;
    
    // Base position for distance calculation
    float baseX = GRID_OFFSET_X + (GRID_SIZE - 1) * CELL_SIZE;
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].alive) continue;
        
        RadarEnemy re;
        re.distance = (baseX - enemies[i].x) / CELL_SIZE;  // Distance to base in grid units
        re.health = enemies[i].health;
        re.isFlying = false;  // Regular enemies don't fly (could be expanded later!)
        radarList.push_back(re);
    }
    return radarList;
}

// ═══════════════════════════════════════════════════════════════════════
// UPGRADE HELPER - Convert game towers to Upgrade format
// ═══════════════════════════════════════════════════════════════════════
// This creates data that can be passed to student's 05_Upgrades.cpp functions

std::vector<UpgradeTower> getUpgradeTowers() {
    std::vector<UpgradeTower> upgradeList;
    
    for (int i = 0; i < MAX_TOWERS; i++) {
        if (!towers[i].active) continue;
        
        UpgradeTower ut;
        ut.level = towers[i].level;
        ut.damage = getTowerBaseDamage();
        ut.range = getTowerRange();
        ut.type = towers[i].type;
        ut.kills = towers[i].kills;
        upgradeList.push_back(ut);
    }
    return upgradeList;
}

UpgradePlayer getUpgradePlayer() {
    UpgradePlayer up;
    up.name = getCommanderName();
    up.money = playerMoney;
    up.totalKills = totalPlayerKills;
    up.wavesCompleted = wave - 1;
    up.isPremium = false;  // Could be toggled with a key!
    return up;
}

// ═══════════════════════════════════════════════════════════════════════
// GAME INITIALIZATION
// ═══════════════════════════════════════════════════════════════════════

void initStars() {
    for (int i = 0; i < NUM_STARS; i++) {
        stars[i].x = (float)(rand() % SCREEN_WIDTH);
        stars[i].y = (float)(rand() % SCREEN_HEIGHT);
        stars[i].brightness = 0.3f + (float)(rand() % 70) / 100.0f;
    }
}

void initGame() {
    // Initialize starfield
    initStars();
    
    // *** CALLING STUDENT'S FUNCTIONS from 01_Economy.cpp ***
    // These values come from the student's code!
    playerMoney = getStartingMoney();
    baseHealth = getBaseHealth();
    
    score = 0;
    wave = 1;
    towerCount = 0;
    enemyCount = 0;
    enemySpawnTimer = 2.0f;
    gameOver = false;
    gamePaused = false;
    
    // Clear towers
    for (int i = 0; i < MAX_TOWERS; i++) {
        towers[i].active = false;
        towers[i].kills = 0;
        towers[i].level = 1;
        towers[i].type = "laser";
    }
    
    // Reset total kills
    totalPlayerKills = 0;
    
    // Clear enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].alive = false;
    }
    
    // Clear projectiles
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        projectiles[i].active = false;
    }
    
    // Reset UI state
    selectedTowerIndex = -1;
    showUpgradePanel = false;
    
    // *** CALLING STUDENT'S FUNCTION: getCommanderName() ***
    setMessage("Welcome, " + getCommanderName() + "! Click to place towers.");
}

// Spawn a new enemy
void spawnEnemy() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].alive) {
            enemies[i].x = (float)GRID_OFFSET_X;
            enemies[i].y = (float)(GRID_OFFSET_Y + (3 + (wave % 5)) * CELL_SIZE + CELL_SIZE / 2);
            enemies[i].health = 50 + wave * 10;
            enemies[i].speed = 30.0f + wave * 2.0f;
            enemies[i].alive = true;
            enemyCount++;
            return;
        }
    }
}

// Fire a projectile from tower to enemy
void fireProjectile(float fromX, float fromY, float toX, float toY) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!projectiles[i].active) {
            projectiles[i].x = fromX;
            projectiles[i].y = fromY;
            projectiles[i].targetX = toX;
            projectiles[i].targetY = toY;
            projectiles[i].speed = 500.0f;
            projectiles[i].active = true;
            return;
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════
// GAME UPDATE
// ═══════════════════════════════════════════════════════════════════════

void updateGame(float deltaTime) {
    if (gamePaused || gameOver) return;
    
    // Update message timer
    if (messageTimer > 0) {
        messageTimer -= deltaTime;
    }
    
    // Spawn enemies
    enemySpawnTimer -= deltaTime;
    if (enemySpawnTimer <= 0) {
        spawnEnemy();
        enemySpawnTimer = 3.0f - (wave * 0.1f);  // Faster spawns each wave
        if (enemySpawnTimer < 1.0f) enemySpawnTimer = 1.0f;
    }
    
    // Update enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].alive) continue;
        
        // Move enemy toward base
        enemies[i].x += enemies[i].speed * deltaTime;
        
        // Check if enemy reached base
        float baseX = GRID_OFFSET_X + (GRID_SIZE - 1) * CELL_SIZE;
        if (enemies[i].x >= baseX) {
            enemies[i].alive = false;
            enemyCount--;
            baseHealth -= 20;
            setMessage("ALERT: Enemy breached defenses! -20 HP");
            
            if (baseHealth <= 0) {
                gameOver = true;
                setMessage("GAME OVER - Base Destroyed!");
            }
        }
    }
    
    // Update towers - check for enemies in range
    for (int t = 0; t < MAX_TOWERS; t++) {
        if (!towers[t].active) continue;
        
        // *** CALLING STUDENT'S FUNCTION: calculateFireRate() from 03_Targeting.cpp ***
        // Fire rate based on upgrade level (simulated as tower index for variety)
        int upgradeLevel = t % 3;  // 0, 1, or 2
        int fireRateMs = calculateFireRate(500, upgradeLevel);  // Base 500ms
        float cooldownTime = fireRateMs / 1000.0f;  // Convert to seconds
        
        // Cooldown
        if (towers[t].cooldown > 0) {
            towers[t].cooldown -= deltaTime;
            continue;
        }
        
        Vector2 towerPos = gridToScreen(towers[t].gridX, towers[t].gridY);
        
        // *** CALLING STUDENT'S FUNCTION: getTowerRange() from 01_Economy.cpp ***
        double towerRange = getTowerRange();
        
        // *** CALLING STUDENT'S FUNCTION: calculateGridDistance() from 03_Targeting.cpp ***
        // Find up to 3 enemies and their distances for selectTarget()
        double enemy1Dist = 9999, enemy2Dist = 9999, enemy3Dist = 9999;
        int enemy1Idx = -1, enemy2Idx = -1, enemy3Idx = -1;
        int foundCount = 0;
        
        for (int e = 0; e < MAX_ENEMIES && foundCount < 3; e++) {
            if (!enemies[e].alive) continue;
            
            // Use student's calculateGridDistance for pixel-to-grid conversion
            double gridDist = calculateGridDistance(
                towers[t].gridX, towers[t].gridY,
                (int)((enemies[e].x - GRID_OFFSET_X) / CELL_SIZE),
                (int)((enemies[e].y - GRID_OFFSET_Y) / CELL_SIZE)
            );
            
            // Fallback if student hasn't implemented it yet
            if (gridDist == 0.0) {
                double dx = towerPos.x - enemies[e].x;
                double dy = towerPos.y - enemies[e].y;
                gridDist = sqrt(dx*dx + dy*dy) / CELL_SIZE;
            }
            
            if (foundCount == 0) { enemy1Dist = gridDist; enemy1Idx = e; }
            else if (foundCount == 1) { enemy2Dist = gridDist; enemy2Idx = e; }
            else if (foundCount == 2) { enemy3Dist = gridDist; enemy3Idx = e; }
            foundCount++;
        }
        
        if (foundCount == 0) continue;  // No enemies
        
        // *** CALLING STUDENT'S FUNCTION: selectTarget() from 03_Targeting.cpp ***
        int targetNum = selectTarget(enemy1Dist, enemy2Dist, enemy3Dist);
        int targetIdx = (targetNum == 1) ? enemy1Idx : (targetNum == 2) ? enemy2Idx : enemy3Idx;
        if (targetIdx < 0) targetIdx = enemy1Idx;  // Fallback
        
        double dist = calculateDistance(towerPos.x, towerPos.y, enemies[targetIdx].x, enemies[targetIdx].y);
        
        // *** CALLING STUDENT'S FUNCTION: isEnemyInRange() from 03_Targeting.cpp ***
        bool inRange = isEnemyInRange(dist / CELL_SIZE, towerRange);
        
        if (inRange) {
            // *** CALLING STUDENT'S FUNCTIONS from 01_Economy.cpp and 03_Targeting.cpp ***
            int baseDamage = getTowerBaseDamage();
            int damage = calculateDamage(baseDamage, TOWER_LEVEL);
            
            // *** CALLING STUDENT'S FUNCTION: calculateBonusDamage() from 03_Targeting.cpp ***
            // Extra damage for weak enemies!
            int bonusDmg = calculateBonusDamage(baseDamage, enemies[targetIdx].health, TOWER_LEVEL);
            if (bonusDmg > damage) damage = bonusDmg;  // Use bonus if student implemented it
            
            // *** CALLING STUDENT'S FUNCTION: calculateTotalDamage() from 03_Targeting.cpp ***
            // Random critical hit (1 in 5 chance)
            int critMultiplier = (rand() % 5 == 0) ? 2 : 1;
            int totalDmg = calculateTotalDamage(baseDamage, TOWER_LEVEL, critMultiplier);
            if (totalDmg > 0 && critMultiplier == 2) {
                damage = totalDmg;  // Use crit damage if student implemented it
            }
            
            // *** CALLING STUDENT'S FUNCTION: predictEnemyPosition() from 03_Targeting.cpp ***
            // Lead the shot! Predict where enemy will be
            float projectileTime = dist / 500.0f;  // Time for projectile to reach
            double predictedX = predictEnemyPosition(enemies[targetIdx].x, enemies[targetIdx].speed, projectileTime);
            
            // Fire projectile (use predicted position if student implemented it, otherwise current)
            float targetX = (predictedX != enemies[targetIdx].x) ? (float)predictedX : enemies[targetIdx].x;
            fireProjectile(towerPos.x, towerPos.y, targetX, enemies[targetIdx].y);
            
            // Apply damage
            enemies[targetIdx].health -= damage;
            
            if (enemies[targetIdx].health <= 0) {
                enemies[targetIdx].alive = false;
                enemyCount--;
                score += 50;
                
                // *** Track kills for upgrade system ***
                towers[t].kills++;
                totalPlayerKills++;
                
                // *** CALLING STUDENT'S FUNCTION: getKillBonus() from 01_Economy.cpp ***
                playerMoney += getKillBonus();
                
                // Check for wave completion
                if (enemyCount <= 0) {
                    wave++;
                    // *** CALLING STUDENT'S FUNCTION: getWaveBonus() from 01_Economy.cpp ***
                    playerMoney += getWaveBonus(wave);
                    setMessage("Wave " + std::to_string(wave) + " incoming! Bonus: $" + std::to_string(getWaveBonus(wave)));
                }
            }
            
            towers[t].cooldown = cooldownTime;  // Use calculated fire rate!
            // Don't break - tower has done its job for this frame
        }
    }
    
    // Update projectiles
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!projectiles[i].active) continue;
        
        float dx = projectiles[i].targetX - projectiles[i].x;
        float dy = projectiles[i].targetY - projectiles[i].y;
        float dist = sqrt(dx * dx + dy * dy);
        
        if (dist < 10) {
            projectiles[i].active = false;
        } else {
            projectiles[i].x += (dx / dist) * projectiles[i].speed * deltaTime;
            projectiles[i].y += (dy / dist) * projectiles[i].speed * deltaTime;
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════
// INPUT HANDLING
// ═══════════════════════════════════════════════════════════════════════

void handleInput() {
    // Get mouse position
    int mouseX = GetMouseX();
    int mouseY = GetMouseY();
    
    // Update hovered cell
    screenToGrid(mouseX, mouseY, &hoveredCellX, &hoveredCellY);
    if (!isValidGridPos(hoveredCellX, hoveredCellY)) {
        hoveredCellX = -1;
        hoveredCellY = -1;
    }
    
    // ─────────────────────────────────────────────────────────────────
    // BUTTON CLICK HANDLING
    // ─────────────────────────────────────────────────────────────────
    Vector2 mousePos = GetMousePosition();
    
    // Check if clicked on RESTART button
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePos, btnRestart)) {
        initGame();
        return;  // Don't process other clicks
    }
    
    // Check if clicked on AUTO-UPGRADE button
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePos, btnAutoUpgrade) && !gameOver) {
        // Build upgrade tower list from current towers
        std::vector<UpgradeTower> upgradeList;
        for (int i = 0; i < MAX_TOWERS; i++) {
            if (!towers[i].active) continue;
            UpgradeTower ut;
            ut.level = towers[i].level;
            ut.damage = getTowerBaseDamage();
            ut.range = getTowerRange();
            ut.type = towers[i].type;
            ut.kills = towers[i].kills;
            upgradeList.push_back(ut);
        }
        
        int originalMoney = playerMoney;
        int remainingMoney = autoUpgradeAll(upgradeList, playerMoney);
        int spent = originalMoney - remainingMoney;
        
        // Apply upgrades back to actual towers
        int upgradeIdx = 0;
        int upgradesApplied = 0;
        for (int i = 0; i < MAX_TOWERS && upgradeIdx < (int)upgradeList.size(); i++) {
            if (!towers[i].active) continue;
            if (upgradeList[upgradeIdx].level > towers[i].level) {
                towers[i].level = upgradeList[upgradeIdx].level;
                upgradesApplied++;
            }
            upgradeIdx++;
        }
        
        playerMoney = remainingMoney;
        
        if (upgradesApplied > 0) {
            setMessage("AUTO-UPGRADE: " + std::to_string(upgradesApplied) + " towers upgraded! Spent $" + std::to_string(spent));
        } else if (spent == 0) {
            setMessage("AUTO-UPGRADE: No upgrades possible (solve exercises first!)");
        }
        return;
    }
    
    // Check if clicked on UPGRADE TOWER button (when tower selected)
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && showUpgradePanel && selectedTowerIndex >= 0 
        && CheckCollisionPointRec(mousePos, btnUpgradeTower) && !gameOver) {
        
        Tower& selTower = towers[selectedTowerIndex];
        UpgradeTower ut;
        ut.level = selTower.level;
        ut.damage = getTowerBaseDamage();
        ut.range = getTowerRange();
        ut.type = selTower.type;
        ut.kills = selTower.kills;
        
        // *** CALLING STUDENT'S FUNCTION: canUpgrade() from 05_Upgrades.cpp ***
        if (canUpgrade(ut, playerMoney)) {
            // *** CALLING STUDENT'S FUNCTION: getUpgradeCost() from 05_Upgrades.cpp ***
            int cost = getUpgradeCost(ut);
            playerMoney -= cost;
            selTower.level++;
            
            // *** CALLING STUDENT'S FUNCTIONS: getUpgradedDamage/Range() ***
            int newDmg = getUpgradedDamage(ut);
            double newRange = getUpgradedRange(ut);
            
            setMessage(selTower.type + " upgraded to Lv" + std::to_string(selTower.level) + "! DMG: " + std::to_string(newDmg));
        } else {
            // *** CALLING STUDENT'S FUNCTION: getUpgradeCost() from 05_Upgrades.cpp ***
            int cost = getUpgradeCost(ut);
            if (selTower.level >= 5) {
                setMessage("Tower already at MAX LEVEL!");
            } else {
                setMessage("Need $" + std::to_string(cost) + " to upgrade (solve exercises!)");
            }
        }
        return;
    }
    
    // Handle mouse click - place tower OR select tower
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !gameOver) {
        if (isValidGridPos(hoveredCellX, hoveredCellY)) {
            // First check if clicking on existing tower to SELECT it
            bool clickedOnTower = false;
            for (int i = 0; i < MAX_TOWERS; i++) {
                if (towers[i].active && towers[i].gridX == hoveredCellX && towers[i].gridY == hoveredCellY) {
                    selectedTowerIndex = i;
                    showUpgradePanel = true;
                    clickedOnTower = true;
                    setMessage("Tower selected! Click UPGRADE button or Right-Click");
                    break;
                }
            }
            
            if (!clickedOnTower) {
                // Deselect tower if clicking elsewhere
                selectedTowerIndex = -1;
                showUpgradePanel = false;
                
                // Check if position is valid (not base, not occupied)
                if (isBasePosition(hoveredCellX, hoveredCellY)) {
                    setMessage("Cannot build on the base!");
                }
                else if (hasTowerAt(hoveredCellX, hoveredCellY)) {
                    // This shouldn't happen since we check above, but safety
                    setMessage("Position occupied!");
                }
                else {
                    // *** CALLING STUDENT'S FUNCTIONS from 01_Economy.cpp and 02_Shop.cpp ***
                    int towerCost = getTowerCost();
                    bool canBuy = canAffordTower(playerMoney, towerCost);
                    bool cellEmpty = !hasTowerAt(hoveredCellX, hoveredCellY);
                    
                    // *** CALLING STUDENT'S FUNCTION: canBuildTowerAt() from 02_Shop.cpp ***
                    bool canBuild = canBuildTowerAt(hoveredCellX, hoveredCellY, cellEmpty);
                    
                    // *** CALLING STUDENT'S FUNCTION: shouldShowWarning() from 02_Shop.cpp ***
                    bool showWarn = shouldShowWarning(canBuy, towerCount < MAX_TOWERS);
                    
                    if (canBuy && towerCount < MAX_TOWERS) {
                        // Place tower
                        for (int i = 0; i < MAX_TOWERS; i++) {
                            if (!towers[i].active) {
                                towers[i].gridX = hoveredCellX;
                                towers[i].gridY = hoveredCellY;
                                towers[i].cooldown = 0;
                                towers[i].active = true;
                                towers[i].kills = 0;
                                towers[i].level = 1;
                                
                                // *** CALLING STUDENT'S FUNCTION: recommendTower() from 05_Upgrades.cpp ***
                                bool hasAirEnemies = false;  // Could be expanded for flying enemies!
                                std::string recType = recommendTower(playerMoney, hasAirEnemies, wave);
                                towers[i].type = (recType == "none") ? "laser" : recType;
                                
                                towerCount++;
                                playerMoney -= towerCost;
                                
                                // *** CALLING STUDENT'S FUNCTION: getTowerType() from 02_Shop.cpp ***
                                int tType = getTowerType(playerMoney);
                                std::string typeStr = (tType == 3) ? "SUPER" : (tType == 2) ? "ADVANCED" : (tType == 1) ? "BASIC" : "NONE";
                                setMessage("Tower (" + towers[i].type + ") deployed! Next: " + typeStr);
                                break;
                            }
                        }
                    } else if (showWarn) {
                        // Warning from student's function!
                        setMessage("WARNING: Not enough credits or no slots!");
                    } else {
                        setMessage("Not enough credits! Need $" + std::to_string(towerCost));
                    }
                }
            }
        } else {
            // Clicked outside grid - deselect tower
            selectedTowerIndex = -1;
            showUpgradePanel = false;
        }
    }
    
    // *** RIGHT-CLICK to upgrade existing tower ***
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && !gameOver) {
        if (isValidGridPos(hoveredCellX, hoveredCellY)) {
            // Find tower at this position
            for (int i = 0; i < MAX_TOWERS; i++) {
                if (towers[i].active && 
                    towers[i].gridX == hoveredCellX && 
                    towers[i].gridY == hoveredCellY) {
                    
                    // Create upgrade tower data
                    UpgradeTower ut;
                    ut.level = towers[i].level;
                    ut.damage = getTowerBaseDamage();
                    ut.range = getTowerRange();
                    ut.type = towers[i].type;
                    ut.kills = towers[i].kills;
                    
                    // *** CALLING STUDENT'S FUNCTION: canUpgrade() from 05_Upgrades.cpp ***
                    if (canUpgrade(ut, playerMoney)) {
                        // *** CALLING STUDENT'S FUNCTION: getUpgradeCost() from 05_Upgrades.cpp ***
                        int cost = getUpgradeCost(ut);
                        playerMoney -= cost;
                        towers[i].level++;
                        
                        // *** CALLING STUDENT'S FUNCTIONS: getUpgradedDamage/Range() ***
                        int newDmg = getUpgradedDamage(ut);
                        double newRange = getUpgradedRange(ut);
                        
                        setMessage(towers[i].type + " upgraded to Lv" + std::to_string(towers[i].level) + 
                                   "! DMG: " + std::to_string(newDmg));
                    } else {
                        // *** CALLING STUDENT'S FUNCTION: getUpgradeCost() from 05_Upgrades.cpp ***
                        int cost = getUpgradeCost(ut);
                        if (towers[i].level >= 5) {
                            setMessage("Tower already at MAX LEVEL!");
                        } else {
                            setMessage("Need $" + std::to_string(cost) + " to upgrade!");
                        }
                    }
                    break;
                }
            }
        }
    }
    
    // Keyboard shortcuts (also work via buttons now)
    if (IsKeyPressed(KEY_SPACE)) {
        gamePaused = !gamePaused;
    }
    if (IsKeyPressed(KEY_R)) {
        initGame();
    }
    if (IsKeyPressed(KEY_U) && !gameOver) {
        // Same logic as button - trigger auto-upgrade
        std::vector<UpgradeTower> upgradeList;
        for (int i = 0; i < MAX_TOWERS; i++) {
            if (!towers[i].active) continue;
            UpgradeTower ut;
            ut.level = towers[i].level;
            ut.damage = getTowerBaseDamage();
            ut.range = getTowerRange();
            ut.type = towers[i].type;
            ut.kills = towers[i].kills;
            upgradeList.push_back(ut);
        }
        int originalMoney = playerMoney;
        int remainingMoney = autoUpgradeAll(upgradeList, playerMoney);
        int spent = originalMoney - remainingMoney;
        int upgradeIdx = 0;
        int upgradesApplied = 0;
        for (int i = 0; i < MAX_TOWERS && upgradeIdx < (int)upgradeList.size(); i++) {
            if (!towers[i].active) continue;
            if (upgradeList[upgradeIdx].level > towers[i].level) {
                towers[i].level = upgradeList[upgradeIdx].level;
                upgradesApplied++;
            }
            upgradeIdx++;
        }
        playerMoney = remainingMoney;
        if (upgradesApplied > 0) {
            setMessage("AUTO-UPGRADE: " + std::to_string(upgradesApplied) + " towers upgraded!");
        } else {
            setMessage("AUTO-UPGRADE: Solve exercises first!");
        }
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        selectedTowerIndex = -1;
        showUpgradePanel = false;
    }
}

// ═══════════════════════════════════════════════════════════════════════
// RENDERING
// ═══════════════════════════════════════════════════════════════════════

void drawGame() {
    BeginDrawing();
    ClearBackground(COLOR_BACKGROUND);
    
    // ─────────────────────────────────────────────────────────────────
    // Draw Starfield Background
    // ─────────────────────────────────────────────────────────────────
    for (int i = 0; i < NUM_STARS; i++) {
        // Twinkle effect
        float twinkle = stars[i].brightness + 0.1f * sinf(GetTime() * 2.0f + i);
        if (twinkle > 1.0f) twinkle = 1.0f;
        Color starColor = Fade(WHITE, twinkle);
        DrawPixel((int)stars[i].x, (int)stars[i].y, starColor);
        // Some stars are bigger
        if (stars[i].brightness > 0.7f) {
            DrawPixel((int)stars[i].x + 1, (int)stars[i].y, Fade(WHITE, twinkle * 0.5f));
        }
    }
    
    // ─────────────────────────────────────────────────────────────────
    // Draw Header / HUD with sleek panels
    // ─────────────────────────────────────────────────────────────────
    // Pre-calculate some values for display
    int wavePointsSum = countdownSum(wave);
    
    // Title with glow
    DrawTextCustom("PLANETARY DEFENSE", 22, 12, 22, Fade(COLOR_TOWER, 0.3f));
    DrawTextCustom("PLANETARY DEFENSE", 20, 10, 22, COLOR_UI_TEXT);
    
    // Wave indicator (top right)
    DrawRectangleRounded((Rectangle){(float)(SCREEN_WIDTH - 120), 8, 110, 32}, 0.3f, 8, Fade(COLOR_BASE, 0.2f));
    DrawRectangleRoundedLines((Rectangle){(float)(SCREEN_WIDTH - 120), 8, 110, 32}, 0.3f, 8, 1, Fade(COLOR_BASE, 0.5f));
    DrawTextCustom(TextFormat("WAVE %d", wave), SCREEN_WIDTH - 100, 14, 22, COLOR_BASE);
    
    // Stats bar (horizontal strip below title)
    // Money (taller to show build type)
    DrawRectangleRounded((Rectangle){20, 42, 150, 45}, 0.2f, 8, Fade(BLACK, 0.6f));
    DrawRectangleRoundedLines((Rectangle){20, 42, 150, 45}, 0.2f, 8, 1, Fade(COLOR_MONEY, 0.4f));
    DrawTextCustom(TextFormat("$ %d", playerMoney), 32, 48, 24, COLOR_MONEY);
    
    // Health bar
    DrawRectangleRounded((Rectangle){180, 42, 170, 45}, 0.2f, 8, Fade(BLACK, 0.6f));
    DrawRectangleRoundedLines((Rectangle){180, 42, 170, 45}, 0.2f, 8, 1, Fade(COLOR_HEALTH, 0.4f));
    DrawTextCustom("HP", 190, 50, 18, COLOR_HEALTH);
    DrawRectangleRounded((Rectangle){220, 52, 100, 12}, 0.3f, 4, Fade(RED, 0.3f));
    DrawRectangleRounded((Rectangle){220, 52, (float)baseHealth, 12}, 0.3f, 4, COLOR_HEALTH);
    DrawTextCustom(TextFormat("%d%%", baseHealth), 328, 50, 18, COLOR_HEALTH);
    
    // Score (wider panel)
    DrawRectangleRounded((Rectangle){360, 42, 160, 45}, 0.2f, 8, Fade(BLACK, 0.6f));
    DrawRectangleRoundedLines((Rectangle){360, 42, 160, 45}, 0.2f, 8, 1, Fade(WHITE, 0.3f));
    DrawTextCustom(TextFormat("SCORE: %d", score), 375, 48, 20, COLOR_UI_TEXT);
    DrawTextCustom(TextFormat("Wave Pts: %d", wavePointsSum), 375, 70, 14, Fade(COLOR_UI_TEXT, 0.6f));
    
    // *** CALLING STUDENT'S FUNCTION: isBaseInDanger() from 02_Shop.cpp ***
    bool danger = isBaseInDanger(baseHealth, enemyCount);
    if (danger) {
        // Pulsing danger indicator
        float dangerPulse = 0.5f + 0.5f * sinf(GetTime() * 6);
        DrawRectangleRounded((Rectangle){530, 42, 90, 45}, 0.3f, 8, Fade(RED, 0.3f + 0.3f * dangerPulse));
        DrawRectangleRoundedLines((Rectangle){530, 42, 90, 45}, 0.3f, 8, 1, Fade(RED, 0.8f));
        DrawTextCustom("DANGER!", 542, 55, 18, Fade(WHITE, dangerPulse));
    }
    
    // ─────────────────────────────────────────────────────────────────
    // RADAR PANEL (right side, below wave indicator) - ENLARGED
    // ─────────────────────────────────────────────────────────────────
    std::vector<RadarEnemy> radarData = getRadarEnemies();
    
    // Panel: X=560, width=230 (fills right side)
    DrawRectangleRounded((Rectangle){560, 50, 230, 170}, 0.1f, 8, Fade(BLACK, 0.85f));
    DrawRectangleRoundedLines((Rectangle){560, 50, 230, 170}, 0.1f, 8, 2, Fade(YELLOW, 0.7f));
    DrawTextCustom("RADAR", 580, 58, 24, YELLOW);
    
    // *** CALLING STUDENT'S FUNCTION: countAllEnemies() from 04_Radar.cpp ***
    int totalEnemies = countAllEnemies(radarData);
    DrawTextCustom(TextFormat("Enemies: %d", totalEnemies), 575, 90, 20, COLOR_UI_TEXT);
    
    // *** CALLING STUDENT'S FUNCTION: countCloseEnemies() from 04_Radar.cpp ***
    int closeEnemies = countCloseEnemies(radarData, 5.0);
    Color closeColor = (closeEnemies > 2) ? RED : (closeEnemies > 0) ? ORANGE : GREEN;
    DrawTextCustom(TextFormat("Close: %d", closeEnemies), 575, 115, 20, closeColor);
    
    // *** CALLING STUDENT'S FUNCTION: getTotalEnemyHealth() from 04_Radar.cpp ***
    int totalHP = getTotalEnemyHealth(radarData);
    DrawTextCustom(TextFormat("Total HP: %d", totalHP), 575, 140, 20, COLOR_ENEMY);
    
    // *** CALLING STUDENT'S FUNCTION: findClosestEnemy() from 04_Radar.cpp ***
    double closestDist = findClosestEnemy(radarData);
    if (closestDist >= 0 && closestDist < 9999) {
        Color distColor = (closestDist < 3) ? RED : (closestDist < 6) ? ORANGE : GREEN;
        DrawTextCustom(TextFormat("Nearest: %.1f", closestDist), 575, 165, 20, distColor);
    } else {
        DrawTextCustom("Nearest: ---", 575, 165, 20, GREEN);
    }
    
    // *** CALLING STUDENT'S FUNCTION: findStrongestEnemy() from 04_Radar.cpp ***
    int strongestHP = findStrongestEnemy(radarData);
    if (strongestHP > 0) {
        Color hpColor = (strongestHP > 100) ? RED : (strongestHP > 50) ? ORANGE : COLOR_UI_TEXT;
        DrawTextCustom(TextFormat("Max HP: %d", strongestHP), 575, 190, 20, hpColor);
    } else {
        DrawTextCustom("Max HP: ---", 575, 190, 20, GREEN);
    }
    
    // *** CALLING STUDENT'S FUNCTION: getTowerType() from 02_Shop.cpp ***
    int affordableType = getTowerType(playerMoney);
    const char* typeNames[] = {"---", "BASIC", "ADV", "SUPER"};
    Color typeColors[] = {GRAY, COLOR_TOWER, ORANGE, PURPLE};
    // Tower type shown in top-left area (below money)
    if (affordableType >= 0 && affordableType <= 3 && affordableType > 0) {
        DrawTextCustom(TextFormat("Build: %s", typeNames[affordableType]), 32, 72, 14, typeColors[affordableType]);
    }
    
    // ─────────────────────────────────────────────────────────────────
    // STATUS PANEL (right side, below RADAR) - ENLARGED
    // Shows VIP & Weekend from 02_Shop.cpp
    // ─────────────────────────────────────────────────────────────────
    DrawRectangleRounded((Rectangle){560, 230, 230, 90}, 0.1f, 8, Fade(BLACK, 0.85f));
    DrawRectangleRoundedLines((Rectangle){560, 230, 230, 90}, 0.1f, 8, 2, Fade(SKYBLUE, 0.7f));
    DrawTextCustom("STATUS", 580, 238, 22, SKYBLUE);
    
    // *** CALLING STUDENT'S FUNCTION: isVIPPlayer() from 02_Shop.cpp ***
    bool isVIP = isVIPPlayer(score, wave, false);  // score, wavesCompleted, hasPremium
    if (isVIP) {
        float pulse = 0.7f + 0.3f * sinf(GetTime() * 3);
        DrawTextCustom("VIP", 575, 268, 22, Fade(GOLD, pulse));
    } else {
        DrawTextCustom("VIP: ---", 575, 268, 20, GRAY);
    }
    
    // *** CALLING STUDENT'S FUNCTION: calculateDiscount() from 02_Shop.cpp ***
    // Simulate weekend based on wave number (every 5 waves = "weekend sale")
    bool isWeekend = (wave % 5 == 0 && wave > 0);
    int discount = calculateDiscount(score, isWeekend);  // using score as "purchases"
    
    if (isWeekend) {
        float pulse = 0.6f + 0.4f * sinf(GetTime() * 4);
        DrawTextCustom("WKND", 680, 268, 20, Fade(MAGENTA, pulse));
    }
    
    if (discount > 0) {
        DrawTextCustom(TextFormat("Discount: -%d%%", discount), 575, 295, 20, GREEN);
    } else {
        DrawTextCustom("Discount: ---", 575, 295, 18, GRAY);
    }
    
    // ─────────────────────────────────────────────────────────────────
    // COMMANDER PANEL (right side, below STATUS) - ENLARGED
    // ─────────────────────────────────────────────────────────────────
    std::vector<UpgradeTower> upgradeData = getUpgradeTowers();
    UpgradePlayer playerData = getUpgradePlayer();
    
    DrawRectangleRounded((Rectangle){560, 330, 230, 175}, 0.1f, 8, Fade(BLACK, 0.85f));
    DrawRectangleRoundedLines((Rectangle){560, 330, 230, 175}, 0.1f, 8, 2, Fade(PURPLE, 0.7f));
    DrawTextCustom("COMMANDER", 580, 338, 22, PURPLE);
    
    // *** CALLING STUDENT'S FUNCTION: getPlayerRank() from 05_Upgrades.cpp ***
    std::string playerRank = getPlayerRank(playerData);
    Color rankColor = (playerRank == "Θρύλος") ? GOLD : 
                      (playerRank == "Ήρωας") ? PURPLE :
                      (playerRank == "Βετεράνος") ? ORANGE :
                      (playerRank == "Μαθητευόμενος") ? COLOR_TOWER : GRAY;
    DrawTextCustom(playerRank.c_str(), 575, 368, 24, rankColor);
    DrawTextCustom(TextFormat("Kills: %d", totalPlayerKills), 575, 398, 20, COLOR_UI_TEXT);
    
    // *** CALLING STUDENT'S FUNCTION: getTotalFleetDamage() from 05_Upgrades.cpp ***
    int fleetDamage = getTotalFleetDamage(upgradeData);
    DrawTextCustom(TextFormat("Fleet DMG: %d", fleetDamage), 575, 425, 18, COLOR_ENEMY);
    
    // *** CALLING STUDENT'S FUNCTION: findMVPTower() from 05_Upgrades.cpp ***
    int mvpIdx = findMVPTower(upgradeData);
    if (mvpIdx >= 0 && mvpIdx < (int)upgradeData.size()) {
        DrawTextCustom(TextFormat("MVP: %s", upgradeData[mvpIdx].type.c_str()), 575, 450, 18, GOLD);
    } else {
        DrawTextCustom("MVP: ---", 575, 450, 18, GRAY);
    }
    
    // *** CALLING STUDENT'S FUNCTION: recommendTower() from 05_Upgrades.cpp ***
    bool hasAir = false;
    std::string recommended = recommendTower(playerMoney, hasAir, wave);
    Color recColor = (recommended == "plasma") ? PURPLE : 
                     (recommended == "missile") ? ORANGE :
                     (recommended == "laser") ? COLOR_TOWER : GRAY;
    DrawTextCustom(TextFormat("Recommend: %s", recommended.c_str()), 575, 477, 18, recColor);

    // ─────────────────────────────────────────────────────────────────
    // THREAT WARNING (only shown when dangerous enemy exists)
    // ─────────────────────────────────────────────────────────────────
    std::vector<RadarEnemy> threatData = getRadarEnemies();
    
    // *** CALLING STUDENT'S FUNCTION: findFirstDangerousEnemy() from 04_Radar.cpp ***
    int dangerousIdx = findFirstDangerousEnemy(threatData, 75);
    if (dangerousIdx >= 0 && dangerousIdx < (int)threatData.size()) {
        float pulse = 0.6f + 0.4f * sinf(GetTime() * 5);
        DrawRectangleRounded((Rectangle){(float)(SCREEN_WIDTH / 2 - 80), 75, 160, 25}, 0.3f, 8, Fade(RED, 0.4f * pulse));
        DrawRectangleRoundedLines((Rectangle){(float)(SCREEN_WIDTH / 2 - 80), 75, 160, 25}, 0.3f, 8, 1, Fade(RED, 0.8f));
        DrawTextCustom(TextFormat("! THREAT: %d HP !", threatData[dangerousIdx].health), SCREEN_WIDTH / 2 - 65, 80, 16, Fade(WHITE, pulse));
    }
    
    // *** CALLING STUDENT'S FUNCTION: countFlyingEnemiesInRange() from 04_Radar.cpp ***
    int flyingInRange = countFlyingEnemiesInRange(threatData, 5.0);
    
    // *** CALLING STUDENT'S FUNCTION: countGridCells() from 04_Radar.cpp ***
    int totalCells = countGridCells(GRID_SIZE, GRID_SIZE);
    
    // *** CALLING STUDENT'S FUNCTION: simulateWaiting() from 04_Radar.cpp ***
    int wavesToMassAttack = simulateWaiting(wave, 20);
    
    // *** CALLING STUDENT'S FUNCTION: calculateDailyBonus() from 05_Upgrades.cpp ***
    int dayBonus = calculateDailyBonus(playerData, wave);
    
    // ─────────────────────────────────────────────────────────────────
    // Draw Grid with subtle glow
    // ─────────────────────────────────────────────────────────────────
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            int screenX = GRID_OFFSET_X + x * CELL_SIZE;
            int screenY = GRID_OFFSET_Y + y * CELL_SIZE;
            
            // Highlight hovered cell with glow
            if (x == hoveredCellX && y == hoveredCellY && !gameOver) {
                DrawRectangle(screenX + 2, screenY + 2, CELL_SIZE - 4, CELL_SIZE - 4, 
                             Fade(COLOR_TOWER, 0.15f + 0.05f * sinf(GetTime() * 4)));
                DrawRectangleLinesEx((Rectangle){(float)screenX, (float)screenY, CELL_SIZE, CELL_SIZE}, 2, 
                                     Fade(COLOR_TOWER, 0.6f));
            } else {
                // Subtle grid dots at corners
                DrawCircle(screenX, screenY, 2, Fade(COLOR_GRID, 0.5f));
            }
        }
    }
    // Grid border
    DrawRectangleLinesEx((Rectangle){GRID_OFFSET_X - 2, GRID_OFFSET_Y - 2, 
                         GRID_SIZE * CELL_SIZE + 4, GRID_SIZE * CELL_SIZE + 4}, 2, Fade(COLOR_GRID, 0.8f));
    
    // ─────────────────────────────────────────────────────────────────
    // Draw Base (right side) with pulsing glow
    // ─────────────────────────────────────────────────────────────────
    int baseScreenX = GRID_OFFSET_X + (GRID_SIZE - 1) * CELL_SIZE;
    int baseScreenY = GRID_OFFSET_Y + 4 * CELL_SIZE;
    
    // Pulsing glow behind base
    float pulse = 0.5f + 0.3f * sinf(GetTime() * 2);
    DrawRectangle(baseScreenX - 5, baseScreenY - 5, CELL_SIZE + 10, CELL_SIZE * 2 + 10, 
                  Fade(COLOR_BASE, pulse * 0.2f));
    
    // Base body with gradient effect (layered rectangles)
    DrawRectangle(baseScreenX + 3, baseScreenY + 3, CELL_SIZE - 6, CELL_SIZE * 2 - 6, COLOR_BASE);
    DrawRectangle(baseScreenX + 8, baseScreenY + 8, CELL_SIZE - 16, CELL_SIZE * 2 - 16, 
                  Fade(WHITE, 0.2f));
    DrawRectangleLinesEx((Rectangle){(float)baseScreenX + 3, (float)baseScreenY + 3, 
                         CELL_SIZE - 6, CELL_SIZE * 2 - 6}, 2, Fade(WHITE, 0.5f));
    
    // Base icon (shield shape)
    Vector2 shieldCenter = {(float)baseScreenX + CELL_SIZE/2, (float)baseScreenY + CELL_SIZE};
    DrawCircle((int)shieldCenter.x, (int)shieldCenter.y - 10, 12, Fade(WHITE, 0.3f));
    DrawText("HQ", baseScreenX + 14, baseScreenY + CELL_SIZE - 8, 18, WHITE);
    
    // ─────────────────────────────────────────────────────────────────
    // Draw Towers - Futuristic turrets with upgrade levels
    // ─────────────────────────────────────────────────────────────────
    for (int i = 0; i < MAX_TOWERS; i++) {
        if (!towers[i].active) continue;
        
        Vector2 pos = gridToScreen(towers[i].gridX, towers[i].gridY);
        double towerRange = getTowerRange();
        
        // *** CALLING STUDENT'S FUNCTION: getUpgradedRange() from 05_Upgrades.cpp ***
        // Apply range upgrade based on level
        UpgradeTower ut;
        ut.level = towers[i].level;
        ut.range = towerRange;
        ut.damage = getTowerBaseDamage();
        ut.type = towers[i].type;
        ut.kills = towers[i].kills;
        double upgradedRange = getUpgradedRange(ut);
        if (upgradedRange > towerRange) towerRange = upgradedRange;
        
        // Tower color based on type
        Color towerColor = COLOR_TOWER;
        Color towerDark = COLOR_TOWER_DARK;
        if (towers[i].type == "missile") { towerColor = ORANGE; towerDark = (Color){180, 100, 30, 255}; }
        else if (towers[i].type == "plasma") { towerColor = PURPLE; towerDark = (Color){100, 50, 150, 255}; }
        
        // Draw range indicator (subtle fill + line)
        DrawCircle((int)pos.x, (int)pos.y, (float)(towerRange * CELL_SIZE), 
                   Fade(towerColor, 0.05f));
        DrawCircleLines((int)pos.x, (int)pos.y, (float)(towerRange * CELL_SIZE), 
                        Fade(towerColor, 0.3f));
        
        // Tower base (octagon-like using circle)
        DrawCircle((int)pos.x, (int)pos.y + 5, 18, Fade(BLACK, 0.5f));  // Shadow
        DrawCircle((int)pos.x, (int)pos.y, 18, towerDark);
        DrawCircle((int)pos.x, (int)pos.y, 14, towerColor);
        DrawCircle((int)pos.x, (int)pos.y, 8, Fade(WHITE, 0.3f));  // Highlight
        
        // Rotating cannon (based on time + level for faster rotation)
        float angle = GetTime() * (0.5f + towers[i].level * 0.2f) + i;
        float cannonLen = 16 + towers[i].level;  // Longer cannon at higher levels
        Vector2 cannonEnd = {
            pos.x + cosf(angle) * cannonLen,
            pos.y + sinf(angle) * cannonLen
        };
        DrawLineEx(pos, cannonEnd, 6, towerDark);
        DrawLineEx(pos, cannonEnd, 3, towerColor);
        
        // Glowing core (brighter at higher levels)
        DrawCircle((int)pos.x, (int)pos.y, 4, Fade(WHITE, 0.8f + 0.2f * sinf(GetTime() * 5 + i)));
        
        // Show level indicator
        if (towers[i].level > 1) {
            DrawText(TextFormat("L%d", towers[i].level), (int)pos.x - 8, (int)pos.y - 30, 10, GOLD);
        }
        
        // *** CALLING STUDENT'S FUNCTION: getBonusDamageFromKills() from 05_Upgrades.cpp ***
        int bonusDmg = getBonusDamageFromKills(ut);
        if (bonusDmg > 0) {
            DrawText(TextFormat("+%d", bonusDmg), (int)pos.x + 12, (int)pos.y - 20, 9, COLOR_MONEY);
        }
    }
    
    // ─────────────────────────────────────────────────────────────────
    // Draw Enemies - Alien invaders
    // ─────────────────────────────────────────────────────────────────
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].alive) continue;
        
        float ex = enemies[i].x;
        float ey = enemies[i].y;
        
        // Pulsing glow behind enemy
        float glow = 0.3f + 0.1f * sinf(GetTime() * 4 + i);
        DrawCircle((int)ex, (int)ey, 22, Fade(COLOR_ENEMY, glow));
        
        // Enemy body with shading
        DrawCircle((int)ex + 2, (int)ey + 2, 16, Fade(BLACK, 0.4f));  // Shadow
        DrawCircle((int)ex, (int)ey, 16, COLOR_ENEMY_DARK);
        DrawCircle((int)ex, (int)ey, 12, COLOR_ENEMY);
        
        // Evil eye
        DrawCircle((int)ex, (int)ey, 6, Fade(BLACK, 0.5f));
        DrawCircle((int)ex + 2, (int)ey - 1, 3, Fade(WHITE, 0.9f));
        
        // Spikes (triangle shapes pointing outward)
        float spikeAngle = GetTime() * 2 + i;
        for (int s = 0; s < 3; s++) {
            float a = spikeAngle + s * 2.094f;  // 120 degrees apart
            Vector2 p1 = {ex + cosf(a) * 14, ey + sinf(a) * 14};
            Vector2 p2 = {ex + cosf(a - 0.3f) * 8, ey + sinf(a - 0.3f) * 8};
            Vector2 p3 = {ex + cosf(a + 0.3f) * 8, ey + sinf(a + 0.3f) * 8};
            DrawTriangle(p1, p2, p3, COLOR_ENEMY);
        }
        
        // Health bar with nicer style
        int maxHealth = 50 + wave * 10;
        float healthPercent = (float)enemies[i].health / maxHealth;
        DrawRectangleRounded((Rectangle){ex - 16, ey - 28, 32, 6}, 0.5f, 4, Fade(BLACK, 0.7f));
        if (healthPercent > 0) {
            Color hpColor = healthPercent > 0.5f ? COLOR_ENEMY_HEALTH : 
                           (healthPercent > 0.25f ? ORANGE : RED);
            DrawRectangleRounded((Rectangle){ex - 15, ey - 27, 30 * healthPercent, 4}, 0.5f, 4, hpColor);
        }
    }
    
    // ─────────────────────────────────────────────────────────────────
    // Draw Projectiles - Laser beams
    // ─────────────────────────────────────────────────────────────────
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!projectiles[i].active) continue;
        
        // Calculate direction for trail
        float dx = projectiles[i].targetX - projectiles[i].x;
        float dy = projectiles[i].targetY - projectiles[i].y;
        float dist = sqrtf(dx * dx + dy * dy);
        if (dist > 0) {
            dx /= dist; dy /= dist;
        }
        
        // Draw laser trail (line from slightly behind)
        Vector2 trailStart = {projectiles[i].x - dx * 15, projectiles[i].y - dy * 15};
        Vector2 trailEnd = {projectiles[i].x, projectiles[i].y};
        
        // Glow layer
        DrawLineEx(trailStart, trailEnd, 8, COLOR_PROJECTILE_GLOW);
        // Core beam
        DrawLineEx(trailStart, trailEnd, 3, COLOR_PROJECTILE);
        // Bright center
        DrawLineEx(trailStart, trailEnd, 1, WHITE);
        
        // Projectile head with glow
        DrawCircle((int)projectiles[i].x, (int)projectiles[i].y, 6, COLOR_PROJECTILE_GLOW);
        DrawCircle((int)projectiles[i].x, (int)projectiles[i].y, 3, COLOR_PROJECTILE);
        DrawCircle((int)projectiles[i].x, (int)projectiles[i].y, 1, WHITE);
    }
    
    // ─────────────────────────────────────────────────────────────────
    // Draw Status Message (positioned well above controls)
    // ─────────────────────────────────────────────────────────────────
    if (messageTimer > 0) {
        float alpha = messageTimer > 1 ? 1.0f : messageTimer;
        DrawRectangleRounded((Rectangle){15, (float)(SCREEN_HEIGHT - 68), (float)(SCREEN_WIDTH - 200), 28}, 
                             0.3f, 8, Fade(BLACK, 0.9f * alpha));
        DrawRectangleRoundedLines((Rectangle){15, (float)(SCREEN_HEIGHT - 68), (float)(SCREEN_WIDTH - 200), 28}, 
                                  0.3f, 8, 1, Fade(COLOR_TOWER, 0.7f * alpha));
        DrawTextCustom(statusMessage.c_str(), 25, SCREEN_HEIGHT - 62, 18, Fade(WHITE, alpha));
    }
    
    // ─────────────────────────────────────────────────────────────────
    // Draw BUTTON BAR (at very bottom)
    // ─────────────────────────────────────────────────────────────────
    int towerCost = getTowerCost();
    DrawRectangle(0, SCREEN_HEIGHT - 45, SCREEN_WIDTH, 45, Fade(BLACK, 0.85f));
    DrawRectangle(0, SCREEN_HEIGHT - 45, SCREEN_WIDTH, 2, Fade(COLOR_TOWER, 0.5f));
    
    Vector2 mousePos = GetMousePosition();
    
    // Button: RESTART [R]
    btnRestart = (Rectangle){20, (float)(SCREEN_HEIGHT - 50), 100, 42};
    bool hoverRestart = CheckCollisionPointRec(mousePos, btnRestart);
    DrawRectangleRounded(btnRestart, 0.3f, 8, hoverRestart ? Fade(RED, 0.8f) : Fade(RED, 0.4f));
    DrawRectangleRoundedLines(btnRestart, 0.3f, 8, 1, RED);
    DrawTextCustom("[R] RESTART", 26, SCREEN_HEIGHT - 38, 18, WHITE);
    
    // Button: AUTO-UPGRADE [U]
    btnAutoUpgrade = (Rectangle){130, (float)(SCREEN_HEIGHT - 50), 130, 42};
    bool hoverAutoUpgrade = CheckCollisionPointRec(mousePos, btnAutoUpgrade);
    // Check if student solved autoUpgradeAll (test with small budget)
    std::vector<UpgradeTower> testTowers;
    UpgradeTower testT; testT.level = 1; testT.damage = 10; testT.range = 2.0; testT.type = "laser"; testT.kills = 0;
    testTowers.push_back(testT);
    int testResult = autoUpgradeAll(testTowers, 1000);
    bool autoUpgradeSolved = (testResult < 1000);  // If money was spent, function works!
    
    Color btnAutoColor = autoUpgradeSolved ? PURPLE : DARKGRAY;
    DrawRectangleRounded(btnAutoUpgrade, 0.3f, 8, hoverAutoUpgrade ? Fade(btnAutoColor, 0.9f) : Fade(btnAutoColor, 0.5f));
    DrawRectangleRoundedLines(btnAutoUpgrade, 0.3f, 8, 1, btnAutoColor);
    DrawTextCustom("[U] AUTO-UPG", 140, SCREEN_HEIGHT - 38, 18, autoUpgradeSolved ? WHITE : GRAY);
    if (!autoUpgradeSolved) {
        DrawTextCustom("?", 245, SCREEN_HEIGHT - 42, 18, YELLOW);  // Hint: not solved
    }
    
    // ─────────────────────────────────────────────────────────────────
    // Draw UPGRADE PANEL (when tower selected)
    // ─────────────────────────────────────────────────────────────────
    if (showUpgradePanel && selectedTowerIndex >= 0 && selectedTowerIndex < MAX_TOWERS && towers[selectedTowerIndex].active) {
        Tower& selTower = towers[selectedTowerIndex];
        Vector2 towerPos = gridToScreen(selTower.gridX, selTower.gridY);
        
        // Panel position (near selected tower but not overlapping)
        float panelX = towerPos.x + 30;
        float panelY = towerPos.y - 60;
        if (panelX > SCREEN_WIDTH - 150) panelX = towerPos.x - 160;
        if (panelY < 50) panelY = towerPos.y + 30;
        
        // Draw panel background
        DrawRectangleRounded((Rectangle){panelX, panelY, 140, 95}, 0.15f, 8, Fade(BLACK, 0.9f));
        DrawRectangleRoundedLines((Rectangle){panelX, panelY, 140, 95}, 0.15f, 8, 2, COLOR_TOWER);
        
        // Tower info
        DrawTextCustom(TextFormat("%s Lv%d", selTower.type.c_str(), selTower.level), (int)panelX + 10, (int)panelY + 8, 14, COLOR_TOWER);
        DrawTextCustom(TextFormat("Kills: %d", selTower.kills), (int)panelX + 10, (int)panelY + 26, 12, COLOR_UI_TEXT);
        
        // Create upgrade tower data for checking
        UpgradeTower ut;
        ut.level = selTower.level;
        ut.damage = getTowerBaseDamage();
        ut.range = getTowerRange();
        ut.type = selTower.type;
        ut.kills = selTower.kills;
        
        int upgradeCost = getUpgradeCost(ut);
        bool canUpg = canUpgrade(ut, playerMoney);
        
        // Show upgrade cost
        DrawTextCustom(TextFormat("Cost: $%d", upgradeCost), (int)panelX + 10, (int)panelY + 42, 12, 
                       (playerMoney >= upgradeCost) ? COLOR_MONEY : RED);
        
        // UPGRADE button
        btnUpgradeTower = (Rectangle){panelX + 10, panelY + 58, 120, 28};
        bool hoverUpg = CheckCollisionPointRec(mousePos, btnUpgradeTower);
        
        // Button color based on whether student solved canUpgrade()
        Color upgBtnColor = canUpg ? GREEN : (selTower.level >= 5 ? ORANGE : DARKGRAY);
        DrawRectangleRounded(btnUpgradeTower, 0.3f, 8, hoverUpg ? Fade(upgBtnColor, 0.9f) : Fade(upgBtnColor, 0.6f));
        DrawRectangleRoundedLines(btnUpgradeTower, 0.3f, 8, 1, upgBtnColor);
        
        if (selTower.level >= 5) {
            DrawTextCustom("MAX LEVEL", (int)panelX + 30, (int)panelY + 64, 14, WHITE);
        } else if (canUpg) {
            DrawTextCustom("UPGRADE", (int)panelX + 35, (int)panelY + 64, 14, WHITE);
        } else {
            DrawTextCustom("UPGRADE ?", (int)panelX + 28, (int)panelY + 64, 14, GRAY);
            // Hint that exercise not solved
        }
        
        // Selection highlight on tower
        DrawCircleLines((int)towerPos.x, (int)towerPos.y, 25, Fade(COLOR_TOWER, 0.5f + 0.3f * sinf(GetTime() * 4)));
    }
    
    // Help text
    DrawTextCustom("[CLICK] Build/Select  [ESC] Deselect  [SPACE] Pause", 
                   275, SCREEN_HEIGHT - 38, 16, Fade(COLOR_UI_TEXT, 0.8f));
    
    // ─────────────────────────────────────────────────────────────────
    // Draw Pause / Game Over Overlay
    // ─────────────────────────────────────────────────────────────────
    if (gamePaused && !gameOver) {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.8f));
        
        // Pause panel
        DrawRectangleRounded((Rectangle){(float)(SCREEN_WIDTH/2 - 150), (float)(SCREEN_HEIGHT/2 - 60), 300, 120}, 
                             0.1f, 8, Fade(COLOR_GRID, 0.9f));
        DrawRectangleRoundedLines((Rectangle){(float)(SCREEN_WIDTH/2 - 150), (float)(SCREEN_HEIGHT/2 - 60), 300, 120}, 
                                  0.1f, 8, 2, COLOR_TOWER);
        
        DrawTextCustom("PAUSED", SCREEN_WIDTH/2 - 65, SCREEN_HEIGHT/2 - 40, 36, COLOR_TOWER);
        DrawTextCustom("Press SPACE to continue", SCREEN_WIDTH/2 - 105, SCREEN_HEIGHT/2 + 20, 18, COLOR_UI_TEXT);
    }
    
    if (gameOver) {
        // Animated background
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.85f));
        
        // Pulsing red vignette
        float pulse = 0.3f + 0.1f * sinf(GetTime() * 3);
        for (int r = 0; r < 5; r++) {
            DrawRectangleLinesEx((Rectangle){(float)(r * 30), (float)(r * 30), 
                                 (float)(SCREEN_WIDTH - r * 60), (float)(SCREEN_HEIGHT - r * 60)}, 
                                 2, Fade(COLOR_ENEMY, pulse - r * 0.05f));
        }
        
        // Game over panel
        DrawRectangleRounded((Rectangle){(float)(SCREEN_WIDTH/2 - 180), (float)(SCREEN_HEIGHT/2 - 100), 360, 200}, 
                             0.1f, 8, Fade(COLOR_ENEMY_DARK, 0.95f));
        DrawRectangleRoundedLines((Rectangle){(float)(SCREEN_WIDTH/2 - 180), (float)(SCREEN_HEIGHT/2 - 100), 360, 200}, 
                                  0.1f, 8, 2, COLOR_ENEMY);
        
        // Glowing text
        DrawTextCustom("GAME OVER", SCREEN_WIDTH/2 - 108, SCREEN_HEIGHT/2 - 77, 44, Fade(COLOR_ENEMY, 0.5f));
        DrawTextCustom("GAME OVER", SCREEN_WIDTH/2 - 110, SCREEN_HEIGHT/2 - 80, 44, WHITE);
        
        DrawTextCustom(TextFormat("Score: %d", score), SCREEN_WIDTH/2 - 55, SCREEN_HEIGHT/2 - 15, 26, COLOR_MONEY);
        DrawTextCustom(TextFormat("Waves Survived: %d", wave - 1), SCREEN_WIDTH/2 - 85, SCREEN_HEIGHT/2 + 25, 20, COLOR_UI_TEXT);
        
        // Pulsing restart hint
        float hintAlpha = 0.5f + 0.5f * sinf(GetTime() * 4);
        DrawTextCustom("Press R to restart", SCREEN_WIDTH/2 - 78, SCREEN_HEIGHT/2 + 70, 18, Fade(WHITE, hintAlpha));
    }
    
    EndDrawing();
}

// ═══════════════════════════════════════════════════════════════════════
// MAIN ENTRY POINT
// ═══════════════════════════════════════════════════════════════════════

int main() {
    // Initialize Raylib window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Planetary Defense Command");
    SetTargetFPS(60);
    
    // Load custom font (Segoe UI - clean, modern Windows font)
    // Try multiple font paths for compatibility
    gameFont = LoadFontEx("C:/Windows/Fonts/segoeui.ttf", 32, 0, 0);
    if (gameFont.texture.id == 0) {
        gameFont = LoadFontEx("C:/Windows/Fonts/arial.ttf", 32, 0, 0);
    }
    if (gameFont.texture.id == 0) {
        gameFont = GetFontDefault();  // Fallback to default
        fontLoaded = false;
    } else {
        fontLoaded = true;
        SetTextureFilter(gameFont.texture, TEXTURE_FILTER_BILINEAR);  // Smooth rendering
    }
    
    // *** CALLING STUDENT'S FUNCTION: SetupEconomy() ***
    // This function is implemented by the student in 01_Economy.cpp
    // SetupEconomy();  // Uncomment when student completes Challenge 1
    
    // Initialize game state
    initGame();
    
    // ─────────────────────────────────────────────────────────────────
    // MAIN GAME LOOP (Raylib style!)
    // ─────────────────────────────────────────────────────────────────
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        
        handleInput();
        updateGame(deltaTime);
        drawGame();
    }
    
    // Cleanup
    if (fontLoaded) UnloadFont(gameFont);
    CloseWindow();
    
    return 0;
}
