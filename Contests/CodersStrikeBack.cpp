/*
Best builds	Max rank after 100% eval
Build 19:	112
Build 20:	229
Build 21:   167-177
Build 22:   136 (exact resubmit of Build 19)
Build 23:   243
Build 24:	170
Build 25:   170 at 10% (exact resubmit of Build 18)
Build 26:   754 at 4%
Build 27:   current

This made me orbit vs Default (Build 19):
seed=949832639
map=7306 6659 5486 2848 10338 3349 11234 5413
*/

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>
#include <ctime>

// Math constants
#define CIRCLE_DEGREES 360				// The number of degrees in a circle
#define RAD_TO_DEG 180.0f/3.14159f		// Used to convert radians to degrees
#define DEG_TO_RAD 3.14159f/180.0f		// Used to convert degrees to radians

// Given in the problem
#define PODS_PER_TEAM 2					// The number of pods on each team
#define CHECKPOINT_RADIUS 600			// The radius of a checkpoint
#define MAX_THRUST 200					// The maximum thrust
#define ANGLE_PER_STEP 18				// The maximum number of degrees that can be changed each turn
#define SHIELD_REGEN 3					// The amount of turns the shield kills the thrust
#define FRICTION 0.85					// The velocity is multiplied by this each turn

// Goal controls
#define CHECKPOINT_TARGET_RADIUS 400	// This is used for checkpoint targetting
#define PODS_TOUCH_SQUARED 640000		// The square of the distance that makes pods touch (800 units)
#define FAST_SPEED 600					// The minimum speed that is fast
#define CHECKPOINT_RECOVERY_TURNS 30	// If a racer has not hit a checkpoint in this many turns, they will turn on the shield in an attempt to recover
#define ANTILOCK_AIRBRAKE 5				// Helps prevent orbiting the checkpoint
#define CIRCLING_DISTANCE 3000			// The distance in which orbiting is detected

// Simulation controls
#define SIMULATE_STEPS 3				// The number of turns for the simulation look into the future
#define SIMULATION_DEADLINE 0.095		// The amount of time (in seconds) the simulation has before it must abort

// Simulation scoring
#define HIT_TARGET_SCORE 20000			// Score when the target is hit
#define DISTANCE_SCORE -100				// Score multiplied by distance from the target
#define NEXT_DISTANCE_SCORE -1 * 0		// Score multiplied by the distance from the next target
#define ANGLE_SCORE -2					// Score multiplied by the angle deviation from the target
#define COLLIDE_SCORE -50				// Score when my teammates collide
#define NOT_EVADING_SCORE -10000		// Score when a racer doesn't try to evade an oncoming enemy

using namespace std;

// A character to ride in a pod
enum character
{
	SEBULBA,	// A grumpy dug
	ANAKIN		// The chosen one
};

// A role for the pods.
enum role
{
	INITIAL,	// The first role
	ENEMY,		// Not my pod
	DEFAULT,	// This should never happen
	RACER,		// This role tries to complete the race
	BRAWLER,	// This role tries to bash the nearest enemy
	HUNTER		// This role tries to bash the winningest enemy
};

// A coordinate holds position.  It can also be used for vector calculations
class Coordinate
{
    public:
    int x;
    int y;
    
    Coordinate() : x(-1), y(-1)
    {}
    
    Coordinate(int x, int y) : x(x), y(y)
    {}
    
    Coordinate& operator=(const Coordinate& other)
    {
        x = other.x;
        y = other.y;
        return *this;
    }
    
    bool operator==(const Coordinate& other)
    {
        return x == other.x && y == other.y;
    }
	
    // The Eucledean distance between this coordinate and the input
    int distance(Coordinate to)
    {
        return sqrt(squaredDistance(to));
    }
	
    // The squared Eucledean distance between this coordinate and the input
	// Avoids using sqrt
    int squaredDistance(Coordinate to)
    {
        return pow(to.x - x, 2) + pow(to.y - y, 2);
    }
	
	bool operator< (const Coordinate& other) const
	{
		return x < other.x || y < other.y;
	}

	Coordinate operator+(const Coordinate& other)
	{
		return Coordinate(x + other.x, y + other.y);
	}

	Coordinate operator-(const Coordinate& other)
	{
		return Coordinate(x - other.x, y - other.y);
	}

	Coordinate operator*(const Coordinate& other)
	{
		return Coordinate(x * other.x, y * other.y);
	}

	Coordinate operator*(const float scalar)
	{
		int precision = 100;
		int bigScalar = (scalar * precision);
		return Coordinate((x * bigScalar) / precision, (y * bigScalar) / precision);
	}

	Coordinate operator/(const float scalar)
	{
		if (scalar == 0)
		{
			return Coordinate(0,0);
		}

		int precision = 100;
		int bigScalar = (scalar * precision);
		return Coordinate(((x * precision) / bigScalar), ((y * precision) / bigScalar));
	}

	Coordinate& operator+=(const Coordinate& other)
	{
		*this = *this + other;
		return *this;
	}

	string outputXY()
	{
		return to_string(x) + " " + to_string(y);
	}

	// Input an angle in degrees.  Get a point in front of this coordinate at that angle
	Coordinate pointFromAngle(float angle)
	{
		float radianAngle = angle * DEG_TO_RAD;
		return Coordinate(1000 * cos(radianAngle) + x, 1000 * sin(radianAngle) + y);
	}
	
	//// VECTOR JUNK ////
	
	// This gives the magnitude of the vector
	int magnitude()
	{
        return sqrt(pow(x, 2) + pow(y, 2));		
	}

	// Returns the difference angle
	int angleTo(Coordinate other)
	{
		Coordinate diff = *this - other;
		return diff.angle();
	}
	
	// Returns the angle of a vector in degrees
	int angle()
	{
		return ((int)((atan2(y, x)) * RAD_TO_DEG) + CIRCLE_DEGREES) % CIRCLE_DEGREES;
	}
	
	int dotProduct(Coordinate other)
	{
		return (x * other.x) + (y * other.y);
	}

	friend ostream &operator<<(ostream &os, Coordinate const &m)
	{
		return os << "(" << m.x << "," << m.y << ")";
	}
};

// A checkpoint is a place the pods need to go to in order to complete the race
class Checkpoint
{
	public:
	Coordinate location;
	int name;

	Checkpoint(){}
	
	Checkpoint(Coordinate location) : location(location)
	{
	}
	
	Checkpoint(int x, int y)
	{
		location.x = x;
		location.y = y;
	}
};

// A quote is said for a certain amount of turns
class Quote
{
	private:
	Quote();

	public:
	int character;
	int turns;
	string quote;
	string lookup;

	Quote(string lookup, int character, int turns, string quote) : lookup(lookup), character(character), turns(turns), quote(quote)
	{
	}
	
	friend ostream &operator<<(ostream &os, Quote const &m)
	{
		return os << " " << m.quote;
	}
};

vector<Checkpoint*> checkpoints;
vector<Quote*> quotes;

// A pod is the thing that drives around the track
class Pod
{
	public:
	int name;
	int angle;
	int thrust;
	Coordinate steeringTarget;
	Coordinate location;
	Coordinate velocity;
	Coordinate target;
	Pod* targetEnemy;
	bool activateShield;
	int nextCheckpoint;	
	role currentRole;
	bool ramming;
	int checkpointsRemaining;
	int turnsSinceHitCheckpoint;

	private:	
    Pod* teammate;
    vector<Pod*> enemies;
    int numCheckpoints;
	bool mine;
	int previousCheckpoint;
	int lapsRemaining;
	character myCharacter;
	string quote;
	int quoteTurns;
	Pod* closestEnemy;
	Pod* winningestEnemy;
	int sharpTurnCount;

	// Constructor
	Pod() : previousCheckpoint(0), nextCheckpoint(0), turnsSinceHitCheckpoint(0), sharpTurnCount(0)
	{
	}
	
	// Update the lap and checkpoint counters
	void checkLaps()
	{
		if (previousCheckpoint != nextCheckpoint)
		{
			checkpointsRemaining--;
			if (previousCheckpoint == 0)
			{
				lapsRemaining--;
			}
			previousCheckpoint = nextCheckpoint;
			turnsSinceHitCheckpoint = 0;
			circling(false);
		}
		else
		{
			turnsSinceHitCheckpoint++;
		}
	}
	
	// Update my role based on current race status.
	// There should always be at least one racer.
	void updateRole()
	{
		currentRole = role::DEFAULT;

		bool enemyInLead = false;
		int enemyLeastLaps = std::numeric_limits<int>::max();
   	    for (vector<Pod*>::iterator it = enemies.begin(); it != enemies.end(); ++it)
   	    {
			int chk = (*it)->checkpointsRemaining;
			int lap = (*it)->lapsRemaining;
			if (lap < enemyLeastLaps)
			{
				enemyLeastLaps = lap;
			}

			if (chk < min(checkpointsRemaining, teammate->checkpointsRemaining))
			{
				enemyInLead = true;
				break;
			}
		}
		
		bool behindTeammate = checkpointsRemaining > teammate->checkpointsRemaining;
		bool withTeammate = checkpointsRemaining <= teammate->checkpointsRemaining + 1;
		
		if ((enemyInLead || enemyLeastLaps == 0) && behindTeammate)
		{
			currentRole = role::HUNTER;
		}
		else if (withTeammate)
		{
			currentRole = role::RACER;
		}
		else
		{
			currentRole = role::BRAWLER;
		}
	}

	// Figure out enemy statuses
	void findEnemies()
	{
		int minCheckpoints = std::numeric_limits<int>::max();
		int enemyDistance = std::numeric_limits<int>::max();
		closestEnemy = NULL;
		winningestEnemy = NULL;
		for (vector<Pod*>::iterator it = enemies.begin(); it != enemies.end(); ++it)
		{
			int dist = (*it)->location.squaredDistance(location);
			int chk = (*it)->checkpointsRemaining;
			if (dist < enemyDistance)
			{
				enemyDistance = dist;
				closestEnemy = (*it);
			}

			if (chk < minCheckpoints)
			{
				minCheckpoints = chk;
				winningestEnemy = (*it);
			}
			else if ((chk == minCheckpoints) && (closestEnemy == (*it)))
			{
				winningestEnemy = (*it);
			}
		}
	}
	
	// Find my next target
	void acquireTarget()
	{
		ramming = false;
		targetEnemy = NULL;

		switch (currentRole)
		{
			case role::DEFAULT:
			case role::RACER:
				target = checkpoints[nextCheckpoint]->location;
				break;
			case role::HUNTER:
				findEnemies();
				targetEnemy = winningestEnemy;
				target = targetEnemy->location;
				ramming = true;
				break;
			case role::BRAWLER:
			default:
				findEnemies();
				targetEnemy = closestEnemy;
				target = targetEnemy->location;
				ramming = true;
				break;
		}
	}

	public:
	// Initializer
	Pod(int _name, int _lapsRemaining, character _myCharacter = character::SEBULBA, bool _mine = false)
	{
		Pod();
		lapsRemaining = _lapsRemaining;
		mine = _mine;
	    numCheckpoints = checkpoints.size();
		checkpointsRemaining = lapsRemaining * numCheckpoints;
		myCharacter = _myCharacter;
		name = _name;
		quote = "";
		if (mine)
		{
			currentRole = role::INITIAL;
			sayQuote("Start");
		}
		else
		{
			currentRole = role::ENEMY;
		}
	}

	// Detects circling around the target.
	// Returns 2 if shield should be applied
	// Returns 1 if slow down required
	int circling(bool sharpTurn)
	{
		if (sharpTurn)
		{
			sharpTurnCount++;
		}
		else
		{
			sharpTurnCount = 0;
		}
		
		int div = sharpTurnCount / ANTILOCK_AIRBRAKE;
		int mod = sharpTurnCount % ANTILOCK_AIRBRAKE;
		if (div - mod > 0)
		{
			if (div >= 3)
			{
    			sharpTurnCount = 0;
				return 2;
			}
			return 1;
		}
		
		return 0;
	}
	
	// Update this pod with information that was given this turn
	void update(int _x, int _y, int _vx, int _vy, int _angle, int _nextCheckpoint)
	{
		location.x = _x;
		location.y = _y;
		velocity.x = _vx;
		velocity.y = _vy;
		angle = _angle;
		nextCheckpoint = _nextCheckpoint;

		checkLaps();
	}

	// Get pointers to the other pods
	void setOtherPods(Pod* mate, vector<Pod*> them)
	{
		teammate = mate;
		enemies = them;
	}
	
	// Say something for an amount of turns
	void sayQuote(string lookup)
	{
		for (vector<Quote*>::iterator it = quotes.begin(); it != quotes.end(); ++it)
		{
			if (((*it)->character == myCharacter) && ((*it)->lookup == lookup))
			{
				quote = (*it)->quote;
				quoteTurns = (*it)->turns;
				break;
			}
		}
	}

	// Call methods required for the simulation to work
	void simulationPrerequisites()
	{
		updateRole();
		acquireTarget();
	}

	// Output for the program
	string command()
	{
	    string output = "";
		
		// Give target point
		output += to_string(steeringTarget.x);
		output += " ";
		output += to_string(steeringTarget.y);
		output += " ";

		// Activate thrust or shield
		if (activateShield)
		{
			output += "SHIELD";
		}
		else
		{
			output += to_string(thrust);
		}
		
		// Display quotes
		if (quote != "")
		{
			output += " " + quote;
			quoteTurns--;
			if (quoteTurns < 0)
			{
				quote = "";
			}
		}

        return output;
	}
	
	// Pretty output for couts
	friend ostream &operator<<(ostream &os, Pod const &m)
	{
	    os << (m.mine ? "M" : "T") << m.currentRole << m.name << m.location << m.velocity << m.angle << "° >" << m.nextCheckpoint << " #" << m.lapsRemaining << " !" << m.checkpointsRemaining;

		return os;
	}
};

void fillQuotes()
{
	quotes.push_back(new Quote("Start", character::SEBULBA, 10, ",,_____o^ Sebulba       [~__;__<\\"));
	quotes.push_back(new Quote("Start", character::ANAKIN, 10, "(|O:O|)  Annie             `   \\=/   ´"));
	quotes.push_back(new Quote("Hunter", character::SEBULBA, 10, "Bazda wahota, shag. Dobiellia Nok.")); // You won't walk away from this one, slave scum.
	quotes.push_back(new Quote("Hunter", character::ANAKIN, 10, "I know we're in trouble, just hang on!"));
	quotes.push_back(new Quote("Ramming", character::SEBULBA, 5, "Watchout slimo! Aahhh... chuba!")); // Look out slimeball.  Ah you!
	quotes.push_back(new Quote("Ramming", character::ANAKIN, 5, "Don't count on it, slimeball!"));
	quotes.push_back(new Quote("GuardCheckpoint", character::SEBULBA, 10, "Una notu wo shag, me wompity du pom pom.")); // If you weren't a slave, I'd squash you right now.
	quotes.push_back(new Quote("GuardCheckpoint", character::ANAKIN, 10, "Qui-Gon told me to stay in this cockpit, and that's what I'm going to do!"));
	quotes.push_back(new Quote("GoingFast", character::SEBULBA, 1, "Yoka to bantha poodoo.")); // You're bantha shit
	quotes.push_back(new Quote("GoingFast", character::ANAKIN, 1, "Now this is pod racing!"));
	quotes.push_back(new Quote("SharpTurn", character::SEBULBA, 3, "Poodoo!")); // Shit!
	quotes.push_back(new Quote("SharpTurn", character::ANAKIN, 3, "Woah"));
	quotes.push_back(new Quote("Impact", character::SEBULBA, 1, "Chuba! Ni chuba na?")); // You! Is this yours?
	quotes.push_back(new Quote("Impact", character::ANAKIN, 1, "What are midi-chlorians?"));
	quotes.push_back(new Quote("End", character::SEBULBA, 10, "Neek me chawa, wermo, mo killee ma ka nunkee.")); // Next time we race boy, it will be the end of you.
	quotes.push_back(new Quote("End", character::ANAKIN, 10, "I've built a racer. It's the fastest ever!"));
}

bool counterClockWise(Coordinate A, Coordinate B, Coordinate C)
{
	return (C.y - A.y) * (B.x - A.x) > (B.y - A.y) * (C.x - A.x);
}

bool intersect(Coordinate A1, Coordinate A2, Coordinate B1, Coordinate B2)
{
	return counterClockWise(A1, B1, B2) != counterClockWise(A2, B1, B2)
		&& counterClockWise(A1, A2, B1) != counterClockWise(A1, A2, B2);
}

struct sim
{
	int bestAngle;
	int bestThrust;
	bool bestHitEnemy;
	Coordinate targetLocation;
	Coordinate nextTargetLocation;
	Coordinate targetVelocity;
	Coordinate newTargetLocation;
	bool ram;
	Coordinate newLocation;
	Coordinate newVelocity;
	Coordinate newLocationUpdater;
	Coordinate newVelocityUpdater;
	int newDistance;
	int positiveCheckAngle;
	int throttleIndex;
	int checkThrottle;
	int checkAngle;
	bool sharpTurn;
	bool closerToTarget;
	bool hitEnemy;
	bool maxAngle;
};

clock_t turnStartTime;
vector<Pod*> myPods;
vector<Pod*> theirPods;
vector<int> throttles;
vector<sim> sims;

// Returns the difference angle
int angleDifference(int angleA, int angleB)
{
	return min(abs(((angleA + CIRCLE_DEGREES) - angleB) % CIRCLE_DEGREES), abs(((angleB + CIRCLE_DEGREES) - angleA) % CIRCLE_DEGREES));
}

void setupSimulation()
{
	sims.resize(PODS_PER_TEAM);

	// Consider only these throttle values during the simulation
	throttles.push_back(20);
	throttles.push_back(50);
	throttles.push_back(100);
	throttles.push_back(150);
	throttles.push_back(MAX_THRUST); // 200
}

void simulate(double deadline)
{
	int bestScore = std::numeric_limits<int>::min();
	bool abort = false;
	int numThrottles = throttles.size();

	double timeout = turnStartTime + (double)CLOCKS_PER_SEC * deadline;

	// Initialize things that are constant throughout all simulations
	for (int i = 0; i < PODS_PER_TEAM; i++)
	{
		myPods[i]->simulationPrerequisites();

		sims[i].bestAngle = myPods[i]->angle;
		sims[i].bestThrust = myPods[i]->thrust;
		sims[i].targetLocation = myPods[i]->target;
		sims[i].targetVelocity = Coordinate(0,0);
		sims[i].ram = myPods[i]->targetEnemy != NULL;
		
		if (sims[i].ram)
		{
			// If hunting an enemy, its position will change based on its velocity
			sims[i].targetVelocity = theirPods[myPods[i]->targetEnemy->name]->velocity;			
		}
		else
		{
			// If racing, figure out the next checkpoint so we can try to get to it as quickly as possible
			sims[i].nextTargetLocation = checkpoints[(myPods[i]->nextCheckpoint + 1) % checkpoints.size()]->location;
		}
	}
	
	// For every throttle and angle of both of my pods
	for (sims[0].throttleIndex = 0; !abort && sims[0].throttleIndex < numThrottles; ++sims[0].throttleIndex)	
	{
		sims[0].checkThrottle = throttles[sims[0].throttleIndex];
		for (sims[0].checkAngle = myPods[0]->angle - ANGLE_PER_STEP; !abort && sims[0].checkAngle <= myPods[0]->angle + ANGLE_PER_STEP; ++sims[0].checkAngle)
		{
			for (sims[1].throttleIndex = 0; !abort && sims[1].throttleIndex < numThrottles; ++sims[1].throttleIndex)	
			{
				sims[1].checkThrottle = throttles[sims[1].throttleIndex];
				for (sims[1].checkAngle = myPods[1]->angle - ANGLE_PER_STEP; !abort && sims[1].checkAngle <= myPods[1]->angle + ANGLE_PER_STEP; ++sims[1].checkAngle)
				{
					// Initialize this simulation variables
					int score = 0;
					for (int i = 0; i < PODS_PER_TEAM; i++)
					{
						sims[i].newLocation = myPods[i]->location;
						sims[i].newVelocity = myPods[i]->velocity;
						sims[i].positiveCheckAngle = (sims[i].checkAngle + CIRCLE_DEGREES) % CIRCLE_DEGREES;
						sims[i].newLocationUpdater = Coordinate(round(sims[i].checkThrottle * cos(sims[i].checkAngle * DEG_TO_RAD)), round(sims[i].checkThrottle * sin(sims[i].checkAngle * DEG_TO_RAD)));
						sims[i].newVelocityUpdater = Coordinate(floor(sims[i].checkThrottle * cos(sims[i].checkAngle * DEG_TO_RAD)), floor(sims[i].checkThrottle * sin(sims[i].checkAngle * DEG_TO_RAD)));
						sims[i].hitEnemy = false;
						sims[i].maxAngle = sims[i].checkAngle == myPods[i]->angle - ANGLE_PER_STEP || sims[i].checkAngle == myPods[i]->angle + ANGLE_PER_STEP;
					}

					// Run simulation
					for (int step = 1; step <= SIMULATE_STEPS; ++step)
					{
						for (int i = 0; i < PODS_PER_TEAM; i++)
						{
							sims[i].newLocation = sims[i].newLocation + sims[i].newVelocity + sims[i].newLocationUpdater;
							sims[i].newVelocity = (sims[i].newVelocity + sims[i].newVelocityUpdater) * FRICTION;
							sims[i].newTargetLocation = sims[i].targetLocation + (sims[i].targetVelocity * step);

							// If a racer isn't on the home stretch
							if (!sims[i].ram && myPods[i]->checkpointsRemaining != 0)
							{
								// Take into account the next checkpoint
								score += angleDifference(sims[i].nextTargetLocation.angleTo(sims[i].newLocation), sims[i].positiveCheckAngle) * ANGLE_SCORE;
							}

							sims[i].newDistance = sims[i].newTargetLocation.distance(sims[i].newLocation);
							score += sims[i].newDistance * DISTANCE_SCORE;

							// If hit my checkpoint
							if (!sims[i].ram && sims[i].newDistance < CHECKPOINT_TARGET_RADIUS)
							{
								score += HIT_TARGET_SCORE;
								
								// If about to hit the target
								if (step == 1)
								{
									// If can hit at a slow thrust, can also hit at a fast thrust.
									sims[i].checkThrottle = MAX_THRUST;
								}
							}

							// Check whether enemies are hit this turn
							for (int j = 0; j < PODS_PER_TEAM; j++)
							{
								Coordinate enemyVelocity = theirPods[j]->velocity * step;
								if (sims[i].newVelocity.dotProduct(enemyVelocity) <= 0
								 && sims[i].newLocation.squaredDistance(
										theirPods[j]->location + enemyVelocity) < PODS_TOUCH_SQUARED)
								{
									if (sims[i].ram && step == 1)
									{
										sims[i].hitEnemy = true;
										score += HIT_TARGET_SCORE;
									}
									else
									{
										// Racers should try to evade enemies
										if (!sims[i].maxAngle)
										{
											score += NOT_EVADING_SCORE;
										}
									}
								}
							}
						}

						// If my pods collided with each other
						if (sims[0].newLocation.squaredDistance(sims[1].newLocation) < PODS_TOUCH_SQUARED)
						{
							score += COLLIDE_SCORE;
						}
					}

					// Store the best simulation parameters
					if (bestScore < score)
					{
						bestScore = score;
						for (int i = 0; i < PODS_PER_TEAM; i++)
						{
							sims[i].bestAngle = sims[i].positiveCheckAngle;
							sims[i].bestThrust = sims[i].checkThrottle;
							sims[i].bestHitEnemy = sims[i].hitEnemy;
							sims[i].sharpTurn = (sims[i].newDistance <= CIRCLING_DISTANCE) && sims[i].maxAngle;
							sims[i].closerToTarget = sims[i].newDistance < myPods[i]->target.distance(myPods[i]->location);
						}
					}

					// If this is taking too long, get out before disqualification happens
					if (clock() >= timeout)
					{
						abort = true;
					}
				}
			}
		}
	}

	// Set the vars from the best simulation to the actual pods
	for (int i = 0; i < PODS_PER_TEAM; i++)
	{
		myPods[i]->steeringTarget = myPods[i]->location.pointFromAngle(sims[i].bestAngle);
		myPods[i]->thrust = sims[i].bestThrust;
		myPods[i]->activateShield = sims[i].bestHitEnemy;

		// If trying to hit a checkpoint but haven't reached in a while
		int circling = myPods[i]->circling(sims[i].sharpTurn);
		if (!sims[i].closerToTarget)
		{
			// Stops the thrust so we can recover if orbiting.
			if (circling == 2)
			{
				// Stops for at least 3 turns so might as well use shield too.
				myPods[i]->activateShield = true;
			}
			else if (circling == 1)
			{
				myPods[i]->thrust = 0;
			}
		}
	}

	// Output quotes
	for (int i = 0; i < PODS_PER_TEAM; i++)
	{
		if (myPods[i]->checkpointsRemaining == 0)
		{
			myPods[i]->sayQuote("End");
		}
		else if (sims[i].bestHitEnemy)
		{
			myPods[i]->sayQuote("Impact");
		}
		else if (myPods[i]->currentRole == role::HUNTER)
		{
			myPods[i]->sayQuote("Hunter");
		}
		else if (myPods[i]->ramming)
		{
			myPods[i]->sayQuote("Ramming");
		}
		else if (myPods[i]->velocity.magnitude() > FAST_SPEED)
		{
			myPods[i]->sayQuote("GoingFast");
		}
		else if (myPods[i]->thrust == 0)
		{
			myPods[i]->sayQuote("SharpTurn");
		}
		else if (myPods[i]->activateShield)
		{
			myPods[i]->sayQuote("SharpTurn");
		}
	}	
}

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
	int laps;
	cin >> laps; cin.ignore();
	int checkpointCount;
	cin >> checkpointCount; cin.ignore();
	for (int i = 0; i < checkpointCount; i++) {
		int checkpointX;
		int checkpointY;
		cin >> checkpointX >> checkpointY; cin.ignore();
		
		checkpoints.push_back(new Checkpoint(checkpointX, checkpointY));
	}

    fillQuotes();

	myPods.push_back(new Pod(0, laps, character::SEBULBA, true));
	myPods.push_back(new Pod(1, laps, character::ANAKIN, true));
	theirPods.push_back(new Pod(0, laps));
	theirPods.push_back(new Pod(1, laps));
	
	myPods[0]->setOtherPods(myPods[1], theirPods);
	myPods[1]->setOtherPods(myPods[0], theirPods);

	int initialAngle = checkpoints[1]->location.angleTo(checkpoints[0]->location); // System feeds angle -1 so use this instead
	setupSimulation();

	// game loop
	while (1) {
		turnStartTime = clock();

		for (int i = 0; i < 2; i++) {
			int x;
			int y;
			int vx;
			int vy;
			int angle;
			int nextCheckPointId;
			cin >> x >> y >> vx >> vy >> angle >> nextCheckPointId; cin.ignore();

			// Initial angle is wrong so update it to something mostly right
			if (angle < 0)
			{
				angle = initialAngle;
			}
			myPods[i]->update(x, y, vx, vy, angle, nextCheckPointId);
		}
		for (int i = 0; i < 2; i++) {
			int x;
			int y;
			int vx;
			int vy;
			int angle;
			int nextCheckPointId;
			cin >> x >> y >> vx >> vy >> angle >> nextCheckPointId; cin.ignore();
			if (angle < 0)
			{
				angle = initialAngle;
			}
			theirPods[i]->update(x, y, vx, vy, angle, nextCheckPointId);
		}
		
		simulate(SIMULATION_DEADLINE);
		// Write an action using cout. DON'T FORGET THE "<< endl"
		// To debug: cerr << "Debug messages..." << endl;
		for (int i = 0; i < 2; i++)
		{
			cout << myPods[i]->command() << endl;
		    cerr << *myPods[i] << endl;	
		}

		for (int i = 0; i < 2; i++)
		{
			cerr << *theirPods[i] << endl;
		}

		double duration = (clock() - turnStartTime) / (double)CLOCKS_PER_SEC;
		cerr << "Duration: " << duration << (duration > SIMULATION_DEADLINE ? "!!!!" : "") << endl;
	}
}
