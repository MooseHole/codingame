#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

class Player
{
public:
	int health;
	int mana;
	int deck;
	int rune;
	bool self;

	void setup (int index, int _health, int _mana, int _deck, int _rune)
	{
		self = index == 0;
		health = _health;
		mana = _mana;
		deck = _deck;
		rune = _rune;
	}
};

class Card
{
public:
    int number;
    int instanceId;
    int location;
    int type;
    int cost;
    int attack;
    int defense;
    string abilities;
    int myHealthChange;
    int opponentHealthChange;
    int cardDraw;
	bool breakthrough;
	bool charge;
	bool drain;
	bool guard;
	bool lethal;
	bool ward;

	void setup (int _number, int _instanceId, int _location, int _type, int _cost, int _attack, int _defense, string _abilities, int _myHealthChange, int _opponentHealthChange, int _cardDraw)
	{
		number = _number;
		instanceId = _instanceId;
		location = _location;
		type = _type;
		cost = _cost;
		attack = _attack;
		defense = _defense;
		abilities = _abilities;
		myHealthChange = _myHealthChange;
		opponentHealthChange = _opponentHealthChange;
		cardDraw = _cardDraw;
		breakthrough = _abilities[0] == 'B';
		charge = _abilities[1] == 'C';
		drain = _abilities[2] == 'D';
		guard = _abilities[3] == 'G';
		lethal = _abilities[4] == 'L';
		ward = _abilities[5] == 'W';
	}
	
	int rawWorth() const
	{
		return (12-cost) + (attack*(breakthrough ? 4 : 2)*(charge ? 2 : 1)) + (defense*(guard ? 2 : 1)) + myHealthChange - opponentHealthChange + (cardDraw*5);
	}
};

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
	Player self;
	Player opponent;
	map<int, Card> myHand;
	map<int, Card> mySide;
	map<int, Card> opponentSide;
	int draftCounter = 30;
    // game loop
    while (1)
	{
		bool drafting = draftCounter > 0;
		myHand.clear();
		mySide.clear();
		opponentSide.clear();
        for (int i = 0; i < 2; i++)
		{
            int playerHealth;
            int playerMana;
            int playerDeck;
            int playerRune;
            cin >> playerHealth >> playerMana >> playerDeck >> playerRune; cin.ignore();
			if (i==0)
			{
				self.setup(i, playerHealth, playerMana, playerDeck, playerRune);
			}
			else
			{
				opponent.setup(i, playerHealth, playerMana, playerDeck, playerRune);
			}
        }

        int opponentHand;
        cin >> opponentHand; cin.ignore();
        int cardCount;
        cin >> cardCount; cin.ignore();
        for (int i = 0; i < cardCount; i++)
		{
            int cardNumber;
            int instanceId;
            int location;
            int cardType;
            int cost;
            int attack;
            int defense;
            string abilities;
            int myHealthChange;
            int opponentHealthChange;
            int cardDraw;
            cin >> cardNumber >> instanceId >> location >> cardType >> cost >> attack >> defense >> abilities >> myHealthChange >> opponentHealthChange >> cardDraw; cin.ignore();
			if (drafting)
			{
				myHand[i] = Card();
				myHand[i].setup(cardNumber, instanceId, location, cardType, cost, attack, defense, abilities, myHealthChange, opponentHealthChange, cardDraw);
			}
			else
			{
				Card thisCard;
				thisCard.setup(cardNumber, instanceId, location, cardType, cost, attack, defense, abilities, myHealthChange, opponentHealthChange, cardDraw);
				switch (location)
				{
					case 0:
						myHand[instanceId] = thisCard;
						break;
					case 1:
						mySide[instanceId] = thisCard;
						break;
					case -1:
						opponentSide[instanceId] = thisCard;
						break;
				}
			}
			int key = drafting ? i : instanceId;
        }
		draftCounter--;
		
		string turnOutput = "";
		if (drafting)
		{
			int highestWorth = -1000;
			int bestId = -1;
			for (auto it = myHand.begin(); it != myHand.end(); ++it)
			{
				if(it->second.rawWorth() > highestWorth)
				{
					bestId = it->first;
					highestWorth = it->second.rawWorth();
				}
			}
			
			if (!turnOutput.empty())
			{
				turnOutput += ";";
			}
			turnOutput += "PICK " + std::to_string(bestId);
		}
		else
		{
			for (auto it = myHand.begin(); it != myHand.end(); )
			{
				if (self.mana >= it->second.cost)
				{
					if (!turnOutput.empty())
					{
						turnOutput += ";";
					}
					turnOutput += "SUMMON " + std::to_string(it->first);
					self.mana -= it->second.cost;
					it = myHand.erase(it);
				}
				else
				{
					++it;
				}
			}
			
			for(auto it = mySide.begin(); it != mySide.end(); ++it)
			{
				if (!turnOutput.empty())
				{
					turnOutput += ";";
				}
				turnOutput += "ATTACK " + std::to_string(it->first) + " -1";
			}
		}
			
		if (turnOutput.empty())
		{
			cout << "PASS" << endl;
		}
		else
		{
			cout << turnOutput << endl;
		}
    }
}