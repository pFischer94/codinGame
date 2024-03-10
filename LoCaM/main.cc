#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

struct Player;
struct Card;
struct Action;
struct State;

vector<Card> readDraftCards();
State readAndSortBattleState();
pair<Player, Player> readPlayers();
vector<Action> readOpponentsActions();
Action readOpponentsAction();
tuple<vector<Card>, vector<Card>, vector<Card>> readCards();

void printDraftCards(const vector<Card>& DRAFT_CARDS);
void printState(const State& STATE);

void draft();
int chooseCardNumberToPick(const vector<Card>& DRAFT_CARDS, double avgCost);

void battle();
vector<Card> findComboToUseMostMana(int mana, const vector<Card>& PLAYERS_HAND, const int SPACE_ON_BOARD);
int calculateCost(const vector<Card>& COMBO);
void playCombo(const vector<Card>& COMBO, vector<Card>& playersBoard, int& mana, int& spaceOnBoard, 
        vector<Card>& playersHand);
void attack(vector<Card>& playersBoard, vector<Card>& opponentsBoard, int& spaceOnBoard, bool& createdSpaceOnBoard);

int main();

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
    int instanceId, type, cost, att, def;
    bool canAttack = false, canBreak = false, canGuard = false;
    int playerHealthCharge, opponentHealthCharge, cardDraw;

    Card(int instanceId, int type, int cost, int att, int def, const string& ABILITIES, int my_health_change, 
            int opponent_health_change, int card_draw) : instanceId(instanceId), type(type), cost(cost), 
            att(att), def(def), playerHealthCharge(my_health_change), opponentHealthCharge(opponent_health_change), 
            cardDraw(card_draw) {
        for (char c : ABILITIES) {
            if (c == 'C') this->canAttack = true;
            else if (c == 'B') this->canBreak = true;
            else if (c == 'G') this->canGuard = true;
            else if (c != '-')  {
                throw runtime_error("illegal ABILITIES in Card(): " + ABILITIES);
            }
        }
    }

    // TODO ? def into battleValue?
    double calculateBattleValue() const {
        const int BREAK_VALUE = !this->canBreak ? 0 : 1;
        const double GUARD_VALUE = !this->canGuard ? 0 : def * 0.2;
        const double PHC_VALUE = playerHealthCharge * def * 0.3;
        const double OHC_VALUE = opponentHealthCharge * def * -0.3;

        const double BATTLE_VALUE = this->att + BREAK_VALUE + GUARD_VALUE + PHC_VALUE + OHC_VALUE;
        cerr << "att: " << this->att << ", brV: " << BREAK_VALUE << ", guV: " << GUARD_VALUE 
                << ", pHV: " << PHC_VALUE << ", oHV: " << OHC_VALUE;
        return BATTLE_VALUE;
    }
    double calculateDraftValue(double avgCost) const {
        this->print();
        const double TARGET_AVG_COST = 4;
        if (avgCost == 0) avgCost = TARGET_AVG_COST;
        const double AVG_COST_VALUE = (avgCost - TARGET_AVG_COST) * (this->cost - TARGET_AVG_COST) * -1;

        const int CHARGE_VALUE = !this->canAttack ? 0 : 1;
        const double DRW_VALUE = cardDraw * 0.5;
        const double DIVIDER = cost == 0 ? 0.5 : cost;
        
        const double DRAFT_VALUE = 
                (this->calculateBattleValue() + this->def + CHARGE_VALUE + DRW_VALUE + AVG_COST_VALUE) / DIVIDER;
        cerr << ", def: " << this->def << ", chV: " << CHARGE_VALUE << ", drV: " << DRW_VALUE 
                << ", aCV: " << AVG_COST_VALUE << ", DIV: " << DIVIDER << ", DV: " << DRAFT_VALUE << endl << endl;
        return DRAFT_VALUE;
    }
    void print() const {
        cerr << "Card [ id: " << setw(2) << instanceId << ", att: " << setw(2) << att << ", cost: " << setw(2) << cost 
                << ", def: " << setw(2) << def << ", rdy: " << canAttack 
                << ", pHC: " << setw(2) << playerHealthCharge << ", oHC: " << setw(2) << opponentHealthCharge 
                << ", drw: " << setw(1) << cardDraw << ", val: " << setw(4) << left << calculateBattleValue() << right 
                << (canBreak ? ", B" : "") << (canGuard ? ", G" : "") << " ]" << endl;
    }

    bool operator==(const Card& OTHER) const {
        return this->instanceId == OTHER.instanceId && this->type == OTHER.type && this->cost == OTHER.cost;
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
    Action(int attId, int targetId) {
        this->actionType = "ATTACK";
        this->activeId = attId;
        this->passiveId = targetId;
    }
    Action(string actionType, int itemId, int targetId) {
        this->actionType = actionType;
        this->activeId = itemId;
        this->passiveId = targetId;
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

    static Action readSummonAction(bool debug) {
        int summonedId;
        cin >> summonedId; 
        if (debug) cerr << ", summonedId: " << summonedId;
        return Action(summonedId);
    }
    static Action readAttackAction(bool debug) {
        int attId;
        int defId;
        cin >> attId >> defId;
        if (debug) cerr << ", attId: " << attId;
        if (debug) cerr << ", defId: " << defId;
        return Action(attId, defId);
    }
    static Action readUseAction(bool debug) {
        int itemId;
        int targetId;
        cin >> itemId >> targetId;
        if (debug) cerr << ", itemId: " << itemId;
        if (debug) cerr << ", targetId: " << targetId;
        return Action("USE", itemId, targetId);
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

vector<Card> readDraftCards() {
    readPlayers();
    readOpponentsActions();
    auto [ opponentsBoard, playersHand, playersBoard ] = readCards();
    return playersHand;
}
State readAndSortBattleState() {
    auto [ player, opponent ] = readPlayers();
    vector<Action> opponentsActions = readOpponentsActions();
    auto [ opponentsBoard, playersHand, playersBoard ] = readCards();

    sort(opponentsBoard.begin(), opponentsBoard.end(), 
            [](Card a, Card b) { return a.calculateBattleValue() > b.calculateBattleValue(); });
    sort(opponentsBoard.begin(), opponentsBoard.end(), 
            [](Card a, Card b) { return a.canGuard > b.canGuard; });
    sort(playersHand.begin(), playersHand.end(), [](Card a, Card b) { return a.cost > b.cost; });
    sort(playersBoard.begin(), playersBoard.end(), 
            [](Card a, Card b) { return a.calculateBattleValue() < b.calculateBattleValue(); });

    for_each(playersBoard.begin(), playersBoard.end(), [](Card& a) { a.canAttack = true; });
    return State(player, opponent, opponentsActions, opponentsBoard, playersHand, playersBoard);
}
pair<Player, Player> readPlayers() {
    int player_health, player_mana, player_deck, player_rune, player_draw;

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
        opponentsActions.emplace_back(readOpponentsAction());
    }
    return opponentsActions;
}
Action readOpponentsAction() {
    bool debug = false;
    int cardNumber;
    string actionType;
    cin >> cardNumber;
    cin >> actionType;

    if (debug) cerr << "readOpponentsAction(): cardNumber: " << cardNumber << ", actionType: " << actionType;
    if (actionType == "SUMMON") {
        return Action::readSummonAction(debug);
    } else if (actionType == "ATTACK") {
        return Action::readAttackAction(debug);
    } else if (actionType == "USE") {
        return Action::readUseAction(debug);
    } else {
        throw runtime_error("illegal actionType in readOpponentsActions(): " + actionType);
    }
}
tuple<vector<Card>, vector<Card>, vector<Card>> readCards() {
    bool debug = false;
    vector<Card> opponentsBoard;
    vector<Card> playersHand;
    vector<Card> playersBoard;
    int card_count;
    cin >> card_count; cin.ignore();
    
    for (int i = 0; i < card_count; i++) {
        int card_number, instance_id, location, card_type, cost, attack, defense, 
                my_health_change, opponent_health_change, card_draw;
        string abilities;
        cin >> card_number >> instance_id >> location >> card_type >> cost >> attack >> defense 
                >> abilities >> my_health_change >> opponent_health_change >> card_draw; cin.ignore();
        
        if (debug) cerr << "readCards(): attack: " << attack << ", defense: " << defense << ", abilities: " << abilities 
                << ", pHC: " << my_health_change << ", oHC " << opponent_health_change << endl;

        Card card = Card(instance_id, card_type, cost, attack, defense, abilities, my_health_change, 
                opponent_health_change, card_draw);
        
        if (location == -1) opponentsBoard.emplace_back(card);
        else if (location == 0) playersHand.emplace_back(card);
        else if (location == 1) playersBoard.emplace_back(card);
        else {
            string msg = "illegal location in readCards(): ";
            msg += location;
            throw runtime_error(msg);
        }
    }
    return tuple(opponentsBoard, playersHand, playersBoard);
}

void printDraftCards(const vector<Card>& DRAFT_CARDS) {
    cerr << "draftCards:" << endl;
    for (const Card& CARD : DRAFT_CARDS) {
        CARD.print();
    }
}
void printState(const State& STATE) {
    bool complete = false;
    
    if (complete) {
        cerr << "opponent:" << endl; 
        STATE.opponent.print(); 
        cerr << endl;
    }
    
    cerr << "opponentsBoard:" << endl;
    for (const Card& CARD : STATE.opponentsBoard) {
        CARD.print();
    }

    if (complete) {
        cerr << endl << "opponentsActions:" << endl;
            for (const Action& ACTION : STATE.opponentsActions) {
            ACTION.print();
        }
        cerr << endl << "player:" << endl;
        STATE.player.print();
    } 
    
    cerr << endl << "playersHand:" << endl;
    for (const Card& CARD : STATE.playersHand) {
        CARD.print();
    }
    cerr << endl << "playersBoard:" << endl;
    for (const Card& CARD : STATE.playersBoard) {
        CARD.print();
    }
}

void draft() {
    int playersDeckCards = 0;
    double avgCost = 0;
    while (playersDeckCards < 30) {
        const vector<Card> DRAFT_CARDS = readDraftCards();
        cerr << "avgCost: " << avgCost << endl << endl;
        int cardNumberToPick = chooseCardNumberToPick(DRAFT_CARDS, avgCost);
        const Card& CARD = DRAFT_CARDS[cardNumberToPick];

        cout << "PICK " << cardNumberToPick << endl;
        avgCost = (avgCost * playersDeckCards + CARD.cost) / ++playersDeckCards;
    }
    cerr << "avgCost: " << avgCost << endl;
}
int chooseCardNumberToPick(const vector<Card>& DRAFT_CARDS, double avgCost) {
    double maxDraftValue = 0;
    int cardNumberToPick = 0;

    for (int cardNumber = 0; cardNumber < 3; cardNumber++) {
        const Card& CARD = DRAFT_CARDS[cardNumber];
        if (CARD.type == 0) {
            double draftValue = CARD.calculateDraftValue(avgCost);
            if (draftValue > maxDraftValue) {
                maxDraftValue = draftValue;
                cardNumberToPick = cardNumber;
            }
        }
    }
    return cardNumberToPick;
}

void battle() {
    while (true) {
        State state = readAndSortBattleState();
        printState(state);
        
        if (state.playersHand.size() == 0 && state.playersBoard.size() == 0) {
            cout << "PASS";
        } else {
            int spaceOnBoard = 6 - state.playersBoard.size();
            bool createdSpaceOnBoard;
            do {
                createdSpaceOnBoard = false;
                const vector<Card> COMBO = findComboToUseMostMana(state.player.mana, state.playersHand, spaceOnBoard);
                playCombo(COMBO, state.playersBoard, state.player.mana, spaceOnBoard, state.playersHand);

                // TODO ? for each oppCard find combo with min battleValue to kill?
                // TODO only attack guards and then opponent?
                attack(state.playersBoard, state.opponentsBoard, spaceOnBoard, createdSpaceOnBoard);
            } while (createdSpaceOnBoard);
            
        }
        cout << endl;
    }
}
vector<Card> findComboToUseMostMana(int mana, const vector<Card>& PLAYERS_HAND, const int SPACE_ON_BOARD) {
    for (int manaToBeUsed = mana; manaToBeUsed >= 0; manaToBeUsed--) {
        for (int cardIndexToStart = 0; cardIndexToStart < PLAYERS_HAND.size(); cardIndexToStart++) {
            vector<Card> combo;
            int manaLeft = manaToBeUsed;
            for (int cardIndex = cardIndexToStart; cardIndex < PLAYERS_HAND.size(); cardIndex++) {
                const Card& CARD = PLAYERS_HAND[cardIndex];
                if (CARD.type == 0 && CARD.cost <= manaLeft && combo.size() < SPACE_ON_BOARD) {
                    combo.emplace_back(CARD);
                    manaLeft -= CARD.cost;
                }
            }
            if (calculateCost(combo) == manaToBeUsed) {
                return combo;
            }
        }
    }
    return vector<Card>();
}
int calculateCost(const vector<Card>& COMBO) {
    int cost = 0;
    for (const Card& CARD : COMBO) {
        cost += CARD.cost;
    }
    return cost;
}
void playCombo(const vector<Card>& COMBO, vector<Card>& playersBoard, int& mana, int& spaceOnBoard, 
        vector<Card>& playersHand) {
    for (const Card& CARD : COMBO) {
        cout << "SUMMON " << CARD.instanceId << ";";
        mana -= CARD.cost;
        playersBoard.emplace_back(CARD);
        spaceOnBoard--;
        playersHand.erase(remove(playersHand.begin(), playersHand.end(), CARD));
        if (spaceOnBoard < 0) {
            string msg = "illegal spaceOnBoard: ";
            msg += spaceOnBoard;
            throw runtime_error(msg);
        }
    }
}
void attack(vector<Card>& playersBoard, vector<Card>& opponentsBoard, int& spaceOnBoard, bool& createdSpaceOnBoard) {
    for (Card& playersCard : playersBoard) {
        if (playersCard.canAttack && playersCard.att > 0 && playersCard.def > 0) {
            for (Card& opponentsCard : opponentsBoard) {
                if (playersCard.canAttack && opponentsCard.def > 0) {
                    cout << "ATTACK " << playersCard.instanceId << " " << opponentsCard.instanceId << ";";
                    opponentsCard.def -= playersCard.att;
                    playersCard.def -= opponentsCard.att;
                    if (playersCard.def <= 0) {
                        spaceOnBoard++;
                        createdSpaceOnBoard = true;
                    }
                    playersCard.canAttack = false;
                }
            }
        }
        if (playersCard.canAttack) {
            cout << "ATTACK " << playersCard.instanceId << " -1;";
            playersCard.canAttack = false;
        }
    }
}

int main() {
    // TODO GitHub Repo with working .gitignore ignoring .vscode, a.out, solution.cc, .DS_Store
    // TODO use items
    // TODO take care of warnings
    // TODO ? struct DeckStructure with targetStructure, use it to calculateRating?
    draft();
    battle();
}