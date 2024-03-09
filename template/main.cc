#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <fstream>

using namespace std;

vector<int> read(const string& INPUT_NAME);
vector<int> readParamsFromLine(const string& LINE);
int runTest(const string& TEST_NAME);
void assertTest(const string& TEST_NAME, int result);
int main(int argc, char* argv[]);

vector<int> read(const string& INPUT_NAME) {
    int n;
    ifstream input;
    if (INPUT_NAME == "cin") {
        cin >> n; cin.ignore();
    } else {
        input = ifstream("inputs/" + INPUT_NAME + "_input");
        input >> n; input.ignore();
    }
    vector<int> ints;

    for (int i = 0; i < n; i++) {
        string line;
        if (INPUT_NAME == "cin") {
            getline(cin, line);
        } else {
            getline(input, line);
        }
        vector<int> params = readParamsFromLine(line);
    }
    return ints;
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

int runTest(const string& TEST_NAME) {
    vector<int> rooms;
    cerr << endl << endl;
    cerr << "Test name: " << TEST_NAME << endl;
    rooms = read(TEST_NAME);

    int result = 0;

    return result;
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

        int result = runTest(inputName);
        assertTest(inputName, result);

        double elapsed_seconds = endStopwatch(start);
        cout << "elapsed time: " << elapsed_seconds << " seconds" << endl;
    }
    cerr << endl << endl;
}