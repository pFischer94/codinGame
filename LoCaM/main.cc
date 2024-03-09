#include <iostream>
#include <iomanip>
// #include <string>
#include <vector>
// #include <algorithm>
#include <map>

using namespace std;

struct Player;
struct Card;
struct Action;
struct State;

State readAndSortBattleState();
pair<Player, Player> readPlayers();
vector<Action> readOpponentsActions();
tuple<vector<Card>, vector<Card>, vector<Card>> readCards();
void printState(const Player& OPPONENT, const vector<Card>& OPPONENTS_BOARD, const vector<Action>& OPPONENTS_ACTIONS, 
        const Player& PLAYER, const vector<Card>& PLAYERS_HAND, const vector<Card>& PLAYERS_BOARD);
void printState(const State& STATE);
int findMaxCost(const vector<Card>& CARDS);
vector<Card> findComboToUseMostMana(int mana, const vector<Card>& PLAYERS_HAND);
int calculateCost(const vector<Card>& COMBO);
void playCombo(const vector<Card>& COMBO, vector<Card>& playersBoard);

struct Player {
    int health;
    int mana;
    int deck;
    int hand;

    Player() {};
    Player(int health, int mana, int deck) {
        this->health = health;
        this->mana = mana;
        this->deck = deck;
        this->hand = 0;
    }

    void print() const {
        cerr << "Player [ health: " << setw(2) << health << ", mana: " << setw(2) << mana 
                << ", deck: " << setw(2) << deck << ", hand: " << setw(1) << hand << " ]" << endl;
    }
};
struct Card {
    int instanceId;
    int cost;
    int att;
    int def;

    Card(int instanceId, int cost, int att, int def) {
        this->instanceId = instanceId;
        this->cost = cost;
        this->att = att;
        this->def = def;
    }

    // TODO calculateRating with effects, calculateBattleValue
    double calculateRating(const map<int, int>& PLAYERS_DECK_STRUCTURE) const {
        double costEfficiency = (this->att + this->def) / (double) this->cost;
        return costEfficiency;
    }
    // double calculateThreat() {
    //     return this->att / (double) this->def;
    // }
    void print() const {
        cerr << "Card [ instanceId: " << setw(3) << instanceId << ", cost: " << setw(2) << cost 
                << ", att: " << setw(2) << att << ", def: " << setw(3) << def << " ]" << endl;
    }
};
struct Action {
    string actionType;
    int activeId;
    int passiveId;

    Action(int summonedId) {
        this->actionType = "SUMMON";
        this->activeId = summonedId;
        this->passiveId = 0;
    }
    Action(int attId, int passiveId) {
        this->actionType = "ATTACK";
        this->activeId = attId;
        this->passiveId = passiveId;
    }
    Action() {
        this->actionType = "PASS";
        this->activeId = 0;
        this->passiveId = 0;
    }

    void print() const {
        cerr << "Action [ actionType: " << setw(6) << actionType << ", activeId: " << setw(2) << activeId 
                << ", passiveId: " << setw(2) << passiveId << " ]" << endl;
    }
};
struct State {
    Player player;
    Player opponent;
    vector<Action> opponentsActions;
    vector<Card> opponentsBoard;
    vector<Card> playersHand;
    vector<Card> playersBoard;

    State(Player& player, Player& opponent, vector<Action>& opponentsActions, vector<Card>& opponentsBoard, 
            vector<Card>& playersHand, vector<Card>& playersBoard) : 
            player(player), opponent(opponent), opponentsActions(opponentsActions), opponentsBoard(opponentsBoard), 
            playersHand(playersHand), playersBoard(playersBoard) {}
};

State readAndSortBattleState() {
    auto [ player, opponent ] = readPlayers();
    vector<Action> opponentsActions = readOpponentsActions();
    auto [ opponentsBoard, playersHand, playersBoard ] = readCards();
    sort(opponentsBoard.begin(), opponentsBoard.end(), [](Card a, Card b) { return a.att > b.att; });
    sort(playersHand.begin(), playersHand.end(), [](Card a, Card b) { return a.cost > b.cost; });
    sort(playersBoard.begin(), playersBoard.end(), [](Card a, Card b) { return a.def < b.def; });

    return State(player, opponent, opponentsActions, opponentsBoard, playersHand, playersBoard);
}
pair<Player, Player> readPlayers() {
    int player_health;
    int player_mana;
    int player_deck;
    int player_rune;
    int player_draw;

    cin >> player_health >> player_mana >> player_deck >> player_rune >> player_draw; cin.ignore();
    Player player(player_health, player_mana, player_deck);
    cin >> player_health >> player_mana >> player_deck >> player_rune >> player_draw; cin.ignore();
    Player opponent(player_health, player_mana, player_deck);
    int opponent_hand;
    cin >> opponent_hand;
    opponent.hand = opponent_hand;
    return pair(player, opponent);
}
vector<Action> readOpponentsActions() {
    vector<Action> opponentsActions;
    int opponent_actions;
    cin >> opponent_actions; cin.ignore();
    for (int i = 0; i < opponent_actions; i++) {
        int cardNumber;
        cin >> cardNumber;
        string actionType;
        cin >> actionType;
        if (actionType == "SUMMON") {
            int summonedId;
            cin >> summonedId; cin.ignore();
            opponentsActions.emplace_back(Action(summonedId));
        } else if (actionType == "ATTACK") {
            int attId;
            int defId;
            cin >> attId;
            cin >> defId;
            opponentsActions.emplace_back(Action(attId, defId));
        }
    }
    return opponentsActions;
}
tuple<vector<Card>, vector<Card>, vector<Card>> readCards() {
    vector<Card> opponentsBoard;
    vector<Card> playersHand;
    vector<Card> playersBoard;
    int card_count;
    cin >> card_count; cin.ignore();
    for (int i = 0; i < card_count; i++) {
        int card_number;
        int instance_id;
        int location;
        int card_type;
        int cost;
        int attack;
        int defense;
        string abilities;
        int my_health_change;
        int opponent_health_change;
        int card_draw;
        cin >> card_number >> instance_id >> location >> card_type >> cost >> attack >> defense >> abilities >> my_health_change >> opponent_health_change >> card_draw; cin.ignore();

        switch (location) {
            case -1:
                opponentsBoard.emplace_back(Card(instance_id, cost, attack, defense));
                break;
            case 0:
                playersHand.emplace_back(Card(instance_id, cost, attack, defense));
                break;
            case 1:
                playersBoard.emplace_back(Card(instance_id, cost, attack, defense));
                break;
            default: throw runtime_error("illegal location in readCards");
        }
    }
    return tuple(opponentsBoard, playersHand, playersBoard);
}
void printState(const Player& OPPONENT, const vector<Card>& OPPONENTS_BOARD, const vector<Action>& OPPONENTS_ACTIONS, 
        const Player& PLAYER, const vector<Card>& PLAYERS_HAND, const vector<Card>& PLAYERS_BOARD) {
    cerr << "opponent:" << endl;
    OPPONENT.print();

    cerr << endl << "opponentsBoard:" << endl;
    for (const Card& CARD : OPPONENTS_BOARD) {
        CARD.print();
    }

    cerr << endl << "opponentsActions:" << endl;
    for (const Action& ACTION : OPPONENTS_ACTIONS) {
        ACTION.print();
    }

    cerr << endl << "player:" << endl;
    PLAYER.print();

    cerr << endl << "playersHand:" << endl;
    for (const Card& CARD : PLAYERS_HAND) {
        CARD.print();
    }

    cerr << endl << "playersBoard:" << endl;
    for (const Card& CARD : PLAYERS_BOARD) {
        CARD.print();
    }
}
void printState(const State& STATE) {
    cerr << "opponent:" << endl;
    STATE.opponent.print();

    cerr << endl << "opponentsBoard:" << endl;
    for (const Card& CARD : STATE.opponentsBoard) {
        CARD.print();
    }

    cerr << endl << "opponentsActions:" << endl;
    for (const Action& ACTION : STATE.opponentsActions) {
        ACTION.print();
    }

    cerr << endl << "player:" << endl;
    STATE.player.print();

    cerr << endl << "playersHand:" << endl;
    for (const Card& CARD : STATE.playersHand) {
        CARD.print();
    }

    cerr << endl << "playersBoard:" << endl;
    for (const Card& CARD : STATE.playersBoard) {
        CARD.print();
    }
}
int findMaxCost(const vector<Card>& CARDS) {
    int maxCost = 0;
    for (const Card& CARD : CARDS) {
        if (CARD.cost >= maxCost) {
            maxCost = CARD.cost;
        }
    }
    return maxCost;
}
vector<Card> findComboToUseMostMana(int mana, const vector<Card>& PLAYERS_HAND) {
    for (int manaToBeUsed = mana; manaToBeUsed >= 0; manaToBeUsed--) {
        for (int cardIndexToStart = 0; cardIndexToStart < PLAYERS_HAND.size(); cardIndexToStart++) {
            vector<Card> combo;
            for (int cardIndex = cardIndexToStart; cardIndex < PLAYERS_HAND.size(); cardIndex++) {
                const Card& CARD = PLAYERS_HAND[cardIndex];
                if (CARD.cost <= manaToBeUsed) {
                    combo.emplace_back(CARD);
                }
            }
            if (calculateCost(combo) == manaToBeUsed) {
                return combo;
            }
        }
    }    
}
int calculateCost(const vector<Card>& COMBO) {
    int cost = 0;
    for (const Card& CARD : COMBO) {
        cost += CARD.cost;
    }
    return cost;
}
void playCombo(const vector<Card>& COMBO, vector<Card>& playersBoard) {
    for (const Card& CARD : COMBO) {
        cout << "SUMMON " << CARD.instanceId << " yeeehhaaw;";
        playersBoard.emplace_back(CARD);
    }
}

int main() {
    // TODO struct DeckStructure with targetStructure, use it to calculateRating
    map<int, int> playersDeckStructure;
    int playersDeckCards = 0;

    // DRAFT
    while (playersDeckCards < 30) {
        auto [ player, opponent ] = readPlayers();
        vector<Action> opponentsActions = readOpponentsActions();
        auto [ opponentsBoard, playersHand, playersBoard ] = readCards();
        printState(opponent, opponentsBoard, opponentsActions, player, playersHand, playersBoard);
        double maxCardRating = 0;
        int cardNumberToPick = -1;

        cerr << "cardRatings: ";
        for (int cardNumber = 0; cardNumber < 3; cardNumber++) {
            const Card& CARD = playersHand[cardNumber];
            double cardRating = CARD.calculateRating(playersDeckStructure);
            cerr << cardRating << ", ";
            if (cardRating > maxCardRating) {
                maxCardRating = cardRating;
                cardNumberToPick = cardNumber;
            }
        }
        cerr << endl;
        cout << "PICK " << cardNumberToPick << endl;
        playersDeckCards++;
    }

    // BATTLE
    while (true) {
        State state = readAndSortBattleState();
        printState(state);
        
        const vector<Card> COMBO = findComboToUseMostMana(state.player.mana, state.playersHand);
        playCombo(COMBO, state.playersBoard);

        // TODO attack opponent with biggest att
        for (Card& playersCard : state.playersBoard) {
            bool canAttack = true;
            for (Card& opponentsCard : state.opponentsBoard) {
                if (canAttack == true && (opponentsCard.def > 0)) {
                    cout << "ATTACK " << playersCard.instanceId << " " << opponentsCard.instanceId << " DIE!;";
                    opponentsCard.def -= playersCard.att;
                    canAttack = false;
                }
            }
            if (canAttack == true) {
                cout << "ATTACK " << playersCard.instanceId << " -1 DIE MF!;";
            }
        }
        cout << endl;
    }
}