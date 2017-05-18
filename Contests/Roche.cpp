#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

#define MAX_SAMPLES 3
#define MAX_MOLECULES 10

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

	Sample& operator=(const Sample& other)
	{
		sampleId = other.sampleId;
		carriedBy = other.carriedBy;
		rank = other.rank;
		expertiseGain = other.expertiseGain;
		health = other.health;
		costA = other.costA;
		costB = other.costB;
		costC = other.costC;
		costD = other.costD;
		costE = other.costE;
		return *this;
	}

	bool Diagnosed() const
	{
		return health >= 0;
	}
	
	bool canBeMadeWith(int A, int B, int C, int D, int E) const
	{
		return A >= costA && B >= costB && C >= costC && D >= costD && E >= costE;
	}
};

int availableA;
int availableB;
int availableC;
int availableD;
int availableE;
vector<Sample> cloud;

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

	bool canMakeAfterGather(Sample sample) const
	{
		return sample.canBeMadeWith(storageA + expertiseA + availableA,
									storageB + expertiseB + availableB,
									storageC + expertiseC + availableC,
									storageD + expertiseD + availableD,
									storageE + expertiseE + availableE);
	}

	bool canProduce(Sample sample) const
	{
		return sample.canBeMadeWith(storageA + expertiseA,
									storageB + expertiseB,
									storageC + expertiseC,
									storageD + expertiseD,
									storageE + expertiseE);
	}

	int heldMolecules() const
	{
		return storageA + storageB + storageC + storageD + storageE;
	}
	
	int heldSamples() const
	{
		return undiagnosed.size() + samples.size();
	}
	
	bool readyToCollect() const
	{
		return target == "SAMPLES" && eta == 0 && needToCollect();
	}
	
	bool readyToAnalyze() const
	{
		return target == "DIAGNOSIS" && eta == 0 && needToAnalyze();
	}
	
	bool readyToGather() const
	{
		return target == "MOLECULES" && eta == 0 && needToGather();
	}
	
	bool readyToProduce() const
	{
		return target == "LABORATORY" && eta == 0 && needToProduce();
	}
		
	bool needToCollect() const
	{
		return heldSamples() < MAX_SAMPLES && samples.empty();
	}
	
	bool needToAnalyze() const
	{
		return !undiagnosed.empty() || unmakeableSample() >= 0;
	}
	
	bool needToGather() const
	{
		return !samples.empty() && canGatherMolecule(needMolecule()) && heldMolecules() < MAX_MOLECULES;
	}
	
	bool needToProduce() const
	{
		return !samples.empty() && needMolecule() == 0;
	}
	
	bool canGatherMolecule(char molecule) const
	{
		if (molecule == 0) return false;
		switch (molecule)
		{
			case 'A':
				return availableA > 0;
			case 'B':
				return availableB > 0;
			case 'C':
				return availableC > 0;
			case 'D':
				return availableD > 0;
			case 'E':
				return availableE > 0;
			default:
				return false;
		}
	}
	
	int sampleToCollect() const
	{
		bool need1 = true;
		bool need2 = true;
		bool need3 = true;
		for (vector<Sample>::const_iterator sample = samples.begin(); sample != samples.end(); ++sample)
		{
			if (sample->rank == 1)
			{
				need1 = false;
			}
			else if (sample->rank == 2)
			{
				need2 = false;
			}
			else if (sample->rank == 3)
			{
				need3 = false;
			}
		}
		for (vector<Sample>::const_iterator sample = undiagnosed.begin(); sample != undiagnosed.end(); ++sample)
		{
			if (sample->rank == 1)
			{
				need1 = false;
			}
			else if (sample->rank == 2)
			{
				need2 = false;
			}
			else if (sample->rank == 3)
			{
				need3 = false;
			}
		}

		if (need1) return 1;
		if (need2) return 2;
		if (need3) return 3;
	}

	int unmakeableSample() const
	{
		for (vector<Sample>::const_iterator sample = samples.begin(); sample != samples.end(); ++sample)
		{
			if (!canMakeAfterGather(*sample))
			{
				return sample->sampleId;
			}
		}
		
		return -1;
	}

	int sampleToProduce() const
	{
		int bestHealth = samples[0].sampleId;
		int highestHealth = 0;
		for (vector<Sample>::const_iterator sample = samples.begin(); sample != samples.end(); ++sample)
		{
			if (canProduce(*sample))
			{
				if (sample->health > highestHealth)
				{
					highestHealth = sample->health;
					bestHealth = sample->sampleId;
				}
			}
		}

		return bestHealth;
	}
	
	int totalA() const
	{
		return storageA + expertiseA;
	}
	
	int totalB() const
	{
		return storageB + expertiseB;
	}
	
	int totalC() const
	{
		return storageC + expertiseC;
	}
	
	int totalD() const
	{
		return storageD + expertiseD;
	}
	
	int totalE() const
	{
		return storageE + expertiseE;
	}
	
	int distanceToSamples() const
	{
		if (target == "")
		{
			return 2;
		}
		else if (target == "SAMPLES")
		{
			return eta;
		}
		else if (eta > 0)
		{
			return 100000000;
		}
		else
		{
			return 3;
		}
	}
	
	int distanceToDiagnosis() const
	{
		if (target == "")
		{
			return 2;
		}
		else if (target == "DIAGNOSIS")
		{
			return eta;
		}
		else if (eta > 0)
		{
			return 100000000;
		}
		else if (target == "LABORATORY")
		{
			return 4;
		}
		else
		{
			return 3;
		}
	}
	
	int distanceToMolecules() const
	{
		if (target == "")
		{
			return 2;
		}
		else if (target == "MOLECULES")
		{
			return eta;
		}
		else if (eta > 0)
		{
			return 100000000;
		}
		else
		{
			return 3;
		}
	}

	int distanceToLaboratory() const
	{
		if (target == "")
		{
			return 2;
		}
		else if (target == "LABORATORY")
		{
			return eta;
		}
		else if (eta > 0)
		{
			return 100000000;
		}
		else if (target == "DIAGNOSIS")
		{
			return 4;
		}
		else
		{
			return 3;
		}
	}

	char needMolecule() const
	{
		if (heldMolecules() >= MAX_MOLECULES)
		{
			return 0;
		}

		int requiredA = 0;
		int requiredB = 0;
		int requiredC = 0;
		int requiredD = 0;
		int requiredE = 0;
		for (vector<Sample>::const_iterator sample = samples.begin(); sample != samples.end(); ++sample)
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
	
	int sampleFromCloud() const
	{
		if (!cloud.empty() && heldSamples() < MAX_SAMPLES)
		{
			// Get a sample from the cloud
			int highestHealth = 0;
			int bestSample = -1;
			for (vector<Sample>::iterator sample = cloud.begin(); sample != cloud.end(); ++sample)
			{
				if (canMakeAfterGather(*sample))
				{
					if (sample->health > highestHealth)
					{
						highestHealth = sample->health;
						bestSample = sample->sampleId;
					}
				}
			}
			
			if (bestSample >= 0)
			{
				return bestSample;
			}
		}
		
		return -1;
	}

	string travelTo() const
	{
		string travelTarget = "ERROR";
		if (needToCollect() && distanceToSamples() < 10)
		{
			travelTarget = "SAMPLES";
		}
		else if ((needToAnalyze() || sampleFromCloud() >= 0) && distanceToDiagnosis() < 10)
		{
			travelTarget = "DIAGNOSIS";
		}
		else if (needToGather() && distanceToMolecules() < 10)
		{
			travelTarget = "MOLECULES";
		}
		else if (needToProduce() && distanceToLaboratory() < 10)
		{
			travelTarget = "LABORATORY";
		}
		
		return travelTarget;
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
		
		// If not at any station
		if (!players[0].readyToCollect() && !players[0].readyToAnalyze() && !players[0].readyToGather() && !players[0].readyToProduce())
		{
			cout << "GOTO " << players[0].travelTo() << endl;
		}
		else if (players[0].readyToCollect())
		{
			cout << "CONNECT " << players[0].sampleToCollect() << endl;
		}
		else if (players[0].readyToAnalyze())
		{
			int cloudSample = players[0].sampleFromCloud();
			int unmakeableSample = players[0].unmakeableSample();
			if (cloudSample >= 0)
			{
				cout << "CONNECT " << cloudSample << endl;
			}
			else if (unmakeableSample >= 0)
			{
				// Upload to cloud
				cout << "CONNECT " << unmakeableSample << endl;
			}
			else
			{
				// Diagnose a sample
				cout << "CONNECT " << players[0].undiagnosed[0].sampleId << endl;
			}
		}
		else if (players[0].readyToGather())
		{
			// Get molecules
			cout << "CONNECT " << players[0].needMolecule() << endl;
		}
		else if (players[0].readyToProduce())
		{
			// Produce medicine
			cout << "CONNECT " << players[0].sampleToProduce() << endl;
		}        
    }
}