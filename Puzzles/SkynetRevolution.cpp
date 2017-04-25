#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/*
STRATEGY:
A) Next to a gate:
1) Cut the link.
B) Double-gate nodes exist:
1) Find all nodes with double-gates.
2) Find all nodes with single-gates on the path to the double-gates.
3) Find the closest beginning of the double gate path.
4) Cut the double gate at the end of that path.
C) Only single-gate nodes exist:
1) Cut closest single-gate.
*/

class Link;

class INode
{
    public:
	virtual void addLink(Link* link) = 0;
	virtual void removeLink(Link* link) = 0;
	virtual int getName() = 0;
	virtual int getDistanceFromAgent() = 0;
	virtual bool agentCanAccess() = 0;
	virtual bool isOnDoubleGatePath() = 0;
	virtual int getDoubleGateTarget() = 0;
	virtual bool isGate() = 0;
	virtual int connectedGates() = 0;
	virtual void setGate() = 0;
	virtual void setDistance(int distance) = 0;
	virtual Link* findGateLink() = 0;
	virtual void resetDistanceFromAgent() = 0;
	virtual void resetDoubleGate() = 0;
	virtual void fillDistance(int newDistance) = 0;
	virtual void fillDoubleGate(int target) = 0;
	virtual Link* gateLink() = 0;
};

vector<INode*> nodes;
vector<int> exclude;

class Link
{
    public:
    bool gateConnected;
	int name;
    vector<INode*> nodes;
	
	Link() : gateConnected(false)
	{}
	
	Link(INode* A, INode* B, int _name) : Link()
	{
		name = _name;
		nodes.push_back(A);
		nodes.push_back(B);
		A->addLink(this);
		B->addLink(this);
	}
	
	~Link()
	{
		nodes[0]->removeLink(this);
		nodes[1]->removeLink(this);
	}
	
	INode* otherNode(INode* myNode)
	{
		if (nodes[0]->getName() == myNode->getName())
		{
			return nodes[1];
		}
		
		return nodes[0];
	}
	
	string output()
	{
		return to_string(nodes[0]->getName()) + " " + to_string(nodes[1]->getName());
	}

};



class Node : public INode
{
	public:
	int name;
    bool gate;
    int distanceFromAgent;
    int doubleGateTarget;
    vector<Link*> links;
	
	Node() : gate(false)
	{}
	
	Node(int _name) : Node()
	{
		name = _name;
		resetDistanceFromAgent();
		resetDoubleGate();
	}
	
	int getName()
	{
		return name;
	}
	
	bool agentCanAccess()
	{
		return distanceFromAgent >= 0;
	}
	
	int getDistanceFromAgent()
	{
		return distanceFromAgent;
	}
	
	bool isOnDoubleGatePath()
	{
		return agentCanAccess() && doubleGateTarget >= 0;
	}
	
	int getDoubleGateTarget()
	{
		return doubleGateTarget;
	}
	
	bool isGate()
	{
		return gate;
	}
	
	void resetDistanceFromAgent()
	{
		distanceFromAgent = -1;
	}
	
	void resetDoubleGate()
	{
		doubleGateTarget = -1;
	}
	
	void removeLink(Link* link)
	{
		for (vector<Link*>::iterator it = links.begin(); it != links.end(); ++it)
		{
			if ((*it)->name == link->name)
			{
				links.erase(it);
				break;
			}
		}
	}
	
	Link* gateLink()
	{
		for (vector<Link*>::iterator it = links.begin(); it != links.end(); ++it)
		{
			if ((*it)->gateConnected)
			{
				return (*it);
			}
		}
		
		return NULL;
	}
	
	int connectedGates()
	{
		int totalConnectedGates = 0;
		if (!gate)
		{
			for (vector<Link*>::iterator it = links.begin(); it != links.end(); ++it)
			{
				if ((*it)->gateConnected)
				{
					totalConnectedGates++;
				}
			}
		}
		
		return totalConnectedGates;
	}
	
	bool checkDoubleGatePath()
	{
		for (vector<Link*>::iterator it = links.begin(); !isOnDoubleGatePath() && it != links.end(); ++it)
		{
			INode* otherNode = (*it)->otherNode(this);
			if (otherNode->isOnDoubleGatePath())
			{
				doubleGateTarget = otherNode->getDoubleGateTarget();
			}
		}

		return isOnDoubleGatePath();
	}
	
	Link* findGateLink()
	{
		for (vector<Link*>::iterator it = links.begin(); it != links.end(); ++it)
		{
			if ((*it)->gateConnected)
			{
				return (*it);
			}
		}
		
		return NULL;
	}

	void addLink(Link* link)
	{
		links.push_back(link);
	}
	
	void fillDistance(int newDistance)
	{
		distanceFromAgent = newDistance;
		int nextDistance = distanceFromAgent + 1;
		exclude.push_back(name);
		for (vector<Link*>::iterator it = links.begin(); it != links.end(); ++it)
		{
			INode* other = (*it)->otherNode(this);
			if (!other->agentCanAccess() || other->getDistanceFromAgent() > nextDistance)
			{
				other->setDistance(distanceFromAgent + 1);
				other->fillDistance(nextDistance);
			}
		}
/*
		for (vector<Link*>::iterator it = links.begin(); it != links.end(); ++it)
		{
			INode* other = (*it)->otherNode(this);
			vector<int>::iterator test = find (exclude.begin(), exclude.end(), other->getName());
			if (test == exclude.end())
			{
				other->fillDistance(distanceFromAgent + 1);
			}
		}
*/
	}
	
	void fillDoubleGate(int target)
	{
		if (isOnDoubleGatePath())
		{
			if (nodes[doubleGateTarget]->getDistanceFromAgent() > nodes[target]->getDistanceFromAgent())
			{
				doubleGateTarget = target;
			}
		}
		else
		{
			doubleGateTarget = target;
		}

		if (isOnDoubleGatePath())
		{
			exclude.push_back(name);
			for (vector<Link*>::iterator it = links.begin(); it != links.end(); ++it)
			{
				INode* other = (*it)->otherNode(this);
				vector<int>::iterator test = find (exclude.begin(), exclude.end(), other->getName());
				if (test == exclude.end() && other->connectedGates() > 0)
				{
					other->fillDoubleGate(doubleGateTarget);
				}
			}
		}
	}

	void setGate()
	{
		gate = true;
		for (vector<Link*>::iterator it = links.begin(); it != links.end(); ++it)
		{
			(*it)->gateConnected = true;
		}
	}
	
	void setDistance(int distance)
	{
		distanceFromAgent = distance;
	}
};


bool fillDoubleGatePaths()
{
	exclude.clear();
	for (vector<INode*>::iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		(*it)->resetDoubleGate();
	}

	bool doubleGates = false;
	// Mark double gate paths
	for (vector<INode*>::iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		int connectedGates = (*it)->connectedGates();
		if (connectedGates > 1)
		{
			(*it)->fillDoubleGate((*it)->getName());
			doubleGates = true;
		}
	}
	
	return doubleGates;
}	
	
void fillDistances(INode* agentNode)
{
	for (vector<INode*>::iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		(*it)->resetDistanceFromAgent();
	}
	exclude.clear();
	agentNode->fillDistance(0);
}

Link* findDoubleLinkPath()
{
	Link* removeMe = NULL;
	int doubleGateTarget = -1;
	int distance = 999999999;
	for (vector<INode*>::iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		if ((*it)->isOnDoubleGatePath() && (*it)->getDistanceFromAgent() < distance)
		{
			distance = (*it)->getDistanceFromAgent();
			doubleGateTarget = (*it)->getDoubleGateTarget();
		}
	}

	if (doubleGateTarget >= 0)
	{
		removeMe = nodes[doubleGateTarget]->gateLink();
	}

	
	return removeMe;
}


Link* findNearestSingleLink()
{
	Link* removeMe = NULL;
	int distance = 999999999;
	for (vector<INode*>::iterator it = nodes.begin(); it != nodes.end(); ++it)
	{
		int nodeDistance = (*it)->getDistanceFromAgent();
		if ((*it)->isGate() && nodeDistance < distance && nodeDistance >= 0)
		{
			distance = nodeDistance;
			removeMe = (*it)->gateLink();
		}
	}
	
	return removeMe;
}

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
    int N; // the total number of nodes in the level, including the gateways
    int L; // the number of links
    int E; // the number of exit gateways
    cin >> N >> L >> E; cin.ignore();
    for (int i = 0; i < N; i++)
	{
		nodes.push_back(new Node(i));
	}

    for (int i = 0; i < L; i++)
	{
        int N1; // N1 and N2 defines a link between these nodes
        int N2;
        cin >> N1 >> N2; cin.ignore();
		new Link(nodes[N1], nodes[N2], i);
    }
    for (int i = 0; i < E; i++)
	{
        int EI; // the index of a gateway node
        cin >> EI; cin.ignore();
		
		nodes[EI]->setGate();
    }

    // game loop
    while (1)
	{
        int SI; // The index of the node on which the Skynet agent is positioned this turn
        cin >> SI; cin.ignore();

		Link* removableLink = nodes[SI]->findGateLink();
		
		if (removableLink == NULL)
		{
			fillDistances(nodes[SI]);
			
			if (fillDoubleGatePaths())
			{
				removableLink = findDoubleLinkPath();
			}
			else
			{
				removableLink = findNearestSingleLink();
			}
		}

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

		if (removableLink != NULL)
		{
			cout << removableLink->output() << endl; // Example: 3 4 are the indices of the nodes you wish to sever the link between
			delete(removableLink);
		}
		else
		{
			cout << "ERROR" << endl;
		}
   }
}