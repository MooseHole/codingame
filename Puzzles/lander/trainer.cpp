#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>

using namespace std;

#define CIRCLE_DEGREES 360				// The number of degrees in a circle
#define RAD_TO_DEG 180.0f/3.14159f		// Used to convert radians to degrees
#define DEG_TO_RAD 3.14159f/180.0f		// Used to convert degrees to radians

#define LANDING_H_SPEED 20
#define LANDING_V_SPEED 40
#define BOUNDS_X_MIN 0
#define BOUNDS_Y_MIN 0
#define BOUNDS_X_MAX 7000
#define BOUNDS_Y_MAX 3000

// A coordinate holds position.  It can also be used for vector calculations
class Coordinate
{
    public:
    float x;
    float y;
    
    Coordinate() : x(-1), y(-1)
    {}
    
    Coordinate(float x, float y) : x(x), y(y)
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
    float distance(Coordinate to)
    {
        return sqrt(squaredDistance(to));
    }
	
    // The squared Eucledean distance between this coordinate and the input
	// Avoids using sqrt
    float squaredDistance(Coordinate to)
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

	Coordinate operator*(const float scalar)
	{
		return Coordinate(x * scalar, y * scalar);
	}

	Coordinate operator/(const float scalar)
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
	Coordinate pointFromAngle(float angle)
	{
		float radianAngle = angle * DEG_TO_RAD;
		return Coordinate(1000 * cos(radianAngle) + x, 1000 * sin(radianAngle) + y);
	}
	
	//// VECTOR JUNK ////
	
	// This gives the magnitude of the vector
	float magnitude()
	{
        return sqrt(pow(x, 2) + pow(y, 2));		
	}

	float dotProduct(Coordinate other)
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
	private:

	public:
	Coordinate points[2];

	Segment(){}

	Segment(Coordinate A, Coordinate B)
	{
		points[0] = A;
		points[1] = B;
	}
	
	bool intersects(Segment other)
	{
		Coordinate s1, s2;
		s1.x = points[1].x - points[0].x;     s1.y = points[1].y - points[0].y;
		s2.x = other.points[1].x - other.points[0].x;     s2.y = other.points[1].y - other.points[0].y;

		float denominator = -s2.x * s1.y + s1.x * s2.y;
		if (denominator != 0)
		{
			float s = (-s1.y * (points[0].x - other.points[0].x) + s1.x * (points[0].y - other.points[0].y)) / denominator;
			if (s >= 0 && s <= 1)
			{
				float t = ( s2.x * (points[0].y - other.points[0].y) - s2.y * (points[0].x - other.points[0].x)) / denominator;
				if (t >= 0 && t <= 1)
				{
					// Collision detected
					return true;
				}
			}
		}

		return false; // No collision
	}
	
	bool isFlat()
	{
		return points[0].y == points[1].y;
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
	
	addSegment(Segment segment)
	{
		segments.push_back(segment);
	}
	
	bool collide(Segment path, Segment* collided = NULL)
	{
		for (vector<Segment>::iterator segment = segments.begin(); segment != segments.end(); ++segment)
		{
			if ((*segment).intersects(path))
			{
				collided = &(*segment);
				return true;
			}
		}
		
		return false;
	}
};

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cerr << "trainer: Need filename for " << argv[0] << endl;
		exit(-1);
	}
	
	string filename = argv[1];
    ifstream setupIn(filename.c_str());
	
	ofstream out;
	do
	{
		out.open("trainerOutput.txt");
	} while (out.fail());
	
	int width, height;
	float gravity;
	float unknown1, unknown2;
	int unknown3;
	int minThrust, maxThrust;
	int minAngle, maxAngle;
	setupIn >> width >> height >> gravity >> unknown1 >> unknown2 >> unknown3 >> minThrust >> maxThrust >> minAngle >> maxAngle; setupIn.ignore();

	int landingHSpeed, landingVSpeed;
	int unknown6, unknown7;
	string unknown8;
	int maxRotateSpeed, maxPowerSpeed;
	setupIn >> landingHSpeed >> landingVSpeed >> unknown6 >> unknown7 >> unknown8 >> maxRotateSpeed >> maxPowerSpeed; setupIn.ignore();

    int surfaceN; // the number of points used to draw the surface of Mars.
    setupIn >> surfaceN; setupIn.ignore();
	out << to_string(surfaceN) << endl;

	Surface surface;
	Coordinate previous, current;
    for (int i = 0; i < surfaceN; i++) {
        int landX; // X coordinate of a surface point. (0 to width-1)
        int landY; // Y coordinate of a surface point. By linking all the points together in a sequential fashion, you form the surface of Mars.
        setupIn >> landX >> landY; setupIn.ignore();
		out << landX << " " << landY << endl;

		current.x = landX;
		current.y = landY;
		if (i > 0)
		{
			surface.addSegment(Segment(previous, current));
		}
		
		previous = current;
    }

    float X;
    float Y;
    float hSpeed; // the horizontal speed (in m/s), can be negative.
    float vSpeed; // the vertical speed (in m/s), can be negative.
    int fuel; // the quantity of remaining fuel in liters.
    int rotate; // the rotation angle in degrees (-90 to 90).
    int power; // the thrust power (0 to 4).
    setupIn >> X >> Y >> hSpeed >> vSpeed >> fuel >> rotate >> power; setupIn.ignore();
	cerr << "trainer: Initial fuel: " << fuel << endl;
	
	setupIn.close();
	ifstream in;
	do
	{
		in.open("landerOutput.txt");
	} while (in.fail());

	in.clear();
	in.seekg(0);

	int rotateIn, powerIn = -1;

    // game loop
    while (1) {
		out << (int)X << " " << (int)Y << " " << (int)hSpeed << " " << (int)vSpeed << " " << fuel << " " << rotate << " " << power << endl;

		while (in.peek() == ifstream::traits_type::eof())
			in.clear();

		in >> rotateIn >> powerIn; in.ignore();
		cerr << "trainer: Read settings = " << rotateIn << " " << powerIn << endl;

		if (fuel < powerIn)
		{
			powerIn = fuel;
		}
		
		if (powerIn > power)
		{
			power = min(power + maxPowerSpeed, maxThrust);
		}
		else if (powerIn < power)
		{
			power = max(power - maxPowerSpeed, minThrust);
		}
		
		if (rotateIn > rotate)
		{
			rotate = min(rotateIn, rotate + maxRotateSpeed);
			rotate = min(rotate, maxAngle);
		}
		else if (rotateIn < rotate)
		{
			rotate = max(rotateIn, rotate - maxRotateSpeed);
			rotate = max(rotate, minAngle);
		}
		
		fuel = max(fuel - power, 0);
		
		hSpeed = (hSpeed + sin(rotate * DEG_TO_RAD) * power);
		vSpeed = (vSpeed - gravity + cos(rotate * DEG_TO_RAD) * power);
	
		float newX = X + hSpeed;
		float newY = Y + vSpeed;
		
		Segment collided;
		if (surface.collide(path, &collided))
		{
			if (collided.isFlat())
			{
				if (abs(hSpeed) <= landingHSpeed && abs(vSpeed) <= landingVSpeed && rotate == 0)
				{
					cerr << "trainer: Landed!" << endl;
					out << (int)newX << " " << (int)newY << " " << (int)hSpeed << " " << (int)vSpeed << " " << fuel << " " << rotate << " " << power << endl;

					exit(0);
				}
			}
			else
			{
				cerr << "trainer: CRASHED!!" << endl;
				out << (int)newX << " " << (int)newY << " " << (int)hSpeed << " " << (int)vSpeed << " " << fuel << " " << rotate << " " << power << endl;

				exit(1);
			}
		}
		else
		{
			if (newX < BOUNDS_X_MIN
			 || newX > BOUNDS_X_MAX
			 || newY < BOUNDS_Y_MIN
			 || newY > BOUNDS_Y_MAX)
			{
				cerr << "trainer: OUT OF BOUNDS!!" << endl;
				out << (int)newX << " " << (int)newY << " " << (int)hSpeed << " " << (int)vSpeed << " " << fuel << " " << rotate << " " << power << endl;
				exit(2);
			}
		}
		
		X = newX;
		Y = newY;
    }
}
