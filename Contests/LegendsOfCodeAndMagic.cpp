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
	bool isCreature;
	bool hasAttacked;

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
		isCreature = std::move(other.isCreature);
		hasAttacked = std::move(other.hasAttacked);
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
		isCreature = other.isCreature;
		hasAttacked = other.hasAttacked;
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
		isCreature = _type == 0;
		hasAttacked = false;
	}
	
	int rawWorth() const
	{
		return (lethal ? 10 : 0)
			 + (attack*2*(breakthrough ? 4 : 1)*(charge ? 2 : 1)*(drain ? 2 : 1))
			 + (defense*(guard ? (ward ? 5 : 3) : 1))
			 + myHealthChange
			 - opponentHealthChange
			 + (cardDraw*5);
	}
	
	int abilityMatch(Card compare) const
	{
		return ((breakthrough && compare.breakthrough) ? 1 : 0)
			 + ((charge && compare.charge) ? 1 : 0)
			 + ((drain && compare.drain) ? 1 : 0)
			 + ((guard && compare.guard) ? 1 : 0)
			 + ((lethal && compare.lethal) ? 1 : 0)
			 + ((ward && compare.ward) ? 1 : 0);
	}
};


Card compareTarget;
// All cards are assumed to be isCreature.  lhs and rhs are assumed to be !hasAttacked.
bool compareAttacker(Card lhs, Card rhs)
{
	if (!lhs.guard && rhs.guard) return true;
	if (lhs.guard && !rhs.guard) return false;
	if (lhs.lethal && rhs.lethal) return lhs.attack < rhs.attack;
	if (lhs.lethal && !rhs.lethal) return true;
	if (!lhs.lethal && rhs.lethal) return false;
	if (lhs.attack == compareTarget.defense && rhs.attack == compareTarget.defense) return lhs.rawWorth() < rhs.rawWorth();
	if (lhs.attack == compareTarget.defense && rhs.attack != compareTarget.defense) return true;
	if (lhs.attack != compareTarget.defense && rhs.attack == compareTarget.defense) return false;
	if (lhs.attack > compareTarget.defense && rhs.attack < compareTarget.defense) return true;
	if (lhs.attack > compareTarget.defense && rhs.attack > compareTarget.defense) return lhs.rawWorth() < rhs.rawWorth();
	if (lhs.attack < compareTarget.defense && rhs.attack < compareTarget.defense) return lhs.attack > rhs.attack;
	return lhs.rawWorth() < rhs.rawWorth();
}

bool compareRawWorth(Card lhs, Card rhs)
{
	return lhs.rawWorth() < rhs.rawWorth();
}

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

	int firstCard() const
	{
		for (auto it = cards.begin(); it != cards.end(); ++it)
		{
			return it->first;
		}
		
		return -1;
	}

	int nextTarget() const
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
	
	int friendToEnchant(Card enchantment) const
	{
		int lowestMatch = 10000;
		int target = -1;
		for (auto it = cards.begin(); it != cards.end(); ++it)
		{
			int match = it->second.abilityMatch(enchantment);
			if (lowestMatch > match)
			{
				lowestMatch = match;
				target = it->first;
			}
		}
		
		return target;
	}

	int enemyToEnchant(Card enchantment) const
	{
		int highestMatch = -10000;
		int target = -1;
		for (auto it = cards.begin(); it != cards.end(); ++it)
		{
			int match = it->second.abilityMatch(enchantment);
			if (highestMatch < match)
			{
				highestMatch = match;
				target = it->first;
			}
		}
		
		// If no abilities removed for anyone
		if (highestMatch == 0)
		{
			int target = nextTarget();
			if (target == -1)
			{
				target = firstCard();
			}
		}
		
		return target;
	}

	int nextAttacker(Card& target) const
	{
		compareTarget = target;
		vector<Card> attackers;
		for (auto it = cards.begin(); it != cards.end(); ++it)
		{
			if (!it->second.hasAttacked)
			{
				attackers.push_back(it->second);
			}
		}
		
		if (attackers.size() > 0)
		{
			sort (attackers.begin(), attackers.end(), compareAttacker);
			return attackers[0].instanceId;
		}

		return -1;
	}
	
	void damageCard(int instanceId, int damage, bool lethal)
	{
		if (cards[instanceId].ward)
		{
			cards[instanceId].ward = false;
		}
		else if (lethal)
		{
			cards[instanceId].defense = 0;
		}
		else
		{
			cards[instanceId].defense -= damage;
		}
		
		if (cards[instanceId].defense <= 0)
		{
			removeCard(instanceId);
		}
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
	
	int bestPick() const
	{
		int highestWorth = -1000;
		int bestId = -1;
		for (auto it = cards.begin(); it != cards.end(); ++it)
		{
			if(it->second.rawWorth() > highestWorth)
			{
				bestId = it->first;
				highestWorth = (12-it->second.cost) + it->second.rawWorth();
			}
		}

		return bestId;
	}
	
	int nextCanCast(int mana) const
	{
		vector<Card> casts;
		for (auto it = cards.begin(); it != cards.end(); ++it)
		{
			if (!it->second.hasAttacked)
			{
				casts.push_back(it->second);
			}
		}
		
		if (casts.size() > 0)
		{
			sort (casts.begin(), casts.end(), compareRawWorth);
			for (auto it = casts.begin(); it != casts.end(); ++it)
			{
				if (it->cost <= mana)
				{
					return it->instanceId;
				}
			}
		}
		
		return -1;
	}
};

string appendOutput(string&& currentOutput, string action, int source, int target, string comment="")
{
	if (!currentOutput.empty())
	{
		currentOutput += ";";
	}
	currentOutput += action + " " + std::to_string(source);
	if (target != -10000)
	{
		currentOutput += " " + std::to_string(target);
	}
	if (!comment.empty())
	{
		currentOutput += " " + comment;
	}

	return (std::move(currentOutput));
}

string appendOutput(string&& currentOutput, string action, int source, string comment="")
{
	return appendOutput(std::move(currentOutput), action, source, -10000, comment);
}

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
			turnOutput = appendOutput(std::move(turnOutput), "PICK", myHand.bestPick());
		}
		else
		{
			for (int next = myHand.nextCanCast(self.mana); next >= 0; next = myHand.nextCanCast(self.mana))
			{
				Card nextCard = myHand.getCard(next);
				myHand.removeCard(next);

				if (nextCard.isCreature)
				{
					turnOutput = appendOutput(std::move(turnOutput), "SUMMON", nextCard.instanceId);

					self.cast(nextCard.cost);
					if (!nextCard.charge)
					{
						nextCard.hasAttacked = true;
					}
					mySide.putCard(nextCard.instanceId, std::move(nextCard));
				}
				else
				{
					int target = -1;
					bool use = false;
					switch (nextCard.type)
					{
						case 1:
							// Green, for positive for creatures
							target = mySide.friendToEnchant(nextCard);
							if (target >= 0)
							{
								use = true;
							}
							break;
						case 2:
							// Red, for negative for creatures
							target = opponentSide.enemyToEnchant(nextCard);
							if (target >= 0)
							{
								use = true;
							}
							break;
						case 3:
							// Blue, generally use on players
							use = true;
							break;
					}

					if (use)
					{
						turnOutput = appendOutput(std::move(turnOutput), "USE", nextCard.instanceId, target);
					}
				}
			}

			while (true)
			{
				int target = opponentSide.nextTarget();
				int source = mySide.nextAttacker(opponentSide.cards[target]);
				if (source >= 0)
				{
					if (target >= 0)
					{
						opponentSide.damageCard(target, mySide.cards[source].attack, mySide.cards[source].lethal);
					}
					mySide.cards[source].hasAttacked = true;
					turnOutput = appendOutput(std::move(turnOutput), "ATTACK", source, target);
				}
				else
				{
					break;
				}
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