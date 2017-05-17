#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class Sample
{
	public:
	int sampleId;
	int carriedBy;
	int rank;
	string expertiseGain;
	int health;
	int costA;
	int costB;
	int costC;
	int costD;
	int costE;
	
	Sample()
	{
	}
	
	bool Diagnosed()
	{
		return health >= 0;
	}
};

class Player
{
	public:
	string target;
	int eta;
	int score;
	int storageA;
	int storageB;
	int storageC;
	int storageD;
	int storageE;
	int expertiseA;
	int expertiseB;
	int expertiseC;
	int expertiseD;
	int expertiseE;
	vector<Sample> undiagnosed;
	vector<Sample> samples;
	
	Player()
	{
	}
	
	bool readyToCollect()
	{
		return target == "SAMPLES" && eta == 0;
	}
	
	bool readyToAnalyze()
	{
		return target == "DIAGNOSIS" && eta == 0;
	}
	
	bool readyToGather()
	{
		return target == "MOLECULES" && eta == 0;
	}
	
	bool readyToProduce()
	{
		return target == "LABORATORY" && eta == 0;
	}
	
	bool needToCollect()
	{
		return samples.empty() && undiagnosed.empty();
	}
	
	bool needToAnalyze()
	{
		return samples.empty() && !undiagnosed.empty();
	}
	
	bool needToGather()
	{
		return !samples.empty() && needMolecule() != 0;
	}
	
	bool needToProduce()
	{
		return !samples.empty() && needMolecule() == 0;
	}

	char needMolecule()
	{
		int requiredA = 0;
		int requiredB = 0;
		int requiredC = 0;
		int requiredD = 0;
		int requiredE = 0;
		for (vector<Sample>::iterator sample = samples.begin(); sample != samples.end(); ++sample)
		{
			requiredA += sample->costA;
			requiredB += sample->costB;
			requiredC += sample->costC;
			requiredD += sample->costD;
			requiredE += sample->costE;
		}
		
		if (requiredA > storageA + expertiseA)
		{
			return 'A';
		}
		if (requiredB > storageB + expertiseB)
		{
			return 'B';
		}
		if (requiredC > storageC + expertiseC)
		{
			return 'C';
		}
		if (requiredD > storageD + expertiseD)
		{
			return 'D';
		}
		if (requiredE > storageE + expertiseE)
		{
			return 'E';
		}
		return 0;
	}
};

/**
 * Bring data on patient samples from the diagnosis machine to the laboratory with enough molecules to produce medicine!
 **/
int main()
{
    int projectCount;
    cin >> projectCount; cin.ignore();
    for (int i = 0; i < projectCount; i++) {
        int a;
        int b;
        int c;
        int d;
        int e;
        cin >> a >> b >> c >> d >> e; cin.ignore();
    }
	
	Player players[2];
	vector<Sample> cloud;

    // game loop
    while (1) {
        for (int i = 0; i < 2; i++) {
            string target;
            int eta;
            int score;
            int storageA;
            int storageB;
            int storageC;
            int storageD;
            int storageE;
            int expertiseA;
            int expertiseB;
            int expertiseC;
            int expertiseD;
            int expertiseE;
            cin >> target >> eta >> score >> storageA >> storageB >> storageC >> storageD >> storageE >> expertiseA >> expertiseB >> expertiseC >> expertiseD >> expertiseE; cin.ignore();
			players[i].target = target;
			players[i].eta = eta;
			players[i].eta = eta;
			players[i].storageA = storageA;
			players[i].storageB = storageB;
			players[i].storageC = storageC;
			players[i].storageD = storageD;
			players[i].storageE = storageE;
			players[i].expertiseA = expertiseA;
			players[i].expertiseB = expertiseB;
			players[i].expertiseC = expertiseC;
			players[i].expertiseD = expertiseD;
			players[i].expertiseE = expertiseE;
        }
        int availableA;
        int availableB;
        int availableC;
        int availableD;
        int availableE;
        cin >> availableA >> availableB >> availableC >> availableD >> availableE; cin.ignore();
        int sampleCount;
        cin >> sampleCount; cin.ignore();
		cloud.clear();
		players[0].samples.clear();
		players[1].samples.clear();
		players[0].undiagnosed.clear();
		players[1].undiagnosed.clear();
        for (int i = 0; i < sampleCount; i++) {
            int sampleId;
            int carriedBy;
            int rank;
            string expertiseGain;
            int health;
            int costA;
            int costB;
            int costC;
            int costD;
            int costE;
            cin >> sampleId >> carriedBy >> rank >> expertiseGain >> health >> costA >> costB >> costC >> costD >> costE; cin.ignore();
			Sample sample;
			sample.sampleId = sampleId;
			sample.carriedBy = carriedBy;
			sample.rank = rank;
			sample.expertiseGain = expertiseGain;
			sample.health = health;
			sample.costA = costA;
			sample.costB = costB;
			sample.costC = costC;
			sample.costD = costD;
			sample.costE = costE;
			if (sample.carriedBy == -1)
			{
				cloud.push_back(sample);
			}
			else
			{
				if (sample.Diagnosed())
				{
					players[sample.carriedBy].samples.push_back(sample);
				}
				else
				{
					players[sample.carriedBy].undiagnosed.push_back(sample);
				}
			}
        }
		
		if (players[0].needToAnalyze() || !cloud.empty())
		{
			if (players[0].readyToAnalyze())
			{
				if (!cloud.empty())
				{
					// Get a sample
					cout << "CONNECT " << cloud[0].sampleId << endl;
				}
				else
				{
					// Diagnose a sample
					cout << "CONNECT " << players[0].undiagnosed[0].sampleId << endl;
				}
			}
			else
			{
				// Go to station
				cout << "GOTO DIAGNOSIS" << endl;
			}
		}
		else if (players[0].needToCollect())
		{
			if (players[0].readyToCollect())
			{
				// Get a sample
				cout << "CONNECT " << 1 << endl;
			}
			else
			{
				// Go to station
				cout << "GOTO SAMPLES" << endl;
			}
		}
		else if (players[0].needToGather())
		{
			if (players[0].readyToGather())
			{
				// Get molecules
				cout << "CONNECT " << players[0].needMolecule() << endl;
			}
			else
			{
				// Go to station
				cout << "GOTO MOLECULES" << endl;
			}
		}
		else if (players[0].needToProduce())
		{
			if (players[0].readyToProduce())
			{
				// Produce medicine
				cout << "CONNECT " << players[0].samples[0].sampleId << endl;
			}
			else
			{
				// Go to station
				cout << "GOTO LABORATORY" << endl;
			}
		}

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        
    }
}