#include <cmath>
#include <iostream>
#include <map>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

#define BIG_DISTANCE 9999999999
#define EARTH_RADIUS 6371

const float halfC = 3.14159 / 180.0;

using namespace std;

vector<string> output;

class Stop
{
public:
	string Identifier;
	string Name;
	string Description;
	float Latitude = 0;
	float Longitude = 0;
	string Url;
	string Type;
	string Mother;

	Stop(string stop_name)
	{
		vector<string> result;
		stringstream s_stream(stop_name); //create string stream from the string
		while (s_stream.good())
		{
			string substr;
			getline(s_stream, substr, ','); //get first string delimited by comma
			result.push_back(substr);
		}

		if (result.size() >= 8)
		{
			Identifier = result[0];
			Name = StripDoubleQuotes(result[1]);
			Description = result[2];
			Latitude = stof(result[3]) * halfC;
			Longitude = stof(result[4]) * halfC;
			Url = result[5];
			Type = result[6];
			Mother = result[7];
		}
	}

	void AddLink(Stop* destination)
	{
		_links[destination->Identifier] = make_pair(destination, GetDistance(destination));
	}

	void AddGoal(const Stop* goal)
	{
		_distanceToGoal = GetDistance(goal);
	}

	bool CheckPath(Stop* from)
	{
		float checkDistanceFromStart = from->GetDistanceFromStart() + from->GetDistanceToStop(Identifier);
		if (_distanceFromStart > checkDistanceFromStart)
		{
			_predecessor = (Stop*)from;
			_distanceFromStart = checkDistanceFromStart;
			return true;
		}

		return false;
	}

	float GetDistanceFromStart() const
	{
		return _distanceFromStart;
	}

	void SetDistanceFromStart(float distance)
	{
		_distanceFromStart = distance;
	}

	float GetDistanceToStop(string stopName)
	{
		return _links[stopName].second;
	}

	Stop* Output() const
	{
		output.push_back(Name);
		return _predecessor;
	}

	// This is the F Score
	float BestGuess() const
	{
		return _distanceFromStart + _distanceToGoal;
	}

	vector<Stop*> UpdateNeighbors()
	{
		vector<Stop*> updated;
		for (auto &neighbor : _links)
		{
			Stop* test = neighbor.second.first;
			if (test->CheckPath(this))
			{
				updated.push_back(test);
			}
		}

		return updated;
	}

	bool Processed()
	{
		return _distanceFromStart < BIG_DISTANCE;
	}

private:
	map<string, pair<Stop*, float>> _links;
	float _distanceToGoal = BIG_DISTANCE;
	float _distanceFromStart = BIG_DISTANCE; // This is the G Score
	Stop* _predecessor = NULL;

	float GetDistance(const Stop* destination) const
	{
		float x = (destination->Longitude - Longitude) * cos((Latitude + destination->Latitude) / 2);
		float y = destination->Latitude - Latitude;
		return sqrt(x * x + y * y) * EARTH_RADIUS;
	}

	// This is h(n)
	float Heuristic()
	{
		return _distanceToGoal + _links.size();
	}

	string StripDoubleQuotes(string input) const
	{
		string output;

		for (char c : input)
		{
			if (c != '"')
			{
				output += c;
			}
		}

		return output;
	}
};

class Compare
{
public:
	bool operator() (Stop* first, Stop* second)
	{
		return first->BestGuess() < second->BestGuess();
	}
};

map<string, Stop*> stops;
priority_queue<Stop*, vector<Stop*>, Compare> openSet;

int main()
{
	string start_point;
	cin >> start_point; cin.ignore();
	string end_point;
	cin >> end_point; cin.ignore();
	int n;
	cin >> n; cin.ignore();
	for (int i = 0; i < n; i++)
	{
		string stop_name;
		getline(cin, stop_name);

		string key = stop_name.substr(0, stop_name.find(','));
		stops[key] = new Stop(stop_name);
	}

	int m;
	cin >> m; cin.ignore();
	for (int i = 0; i < m; i++)
	{
		string route;
		getline(cin, route);

		string::size_type separator = route.find(' ');
		string from = route.substr(0, separator);
		string to = route.substr(separator + 1, route.size());
		stops[from]->AddLink(stops[to]);
	}

	Stop* goal = stops[end_point];
	for (pair<std::string, Stop*> node : stops)
	{
		node.second->AddGoal(goal);
	}

	stops[start_point]->SetDistanceFromStart(0);
	openSet.push(stops[start_point]);
	while (!openSet.empty())
	{
		Stop* current = openSet.top();
		openSet.pop();
/*
		if (current->Identifier == end_point)
		{
			while (current != NULL)
			{
				current = current->Output();
			}

			for (vector<string>::reverse_iterator i = output.rbegin(); i != output.rend(); ++i)
			{
				cout << *i << endl;
			}

			exit(0);
		}
		*/
		vector<Stop*> updated = current->UpdateNeighbors();
		for (Stop* update : updated)
		{
			openSet.push(update);
		}
	}

	if (stops[end_point]->Processed())
	{
		Stop* current = stops[end_point];

		while (current != NULL)
		{
			current = current->Output();
		}

		for (vector<string>::reverse_iterator i = output.rbegin(); i != output.rend(); ++i)
		{
			cout << *i << endl;
		}
	}
	else
	{
		cout << "IMPOSSIBLE" << endl;
	}
}