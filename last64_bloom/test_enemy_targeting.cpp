#include <iostream>
#include <cstdlib>
#include <ctime>

// Simplified version of our enemy targeting logic for testing
struct Player {
    int id;
    Player(int playerId) : id(playerId) {}
};

struct Enemy {
    Player* targetPlayer;
    
    Enemy() : targetPlayer(nullptr) {}
    
    static Enemy* spawn(Player* player1, Player* player2) {
        static Enemy enemyPool[10];
        static int poolIndex = 0;
        
        if (poolIndex >= 10) return nullptr;
        
        Enemy* enemy = &enemyPool[poolIndex++];
        enemy->targetPlayer = (rand() % 2 == 0) ? player1 : player2;
        
        return enemy;
    }
};

int main() {
    srand(time(nullptr));
    
    Player player1(1);
    Player player2(2);
    
    int player1TargetCount = 0;
    int player2TargetCount = 0;
    
    // Spawn 100 enemies and count how many target each player
    for (int i = 0; i < 100; ++i) {
        Enemy* enemy = Enemy::spawn(&player1, &player2);
        if (enemy) {
            if (enemy->targetPlayer == &player1) {
                player1TargetCount++;
            } else if (enemy->targetPlayer == &player2) {
                player2TargetCount++;
            }
        }
    }
    
    std::cout << "Player 1 targeted by " << player1TargetCount << " enemies" << std::endl;
    std::cout << "Player 2 targeted by " << player2TargetCount << " enemies" << std::endl;
    
    // Check if both players were targeted (they should be with high probability)
    if (player1TargetCount > 0 && player2TargetCount > 0) {
        std::cout << "SUCCESS: Both players were targeted by enemies" << std::endl;
    } else {
        std::cout << "FAILURE: Not all players were targeted" << std::endl;
    }
    
    return 0;
}