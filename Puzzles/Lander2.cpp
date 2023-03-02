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

#define ROTATION_MIN -90
#define ROTATION_MAX 90
#define ROTATION_SPEED 15
#define ROTATION_ADJUST_MIN -15
#define ROTATION_ADJUST_MAX 15
#define ROTATION_RANGE 31
#define THRUST_MIN 0
#define THRUST_MAX 4
#define THRUST_SPEED 1
#define THRUST_ADJUST_MIN -1
#define THRUST_ADJUST_MAX 1
#define THRUST_RANGE 3
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

#define SCORE_DISTANCE_X		-5
#define SCORE_DISTANCE_Y		-1
#define SCORE_FUEL				1
#define SCORE_LANDING_H_SPEED	-1000
#define SCORE_LANDING_V_SPEED	-1000
#define SCORE_LANDING_ROTATE	-10000
#define SCORE_CRASHED			-100000
#define SCORE_LANDED			100000

//static default_random_engine randomEngine(clock());
//static ranlux24 randomEngine(clock());
static minstd_rand randomEngine(clock());
static uniform_int_distribution<int> randomRotation{ROTATION_ADJUST_MIN, ROTATION_ADJUST_MAX};
static uniform_int_distribution<int> randomThrust{THRUST_ADJUST_MIN, THRUST_ADJUST_MAX};
static uniform_int_distribution<int> randomName{0, 10000};
static uniform_real_distribution<double> randomDecimal{0, 1};

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
};

class Lander
{
public:
	Coordinate location;
	Coordinate vector;
	int fuel;
	int rotation; // Degrees
	int power;
	Segment target;


	Lander()
	{}

	void updateTarget(Segment _target)
	{
		target = _target;
	}

	void update(int X, int Y, int hSpeed, int vSpeed, int _fuel, int _rotation, int _power)
	{
		location.update(X, Y);
		vector.update(hSpeed, vSpeed);
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
			&& abs(vector.x) <= 20
			&& abs(vector.y) <= 40;
	}

	void findSolution()
	{
cerr << "findSolution location=" << location << " target=" << target << endl;
		if (target.points[0].x > location.x)
		{
			rotate(-15);
			increasePower();
		}
		else if (target.points[1].x < location.x)
		{
			rotate(15);
			increasePower();
		}
		else
		{
			goVertical();
			decreasePower();
		}
	}

private:
	const int MIN_POWER = 0;
	const int MAX_POWER = 4;
	const int MAX_ROTATE = 15;
	const int MIN_ROTATE = -15;
	const int MAX_ROTATION = 90;
	const int MIN_ROTATION = -90;
	
};

Surface surface;

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
