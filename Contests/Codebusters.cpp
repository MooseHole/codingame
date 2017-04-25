// Build 29:  Gold#CURR/833@201607031132 Tighten staleness resolution.
// Build 28:  Gold# 699/833@201607031121 More goalies.
// Build 27:  Gold# 672/833@201607030120 Resubmit for Gold.
// Build 26:Silver#   1/240@201607030120 Resubmit for Silver.
// Build 25:Bronze#   1/334@201607030120 Don't hunt for carried nor busted ghosts.
// Build 24:Bronze#   4/334@201607030036 Keep goalie exploring in bounds.  Only stun carriers.
// Build 23:Bronze#  20/335@201607030013 Fix stamina problems.
// Build 22:Bronze# 215/315@201607022047 Resubmit for Bronze.
// Build 21: Wood1#   1/300@201607022047 Fix exploration scoring and goalie targetting.
// Build 20: Wood1#  47/300@201607021304 Add goalie.
// Build 19: Wood1#  48/307@201607021244 Fix untargetted ghosts logic.  Move to enemy buster.
// Build 18: Wood1# 237/307@201607020954 Don't explore when ghosts should be targetted.
// Build 17: Wood1#   6/306@201607012202 Explore enemy base.
// Build 16: Wood1#   5/308@201607012118 Busting is more important than moving around.
// Build 15: Wood1#   3/309@201607011633 Return directly to corner.  Only stun carriers.
// Build 14: Wood1#   4/298@201607011605 Don't look ahead too far when you have a target.
// Build 13: Wood1#  43/299@201607011233 Only use max speed.
// Build 12: Wood1# 140/298@201607011233 Fix simulation lookup problems.
// Build 11: Wood1# 216/321@201606301528 More detailed simulation.
// Build 10: Wood1# 175/324@201606301413 Refactor.
// Build  9: Wood1# 267/328@201606301340 Step through exploration to see if direction is good.
// Build  8: Wood1# 265/331@201606301131 Favor exploration last.
// Build  7: Wood1# 305/323@201606301039 Add staleness (exploration).
// Build  6: Wood1# 206/323@201606292318 Add stun.
// Build  5: Wood1# 188/354@201606292050 Resubmit for Wood1.
// Build  4: Wood2#   1/155@201606292050 Target different ghosts.  Don't stop.
// Build  3: Wood2#   8/159@201606292035 Make score calculation long long int so everything fits.
// Build  2: Wood2#   3/156@201606291222 Decouple movement.
// Build  1: Wood2#  14/153@201606291058 Initial submission.

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <algorithm>

using namespace std;

// Defined by the problem
#define SQUARE_BASE_SIZE			2560000	// Square of the size of the base
#define SQUARE_MIN_CAPTURE_DISTANCE	810000	// Square of the minimum distance to bust
#define SQUARE_MAX_CAPTURE_DISTANCE	3097600 // Square of the maximum distance to bust
#define SQUARE_MAX_VISION			4840000	// Square of the maximum distance I can see
#define MAX_SPEED					800		// Maximum walking speed
#define GHOST_SPEED					400		// Speed of ghosts
#define GRID_WIDTH					16000	// Width of the grid
#define GRID_HEIGHT					9000	// Height of the grid

// General definitions
#define CIRCLE_DEGREES	360						// The number of degrees in a circle
#define RAD_TO_DEG		180.0f/3.14159f			// Used to convert radians to degrees
#define DEG_TO_RAD		3.14159f/180.0f			// Used to convert degrees to radians
#define MIN_LONG_LONG	-9223372036854775807	// Minimum value of long long int

// Variables to be tweaked
#define NUM_SPEEDS				1		// The number of speeds to check in simulation
#define NUM_ANGLES				20		// The number of angles to check in simulation
#define STALE_RESOLUTION		400		// The how far apart checked stale pixels are
#define EDGE_STALE_BOOST        10		// How much more the edges get stale
#define ENEMY_BASE_STALE_BOOST  10		// How much more the edges get stale
#define GHOST_SEARCH_TURNS		10		// How long since I've seen ghost that I should search
#define GHOST_STALENESS_TURNS	30		// How long since I've seen ghost that I should ignore
#define MAX_BUST_STAMINA		15		// Ghosts with more stamina will be ignored
#define MAX_STEPS				5		// Number of steps to simulate for exploration
#define SCORE_RELEASE			90000000000//10000	// Score for releasing a ghost
#define SCORE_BUST				70000000000 //2000	// Score for busting a ghost
#define SCORE_DISTANCE			-10		// Score for how far I am from my target
#define SCORE_STALE_RESET		1		// Score for reseting stale pixels
#define SCORE_STUNNED			0		// Score for being stunned
#define SCORE_DO_STUN			100		// Score for stunning an enemy
#define SCORE_GHOST_DISTANCE	-1		// Score for how far away ghosts are from base
#define SIMULATION_DEADLINE		0.095	// Amount of time to simulate in seconds
#define SQUARE_GOALIE_BOUNDARY	SQUARE_BASE_SIZE * 8
#define GOALIE_INCREASE_TURN    150

class Coordinate;
class Ghost;
class Buster;
static minstd_rand randomEngine(clock());
static uniform_int_distribution<int> randomRotation{0, NUM_ANGLES};
static uniform_int_distribution<int> randomSpeed{0, NUM_SPEEDS};
vector<Coordinate> base;
map<int, Ghost> ghosts;
map<int, Buster> busters;
vector<Buster> sims;
vector<Ghost> simGhosts;

int staleness[GRID_WIDTH][GRID_HEIGHT];
int myTeamId; // if this is 0, your base is on the top left of the map, if it is one, on the bottom right
int enemyTeamId;
int numGoalies;

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
	
	void contrain(int minX, int minY, int maxX, int maxY)
	{
		if (x < minX)
		{
			x = minX;
		}
		if (y < minY)
		{
			y = minY;
		}
		if (x > maxX)
		{
			x = maxX;
		}
		if (y > maxY)
		{
			y = maxY;
		}
	}
	
	//// VECTOR JUNK ////
	
	// This gives the magnitude of the vector
	int magnitude()
	{
		return sqrt(pow(x, 2) + pow(y, 2));		
	}

	// Returns the difference angle in degrees
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

enum command
{
	MOVE,
	BUST,
	RELEASE,
	STUN
};

class Ghost
{
	public:
	int id;
	Coordinate location;
	int type;
	int state;
	int value;
	int carriedBy;
	bool busted;
	int known;
	bool targetted;
	bool everSeen;
	
	Ghost()
	{
	}
	
	Ghost (int entityId, int x, int y, int entityType, int _state, int _value)
	{
		id = entityId;
		location = Coordinate(x, y);
		type = entityType;
		state = _state;
		value = _value;
		carriedBy = -1;
		busted = false;
		targetted = false;
		everSeen = false;
		known = 0;
	}
	
	void update(int x, int y, int entityType, int _state, int _value)
	{
		location.x = x;
		location.y = y;
		type = entityType;
		state = _state;
		value = _value;
//		carriedBy = -1;
		known = 0;
		everSeen = true;
		targetted = false;
	}

	bool operator< (const Ghost& other) const
	{
		return getStamina() < other.getStamina();
	}
	
	void setUnknown()
	{
		known++;
		carriedBy = -1;
	}
	
	bool canBeBusted()
	{
		return id >= 0 && carriedBy < 0 && !busted;
	}

	int getStamina() const
	{
		return state;
	}

	int numberOfBusters()
	{
		return value;
	}

	friend ostream &operator<<(ostream &os, Ghost const &m)
	{
		return os << "{" << m.id << "," << m.location << "," << m.type << "," << m.state << "," << m.value << "," << m.carriedBy << "," << (m.busted ? "B" : "n") << "," << m.known << "}";
	}
};

class Buster
{
	public:
	int id;
	Coordinate location;
	int type;
	int state;
	int value;
	int known;
	Coordinate targetLocation;
	int targetGhostId;
	int targetStunId;
	command targetCommand;
	int stunRecharge;
	string comment;
	
	command storedCommand;
	Coordinate storedLocation;
	int storedGhostId;
	int storedStunId;
	Coordinate storedMovement;
	bool goalie;
	bool goalieOutOfBounds;

	Buster()
	{
	}
	
	Buster (int entityId, int x, int y, int entityType, int _state, int _value)
	{
		id = entityId;
		location = Coordinate(x, y);
		type = entityType;
		state = _state;
		value = _value;
		targetGhostId = -1;
		targetStunId = -1;
		targetCommand = command::MOVE;
		stunRecharge = 0;
		known = 0;
		storedCommand = command::MOVE;
		storedLocation = location;
		storedGhostId = targetGhostId;
		storedStunId = targetStunId;
		storedMovement = Coordinate(0,0);
		goalie = false;
		goalieOutOfBounds = false;
	}

	void update(int x, int y, int entityType, int _state, int _value)
	{
		location.x = x;
		location.y = y;
		type = entityType;
		state = _state;
		value = _value;
		if (state == 1 && value >= 0)
		{
			ghosts[value].carriedBy = id;
		}
		known = 0;
		if (stunRecharge > 0)
		{
			stunRecharge--;
		}
		
		if (type == myTeamId)
		{
			for (int x = 0; x < GRID_WIDTH; x += STALE_RESOLUTION)
			{
				for (int y = 0; y < GRID_HEIGHT; y += STALE_RESOLUTION)
				{
					if (location.squaredDistance(Coordinate(x,y)) <= SQUARE_MAX_VISION)
					{
						staleness[x][y] = 0;
					}
				}
			}
		}
		
		goalie = false;
		goalieOutOfBounds = false;
	}

	void setUnknown()
	{
		known++;
	}

	int getTeam()
	{
		return type;
	}
	
	bool isCarrying()
	{
		return state == 1;
	}
	
	bool isTrapping(int myGhost)
	{
		if (state == 3)
		{
			return value == myGhost;
		}

		return false;
	}
	
	bool inBase()
	{
		return location.squaredDistance(base[getTeam()]) < SQUARE_BASE_SIZE;
	}
	
	bool canScore()
	{
		return isCarrying() && inBase();
	}
	
	bool haveTargetGhost()
	{
		if (targetGhostId >= 0)
		{
			for (vector<Ghost>::iterator git = simGhosts.begin(); git != simGhosts.end(); ++git)
			{
				if ((*git).id == targetGhostId)
				{
					return (*git).canBeBusted() && (*git).known < GHOST_SEARCH_TURNS;
				}
			}
		}
		
		return false;
	}

	bool stunned()
	{
		return state == 2;
	}
	
	bool canStun()
	{
		targetStunId = -1;

		if (stunRecharge > 0)
		{
			return false;
		}

		for (map<int, Buster>::iterator bit = busters.begin(); bit != busters.end(); ++bit)
		{
			if ((*bit).second.known == 0 && ((*bit).second.isCarrying()/* || isCarrying()*/) && !(*bit).second.stunned() && (*bit).second.getTeam() != getTeam()
				&& (*bit).second.location.squaredDistance(location) < SQUARE_MAX_CAPTURE_DISTANCE
				&& (*bit).second.location.squaredDistance(base[enemyTeamId]) > SQUARE_BASE_SIZE)
			{
				// Make sure nobody else is also stunning
				bool setTarget = true;
				for (map<int, Buster>::iterator bitCheck = busters.begin(); bitCheck != busters.end(); ++bitCheck)
				{
					if ((*bitCheck).second.targetStunId == (*bit).second.id)
					{
						setTarget = false;
						break;
					}
				}
				
				if (setTarget)
				{
					targetStunId = (*bit).second.id;
					break;
				}
			}
		}
		
		return targetStunId >= 0;
	}
	
	bool searchForGhost()
	{
		if (isCarrying() || stunned())
		{
			return false;
		}
		
		if (!haveTargetGhost())
		{
			acquireTargetGhost();
		}
	}

	bool canBust()
	{
		int squareDistance = SQUARE_MAX_CAPTURE_DISTANCE * 2; // initialize to out of bust range
		if (haveTargetGhost())
		{
			for (vector<Ghost>::iterator git = simGhosts.begin(); git != simGhosts.end(); ++git)
			{
				if ((*git).id == targetGhostId)
				{
					if ((*git).known == 0)
					{
						squareDistance = (*git).location.squaredDistance(location);
					}
					break;
				}
			}

			return atBustDistance(squareDistance);
		}
		
		return false;
	}
	
	bool atBustDistance(int squareDistance)
	{
		return squareDistance <= SQUARE_MAX_CAPTURE_DISTANCE && squareDistance >= SQUARE_MIN_CAPTURE_DISTANCE;
	}
	
	int squareDistanceFromTarget()
	{
		if (isCarrying())
		{
			int squareDistance = location.squaredDistance(base[getTeam()]);
			if (squareDistance < SQUARE_BASE_SIZE)
			{
				return SQUARE_BASE_SIZE - squareDistance;
			}
			else if (squareDistance > SQUARE_BASE_SIZE)
			{
				return squareDistance;
			}
			else
			{
				return 0;
			}

		}
		else if (haveTargetGhost())
		{
			int squareDistance = 0;
			for (vector<Ghost>::iterator git = simGhosts.begin(); git != simGhosts.end(); ++git)
			{
				if ((*git).id == targetGhostId)
				{
					squareDistance = (*git).location.squaredDistance(location);
					break;
				}
			}

			if (squareDistance < SQUARE_MIN_CAPTURE_DISTANCE)
			{
				return SQUARE_MIN_CAPTURE_DISTANCE - squareDistance;
			}
			else if (squareDistance > SQUARE_MAX_CAPTURE_DISTANCE)
			{
				return squareDistance - SQUARE_MAX_CAPTURE_DISTANCE;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			// No target, so wander aimlessly.
			return 0;
		}
	}

	bool acquireTargetGhost()
	{
		int minDistance = pow(GRID_WIDTH + GRID_HEIGHT, 2);
		targetGhostId = -1;
		int numChecked = 0;

		vector<Ghost>::iterator git;
		for (git = simGhosts.begin(); git != simGhosts.end(); ++git)
		{
			// Provided simGhosts is sorted by stamina, break after counting too high
			if (numChecked > sims.size())
			{
				break;
			}
			
			if ((*git).everSeen && (*git).known < GHOST_STALENESS_TURNS)
			{
				numChecked++;
			}

			if (!(*git).canBeBusted() || (*git).targetted || (*git).getStamina() > MAX_BUST_STAMINA)
			{
				continue;
			}
			
			if (goalie)
			{
				if ((*git).location.squaredDistance(base[enemyTeamId]) > SQUARE_GOALIE_BOUNDARY)
				{
					continue;
				}
			}

			
			int squareDistance = (*git).location.squaredDistance(location);
			if (atBustDistance(squareDistance))
			{
				targetGhostId = (*git).id;
				break;
			}
			else if (squareDistance < minDistance && (*git).known < GHOST_STALENESS_TURNS)
			{
				minDistance = squareDistance;
				targetGhostId = (*git).id;
			}
		}

		if (haveTargetGhost())
		{
			for (git = simGhosts.begin(); git != simGhosts.end(); ++git)
			{
				if ((*git).id == targetGhostId)
				{
					(*git).targetted = true;
					break;
				}
			}

			return true;
		}

		return false;
	}

	void executeCommand()
	{
		switch (targetCommand)
		{
			case MOVE:
				cout << "MOVE " << targetLocation.x << " " << targetLocation.y << " " << comment << endl;
				break;
			case BUST:
				cout << "BUST " << targetGhostId << " " << comment << endl;
				break;
			case STUN:
				stunRecharge = 20;
				cout << "STUN " << targetStunId << " " << comment << endl;
				break;
			case RELEASE:
				if (canScore())
				{
					ghosts[value].busted = true;
				}
				cout << "RELEASE" << " " << comment << endl;
				break;
		}
	}

	friend ostream &operator<<(ostream &os, Buster const &m)
	{
		return os << "(" << m.id << "," << m.location << "," << m.type << "," << m.state << "," << m.value << "," << m.targetGhostId << "," << m.targetStunId << "," << m.stunRecharge << ")";
	}
};


class Team
{
	public:
	int id;
	vector<int> angles;
	vector<int> speeds;
	
	Team()
	{
		int speedIncrement = MAX_SPEED / NUM_SPEEDS;
		int speed = MAX_SPEED;
		while (speed > 0)
		{
			speeds.push_back(speed);
			speed -= speedIncrement;
		}
		
		int angleIncrement = CIRCLE_DEGREES / NUM_ANGLES;
		int angle = 0;
		while (angle < CIRCLE_DEGREES)
		{
			angles.push_back(angle);
			angle += angleIncrement;
		}
	}

	void simulate(double deadline)
	{
		long long int bestScore = MIN_LONG_LONG;

		int iterations = 0;
		// for rest of time limit
		while (clock() < deadline)
		{
			vector<Coordinate> exploredLocations;
			iterations++;
			sims.clear();
			simGhosts.clear();

			for (map<int, Buster>::iterator bit = busters.begin(); bit != busters.end(); ++bit)
			{
				if ((*bit).second.type == myTeamId)
				{
					sims.push_back((*bit).second);
				}
			}
			// Different evaluation order might make a difference
			random_shuffle (sims.begin(), sims.end());

			for (map<int, Ghost>::iterator git = ghosts.begin(); git != ghosts.end(); ++git)
			{
				simGhosts.push_back((*git).second);
			}
			sort (simGhosts.begin(), simGhosts.end());    
		
			long long int score = 0;
			
			// Designate goalie
			for (int g = 0; g < numGoalies; g++)
			{
				int closestSim = -1;
				int closestSimDistance = pow(GRID_WIDTH + GRID_HEIGHT, 2);
				for (vector<Buster>::iterator sim = sims.begin(); sim != sims.end(); ++sim)
				{
					if (!(*sim).isCarrying() && !(*sim).stunned() && (*sim).stunRecharge == 0 && !(*sim).goalie)
					{
						int squareDistance = (*sim).location.squaredDistance(base[enemyTeamId]);
						if (squareDistance < closestSimDistance)
						{
							closestSimDistance = squareDistance;
							closestSim = (*sim).id;
						}
					}
				}

				for (vector<Buster>::iterator sim = sims.begin(); sim != sims.end(); ++sim)
				{
					if ((*sim).id == closestSim)
					{
						(*sim).goalie = true;
						(*sim).goalieOutOfBounds = closestSimDistance > SQUARE_GOALIE_BOUNDARY;
					}
				}
			}

			bool anySimsExploring = false;
			for (int step = 0; step < MAX_STEPS; ++step)
			{
				for (vector<Buster>::iterator sim = sims.begin(); sim != sims.end(); ++sim)
				{
					if (step == 0)
					{
						int angle = angles[randomRotation(randomEngine)];
						int speed = speeds[randomSpeed(randomEngine)];
						if (speed == 0)
						{
							speed = MAX_SPEED;
						}

						(*sim).storedMovement = Coordinate(round(speed * cos(angle * DEG_TO_RAD)), round(speed * sin(angle * DEG_TO_RAD)));
					}

					if (step == 0)
					{
						(*sim).comment = "";
					}
					
					(*sim).searchForGhost();

					if ((*sim).stunned())
					{
						score += SCORE_STUNNED;

						if ((*sim).value > 0)
						{
							(*sim).value--;
						}
						else
						{
							(*sim).state = 0;
						}
						
						if (step == 0)
						{
							(*sim).storedCommand = command::RELEASE;
							(*sim).comment = "ow";
						}
					}
					else if ((*sim).canScore())
					{
						score += SCORE_RELEASE;

						for (vector<Ghost>::iterator gsim = simGhosts.begin(); gsim != simGhosts.end(); ++gsim)
						{
							if ((*sim).value == (*gsim).id)
							{
								(*gsim).busted = true;
								break;
							}
						}
						(*sim).state = 0;

						if (step == 0)
						{
							(*sim).storedCommand = command::RELEASE;
							(*sim).comment = "Got number " + to_string((*sim).value) + "!";
						}
					}
					else if ((*sim).canStun())
					{
						score += SCORE_DO_STUN;
						(*sim).stunRecharge = 20;

						if (step == 0)
						{
							(*sim).storedStunId = (*sim).targetStunId;
							(*sim).storedCommand = command::STUN;
							(*sim).comment = "Eat this " + to_string((*sim).targetStunId) + "!";
						}
					}
					else if ((*sim).goalieOutOfBounds)
					{
						if (step == 0)
						{
							(*sim).storedCommand = command::MOVE;
							(*sim).storedLocation = base[enemyTeamId];
							(*sim).comment = "Infiltrating";
						}
					}
					else if ((*sim).canBust())
					{
						score += SCORE_BUST;
						if (step == 0)
						{
							for (vector<Ghost>::iterator gsim = simGhosts.begin(); gsim != simGhosts.end(); ++gsim)
							{
								if ((*sim).targetGhostId == (*gsim).id)
								{
									if ((*gsim).numberOfBusters() > 1)
									{
//										int minEnemyDistance = SQUARE_MAX_CAPTURE_DISTANCE * 2;
										for (map<int, Buster>::iterator bit = busters.begin(); bit != busters.end(); ++bit)
										{
											if ((*bit).second.known == 0 && (*bit).second.getTeam() != myTeamId && (*bit).second.isTrapping((*sim).targetGhostId)/* && (*bit).second.location.squaredDistance((*sim).location) < minEnemyDistance*/)
											{
												(*sim).storedCommand = command::MOVE;
												(*sim).storedLocation = (*bit).second.location;
												(*sim).comment = "Yo Homie!";
												break;
											}
										}
									}
									else
									{
										(*gsim).carriedBy = (*sim).id;
										(*sim).state = 1;
										(*sim).value = (*sim).targetGhostId;
										(*sim).storedGhostId = (*sim).targetGhostId;
										(*sim).storedCommand = command::BUST;
										(*sim).comment = "Busting " + to_string((*sim).targetGhostId) + "!";
										break;
									}
								}
							}
						}
					}
					else if ((*sim).isCarrying() || (*sim).haveTargetGhost())
					{
						// Don't look too far ahead if you have a target
						if (step == 0)
						{
							(*sim).storedCommand = command::MOVE;

							(*sim).location += (*sim).storedMovement;
							(*sim).location.contrain(0, 0, GRID_WIDTH, GRID_HEIGHT);

							int squareDistance = (*sim).squareDistanceFromTarget();
							score += SCORE_DISTANCE * squareDistance;

							exploredLocations.push_back((*sim).location);
							(*sim).storedLocation = (*sim).location;

							if ((*sim).isCarrying())
							{
								// FORCE THEM TO GO HOME
								(*sim).storedLocation = base[myTeamId];
								(*sim).comment = "Returning to base.";
							}
							else if ((*sim).haveTargetGhost())
							{
								(*sim).comment = "Hunting for " + to_string((*sim).targetGhostId) + ".";
							}
						}
					}
					else
					{
						anySimsExploring = true;
						(*sim).location += (*sim).storedMovement;
						(*sim).location.contrain(0, 0, GRID_WIDTH, GRID_HEIGHT);
						
						if (!(*sim).goalie || (*sim).location.squaredDistance(base[enemyTeamId]) < SQUARE_GOALIE_BOUNDARY)
						{
							exploredLocations.push_back((*sim).location);
						}
						
						if (step == 0)
						{
							(*sim).storedCommand = command::MOVE;
							(*sim).storedLocation = (*sim).location;
						}
					}
					
					if ((*sim).stunRecharge > 0)
					{
						(*sim).stunRecharge--;
					}
				}

				for (vector<Ghost>::iterator gsim = simGhosts.begin(); gsim != simGhosts.end(); ++gsim)
				{
					if ((*gsim).known == 0 && !(*gsim).busted && (*gsim).carriedBy < 0)
					{
						int xTotal = 0;
						int yTotal = 0;
						int totalSims = 0;
						for (vector<Buster>::iterator sim = sims.begin(); sim != sims.end(); ++sim)
						{
							if ((*sim).known == 0)
							{
								if ((*gsim).location.squaredDistance((*sim).location) <= SQUARE_MAX_VISION)
								{
									xTotal += (*sim).location.x;
									yTotal += (*sim).location.y;
									totalSims++;
								}
							}
						}
						
						if (totalSims > 0)
						{
							Coordinate meanLocation = Coordinate(xTotal/totalSims, yTotal/totalSims);
							int meanAngle = (*gsim).location.angleTo(meanLocation);
							
							(*gsim).location.x += round(GHOST_SPEED * cos(meanAngle));
							(*gsim).location.y += round(GHOST_SPEED * sin(meanAngle));
							(*gsim).location.contrain(0, 0, GRID_WIDTH, GRID_HEIGHT);
							if (!(*gsim).targetted)
							{
								score += (*gsim).location.squaredDistance(base[myTeamId]) * SCORE_GHOST_DISTANCE;
							}
						}
					}
				}
			}

			// Don't score for exploration if not all ghosts have been targetted.
			bool anyGhostsUntargetted = false;
			for (vector<Ghost>::iterator gsim = simGhosts.begin(); gsim != simGhosts.end(); ++gsim)
			{
				if ((*gsim).known == 0 && !(*gsim).targetted)
				{
					anyGhostsUntargetted = true;
					break;
				}
			}
					
			// Exploration score
			if (!anyGhostsUntargetted || !anySimsExploring)
			{
				for (int x = 0; x < GRID_WIDTH; x += STALE_RESOLUTION)
				{
					for (int y = 0; y < GRID_HEIGHT; y += STALE_RESOLUTION)
					{
						for (vector<Coordinate>::iterator loc = exploredLocations.begin(); loc != exploredLocations.end(); ++loc)
						{
							if ((*loc).squaredDistance(Coordinate(x,y)) <= SQUARE_MAX_VISION)
							{
								score += staleness[x][y] * SCORE_STALE_RESET;
								break;
							}
						}
					}
				}
			}

			if (score > bestScore)
			{
				bestScore = score;
				for (vector<Buster>::iterator sim = sims.begin(); sim != sims.end(); ++sim)
				{
					busters[(*sim).id].targetCommand = (*sim).storedCommand;
					busters[(*sim).id].targetLocation = (*sim).storedLocation;
					busters[(*sim).id].targetGhostId = (*sim).storedGhostId;
					busters[(*sim).id].targetStunId = (*sim).storedStunId;
					busters[(*sim).id].comment = (*sim).comment;
				}
			}
		}
		
		cerr << "Iterations: " << iterations << endl;
	}

	void execute()
	{
		for (map<int, Buster>::iterator bit = busters.begin(); bit != busters.end(); ++bit)
		{
			if ((*bit).second.type == myTeamId)
			{
				(*bit).second.executeCommand();
			}
		}
	}
};

/**
 * Send your busters out into the fog to trap ghosts and bring them home!
 **/
int main()
{
	int bustersPerPlayer; // the amount of busters you control
	cin >> bustersPerPlayer; cin.ignore();
	int ghostCount; // the amount of ghosts on the map
	cin >> ghostCount; cin.ignore();
	cin >> myTeamId; cin.ignore();
	Team team;
	team.id = myTeamId;
	enemyTeamId = myTeamId == 0 ? 1 : 0;
	numGoalies = bustersPerPlayer < 3 ? 1 : 2;

	// Set up base locations
	base.push_back(Coordinate(0, 0));
	base.push_back(Coordinate(GRID_WIDTH, GRID_HEIGHT));

	ghosts[-1] = Ghost(-1, -1, -1, -1, -1, -1); // Set up the null ghost
	ghosts[0] = Ghost(0, GRID_WIDTH/2, GRID_HEIGHT/2, -1, 0, 0); // Set up the known center ghost
	for (int git = 0; git < ghostCount; ++git)
	{
		ghosts[git] = Ghost(git, GRID_WIDTH/2, GRID_HEIGHT/2, -1, 0, 0);
	}
	
	for (int x = 0; x < GRID_WIDTH; x += STALE_RESOLUTION)
	{
		for (int y = 0; y < GRID_HEIGHT; y += STALE_RESOLUTION)
		{
			staleness[x][y] = 0;
		}
	}

	int turn = 0;
	// game loop
	while (1)
	{
		clock_t turnStartTime = clock();	
		turn++;
		if (turn > GOALIE_INCREASE_TURN)
		{
			numGoalies = bustersPerPlayer;
		}

		for (map<int, Ghost>::iterator git = ghosts.begin(); git != ghosts.end(); ++git)
		{
			(*git).second.setUnknown();
		}
		for (map<int, Buster>::iterator bit = busters.begin(); bit != busters.end(); ++bit)
		{
			(*bit).second.setUnknown();
		}
		for (int x = 0; x < GRID_WIDTH; x += STALE_RESOLUTION)
		{
			for (int y = 0; y < GRID_HEIGHT; y += STALE_RESOLUTION)
			{
				Coordinate thisCoordinate = Coordinate(x,y);
				if (thisCoordinate.squaredDistance(base[myTeamId]) > SQUARE_BASE_SIZE)
				{
					staleness[x][y]++;
					
					// If edge
					if (x == 0 || y == 0
				     || x + STALE_RESOLUTION > GRID_WIDTH 
				     || y + STALE_RESOLUTION > GRID_HEIGHT)
					{
						staleness[x][y] += EDGE_STALE_BOOST;
					}
					
					if (thisCoordinate.squaredDistance(base[enemyTeamId]) < SQUARE_BASE_SIZE)
					{
						staleness[x][y] += ENEMY_BASE_STALE_BOOST;
					}					
				}
			}
		}

		int entities; // the number of busters and ghosts visible to you
		cin >> entities; cin.ignore();
		for (int i = 0; i < entities; i++)
		{
			int entityId; // buster id or ghost id
			int x;
			int y; // position of this buster / ghost
			int entityType; // the team id if it is a buster, -1 if it is a ghost.
			int state; // For busters: 0=idle, 1=carrying a ghost.
			int value; // For busters: Ghost id being carried. For ghosts: number of busters attempting to trap this ghost.
			cin >> entityId >> x >> y >> entityType >> state >> value; cin.ignore();
//			cerr << entityId << "," << x << "," << y << "," << entityType << "," << state << "," << value << endl;

			if (entityType < 0)
			{
				if (!ghosts[entityId].everSeen)
				{
					// If not center, there is a mirror ghost to consider
					if (entityId > 0)
					{
						int mirrorGhostId = entityId - 1;
						
						// If odd
						if (entityId % 2 == 1)
						{
							mirrorGhostId = entityId + 1;
						}

						if (!ghosts[mirrorGhostId].everSeen)
						{
							ghosts[mirrorGhostId].update(GRID_WIDTH - x, GRID_HEIGHT - y, entityType, state, value);
						}
					}
				}

				ghosts[entityId].update(x, y, entityType, state, value);
			}
			else
			{
				map<int,Buster>::iterator bit;
				bit = busters.find(entityId);
				if (bit == busters.end())
				{
					busters[entityId] = Buster(entityId, x, y, entityType, state, value);
				}
				else
				{
					(*bit).second.update(x, y, entityType, state, value);
				}
			}
		}
		
		for (map<int, Buster>::iterator bit = busters.begin(); bit != busters.end(); ++bit)
		{
//			cerr << (*bit).first << ":" <<(*bit).second << endl;
		}
		for (map<int, Ghost>::iterator git = ghosts.begin(); git != ghosts.end(); ++git)
		{
//			cerr << (*git).first << ":" <<(*git).second << endl;
		}

		
		team.simulate(turnStartTime + (double)CLOCKS_PER_SEC * SIMULATION_DEADLINE);
		team.execute();
	}
}