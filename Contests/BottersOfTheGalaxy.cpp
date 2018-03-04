#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

// Math constants
#define CIRCLE_DEGREES 360				// The number of degrees in a circle
#define RAD_TO_DEG 180.0f/3.14159f		// Used to convert radians to degrees
#define DEG_TO_RAD 3.14159f/180.0f		// Used to convert degrees to radians

class Location
{
public:
	int x;
	int y;

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

	Location operator+(const Location& other)
	{
		return Location(x + other.x, y + other.y);
	}

	Location operator-(const Location& other)
	{
		return Location(x - other.x, y - other.y);
	}

	Location operator*(const Location& other)
	{
		return Location(x * other.x, y * other.y);
	}

	Location operator*(const float scalar)
	{
		int precision = 100;
		int bigScalar = (scalar * precision);
		return Location((x * bigScalar) / precision, (y * bigScalar) / precision);
	}

	Location operator/(const float scalar)
	{
		if (scalar == 0)
		{
			return Location(0, 0);
		}

		int precision = 100;
		int bigScalar = (scalar * precision);
		return Location(((x * precision) / bigScalar), ((y * precision) / bigScalar));
	}

	Location& operator+=(const Location& other)
	{
		*this = *this + other;
		return *this;
	}

	// The Eucledean distance between this coordinate and the input
	int distance(Location to)
	{
		return sqrt(squaredDistance(to));
	}

	// The squared Eucledean distance between this coordinate and the input
	// Avoids using sqrt
	int squaredDistance(Location to)
	{
		return pow(to.x - x, 2) + pow(to.y - y, 2);
	}

	// Returns the difference angle in degrees
	int angleTo(Location other)
	{
		Location diff = *this - other;
		return diff.angle();
	}

	// Returns the angle of a vector in degrees
	int angle()
	{
		return ((int)((atan2(y, x)) * RAD_TO_DEG) + CIRCLE_DEGREES) % CIRCLE_DEGREES;
	}

	string locationOutput() const
	{
		return to_string(x) + " " + to_string(y);
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
public:
	string entityType;
	Location location;

	Entity()
	{
	}

	Entity(string _entityType, Location _location)
	{
		entityType = _entityType;
		location = _location;
	}

	string locationOutput() const
	{
		return to_string(location.x) + " " + to_string(location.y);
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
public:
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

	Unit()
	{

	}

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

	Unit& operator=(Unit other)
	{
		entityType = other.entityType;
		location = other.location;
		unitId = other.unitId;
		myTeam = other.myTeam;
		attackRange = other.attackRange;
		health = other.health;
		maxHealth = other.maxHealth;
		shield = other.shield;
		attackDamage = other.attackDamage;
		movementSpeed = other.movementSpeed;
		stunDuration = other.stunDuration;
		goldValue = other.goldValue;
		countDown1 = other.countDown1;
		countDown2 = other.countDown2;
		countDown3 = other.countDown3;
		mana = other.mana;
		maxMana = other.maxMana;
		manaRegeneration = other.manaRegeneration;
		heroType = other.heroType;
		isVisible = other.isVisible;
		itemsOwned = other.itemsOwned;
		return *this;
	}

	int healthPercent() const
	{
		return (health * 100) / maxHealth;
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

Unit enemyHero;
Unit myHero;
Unit enemyTower;
Unit myTower;
Unit closestEnemy;

string attackPlace(Unit target)
{
	int angle = target.location.angleTo(myTower.location) + CIRCLE_DEGREES/2;
	return to_string(myHero.attackRange * cos(angle * DEG_TO_RAD) + target.location.x) + " " + to_string(myHero.attackRange * sin(angle * DEG_TO_RAD) + target.location.y);
}

void attackHero()
{
	cout << "MOVE_ATTACK " << attackPlace(enemyHero) << " " << enemyHero.unitId << endl;
}

void retreat()
{
	int attackId = enemyHero.unitId;
	if (enemyHero.location.distance(myHero.location) <= myHero.attackRange)
	{
		attackId = closestEnemy.unitId;
	}
	cout << "MOVE_ATTACK " << myTower.locationOutput() << " " << enemyHero.unitId << endl;
}

void chooseHero()
{
	cout << "WAIT" << endl;
}

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
	while (1)
	{
		int gold;
		cin >> gold; cin.ignore();
		int enemyGold;
		cin >> enemyGold; cin.ignore();
		int roundType; // a positive value will show the number of heroes that await a command
		cin >> roundType; cin.ignore();
		int entityCount;
		cin >> entityCount; cin.ignore();

		// Reset enemy distance
		closestEnemy.location.x = -1000000;
		closestEnemy.location.y = -1000000;

		for (int i = 0; i < entityCount; i++)
		{
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
					enemyTower = e;
				}
				else if (unitType == "HERO")
				{
					enemyHero = e;
					if (closestEnemy.location.squaredDistance(myHero.location) > e.location.squaredDistance(myHero.location))
					{
						closestEnemy = e;
					}
				}
				else if (unitType == "UNIT")
				{
					if (closestEnemy.location.squaredDistance(myHero.location) > e.location.squaredDistance(myHero.location))
					{
						closestEnemy = e;
					}
				}
			}
			else
			{
				if (unitType == "TOWER")
				{
					myTower = e;
				}
				else if (unitType == "HERO")
				{
					myHero = e;
				}
			}
		}

		if (roundType < 0)
		{
			chooseHero();
		}
		else
		{
			if (myHero.healthPercent() < enemyHero.healthPercent())
			{
				retreat();
			}
			else
			{
				attackHero();
			}
		}
	}
}
