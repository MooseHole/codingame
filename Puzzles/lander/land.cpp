#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cfloat>
#include <math.h>

#define CIRCLE_DEGREES 360				// The number of degrees in a circle
#define RAD_TO_DEG 180.0f/3.14159f		// Used to convert radians to degrees
#define DEG_TO_RAD 3.14159f/180.0f		// Used to convert degrees to radians

#define GRAVITY           -3.711f
#define MAX_DELTA_ANGLE   15
#define MIN_LANDING_SPEED Coordinate(  5,   5)
#define MAX_LANDING_SPEED Coordinate( 20,  40)
#define SCORE_SPEED       Coordinate( 10, 100)
#define SCORE_LOCATION    Coordinate(  1,   0)
#define SCORE_FUEL        1

using namespace std;

class Coordinate
{
    public:
        float x;
        float y;
        
        Coordinate()
        {
            x = 0.0f;
            y = 0.0f;
        }
        
        Coordinate(float _x, float _y)
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
        
        const bool operator<(const Coordinate& other)
        {
            return x < other.x && y < other.y;
        }    
        
        const bool operator>(const Coordinate& other)
        {
            return x > other.x && y > other.y;
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

        Coordinate& operator+=(const Coordinate& other)
        {
            *this = *this + other;
            return *this;
        }

        friend ostream &operator<<(ostream &os, Coordinate const &m)
        {
            return os << "(" << m.x << "," << m.y << ")";
        }
};

class Span
{
    public:
    
    Coordinate left;
    Coordinate right;
    
    Span()
    {
    }
    
    Span(Coordinate c1, Coordinate c2)
    {
        left = c1;
        right = c2;
    }

    int height(int x)
    {
        float o1, a1, a2, o2, base;
        
        if (left.y < right.y)
        {
            // Quadrant I
            o1 = right.y - left.y;
            a1 = right.x - left.x;
            a2 = x - left.x;
        }
        else
        {
            // Quadrant II
            o1 = left.y - right.y;
            a1 = right.x - left.x;
            a2 = right.x - x;
        }

        o2 = o1 * a2 / a1;
        base = left.y;

        return (int)(base + o2);
    }

    bool flat()
    {
        return (int)left.y == (int)right.y;
    }

    bool contains(int x)
    {
        return left.x <= x && right.x >= x;
    }
    
    Span zone()
    {
        float width = right.x - left.x;
        float buffer = width * 0.4f;
        return Span(Coordinate(left.x + buffer, left.y), Coordinate(right.x - buffer, right.y));
    }
    
    Coordinate center()
    {
        float centerX = (right.x + left.x) / 2;
        return Coordinate(centerX, height(centerX));
    }

    friend ostream &operator<<(ostream &os, Span const &m)
    {
        return os << m.left << m.right;
    }
};

class Surface
{
    private:
    Span getSpan(int x)
    {
        Coordinate previous = *coordinates.begin();
        Coordinate next = *coordinates.begin();

        if (x < 0)
        {
            previous.x = x;
            next.x = 0;
            previous.y++; // Make sure this span is not flat
        }
        else if (x > 7000)
        {
            previous = *coordinates.end();
            next = *coordinates.end();
            previous.x = 7000;
            next.x = x;
            next.y++; // Make sure this span is not flat
        }
        else
        {
            for(vector<Coordinate>::iterator it = coordinates.begin(); it != coordinates.end(); ++it)
            {
                // We have surpassed the target, so stop searching
                if (previous.x > x)
                {
                    cerr << "Coordinate lookup failed for " << x << endl;
                    break;
                }
                
                next = (*it);
                
                // This is the location
                if (next.x > x)
                {
                    break;
                }
        
                previous = next;
            }
        }
        
        return Span(previous, next);
    }

    public:
    vector<Coordinate> coordinates;
    
    Surface()
    {
    }
    
    bool addCoordinate(int x, int y)
    {
        Coordinate c(x, y);
        addCoordinate(c);
    }
    
    bool addCoordinate(Coordinate c)
    {
        coordinates.push_back(c);
    }
            
    int height(int x)
    {
        return getSpan(x).height(x);
    }
    
    bool flat(int x)
    {
        return getSpan(x).flat();
    }
    
    int highestBetween(Coordinate a, Coordinate b)
    {
        int highest = 0;
        int leftX = (int)min(a.x, b.x);
        int rightX = (int)max(a.x, b.x);
        
        highest = max(highest, height(a.x));
        highest = max(highest, height(b.x));
        
        for(vector<Coordinate>::iterator it = coordinates.begin(); it != coordinates.end(); ++it)
        {
            if ((*it).x > leftX && ((*it).x < rightX))
            {
                highest = max(highest, (int)((*it).y));
            }
        }
        
        return highest;
    }
    
    bool inLandingZone(int x)
    {
        Span span = getSpan(x);
        Span zone = span.zone();

        return zone.flat() && zone.contains(x);
    }
    
    bool impact(Coordinate location)
    {
        return height(location.x) >= location.y;
    }
    
    bool outOfBounds(Coordinate location)
    {
        return location.x < 0 || location.x > 7000 || location.y > 3000;
    }

    Coordinate findTarget()
    {
        Coordinate previous = coordinates[0];
        Coordinate next = coordinates[0];
        for(vector<Coordinate>::iterator it = coordinates.begin(); it != coordinates.end(); ++it)
        {
            next = (*it);
            
            // This is a span
            if (next.x > previous.x)
            {
                Span span = Span(previous, next);
                if (span.flat())
                {
                    return span.center();
                }
            }
            
            previous = next;
        }

        return Coordinate();
    }

    friend ostream &operator<<(ostream &os, Surface const &m)
    {
        for(vector<Coordinate>::const_iterator it = m.coordinates.begin(); it != m.coordinates.end(); ++it)
        {
            os << (*it) << " ";
        }
        
        return os;
    }
};

class Lander
{
    private:
    int normalizePower(int power)
    {
    	if (power > 4)
    	{
    		power = 4;
    	}
    	else if (power < 0)
    	{
    		power = 0;
    	}
    
    	return power;
    }
    
    
    int normalizeAngle(int angle)
    {
    	while (angle > 90)
    	{
    		angle -= 180;
    	}
    	
    	while (angle < -90)
    	{
    		angle += 180;
    	}
    
    	return angle;
    }

    public:
    Coordinate location;
    Coordinate speed;
    int fuel;
    int rotate;
    int power;
    
    int desiredRotate;
    int desiredPower;
    float deltaV;
    float deltaH;
    
    Lander()
    {
        initialize();
    }
    
    Lander(int X, int Y, int hSpeed, int vSpeed, int _fuel, int _rotate, int _power)
    {
		Lander();
        update(X, Y, hSpeed, vSpeed, _fuel, _rotate, _power);
    }
    
    Lander(const Lander &other)
    {
        update(other.location, other.speed, other.fuel, other.rotate, other.power);
    }

    Lander& operator=(Lander other)
    {
        update(other.location, other.speed, other.fuel, other.rotate, other.power);
        return *this;
    }
        
    void initialize()
    {
        desiredRotate = 0;
        desiredPower = 0;
    }
        
    void update(Coordinate _location, Coordinate _speed, int _fuel, int _rotate, int _power)
    {
        location = _location;
        speed = _speed;
        fuel = _fuel;
        rotate = _rotate;
        power = _power;

        deltaH = sin(rotate * DEG_TO_RAD) * power;
        deltaV = cos(rotate * DEG_TO_RAD) * power;
    }
        
    void update(int X, int Y, int hSpeed, int vSpeed, int _fuel, int _rotate, int _power)
    {
        location.x = X;
        location.y = Y;
        speed.x = hSpeed;
        speed.y = vSpeed;
        fuel = _fuel;
        rotate = _rotate;
        power = _power;

        deltaH = sin(rotate * DEG_TO_RAD) * power;
        deltaV = cos(rotate * DEG_TO_RAD) * power;
    }
    
    // This is used for testing projections
    void step(Coordinate thrust, int consumption)
    {
    	speed += thrust;
    	location += speed;
    	fuel -= consumption;
    }

    void setDesiredPower(int power)
    {
    	desiredPower = normalizePower(power);
    }

    void setDesiredRotate(int angle)
    {
    	desiredRotate = normalizeAngle(angle);
    }
    
    friend ostream &operator<<(ostream &os, Lander const &m)
    {
        return os << "Location: " << m.location << " Speed:" << m.speed << " Fuel: " << m.fuel << " Rotate: " << m.rotate << " Power: " << m.power << " DeltaH: " << m.deltaH << " DeltaV: " << m.deltaV;
    }
};

class Setting
{
    private:
    Coordinate distance;
	Coordinate scoreSpeed;
	Coordinate scoreLocation;
	int scoreFuel;

    public:
    Surface* surface;
    Lander* lander;
    Coordinate target;
    Coordinate projectedLocation;
    Coordinate projectedSpeed;
    int projectedFuel;
    int projectedSteps;
    int rotate;
    int power;
    Coordinate thrust;
    float score;
    bool initialized;

    Setting()
    {
        rotate = 0;
        power = 0;
        score = FLT_MIN;
        initialized = false;
		scoreSpeed = SCORE_SPEED;
		scoreLocation = SCORE_LOCATION;
		scoreFuel = SCORE_FUEL;
        thrust = getThrust();
    }

    Setting(Surface* _surface, Lander* _lander, int _rotate, int _power)
    {
        Setting();
        surface = _surface;
        lander = _lander;
        rotate = _rotate;
        power = _power;
        initialized = true;
        thrust = getThrust();
    }

    void setScoreScalars(Coordinate _scoreSpeed, Coordinate _scoreLocation, int _scoreFuel)
    {
        scoreSpeed = _scoreSpeed;
        scoreLocation = _scoreLocation;
        int scoreFuel = _scoreFuel;
    }

    Coordinate getThrust()
    {
        return Coordinate((float)power * sin(rotate * DEG_TO_RAD), (float)power * cos(rotate * DEG_TO_RAD) + GRAVITY);
    }
    
    bool isPossible()
    {
        return (abs(lander->rotate - rotate) <= 15) && (abs(lander->power - power) <= 1);
    }

    bool willImpact()
    {
    	bool impact = surface->impact(lander->location + thrust);

    	return impact;
    }
    
    bool willLand()
    {
	    return willImpact() && rotate == 0 && lander->speed + thrust > MAX_LANDING_SPEED;
    }

    void projectPath()
    {
        projectedLocation = lander->location;
        projectedSpeed = lander->speed;
        projectedSteps = -1;
        while (!surface->impact(projectedLocation) && !surface->outOfBounds(projectedLocation))
        {
            projectedSpeed.x += thrust.x;
            projectedSpeed.y += thrust.y;
            projectedLocation.x += projectedSpeed.x;
            projectedLocation.y += projectedSpeed.y;
            projectedSteps++;
        }
        
        if (projectedSteps < 0)
        {
            projectedSteps = 0;
        }
        
        projectedFuel = lander->fuel - (projectedSteps * power);
    }

    void findScore()
    {
        score = 0;
        score -= fabs((target.x - projectedLocation.x)) * scoreLocation.x;
        score -= fabs((target.y - projectedLocation.y)) * scoreLocation.y;
        score += (MAX_LANDING_SPEED.x - fabs(projectedSpeed.x)) * scoreSpeed.x;
        score += (MAX_LANDING_SPEED.y - fabs(projectedSpeed.y)) * scoreSpeed.y;
//        score += (fabs(projectedSpeed.x) - minLandingSpeed.x) * scoreSpeed.x;
//        score += (fabs(projectedSpeed.y) - minLandingSpeed.y) * scoreSpeed.y;
        score += projectedFuel * scoreFuel;

//        if (!surface->inLandingZone(projectedLocation.x))
//        {
//            score -= 10000;
//        }
        
//        if (!isPossible())
//        {
//            score -= 100000000;
//        }
    }

    friend ostream &operator<<(ostream &os, Setting const &m)
    {
        return os << "[" << m.rotate << "][" << m.power << "]=" << m.score << m.projectedLocation << m.projectedSpeed << m.projectedFuel << m.thrust;
    }
};

class Settings
{
    private:
    vector<Setting> settings;

    public:
    Surface* surface;
    Lander* lander;
    
    Settings()
	{
	}

    Settings(Surface* _surface, Lander* _lander)
    {
        Settings();
        surface = _surface;
        lander = _lander;
        for (int power = 0; power <= 4; power++)
        {
            for (int rotate = -90; rotate <= 90; rotate++)
            {
                settings.push_back(Setting(surface, lander, rotate, power));
            }
        }
    }
    
    void setTarget(Coordinate _target)
    {
        for (vector<Setting>::iterator it = settings.begin(); it != settings.end(); ++it)
        {
            (*it).target = _target;
        }
    }
        
    void setLander(Lander* _lander)
    {
        lander = _lander;
    }

    void setScoreScalars(Coordinate _scoreSpeed, Coordinate _scoreLocation, int _scoreFuel)
    {
        for (vector<Setting>::iterator it = settings.begin(); it != settings.end(); ++it)
        {
            (*it).setScoreScalars(_scoreSpeed, _scoreLocation, _scoreFuel);
        }
    }
    
    Setting findBestSetting()
    {
        int i = 0;
        Setting bestSetting = Setting();
        for (vector<Setting>::iterator it = settings.begin(); it != settings.end(); ++it)
        {
            if ((*it).isPossible())
            {
                (*it).projectPath();
                (*it).findScore();
                if (!bestSetting.initialized || (*it).score > bestSetting.score)
                {
                    bestSetting = *it;
                }

//                cerr << "Setting(" << i++ << ") = " << (*it) << endl;
            }
        }
        
//        cerr << "Best Setting = " << bestSetting << endl;
        return bestSetting;
    }
};

class Solution
{
	private:
	vector<Setting> commands;
	
	public:
    Surface* surface;
    Lander* lander;
    Settings settings;
    Coordinate target;
    
    Solution(){}
    
	Solution(Surface* _surface, Lander* _lander, Coordinate _target)
	{
	    Solution();
		surface = _surface;
		lander = _lander;
		target = _target;
	}
	
	bool getPath()
	{
	    int minScoreSpeedX = 0;
	    int maxScoreSpeedX = 10;
	    int minScoreSpeedY = 0;
	    int maxScoreSpeedY = 10;
	    int minScoreLocationX = 0;
	    int maxScoreLocationX = 10;
	    int minScoreLocationY = 0;
	    int maxScoreLocationY = 10;
	    int minScoreFuel = 0;
	    int maxScoreFuel = 10;
	    
		Lander testLander = (*lander);
		bool success = false;

        settings = Settings(surface, &testLander);
        settings.setTarget(target);
        
        int tests = 0;        
        for (int scoreFuel = minScoreFuel; scoreFuel <= maxScoreFuel; ++scoreFuel)
        {
            for (int scoreSpeedY = minScoreSpeedY; scoreSpeedY <= maxScoreSpeedY; ++scoreSpeedY)
            {
				for (int scoreSpeedX = minScoreSpeedX; scoreSpeedX <= maxScoreSpeedX; ++scoreSpeedX)
				{
					for (int scoreLocationY = minScoreLocationY; scoreLocationY <= maxScoreLocationY; ++scoreLocationY)
					{
						for (int scoreLocationX = minScoreLocationX; scoreLocationX <= maxScoreLocationX; ++scoreLocationX)
						{
//                            if (tests++ % 100 == 0)
                            {
//                                cerr << tests << " scores:" << scoreSpeedX << ":" << scoreSpeedY << ":" << scoreLocationX << ":" << scoreLocationY << ":" << scoreFuel << endl;
                            }

                            testLander = (*lander);
                            settings.setLander(&testLander);
                            settings.setScoreScalars(Coordinate(scoreSpeedX, scoreSpeedY), Coordinate(scoreLocationX, scoreLocationY), scoreFuel);
    
                            while(1)
                            {
                                Setting setting = settings.findBestSetting();
                            
                				commands.push_back(setting);
                				testLander.step(setting.thrust, setting.power);
//                				cerr << testLander << endl;
                				if (setting.willImpact())
                				{
//            					    cerr << "IMPACT" << endl;
                					if (setting.willLand())
                					{
                						return true;
                					}        

									cerr << testLander << tests << " scores:" << scoreSpeedX << ":" << scoreSpeedY << ":" << scoreLocationX << ":" << scoreLocationY << ":" << scoreFuel << endl;
    
                					commands.clear();
                					break;
                				}
                            }
                        }
                    }
                }
            }
        }

		return false;
	}
};

class Pilot
{
    private:
    Coordinate projectedLocation;
    Coordinate projectedSpeed;
    int projectedSteps;
    Coordinate target;
    Coordinate distance;
    Settings settings;
    Solution solution;

    Pilot(){}

    public:
    Surface* surface;
    Lander* lander;

    Pilot(Surface* _surface, Lander* _lander)
    {
        Pilot();
        surface = _surface;
        lander = _lander;
        settings = Settings(surface, lander);
        solution = Solution(surface, lander, target);
    }

    void setTarget()
    {
        target = surface->findTarget();
        settings.setTarget(target);
    }

    void checkDistance()
    {
        distance = target - lander->location;
    }

    bool overFlat()
    {
        return surface->flat(lander->location.x);
    }
    
    Coordinate getThrust()
    {
        return Coordinate(lander->power * sin(lander->rotate * DEG_TO_RAD), lander->power * cos(lander->rotate * DEG_TO_RAD) - GRAVITY);
    }

    Coordinate findImpact()
    {
        projectedLocation = lander->location;
        projectedSpeed = lander->speed;
        projectedSteps = -1;
        
        Coordinate thrust = getThrust();
        
        while (!surface->impact(projectedLocation) /*&& !surface->outOfBounds(projectedLocation)*/)
        {
            projectedSpeed.x += thrust.x;
            projectedSpeed.y += thrust.y;
            projectedLocation.x += projectedSpeed.x;
            projectedLocation.y += projectedSpeed.y;
            projectedSteps++;
            cerr << thrust <<  projectedLocation << projectedSpeed << projectedSteps << endl;
        }
        
        if (projectedSteps < 0)
        {
            projectedSteps = 0;
        }
    }
    
    bool onTarget()
    {
        return surface->inLandingZone(projectedLocation.x);
    }
    
    bool leftOfTarget()
    {
        return projectedLocation.x < target.x;
    }
    
    bool rightOfTarget()
    {
        return projectedLocation.x > target.x;
    }
    
    void hardTurn(string direction)
    {
        int sign = 1;
        
        if (direction == "left")
        {
            sign = -1;
        }
        
        lander->setDesiredRotate(sign * 90);
        if (lander->rotate * sign > 0)
        {
            lander->setDesiredPower(4);
        }
        else
        {
            lander->setDesiredPower(0);
        }
    }
    
    bool tooSlowX()
    {
        return fabs(lander->speed.x) < MIN_LANDING_SPEED.x;
    }
    
    bool tooSlowY()
    {
        return fabs(lander->speed.y) < MIN_LANDING_SPEED.y;
    }
    
    bool tooFastX()
    {
        return (fabs(lander->speed.x) >= MAX_LANDING_SPEED.x);
    }
    
    bool tooFastY()
    {
        return (fabs(lander->speed.y) >= MAX_LANDING_SPEED.y);
    }
    
    bool recoverX()
    {
        if (tooFastX())
        {
            if (lander->speed.x > 0)
            {
                lander->setDesiredRotate(90);
                if (lander->rotate > 0)
                {
                    lander->setDesiredPower(3);
                }
                else
                {
                    lander->setDesiredPower(0);
                }
            }
            else
            {
                lander->setDesiredRotate(-90);
                if (lander->rotate < 0)
                {
                    lander->setDesiredPower(3);
                }
                else
                {
                    lander->setDesiredPower(0);
                }
            }
            
            cerr << "Recovering X!" << endl;
            return true;
        }
        
        return false;
    }

    bool recoverY()
    {
        if (tooFastY())
        {
            lander->setDesiredRotate(0);
            lander->setDesiredPower(4);

            cerr << "Recovering Y!" << endl;
            return true;
        }
        
        return false;
    }
    
    bool recoverXY()
    {
        if (tooFastY() && tooFastX())
        {
            Coordinate proportion = Coordinate(fabs(lander->speed.x) - MAX_LANDING_SPEED.x, fabs(lander->speed.y) - MAX_LANDING_SPEED.y);
            
            int angle = (int)(90.0f - atan(proportion.y / proportion.x) * 180.0f/3.14159f);
            cerr << "Recovering XY angle: " << angle << " Proportions: " << proportion << endl;
            
            if (lander->speed.x > 0)
            {
                lander->setDesiredRotate(1 * angle);
                if (lander->rotate > 0)
                {
                    lander->setDesiredPower(4);
                }
                else
                {
                    lander->setDesiredPower(0);
                }
            }
            else
            {
                lander->setDesiredRotate(-1 * angle);
                if (lander->rotate < 0)
                {
                    lander->setDesiredPower(4);
                }
                else
                {
                    lander->setDesiredPower(0);
                }
            }
            
            cerr << "Recovering XY!" << endl;
            return true;
        }
        
        return false;
    }
    
    

    bool recovering()
    {
        if (!tooSlowX() && !tooSlowY())
        {
            return recoverXY() || recoverY() || recoverX();
        }
        
        return false;
    }
    
    // Get the angle meeded to land on the target
    int getAngle()
    {
        // Aim at the target
        Coordinate difference(projectedLocation - target);
        Coordinate aim = target - difference;
        int angle = (int)((atan(fabs(aim.y)/fabs(aim.x))) * 180.0f/3.14159f);
        if ((aim.x < 0 && angle < 0) || (aim.x > 0 && angle > 0))
        {
            angle *= -1;
        }

        cerr << "getAngle() difference: " << difference << " aim:" << aim << " angle:" << angle << endl;
        return angle;
    }

    void steer()
    {
        if (onTarget() /*|| projectedLocation.y < target.y*/)
        {
            // On the right track or need to climb
            lander->setDesiredRotate(0);
        }
        else
        {
            lander->setDesiredRotate(getAngle());
        }
    }

    void throttle()
    {
        if ((lander->desiredRotate < 0 && lander->rotate > 0)
         || (lander->desiredRotate > 0 && lander->rotate < 0))
        {
            lander->setDesiredPower(0);
        }
        if (fabs(projectedSpeed.y) >= MAX_LANDING_SPEED.y)
        {
            lander->setDesiredPower(4);
        }
        else if (!onTarget())
        {
            lander->setDesiredPower(3);
        }
        else
        {
            lander->setDesiredPower(0);
        }
    }
    
    void drop()
    {
        lander->setDesiredRotate(0);
        lander->setDesiredPower(0);
    }
    
    void fixRotation(int steps)
    {
        int maxRotate = min(90, steps * MAX_DELTA_ANGLE);

        if (abs(lander->desiredRotate) > maxRotate)
        {
//            lander->setDesiredRotate(maxRotate * (lander->desiredRotate < 0 ? -1 : 1));
            lander->setDesiredRotate(0);
            if (overFlat())
            {
                lander->setDesiredPower(4);
            }
        }
    }
    
    // Get high enough to clear anything in the way
    bool clearObstacles()
    {
        int obstacleElevation = surface->highestBetween(lander->location, target);
        
        bool mustClear = false;
        
        if (lander->location.y < obstacleElevation)
        {
            cerr << "Must clear because " << lander->location.y << " < " << obstacleElevation << endl;
            mustClear = true;
        }
        
        findImpact();
        if (projectedSpeed.y < MAX_LANDING_SPEED.y * -1)
        {
            cerr << "Must clear because " << projectedSpeed.y << " < " << (MAX_LANDING_SPEED.y * -1) << endl;
            mustClear = true;
        }
        
        if (mustClear)
        {
            lander->setDesiredRotate(0);
            lander->setDesiredPower(4);
        }
        
        return mustClear;
    }

    // Get over the target and stop going sideways
    bool hoverOverTarget()
    {
        if (lander->location.x < target.x)
        {
            hardTurn("left");
        }
        else if (lander->location.x > target.x)
        {
            hardTurn("right");
        }
        else
        {
            return false;
        }
        
        return true;
    }
    
    void land()
    {
        lander->setDesiredRotate(0);

        if (lander->speed.y > MAX_LANDING_SPEED.y * -1)
        {
            lander->setDesiredPower(4);
        }
        else
        {
            lander->setDesiredPower(0);
        }
    }

    void drive()
    {
/*
        if (!clearObstacles())
        {
            if (!hoverOverTarget())
            {
                land();
            }
        }
*/

        int i = 0;
        if (solution.getPath())
		{
			cerr << "WOOO" << endl;
		}
		else
		{
			cerr << "awww" << endl;
		}
        
/*
        Setting setting = settings.findBestSetting();
        lander->setDesiredRotate(setting.rotate);
        lander->setDesiredPower(setting.power);
        fixRotation(setting.projectedSteps);
*/
/*
        checkDistance();
        findImpact();
        drop();
        if (onTarget())
        {
            recovering();
        }
        else //if (!recovering())
        {
            cerr << "I'm driving!" << endl;
            steer();
            throttle();
        }
        
        fixRotation();
*/
    }

    friend ostream &operator<<(ostream &os, Pilot const &m)
    {
        cerr << "Maximum landing velocity: " << MAX_LANDING_SPEED << endl;
        cerr << "Target: " << m.target << endl;
        cerr << "Distance: " << m.distance << endl;
        cerr << "Projection: " << m.projectedLocation << " at " << m.projectedSpeed << "m/s in " << m.projectedSteps << " steps." << endl;
        return os << m.lander->desiredRotate << " " << m.lander->desiredPower;
    }
};

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int process()
{
    Surface surface;
    Lander lander;
    Pilot pilot(&surface, &lander);
    
    int surfaceN; // the number of points used to draw the surface of Mars.
    cin >> surfaceN; cin.ignore();
    for (int i = 0; i < surfaceN; i++) {
        int landX; // X coordinate of a surface point. (0 to 6999)
        int landY; // Y coordinate of a surface point. By linking all the points together in a sequential fashion, you form the surface of Mars.
        cin >> landX >> landY; cin.ignore();
        pilot.surface->addCoordinate(landX, landY);
    }

    pilot.setTarget();

    // game loop
    while (1) {
        int X;
        int Y;
        int hSpeed; // the horizontal speed (in m/s), can be negative.
        int vSpeed; // the vertical speed (in m/s), can be negative.
        int fuel; // the quantity of remaining fuel in liters.
        int rotate; // the rotation angle in degrees (-90 to 90).
        int power; // the thrust power (0 to 4).
        cin >> X >> Y >> hSpeed >> vSpeed >> fuel >> rotate >> power; cin.ignore();
        
        pilot.lander->update(X, Y, hSpeed, vSpeed, fuel, rotate, power);
        
        cerr << surface << endl;
        cerr << lander << endl;
        
        if (pilot.overFlat())
        {
            cerr << "That looks like a nice place to land!" << endl;
        }
        else
        {
            cerr << "Hmm, this looks too hilly." << endl;
        }
        
        pilot.drive();

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        cout << pilot << endl; // rotate power. rotate is the desired rotation angle. power is the desired thrust power.
    }
}

void test1()
{
    Surface surface;
    Lander lander;
    Pilot pilot(&surface, &lander);
    
	pilot.surface->addCoordinate(0,100);
	pilot.surface->addCoordinate(1000,500);
	pilot.surface->addCoordinate(1500,1500);
	pilot.surface->addCoordinate(3000,1000);
	pilot.surface->addCoordinate(4000,150);
	pilot.surface->addCoordinate(5500,150);
	pilot.surface->addCoordinate(6999,800);

    pilot.setTarget();


    // game loop
	pilot.lander->update(2500, 2700, 0, 0, 550, 0, 0);
    
    
    if (pilot.overFlat())
    {
        cerr << "That looks like a nice place to land!" << endl;
    }
    else
    {
        cerr << "Hmm, this looks too hilly." << endl;
    }
    
    pilot.drive();
	
	cerr << "Done driving." << endl;
	
    // Write an action using cout. DON'T FORGET THE "<< endl"
    // To debug: cerr << "Debug messages..." << endl;

    cout << pilot << endl; // rotate power. rotate is the desired rotation angle. power is the desired thrust power.
}

int main()
{
	test1();
//	process();
}