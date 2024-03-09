#include <iostream>
#include <string>
#include <cstring>  //memcmp()
#include <vector>

using namespace std;


struct POS {
    int x, y; 
    POS &operator+=(const POS &other) { x += other.x; y += other.y; return *this; }
    bool operator==(const POS &other) const { return (x == other.x) && (y == other.y); }
};
inline POS operator+(POS p1, const POS& p2) { return p1 += p2; }
inline bool operator!=(const POS& p1, const POS& p2) { return !(p1 == p2); }


template <typename T>
typename T::value_type& at(vector<T> &area, POS const &pos) { return area[pos.y][pos.x]; };


enum DIRECTION {
    DIR_MIN = 0,
    DIR_SOUTH = 0,
    DIR_EAST,
    DIR_NORTH,
    DIR_WEST,
    DIR_MAX
};

POS DPOS[] = { { 0, 1 }, { 1, 0 }, { 0,-1 }, {-1, 0 } };

char* DIR_TO_STRINGZ[] = { "SOUTH", "EAST", "NORTH", "WEST" };


struct ROBOT_STATE {
    int     direction;  // robot directon;
    bool    inverted;   // circuit state
    bool    breaker;    // breaker mode
    int     gen;        // state generation - generations obsolete when robot breaks obstacles
    bool operator==(const ROBOT_STATE& other) {
        return !memcmp(this, &other, sizeof(*this));
    }
};


vector<POS> TP_POSITIONS;
vector <string> city_map;
vector <vector <ROBOT_STATE>> city_states;  // robot state snapshots to check for infinite loop
const int GEN_START = 0;                        // city_states starting generation

class CRobot
{
    static vector<vector<int>> DirPriorities;

    POS         pos;
    ROBOT_STATE state;
public:
    bool        is_alive;
    
    CRobot(POS p) :
        is_alive(true),
        pos(p),
        state{ DIR_SOUTH, false, false, (GEN_START + 1) }
    {}
    
    bool StateRepeated(void) {
        return state == at(city_states, pos);
    }
    
    void ImprintState(void) {
        at(city_states, pos) = state;
    }
    
    bool CanMoveDir(int direction)
    {
        if (direction < DIR_MIN || direction >= DIR_MAX) throw "Invalid direction!";
        char ch = at(city_map, pos + DPOS[direction]);
        switch(ch) {
            case 'X': return state.breaker;
            case '#': return false;
        }
        return true;
    }

    int Wander(void)
    {
        auto movement_direction = state.direction;
        POS new_pos = pos + DPOS[movement_direction];

        switch(at(city_map, new_pos))
        {
            case ' ': case '@': break;
            case 'X':    
                if (state.breaker) {
                    at(city_map, new_pos) = ' ';// break obstacle
                    state.gen++;// invalidate city states
                    break;
                }
                // else fall through
            case '#':   // unbreakable obstacle - change dir
                for (auto &dir : DirPriorities[state.inverted]) {
                    if (CanMoveDir(dir)) {
                        state.direction = dir;
                        return Wander();
                    }
                }
                throw "Can't move!";
                
            case '$': is_alive = false; break;
            
            case 'N': state.direction = DIR_NORTH; break;
            case 'S': state.direction = DIR_SOUTH; break;
            case 'E': state.direction = DIR_EAST;  break;
            case 'W': state.direction = DIR_WEST;  break;
            
            case 'I': state.inverted = !state.inverted; break;
            case 'B': state.breaker  = !state.breaker;  break;
            
            case 'T':
                for (auto &tp : TP_POSITIONS) {
                    // Find first teleporter we aren't standing on.
                    if (tp != new_pos) { new_pos = tp; break; }
                }
                break;

            default: throw "Unknown city map symbol!";
        }
        
        pos = new_pos;
        return movement_direction;
    }
};

vector<vector<int>> CRobot::DirPriorities {
    {DIR_SOUTH, DIR_EAST, DIR_NORTH, DIR_WEST},
    {DIR_WEST, DIR_NORTH, DIR_EAST, DIR_SOUTH}
};


int main()
{
    POS start_pos;
    vector<int> Output;
    
    int L, C;
    cin >> L >> C; cin.ignore();
    
    city_map.resize(L);
    city_states.resize(L);
    
    for (int y = 0; y < L; y++)
    {
        getline(cin, city_map[y]); cerr << city_map[y] << endl;
        
        city_states[y].resize(C);
        for (int x = 0; x < C; x++)
        {
            POS pos{x, y};
            at(city_states, pos).gen = GEN_START;
            
            switch (at(city_map, pos)) {
                case '@': start_pos = pos; break;
                case 'T': TP_POSITIONS.push_back(pos); break;
            }
        }
    }
    
    try {    
        CRobot Bender(start_pos);
    
        while(Bender.is_alive)
        {
            if (Bender.StateRepeated()) { cout << "LOOP" << endl; return 0; }

            Bender.ImprintState();
    
            Output.push_back(Bender.Wander());
        }
    }
    catch (typeof("") ex) {
        cerr << ex << endl; exit(0);
    }

    for(auto &i : Output) {
        cout << DIR_TO_STRINGZ[i] << endl;
    }
}
