#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * Made with love by AntiSquid, Illedan and Wildum.
 * You can help children learn to code while you participate by donating to CoderDojo.
 **/
int main()
{
    int myTeam;
    cin >> myTeam; cin.ignore();
    int bushAndSpawnPointCount; // usefrul from wood1, represents the number of bushes and the number of places where neutral units can spawn
    cin >> bushAndSpawnPointCount; cin.ignore();
    for (int i = 0; i < bushAndSpawnPointCount; i++) {
        string entityType; // BUSH, from wood1 it can also be SPAWN
        int x;
        int y;
        int radius;
        cin >> entityType >> x >> y >> radius; cin.ignore();
    }
    int itemCount; // useful from wood2
    cin >> itemCount; cin.ignore();
    for (int i = 0; i < itemCount; i++) {
        string itemName; // contains keywords such as BRONZE, SILVER and BLADE, BOOTS connected by "_" to help you sort easier
        int itemCost; // BRONZE items have lowest cost, the most expensive items are LEGENDARY
        int damage; // keyword BLADE is present if the most important item stat is damage
        int health;
        int maxHealth;
        int mana;
        int maxMana;
        int moveSpeed; // keyword BOOTS is present if the most important item stat is moveSpeed
        int manaRegeneration;
        int isPotion; // 0 if it's not instantly consumed
        cin >> itemName >> itemCost >> damage >> health >> maxHealth >> mana >> maxMana >> moveSpeed >> manaRegeneration >> isPotion; cin.ignore();
    }

    // game loop
    while (1) {
        int gold;
        cin >> gold; cin.ignore();
        int enemyGold;
        cin >> enemyGold; cin.ignore();
        int roundType; // a positive value will show the number of heroes that await a command
        cin >> roundType; cin.ignore();
        int entityCount;
        cin >> entityCount; cin.ignore();
        for (int i = 0; i < entityCount; i++) {
            int unitId;
            int team;
            string unitType; // UNIT, HERO, TOWER, can also be GROOT from wood1
            int x;
            int y;
            int attackRange;
            int health;
            int maxHealth;
            int shield; // useful in bronze
            int attackDamage;
            int movementSpeed;
            int stunDuration; // useful in bronze
            int goldValue;
            int countDown1; // all countDown and mana variables are useful starting in bronze
            int countDown2;
            int countDown3;
            int mana;
            int maxMana;
            int manaRegeneration;
            string heroType; // DEADPOOL, VALKYRIE, DOCTOR_STRANGE, HULK, IRONMAN
            int isVisible; // 0 if it isn't
            int itemsOwned; // useful from wood1
            cin >> unitId >> team >> unitType >> x >> y >> attackRange >> health >> maxHealth >> shield >> attackDamage >> movementSpeed >> stunDuration >> goldValue >> countDown1 >> countDown2 >> countDown3 >> mana >> maxMana >> manaRegeneration >> heroType >> isVisible >> itemsOwned; cin.ignore();
        }

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;


        // If roundType has a negative value then you need to output a Hero name, such as "DEADPOOL" or "VALKYRIE".
        // Else you need to output roundType number of any valid action, such as "WAIT" or "ATTACK unitId"
        cout << "WAIT" << endl;
    }
}
