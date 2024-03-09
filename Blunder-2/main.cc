#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <fstream>

using namespace std;

struct Room;
vector<Room*> createRoomPointers(int n);
vector<Room*> readRooms(const string& INPUT_NAME);
vector<int> readParamsFromLine(const string& line);
vector<Room*> findEntriesAnd0(const vector<Room*>& ROOM_POINTERS);
void deleteRooms(vector<Room*>& roomPointers);
void printRooms(const vector<Room*>& ROOM_POINTERS);
int runTest(const string& TEST_NAME);
void assertTest(const string& TEST_NAME, int result);
int main(int argc, char* argv[]);

struct Room {

    int id;
    int money;
    int maxMoneyFromHere;
    bool isEntry;
    vector<Room*> roomsToGoTo;

    Room(int id) {
        this->id = id;
        this->money = 0;
        this->maxMoneyFromHere = 0;
        this->isEntry = true;
    }

    bool acceptsParams(const vector<int>& PARAMS, const vector<Room*>& ROOM_POINTERS) {
        if (this->id != PARAMS[0]) {
            return false;
        }
        this->money = PARAMS[1];
        
        for (int i = 2; i < PARAMS.size(); i++) {
            int roomNumber = PARAMS[i];
            Room* roomToGoTo = ROOM_POINTERS[roomNumber];
            roomToGoTo->isEntry = false;
            this->roomsToGoTo.emplace_back(roomToGoTo);
        }
        return true;
    }

    void calculateMaxMoneyFromEntry() {
        int maxMoneyFromPossiblePaths = 0;
        for (Room* roomToGoTo : roomsToGoTo) {
            int maxMoneyFromThisPath = roomToGoTo->calculateMaxMoneyFromRoom();
            if (maxMoneyFromThisPath > maxMoneyFromPossiblePaths) {
                maxMoneyFromPossiblePaths = maxMoneyFromThisPath;
            }
        }

        maxMoneyFromHere = money + maxMoneyFromPossiblePaths;
    }

    int calculateMaxMoneyFromRoom() {
        if (this->maxMoneyFromHere != 0) {
            return maxMoneyFromHere;
        }

        int maxMoneyFromPossiblePaths = 0;
        for (Room* roomToGoTo : roomsToGoTo) {
            int maxMoneyFromThisPath = roomToGoTo->calculateMaxMoneyFromRoom();
            if (maxMoneyFromThisPath > maxMoneyFromPossiblePaths) {
                maxMoneyFromPossiblePaths = maxMoneyFromThisPath;
            }
        }

        maxMoneyFromHere = money + maxMoneyFromPossiblePaths;
        return maxMoneyFromHere;
    }

    string toString() const {
        stringstream ss;
        ss << "Room [ id: " << setw(2) << id << ", money: " << setw(2) << money 
                << ", maxMoneyFromHere: " << maxMoneyFromHere << ", isEntry: " << isEntry << ", roomNumbersToGoTo: [";
        for (const Room* ROOM_TO_GO_TO : roomsToGoTo) {
            ss << ROOM_TO_GO_TO->id << ", ";
        }
        string result = ss.str();
        if (!roomsToGoTo.empty()) {
            result = result.substr(0, result.length() - 2);
        }
        return result + "] ]";
    }

};

vector<Room*> createRoomPointers(int n) {
    vector<Room*> roomPointers;
    for (int i = 0; i < n; i++) {
        roomPointers.emplace_back(new Room(i));
    }
    return roomPointers;
}

vector<Room*> readRooms(const string& INPUT_NAME) {
    int n;
    ifstream input;
    if (INPUT_NAME == "cin") {
        cin >> n; cin.ignore();
    } else {
        input = ifstream("inputs/" + INPUT_NAME + "_input");
        input >> n; input.ignore();
    }
    vector<Room*> roomPointers = createRoomPointers(n);

    for (int i = 0; i < n; i++) {
        string line;
        if (INPUT_NAME == "cin") {
            getline(cin, line);
        } else {
            getline(input, line);
        }
        vector<int> params = readParamsFromLine(line);
        
        Room* room = roomPointers[i];
        if (!room->acceptsParams(params, roomPointers)) {
            throw runtime_error("line [" + line + "] does not match " + room->toString());
        }
    }
    return roomPointers;
}

vector<int> readParamsFromLine(const string& LINE) {
    char delimiter = ' ';
    vector<int> params;

    int delimiterIndex = LINE.find(delimiter);
    string subStr = LINE.substr(0, delimiterIndex);
    int id = stoi(subStr);
    params.emplace_back(id);

    int startIndex = delimiterIndex + 1;
    delimiterIndex = LINE.find(delimiter, startIndex);
    subStr = LINE.substr(startIndex, delimiterIndex - startIndex);
    int money = stoi(subStr);
    params.emplace_back(money);

    while (delimiterIndex != string::npos) {
        startIndex = delimiterIndex + 1;
        delimiterIndex = LINE.find(delimiter, startIndex);
        subStr = LINE.substr(startIndex, delimiterIndex - startIndex);
        if (subStr != "E") {
            int roomNr = stoi(subStr);
            params.emplace_back(roomNr);
        }
    }
    return params;
}

vector<Room*> findEntriesAnd0(const vector<Room*>& ROOM_POINTERS) {
    vector<Room*> entries;
    for (Room* room : ROOM_POINTERS) {
        if (room->isEntry || room->id == 0) {
            entries.emplace_back(room);
        }
    }
    return entries;
}

void deleteRooms(vector<Room*>& roomPointers) {
    for (Room* ptr : roomPointers) {
        delete ptr;
        ptr = nullptr;
    }
}

void printRooms(const vector<Room*>& ROOM_POINTERS) {
    for (const Room* ROOM_POINTER : ROOM_POINTERS) {
        cerr << ROOM_POINTER->toString() << endl;
    }
}

int runTest(const string& TEST_NAME) {
    vector<Room*> rooms;
    cerr << endl << endl;
    cerr << "Test name: " << TEST_NAME << endl;
    rooms = readRooms(TEST_NAME);
    // printRooms(rooms);

    vector<Room*> entriesAnd0 = findEntriesAnd0(rooms);
    for (Room* entry : entriesAnd0) {
        entry->calculateMaxMoneyFromEntry();
    }
    // printRooms(entriesAnd0);

    int maxMoney = 0;
    for (Room* entry : entriesAnd0) {
        if (entry->maxMoneyFromHere > maxMoney) {
            maxMoney = entry->maxMoneyFromHere;
        }
    }
    cerr << "maxMoney from all entries: " << maxMoney << endl;

    maxMoney = entriesAnd0[0]->maxMoneyFromHere;
    cerr << "maxMoney from room 0: " << endl;
    cout << maxMoney << endl;
    deleteRooms(rooms);

    return maxMoney;
}

void assertTest(const string& TEST_NAME, int result) {
    ifstream output("inputs/" + TEST_NAME + "_output");
    int expected;
    output >> expected;
    if (result == expected) {
        cerr << "\033[32m";
    } else {
        cerr << "\033[31m";
    }
    cerr << "expected: " << expected << endl;
    cerr << "\033[0m";
}

chrono::system_clock::time_point startStopwatch() {
    chrono::system_clock::time_point start = chrono::system_clock::now();
    const time_t start_time = chrono::system_clock::to_time_t(start);
    return start;
}

double endStopwatch(const chrono::system_clock::time_point &start) {
    const chrono::system_clock::time_point end = chrono::system_clock::now();
    const chrono::duration<double> elapsed_seconds = end - start;
    return elapsed_seconds.count();
}

int main(int argc, char* argv[]) {
    bool shouldRunFromCIN = argc == 1;
    vector<string> inputNames;
    if (shouldRunFromCIN) {
        inputNames = { "cin" };
    } else {
        inputNames = { "1_room", "3_rooms", "15_rooms_small_range", "55_rooms", "1275_rooms", "5050_rooms", 
                "9870_rooms", "square_building", "multiple_entries" };
    }

    for (string inputName : inputNames) {
        chrono::system_clock::time_point start = startStopwatch();

        int maxMoney = runTest(inputName);
        assertTest(inputName, maxMoney);

        double elapsed_seconds = endStopwatch(start);
        cout << "elapsed time: " << elapsed_seconds << " seconds" << endl;
    }
    cerr << endl << endl;
}