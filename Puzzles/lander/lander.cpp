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

#define GA_MIN_GENE_LENGTH 4
#define GA_MAX_GENE_LENGTH 10
#define GA_INITIAL_INDIVIDUALS 10
#define GA_UNIFORM_RATE 0.5
#define GA_MUTATION_RATE 0.015
#define GA_TOURNAMENT_SIZE 3
#define GA_ELITISM true

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

class Segment;

struct hash_segment
{
	size_t operator()(const Segment &seg ) const;
};
 
class Segment
{
	private:
	unordered_map<Segment, bool, hash_segment> intersection;
	
	public:
	Coordinate points[2];

	Segment(){}

	Segment(Coordinate A, Coordinate B)
	{
		points[0] = A;
		points[1] = B;
	}
	
    Segment& operator=(const Segment& other)
    {
		points[0] = other.points[0];
		points[1] = other.points[1];
        return *this;
    }
	
    bool operator==(const Segment& other)
    {
        return points[0] == other.points[0] && points[1] == other.points[1];
    }

	bool intersects(Segment other)
	{
		bool intersect = false;
		unordered_map<Segment, bool>::iterator it;
		it = intersection.find(other);
		if (it == intersection.end())
		{
			Coordinate s1, s2;
			s1.x = points[1].x - points[0].x;
			s1.y = points[1].y - points[0].y;
			s2.x = other.points[1].x - other.points[0].x;
			s2.y = other.points[1].y - other.points[0].y;

			double denominator = -s2.x * s1.y + s1.x * s2.y;
			if (denominator != 0)
			{
				double s = (-s1.y * (points[0].x - other.points[0].x) + s1.x * (points[0].y - other.points[0].y)) / denominator;
				if (s >= 0 && s <= 1)
				{
					double t = ( s2.x * (points[0].y - other.points[0].y) - s2.y * (points[0].x - other.points[0].x)) / denominator;
					if (t >= 0 && t <= 1)
					{
						// Collision detected
						intersect = true;
					}
				}
			}

			intersection.insert(pair<Segment, bool>(other, intersect));
		}
		else
		{
			intersect = it->second;
		}
		
		return intersect;
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
	
	Coordinate center()
	{
		return Coordinate(points[0].x + (points[1].x - points[0].x / 2), points[0].y + (points[1].y - points[0].y / 2));
	}
};

struct hash_segment
{
    size_t operator()(const Segment &seg ) const
    {
        size_t h0x = hash<double>()(seg.points[0].x);
        size_t h0y = hash<double>()(seg.points[0].y);
        size_t h1x = hash<double>()(seg.points[1].x);
        size_t h1y = hash<double>()(seg.points[1].y);
        return h0x ^ (h0y << 1) ^ (h1x << 2) ^ (h1y << 3);
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

Surface surface;
Segment target;
Coordinate targetCenter;

int X;
int Y;
int hSpeed;
int vSpeed;
int fuel;
int rotation;
int power;

class Gene
{
	private:
	void randomize()
	{
		myRotate = randomRotation(randomEngine);
		myPower = randomThrust(randomEngine);
	}

	public:
	int myRotate;
	int myPower;
	
	Gene()
	{
		randomize();
	}
	
	Gene (int myRotate, int myPower) : myRotate(myRotate), myPower(myPower)
	{}
   
    Gene& operator=(const Gene& other)
    {
        myRotate = other.myRotate;
        myPower = other.myPower;
        return *this;
    }
	
	friend ostream &operator<<(ostream &os, Gene const &m)
	{
		return os << "Gene myRotate:" << m.myRotate << " myPower:" << m.myPower;
	}
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

class Individual
{
	int fitness;
	vector<Gene> genes;
	int name;
	
	void resetFitness()
	{
		fitness = numeric_limits<int>::min();
	}

	void deleteGenes()
	{
		genes.clear();
		resetFitness();
	}

	void resizeGenes(int newSize)
	{
		deleteGenes();
		
		try
		{
			genes.reserve(newSize);
			genes.resize(newSize, Gene());
		}
		catch (const bad_alloc&)
		{
			cerr << "BAD ALLOC IN Individual::resizeGenes" << endl;
		}
	}
	
	void clampInputs(int &inRotate, int &inPower)
	{
		inRotate = min(inRotate, ROTATION_MAX);
		inRotate = max(inRotate, ROTATION_MIN);
		inPower = min(inPower, THRUST_MAX);
		inPower = max(inPower, THRUST_MIN);
	}

	public:
	Individual()
	{
		name = randomName(randomEngine);

		resetFitness();
	}
	
	Individual(int geneLength) : Individual()
	{
		resizeGenes(geneLength);
	}
	
	~Individual()
	{
		deleteGenes();
	}
	
    Individual& operator=(const Individual& other)
    {
		fitness = other.fitness;
		genes.resize(other.size());
		for (int i = 0; i < other.size(); i++)
		{
			genes[i] = other.genes[i];
		}

        return *this;
    }
	
	string stepOutput(int step, int currentRotate, int currentPower)
	{
		int outRotate = genes[step].myRotate + currentRotate;
		int outPower = genes[step].myPower + currentPower;		
		clampInputs(outRotate, outPower);
		return to_string(outRotate) + " " + to_string(outPower);
	}
	
	void generateIndividual()
	{
		for (int i = 0; i < size(); i++)
		{
			setGene(i, Gene());
		}
	}

	void step(int stepNumber, Coordinate& stepPosition, Coordinate& stepSpeed, int& stepPower, int& stepRotate, int &stepFuel, bool& stepLanded, bool& stepCrashed, Coordinate& stepDistance)
	{
		stepPower = stepPower + genes[stepNumber].myPower;
		stepRotate = stepRotate + genes[stepNumber].myRotate;
		if (stepFuel < stepPower)
		{
			stepPower = stepFuel;
		}

		clampInputs(stepRotate, stepPower);

		stepFuel = max(stepFuel - stepPower, 0);

//		stepSpeed.x = (stepSpeed.x + sin(-stepRotate * DEG_TO_RAD) * stepPower);
//		stepSpeed.y = (stepSpeed.y - GRAVITY + cos(-stepRotate * DEG_TO_RAD) * stepPower);
		pair<double, double> trigAngle = getTrigAngle(stepRotate);
		stepSpeed.x = (stepSpeed.x + trigAngle.first * stepPower);
		stepSpeed.y = (stepSpeed.y - GRAVITY + trigAngle.second * stepPower);

		Coordinate nextPosition = stepPosition + stepSpeed;
		Segment path(stepPosition, nextPosition);

		stepPosition = nextPosition;

		Segment collided;
		if (surface.collide(path, &collided))
		{
			if (collided.isFlat())
			{
//				if (abs(stepSpeed.x) <= LANDING_H_SPEED && abs(stepSpeed.y) <= LANDING_V_SPEED && stepRotate == LANDING_ROTATION)
				{
//					cerr << "Landed!" << endl;
					stepLanded = true;
				}
			}
			else
			{
//				cerr << "CRASHED!!" << endl;
				stepCrashed = true;
			}
		}
		else
		{
			if (stepPosition.x < BOUNDS_X_MIN
			 || stepPosition.x > BOUNDS_X_MAX
			 || stepPosition.y < BOUNDS_Y_MIN
			 || stepPosition.y > BOUNDS_Y_MAX)
			{
//				cerr << "OUT OF BOUNDS!!" << endl;
				stepCrashed = true;
			}
		}
		
		stepDistance = stepPosition - targetCenter;
	}
	
	Gene getGene(int index)
	{
		return genes[index];
	}
	
	void setGene(int index, Gene gene)
	{
		genes[index] = gene;
		resetFitness();
	}

	int size() const
	{
        return genes.size();
    }
	
	int findFitness(int steps, bool display)
	{
		Coordinate stepPosition = Coordinate(X, Y);
		Coordinate stepSpeed = Coordinate(hSpeed, vSpeed);
		int stepPower = power;
		int stepRotate = rotation;
		int stepFuel = fuel;
		bool stepCrashed = false;
		bool stepLanded = false;
		Coordinate stepDistance;
		for (int stepNumber = 0; stepNumber < steps; stepNumber++)
		{
			if (stepCrashed || stepLanded)
			{
				break;
			}

			step(stepNumber, stepPosition, stepSpeed, stepPower, stepRotate, stepFuel, stepCrashed, stepLanded, stepDistance);
		}

		int thisFitness = 0;
		if (display)
		{
			cerr << "X=" << stepPosition.x << "m, Y=" << stepPosition.y << "m, HSpeed=" << stepSpeed.x << "m/s VSpeed=" << stepSpeed.y << "m/s" << endl;
			cerr << "Fuel=" << stepFuel << "l, Angle=" << stepRotate << "°, Power=" << stepPower << endl;
		}
		else
		{
			if (stepLanded)
			{
				thisFitness += SCORE_LANDED;
				thisFitness += (abs(stepSpeed.x) <= LANDING_H_SPEED) ? 0 : abs(stepSpeed.x) * SCORE_LANDING_H_SPEED;
				thisFitness += (abs(stepSpeed.y) <= LANDING_V_SPEED) ? 0 : abs(stepSpeed.y) * SCORE_LANDING_V_SPEED;
				thisFitness += abs(stepRotate) * SCORE_LANDING_ROTATE;
			}
			else
			{
				thisFitness += abs(stepDistance.x) * SCORE_DISTANCE_X;
				thisFitness += abs(stepDistance.y) * SCORE_DISTANCE_Y;
				thisFitness += stepFuel * SCORE_FUEL;
				thisFitness += stepCrashed ? SCORE_CRASHED : 0;
			}
		}

		return thisFitness;
	}

	int getFitness()
	{
		if (fitness == numeric_limits<int>::min())
		{
			fitness = findFitness(size(), false);
		}

		return fitness;
	}

	friend ostream &operator<<(ostream &os, Individual const &m)
	{
		os << "Individual name:" << m.name << " fitness:" << m.fitness << endl;
		for (int i = 0; i < m.genes.size(); i++)
		{
			os << m.genes[i] << endl;
		}

		return os;
	}
};

class Population
{
	public:
	vector<Individual> individuals;
	
	Population()
	{
	}

	Population(int populationSize, bool initialize, int geneLength) : Population()
	{
		deleteIndividuals();

		try
		{
			individuals.reserve(populationSize);
			individuals.resize(populationSize, Individual(geneLength));
		}
		catch (const bad_alloc&)
		{
			cerr << "BAD ALLOC IN Population::Population" << endl;
		}

		if (initialize)
		{
			for (int i = 0; i < size(); i++)
			{
				individuals[i].generateIndividual();
			}
		}
	}
	
	~Population()
	{
		deleteIndividuals();
	}

    Population& operator=(const Population& other)
    {
		int individualsSize = other.individuals.size();

		try
		{
			individuals.reserve(individualsSize);
			individuals.resize(individualsSize);
		}
		catch (const bad_alloc&)
		{
			cerr << "BAD ALLOC IN Population::operator=" << endl;
		}

		for (int i = 0; i < individualsSize; i++)
		{
			individuals[i] = other.individuals[i];
		}

        return *this;
    }

	void deleteIndividuals()
	{
		individuals.clear();
	}
	
	Individual getIndividual(int index)
	{
		return individuals[index];
	}
	
	Individual getFittest()
	{
		Individual fittest = individuals[0];
		for (int i = 0; i < size(); i++)
		{
			if (fittest.getFitness() < getIndividual(i).getFitness())
			{
				fittest = getIndividual(i);
			}
		}
		
		return fittest;
	}
	
	int size()
	{
		return individuals.size();
	}
	
	void saveIndividual(int index, Individual in)
	{
		individuals[index] = in;
	}

	friend ostream &operator<<(ostream &os, Population const &m)
	{
		return os << "Population with " << m.individuals.size() << " individuals.";
	}
};

class Algorithm
{
	public:
	Algorithm()
	{
	}
    
    // Evolve a population
    Population evolvePopulation(Population pop)
	{
        Population newPopulation(pop.size(), false, pop.getIndividual(0).size());
		
        // Keep our best individual
        if (GA_ELITISM)
		{
            newPopulation.saveIndividual(0, pop.getFittest());
        }

        // Crossover population
        int elitismOffset = GA_ELITISM ? 1 : 0;

        // Loop over the population size and create new individuals with crossover
        for (int i = elitismOffset; i < pop.size(); i++)
		{
            Individual indiv1 = tournamentSelection(pop);
            Individual indiv2 = tournamentSelection(pop);
            Individual newIndiv = crossover(indiv1, indiv2);
            newPopulation.saveIndividual(i, newIndiv);
        }

        // Mutate population
        for (int i = elitismOffset; i < newPopulation.size(); i++)
		{
            mutate(newPopulation.getIndividual(i));
        }

        return newPopulation;
    }

    // Crossover individuals
    Individual crossover(Individual indiv1, Individual indiv2)
	{
        Individual newSol = indiv1;
        // Loop through genes
        for (int i = 0; i < indiv1.size(); i++)
		{
            // Crossover
			if (randomDecimal(randomEngine) < GA_UNIFORM_RATE)
			{
                newSol.setGene(i, indiv2.getGene(i));
            }
        }

        return newSol;
    }

    // Mutate an individual
    void mutate(Individual indiv)
	{
        // Loop through genes
        for (int i = 0; i < indiv.size(); i++)
		{
			if (randomDecimal(randomEngine) <= GA_MUTATION_RATE)
			{
                // Create random gene
				Gene gene = Gene();
                indiv.setGene(i, gene);
            }
        }
    }

    // Select individuals for crossover
    Individual tournamentSelection(Population pop)
	{
        // Create a tournament population
        Population tournament(GA_TOURNAMENT_SIZE, false, pop.getIndividual(0).size());
		uniform_int_distribution<int> randomIndividual{0, pop.size()-1};
        // For each place in the tournament get a random individual
        for (int i = 0; i < GA_TOURNAMENT_SIZE; i++)
		{
			int randomId = randomIndividual(randomEngine);
            tournament.saveIndividual(i, pop.getIndividual(randomId));
        }

        // Get the fittest
        Individual fittest = tournament.getFittest();

        return fittest;
    }
};

Population GA(double timeLimit, int geneLength, Population& myPop)
{
	double start = clock();
	double deadline = start + (double) CLOCKS_PER_SEC * timeLimit;
	Algorithm algorithm;
       
	// Evolve our population until we reach an optimum solution
	int generationCount = 0;
	while (clock() < deadline)
	{
		generationCount++;
		myPop = algorithm.evolvePopulation(myPop);
	}
	
	cerr << "GA returns after " << generationCount << " generations: " << myPop << endl;
	return myPop;
}

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
    int surfaceN; // the number of points used to draw the surface of Mars.
    cin >> surfaceN; cin.ignore();
	Coordinate previous, current;
    for (int i = 0; i < surfaceN; i++) {
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
				target = thisSegment;
				target.scale(TARGET_SCALAR);
				targetCenter = target.center();
			}
		}
		
		previous = current;
    }
	
	int geneNumber = 0;
	bool initialized = false;
	Population myPop;
	int geneLength = GA_MIN_GENE_LENGTH;
	Individual fittest;
	// game loop
    while (1)
	{
        cin >> X >> Y >> hSpeed >> vSpeed >> fuel >> rotation >> power; cin.ignore();

		if (geneLength < GA_MAX_GENE_LENGTH)
		{
			geneLength++;
		}
		myPop = Population(GA_INITIAL_INDIVIDUALS, true, geneLength);
		myPop = GA(TIMEOUT, geneLength, myPop);
		fittest = myPop.getFittest();
		cerr << fittest << endl;

		fittest.findFitness(1, true);	

		cout << fittest.stepOutput(0, rotation, power) << endl; // rotate power. rotate is the desired rotation angle. power is the desired thrust power.
	}
}
