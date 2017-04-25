// Build 40: Gold  # 69/ 625@201605080045 Tweak dropBlock method.  Use more explicit output move.
// Build 39: Gold  # 84/ 625@201605080024 Fix panic mode so it updates the output move.
// Build 38: Gold  # 57/ 625@201605072316 Add panic mode if ends up with an invalid move.
// Build 37: Gold  # 94/ 623@201605072253 Applied enemy skull anticipation to victory path.
// Build 36: Gold  # 64/ 621@201605071625 "Better first block processing." was the problem.  Removed and re-added enemy skull anticipation.
// Build 35: Gold  #131/ 616@201605071625 Better first block processing.
// Build 34: Gold  # 99/ 607@201605071613 Rollback to Build 31.
// Build 33: Gold  #346/ 607@201605071433 Enemy skull anticipation for best score.  May be a problem where first move drops when no space.
// Build 32: Gold  #352/ 605@201605071401 Better first block processing.  Started enemy skull anticipation.
// Build 31: Gold  # 23/ 594@201605061203 Implemented separate first block processing.
// Build 30: Gold  # 42/ 550@201605060846 Lowered deadline to 0.097 due to some timeout losses.
// Build 29: Gold  # 53/ 549@201605060838 Fix first block so it actually works.  Started separate first block processing.
// Build 28: Gold  # 31/ 523@201605060022 Drop first block when able to bury, or when they are about to skull and I can do any score.  Added bury path, it sucks.
// Build 27: Gold  # 34/ 513@201605051608 Blocks keep track of victory status.  Increased efficiency of enemy next score evaluation.  Increased TIMEOUT to 0.098.
// Build 26: Gold  # 50/ 415@201605042010 Rollback to Build 23.
// Build 25: Gold  #286/ 409@201605042000 Revert findReactionScore because it may have been buggy.
// Build 24: Gold  #385/ 409@201605041809 Use better reaction finder.  Give more time for deadline.
// Build 23: Gold  # 12/ 391@201605041221 Give more weight to moves that drop skulls.
// Build 22: Gold  # 39/ 389@201605041159 Commit outside of simulation.
// Build 21: Gold  #184/ 479@201605041133 Optimize tall column 3.  Commit inside of simulation again.
// Build 20: Gold  #140/ 157@201605041111 Restore squaring of point multiplier.  Optimize short column 3(?).
// Build 19: Gold  #104/ 157@201605041050 Remove squaring of point multiplier.
// Build 18: Gold  # 87/ 157@201605041021 Optimize tall column 3.  Square point multiplier to optimize quick score.  Commit outside of simulation to increase simulate speed.
// Build 17: Gold  # 59/ 153@201605040852 Only do one cout per turn.
// Build 16: Gold  # 73/ 151@201605040834 Encapsulated moves.  Fixed block commands after victory path.
// Build 15: Gold  # 64/ 142@201605040003 Improved imminent skull checking.  Added command staging.
// Build 14: Gold  #126/ 131@201605031453 Added checkmate comments.
// Build 13: Gold  #110/ 119@201605031409 Determine enemy score.  Use nuisance points to determine score needed to kill.
// Build 11: Silver#  8/1217@201605031052 Go back to using 5040 to check for kill.
// Build 10: Silver# 45/1212@201605031040 Use current block levels to determine score needed to kill.
// Build  9: Silver# 11/1207@201605030833 Optimize score to fire as soon as can drop full grid of skulls.
// Build  8: Silver#  5/1202@201605030808 Add more weight to next blocks in score evaluation.  Use immediate block if they are close to dropping nuisance.
// Build  7: Silver#631/1061@201605021336 Fix bad group allocation.  Add AI to randomly test all 8 blocks.
// Build  6: Silver#334/ 842@201605012037 Fix score calculation.  Add simple AI for 1 step best score.
// Build  5: Silver#333/ 827@201605012037 Fix skull pops.  Add class Group.  Begin score calculation.
// Build  3: Silver#207/ 500@201605011019 Fix grid compression (needs skulls added).  Fix silver output (rotation).  Fix debug formatting.
// Build  2: Bronze#154/ 500@201604302256 Check upper layers if out of space at 3.  Also developing group evaluations.
// Build  1: Bronze#290/ 480@201604301814 Always drop in leftmost column with at least 3 space.

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <random>

using namespace std;

#define GRID_WIDTH			6
#define GRID_HEIGHT			12
#define SKULL_POINT_SCORE	70.0f
#define TIMEOUT				(double) CLOCKS_PER_SEC * 0.097

static minstd_rand randomEngine(clock());
static uniform_int_distribution<int> randomRotation{0, 3};
static uniform_int_distribution<int> randomX{0, GRID_WIDTH-1};
static uniform_int_distribution<int> randomXrot0{0, GRID_WIDTH-2};
static uniform_int_distribution<int> randomXrot2{1, GRID_WIDTH-1};
static uniform_int_distribution<int> randomReaction{0, 21};

map<int, int> bestScore;
int bestScoreX3Height;

char colorOutput(int color)
{
	switch (color)
	{
		case 0:
			return 's';
		case 1:
			return 'b';
		case 2:
			return 'g';
		case 3:
			return 'v';
		case 4:
			return 'r';
		case 5:
			return 'y';
		default:
			return '.';
	}
}

class Move
{
	public:
	int x;
	int rotation;
	
	Move()
	{
		reset();
	}
	
	Move(int _x, int _rotation)
	{
		x = _x;
		rotation = _rotation;
	}
	
	void reset()
	{
		x = -1;
		rotation = -1;
	}
	
	bool isSet() const
	{
		return x >= 0;
	}
	
	void randomize()
	{
		rotation = randomRotation(randomEngine);
		switch (rotation)
		{
			case 0:
				x = randomXrot0(randomEngine);
				break;
			case 2:
				x = randomXrot2(randomEngine);
				break;
			default:
				x = randomX(randomEngine);
				break;
		}
	}
	
	friend ostream &operator<<(ostream &os, Move const &m)
	{
		return os << m.x << " " << m.rotation;
	}
};

class Block
{
	private:
	
	public:
	int colorA;
	int colorB;
	Move assigned;
	Move temp;
	map<int, Move> bestScore;
	Move skullNow;
	map<int, Move> victoryPath;
	map<int, bool> victoryTemp;
	bool victory;
	Move buryPath;
	bool buryTemp;
	bool bury;

	Block()
	{
		resetCommands();
	}

	Block(int a, int b)
	{
		resetCommands();
		colorA = a;
		colorB = b;
	}
	
	void assign(int x, int rot)
	{
		assigned.x = x;
		assigned.rotation = rot;
	}
	
	void resetCommands()
	{
		resetAssignedData();
		resetTempData();
		resetStagingData();
	}
	
	void resetAssignedData()
	{
		assigned.reset();
		victory = false;
		bury = false;
	}
	
	void resetTempData()
	{
		temp.reset();
	}
	
	void resetStagingData()
	{
		bestScore[0].reset();
		skullNow.reset();
		resetVictoryData();
		resetBuryData();
	}
	
	void resetVictoryData()
	{
		victoryPath[0].reset();
		victoryTemp[0] = false;
	}
	
	void stageVictory(int droppedSkulls)
	{
		victoryTemp[droppedSkulls] = true;
	}

	void resetBuryData()
	{
		buryPath.reset();
		buryTemp = false;
	}
	
	void stageBury()
	{
		buryTemp = true;
	}

	void stageBestScoreCommands(int droppedSkulls)
	{
		bestScore[droppedSkulls] = temp;
	}
	
	void stageSkullNowCommands()
	{
		skullNow = temp;
	}
	
	void stageVictoryPathCommands(int droppedSkulls)
	{
		victoryPath[droppedSkulls] = temp;
	}
	
	void stageBuryPathCommands()
	{
		buryPath = temp;
	}
	
	void commitBestScoreCommands()
	{
		assigned = bestScore[0];
	}
	
	void commitSkullNowCommands()
	{
		assigned = skullNow;
	}
	
	void commitVictoryPathCommands()
	{
		assigned = victoryPath[0];
		victory = victoryTemp[0];
	}
	
	void commitBuryPathCommands()
	{
		assigned = buryPath;
		bury = buryTemp;
	}
	
	bool noCommand() const
	{
		return !assigned.isSet();
	}
	
	void randomizeTemp()
	{
		temp.randomize();
	}
	
	friend ostream &operator<<(ostream &os, Block const &m)
	{
		os << "[" << colorOutput(m.colorA)<< colorOutput(m.colorB);
		if (!m.noCommand())
		{
			os << m.assigned;
		}
		os << "]";

		return os;
	}
};

class BlockQueue
{
	vector<Block> blocks;
	public:
	
	vector<Block>::iterator begin()
	{
		return blocks.begin();
	}
	
	vector<Block>::iterator end()
	{
		return blocks.end();
	}

	void resetCommands()
	{
		for (vector<Block>::iterator it = blocks.begin(); it != blocks.end(); ++it)
		{
			(*it).resetCommands();
		}
	}
	
	void updateStaging(int skullsDropped)
	{
		for (vector<Block>::iterator it = blocks.begin(); it != blocks.end(); ++it)
		{
			if ((*it).bestScore[skullsDropped].isSet())
			{
				(*it).bestScore[0] = (*it).bestScore[skullsDropped];
			}

			if ((*it).victoryPath[skullsDropped].isSet())
			{
				(*it).victoryPath[0] = (*it).victoryPath[skullsDropped];
				(*it).victoryTemp[0] = (*it).victoryTemp[skullsDropped];
			}

			for (map<int, Move>::iterator stage = (*it).bestScore.begin(); stage != (*it).bestScore.end(); ++stage)
			{
				if ((*stage).first != 0)
				{
					(*it).bestScore.erase(stage);
				}
			}
			for (map<int, Move>::iterator stage = (*it).victoryPath.begin(); stage != (*it).victoryPath.end(); ++stage)
			{
				if ((*stage).first != 0)
				{
					(*it).victoryPath.erase(stage);
				}
			}
					
			for (map<int, bool>::iterator stage = (*it).victoryTemp.begin(); stage != (*it).victoryTemp.end(); ++stage)
			{
				if ((*stage).first != 0)
				{
					(*it).victoryTemp.erase(stage);
				}
			}
		}
	}
	
	void stageSkullNowCommands()
	{
		blocks[0].stageSkullNowCommands();
	}
	
	void commitSkullNowCommands()
	{
		blocks[0].commitSkullNowCommands();
	}
	
	void stageVictoryPathCommands(int droppedSkulls, int numBlocks)
	{
		int blockNum = 0;
		for (vector<Block>::iterator it = blocks.begin(); it != blocks.end(); ++it)
		{
			blockNum++;
			(*it).resetVictoryData();
			if (blockNum <= numBlocks)
			{
				(*it).stageVictoryPathCommands(droppedSkulls);
				if (blockNum == numBlocks)
				{
					(*it).stageVictory(droppedSkulls);
				}
			}
		}
	}
	
	void commitVictoryPathCommands()
	{
		for (vector<Block>::iterator it = blocks.begin(); it != blocks.end(); ++it)
		{
			(*it).commitVictoryPathCommands();
		}
	}
	
	void stageBuryPathCommands(int numBlocks)
	{
		int blockNum = 0;
		for (vector<Block>::iterator it = blocks.begin(); it != blocks.end(); ++it)
		{
			blockNum++;
			(*it).resetBuryData();
			if (blockNum <= numBlocks)
			{
				(*it).stageBuryPathCommands();
				if (blockNum == numBlocks)
				{
					(*it).stageBury();
				}
			}
		}
	}
	
	void commitBuryPathCommands()
	{
		for (vector<Block>::iterator it = blocks.begin(); it != blocks.end(); ++it)
		{
			(*it).commitBuryPathCommands();
		}
	}
	
	void stageBestScoreCommands(int droppedSkulls)
	{
		for (vector<Block>::iterator it = blocks.begin(); it != blocks.end(); ++it)
		{
			(*it).stageBestScoreCommands(droppedSkulls);
		}
	}
	
	void commitBestScoreCommands()
	{
		for (vector<Block>::iterator it = blocks.begin(); it != blocks.end(); ++it)
		{
			(*it).commitBestScoreCommands();
		}
	}
	
	void add(Block block, int index)
	{
		if (blocks.size() <= index)
		{
			blocks.push_back(block);
		}
	}

	int victoryTurns(int droppedSkulls)
	{
		int blockNum = 0;
		for (vector<Block>::iterator it = blocks.begin(); it != blocks.end(); ++it)
		{
			blockNum++;
			if ((*it).victoryTemp[droppedSkulls])
			{
				return blockNum;
			}
		}
		
		return 9999;
	}
	
	int buryTurns()
	{
		int blockNum = 0;
		for (vector<Block>::iterator it = blocks.begin(); it != blocks.end(); ++it)
		{
			blockNum++;
			if ((*it).buryTemp)
			{
				return blockNum;
			}
		}
		
		return 9999;
	}
	
	bool noCommands()
	{
		return blocks[0].noCommand();
	}

	Block front()
	{
		return blocks[0];
	}

	void popFront()
	{
		if (blocks.size() != 0)
		{
			// Remove first block
			blocks.erase(blocks.begin());
		}
	}

	friend ostream &operator<<(ostream &os, BlockQueue const &m)
	{
		for (vector<Block>::const_iterator it = m.blocks.begin(); it != m.blocks.end(); ++it)
		{
			os << (*it);
		}

		return os;
	}
};

class Square
{
	bool checked;
	bool grouped;

	public:
	int color;
	int x;
	int y;

	Square()
	{
		setAvailable();
	}
	
	Square(int _x, int _y)
	{
		setAvailable();
		x = _x;
		y = _y;
	}

	void update(char content)
	{
		color = -1;
		if (content != '.')
		{
			color = content - '0';
		}
	}

	void update(int content)
	{
		color = content;
	}
	
	void setAvailable()
	{
		color = -1;
		checked = false;
		grouped = false;
	}
	
	bool available() const
	{
		return color < 0;
	}
	
	void setChecked()
	{
		checked = true;
	}

	void resetChecked()
	{
		checked = false;
	}

	bool wasChecked() const
	{
		if (!checked && color > 0)
		{
			return false;
		}
		
		return true;
	}
	
	bool isSkull() const
	{
		return color == 0;
	}
	
	bool canGroup() const
	{
		return color > 0;
	}

	friend ostream &operator<<(ostream &os, Square const &m)
	{
		return os << colorOutput(m.color);
	}
};

class Group
{
	public:
	vector<Square> squares;
	int color;
	int blocks;
	int skulls;
	
	Group()
	{
		clear();
	}

	bool add(Square square)
	{
		if (color < 0)
		{
			color = square.color;
		}

		bool added = false;
		bool spread = false;

		if (!square.wasChecked() && square.color == color)
		{
			added = true;
			spread = true;
			blocks++;
		}
		else if (square.isSkull())
		{
			added = true;
			skulls++;
		}

		if (added)
		{
			squares.push_back(square);
		}
		
		return spread;
	}
	
	int groupBonus()
	{
		if (blocks >= 11)
		{
			return 8;
		}
		
		return blocks - 4;
	}
	
	void clear()
	{
		squares.clear();
		blocks = 0;
		skulls = 0;
		color = -1;
	}

	bool poppable()
	{
		return blocks >= 4;
	}
	
	void pop()
	{
		for (vector<Square>::iterator it = squares.begin(); it != squares.end(); ++it)
		{
			(*it).setAvailable();
		}
		
		squares.clear();
	}
};

class Grid
{
	private:
	vector<vector<Square> > squares;
	Group group;
	int score;
	
	public:
	int lowestHeight;
	int highestHeight;
	int spaceToFill;
	int spaceToBury;

	Grid()
	{
		for (int x = 0; x < GRID_WIDTH; ++x)
		{
			vector<Square> column;
			for (int y = 0; y < GRID_HEIGHT; ++y)
			{
				column.push_back(Square(x, y));
			}
			squares.push_back(column);
		}

		score = 0;
	}
	
    bool operator==(const Grid& other)
    {
		for (int x = 0; x < GRID_WIDTH; ++x)
		{
			for (int y = 0; y < GRID_HEIGHT; ++y)
			{
				if (squares[x][y].color != other.squares[x][y].color)
				{
					return false;
				}
			}
		}
		
		return true;
    }
	
    bool operator!=(const Grid& other)
    {
		return !(*this == other);
    }

	void updateRow(int row, string content)
	{
		for (int x = 0; x < GRID_WIDTH; ++x)
		{
			squares[x][row].update(content[x]);
		}
	}
	
	void fillColumnExtremes()
	{
		lowestHeight = 9999;
		highestHeight = 0;
		
		spaceToBury = 0;
		int previousHeight;
		for (int x = 0; x < GRID_WIDTH; ++x)
		{
			int height = columnHeight(x);

			if (height > highestHeight)
			{
				highestHeight = height;
			}
			
			if (height < lowestHeight)
			{
				lowestHeight = height;
			}
			
			if (x > 0)
			{
				int adjacentDistance = abs(height - previousHeight);
				if (adjacentDistance > spaceToBury)
				{
					spaceToBury = adjacentDistance;
				}
			}
			
			previousHeight = height;
		}
		
		spaceToFill = GRID_HEIGHT - lowestHeight;
	}

	bool checkSpace(int column, int spaceToLeave = 0) const
	{
		bool spaceAvailable = false;
		for (int y = GRID_HEIGHT-1; y >= spaceToLeave; --y)
		{
			if (squares[column][y].available())
			{
				spaceAvailable = true;
				break;
			}
		}
		
		return spaceAvailable;
	}
	
	int columnHeight(int column) const
	{
		for (int y = GRID_HEIGHT-1; y >= 0; --y)
		{
			if (squares[column][y].available())
			{
				return GRID_HEIGHT-y;
			}
		}
		
		return GRID_HEIGHT;
	}

	bool dropBlock(Move move, Block block)
	{
		bool placedA = false;
		bool placedB = false;
		
		for (int y = GRID_HEIGHT-1; (!placedA || !placedB) && y >= 0; --y)
		{
			switch (move.rotation)
			{
				case 0:
					if (!placedA && squares[move.x][y].available())
					{
						squares[move.x][y].update(block.colorA);
						placedA = true;
					}

					if (!placedB && squares[move.x+1][y].available())
					{
						squares[move.x+1][y].update(block.colorB);
						placedB = true;
					}
					break;
				case 1:
					if ((y > 0) && squares[move.x][y].available())
					{
						squares[move.x][y].update(block.colorA);
						squares[move.x][y-1].update(block.colorB);
						placedA = true;
						placedB = true;
					}
					break;
				case 2:
					if (!placedB && squares[move.x-1][y].available())
					{
						squares[move.x-1][y].update(block.colorB);
						placedB = true;
					}

					if (!placedA && squares[move.x][y].available())
					{
						squares[move.x][y].update(block.colorA);
						placedA = true;
					}
					break;
				case 3:
					if ((y > 0) && squares[move.x][y].available())
					{
						squares[move.x][y-1].update(block.colorA);
						squares[move.x][y].update(block.colorB);
						placedB = true;
						placedA = true;
					}
					break;
			}
		}
		
		return placedA && placedB;
	}
	
	void dropSkulls(int skullRows)
	{
		for (int x = 0; x < GRID_WIDTH; ++x)
		{
			int skullsLeft = skullRows;
			for (int y = GRID_HEIGHT-1; skullsLeft > 0 && y >= 0; --y)
			{
				if (squares[x][y].available())
				{
					squares[x][y].update(0);
					skullsLeft--;
				}
			}
		}
	}
	
	// Allow blocks to fall into open spaces
	void compress()
	{
		for (int x = 0; x < GRID_WIDTH; ++x)
		{
			int ySpace = -1;
			for (int y = GRID_HEIGHT-1; y >= 0; --y)
			{
				if (squares[x][y].available() && ySpace < 0)
				{
					ySpace = y;
				}
				else if (!squares[x][y].available() && ySpace >= 0)
				{
					squares[x][ySpace].update(squares[x][y].color);
					squares[x][y].setAvailable();
					--ySpace;
				}
			}
		}
	}

	bool addSquareToGroup(int x, int y)
	{
		if (group.add(squares[x][y]))
		{
			squares[x][y].setChecked();
			checkSquare(x, y);
		}
	}

	void checkSquare(int x, int y)
	{
		if (x > 0) addSquareToGroup(x-1, y);
		if (y > 0) addSquareToGroup(x, y-1);
		if (x < GRID_WIDTH-1) addSquareToGroup(x+1, y);
		if (y < GRID_HEIGHT-1) addSquareToGroup(x, y+1);
	}

	bool step(int CP)
	{
		int groups = 0;
		set<int> colors;
		int B = 0;
		int CB = 0;
		int GB = 0;

		for (int y = GRID_HEIGHT-1; y >= 0; --y)
		{
			for (int x = 0; x < GRID_WIDTH; ++x)
			{
				squares[x][y].resetChecked();
			}
		}
		
		bool anyPopped = false;
		for (int y = GRID_HEIGHT-1; y >= 0; --y)
		{
			for (int x = 0; x < GRID_WIDTH; ++x)
			{
				if (!squares[x][y].wasChecked() && squares[x][y].canGroup())
				{
					group.clear();

					addSquareToGroup(x, y);
					
					// If this is a poppable group
					if (group.poppable())
					{
						B += group.blocks;
						GB += group.groupBonus();
						colors.insert(group.color);

						for (vector<Square>::iterator it = group.squares.begin(); it != group.squares.end(); ++it)
						{
							squares[(*it).x][(*it).y].setAvailable();
						}
		
						groups++;
						anyPopped = true;
					}
				}
			}
		}

		switch (colors.size())
		{
			case 2:
				CB = 2;
				break;
			case 3:
				CB = 4;
				break;
			case 4:
				CB = 8;
				break;
			case 5:
				CB = 16;
				break;
			default:
				break;
		}

		int base = 10 * B;
		int bonuses = CP + CB + GB;
		if (bonuses < 1)
		{
			bonuses = 1;
		}
		else if (bonuses > 999)
		{
			bonuses = 999;
		}

		score += base * bonuses;

		return anyPopped;
	}

	int evaluate()
	{
		score = 0;
		int steps = 0;
		int CP = 0;
		while (step(CP))
		{
			compress();

			steps++;
			if (steps == 1)
			{
				CP = 8;
			}
			else
			{
				CP *= 2;
			}
		}
		
		return score;
	}
	
	friend ostream &operator<<(ostream &os, Grid const &m)
	{
		for (int y = 0; y < GRID_HEIGHT; ++y)
		{
			for (int x = 0; x < GRID_WIDTH; x++)
			{
				os << m.squares[x][y];
			}
			os << endl;
		}

		return os;
	}
};

class Reaction
{
	public:
	Move move;
	Grid grid;
	int score;
};

class Player
{
	public:
	int score;
	float skullPoints;
	Grid grid;
	vector<Reaction> reactions;

	Player()
	{
		score = 0;
		skullPoints = 0;
	}
	
	void addScore(int additional)
	{
		score += additional;
		skullPoints += (float)additional / SKULL_POINT_SCORE;
	}
	
	int updateSkullRows()
	{
		int skullRows = 0;
		while (skullPoints >= GRID_WIDTH)
		{
			skullPoints -= GRID_WIDTH;
			skullRows++;
		}
		
		return skullRows;
	}
	
	void dropSkulls(int skullRows)
	{
		for (vector<Reaction>::iterator it = reactions.begin(); it != reactions.end(); ++it)
		{
			(*it).grid.dropSkulls(skullRows);
		}
	}
	
	int findReactionScore()
	{
		for (vector<Reaction>::iterator it = reactions.begin(); it != reactions.end(); ++it)
		{
			if ((*it).grid == grid)
			{
				return (*it).score;
			}
		}
		
		return 0;
	}
	
	bool ableToDropSkulls(int addedScore)
	{
		int addedSkulls = (float)addedScore / SKULL_POINT_SCORE;
		if ((skullPoints + addedSkulls) > GRID_WIDTH)
		{
			return true;
		}
		
		return false;
	}
	
	int numSkullsDroppedWith(int addedScore)
	{
		int addedSkulls = (float)addedScore / SKULL_POINT_SCORE;
		return (skullPoints + addedSkulls) / GRID_WIDTH;
	}
	
	Reaction getRandomReaction()
	{
		return reactions[randomReaction(randomEngine)];
	}

	// Get all possible moves ready to be filled
	void fillAllReactions()
	{
		reactions.clear();
		for (int x = 0; x < GRID_WIDTH; ++x)
		{
			for (int rot = 0; rot < 4; ++rot)
			{
				if (rot == 0 && x == GRID_WIDTH-1)
				{
					continue;
				}
				if (rot == 2 && x == 0)
				{
					continue;
				}
				
				Reaction reaction;
				reaction.move = Move(x, rot);
				reaction.grid = Grid();
				reaction.score = 0;
				reactions.push_back(reaction);
			}
		}
	}
};

void initializeScoreCounters()
{
	bestScore.clear();
	bestScoreX3Height = -1;
}

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
	Player me;
	Player you;
	BlockQueue blocks;
	int turn = 0;
	int bestFirstBlockScore = 0;
	me.fillAllReactions();
	you.fillAllReactions();
	

    // game loop
    while (1)
	{
		turn++;
		double start = clock();
		double deadline = start + TIMEOUT;

		blocks.popFront();

		for (int i = 0; i < 8; i++)
		{
			int colorA; // color of the first block
			int colorB; // color of the attached block
			cin >> colorA >> colorB; cin.ignore();
			
			blocks.add(Block(colorA, colorB), i);
		}

		Grid inputGrid;
        for (int i = 0; i < 12; i++)
		{
            string row;
            cin >> row; cin.ignore();

			inputGrid.updateRow(i, row);
        }
		inputGrid.fillColumnExtremes();

        for (int i = 0; i < 12; i++)
		{
            string row; // One line of the map ('.' = empty, '0' = skull block, '1' to '5' = colored block)
            cin >> row; cin.ignore();

			you.grid.updateRow(i, row);
        }
		you.grid.fillColumnExtremes();
		
		
		// If grid isn't as expected (probably got skulled)
		if (me.grid != inputGrid)
		{
			initializeScoreCounters();
			blocks.resetCommands();
			me.grid = inputGrid;
		}

		// If no command for the next block, start over on accumulating moves
		if (blocks.noCommands())
		{
			initializeScoreCounters();
			blocks.resetCommands();
		}
		Block frontBlock = blocks.front();

		// Check your score based on last turn's result
		int yourAddedScore = you.findReactionScore();
		you.addScore(yourAddedScore);
		you.updateSkullRows();
		int yourSkullsDropped = you.numSkullsDroppedWith(yourAddedScore);
		blocks.updateStaging(yourSkullsDropped);

		// Check your possible next moves
		int yourBestScore = 0;
        set<int> yourPossibleDrops;
		yourPossibleDrops.insert(0);
        for (vector<Reaction>::iterator reaction = you.reactions.begin(); reaction != you.reactions.end(); ++reaction)
		{
			(*reaction).grid = you.grid;
			(*reaction).score = 0;
			if ((*reaction).grid.dropBlock((*reaction).move, frontBlock))
			{
				int score = (*reaction).grid.evaluate();
				(*reaction).score = score;
				if (score > yourBestScore)
				{
					yourBestScore = score;
				}

    			yourPossibleDrops.insert(you.numSkullsDroppedWith(score));
			}
		}

		// Check my possible next moves
		bestFirstBlockScore = -1;
		for (vector<Reaction>::iterator reaction = me.reactions.begin(); reaction != me.reactions.end(); ++reaction)
		{
			(*reaction).grid = me.grid;
			(*reaction).score = -1;
			if ((*reaction).grid.dropBlock((*reaction).move, frontBlock))
			{
				int score = (*reaction).grid.evaluate();
				(*reaction).score = score;
				if (score > bestFirstBlockScore)
				{
					bestFirstBlockScore = score;
					(*(blocks.begin())).temp = (*reaction).move;
					blocks.stageSkullNowCommands();
				}
			}
		}

		int trials = 0;
		map<int, int> victoryTurns;
		int buryTurns = blocks.buryTurns();
		while (clock() < deadline)
		{
			trials++;
			int score = 0;
			int trueScore = 0;
			Reaction check = me.getRandomReaction();

			int blockNum = 0;
			uniform_int_distribution<> randomSkulls(0, yourPossibleDrops.size());
			int testSkulls = randomSkulls(randomEngine);
			int skulls;

			// Randomly drop each block
			for (vector<Block>::iterator it = blocks.begin(); it != blocks.end(); ++it)
			{
				blockNum++;
				int thisMoveScore = 0;

				// First block is already calculated so use it
				if (blockNum == 1)
				{
					thisMoveScore = check.score;
					// If invalid move
					if (thisMoveScore < 0)
					{
						break;
					}
					
					(*(blocks.begin())).temp = check.move;
					skulls = 0;
				}
				else
				{
					(*it).randomizeTemp();
					if (check.grid.dropBlock((*it).temp, (*it)))
					{
						thisMoveScore = check.grid.evaluate();
					}
					else
					{
						// Impossible to do this drop
						(*it).resetTempData();
						break;
					}
					skulls = testSkulls;
				}

				trueScore += thisMoveScore;
				score += thisMoveScore * pow(2, (9-blockNum)) + me.numSkullsDroppedWith(trueScore) * 1000;

				// Find fastest victory path
				if ((me.numSkullsDroppedWith(trueScore) > you.grid.spaceToFill) && blockNum < victoryTurns[skulls])
				{
					blocks.stageVictoryPathCommands(skulls, blockNum);
					victoryTurns[skulls] = blocks.victoryTurns(skulls);
				}

				// Find best possible score
				if (score > bestScore[skulls])
				{
					bestScore[skulls] = score;
					bestScoreX3Height = check.grid.columnHeight(3);
					blocks.stageBestScoreCommands(skulls);
				}
				else if ((score == bestScore[skulls]) && (bestScoreX3Height < check.grid.columnHeight(3)))
				{
					// Increase height of column 3 if it makes no difference to the score
					bestScore[skulls] = score;
					bestScoreX3Height = check.grid.columnHeight(3);
					blocks.stageBestScoreCommands(skulls);
				}
			}
		}

		// Determine strategy
		string comment = "";
		victoryTurns[0] = blocks.victoryTurns(0);
		buryTurns = blocks.buryTurns();
		bool youCanDropBlocks = you.ableToDropSkulls(yourBestScore);
		if (victoryTurns[0] == 1)
		{
			comment = " I can beat you with my eyes closed!";
			blocks.commitVictoryPathCommands();
		}
		else if (youCanDropBlocks && me.numSkullsDroppedWith(bestFirstBlockScore) > you.grid.spaceToBury)
		{
			comment = " Dig this!";
			blocks.commitSkullNowCommands();
		}
		else if (victoryTurns[0] <= 8)
		{
			comment = " " + to_string(victoryTurns[0]-1) + "!";
			blocks.commitVictoryPathCommands();
		}
		else if (youCanDropBlocks && me.ableToDropSkulls(bestFirstBlockScore))
		{
			comment = " I'll do better next time.";
			blocks.commitSkullNowCommands();
		}
		else
		{
			blocks.commitBestScoreCommands();
		}
		
		// Do this move
		Grid finalCheck = me.grid;
		Move outputMove;
		if (finalCheck.dropBlock(blocks.front().assigned, blocks.front()))
		{
			me.grid.dropBlock(blocks.front().assigned, blocks.front());
			outputMove = blocks.front().assigned;
		}
		else
		{
			bool moveFound = false;
			cerr << "PANIC!" << endl;
			// Drop failed because something went wrong!
			for (vector<Reaction>::iterator reaction = me.reactions.begin(); reaction != me.reactions.end(); ++reaction)
			{
				// Use the first valid move
				finalCheck = me.grid;
				if (finalCheck.dropBlock((*reaction).move, blocks.front()))
				{
					me.grid.dropBlock((*reaction).move, blocks.front());
					moveFound = true;
					outputMove = (*reaction).move;
					break;
				}
			}
			
			if (!moveFound)
			{
				comment = " crap";
			}
		}
		me.addScore(me.grid.evaluate());
		you.dropSkulls(me.updateSkullRows());

		// Debug output
		cerr << " turn:" << turn << " score:" << me.score << " youscore:" << you.score << " yourBestScore:" << yourBestScore << " victoryTurns[0]:" << victoryTurns[0] << " buryTurns:" << buryTurns << " bestScore[0]:" << bestScore[0] << " bestFirstBlockScore:" << bestFirstBlockScore << " trials:" << trials << " elapsed: " << (clock() - start) << endl;

		// Output
		cout << outputMove << comment << endl;
    }
}