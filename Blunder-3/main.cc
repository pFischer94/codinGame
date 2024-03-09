#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <math.h>
#include <map>

using namespace std;

vector<vector<double>> read(const string& INPUT_NAME);
double calculateBigOAverageDiff(int bigOIndex, const vector<vector<double>>& XY_VALUES);
double calculate(int bigONumber, int n);
double calculateFactor(const vector<double>& BIG_O_VALUES, const vector<double>& Y_VALUES);
double calculateAverageDiff(const vector<double>& FACTORED_BIG_O_VALUES, const vector<double>& Y_VALUES);
void printMap(const map<string, double>& BIG_O_STANDARD_DEVIATIONS);
string runTest(const string& TEST_NAME);
void assertTest(const string& TEST_NAME, int result);
int main(int argc, char* argv[]);

vector<vector<double>> read(const string& INPUT_NAME) {
    int n;
    ifstream input;
    if (INPUT_NAME == "cin") {
        cin >> n; cin.ignore();
    } else {
        input = ifstream("inputs/" + INPUT_NAME + "_input");
        input >> n; input.ignore();
    }
    vector<vector<double>> values;
    vector<double> xs;
    vector<double> ys;
    values.reserve(2);
    xs.reserve(n);
    ys.reserve(n);

    for (int i = 0; i < n; i++) {
        int x;
        int y;
        if (INPUT_NAME == "cin") {
            cin >> x >> y;
        } else {
            input >> x >> y;
        }
        if (i > n * 0.1) {
            xs.emplace_back(x);
            ys.emplace_back(y);
        }
    }

    values.emplace_back(xs);
    values.emplace_back(ys);
    return values;
}

double calculateBigOAverageDiff(int bigOIndex, const vector<vector<double>>& XY_VALUES) {
    vector<double> bigOValues;
    vector<double> factoredBigOValues;
    bigOValues.reserve(XY_VALUES.size());
    factoredBigOValues.reserve(XY_VALUES.size());

    for (double x : XY_VALUES[0]) {
        bigOValues.emplace_back(calculate(bigOIndex, x));
    }
    double factor = calculateFactor(bigOValues, XY_VALUES[1]);
    for (double bigOValue : bigOValues) {
        factoredBigOValues.emplace_back(factor * bigOValue);
    }
    
    return calculateAverageDiff(factoredBigOValues, XY_VALUES[1]);
}

double calculate(int bigONumber, int n) {
        switch (bigONumber) {
            case 0:
                return 1;
            case 1:
                return  log(n);
            case 2:
                return n;
            case 3:
                return n * log(n);
            case 4:
                return pow(n, 2);
            case 5:
                return pow(n, 2) * log(n);
            case 6:
                return pow(n, 3);
            case 7:
                return pow(2, n);
            default:
                throw runtime_error("tried to calculate for missing Big O");
        }
    }

double calculateFactor(const vector<double>& BIG_O_VALUES, const vector<double>& Y_VALUES) {
    double sumOfDivisions = 0;
    for (int i = 0; i < BIG_O_VALUES.size(); i++) {
        sumOfDivisions += Y_VALUES[i] / BIG_O_VALUES[i];
    }
    return sumOfDivisions / BIG_O_VALUES.size();
}

double calculateAverageDiff(const vector<double>& FACTORED_BIG_O_VALUES, const vector<double>& Y_VALUES) {
    double sumOfAbsoluteDiffs = 0;
    for (int i = 0; i < Y_VALUES.size(); i++) {
        double absoluteDiff = fabs(Y_VALUES[i] - FACTORED_BIG_O_VALUES[i]);
        sumOfAbsoluteDiffs += absoluteDiff;
    }
    return sumOfAbsoluteDiffs / (Y_VALUES.size());
}

void printMap(const map<string, double>& BIG_O_STANDARD_DEVIATIONS) {
    int width = 20;
    for (const auto& entry : BIG_O_STANDARD_DEVIATIONS) {
        cerr << left << setw(width) << entry.first << " has value " << right << setw(width) << entry.second << endl;
    }
}

string runTest(const string& TEST_NAME) {
    vector<string> bigOs = { "O(1)", "O(log n)", "O(n)", "O(n log n)", "O(n^2)", "O(n^2 log n)", "O(n^3)", "O(2^n)" };
    vector<vector<double>> xyValues = read(TEST_NAME);
    map<string, double> bigOAverageDiff;
    double minAverageDiff;
    string bigOWithMinAverageDiff = "blank";

    // cerr << endl << endl;
    // cerr << "Test name: " << TEST_NAME << endl;
    
    for (int bigOIndex = 0; bigOIndex < bigOs.size(); bigOIndex++) {
        double averageDiff = calculateBigOAverageDiff(bigOIndex, xyValues);
        bigOAverageDiff.insert({ bigOs[bigOIndex], averageDiff });
    }
    // printMap(bigOAverageDiff);
    
    minAverageDiff = bigOAverageDiff.at(bigOs[0]);
    for (string bigO : bigOs) {
        if (bigOAverageDiff.at(bigO) <= minAverageDiff) {
            minAverageDiff = bigOAverageDiff.at(bigO);
            bigOWithMinAverageDiff = bigO;
        }
    }

    string result = bigOWithMinAverageDiff;
    // cerr << "result:   ";
    // cout << result << endl;
    return result;
}

void assertTest(const string& TEST_NAME, const string& RESULT) {
    string expected = TEST_NAME;
    if (RESULT == expected) {
        // cerr << "\033[32m";
    } else {
        cerr << "\033[31m";
        cerr << "test failed" << "\033[0m" << endl;
    }
    // cerr << "expected: " << expected << "\033[0m" << endl;
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
        inputNames = { "O(1)", "O(n^2 log n)", "O(2^n)" };
    }
    
    for (string inputName : inputNames) {
        int warmups = 20;
        for (int i = 0; i < warmups; i++) {
            runTest(inputName);
        }

        int executions = 20;
        double sumOfTimes = 0;
        cerr << endl << endl;
        cerr << "Test name: " << inputName << endl;
        for (int i = 0; i < executions; i++) {
            chrono::system_clock::time_point start = startStopwatch();
            string result = runTest(inputName);
            if (inputName != "cin") {
                assertTest(inputName, result);
            }
            double elapsedSeconds = endStopwatch(start);
            // cerr << "elapsed seconds: " << elapsedSeconds << endl;
            sumOfTimes += elapsedSeconds;
        }
        double avg = sumOfTimes / executions;
        cerr << "average elapsed seconds: " << avg << endl;
    }

    cerr << endl << endl;
}