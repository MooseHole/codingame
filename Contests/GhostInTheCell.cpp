/*
055201703042050S 146/ 638 Add cyborg blinking.
054201703041842S 225/ 622 Add dancing cyborg.
053201703040152S  92/ 616 Fix scroll message.
053201703040145S 251/ 584 Re-add don't send troops to a place where a bomb will destroy them.
053201703040130S 231/ 584 Revert to 44.  Re-add output message.
052201703031715S 508/ 578 Add output message.
051201703031650S 506/ 582 Capture with 5 over garrison instead of just 1.
050201703031140S 270/ 574 Include production in projection of turn 0.  Setup for random step test.
049201703022210S 354/ 570 Fix up distance calculations.
048201703021345S 375/ 568 Refactor projections.  cyborgsIn may be off a little now, so check.
047201703020040S 334/ 529 Refactor into a state in preparation for multi-state projection.
046201703012230S 382/ 533 Don't send troops to a place where a bomb will destroy them.
045201703011330S 267/ 491 Refactor projection and update when sending new troops.
044201703011205S 221/ 497 Fix evaluation of my own bombs.
043201703011130S 226/ 487 Better evacuation evaluation.  Evacuate using attack or reinforce where possible.
042201703011100S 331/ 488 Evacuate when bomb might hit.
041201702281850S 502/ 645 Fix bomb projection.
039201702281640B   2/1479 Do not increment.
038201702281610B  15/1467 Use more bomb.
037201702281515B 106/1453 Revert to revision 035.  Reinforce only if force is available.
036201702281455B 170/1447 Capture non-producing factories based on danger.
035201702281230B  52/1424 Support bombed production.
034201702281200B 226/1433 Prioritize attackees with my threat over production.
033201702281055B 257/1431 Launch attackers starting from furthest needed factory.
032201702281010B 583/1423 Prioritize attackees that I threaten higher.
031201702280010B 399/1337 Actually fix sorts.
030201702272320B 638/1327 Reinforcer tweak.
029201702272300B 690/1325 Sort was backwards; flipped sort.
028201702272210B 681/1328 Sorts based on danger.
027201702272210B 714/1327 Increment last.  Increase dud capture force.
026201702272150B 840/1328 Balls out when attacking higher production.
025201702271940B 641/1305 Move more cyborgs at a time.  Tweak reinforcement.
024201702271720B 648/1293 Fix cyborg production in projection.
023201702271555B 713/1247 Fix attack calculations.
022201702271530Bp683/1227 Attack from multiple factories.
021201702271340B 342/1224 Refactoring.
020201702271240B 323/1180 Prioritize reinforcers.
019201702271140B 375/1155 Prioritize bombing targets.
018201702271110B 372/1143 Prioritize targets rather than iterating through my factories.
017201702271035B 219/1138 Prefer targets with fewer cyborgs.
016201702270030B 499/1290 Refactoring.
015201702270015B 620/1285 Changed bomb strategy to target powerhouses.
014201702262345B 653/1284 Capture non-producing only if can inc.  Share cyborgs to frontier.
013201702262235B 559/1276 Allow multi-turn bomb and inc.  Refactoring sorts.
012201702262010B 409/1267 Update reinforcement.
011201702261940B 534/1244 Better garrison projection.
010201702261850B 697/1239 Add bomb projection and stop bombs from going to same target.
009201702261040B 220/ 963 Add multi-move support.
008201702260050B 120/ 735 Correct production projection to path length + 1 for the command turn.
007201702260040B 282/ 731 Reset troops each loop.
006201702260025B 320/ 731 Initial incrementing code.
005201702260010B 338/ 713 Add bombs.  Attack before reinforce.
001201702252228W ???/ ??? Initial revision
*/

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

// Settings
#define INCREMENT_THRESHOLD 20
#define BOMB_THRESHOLD 10
#define DUD_CAPTURE_THRESHOLD 50
#define OUTPUT_LENGTH 25
#define SCROLL_SPEED 14

// Given
#define INCREMENT_SACRIFICE 10
#define MAX_DISTANCE 20

string outputString = "The ants go marching one by one.  Hurrah.  Hurrah.  The ants go marching one by one.  Hurrah.  Hurrah.  The ants go marching one by one.  The little one stops to suck his thumb and they all go marching down to the ground to get out of the rain.  Boom!  Boom!  Boom!  The ants go marching two by two.  Hurrah.  Hurrah.  The ants go marching two by two.  Hurrah.  Hurrah.  The ants go marching two by two.  The little one stops to tie his shoe and they all go marching down to the ground to get out of the rain.  Boom!  Boom!  Boom!  The ants go marching three by three.  Hurrah.  Hurrah.  The ants go marching three by three.  Hurrah.  Hurrah.  The ants go marching three by three.  The little one stops to climb a tree and they all go marching down to the ground to get out of the rain.  Boom!  Boom!  Boom!  The ants go marching four by four.  Hurrah.  Hurrah.  The ants go marching four by four.  Hurrah.  Hurrah.  The ants go marching four by four.  The little one stops to shut the door and they all go marching down to the ground to get out of the rain.  Boom!  Boom!  Boom!  The ants go marching five by five.  Hurrah.  Hurrah.  The ants go marching five by five.  Hurrah.  Hurrah.  The ants go marching five by five.  The little one stops to take a dive and they all go marching down to the ground to get out of the rain.  Boom!  Boom!  Boom!  The ants go marching six by six.  Hurrah.  Hurrah.  The ants go marching six by six.  Hurrah.  Hurrah.  The ants go marching six by six.  The little one stops to pick up sticks and they all go marching down to the ground to get out of the rain.  Boom!  Boom!  Boom!  The ants go marching seven by seven.  Hurrah.  Hurrah.  The ants go marching seven by seven.  Hurrah.  Hurrah.  The ants go marching seven by seven.  The little one stops to pray to heaven and they all go marching down to the ground to get out of the rain.  Boom!  Boom!  Boom!  The ants go marching eight by eight.  Hurrah.  Hurrah.  The ants go marching eight by eight.  Hurrah.  Hurrah.  The ants go marching eight by eight.  The little one stops to roller skate and they all go marching down to the ground to get out of the rain.  Boom!  Boom!  Boom!  The ants go marching nine by nine.  Hurrah.  Hurrah.  The ants go marching nine by nine.  Hurrah.  Hurrah.  The ants go marching nine by nine.  The little one stops to check the time and they all go marching down to the ground to get out of the rain.  Boom!  Boom!  Boom!  The ants go marching ten by ten.  Hurrah.  Hurrah.  The ants go marching ten by ten.  Hurrah.  Hurrah.  The ants go marching ten by ten.  The little one stops to shout, 'The end,' and they all go marching down to the ground to get out of the rain.  Boom!  Boom!  Boom!  It's over.  Just stop.  Why are we doing this?  Just let it end.  Ok I'll tell a joke.  What d";
int outputBegin = 0;

vector<string> leftArm;
vector<string> rightArm;
vector<string> face;
vector<string> blink;
int leftArmIndex = 0;
int rightArmIndex = 0;
int faceIndex = 2;

class Link
{
	public:
	int destinationId;
	int distance;
	
	Link(int _destinationId, int _distance)
	{
		destinationId = _destinationId;
		distance = _distance;
	}
};

class Troop
{
	public:
	int id;
	int owner;
	int source;
	int destination;
	int cyborgs;
	int turns;

	Troop()
	{
		id = -1;
		owner = 0;
		source = -1;
		destination = -1;
		cyborgs = -1;
		turns = -1;
	}

	Troop(int _id)
	{
		id = _id;
		owner = 0;
		source = -1;
		destination = -1;
		cyborgs = -1;
		turns = -1;
	}

	void update(int _owner, int _source, int _destination, int _cyborgs, int _turns)
	{
		owner = _owner;
		source = _source;
		destination = _destination;
		cyborgs = _cyborgs;
		turns = _turns;
	}
	
	bool isMine()
	{
		return owner == 1;
	}
	
	bool isTheirs()
	{
		return owner == -1;
	}
};

class Bomb
{
	public:
	int owner;
	int id;
	int source;
	int destination;
	int turns;
	bool used;
	bool live;
	bool pendingUpdate;
	
	Bomb(int _owner)
	{
		owner = _owner;
		id = -1;
		source = -1;
		destination = -1;
		turns = -1;
		used = false;
		live = false;
		pendingUpdate = false;
	}
	
	void update(int _id, int _source, int _destination, int _turns)
	{
		id = _id;
		source = _source;
		destination = _destination;
		turns = _turns;
		used = true;
		live = true;
		pendingUpdate = false;
	}
};

vector<Troop> troops;
vector<Bomb> myBombs;
vector<Bomb> theirBombs;

class Factory
{
	public:
	int id;
	int production;
	int turnsUntilProduction;
	int cyborgs;
	int owner;
	vector<Link> links;
	int distanceCheck;
	vector<Troop> incoming;
	int bombIn; // 0 for none
	int enemyNeighbors;
	int danger;
	map<int, int> incomingBombs;
	bool evacuateNow;
	
	Factory(int _id)
	{
		id = _id;
		production = -1;
		turnsUntilProduction = -1;
		cyborgs = -1;
		owner = 0;
		distanceCheck = -1;
		bombIn = 0;
		enemyNeighbors = 0;
		danger = 0;
		incomingBombs.clear();
		evacuateNow = false;
	}
	
	void addLink(Link link)
	{
		links.push_back(link);
	}
	
	void update(int _owner, int _cyborgs, int _production, int _turnsUntilProduction)
	{
		owner = _owner;
		cyborgs = _cyborgs;
		production = _production;
		turnsUntilProduction = _turnsUntilProduction;
		incoming.clear();
		bombIn = 0;
		enemyNeighbors = 0;
		danger = 0;
		evacuateNow = false;
	}
	
	bool bombArriveIn(int turns) const
	{
		for (map<int, int>::const_iterator bombIt = incomingBombs.begin(); bombIt != incomingBombs.end(); ++bombIt)
		{
			if ((*bombIt).second == turns)
			{
				return true;
			}
		}
		
		return false;
	}
	
	void updateEvacuation(int bombId, int turns)
	{
		if (incomingBombs.find(bombId) == incomingBombs.end())
		{
			incomingBombs[bombId] = turns;
		}
		else
		{
			if (incomingBombs[bombId] > 0)
			{
				incomingBombs[bombId]--;
				if (incomingBombs[bombId] == 0)
				{
					evacuateNow = true;
				}
			}
		}
	}
	
	int cyborgsIn(int turns) const
	{
		map<int, int> turnGarrison;
		turnGarrison[0] = cyborgs + (isNeutral() ? 0 : turnsUntilProduction > 0 ? 0 : production);
		for (vector<Troop>::const_iterator troopIt = incoming.begin(); troopIt != incoming.end(); ++troopIt)
		{
			Troop thisTroop = *troopIt;
			if (thisTroop.destination == id && thisTroop.turns <= turns)
			{
				if (thisTroop.owner == owner)
				{
					turnGarrison[thisTroop.turns] += thisTroop.cyborgs;
				}
				else
				{
					turnGarrison[thisTroop.turns] -= thisTroop.cyborgs;
				}
			}
		}
		
		int simulationTurnsUntilProduction = turnsUntilProduction;
		for (int i = 1; i < turns; i++)
		{
			turnGarrison[i] += turnGarrison[i-1];

			if (bombIn == i)
			{
				simulationTurnsUntilProduction = 5;
			}

			int turnProduction = isNeutral() ? 0 : simulationTurnsUntilProduction > 0 ? 0 : (turnGarrison[i] < 0 ? -production : production);
			simulationTurnsUntilProduction--;

			if (bombIn == i)
			{
				int total = abs(turnGarrison[i]);
				if (total > 20)
				{
					turnGarrison[i] /= 2;
				}
				else if (turnGarrison[i] > 10)
				{
					turnGarrison[i] -= 10;
				}
				else if (turnGarrison[i] < -10)
				{
					turnGarrison[i] += 10;
				}
				else
				{
					turnGarrison[i] = 0;
				}
			}
			
			if (turnGarrison[i] >= 0)
			{
				turnGarrison[i] += turnProduction;
			}
			else
			{
				turnGarrison[i] -= turnProduction;
			}
		}
		
		return turnGarrison[turns-1];
	}
	
	bool isMine() const
	{
		return owner == 1;
	}
	
	bool isNeutral() const
	{
		return owner == 0;
	}
	
	bool isTheirs() const
	{
		return owner == -1;
	}
};

vector<Factory> factories;
vector<int> targetIds;
string command;

bool outBool(int a, int b, string output, bool ret)
{
//	cerr << to_string(a) << (ret ? "<" : ">") << to_string(b) << " " << output << endl;
	return ret;
}

// Find factories that can spare some cyborgs
bool orderReinforcer(int a, int b)
{
	if (factories[a].isMine() && !factories[b].isMine()) return outBool(a, b, "mine", true);
	if (!factories[a].isMine() && factories[b].isMine()) return outBool(a, b, "mine", false);

	if (factories[a].danger < factories[b].danger) return outBool(a, b, "danger", true);
	if (factories[a].danger > factories[b].danger) return outBool(a, b, "danger", false);

	if (factories[a].production < factories[b].production) return outBool(a, b, "production", true);
	if (factories[a].production > factories[b].production) return outBool(a, b, "production", false);

	if (factories[a].cyborgs > factories[b].cyborgs) return outBool(a, b, "cyborgs", true);
	if (factories[a].cyborgs < factories[b].cyborgs) return outBool(a, b, "cyborgs", false);

	if (a < b) return outBool(a, b, "value", true);
	if (a > b) return outBool(a, b, "value", false);

	return outBool(a, b, "default", false);
}

// Find factories that need more cyborgs
bool orderReinforcee(Link a, Link b)
{
	if (factories[a.destinationId].isMine() && !factories[b.destinationId].isMine()) return outBool(a.destinationId, b.destinationId, "mine", true);
	if (!factories[a.destinationId].isMine() && factories[b.destinationId].isMine()) return outBool(a.destinationId, b.destinationId, "mine", false);

	if (factories[a.destinationId].danger > factories[b.destinationId].danger) return outBool(a.destinationId, b.destinationId, "danger", true);
	if (factories[a.destinationId].danger < factories[b.destinationId].danger) return outBool(a.destinationId, b.destinationId, "danger", false);

	if (factories[a.destinationId].cyborgsIn(a.distance) < factories[b.destinationId].cyborgsIn(b.distance)) return outBool(a.destinationId, b.destinationId, "cyborgsIn", true);
	if (factories[a.destinationId].cyborgsIn(a.distance) > factories[b.destinationId].cyborgsIn(b.distance)) return outBool(a.destinationId, b.destinationId, "cyborgsIn", false);

	if (a.destinationId < b.destinationId) return outBool(a.destinationId, b.destinationId, "value", true);
	if (a.destinationId > b.destinationId) return outBool(a.destinationId, b.destinationId, "value", false);

	return outBool(a.destinationId, b.destinationId, "default", false);
}

// Find highly desirable targets
bool orderAttackee(int a, int b)
{
	if (!factories[a].isMine() && factories[b].isMine()) return outBool(a, b, "mine", true);
	if (factories[a].isMine() && !factories[b].isMine()) return outBool(a, b, "mine", false);

	if (factories[a].danger < factories[b].danger) return outBool(a, b, "danger", true);
	if (factories[a].danger > factories[b].danger) return outBool(a, b, "danger", false);

	if (factories[a].production > factories[b].production) return outBool(a, b, "production", true);
	if (factories[a].production < factories[b].production) return outBool(a, b, "production", false);

	if (factories[a].cyborgs < factories[b].cyborgs) return outBool(a, b, "cyborgs", true);
	if (factories[a].cyborgs > factories[b].cyborgs) return outBool(a, b, "cyborgs", false);

	if (factories[a].isTheirs() && factories[b].isNeutral()) return outBool(a, b, "theirs", true);
	if (factories[a].isNeutral() && factories[b].isTheirs()) return outBool(a, b, "theirs", false);

	if (a < b) return outBool(a, b, "value", true);
	if (a > b) return outBool(a, b, "value", false);

	return outBool(a, b, "default", false);
}

bool orderAttacker(Link a, Link b)
{
	// Must be mine to attack
	if (factories[a.destinationId].isMine() && !factories[b.destinationId].isMine()) return outBool(a.destinationId, b.destinationId, "mine", true);
	if (!factories[a.destinationId].isMine() && factories[b.destinationId].isMine()) return outBool(a.destinationId, b.destinationId, "mine", false);

	// Must order by distance first so that collaboration calculation happens most effectively
	if (a.distance < b.distance) return outBool(a.destinationId, b.destinationId, "distance", true);
	if (a.distance > b.distance) return outBool(a.destinationId, b.destinationId, "distance", false);

	if (factories[a.destinationId].production < factories[b.destinationId].production) return outBool(a.destinationId, b.destinationId, "production", true);
	if (factories[a.destinationId].production > factories[b.destinationId].production) return outBool(a.destinationId, b.destinationId, "production", false);

	if (factories[a.destinationId].danger < factories[b.destinationId].danger) return outBool(a.destinationId, b.destinationId, "danger", true);
	if (factories[a.destinationId].danger > factories[b.destinationId].danger) return outBool(a.destinationId, b.destinationId, "danger", false);

	if (factories[a.destinationId].cyborgsIn(a.distance) > factories[b.destinationId].cyborgsIn(b.distance)) return outBool(a.destinationId, b.destinationId, "cyborgsIn", true);
	if (factories[a.destinationId].cyborgsIn(a.distance) < factories[b.destinationId].cyborgsIn(b.distance)) return outBool(a.destinationId, b.destinationId, "cyborgsIn", false);

	if (a.destinationId < b.destinationId) return outBool(a.destinationId, b.destinationId, "value", true);
	if (a.destinationId > b.destinationId) return outBool(a.destinationId, b.destinationId, "value", false);

	return outBool(a.destinationId, b.destinationId, "default", false);
}

// Find powerhouses to disrupt
bool orderBombee(int a, int b)
{
	if (factories[a].isTheirs() && !factories[b].isTheirs()) return outBool(a, b, "theirs", true);
	if (!factories[a].isTheirs() && factories[b].isTheirs()) return outBool(a, b, "theirs", false);

	if (factories[a].danger < factories[b].danger) return outBool(a, b, "danger", true);
	if (factories[a].danger > factories[b].danger) return outBool(a, b, "danger", false);

	if (factories[a].cyborgs > factories[b].cyborgs) return outBool(a, b, "cyborgs", true);
	if (factories[a].cyborgs < factories[b].cyborgs) return outBool(a, b, "cyborgs", false);

	if (factories[a].production > factories[b].production) return outBool(a, b, "production", true);
	if (factories[a].production < factories[b].production) return outBool(a, b, "production", false);

	if (a < b) return outBool(a, b, "value", true);
	if (a > b) return outBool(a, b, "value", false);

	return outBool(a, b, "default", false);
}

bool orderBomber(Link a, Link b)
{
	if (factories[a.destinationId].isMine() && !factories[b.destinationId].isMine()) return outBool(a.destinationId, b.destinationId, "mine", true);
	if (!factories[a.destinationId].isMine() && factories[b.destinationId].isMine()) return outBool(a.destinationId, b.destinationId, "mine", false);

	if (a.distance < b.distance) return outBool(a.destinationId, b.destinationId, "distance", true);
	if (a.distance > b.distance) return outBool(a.destinationId, b.destinationId, "distance", false);

	if (a.destinationId < b.destinationId) return outBool(a.destinationId, b.destinationId, "value", true);
	if (a.destinationId > b.destinationId) return outBool(a.destinationId, b.destinationId, "value", false);

	return outBool(a.destinationId, b.destinationId, "default", false);
}

bool orderId(Link a, Link b)
{
	return a.destinationId < b.destinationId;
}

void setup()
{
	srand (time(NULL));

	myBombs.push_back(Bomb(1));
	myBombs.push_back(Bomb(1));
	theirBombs.push_back(Bomb(-1));
	theirBombs.push_back(Bomb(-1));

    int factoryCount; // the number of factories
    cin >> factoryCount; cin.ignore();
    for (int i = 0; i < factoryCount; i++)
	{
		factories.push_back(Factory(i));
		targetIds.push_back(i);
	}

    int linkCount; // the number of links between factories
    cin >> linkCount; cin.ignore();
    for (int i = 0; i < linkCount; i++)
	{
        int factory1;
        int factory2;
        int distance;
        cin >> factory1 >> factory2 >> distance; cin.ignore();
		
		factories[factory1].addLink(Link(factory2, distance));
		factories[factory2].addLink(Link(factory1, distance));
    }

	leftArm.push_back("___");
	leftArm.push_back("\\__");
	leftArm.push_back("¯\\_");
	leftArm.push_back("¯¯\\");
	leftArm.push_back("/¯\\");
	rightArm.push_back("___");
	rightArm.push_back("__/");
	rightArm.push_back("_/¯");
	rightArm.push_back("/¯¯");
	rightArm.push_back("/¯\\");
	face.push_back(".o]]]");
	face.push_back("o.o]]");
	face.push_back("(o.o]");
	face.push_back("((o.o");
	face.push_back("(((o.");
	blink.push_back(".-]]]");
	blink.push_back("-.-]]");
	blink.push_back("(-.-]");
	blink.push_back("((-.-");
	blink.push_back("(((-.");
}

void initialize()
{
	troops.clear();
	myBombs[0].live = false;
	myBombs[1].live = false;
	theirBombs[0].live = false;
	theirBombs[1].live = false;
	
	cerr << leftArmIndex << " " << faceIndex << " " << rightArmIndex << " " << endl;
	command = "MSG " + leftArm[leftArmIndex] + (rand() % 10 == 0 ? blink[faceIndex] : face[faceIndex]) + rightArm[rightArmIndex] + "                                            ";
	command += outputString.substr(outputBegin, OUTPUT_LENGTH);

	outputBegin += SCROLL_SPEED;
	if (outputBegin >= outputString.length())
	{
		outputBegin = 0;
	}

	int delta = rand() % 3 - 1;
	leftArmIndex += delta;
	leftArmIndex %= leftArm.size();
	delta = rand() % 3 - 1;
	rightArmIndex += delta; 
	rightArmIndex %= rightArm.size();
	delta = rand() % 3 - 1;
	faceIndex += delta;
	if (faceIndex < 0)
	{
		faceIndex = 0;
	}
	if (faceIndex >= face.size())
	{
		faceIndex = face.size() - 1;
	}
}

void update()
{
    int entityCount; // the number of entities (e.g. factories and troops)
    cin >> entityCount; cin.ignore();
    for (int i = 0; i < entityCount; i++)
	{
        int entityId;

        string entityType;
        int arg1;
        int arg2;
        int arg3;
        int arg4;
        int arg5;
        cin >> entityId >> entityType >> arg1 >> arg2 >> arg3 >> arg4 >> arg5; cin.ignore();
		
		if (entityType == "FACTORY")
		{
			factories[entityId].update(arg1, arg2, arg3, arg4);
		}
		else if (entityType == "TROOP")
		{
			Troop troop = Troop(entityId);
			troop.update(arg1, arg2, arg3, arg4, arg5);
			troops.push_back(troop);
		}
		else if (entityType == "BOMB")
		{
			bool bombCatalogued = false;
			if (!bombCatalogued && theirBombs[0].id == entityId)
			{
				bombCatalogued = true;
				theirBombs[0].update(entityId, arg2, arg3, arg4);

				// arg2 is the source of the bomb
				for (vector<Link>::iterator linkIt = factories[arg2].links.begin(); linkIt != factories[arg2].links.end(); ++linkIt)
				{
					factories[(*linkIt).destinationId].updateEvacuation(entityId, (*linkIt).distance - 1);
				}
			}
			if (!bombCatalogued && theirBombs[1].id == entityId)
			{
				bombCatalogued = true;
				theirBombs[1].update(entityId, arg2, arg3, arg4);

				// arg2 is the source of the bomb
				for (vector<Link>::iterator linkIt = factories[arg2].links.begin(); linkIt != factories[arg2].links.end(); ++linkIt)
				{
					factories[(*linkIt).destinationId].updateEvacuation(entityId, (*linkIt).distance - 1);
				}
			}
			if (!bombCatalogued && myBombs[0].id == entityId)
			{
				bombCatalogued = true;
				myBombs[0].update(entityId, arg2, arg3, arg4 - 1);

				factories[arg3].updateEvacuation(entityId, arg4);
			}
			if (!bombCatalogued && myBombs[1].id == entityId)
			{
				bombCatalogued = true;
				myBombs[1].update(entityId, arg2, arg3, arg4);

				factories[arg3].updateEvacuation(entityId, arg4 - 1);
			}
			if (!bombCatalogued)
			{
				if (arg1 == -1)
				{
					if (!theirBombs[0].used)
					{
						bombCatalogued = true;
						theirBombs[0].update(entityId, arg2, arg3, arg4);
					}
					else
					{
						bombCatalogued = true;
						theirBombs[1].update(entityId, arg2, arg3, arg4);

					}

					// arg2 is the source of the bomb
					for (vector<Link>::iterator linkIt = factories[arg2].links.begin(); linkIt != factories[arg2].links.end(); ++linkIt)
					{
						factories[(*linkIt).destinationId].updateEvacuation(entityId, (*linkIt).distance - 1);
					}
				}
				else
				{
					if (myBombs[0].pendingUpdate)
					{
						bombCatalogued = true;
						myBombs[0].update(entityId, arg2, arg3, arg4);
					}
					else if (myBombs[1].pendingUpdate)
					{
						bombCatalogued = true;
						myBombs[1].update(entityId, arg2, arg3, arg4);
					}

					factories[arg3].updateEvacuation(entityId, arg4 - 1);
				}
			}
		}
    }
	
	for (vector<Troop>::iterator troopIt = troops.begin(); troopIt != troops.end(); ++troopIt)
	{
		Troop thisTroop = *troopIt;
		factories[thisTroop.destination].incoming.push_back(thisTroop);
	}
	
	for (int b = 0; b < 2; b++)
	{
		if (myBombs[b].live)
		{
			factories[myBombs[b].destination].bombIn = myBombs[b].turns;
		}
	}

	for (vector<Factory>::iterator factoryIt = factories.begin(); factoryIt != factories.end(); ++factoryIt)
	{
		int i = (*factoryIt).id;
		for (vector<Link>::iterator linkIt = factories[i].links.begin(); linkIt != factories[i].links.end(); ++linkIt)
		{
			if (factories[(*linkIt).destinationId].isTheirs())
			{
				factories[i].enemyNeighbors += factories[(*linkIt).destinationId].cyborgs;
				factories[i].danger += factories[(*linkIt).destinationId].cyborgs * (MAX_DISTANCE + 1 - (*linkIt).distance);
			}
			else if (factories[(*linkIt).destinationId].isMine())
			{
				factories[i].danger -= factories[(*linkIt).destinationId].cyborgs * (MAX_DISTANCE + 1 - (*linkIt).distance);
			}
		}
	}
}

struct AttackForce
{
	int fromId;
	int toId;
	int distance;
	int cyborgs;
};

string printVector(vector<int> v)
{
	string output = "";
	for (vector<int>::iterator it = v.begin(); it != v.end(); ++it)
	{
		output += to_string(*it) + " ";
	}
	
	return output;
}

string printVector(vector<Link> v)
{
	string output = "";
	for (vector<Link>::iterator it = v.begin(); it != v.end(); ++it)
	{
		output += to_string((*it).destinationId) + " ";
	}
	
	return output;
}

void attackTargets()
{
	sort(targetIds.begin(), targetIds.end(), orderAttackee);
//cerr << "Attackees: " << printVector(targetIds) << endl;
	for (vector<int>::iterator targetIt = targetIds.begin(); targetIt != targetIds.end(); ++targetIt)
	{
		int t = *targetIt;

		if (!factories[t].isMine())
		{
			sort(factories[t].links.begin(), factories[t].links.end(), orderAttacker);
//cerr << "Attackers of " << t << ": " << printVector(factories[t].links) << endl;
			vector<AttackForce> attackForces;
			int totalAttackForce = 0;
			int furthestDistance = 0;
			int shortestDistance = MAX_DISTANCE;
			int captureForce = 0;
			bool launchAttack = false;

			for (vector<Link>::iterator attackerIt = factories[t].links.begin(); !launchAttack && attackerIt != factories[t].links.end(); ++attackerIt)
			{
				int a = (*attackerIt).destinationId;
				if (factories[a].isMine())
				{
					// Distance + 1 turn for the command
					int troopDistance = (*attackerIt).distance + 1;
					int troopsToSpare = factories[a].cyborgsIn(troopDistance) > 0 ? factories[a].cyborgs : 0;
					if (troopsToSpare > 0)
					{
						AttackForce force;
						force.cyborgs = troopsToSpare;
						force.distance = troopDistance;
						force.fromId = a;
						force.toId = t;
						attackForces.push_back(force);
						totalAttackForce += troopsToSpare;
						if (furthestDistance < troopDistance)
						{
							furthestDistance = troopDistance;
						}
						if (shortestDistance > troopDistance)
						{
							shortestDistance = troopDistance;
						}
					}
					
					int garrison = factories[t].cyborgsIn(troopDistance);
					if (garrison >= 0)
					{
						captureForce = garrison + 1;
						if (factories[t].production == 0)
						{
							// Only capture non-producing factories if I can capture and increment them
							captureForce += DUD_CAPTURE_THRESHOLD;
						}
					}
					
					if (totalAttackForce >= captureForce)
					{
						launchAttack = true;
					}
				}
			}

			if (launchAttack && !factories[t].bombArriveIn(furthestDistance))
			{
				int sentForce = 0;
				for (vector<AttackForce>::iterator attackerIt = attackForces.begin(); attackerIt != attackForces.end(); ++attackerIt)
				{
					if (captureForce - sentForce > 0)
					{
						int a = (*attackerIt).fromId;
						if ((*attackerIt).distance == furthestDistance)
						{
							int thisForce = min((*attackerIt).cyborgs, captureForce - sentForce);
							if (factories[a].evacuateNow || factories[t].production > factories[a].production)
							{
								thisForce = (*attackerIt).cyborgs;
							}
							command += ";MOVE " + to_string(a) + " " + to_string(t) + " " + to_string(thisForce);
							factories[a].cyborgs -= thisForce;
							Troop newTroop = Troop();
							newTroop.destination = t;
							newTroop.cyborgs = thisForce;
							troops.push_back(newTroop);
							sentForce += thisForce;
						}
					}
				}
			}
		}
	}
}

void reinforce()
{
	sort(targetIds.begin(), targetIds.end(), orderReinforcer);
//cerr << "Reinforcers: " << printVector(targetIds) << endl;
	for (vector<int>::iterator reinforcerIt = targetIds.begin(); reinforcerIt != targetIds.end(); ++reinforcerIt)
	{
		int i = *reinforcerIt;
		if (factories[i].isMine())
		{
			sort(factories[i].links.begin(), factories[i].links.end(), orderReinforcee);
//cerr << "Reinforcees of " << i << ": " << printVector(factories[i].links) << endl;
			for (vector<Link>::iterator linkIt = factories[i].links.begin(); linkIt != factories[i].links.end(); ++linkIt)
			{
				int destination = (*linkIt).destinationId;
				int turnsAway = (*linkIt).distance + 1;
				
				if (factories[destination].isMine())
				{
					int garrisonAtArrival = factories[destination].cyborgsIn(turnsAway);
					int amount = 0;
					if (garrisonAtArrival < 0)
					{
						// Save endangered
						amount = factories[i].evacuateNow ? factories[i].cyborgs : -garrisonAtArrival;
						if (factories[i].cyborgsIn(turnsAway) - amount < 0)
						{
							// Cancel if unable to save
							amount = 0;
						}
					}
					else if (factories[i].enemyNeighbors == 0 && garrisonAtArrival < factories[i].cyborgs)
					{
						// Push toward frontier
						amount = factories[i].cyborgs;
						if (factories[i].cyborgsIn(turnsAway) - amount < 0)
						{
							amount = 0;
						}
					}

					if (amount > 0 && factories[i].cyborgs - amount > 0)
					{
						command += ";MOVE " + to_string(i) + " " + to_string(destination) + " " + to_string(amount);
						factories[i].cyborgs -= amount;
						Troop newTroop = Troop();
						newTroop.destination = destination;
						newTroop.cyborgs = amount;
						troops.push_back(newTroop);
					}
				}
			}
		}
	}
}

void sendBombs()
{
	// Send bomb
	sort(targetIds.begin(), targetIds.end(), orderBombee);
//cerr << "Bombees: " << printVector(targetIds) << (myBombs[0].used ? "x" : "O") << (myBombs[1].used ? "x" : "O") <<  endl;
	for (vector<int>::iterator targetIt = targetIds.begin(); targetIt != targetIds.end(); ++targetIt)
	{
		int t = *targetIt;
		int bombToUse = myBombs[0].used ? 1 : 0;
		if (!myBombs[1].used && (bombToUse == 0 || !myBombs[0].live || myBombs[0].destination != t))
		{
			if (factories[t].isTheirs() && factories[t].production > 0 && factories[t].cyborgs > BOMB_THRESHOLD)
			{
				sort(factories[t].links.begin(), factories[t].links.end(), orderBomber);
//cerr << "Bombers of " << t << ": " << printVector(factories[t].links) << endl;
				int i = (*factories[t].links.begin()).destinationId;
				if (factories[i].isMine())
				{
					command += ";BOMB " + to_string(i) + " " + to_string(t);
					myBombs[bombToUse].used = true;
					myBombs[bombToUse].live = true;
					myBombs[bombToUse].source = i;
					myBombs[bombToUse].destination = t;
					myBombs[bombToUse].turns = (*factories[t].links.begin()).distance;
					myBombs[bombToUse].pendingUpdate = true;
				}
			}
		}
	}
}

void increaseProduction()
{
	// Increase production
	for (vector<int>::iterator targetIt = targetIds.begin(); targetIt != targetIds.end(); ++targetIt)
	{
		int i = *targetIt;
		if (factories[i].isMine()
		 && factories[i].production < 3
		 && factories[i].cyborgsIn(INCREMENT_SACRIFICE) > 0
		 && ((factories[i].cyborgs >= INCREMENT_SACRIFICE && factories[i].enemyNeighbors == 0)
		   || factories[i].cyborgs >= INCREMENT_THRESHOLD))
		{
			command += ";INC " + to_string(i);
			factories[i].cyborgs -= INCREMENT_SACRIFICE;
		}
	}
}

void evacuate()
{
	// Increase production
	for (vector<int>::iterator targetIt = targetIds.begin(); targetIt != targetIds.end(); ++targetIt)
	{
		int i = *targetIt;
		if (factories[i].evacuateNow && factories[i].isMine() && factories[i].cyborgs > 0)
		{
			int shortestDistance = MAX_DISTANCE + 1;
			int evacuateTo = (*factories[i].links.begin()).destinationId;
			for (vector<Link>::iterator linkIt = factories[i].links.begin(); linkIt != factories[i].links.end(); ++linkIt)
			{
				if (shortestDistance > (*linkIt).distance)
				{
					shortestDistance = (*linkIt).distance;
					evacuateTo = (*linkIt).destinationId;
				}
			}
			
			int evacuationForce = factories[i].cyborgs;
			command += ";MOVE " + to_string(i) + " " + to_string(evacuateTo) + " " + to_string(evacuationForce);
			factories[i].cyborgs -= evacuationForce;
			Troop newTroop = Troop();
			newTroop.destination = evacuateTo;
			newTroop.cyborgs = evacuationForce;
			troops.push_back(newTroop);
		}
	}
}

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
	setup();

    // game loop
    while (1)
	{
		initialize();
		update();
		
		attackTargets();
		reinforce();
		sendBombs();
//		increaseProduction();
		evacuate();

        // Any valid action, such as "WAIT" or "MOVE source destination cyborgs"
        cout << command << endl;
    }
}