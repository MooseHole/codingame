#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <map>
#include <algorithm>

using namespace std;

class Point
{
	public:
	int x;
	int y;

    Point()
    {
        x = -1;
        y = -1;
    }

	Point(int _x, int _y)
	{
		x = _x;
		y = _y;
	}
	
	bool operator==(const Point &other) const
	{
		return x == other.x && y == other.y;
	}
	
	bool operator<(const Point &other) const
	{
		return 100000000 + x * 10000 + y < 100000000 + other.x * 10000 + other.y;
	}
};

class Ball
{
	private:
	void commit(char direction)
	{
		for(int i = 0; i < shotCount; i++)
		{
			path.push_back(direction);
		}
		shotCount--;
	}

	void uncommit()
	{
		if (!path.empty())
		{
			for(int i = 0; i < shotCount; i++)
			{
				path.pop_back();
			}
			shotCount++;
		}
	}
	
	vector<Point> strokePoints(Point begin, char direction, int amount)
	{
		switch(direction)
		{
			case '>':
				return strokePoints(begin, Point(begin.x + amount, begin.y));
			case '<':
				return strokePoints(begin, Point(begin.x - amount, begin.y));
			case '^':
				return strokePoints(begin, Point(begin.x, begin.y - amount));
			case 'v':
				return strokePoints(begin, Point(begin.x, begin.y + amount));
			default:
				return vector<Point>();
		}
	}

	vector<Point> strokePoints(Point begin, Point end)
	{
		vector<Point> points;
		// If up
		if ((begin.x == end.x) && (begin.y > end.y))
		{
			for (int i = 0; i < begin.y - end.y; i++)
			{
				points.push_back(Point(begin.x, begin.y - i));
			}
		}
		// If down
		if ((begin.x == end.x) && (begin.y < end.y))
		{
			for (int i = 0; i < end.y - begin.y; i++)
			{
				points.push_back(Point(begin.x, begin.y + i));
			}
		}
		// If right
		if ((begin.x < end.x) && (begin.y == end.y))
		{
			for (int i = 0; i < end.x - begin.x; i++)
			{
				points.push_back(Point(begin.x + i, begin.y));
			}
		}
		// If left
		if ((begin.x > end.x) && (begin.y == end.y))
		{
			for (int i = 0; i < begin.x - end.x; i++)
			{
				points.push_back(Point(begin.x - i, begin.y));
			}
		}
		
		return points;
	}

	public:
	Point startPoint;
	int startShotCount;
	Point currentPoint;
	int shotCount;
	vector<char> path;
	
	Ball(int x, int y, int _shotCount)
	{
		startPoint = Point(x, y);
		currentPoint = Point(x, y);
		startShotCount = _shotCount;
		shotCount = _shotCount;
	}

	// Assume begin and end have same x or y
	bool crossPath(Point begin, Point end)
	{
		Point pathPoint = startPoint;
		int pathShots = startShotCount;
		vector<Point> checkPoints = strokePoints(begin, end);
		for(vector<char>::iterator stroke = path.begin(); stroke != path.end(); ++stroke)
		{
			vector<Point> strokePathPoints = strokePoints(pathPoint, (*stroke), pathShots--);
			vector<Point> checkPoints = strokePoints(begin, end);
			for(vector<Point>::iterator checkPoint = checkPoints.begin(); checkPoint != checkPoints.end(); ++checkPoint)
			{				
				for(vector<Point>::iterator strokePoint = strokePathPoints.begin(); strokePoint != strokePathPoints.end(); ++strokePoint)
				{
					if ((*strokePoint) == (*checkPoint))
					{
						return true;
					}
				}
			}
		}
		
		return false;
	}
	
	Point left(Point start, int count)
	{
		return Point(start.x - count, start.y);
	}

	Point left(bool doCommit = false)
	{
		Point endPoint = Point(currentPoint.x - shotCount, currentPoint.y);
		if (doCommit)
		{
			commit('<');
		}
		return endPoint;
	}
	Point right(bool doCommit = false)
	{
		Point endPoint = Point(currentPoint.x + shotCount, currentPoint.y);
		if (doCommit)
		{
			commit('>');
		}
		return endPoint;
	}
	Point up(bool doCommit = false)
	{
		Point endPoint = Point(currentPoint.x, currentPoint.y - shotCount);
		if (doCommit)
		{
			commit('^');
		}
		return endPoint;
	}
	Point down(bool doCommit = false)
	{
		Point endPoint = Point(currentPoint.x, currentPoint.y + shotCount);
		if (doCommit)
		{
			commit('v');
		}
		return endPoint;
	}
	
};

class Grid
{
	public:
	vector<string> locations;
	int width;
	int height;
	vector<Ball> balls;
	map<Point, int> holes; // -1 empty, 0 does not exist, 1 full
	Grid(int _width, int _height)
	{
		width = _width;
		height = _height;
		for (int x = 0; x < width; x++)
		{
			locations.push_back("");
		}
	}
	
	bool legal(Point begin, Point end)
	{
		if (end.x < 0 || end.y < 0 || end.x > width || end.y > height)
		{
			return false;
		}
		
		if (locations[end.x][end.y] == 'X')
		{
			return false;
		}
		
		if ((locations[end.x][end.y] == 'H') && (holes[end] >= 0))
		{
			return false;
		}
		
		for(vector<Ball>::iterator ball = balls.begin(); ball != balls.end(); ++ball)
		{
			if (ball->crossPath(begin, end))
			{
				return false;
			}
		}

		return true;
	}
	
	void addRow(string row)
	{
		for (int x = 0; x < width; x++)
		{
			if (row[x] == 'H')
			{
				holes[Point(x, locations[x].length())] = -1;
			}
			else if (row[x] >= '0' && row[x] <= '9')
			{
				balls.push_back(Ball(x, locations[x].length(), row[x] - '0'));
			}

			locations[x] += row[x];
		}
	}
	
	bool solve()
	{
		for(vector<Ball>::iterator ball = balls.begin(); ball != balls.end(); ++ball)
		{
			if (legal(ball->currentPoint, ball->right()))
			{
				ball->right(true);
			}
			if (holes[ball->currentPoint] == -1)
			{
				holes[ball->currentPoint] = 1;
			}
		}
		
		bool solved = true;
		for(map<Point, int>::iterator hole = holes.begin(); hole != holes.end(); ++hole)
		{
			if (hole->second == -1)
			{
				solved = false;
				break;
			}
		}
		
		return solved;
	}
	
	string printAnswer() const
	{
		string answer = "";
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; x++)
			{
				answer += ".";
			}
			
			answer += "\n";
		}
		
		return answer;
	}

	friend ostream &operator<<(ostream &os, Grid const &m)
	{
		for (int y = 0; y < m.height; ++y)
		{
			for (int x = 0; x < m.width; x++)
			{
				os << m.locations[x][y];
			}
			os << endl;
		}

		return os;
	}
};

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
	int width;
	int height;
	cin >> width >> height; cin.ignore();
	Grid grid(width, height);
	cerr << width << " " << height << endl;
	for (int i = 0; i < height; i++)
	{
        string row;
        cin >> row; cin.ignore();
		grid.addRow(row);
    }

	grid.solve();
	cerr << grid << endl;
	cout << grid.printAnswer() << endl;
}