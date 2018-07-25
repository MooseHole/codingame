#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class Player
{
	int health;
	int mana;
	int deck;
	int rune;
	bool self;
	
	void setup(int index, int _health, int _mana, int _deck, int _rune)
		: health{ _health }
		, mana{ _mana }
		, deck{ _deck }
		, rune{ _rune }
	{
		self = index == 0;
	}
};

class Card
{
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
	
	void setup(int _number, int _instanceId, _location, _type, _cost, _attack, _defense, _abilities, _myHealthChange, _opponentHealthChange, _cardDraw)
	: number{ _number }
	, instanceId{ _instanceId }
	, location{ _location }
	, type{ _type }
	, cost{ _cost }
	, attack{ _attack }
	, defense{ _defense }
	, abilities{ _abilities }
	, myHealthChange{ _myHealthChange }
	, opponentHealthChange{ _opponentHealthChange }
	, cardDraw{ _cardDraw }
	{}

	
	
	
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
    while(1)
	{
		bool drafting = draftCounter > 0;
		myHand.clear();
		mySide.clear();
		opponentSide.clear();
        for(int i = 0; i < 2; i++)
		{
            int playerHealth;
            int playerMana;
            int playerDeck;
            int playerRune;
            cin >> playerHealth >> playerMana >> playerDeck >> playerRune; cin.ignore();
			if(i==0)
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
        for(int i = 0; i < cardCount; i++)
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
			if(drafting)
			{
				myHand[i] = Card();
				myHand[i].setup(cardNumber, instanceId, location, cardType, cost, attack, defense, abilities, myHealthChange, opponentHealthChange, cardDraw);
			}
			else
			{
				Card thisCard;
				thisCard.setup(cardNumber, instanceId, location, cardType, cost, attack, defense, abilities, myHealthChange, opponentHealthChange, cardDraw);
				switch(location)
				{
					case 0:
						myHand[instanceId] = thisCard;
						break;
					case 1:
						mySide[instanceId] = thisCard;
						break;
					case -1:
						oppenentSide[instanceId] = thisCard;
						break;
				}
			}
			int key = drafting ? i : instanceId;
        }
		draftCounter--;
		
		string turnOutput = "";
		if(drafting)
		{
			int lowestCost = 1000;
			int lowestId = -1;
			for(const auto it = myHand.begin(); it != myHand.end(); ++it)
			{
				if(it->second.cost < lowestCost)
				{
					lowestId = it->first;
					lowestCost = it->second.cost;
				}
			}
			
			turnOutput = "Pick " << lowestId << ";";
		}
		else
		{
			for(const auto it = myHand.begin(); it != myHand.end(); )
			{
				if(self.mana >= it->second.cost)
				{
					turnOutput += "SUMMON " + it->first + ";";
					self.mana -= it->second.cost;
					myHand.erase(it);
				}
				else
				{
					++it;
				}
			}
			
			for(const auto it = mySide.begin(); it != mySide.end(); ++it)
			{
				turnOutput += "ATTACK " + it->first + " -1;";
			}
		}
			
		if(turnOutput.empty())
		{
			cout << "PASS" << endl;
		}
		else
		{
			cout << turnOutput << endl;
		}
    }
}