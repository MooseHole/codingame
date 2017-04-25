/*
0032011801102016:curr/ 298:Go Blastscore again does not count threatened boxes.  If it is threatened, then I can't destroy it in time for points (probably).
0031003302102016: 235/ 298:Go If no safe destination can be found, reevaluate with no enemy bombings.
0030235801102016: 276/ 298:Go Prioritize Dist 0 and score, Dist 1 and item.  Fixed blast locations.  Blastscore still ticks threatened boxes.
0029113101102016: 165/ 300:Go Evaluate as if all other players dropped a bomb.
0028110301102016: 204/ 268:Go Look ahead at next move so I get trapped less.
0027013901102016: 105/ 248:Go Move on same turn as placing bomb.
0026010701102016:  96/ 248:Go Copy for Gold.
0025010701102016:   1/ 281:Si If no open reachables, lowest blastTurn tiles are not reachable.  Eliminates "Oh crap" check.
0024163530092016:  32/ 251:Si Fix cascading threats to stop at absorbing tiles.
0023110730092016: 174/ 246:Si Fix cascading threats
0022110730092016:  33/ 244:Si Update unreachable check so more accurate and more permissive.
0021110730092016:  94/ 244:Si Fixed offLimits detection to stop backtrackers.
0020002730092016:  78/ 240:Si Added offLimits when distanceFromPlayer == timeLeft.
0019125429092016: 109/ 244:Si Stop the flip-flopping at the beginning.
0018122129092016:????/????:Si Copy for Silver.
0017122129092016:????/1126:Br Prioritize items higher.  Go to blasts or items outsize of 8 if best option.
0016122129092016:????/1126:Br Better blast score determination.  Prioritize targets above 8 steps last.
0015113529092016:   6/1126:Br More suicide prevention.
0014093529092016: 216/1132:Br Don't get blown up.  Less score because threatened targets are not considered.
0013154628092016: 346/1131:Br More suicide prevention.
0012142328092016:????/????:Br Better distance finding and less suicide.
0011133128092016:????/????:Br Copy for Bronze.
0010133128092016:????/????:W1 Determine which tiles I can reach.
0009123028092016:????/????:W1 Copy for Wood 1.
0008123028092016:????/????:W2 Update for box chars.  Ignoring items.
0007114628092016:????/????:W2 Copy for Wood 2.
0006114628092016:????/????:W3 Eval sucks.  Heuristic not bad though.
0005203326092016:????/????:W3 Random moves, accurately figures score.
0004203326092016:????/????:W3 Random moves, remembers stuff better.  Still forgets scores.
0003182726092016:????/????:W3 Random moves, does some score calculation
0002235125092016:????/????:W3 Might do something if it compiles
0001221725092016:????/????:W3 Initial submission
*/

#include <iostream>
#include <string>
#include <set>
#include <list>
#include <vector>
#include <queue>
#include <algorithm>
#include <limits>

using namespace std;


#define SCORE_ME				100
#define SCORE_THEM				-100
#define SCORE_ME_POTENTIAL		10
#define SCORE_THEM_POTENTIAL	-10
#define SCORE_ME_BOMBS_LEFT		1
#define SCORE_THEM_BOMBS_LEFT	-1
#define SCORE_ME_BOX_DISTANCE	-1
#define SCORE_THEM_BOX_DISTANCE	1
#define SIMULATION_DEPTH		5
#define SIMULATION_DEADLINE		0.095
#define TICKS_PER_BOMB			8

static minstd_rand randomEngine(clock());
static uniform_int_distribution<int> randomPlaceBomb{0, 1};

string bestMove;
clock_t turnStartTime;

enum PrintType
{
	BOOM_TURN = 1 << 0,
	BLAST_SCORE = 1 << 1,
	DISTANCE_PLAYER = 1 << 2,
	DISTANCE_TARGET = 1 << 3,
	ENTITY = 1 << 4,
	BOUNDARY = 1 << 5,
};

class Coordinate
{
	public:
	int x;
	int y;
	
	Coordinate()
	{
		x = 0;
		y = 0;
	}
	
	Coordinate(int _x, int _y)
	{
		x = _x;
		y = _y;
	}
	
	Coordinate& operator=(Coordinate other)
	{
		x = other.x;
		y = other.y;
		return *this;
	}
	
    bool operator<(const Coordinate& other) const
    {
        return index() < other.index();
    }
	
    bool operator==(const Coordinate& other) const
    {
        return x == other.x && y == other.y;
    }
	
    bool operator!=(const Coordinate& other) const
    {
		return !(*this == other);
    }
	
	int manhattan(const Coordinate& other) const
	{
		return abs(x - other.x) + abs(y - other.y);
	}
	
	int index() const
	{
		return 10000 + x * 100 + y;
	}

	friend ostream &operator<<(ostream &os, Coordinate const &m)
	{
		return os << "(" << m.x << "," << m.y << ")";
	}
};

class Entity
{
	public:
	int type;
	int owner;
	Coordinate location;
	int param1;
	int param2;

	Entity(int _owner, int _x, int _y, int _param1, int _param2)
	{
		owner = _owner;
		location = Coordinate(_x, _y);
		param1 = _param1;
		param2 = _param2;
	}
	
	virtual bool absorbsBlast() const = 0;
};

class Item : public Entity
{
	public:
	Item(int owner, int x, int y, int param1, int param2) : Entity(owner, x, y, param1, param2)
	{
	}
	
	bool absorbsBlast() const
	{
		return true;
	}

	int type() const
	{
		return param1;
	}
};

class Bomb : public Entity
{
	private:
	bool exploding;

	public:

	Bomb(int owner, int x, int y, int param1, int param2) : Entity(owner, x, y, param1, param2)
	{
		type = 1;
		exploding = false;
	}
	
	Bomb& operator=(Bomb other)
	{
		type = other.type;
		owner = other.owner;
		location = other.location;
		param1 = other.param1;
		param2 = other.param2;

		exploding = other.isExploding();

		return *this;
	}
	
	void setExploding()
	{
		exploding = true;
	}
	
	bool isExploding() const
	{
		return exploding;
	}

	void update(int _param1, int _param2)
	{
		if (param1 > _param1)
		{
			param1 = _param1;
		}

		param2 = _param2;
	}

	int timeLeft() const
	{
//		return param1 - 1
		return param1;
	}
	
	int rawTimeLeft() const
	{
		return param1;
	}
	
	void setTimeLeft(int ticks)
	{
		param1 = ticks;
	}
	
	int blastRadius() const
	{
		return param2;
	}

	bool absorbsBlast() const
	{
		return true;
	}
	
	void tick()
	{
		param1--;
		if (timeLeft() <= 0)
		{
			setExploding();
		}
	}
	
	friend ostream &operator<<(ostream &os, Bomb const &m)
	{
		return os << "B" << m.owner << m.location;
	}
};

class Player : public Entity
{
	public:
	Coordinate destination;
	int score;
	bool bombing;
	vector<Coordinate> validMoves;
	string comment;
	bool inTrouble;
	
	Player(int owner, int x, int y, int param1, int param2) : Entity(owner, x, y, param1, param2)
	{
		type = 0;
		destination = location;
		score = 0;
		bombing = false;
		validMoves.clear();
		inTrouble = false;
	}
	
	Player& operator=(Player other)
	{
		type = other.type;
		owner = other.owner;
		location = other.location;
		param1 = other.param1;
		param2 = other.param2;

		destination = other.destination;
		score = other.score;
		bombing = other.bombing;
		inTrouble = other.inTrouble;

		for (vector<Coordinate>::iterator move = other.validMoves.begin(); move != other.validMoves.end(); ++move)
		{
			validMoves.push_back(*move);
		}

		return *this;
	}

	void update(int x, int y, int _param1, int _param2)
	{
		location = Coordinate(x, y);
		param1 = _param1;
		param2 = _param2;
	}

	int bombsLeft() const
	{
		return param1;
	}	
	
	int blastRadius() const
	{
		return param2;
	}

	bool absorbsBlast() const
	{
		return false;
	}
	
	bool canPlaceBomb() const
	{
		return bombsLeft() > 0;
	}
	
	bool placeBomb()
	{
		bombing = false;
		if (canPlaceBomb())
		{
			param1--;
			return true;
		}
		
		return false;
	}
	
	string moveString()
	{
		string result = "";
		result += bombing ? "BOMB" : "MOVE";
		result += " ";
		result += to_string(destination.x);
		result += " ";
		result += to_string(destination.y);
		if (!comment.empty())
		{
			result += " ";
			result += comment;
		}

		
		return result;
	}

	friend ostream &operator<<(ostream &os, Player const &m)
	{
		return os << "P" << m.owner << m.location;
	}
};

class Floor
{
	public:
	bool box;
	bool wall;
	bool offLimits;
	bool reachable;
	bool targetUpdated;
	set<int> exploders;
	Coordinate location;
	vector<Coordinate> checkMoves;
	int distanceFromPlayer;
	int distanceFromTarget;
	bool threatened;
	int boomTurn;
	int blastScore;
	int item;
	
	Floor(Coordinate _location, bool _box, bool _wall)
	{
		location = _location;
		box = _box;
		box = _wall;
		exploders.clear();

		checkMoves.push_back(Coordinate(location.x, location.y));
		checkMoves.push_back(Coordinate(location.x - 1, location.y));
		checkMoves.push_back(Coordinate(location.x + 1, location.y));
		checkMoves.push_back(Coordinate(location.x, location.y - 1));
		checkMoves.push_back(Coordinate(location.x, location.y + 1));
		distanceFromPlayer = numeric_limits<int>::max();
		distanceFromTarget = numeric_limits<int>::max();
		threatened = false;
		boomTurn = numeric_limits<int>::max();
		blastScore = 0;
		item = 0;
		reachable = false;
		targetUpdated = false;
		offLimits = false;
	}

	Floor& operator=(Floor other)
	{
		box = other.box;
		wall = other.wall;
		reachable = other.reachable;
		targetUpdated = other.targetUpdated;

		exploders.clear();
		for (set<int>::iterator exploder = other.exploders.begin(); exploder != other.exploders.end(); ++exploder)
		{
			exploders.insert(*exploder);
		}

		location = other.location;

		checkMoves.clear();
		for (vector<Coordinate>::iterator move = other.checkMoves.begin(); move != other.checkMoves.end(); ++move)
		{
			checkMoves.push_back(*move);
		}
		
		distanceFromPlayer = other.distanceFromPlayer;
		distanceFromTarget = other.distanceFromTarget;
		threatened = other.threatened;
		boomTurn = other.boomTurn;
		blastScore = other.blastScore;
		item = other.item;
		offLimits = other.offLimits;

		return *this;
	}
	
	vector<vector<Coordinate>> blastLocations(int blastRadius)
	{
		vector<vector<Coordinate>> blast;
		
		blast.push_back(vector<Coordinate>());
		blast.push_back(vector<Coordinate>());
		blast.push_back(vector<Coordinate>());
		blast.push_back(vector<Coordinate>());
		blast.push_back(vector<Coordinate>());

		blast[0].push_back(location);

		for (int i = 1; i < blastRadius; i++)
		{
			blast[1].push_back(Coordinate(location.x + i, location.y));
			blast[2].push_back(Coordinate(location.x - i, location.y));
			blast[3].push_back(Coordinate(location.x, location.y + i));
			blast[4].push_back(Coordinate(location.x, location.y - i));
		}
		
		return blast;
	}
	
	bool isBox() const
	{
		return box;
	}
	
	bool isWall() const
	{
		return wall;
	}
	
	bool walkable() const
	{
		return !box && !wall && !offLimits;
	}
	
	void resetExplosions()
	{
		exploders.clear();
		box = false;
	}
	
	bool exploding()
	{
		return exploders.size() > 0;
	}
	
	bool absorbsBlast() const
	{
		return box || wall || item;
	}
	
	int score() const
	{
		return box ? 1 : 0;
	}
	
	void update(char input)
	{
		switch (input)
		{
			case '.':
				box = false;
				wall = false;
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				box = true;
				wall = false;
				break;
			case 'X':
				box = false;
				wall = true;
				break;
			default:
				break;
		}
	}
};

class Grid
{
	public:
	vector<vector<Floor>> tiles;
	vector<Bomb> bombs;
	vector<Player> players;
	int playerId;
	int turnsLeft;
	vector<Coordinate> reachables;

	Grid()
	{}

	Grid(int width, int height, int myId, int _turnsLeft)
	{
		for (int x = 0; x < width; x++)
		{
			vector<Floor> floorColumn;

			for (int y = 0; y < height; y++)
			{
				floorColumn.push_back(Floor(Coordinate(x, y), false, false));
			}
			
			tiles.push_back(floorColumn);
		}
		
		playerId = myId;
		turnsLeft = _turnsLeft;
	}
	
	Grid& operator=(Grid other)
	{
		tiles.clear();
		bombs.clear();
		players.clear();

		for (int x = 0; x < other.width(); x++)
		{
			vector<Floor> floorColumn;

			for (int y = 0; y < other.height(); y++)
			{
				floorColumn.push_back(other.tiles[x][y]);
			}
			
			tiles.push_back(floorColumn);
		}

		for (vector<Bomb>::iterator bomb = other.bombs.begin(); bomb != other.bombs.end(); ++bomb)
		{
			bombs.push_back(*bomb);
		}
		
		for (vector<Player>::iterator player = other.players.begin(); player != other.players.end(); ++player)
		{
			players.push_back(*player);
		}
		
		playerId = other.playerId;
		turnsLeft = other.turnsLeft;
		
		return *this;
	}
	
	int height() const
	{
		if (width() > 0)
		{
			return tiles[0].size();
		}
		
		return 0;
	}
	
	int width() const
	{
		return tiles.size();
	}

	void updateTiles(int row, string input)
	{
		for (int x = 0; x < width(); x++)
		{
			tiles[x][row].update(input[x]);
		}		
	}

	void updateThreats()
	{
		// Cascade
		bool cascaded = true;
		while (cascaded)
		{
			cascaded = false;
			for (vector<Bomb>::iterator bomb = bombs.begin(); bomb != bombs.end(); ++bomb)
			{
				vector<vector<Coordinate>> blast = tiles[(*bomb).location.x][(*bomb).location.y].blastLocations((*bomb).blastRadius());
				for (vector<vector<Coordinate>>::iterator direction = blast.begin(); direction != blast.end(); ++direction)
				{
					for (vector<Coordinate>::iterator boom = (*direction).begin(); boom != (*direction).end(); ++boom)
					{
						if (inBounds(*boom))
						{
							if (hasBomb(*boom) && ((*boom) != (*bomb).location))
							{
								for (vector<Bomb>::iterator checkBomb = bombs.begin(); checkBomb != bombs.end(); ++checkBomb)
								{
									// If a bomb's location is blown up
									if ((*checkBomb).location == (*boom))
									{
										if ((*checkBomb).timeLeft() > (*bomb).timeLeft())
										{
											(*checkBomb).setTimeLeft((*bomb).rawTimeLeft());
											cascaded = true;
										}
									}
								}
							}
							
							if (tiles[(*boom).x][(*boom).y].absorbsBlast() || (hasBomb(*boom) && ((*boom) != (*bomb).location)))
							{
								break;
							}
						}
					}
				}
			}
		}

		// Threaten
		for (vector<Bomb>::iterator bomb = bombs.begin(); bomb != bombs.end(); ++bomb)
		{
			vector<vector<Coordinate>> blast = tiles[(*bomb).location.x][(*bomb).location.y].blastLocations((*bomb).blastRadius());
			for (vector<vector<Coordinate>>::iterator direction = blast.begin(); direction != blast.end(); ++direction)
			{
				for (vector<Coordinate>::iterator boom = (*direction).begin(); boom != (*direction).end(); ++boom)
				{
					if (inBounds(*boom))
					{
						tiles[(*boom).x][(*boom).y].threatened = true;
						tiles[(*boom).x][(*boom).y].boomTurn = min(tiles[(*boom).x][(*boom).y].boomTurn, (*bomb).timeLeft());
						
						if (tiles[(*boom).x][(*boom).y].absorbsBlast() || (hasBomb(*boom) && ((*boom) != (*bomb).location)))
						{
							break;
						}
					}
				}
			}
		}
	}

	void addEntity(int entityType, int owner, int x, int y, int param1, int param2)
	{
		bool found = false;
		switch (entityType)
		{
			case 0:
				players.push_back(Player(owner, x, y, param1, param2));
				break;
			case 1:
				bombs.push_back(Bomb(owner, x, y, param1, param2));
				break;
			case 2:
				tiles[x][y].item = param1;
			default:
				break;
		}
	}

	void placeBombs()
	{
		for (vector<Player>::iterator player = players.begin(); player != players.end(); ++player)
		{
			if ((*player).bombing)
			{
				if ((*player).placeBomb())
				{
					Bomb bomb = Bomb((*player).owner, (*player).location.x, (*player).location.y, TICKS_PER_BOMB, (*player).param2);
					bombs.push_back(bomb);
				}
			}
		}
	}
	
	void move()
	{
		for (vector<Player>::iterator player = players.begin(); player != players.end(); ++player)
		{
			// Assume only 1 square moves.
			if (!tiles[(*player).destination.x][(*player).destination.y].isBox())
			{
				(*player).location = (*player).destination;
			}
		}
	}
	
	void tickBombs()
	{
		for (vector<Bomb>::iterator bomb = bombs.begin() ; bomb != bombs.end(); ++bomb)
		{
			(*bomb).tick();
		}
	}
	
	bool inBounds(Coordinate check)
	{
		if (check.x < 0 || check.x >= width() || check.y < 0 || check.y >= height())
		{
			return false;
		}
		
		return true;
	}
	
	bool inBounds(int x, int y)
	{
		if (x < 0 || x >= width() || y < 0 || y >= height())
		{
			return false;
		}
		
		return true;
	}
	
	bool isHabitable(Coordinate check)
	{
		return inBounds(check) && tiles[check.x][check.y].walkable();
	}
	
	bool hasBomb(Coordinate check)
	{
		for (vector<Bomb>::iterator bomb = bombs.begin(); bomb != bombs.end(); ++bomb)
		{
			if (check == (*bomb).location)
			{
				return true;
			}
		}
		
		return false;
	}
	
	void clearReachables()
	{
		reachables.clear();
		for (int x = 0; x < width(); x++)
		{
			for (int y = 0; y < height(); y++)
			{
				tiles[x][y].reachable = false;
			}
		}
	}

	void updateReachable()
	{
		clearReachables();
		vector<Coordinate> tempReachable;
		int lowestBoomTurn = numeric_limits<int>::max();
		for (vector<Player>::iterator player = players.begin(); player != players.end(); ++player)
		{
			if ((*player).owner == playerId)
			{
				queue<Coordinate> Q;
				Q.push((*player).location);
				tiles[(*player).location.x][(*player).location.y].distanceFromPlayer = 0;

				while (!Q.empty())
				{
					Coordinate current = Q.front();
					Q.pop();
					tiles[current.x][current.y].reachable = true;
					tempReachable.push_back(current);
					lowestBoomTurn = min(lowestBoomTurn, tiles[current.x][current.y].boomTurn);

					int distance = tiles[current.x][current.y].distanceFromPlayer + 1;
					for (vector<Coordinate>::iterator move = tiles[current.x][current.y].checkMoves.begin(); move != tiles[current.x][current.y].checkMoves.end(); ++move)
					{
						if (inBounds(*move) && !tiles[(*move).x][(*move).y].reachable)
						{
							if (distance == tiles[(*move).x][(*move).y].boomTurn - 1)
							{
								tiles[(*move).x][(*move).y].offLimits = true;
							}
							else if (tiles[(*move).x][(*move).y].walkable() && (!hasBomb(*move) || (*move) == myLocation()))
							{
								tiles[(*move).x][(*move).y].distanceFromPlayer = distance;
								Q.push(*move);
							}
						}
					}				
				}
			}
		}
		
		clearReachables();
		for (vector<Coordinate>::iterator reach = tempReachable.begin(); reach != tempReachable.end(); ++reach)
		{
			if (lowestBoomTurn == numeric_limits<int>::max() || tiles[(*reach).x][(*reach).y].boomTurn != lowestBoomTurn)
			{
				tiles[(*reach).x][(*reach).y].reachable = true;
				reachables.push_back(*reach);
			}
		}
	}

	Coordinate myLocation()
	{
		for (vector<Player>::iterator player = players.begin(); player != players.end(); ++player)
		{
			if ((*player).owner == playerId)
			{
				return (*player).location;
			}
		}
		
		return Coordinate(0, 0);
	}

	void updateTargetDistance(Coordinate target)
	{
		queue<Coordinate> Q;
		Q.push(target);
		tiles[target.x][target.y].distanceFromTarget = 0;
		
		while (!Q.empty())
		{
			Coordinate current = Q.front();
			Q.pop();
			tiles[current.x][current.y].targetUpdated = true;

			for (vector<Coordinate>::iterator move = tiles[current.x][current.y].checkMoves.begin(); move != tiles[current.x][current.y].checkMoves.end(); ++move)
			{
				if (inBounds(*move) && !tiles[(*move).x][(*move).y].targetUpdated && tiles[(*move).x][(*move).y].walkable() && (!hasBomb(*move) || (*move) == myLocation()))
				{
					tiles[(*move).x][(*move).y].distanceFromTarget = tiles[current.x][current.y].distanceFromTarget + 1;
					Q.push(*move);
				}
			}
		}
	}
	

	void updatePlayerMoves()
	{
		updateThreats();

		for (vector<Player>::iterator player = players.begin(); player != players.end(); ++player)
		{
			(*player).validMoves.clear();

			for (vector<Coordinate>::iterator move = tiles[(*player).location.x][(*player).location.y].checkMoves.begin(); move != tiles[(*player).location.x][(*player).location.y].checkMoves.end(); ++move)
			{
				if (isHabitable(*move) && (!hasBomb(*move) || (*move) == (*player).location))
				{
					(*player).validMoves.push_back(*move);
				}
			}
		}
	}
	
	void performMoves()
	{
		// Players then perform their actions simultaneously.
		// Any bombs placed by a player appear at the end of the round.
		placeBombs();		
		move();
	}
	
	bool isTerminal()
	{
		return turnsLeft <= 0;
	}
	
	void updateBlastScores()
	{
		for (vector<Player>::iterator player = players.begin(); player != players.end(); ++player)
		{
			if ((*player).owner == playerId)
			{
				for (int x = 0; x < width(); x++)
				{
					for (int y = 0; y < height(); y++)
					{
						tiles[x][y].blastScore = 0;
						vector<vector<Coordinate>> blast = tiles[x][y].blastLocations((*player).blastRadius());
						for (vector<vector<Coordinate>>::iterator direction = blast.begin(); direction != blast.end(); ++direction)
						{
							for (vector<Coordinate>::iterator boom = (*direction).begin(); boom != (*direction).end(); ++boom)
							{
								if (inBounds(*boom))
								{
									if (!tiles[(*boom).x][(*boom).y].threatened && tiles[(*boom).x][(*boom).y].isBox())
									{
										tiles[x][y].blastScore++;
									}
									
									if (tiles[(*boom).x][(*boom).y].absorbsBlast() || (hasBomb(*boom) && ((*boom) != (*player).location)))
									{
										break;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	
	string outputMove()
	{
		for (vector<Player>::iterator player = players.begin(); player != players.end(); ++player)
		{
			if ((*player).owner == playerId)
			{
				return (*player).moveString();
			}
		}
		
		return "PLAYER NOT FOUND";
	}
	
	string print(int types) const
	{
		string output = "";
		int numTypes = 0;
		
		output += "Types";
		if (types & BOOM_TURN)
		{
			output += " BoomTurns";
			numTypes++;
		}
		if (types & BLAST_SCORE)
		{
			output += " BlastScore";
			numTypes++;
		}
		if (types & DISTANCE_PLAYER)
		{
			output += " PlayerDistance";
			numTypes++;
		}
		if (types & DISTANCE_TARGET)
		{
			output += " TargetDistance";
			numTypes++;
		}
		if (types & ENTITY)
		{
			output += " Entity";
			numTypes++;
		}
		if (types & BOUNDARY)
		{
			output += " Boundary";
			numTypes++;
		}

		output += ":\n";
		output += " /";
		for (int x = 0; x < width(); x++)
		{
			for (int pad = 2; pad <= numTypes; pad++)
			{
				output += "  ";
			}

			if (x <= 9)
			{
				output += " ";
			}
			output += to_string(x);
		}
		output += "\n";
		for (int y = 0; y < height(); y++)
		{
			if (y <= 9)
			{
				output += " ";
			}
			output += to_string(y);
			for (int x = 0; x < width(); x++)
			{
				if (types & BOOM_TURN)
				{
					if (tiles[x][y].boomTurn == numeric_limits<int>::max())
					{
						output += "--";
					}
					else if (tiles[x][y].boomTurn <= 9)
					{
						output += " " + to_string(tiles[x][y].boomTurn);
					}
					else
					{
						output += to_string(tiles[x][y].boomTurn);
					}
				}
				if (types & BLAST_SCORE)
				{
					if (tiles[x][y].blastScore < 0)
					{
						output += "--";
					}
					else if (tiles[x][y].blastScore <= 9)
					{
						output += " " + to_string(tiles[x][y].blastScore);
					}
					else
					{
						output += to_string(tiles[x][y].blastScore);
					}
				}
				if (types & DISTANCE_PLAYER)
				{
					if (tiles[x][y].distanceFromPlayer == numeric_limits<int>::max())
					{
						output += "--";
					}
					else if (tiles[x][y].distanceFromPlayer <= 9)
					{
						output += " " + to_string(tiles[x][y].distanceFromPlayer);
					}
					else
					{
						output +=  to_string(tiles[x][y].distanceFromPlayer);
					}
				}
				if (types & DISTANCE_TARGET)
				{
					if (tiles[x][y].distanceFromTarget == numeric_limits<int>::max())
					{
						output += "--";
					}
					else if (tiles[x][y].distanceFromTarget <= 9)
					{
						output += " " + to_string(tiles[x][y].distanceFromTarget);
					}
					else
					{
						output +=  to_string(tiles[x][y].distanceFromTarget);
					}
				}
				if (types & ENTITY)
				{
					Coordinate coord = Coordinate(x, y);
					string playerString = "-";
					for (vector<Player>::const_iterator player = players.begin(); player != players.end(); ++player)
					{
						if ((*player).location == coord)
						{
							if ((*player).owner == playerId)
							{
								playerString = "P";
							}
							else if (playerString == "-")
							{
								playerString = "p";
							}
						}
					}

					string bombString = "-";
					for (vector<Bomb>::const_iterator bomb = bombs.begin(); bomb != bombs.end(); ++bomb)
					{
						if ((*bomb).location == coord)
						{
							bombString = "b";
						}
					}

					if (tiles[x][y].item > 0)
					{
						if (bombString == "-")
						{
							bombString = to_string(tiles[x][y].item);
						}
						else
						{
							bombString = "B";
						}
					}
					
					output += playerString + bombString;
				}
				if (types & BOUNDARY)
				{
					if (tiles[x][y].wall)
					{
						output += "WA";
					}
					else if (tiles[x][y].box)
					{
						output += "BX";
					}
					else if (tiles[x][y].offLimits)
					{
						output += "0L";
					}
					else
					{
						output += "--";
					}
				}
			}
			output += "\n";
		}
		
		return output;
	}

	friend ostream &operator<<(ostream &os, Grid const &m)
	{
		os << m.print(0xFF);
		os << m.print(BOOM_TURN);
		os << m.print(BLAST_SCORE);
		os << m.print(DISTANCE_PLAYER);
		os << m.print(DISTANCE_TARGET);
		os << m.print(ENTITY);
		os << m.print(BOUNDARY);
		return os;
	}
};

bool timeout()
{
	return clock() > turnStartTime + (double)CLOCKS_PER_SEC * SIMULATION_DEADLINE;
}


Coordinate blastScoreReference;
bool compareBlastScore (const Floor& first, const Floor& second)
{
	int firstDistance = first.distanceFromPlayer;
	int secondDistance = second.distanceFromPlayer;

	// If dropping a bomb here would give me points
	if (firstDistance == 0 && secondDistance > 0 && first.blastScore > 0)
	{
		return true;
	}
	
	// If an item is 1 step away
	if (firstDistance == 1 && secondDistance > 1 && first.item > 0)
	{
		return true;
	}

	if (first.boomTurn != second.boomTurn)
	{
		return first.boomTurn > second.boomTurn;
	}

	if (firstDistance < TICKS_PER_BOMB && secondDistance < TICKS_PER_BOMB)
	{
		if (first.blastScore != second.blastScore)
		{
			return first.blastScore > second.blastScore;
		}
		
		if (first.item != second.item)
		{
			return first.item > second.item;
		}
	}

	if (firstDistance != secondDistance)
	{
		return firstDistance < secondDistance;
	}
	else
	{
		if (first.blastScore != second.blastScore)
		{
			return first.blastScore > second.blastScore;
		}
		
		if (first.item != second.item)
		{
			return first.item > second.item;
		}
	}

	return first.location.index() < second.location.index();
}

bool compareTargetDistance (const Floor& first, const Floor& second)
{
	return first.distanceFromTarget < second.distanceFromTarget;
}
Grid grid;
bool updatePlayerMove(bool placeBombHere, bool enemiesPlaceBombs)
{
	bool nextDestinationNeeded = false;
	
	if (enemiesPlaceBombs)
	{
		for (vector<Player>::iterator player = grid.players.begin(); player != grid.players.end(); ++player)
		{
			if ((*player).owner != grid.playerId)
			{
				// Allows evaluation of everyone else setting a bomb
				(*player).bombing = true;
			}
		}
		grid.performMoves();
	}

	grid.updateThreats();
	grid.updateReachable();
	grid.updateBlastScores();
	grid.updatePlayerMoves();
	cerr << grid.print(BOOM_TURN);
	cerr << grid.print(BOUNDARY);
	cerr << grid.print(ENTITY);
	cerr << grid.print(BLAST_SCORE);

	list<Floor> floorBlasts;

	for (int x = 0; x < grid.width(); x++)
	{
		for (int y = 0; y < grid.height(); y++)
		{
			if (grid.tiles[x][y].reachable)
			{
				floorBlasts.push_back(grid.tiles[x][y]);
			}
		}
	}

	for (vector<Player>::iterator player = grid.players.begin(); player != grid.players.end(); ++player)
	{
		if ((*player).owner != grid.playerId)
		{
			// Allows evaluation of everyone else setting a bomb
			(*player).bombing = true;
		}
		else
		{
			blastScoreReference = ((*player).location);
			(*player).inTrouble = false;
			(*player).bombing = placeBombHere;

			floorBlasts.sort(compareBlastScore);
			bool destinationFound = false;
			for (list<Floor>::iterator floorBlast = floorBlasts.begin(); floorBlast != floorBlasts.end(); ++floorBlast)
			{
				// Go here
				(*player).destination = (*floorBlast).location;

				// Check whether if I place a bomb at the destination, I can escape the blast
				set<Coordinate> unreachables;
				vector<vector<Coordinate>> blast = grid.tiles[(*player).destination.x][(*player).destination.y].blastLocations((*player).blastRadius());
				for (vector<vector<Coordinate>>::iterator direction = blast.begin(); direction != blast.end(); ++direction)
				{
					for (vector<Coordinate>::iterator boom = (*direction).begin(); boom != (*direction).end(); ++boom)
					{
						if (grid.inBounds(*boom))
						{
							for (vector<Coordinate>::iterator reachable = grid.reachables.begin(); reachable != grid.reachables.end(); ++reachable)
							{
								if ((*reachable) == (*boom))
								{
									unreachables.insert(*reachable);
								}
							}

							if (grid.tiles[(*boom).x][(*boom).y].absorbsBlast() || (grid.hasBomb(*boom) && ((*boom) != (*player).destination)))
							{
								break;
							}
						}
					}
				}
				
				// If a safe haven can be reached
				if (unreachables.size() < grid.reachables.size())
				{
					// If already here and can place the bomb
					if ((*player).canPlaceBomb() && (*player).location == (*player).destination)
					{
						(*player).bombing = true;
						nextDestinationNeeded = true;
					}

					destinationFound = true;
					break; // Stop checking floorblasts
				}
			}

			(*player).comment = "";
//			(*player).comment += "Destination (" + to_string((*player).destination.x) + "," + to_string((*player).destination.y) + ")";

			if (!destinationFound)
			{
				(*player).inTrouble = true;
			}


		}
	}
	
	return nextDestinationNeeded;
}

void moveHeuristic()
{
/*
	bool placeBomb = updatePlayerMove(false);
	grid.performMoves();
	grid.tickBombs();
	updatePlayerMove(placeBomb);
*/

	Grid revertGrid = grid;
	if (updatePlayerMove(false, true))
	{
		grid.performMoves();
		updatePlayerMove(true, true);
	}

	// If no safe destination found, reevaluate as if enemies did not add bombs
	for (vector<Player>::iterator player = grid.players.begin(); player != grid.players.end(); ++player)
	{
		if ((*player).owner == grid.playerId)
		{
			if ((*player).inTrouble)
			{
				(*player).comment += "Oh crap.";
				grid = revertGrid;
				updatePlayerMove(false, false);
			}
			break;
		}
	}
	
	for (vector<Player>::iterator player = grid.players.begin(); player != grid.players.end(); ++player)
	{
		if ((*player).owner == grid.playerId)
		{
			grid.updateTargetDistance((*player).destination);
		}
	}
	
//	cerr << grid;
//	cerr << grid.print(BLAST_SCORE);
//	cerr << grid.print(BOOM_TURN);

	for (vector<Player>::iterator player = grid.players.begin(); player != grid.players.end(); ++player)
	{
		if ((*player).owner == grid.playerId)
		{
			list<Floor> nextStep;
			for (vector<Coordinate>::iterator move = (*player).validMoves.begin(); move != (*player).validMoves.end(); ++move)
			{
				nextStep.push_back(grid.tiles[(*move).x][(*move).y]);
			}
			
			nextStep.sort(compareTargetDistance);
			(*player).destination = nextStep.front().location;

			bestMove = (*player).moveString();
			break;
		}
	}
}

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
    int width;
    int height;
    int myId;
    cin >> width >> height >> myId; cin.ignore();

    // game loop
	int turnsLeft = 200;
    while (1)
	{
		grid = Grid(width, height, myId, turnsLeft--);
        for (int i = 0; i < height; i++)
		{
            string row;
            getline(cin, row);
			grid.updateTiles(i, row);
        }

		turnStartTime = clock();

        int entities;
        cin >> entities; cin.ignore();
        for (int i = 0; i < entities; i++)
		{
            int entityType;
            int owner;
            int x;
            int y;
            int param1;
            int param2;
            cin >> entityType >> owner >> x >> y >> param1 >> param2; cin.ignore();
			grid.addEntity(entityType, owner, x, y, param1, param2);
        }

		moveHeuristic();

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        cout << bestMove << endl;
    }
}