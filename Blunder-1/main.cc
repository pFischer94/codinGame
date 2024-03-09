#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

vector<vector<char>> readMap(const int HEIGHT, const int WIDTH);
void printMap(const vector<vector<char>> &MAP);
int main();

enum Directions { NORTH, EAST, SOUTH, WEST };

struct Blunder {

    int row;
    int column;
    char field;
    Directions direction = SOUTH;
    const Directions PRIORITIES[4] = { SOUTH, EAST, NORTH, WEST };
    const Directions INVERTED_PRIORITIES[4] = { WEST, NORTH, EAST, SOUTH };
    string directionsStorage = "";
    bool isInverted = false;
    bool isInBreakerMode = false;

    void findStart(const vector<vector<char>> &MAP) {
        for (int row = 0; row < MAP.size(); row++) {
            for (int column = 0; column < MAP[0].size(); column++) {
                if (MAP[row][column] == '@') {
                    this->row = row;
                    this->column = column;
                    field = '@';
                    return;
                }
            }
        }
    }

    void printPosition(const vector<vector<char>> &MAP) {
        cerr << "position: [" << row << "][" << column << "]" << ", field: " << field << endl << endl;
    }

    bool isAccessible(const char NEXT_FIELD) {
        if (NEXT_FIELD == '#') {
            return false;
        }
        if (NEXT_FIELD == 'X' && !isInBreakerMode) {
            return false;
        }
        return true;
    }

    void changeDirectionIfNecessary(const vector<vector<char>> &MAP) {
        int triesUsed = 0;
        char nextField = getNextField(MAP);
        while (!isAccessible(nextField)) {
            if (triesUsed >= 4) {
                throw runtime_error("could not find direction to go");
            }
            direction = isInverted ? INVERTED_PRIORITIES[triesUsed] : PRIORITIES[triesUsed];
            triesUsed++;
            nextField = getNextField(MAP);
        }
    }

    void storeDirection() {
        switch (direction) {
            case NORTH:
                directionsStorage += '0';
                break;
            case EAST:
                directionsStorage += '1';
                break;
            case SOUTH:
                directionsStorage += '2';
                break;
            case WEST:
                directionsStorage += '3';
                break;
            default:
                throw runtime_error("illegal direction to store: " + direction);
        }
    }

    void detectLoop(bool &hasLoop) {
        int totalLength = directionsStorage.length();
        int rightStart = ceil(totalLength / 2.0);

        if ((totalLength - rightStart) % 2 != 0) rightStart++; // only even lenghts
        for (; rightStart < totalLength; rightStart += 2) {
            string rightPart = directionsStorage.substr(rightStart);
            if (!isSingleLoop(rightPart)) {
                continue;
            }
            int rightLength = rightPart.length();
            int leftStart = rightStart - rightLength;
            string leftPart = directionsStorage.substr(leftStart, rightLength);
            if (leftPart == rightPart) {
                hasLoop = true;
                return;
            }
        }
    }

    int ceil(double num) {
        int floor = (int) num;
        if (floor < num) {
            return floor + 1;
        } else {
            return floor;
        }
    }

    bool isSingleLoop(const string &PART_TO_CHECK) {
        int norths = 0;
        int easts = 0;
        int souths = 0;
        int wests = 0;
        for (char c : PART_TO_CHECK) {
            switch (c) {
                case '0':
                    norths++;
                    break;
                case '1':
                    easts++;
                    break;
                case '2':
                    souths++;
                    break;
                case '3':
                    wests++;
                    break;
                default:
                    cerr << "illegal stored direction: " << (int) c << ", storage: " << directionsStorage << endl;
                    throw runtime_error("illegal stored direction");
            }
        }
        return norths == souths && easts == wests;
    }

    void printDirections() {
        for (int i = 0; i < directionsStorage.length(); i++) {
            printDirection(directionsStorage.at(i));
        }
    }

    void printDirection(char directionNumber) {
        switch (directionNumber) {
            case '0':
                cout << "NORTH" << endl;
                break;
            case '1':
                cout << "EAST" << endl;
                break;
            case '2':
                cout << "SOUTH" << endl;
                break;
            case '3':
                cout << "WEST" << endl;
                break;
        }
    }

    void takeStep() {
        switch (direction) {
            case NORTH:
                row--;
                break;
            case EAST:
                column++;
                break;
            case SOUTH:
                row++;
                break;
            case WEST:
                column--;
                break;
        }
    }

    void teleport(const vector<vector<char>> &MAP) {
        for (int row = 0; row < MAP.size(); row++) {
            for (int column = 0; column < MAP[0].size(); column++) {
                if (MAP[row][column] == 'T' && (row != this->row || column != this->column)) {
                    this->row = row;
                    this->column = column;
                    return;
                }
            }
        }
    }

    void executeStepEffects(vector<vector<char>> &map) {
        field = map[row][column];
        switch (field) {
            case '@':
                throw runtime_error("back at @");
                break;
            case 'X':
                field = ' ';
                map[row][column] = ' ';
                printMap(map);
                break;
            case 'N':
                direction = NORTH;
                break;
            case 'E':
                direction = EAST;
                break;
            case 'S':
                direction = SOUTH;
                break;
            case 'W':
                direction = WEST;
                break;
            case 'B':
                isInBreakerMode = !isInBreakerMode;
                break;
            case 'I':
                isInverted = !isInverted;
                break;
            case 'T':
                teleport(map);
                break;
        }

    }

    void run(vector<vector<char>> &map) {
        bool hasLoop = false;
        while (field != '$' && !hasLoop) {
            changeDirectionIfNecessary(map);
            storeDirection();
            detectLoop(hasLoop);
            takeStep();
            executeStepEffects(map);
        }
        if (hasLoop) {
            cout << "LOOP" << endl;
        } else {
            printDirections();
        }
    }

    char getNextField(const vector<vector<char>> &MAP) {
        switch (direction) {
            case NORTH:
                return MAP[row - 1][column];
            case EAST:
                return MAP[row][column + 1];
            case SOUTH:
                return MAP[row + 1][column];
            case WEST:
                return MAP[row][column - 1];
        }
    }

};

vector<vector<char>> readMap(const int HEIGHT, const int WIDTH) {
    vector<vector<char>> map;
    map.reserve(HEIGHT);
    string line;

    for (int row = 0; row < HEIGHT; row++) {
        vector<char> chars;
        chars.reserve(WIDTH);

        getline(cin, line);
        for (int column = 0; column < WIDTH; column++) {
            chars.emplace_back(line.at(column));
        }
        map.emplace_back(chars);
    }
    return map;
}

void printMap(const vector<vector<char>> &MAP) {
    for (int row = 0; row < MAP.size(); row++) {
        cerr << setw(2) << row << "  ";
        for (int column = 0; column < MAP[0].size(); column++) {
            cerr << MAP[row][column] << " ";
        }
        cerr << endl;
    }
    cerr << endl;
}

int main() {
    int height;
    int width;
    cin >> height >> width; cin.ignore();
    vector<vector<char>> map = readMap(height, width);
    Blunder blunder;

    printMap(map);
    blunder.findStart(map);
    blunder.printPosition(map);
    blunder.run(map);
}