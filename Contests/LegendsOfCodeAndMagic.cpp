#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;
#define MAX_CHEAP 2
#define MIN_ENCHANT 3
#define NO_TARGET -10000
#define LETHAL_SCORE 10
#define ATTACK_SCORE 2
#define BREAKTHROUGH_SCORE 4
#define CHARGE_SCORE 2
#define DRAIN_SCORE 2
#define GUARD_SCORE 3
#define GUARD_AND_WARD_SCORE 5
#define HEAL_ME_SCORE 2
#define DAMAGE_THEM_SCORE 3
#define DRAW_SCORE 5


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
		return (lethal ? LETHAL_SCORE : 0)
			 + (attack*ATTACK_SCORE*(breakthrough ? BREAKTHROUGH_SCORE : 1)*(charge ? CHARGE_SCORE : 1)*(drain ? DRAIN_SCORE : 1))
			 + (defense*(guard ? (ward ? GUARD_AND_WARD_SCORE : GUARD_SCORE) : 1))
			 + myHealthChange * HEAL_ME_SCORE
			 - opponentHealthChange * DAMAGE_THEM_SCORE
			 + (cardDraw*DRAW_SCORE);
	}
	
	int abilityMatch(Card compare) const
	{
		return ((breakthrough && compare.breakthrough) ? 1 : 0)
			 + ((charge       && compare.charge)       ? 1 : 0)
			 + ((drain        && compare.drain)        ? 1 : 0)
			 + ((guard        && compare.guard)        ? 1 : 0)
			 + ((lethal       && compare.lethal)       ? 1 : 0)
			 + ((ward         && compare.ward)         ? 1 : 0);
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

bool compareCheap(Card lhs, Card rhs)
{
	if (lhs.cost < rhs.cost) return true;
	if (lhs.cost == rhs.cost) return compareRawWorth(lhs, rhs);
	return false;
}

bool compareEnchant(Card lhs, Card rhs)
{
	if (!lhs.isCreature && rhs.isCreature) return true;
	if (lhs.isCreature && !rhs.isCreature) return false;
	return compareRawWorth(lhs, rhs);
}

class Deck
{
public:
	map<int, Card> cards;

	void clear()
	{
		cards.clear();
	}
	
	int size() const
	{
		return cards.size();
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
	
	void enchant(int instanceId, Card enchantment)
	{
		cards[instanceId].attack += enchantment.attack;
		cards[instanceId].defense += enchantment.defense;
		// True if Green, False otherwise
		bool abilityModifier = (enchantment.type == 1);
		if (enchantment.breakthrough) cards[instanceId].breakthrough = abilityModifier;
		if (enchantment.charge) cards[instanceId].charge = abilityModifier;
		if (enchantment.drain) cards[instanceId].drain = abilityModifier;
		if (enchantment.guard) cards[instanceId].guard = abilityModifier;
		if (enchantment.lethal) cards[instanceId].lethal = abilityModifier;
		if (enchantment.ward) cards[instanceId].ward = abilityModifier;
	}

	int firstCard() const
	{
		for (auto it : cards)
		{
			return it.first;
		}
		
		return -1;
	}

	int nextTarget() const
	{
		for (auto it : cards)
		{
			if (it.second.guard && it.second.defense > 0)
			{
				return it.first;
			}
		}
		
		return -1;
	}
	
	int friendToEnchant(Card enchantment) const
	{
		int lowestMatch = 10000;
		int target = -1;
		for (auto it : cards)
		{
			int match = it.second.abilityMatch(enchantment);
			if (lowestMatch > match)
			{
				lowestMatch = match;
				target = it.first;
			}
		}
		
		return target;
	}

	int enemyToEnchant(Card enchantment) const
	{
		int highestMatch = -10000;
		int target = -1;
		for (auto it : cards)
		{
			int match = it.second.abilityMatch(enchantment);
			if (highestMatch < match)
			{
				highestMatch = match;
				target = it.first;
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
		for (auto it : cards)
		{
			if (!it.second.hasAttacked)
			{
				attackers.push_back(it.second);
			}
		}
		
		if (attackers.size() > 0)
		{
			sort (attackers.begin(), attackers.end(), compareAttacker);
			return attackers[0].instanceId;
		}

		return -1;
	}
	
	int damageCard(int instanceId, Card attacker)
	{
		int totalDamage = 0;
		if (cards[instanceId].ward)
		{
			cards[instanceId].ward = false;
		}
		else if (attacker.lethal)
		{
			totalDamage = cards[instanceId].defense;
			cards[instanceId].defense = 0;
		}
		else
		{
			totalDamage = std::min(cards[instanceId].defense, attacker.attack);
			cards[instanceId].defense -= attacker.attack;
		}
		
		if (cards[instanceId].defense <= 0)
		{
			removeCard(instanceId);
		}
		
		if (attacker.drain)
		{
			return totalDamage;
		}

		return 0;
	}
	
	int highestWorth() const
	{
		int highestWorth = -1000;
		int bestId = -1;
		for (auto it : cards)
		{
			if(it.second.rawWorth() > highestWorth)
			{
				bestId = it.first;
				highestWorth = it.second.rawWorth();
			}
		}

		return bestId;
	}
	
	int bestPick() const
	{
		int highestWorth = -1000;
		int bestId = -1;
		for (auto it : cards)
		{
			if(it.second.rawWorth() > highestWorth)
			{
				bestId = it.first;
				highestWorth = (12-it.second.cost) + it.second.rawWorth();
			}
		}

		return bestId;
	}
	
	int guardHealth() const
	{
		int guardDefense = 0;
		for (auto it : cards)
		{
			if (!it.second.guard)
			{
				guardDefense += it.second.defense;
			}
		}
		
		return guardDefense;
	}
	
	int attackPower() const
	{
		int power = 0;
		for (auto it : cards)
		{
			power += it.second.attack;
		}
		
		return power;
	}
	
	template <typename Comparator>
	int nextCanCast(int mana, Comparator compare) const
	{
		vector<Card> casts;
		for (auto it : cards)
		{
			if (!it.second.hasAttacked)
			{
				casts.push_back(it.second);
			}
		}
		
		if (casts.size() > 0)
		{
			sort (casts.begin(), casts.end(), compare);
			for (auto cast : casts)
			{
				if (cast.cost <= mana)
				{
					return cast.instanceId;
				}
			}
		}
		
		return -1;
	}
};

class Action
{
	enum class Type { PICK, SUMMON, USE, ATTACK, PASS };
	Type type;
	int source;
	int target;
	string comment;
	
public:
	Action()
	{
		type = Type::PASS;
		source = NO_TARGET;
		target = NO_TARGET;
		comment = "";
	}
	
	bool operator<(const Action& rhs)
	{
		return type < rhs.type;
	}
	
	Action& Pick(int choice)
	{
		type = Type::PICK;
		source = choice;
		return *this;
	}

	Action& Summon(int choice)
	{
		type = Type::SUMMON;
		source = choice;
		return *this;
	}
	
	Action& Use(int choice, int _target)
	{
		type = Type::SUMMON;
		source = choice;
		target = _target;
		return *this;
	}
	
	Action& Attack(int _source, int _target)
	{
		type = Type::ATTACK;
		source = _source;
		target = _target;
		return *this;
	}
	
	Action& Comment(string _comment)
	{
		comment = _comment;
	}
	
	friend ostream &operator<<(ostream &os, Action const &m)
	{
		switch (m.type)
		{
			case Type::PICK:   os << "PICK";   break;
			case Type::SUMMON: os << "SUMMON"; break;
			case Type::USE:    os << "USE";    break;
			case Type::ATTACK: os << "ATTACK"; break;
			default:           os << "PASS";   break;
		}
		
		if (m.source != NO_TARGET)
		{
			os << " " << std::to_string(m.source);
		}

		if (m.target != NO_TARGET)
		{
			os << " " << std::to_string(m.target);
		}
		
		if (!m.comment.empty())
		{
			os << " " << m.comment;
		}
		
		return os;
	}
};

class Player
{
public:
	int health;
	int mana;
	int deck;
	int rune;
	bool self;
	Deck hand;
	Deck field;
	vector<Action> actions;

	Player()
	{
	}

	Player(const Player &other)
	{
		*this = other;
	}

	Player& operator=(const Player &&other)
	{
		health = std::move(other.health);
		mana = std::move(other.mana);
		deck = std::move(other.deck);
		rune = std::move(other.rune);
		self = std::move(other.self);
		hand = std::move(other.hand);
		field = std::move(other.field);
		actions = std::move(other.actions);
		return *this;
	}

	Player& operator=(const Player &other)
	{
		health = other.health;
		mana = other.mana;
		deck = other.deck;
		rune = other.rune;
		self = other.self;
		hand = other.hand;
		field = other.field;
		actions = other.actions;
		return *this;
	}

	bool operator<(const Player& rhs)
	{
		return score() < rhs.score();
	}

	void setup (int index, int _health, int _mana, int _deck, int _rune)
	{
		self = index == 0;
		health = _health;
		mana = _mana;
		deck = _deck;
		rune = _rune;
	}

	void reset()
	{
		hand.clear();
		field.clear();
		actions.clear();
	}

	void cast (int cost)
	{
		mana -= cost;
	}
	
	int score() const
	{
		return health + field.guardHealth() + field.attackPower();
	}
};

template <typename Comparator>
Player simulate(Player sourcePlayer, Player targetPlayer, Comparator compareCast)
{
	for (int next = sourcePlayer.hand.nextCanCast(sourcePlayer.mana, compareCast); next >= 0; next = sourcePlayer.hand.nextCanCast(sourcePlayer.mana, compareCast))
	{
		Card nextCard = sourcePlayer.hand.getCard(next);
		sourcePlayer.hand.removeCard(next);

		if (nextCard.isCreature)
		{
			sourcePlayer.actions.push_back(Action().Summon(nextCard.instanceId));

			sourcePlayer.cast(nextCard.cost);
			if (!nextCard.charge)
			{
				nextCard.hasAttacked = true;
			}
			sourcePlayer.field.putCard(nextCard.instanceId, std::move(nextCard));
		}
		else
		{
			int target = -1;
			bool use = false;
			switch (nextCard.type)
			{
				case 1:
					// Green, for positive for creatures
					target = sourcePlayer.field.friendToEnchant(nextCard);
					if (target >= 0)
					{
						sourcePlayer.field.enchant(target, nextCard);
						use = true;
					}
					break;
				case 2:
					// Red, for negative for creatures
					target = targetPlayer.field.enemyToEnchant(nextCard);
					if (target >= 0)
					{
						targetPlayer.field.enchant(target, nextCard);
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
				sourcePlayer.health += nextCard.myHealthChange;
				targetPlayer.health += nextCard.opponentHealthChange;
				// TODO Modify card draws
				sourcePlayer.actions.push_back(Action().Use(nextCard.instanceId, target));
			}
		}
	}

	while (true)
	{
		int target = targetPlayer.field.nextTarget();
		int source = sourcePlayer.field.nextAttacker(targetPlayer.field.cards[target]);
		if (source >= 0)
		{
			if (target >= 0)
			{
				sourcePlayer.health += targetPlayer.field.damageCard(target, sourcePlayer.field.cards[source]);
			}
			sourcePlayer.field.cards[source].hasAttacked = true;
			sourcePlayer.actions.push_back(Action().Attack(source, target));
		}
		else
		{
			break;
		}
	}

	return sourcePlayer;
}

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
	Player self;
	Player opponent;
	int draftCounter = 30;
    // game loop
    while (1)
	{
		bool drafting = draftCounter > 0;
		self.reset();
		opponent.reset();
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
				self.hand.putCard(i, std::move(thisCard));
			}
			else
			{
				Card thisCard;
				thisCard.setup(cardNumber, instanceId, location, cardType, cost, attack, defense, abilities, myHealthChange, opponentHealthChange, cardDraw);
				switch (location)
				{
					case 0:
						self.hand.putCard(instanceId, std::move(thisCard));
						break;
					case 1:
						self.field.putCard(instanceId, std::move(thisCard));
						break;
					case -1:
						opponent.field.putCard(instanceId, std::move(thisCard));
						break;
				}
			}
			int key = drafting ? i : instanceId;
        }
		draftCounter--;
		
		string turnOutput = "";
		if (drafting)
		{
			self.actions.push_back(Action().Pick(self.hand.bestPick()));
		}
		else
		{
			vector<Player> checkPlayers;
			checkPlayers.push_back(simulate(self, opponent, compareCheap));
			checkPlayers.push_back(simulate(self, opponent, compareEnchant));
			checkPlayers.push_back(simulate(self, opponent, compareRawWorth));
			// Sort highest score first
			std::sort(checkPlayers.rbegin(), checkPlayers.rend());
			self = std::move(checkPlayers[0]);
		}

		if (self.actions.empty())
		{
			cout << Action() << endl;
		}
		else
		{
			bool first = true;
			for (auto action : self.actions)
			{
				if (!first)
				{
					cout << ";";
				}
				first = false;
				cout << action;
			}
			cout << endl;
		}
		
		cerr << "My Score: " << self.score() << "  Opponent Score: " << opponent.score() << endl;
    }
}