#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>

using namespace std;
#define MAX_CHEAP 2
#define MIN_ENCHANT 3
#define BUCKET_MAX 30,10,8,6,5,4,3,3
#define NUMBER_OF_BUCKETS 8
#define BUCKET_MAX_COST 7
#define MAX_HAND_CARDS 8
#define MAX_FIELD_CARDS 6
#define NO_TARGET -10000
#define LETHAL_SCORE 10
#define ATTACK_SCORE 2
#define BREAKTHROUGH_SCORE 4
#define CHARGE_SCORE 2
#define DRAIN_SCORE 2
#define GUARD_SCORE 3
#define GUARD_AND_WARD_SCORE 5
#define WARD_SCORE 2
#define HEAL_ME_SCORE 2
#define DAMAGE_THEM_SCORE 3
#define DRAW_SCORE 5
#define PLAYER_HEALTH_SCORE 100
#define PLAYER_GUARD_SCORE 100
#define PLAYER_ATTACK_SCORE 100
#define PLAYER_DRAW_SCORE 1

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
	bool summoningSickness;
	bool exclude;

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
		summoningSickness = std::move(other.summoningSickness);
		exclude = std::move(other.exclude);
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
		summoningSickness = other.summoningSickness;
		exclude = other.exclude;
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
		summoningSickness = false;
		exclude = false;
	}
	
	int rawWorth() const
	{
		if (isCreature)
		{
			return (lethal ? LETHAL_SCORE : 0)
				 + (attack*ATTACK_SCORE*(breakthrough ? BREAKTHROUGH_SCORE : 1)*(charge ? CHARGE_SCORE : 1)*(drain ? DRAIN_SCORE : 1))
				 + (defense*(guard ? (ward ? GUARD_AND_WARD_SCORE : GUARD_SCORE) : 1))
				 + myHealthChange * HEAL_ME_SCORE
				 - opponentHealthChange * DAMAGE_THEM_SCORE
				 + (cardDraw*DRAW_SCORE);
		}
		
		switch (type)
		{
			case 2:
				// Red, for negative for creatures
				return (lethal ? LETHAL_SCORE : 0)
					 - (attack*ATTACK_SCORE)
					 - (defense)
					 + (breakthrough ? BREAKTHROUGH_SCORE : 0)
					 + (charge ? CHARGE_SCORE : 0)
					 + (drain ? DRAIN_SCORE : 0)
					 + (guard ? GUARD_SCORE : 0)
					 + (ward ? WARD_SCORE : 0)
					 + (myHealthChange*HEAL_ME_SCORE)
					 - (opponentHealthChange*DAMAGE_THEM_SCORE)
					 + (cardDraw*DRAW_SCORE);
				break;
			default:
				// Green, for positive for creatures
				// Blue, generally use on players
				return (lethal ? LETHAL_SCORE : 0)
					 + (attack*ATTACK_SCORE)
					 + (defense)
					 + (breakthrough ? BREAKTHROUGH_SCORE : 0)
					 + (charge ? CHARGE_SCORE : 0)
					 + (drain ? DRAIN_SCORE : 0)
					 + (guard ? GUARD_SCORE : 0)
					 + (ward ? WARD_SCORE : 0)
					 + (myHealthChange*HEAL_ME_SCORE)
					 - (opponentHealthChange*DAMAGE_THEM_SCORE)
					 + (cardDraw*DRAW_SCORE);
				break;
		}
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
	
	friend ostream &operator<<(ostream &os, Card const &m)
	{
		os << "Card: n" << m.number << " i" << m.instanceId << " l" << m.location << " t" << m.type << " $" << m.cost;
		os << " a" << m.attack << " d" << m.defense << " m" << m.myHealthChange << " o" << m.opponentHealthChange;
		os << " d" << m.cardDraw;
		os << " " << (m.breakthrough ? "B" : "-") << (m.charge ? "C" : "-") << (m.drain ? "D" : "-") << (m.guard ? "G" : "-") << (m.lethal ? "L" : "-") << (m.ward ? "W" : "-");
		os << " " << (m.isCreature ? "creature" : "enchantment");
		os << " " << (m.hasAttacked ? "has attacked" : "has not attacked");
		os << " " << (m.exclude ? "exclude" : "include");
		return os;
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
	string name;
	map<int, Card> cards;
	map<int, int> buckets;
	int bucket_max[NUMBER_OF_BUCKETS] = { BUCKET_MAX };

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
		// True if Green, False otherwise
		bool abilityModifier = (enchantment.type == 1);
		if (enchantment.breakthrough) cards[instanceId].breakthrough = abilityModifier;
		if (enchantment.charge) cards[instanceId].charge = abilityModifier;
		if (enchantment.drain) cards[instanceId].drain = abilityModifier;
		if (enchantment.guard) cards[instanceId].guard = abilityModifier;
		if (enchantment.lethal) cards[instanceId].lethal = abilityModifier;
		if (enchantment.ward) cards[instanceId].ward = abilityModifier;
		if (enchantment.type == 1 && enchantment.charge && cards[instanceId].summoningSickness)
		{
			cards[instanceId].summoningSickness = false;
		}
		cards[instanceId].attack += enchantment.attack;
		if (enchantment.defense <= 0 && cards[instanceId].ward)
		{
			cards[instanceId].ward = false;
		}
		else
		{
			cards[instanceId].defense += enchantment.defense;
		}
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
	
	void damageCard(int instanceId, Card attacker, int& attackerHealthChange, int& defenderHealthChange)
	{
		int totalDamage = 0;
		attackerHealthChange = 0;
		defenderHealthChange = 0;
		if (cards[instanceId].ward)
		{
			cards[instanceId].ward = false;
		}
		else
		{
			if (attacker.lethal && attacker.breakthrough)
			{
				cards[instanceId].defense = 0;
				defenderHealthChange = attacker.attack;
			}
			else if (attacker.lethal)
			{
				totalDamage = cards[instanceId].defense;
				cards[instanceId].defense = 0;
			}
			else if (attacker.breakthrough)
			{
				totalDamage = attacker.attack;
				cards[instanceId].defense -= attacker.attack;
				if (cards[instanceId].defense < 0)
				{
					defenderHealthChange = cards[instanceId].defense;
				}
			}
			else
			{
				totalDamage = std::min(cards[instanceId].defense, attacker.attack);
				cards[instanceId].defense -= attacker.attack;
			}
		}
		
		if (cards[instanceId].defense <= 0)
		{
			removeCard(instanceId);
		}
		
		if (attacker.drain)
		{
			attackerHealthChange = totalDamage;
		}
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
	
	int bestPick(string& comment)
	{
		int highestWorth = -1000000;
		int bestId = -1;
		int bestCost = -1;
		for (auto it : cards)
		{
			int thisCost = it.second.cost;
			thisCost = thisCost > BUCKET_MAX_COST ? BUCKET_MAX_COST : thisCost;
			int thisWorth = (buckets[thisCost] >= bucket_max[thisCost] ? -1000 : 0) + it.second.rawWorth();
			if(thisWorth > highestWorth)
			{
				bestId = it.first;
				highestWorth = thisWorth;
				bestCost = thisCost;
			}
		}
		
		if (buckets[bestCost] >= bucket_max[bestCost])
		{
			comment += "Lots of " + (bestCost >= BUCKET_MAX_COST ? std::to_string(BUCKET_MAX_COST)+"+" : std::to_string(bestCost)) + "s.";
		}

		buckets[bestCost]++;
		return bestId;
	}
	
	bool anyGuard() const
	{
		for (auto it : cards)
		{
			if (it.second.guard && it.second.isCreature)
			{
				return true;
			}
		}
		
		return false;
	}
	
	int guardHealth() const
	{
		int guardDefense = 0;
		for (auto it : cards)
		{
			if (it.second.guard)
			{
				guardDefense += it.second.defense;
				if (it.second.ward)
				{
					guardDefense += 1;
				}
			}
		}
		
		return guardDefense;
	}
	
	int attackPower() const
	{
		int power = 0;
		for (auto it : cards)
		{
			power += (it.second.summoningSickness ? (int)ceil((float)it.second.attack / 2.0) : it.second.attack);
			power += (it.second.lethal ? LETHAL_SCORE : 0);
			power += (it.second.breakthrough ? BREAKTHROUGH_SCORE : 0);
			power += (it.second.drain ? DRAIN_SCORE : 0);
		}
		
		return power;
	}
	
	friend ostream &operator<<(ostream &os, Deck const &m)
	{
		os << "Deck " << m.name;
		for (auto card : m.cards)
		{
			os << " " << card.first;
		}
		return os;
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
		type = Type::USE;
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
		return *this;
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
	int draws;

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
		draws = std::move(other.draws);
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
		draws = other.draws;
		return *this;
	}

	bool operator<(const Player& rhs)
	{
		return score() < rhs.score();
	}

	void setup(int index, int _health, int _mana, int _deck, int _rune)
	{
		self = index == 0;
		health = _health;
		mana = _mana;
		deck = _deck;
		rune = _rune;
		draws = 1;
		hand.name = (self ? "SELF HAND" : "OPPONENT HAND");
		field.name = (self ? "SELF FIELD" : "OPPONENT FIELD");
	}

	void reset()
	{
		hand.clear();
		field.clear();
		actions.clear();
		draws = 1;
	}

	void cast(int cost)
	{
		mana -= cost;
	}
	
	void pick()
	{
		string comment;
		actions.push_back(Action().Pick(hand.bestPick(comment)).Comment(comment));
	}
	
	int score() const
	{
		return PLAYER_HEALTH_SCORE*health
			 + PLAYER_GUARD_SCORE*field.guardHealth()
			 + PLAYER_ATTACK_SCORE*field.attackPower()
			 + PLAYER_DRAW_SCORE*draws
			 + (deck < draws ? -100000 : 0)
			 + (health <= 0 ? -100000: 0);
	}
	
	void healthChange(int amount)
	{
		health -= amount;
		updateRunes();
	}
	
	void updateRunes()
	{
		if (rune == 5 && health < 25) breakRune();
		if (rune == 4 && health < 20) breakRune();
		if (rune == 3 && health < 15) breakRune();
		if (rune == 2 && health < 10) breakRune();
		if (rune == 1 && health <  5) breakRune();
	}
	
	void breakRune()
	{
		drawsChange(1);
		rune--;
	}

	void drawsChange(int amount)
	{
		draws += amount;
		updateDraws();
	}
	
	void updateDraws()
	{
		int availableDraws = MAX_HAND_CARDS - hand.size();
		if (draws > availableDraws)
		{
			draws = availableDraws;
		}
	}
};

Player simulate(Player sourcePlayer, Player targetPlayer)
{
	bool anyNewAction;
	do
	{
		anyNewAction = false;
		// Find cards to use/summon
		while (true)
		{
			vector<int> cardsInHand;
			bool anyCreaturesInHand = false;
			for (auto it : sourcePlayer.hand.cards)
			{
				if (!it.second.exclude && it.second.cost <= sourcePlayer.mana)
				{
					cardsInHand.push_back(it.first);
					if (it.second.isCreature)
					{
						anyCreaturesInHand = true;
					}
				}
			}
			if (cardsInHand.empty())
			{
				break;
			}
			
			Player bestSourcePlayer = sourcePlayer;
			Player bestTargetPlayer = targetPlayer;
			bool first = true;
			for (auto cardId : cardsInHand)
			{
				Player testSourcePlayer = sourcePlayer;
				Player testTargetPlayer = targetPlayer;
				Card nextCard = testSourcePlayer.hand.getCard(cardId);
				testSourcePlayer.hand.removeCard(cardId);

				if (nextCard.isCreature && testSourcePlayer.field.size() < MAX_FIELD_CARDS)
				{
					string comment = "";
					if (nextCard.guard)
					{
						comment = "This should keep me safe.";
					}
					else if (nextCard.attack >= 5)
					{
						comment = "Check out this puppy.";
					}
					testSourcePlayer.actions.push_back(Action().Summon(nextCard.instanceId).Comment(comment));

					testSourcePlayer.cast(nextCard.cost);
					testSourcePlayer.drawsChange(nextCard.cardDraw);
					testSourcePlayer.healthChange(nextCard.myHealthChange);
					testTargetPlayer.healthChange(nextCard.opponentHealthChange);

					if (!nextCard.charge)
					{
						nextCard.summoningSickness = true;
					}
					testSourcePlayer.field.putCard(nextCard.instanceId, std::move(nextCard));
				}
				else if (!anyCreaturesInHand && !nextCard.isCreature)
				{
					int target = -1;
					bool use = false;
					string comment = "";
					switch (nextCard.type)
					{
						case 1:
							// Green, for positive for creatures
							target = testSourcePlayer.field.friendToEnchant(nextCard);
							if (target >= 0)
							{
								comment = "Power up!";
								testSourcePlayer.field.enchant(target, nextCard);
								use = true;
							}
							break;
						case 2:
							// Red, for negative for creatures
							target = testTargetPlayer.field.enemyToEnchant(nextCard);
							if (target >= 0)
							{
								comment = "Muahahaha";
								testTargetPlayer.field.enchant(target, nextCard);
								use = true;
							}
							break;
						case 3:
							// Blue, generally use on players
							comment = "Bwoop";
							use = true;
							break;
					}

					if (use)
					{
						testSourcePlayer.healthChange(nextCard.myHealthChange);
						testTargetPlayer.healthChange(nextCard.opponentHealthChange);
						testSourcePlayer.drawsChange(nextCard.cardDraw);
						testSourcePlayer.actions.push_back(Action().Use(nextCard.instanceId, target).Comment(comment));
						testSourcePlayer.cast(nextCard.cost);
					}
					else
					{
						testSourcePlayer.field.cards[nextCard.instanceId].exclude = true;
						bestSourcePlayer.field.cards[nextCard.instanceId].exclude = true;
					}
				}
				
				if (first || bestSourcePlayer.score() - bestTargetPlayer.score() < testSourcePlayer.score() - testTargetPlayer.score())
				{
					first = false;
					bestSourcePlayer = std::move(testSourcePlayer);
					bestTargetPlayer = std::move(testTargetPlayer);
				}
			}

			if (sourcePlayer.actions.size() < bestSourcePlayer.actions.size())
			{
				cerr << "A " << sourcePlayer.actions.size() << " < " << bestSourcePlayer.actions.size() << endl;
				anyNewAction = true;
			}
			sourcePlayer = std::move(bestSourcePlayer);
			targetPlayer = std::move(bestTargetPlayer);
		}

		int previousNumberOfCardsThatCanAttack = -1;
		while (true)
		{
			// Find cards to attack with
			vector<int> cardsThatCanAttack;
			for (auto it : sourcePlayer.field.cards)
			{
				if (it.second.isCreature && !it.second.hasAttacked && !it.second.summoningSickness)
				{
					cardsThatCanAttack.push_back(it.first);
				}
			}
			
			int numberOfCardsThatCanAttack = cardsThatCanAttack.size();
			if (numberOfCardsThatCanAttack == 0 || numberOfCardsThatCanAttack == previousNumberOfCardsThatCanAttack)
			{
				break;
			}
			previousNumberOfCardsThatCanAttack = numberOfCardsThatCanAttack;

			Player bestSourcePlayer = sourcePlayer;
			Player bestTargetPlayer = targetPlayer;

			for (auto attacker : cardsThatCanAttack)
			{
				bool anyGuard = targetPlayer.field.anyGuard();
				vector<int> cardsThatCanDefend;
				for (auto it : targetPlayer.field.cards)
				{
					if (!anyGuard || (it.second.isCreature && it.second.guard))
					{
						cardsThatCanDefend.push_back(it.first);
					}
				}

				if (!anyGuard)
				{
					Player testSourcePlayer = sourcePlayer;
					Player testTargetPlayer = targetPlayer;
					testTargetPlayer.healthChange(testSourcePlayer.field.cards[attacker].attack);
					testSourcePlayer.field.cards[attacker].hasAttacked = true;
					testSourcePlayer.actions.push_back(Action().Attack(attacker, -1).Comment("Eat this!"));

					if (!testSourcePlayer.field.cards[attacker].hasAttacked
					 ||	bestSourcePlayer.score() - bestTargetPlayer.score() < testSourcePlayer.score() - testTargetPlayer.score())
					{
						bestSourcePlayer = std::move(testSourcePlayer);
						bestTargetPlayer = std::move(testTargetPlayer);
					}
				}

				for (auto defender : cardsThatCanDefend)
				{
					Player testSourcePlayer = sourcePlayer;
					Player testTargetPlayer = targetPlayer;

					int attackerHealthChange;
					int defenderHealthChange;
					testTargetPlayer.field.damageCard(defender, sourcePlayer.field.cards[attacker], attackerHealthChange, defenderHealthChange);
					// Health changes only occur for the attacker
					testSourcePlayer.healthChange(attackerHealthChange);
					testTargetPlayer.healthChange(defenderHealthChange);
					testSourcePlayer.field.damageCard(attacker, targetPlayer.field.cards[defender], attackerHealthChange, defenderHealthChange);
					testSourcePlayer.field.cards[attacker].hasAttacked = true;
					testSourcePlayer.actions.push_back(Action().Attack(attacker, defender).Comment("Die."));

					if (!testSourcePlayer.field.cards[attacker].hasAttacked
					 ||	bestSourcePlayer.score() - bestTargetPlayer.score() < testSourcePlayer.score() - testTargetPlayer.score())
					{
						bestSourcePlayer = std::move(testSourcePlayer);
						bestTargetPlayer = std::move(testTargetPlayer);
					}
				}
			}

			if (sourcePlayer.actions.size() < bestSourcePlayer.actions.size())
			{
				cerr << "B " << sourcePlayer.actions.size() << " < " << bestSourcePlayer.actions.size() << endl;
				anyNewAction = true;
			}
			sourcePlayer = std::move(bestSourcePlayer);
			targetPlayer = std::move(bestTargetPlayer);
		}
	} while (anyNewAction);

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
				self.hand.name = "SELF HAND";
				self.field.name = "SELF FIELD";
			}
			else
			{
				opponent.setup(i, playerHealth, playerMana, playerDeck, playerRune);
				opponent.hand.name = "OPPONENT HAND";
				opponent.field.name = "OPPONENT FIELD";
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
        }
		draftCounter--;
		self.updateDraws();
		opponent.updateDraws();
		
		string turnOutput = "";
		if (drafting)
		{
			self.pick();
		}
		else
		{
			self = simulate(self, opponent);
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