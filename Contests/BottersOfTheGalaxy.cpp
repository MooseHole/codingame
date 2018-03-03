#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class Location
{
	int x;
	int y;

public:
	Location()
	{
		x = 0;
		y = 0;
	}

	Location(int _x, int _y)
	{
		x = _x;
		y = _y;
	}

	Location& operator=(Location other)
	{
		x = other.x;
		y = other.y;
		return *this;
	}

	bool operator==(const Location& other) const
	{
		return x == other.x && y == other.y;
	}

	bool operator!=(const Location& other) const
	{
		return !(*this == other);
	}

	string print() const
	{
		return "(" + to_string(x) + "," + to_string(y) + ")";
	}

	friend ostream &operator<<(ostream &os, Location const &m)
	{
		return os << m.print();
	}
};

class Entity
{
protected:
	string entityType;
	Location location;

public:
	Entity()
	{
	}

	Entity(string _entityType, Location _location)
	{
		entityType = _entityType;
		location = _location;
	}

	string print() const
	{
		return "[" + entityType + " " + location.print() + "]";
	}

	friend ostream &operator<<(ostream &os, Entity const &m)
	{
		return os << m.print();
	}
};

class Bush : public Entity
{
	int radius;

public:
	Bush(string _entityType, Location _location, int _radius)
	{
		entityType = _entityType;
		location = _location;
		radius = _radius;
	}

	string print() const
	{
		return "[" + entityType + " " + location.print() + " {" + to_string(radius) + "}]";
	}

	friend ostream &operator<<(ostream &os, Bush const &m)
	{
		return os << m.print();
	}
};

class Unit : public Entity
{
	int unitId;
	bool myTeam;
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
	bool isVisible; // 0 if it isn't
	int itemsOwned; // useful from wood1

public:
	Unit(string _entityType, Location _location, int _unitId, bool _myTeam, int _attackRange, int _health, int _maxHealth, int _shield, int _attackDamage, int _movementSpeed, int _stunDuration, int _goldValue, int _countDown1, int _countDown2, int _countDown3, int _mana, int _maxMana, int _manaRegeneration, string _heroType, bool _isVisible, int _itemsOwned)
	{
		entityType = _entityType;
		location = _location;
		unitId = _unitId;
		myTeam = _myTeam;
		attackRange = _attackRange;
		health = _health;
		maxHealth = _maxHealth;
		shield = _shield;
		attackDamage = _attackDamage;
		movementSpeed = _movementSpeed;
		stunDuration = _stunDuration;
		goldValue = _goldValue;
		countDown1 = _countDown1;
		countDown2 = _countDown2;
		countDown3 = _countDown3;
		mana = _mana;
		maxMana = _maxMana;
		manaRegeneration = _manaRegeneration;
		heroType = _heroType;
		isVisible = _isVisible;
		itemsOwned = _itemsOwned;
	}

	string print() const
	{
		return "[" + entityType + " " + location.print() + " {" + to_string(unitId) + " " + heroType + "}]";
	}

	friend ostream &operator<<(ostream &os, Unit const &m)
	{
		return os << m.print();
	}
};

int main()
{
	int myTeam;
	cin >> myTeam; cin.ignore();
	int bushAndSpawnPointCount; // useful from wood1, represents the number of bushes and the number of places where neutral units can spawn
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
		int enemyTower = 0;
		int enemyHero = 0;
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

			Unit e = Unit(unitType, Location(x, y), unitId, team==myTeam, attackRange, health, maxHealth, shield, attackDamage, movementSpeed, stunDuration, goldValue, countDown1, countDown2, countDown3, mana, maxMana, manaRegeneration, heroType, isVisible, itemsOwned);
			cerr << e << endl;
			if (team != myTeam)
			{
				if (unitType == "TOWER")
				{
					enemyTower = unitId;
				}
				else if (unitType == "HERO")
				{
					enemyHero = unitId;
				}
			}
		}

		// Write an action using cout. DON'T FORGET THE "<< endl"
		// To debug: cerr << "Debug messages..." << endl;


		// If roundType has a negative value then you need to output a Hero name, such as "DEADPOOL" or "VALKYRIE".
		// Else you need to output roundType number of any valid action, such as "WAIT" or "ATTACK unitId"
		if (roundType < 0)
		{
			cout << "HULK" << endl;
		}
		else
		{
			cout << "ATTACK " << enemyHero << endl;
		}
	}
}
