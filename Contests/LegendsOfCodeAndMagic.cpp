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
	
	void cast (int cost)
	{
		mana -= cost;
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

	Card()
	{
	}

	Card(const Card &other)
	{
		*this = other;
	}
	
	Card& operator=(const Card &&other)
	{
		number = std::move(other.number);
		instanceId = std::move(other.instanceId);
		location = std::move(other.location);
		type = std::move(other.type);
		cost = std::move(other.cost);
		attack = std::move(other.attack);
		defense = std::move(other.defense);
		abilities = std::move(other.abilities);
		myHealthChange = std::move(other.myHealthChange);
		opponentHealthChange = std::move(other.opponentHealthChange);
		cardDraw = std::move(other.cardDraw);
		breakthrough = std::move(other.breakthrough);
		charge = std::move(other.charge);
		drain = std::move(other.drain);
		guard = std::move(other.guard);
		lethal = std::move(other.lethal);
		ward = std::move(other.ward);
		return *this;
	}

	Card& operator=(const Card &other)
	{
		number = other.number;
		instanceId = other.instanceId;
		location = other.location;
		type = other.type;
		cost = other.cost;
		attack = other.attack;
		defense = other.defense;
		abilities = other.abilities;
		myHealthChange = other.myHealthChange;
		opponentHealthChange = other.opponentHealthChange;
		cardDraw = other.cardDraw;
		breakthrough = other.breakthrough;
		charge = other.charge;
		drain = other.drain;
		guard = other.guard;
		lethal = other.lethal;
		ward = other.ward;
		return *this;
	}

	void setup(int _number, int _instanceId, int _location, int _type, int _cost, int _attack, int _defense, string _abilities, int _myHealthChange, int _opponentHealthChange, int _cardDraw)
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

class Deck
{
public:
	map<int, Card> cards;

	void clear()
	{
		cards.clear();
	}

	void putCard(int instanceId, Card&& card)
	{
		cards[instanceId] = std::move(card);
	}
	
	Card getCard(int instanceId)
	{
		return cards[instanceId];
	}
	
	void removeCard(int instanceId)
	{
		cards.erase(instanceId);
	}

	int nextGuard() const
	{
		for (auto it = cards.begin(); it != cards.end(); ++it)
		{
			if (it->second.guard && it->second.defense > 0)
			{
				return it->first;
			}
		}
		
		return -1;
	}
	
	void damageCard(int instanceId, int damage)
	{
		cards[instanceId].defense -= damage;
	}
	
	int highestWorth() const
	{
		int highestWorth = -1000;
		int bestId = -1;
		for (auto it = cards.begin(); it != cards.end(); ++it)
		{
			if(it->second.rawWorth() > highestWorth)
			{
				bestId = it->first;
				highestWorth = it->second.rawWorth();
			}
		}

		return bestId;
	}
	
	int nextCanCast(int mana) const
	{
		for (auto it = cards.begin(); it != cards.end(); ++it)
		{
			if (it->second.cost <= mana)
			{
				return it->first;
			}
		}
		
		return -1;
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
	Deck myHand;
	Deck mySide;
	Deck opponentSide;
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
				Card thisCard;
				thisCard.setup(cardNumber, instanceId, location, cardType, cost, attack, defense, abilities, myHealthChange, opponentHealthChange, cardDraw);
				myHand.putCard(i, std::move(thisCard));
			}
			else
			{
				Card thisCard;
				thisCard.setup(cardNumber, instanceId, location, cardType, cost, attack, defense, abilities, myHealthChange, opponentHealthChange, cardDraw);
				switch (location)
				{
					case 0:
						myHand.putCard(instanceId, std::move(thisCard));
						break;
					case 1:
						mySide.putCard(instanceId, std::move(thisCard));
						break;
					case -1:
						opponentSide.putCard(instanceId, std::move(thisCard));
						break;
				}
			}
			int key = drafting ? i : instanceId;
        }
		draftCounter--;
		
		string turnOutput = "";
		if (drafting)
		{
			turnOutput += "PICK " + std::to_string(myHand.highestWorth());
		}
		else
		{
			for (int next = myHand.nextCanCast(self.mana); next >= 0; next = myHand.nextCanCast(self.mana))
			{
				Card summonCard = myHand.getCard(next);
				myHand.removeCard(next);

				if (!turnOutput.empty())
				{
					turnOutput += ";";
				}
				turnOutput += "SUMMON " + std::to_string(summonCard.instanceId);
				self.cast(summonCard.cost);
				if (summonCard.charge)
				{
					mySide.putCard(summonCard.instanceId, std::move(summonCard));
				}
			}

			for (auto it = mySide.cards.begin(); it != mySide.cards.end(); ++it)
			{
				int target = opponentSide.nextGuard();
				if (target >= 0)
				{
					opponentSide.damageCard(target, it->second.attack);
				}
				if (!turnOutput.empty())
				{
					turnOutput += ";";
				}
				turnOutput += "ATTACK " + std::to_string(it->first) + " " + std::to_string(target);
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