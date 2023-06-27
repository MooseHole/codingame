#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>
#include <limits>
#include <random>
#include <map>
#include <unordered_map>

using namespace std;

#define CIRCLE_DEGREES 360				// The number of degrees in a circle
#define RAD_TO_DEG 57.2958279			//180.0f/3.14159f		// Used to convert radians to degrees
#define DEG_TO_RAD 0.01745328			//3.14159f/180.0f		// Used to convert degrees to radians

#define MIN_POWER 0
#define MAX_POWER 4
#define MAX_ROTATE 15
#define MIN_ROTATE -15
#define MAX_ROTATION 90
#define MIN_ROTATION -90
#define LANDING_H_SPEED 20
#define LANDING_V_SPEED 40
#define LANDING_ROTATION 0
#define BOUNDS_X_MIN 0
#define BOUNDS_Y_MIN 0
#define BOUNDS_X_MAX 7000
#define BOUNDS_Y_MAX 3000
#define GRAVITY 3.711

#define TARGET_SCALAR 0.9
#define TIMEOUT 0.100

#define CRASH_CHECK_STEPS 25
#define CRASH_DISTANCE_SCORE 1
#define CRASH_AVOIDED_SCORE 10000000;
#define TARGET_DISTANCE_X_SCORE -10;
#define TARGET_DISTANCE_Y_SCORE -5;

// A coordinate holds position.  It can also be used for vector calculations
class Coordinate
{
public:
	double x;
	double y;

	Coordinate() : x(-1), y(-1)
	{}
    
	Coordinate(double x, double y) : x(x), y(y)
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
	double distance(Coordinate to)
	{
		return sqrt(squaredDistance(to));
	}
	
	// The squared Eucledean distance between this coordinate and the input
	// Avoids using sqrt
	double squaredDistance(Coordinate to)
	{
		return (pow(to.x - x, 2) + pow(to.y - y, 2));
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

	Coordinate operator*(const double scalar)
	{
		return Coordinate(x * scalar, y * scalar);
	}

	Coordinate operator/(const double scalar)
	{
		if (scalar == 0)
		{
			return Coordinate(0,0);
		}

		return Coordinate(x / scalar, y / scalar);
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
	Coordinate pointFromAngle(double angle)
	{
		double radianAngle = angle * DEG_TO_RAD;
		return Coordinate(1000 * cos(radianAngle) + x, 1000 * sin(radianAngle) + y);
	}

	void update(double _x, double _y)
	{
		x = _x;
		y = _y;
	}

	
	//// VECTOR JUNK ////
	
	// This gives the magnitude of the vector
	double magnitude()
	{
		return sqrt(pow(x, 2) + pow(y, 2));		
	}

	// Returns the difference angle
	double angleTo(Coordinate other)
	{
		Coordinate diff = *this - other;
		return diff.angle();
	}
	
	// Returns the angle of a vector in degrees
	double angle()
	{
		return fmod(((atan2(y, x)) * RAD_TO_DEG), CIRCLE_DEGREES);
	}
	
	double dotProduct(Coordinate other)
	{
		return (x * other.x) + (y * other.y);
	}

	friend ostream &operator<<(ostream &os, Coordinate const &m)
	{
		return os << "(" << m.x << "," << m.y << ")";
	}
};


class Segment
{
	public:
	Coordinate points[2];
	Coordinate center;

	Segment(){}

	Segment(Coordinate A, Coordinate B)
	{
		points[0] = A;
		points[1] = B;
		updateCenter();
	}
	
	Segment& operator=(const Segment& other)
	{
		points[0] = other.points[0];
		points[1] = other.points[1];
		center = other.center;
		return *this;
	}
	
	bool operator==(const Segment& other)
	{
		return points[0] == other.points[0] && points[1] == other.points[1];
	}
	
	bool isFlat()
	{
		return points[0].y == points[1].y;
	}
		
	void scale(double scalar)
	{
		double width = points[1].x - points[0].x;
		double adjust = width * (1.0-scalar);
		points[0].y += adjust;
		points[1].x -= adjust;
	}

    // Given three collinear points p, q, r, the function checks if
    // point q lies on line segment 'pr'
    bool onSegment(Coordinate p, Coordinate q, Coordinate r)
    {
        return (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
                q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y));
    }

    // To find orientation of ordered triplet (p, q, r).
    // The function returns following values
    // 0 --> p, q and r are collinear
    // 1 --> Clockwise
    // 2 --> Counterclockwise
    int orientation(Coordinate p, Coordinate q, Coordinate r)
    {
        int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
    
        if (val == 0)
        {
            return 0;  // collinear
        }
    
        return (val > 0) ? 1: 2; // clock or counterclock wise
    }

    // The main function that returns true if line segment 'p1q1'
    // and 'p2q2' intersect.
    bool doIntersect(Coordinate p1, Coordinate q1, Coordinate p2, Coordinate q2)
    {
        // Find the four orientations needed for general and
        // special cases
        int o1 = orientation(p1, q1, p2);
        int o2 = orientation(p1, q1, q2);
        int o3 = orientation(p2, q2, p1);
        int o4 = orientation(p2, q2, q1);
    
        // General case
        if (o1 != o2 && o3 != o4)
            return true;
    
        // Special Cases
        // p1, q1 and p2 are collinear and p2 lies on segment p1q1
        if (o1 == 0 && onSegment(p1, p2, q1)) return true;
    
        // p1, q1 and q2 are collinear and q2 lies on segment p1q1
        if (o2 == 0 && onSegment(p1, q2, q1)) return true;
    
        // p2, q2 and p1 are collinear and p1 lies on segment p2q2
        if (o3 == 0 && onSegment(p2, p1, q2)) return true;
    
        // p2, q2 and q1 are collinear and q1 lies on segment p2q2
        if (o4 == 0 && onSegment(p2, q1, q2)) return true;
    
        return false; // Doesn't fall in any of the above cases
    }

    bool doIntersect(Segment first, Segment second)
    {
        return doIntersect(first.points[0], first.points[1], second.points[0], second.points[1]);
    }

    bool intersects(Segment test)
    {
        return doIntersect(test, *this);
    }

    bool isUnder(Coordinate test)
    {
        return test.x > points[0].x && test.x < points[1].x;
    }

	friend ostream &operator<<(ostream &os, Segment const &m)
	{
		return os << "[0]" << m.points[0] << "{c}" << m.center << "[1]" << m.points[1];
	}

private:
	void updateCenter()
	{
		center = Coordinate(points[0].x + (points[1].x - points[0].x / 2), points[0].y + (points[1].y - points[0].y / 2));
	}
};


class Surface
{
	private:
	vector<Segment> segments;
	
	public:
	Surface()
	{
	}
	
	void addSegment(Segment segment)
	{
		segments.push_back(segment);
	}

    // Less than 0 means no crash
    double crashDistance(Segment path)
    {
        for (std::vector<Segment>::iterator it = segments.begin(); it != segments.end(); ++it)
        {
            if (path.intersects(*it))
            {
                return path.points[1].distance(it->center);
            }
        }

        return -1;
    }
};

Surface surface;

class Lander
{
public:
	Coordinate location;
	Coordinate velocity;
	int fuel;
	int rotation; // Degrees
	int power;
	Segment target;

	Lander()
	{
    }

	Lander& operator=(const Lander& other)
	{
		location = other.location;
		velocity = other.velocity;
        fuel = other.fuel;
        rotation = other.rotation;
        power = other.power;
        target = other.target;
		return *this;
	}

	void updateTarget(Segment _target)
	{
		target = _target;
	}

	void update(int X, int Y, int hSpeed, int vSpeed, int _fuel, int _rotation, int _power)
	{
		location.update(X, Y);
		velocity.update(hSpeed, vSpeed);
		fuel = _fuel;
		rotation = _rotation;
		power = _power;
	}

	void increasePower()
	{
		if (power < MAX_POWER)
		{
			power += 1;
		}
	}

	void decreasePower()
	{
		if (power > MIN_POWER)
		{
			power -= 1;
		}
	}

	void rotate(int degrees)
	{
		if (degrees > MAX_ROTATE)
		{
			degrees = MAX_ROTATE;
		}
		else if (degrees < MIN_ROTATE)
		{
			degrees = MIN_ROTATE;
		}

		rotation += degrees;
		if (rotation > MAX_ROTATION)
		{
			rotation = MAX_ROTATION;
		}
		else if (rotation < MIN_ROTATION)
		{
			rotation = MIN_ROTATION;
		}
	}

	void goVertical()
	{
		rotate(0 - rotation);
	}

	bool landingState()
	{
		return rotation == 0
			&& abs(velocity.x) <= 20
			&& abs(velocity.y) <= 40;
	}

	void findSolution()
	{
        simulate();
	}

    Segment step()
    {
        Coordinate oldLocation = location;

        if (fuel < power)
        {
            power = fuel;
        }

        fuel -= power;
        velocity.y -= GRAVITY;
        velocity.y += power * cos((double)-rotation * DEG_TO_RAD);
        velocity.x += power * sin((double)-rotation * DEG_TO_RAD);

        location += velocity;
        return Segment(oldLocation, location);
    }

    double stepCrash()
    {
        return surface.crashDistance(step());
    }

    double distanceToCrash(int maxSteps)
    {
        int steps = 0;
        double lastStep = -1;
        while (steps < maxSteps && lastStep < 0)
        {
            lastStep = stepCrash();
            steps++;
        }

        return lastStep;
    }

    double distanceToTarget()
    {
        location.distance(target.center);
    }

    void score()
    {
        int crashScore = CRASH_AVOIDED_SCORE;
        if (!target.isUnder(location))
        {
            int crashStepMax = CRASH_CHECK_STEPS;
            int crashMultiplier = CRASH_DISTANCE_SCORE;
            Lander crasher = *this;
            double crashSteps = crasher.distanceToCrash(crashStepMax + 1);
            if (crashSteps >= 0)
            {
                crashScore = crashSteps * crashMultiplier;
            }
        }

        Lander mover = *this;
        mover.step();
        int distanceX = abs(mover.location.x - target.center.x);
        int distanceY = abs(mover.location.y - target.center.y);

        myScore = crashScore + distanceX * TARGET_DISTANCE_X_SCORE + distanceY * TARGET_DISTANCE_Y_SCORE;
    }

    int simulate()
    {
        // W = rotate counter clockwise (widdershins)
        // D = rotate clockwise (deosil)
        // S = slower
        // F = faster

        map<string, Lander> tests;
        tests[""] = *this;
        tests["W"] = *this;
        tests["D"] = *this;
        tests["S"] = *this;
        tests["F"] = *this;
        tests["WS"] = *this;
        tests["WF"] = *this;
        tests["DS"] = *this;
        tests["DF"] = *this;

        for (std::map<string, Lander>::iterator it = tests.begin(); it != tests.end(); ++it)
        {
            if (it->first.find('W') != string::npos)
            {
                it->second.rotate(MAX_ROTATE);
            }
            else if (it->first.find('D') != string::npos)
            {
                it->second.rotate(MIN_ROTATE);
            }

            if (it->first.find('S') != string::npos)
            {
                it->second.decreasePower();
            }
            else if (it->first.find('F') != string::npos)
            {
                it->second.increasePower();
            }
        }

        int bestScore = -10000000;
        string bestTest = "";
        int numWithBestScore = 0;
        for (std::map<string, Lander>::iterator it = tests.begin(); it != tests.end(); ++it)
        {
            it->second.score();

            cerr << "test:" << it->first << " score:" << it->second.myScore;

            if (it->second.myScore > bestScore)
            {
                bestScore = it->second.myScore;
                bestTest = it->first;
                cerr << " BEST";
            }

            cerr << endl;
        }

        rotation = tests[bestTest].rotation;
        power = tests[bestTest].power;
    }

private:
    int myScore;
};

unordered_map<int, pair<double, double>> trigAngle;

pair<double, double> getTrigAngle(int angle)
{
	pair<double, double> myTrigAngle;
	unordered_map<int, pair<double, double>>::iterator it;
	it = trigAngle.find(angle);
	if (it == trigAngle.end())
	{
		myTrigAngle = pair<double, double>(sin((double)-angle * DEG_TO_RAD), cos((double)-angle * DEG_TO_RAD));
		trigAngle.insert(pair<int, pair<double, double>>(angle, myTrigAngle));
	}
	else
	{
		myTrigAngle = it->second;
	}
	
	return myTrigAngle;
}


int main()
{
	Lander lander;

	int surfaceN; // the number of points used to draw the surface of Mars.
	cin >> surfaceN; cin.ignore();

	Coordinate previous, current;
	for (int i = 0; i < surfaceN; i++)
	{
		int landX; // X coordinate of a surface point. (0 to 6999)
		int landY; // Y coordinate of a surface point. By linking all the points together in a sequential fashion, you form the surface of Mars.
		cin >> landX >> landY; cin.ignore();
		
		current.x = landX;
		current.y = landY;
		if (i > 0)
		{
			Segment thisSegment = Segment(previous, current);
			surface.addSegment(thisSegment);
			if (thisSegment.isFlat())
			{
				Segment target = thisSegment;
				target.scale(TARGET_SCALAR);
				lander.updateTarget(target);
			}
		}

		previous = current;
	}

    // Add boundaries to surface
    surface.addSegment(Segment(Coordinate(BOUNDS_X_MIN, BOUNDS_Y_MIN), Coordinate(BOUNDS_X_MIN, BOUNDS_Y_MAX)));
    surface.addSegment(Segment(Coordinate(BOUNDS_X_MIN, BOUNDS_Y_MAX), Coordinate(BOUNDS_X_MAX, BOUNDS_Y_MAX)));
    surface.addSegment(Segment(Coordinate(BOUNDS_X_MAX, BOUNDS_Y_MAX), Coordinate(BOUNDS_X_MAX, BOUNDS_Y_MIN)));
    surface.addSegment(Segment(Coordinate(BOUNDS_X_MAX, BOUNDS_Y_MIN), Coordinate(BOUNDS_X_MIN, BOUNDS_Y_MIN)));


	bool initialized = false;

	int X;
	int Y;
	int hSpeed;
	int vSpeed;
	int fuel;
	int rotation;
	int power;

	// game loop
	while (1)
	{
		cin >> X >> Y >> hSpeed >> vSpeed >> fuel >> rotation >> power; cin.ignore();
		lander.update(X, Y, hSpeed, vSpeed, fuel, rotation, power);

		lander.findSolution();

		cout << lander.rotation << " " << lander.power << endl;
	}
}
