/*
 * ═══════════════════════════════════════════════════════════════════════
 *  PLANETARY DEFENSE COMMAND - Game Engine Header
 * ═══════════════════════════════════════════════════════════════════════
 *  This file declares the functions that students implement.
 *  The engine calls these functions - students write the logic!
 * ═══════════════════════════════════════════════════════════════════════
 */

#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <string>
#include <vector>

using namespace std;

// ═══════════════════════════════════════════════════════════════════════
// FROM 01_Economy.cpp - Μεταβλητές & Τύποι
// ═══════════════════════════════════════════════════════════════════════
int getStartingMoney();
int getBaseHealth();
string getCommanderName();
int getTowerCost();
int getTowerBaseDamage();
double getTowerRange();
int getKillBonus();
int getWaveBonus(int waveNumber);

// ═══════════════════════════════════════════════════════════════════════
// FROM 02_Shop.cpp - If/Else & Λογικοί Τελεστές  
// ═══════════════════════════════════════════════════════════════════════
bool canAffordTower(int myMoney, int towerCost);
int getTowerType(int myMoney);
bool canBuildTowerAt(int gridX, int gridY, bool cellIsEmpty);
bool isBaseInDanger(int baseHealth, int enemiesNearBase);
bool isVIPPlayer(int score, int wavesCompleted, bool hasPremium);
int calculateDiscount(int totalPurchases, bool isWeekend);
bool shouldShowWarning(bool hasEnoughMoney, bool towerSlotAvailable);

// ═══════════════════════════════════════════════════════════════════════
// FROM 03_Targeting.cpp - Συναρτήσεις & Μαθηματικά
// ═══════════════════════════════════════════════════════════════════════
bool isEnemyInRange(double distance, double range);
int calculateDamage(int baseDamage, int level);
int calculateTotalDamage(int baseDamage, int level, int critMultiplier);
int calculateFireRate(int baseSpeed, int upgradeLevel);
double calculateGridDistance(int x1, int y1, int x2, int y2);
int selectTarget(double enemy1Dist, double enemy2Dist, double enemy3Dist);
int calculateBonusDamage(int baseDamage, int enemyHealth, int towerLevel);
double predictEnemyPosition(double currentX, double speed, double time);

// ═══════════════════════════════════════════════════════════════════════
// FROM 04_Radar.cpp - Βρόχοι (Loops)
// ═══════════════════════════════════════════════════════════════════════
struct RadarEnemy {
    double distance;
    int health;
    bool isFlying;
};

int countAllEnemies(vector<RadarEnemy> enemies);
int countCloseEnemies(vector<RadarEnemy> enemies, double maxDistance);
int getTotalEnemyHealth(vector<RadarEnemy> enemies);
double findClosestEnemy(vector<RadarEnemy> enemies);
int findStrongestEnemy(vector<RadarEnemy> enemies);
int countFlyingEnemiesInRange(vector<RadarEnemy> enemies, double range);
int simulateWaiting(int startWave, int targetEnemies);
int findFirstDangerousEnemy(vector<RadarEnemy> enemies, int dangerThreshold);
int countdownSum(int start);
int countGridCells(int width, int height);

// ═══════════════════════════════════════════════════════════════════════
// FROM 05_Upgrades.cpp - Συνδυασμός Όλων (Variables + If/Else + Loops + Functions)
// ═══════════════════════════════════════════════════════════════════════
struct UpgradeTower {
    int level;
    int damage;
    double range;
    string type;
    int kills;
};

struct UpgradePlayer {
    string name;
    int money;
    int totalKills;
    int wavesCompleted;
    bool isPremium;
};

int getUpgradeCost(UpgradeTower tower);
bool canUpgrade(UpgradeTower tower, int playerMoney);
int getUpgradedDamage(UpgradeTower tower);
double getUpgradedRange(UpgradeTower tower);
int getBonusDamageFromKills(UpgradeTower tower);
int getTotalFleetDamage(vector<UpgradeTower> towers);
int findMVPTower(vector<UpgradeTower> towers);
string getPlayerRank(UpgradePlayer player);
int calculateDailyBonus(UpgradePlayer player, int dayNumber);
string recommendTower(int money, bool hasAirEnemies, int waveNumber);
int autoUpgradeAll(vector<UpgradeTower>& towers, int budget);

#endif // GAME_ENGINE_H
