#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class Group
{
	public:
	long long int people;
	long long int valueIfFirst;
	int numGroupsRide;
	
	Group() : people(0), valueIfFirst(0), numGroupsRide(0) {}
	
	Group(long long int people) : people(people), valueIfFirst(0), numGroupsRide(0) {}
};

long long int rideTrain(vector<Group*>* groups, int spaces, int &head)
{
	long long int total = 0;
	int numGroups = groups->size();

	// If this group has already been first, then we are repeating from an earlier sequence and can skip to the good stuff
	Group* firstGroup = (*groups)[head];
	if (firstGroup->valueIfFirst > 0)
	{
		total = firstGroup->valueIfFirst;
		head = (head + firstGroup->numGroupsRide) % numGroups;
		return total;
	}

	int totalGroupsRide = 0;

	Group* group;

	// Don't let the same group ride twice on the same ride
	for (int i = 0; i < numGroups; i++)
	{
		group = (*groups)[head];
		int peopleInGroup = group->people;
		
		// If there is space
		if (total + peopleInGroup <= spaces)
		{
			// Board the train
			head = (head + 1) % numGroups;
			
			// Collect admission from this group
			total += peopleInGroup;
		}
		else
		{
			// No space for more groups, start the train
			totalGroupsRide = i;
			break;
		}
	}
	
	firstGroup->valueIfFirst = total;
	firstGroup->numGroupsRide = totalGroupsRide;

	// Deposit total admission
	return total;
}

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
	vector<Group*> groups;
    int L;
    int C;
    int N;
    cin >> L >> C >> N; cin.ignore();
    
    cerr << "L" << L << "C" << C << "N" << N << endl;
    for (int i = 0; i < N; i++)
	{
        int Pi;
        cin >> Pi; cin.ignore();
		
		groups.push_back(new Group(Pi));
    }
	
	long long int dayTotal = 0;
	int head = 0;
	for (int i = 0; i < C; i++)
	{
		dayTotal += rideTrain(&groups, L, head);
	}

    // Write an action using cout. DON'T FORGET THE "<< endl"
    // To debug: cerr << "Debug messages..." << endl;

    cout << dayTotal << endl;
}