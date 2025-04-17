#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <cmath>
#include "json.hpp"  // Make sure json.hpp is in the same directory

using json = nlohmann::json;
using namespace std;

// Convert number from a given base to decimal
long long baseToDecimal(const string& value, int base) {
    long long result = 0;
    for (int i = 0; i < value.length(); i++) {
        char ch = value[i];
        int digit;
        if (isdigit(ch)) digit = ch - '0';
        else digit = tolower(ch) - 'a' + 10;
        result = result * base + digit;
    }
    return result;
}

// Function to calculate Lagrange interpolation at x = 0
long long lagrangeInterpolationAtZero(const vector<pair<int, long long>>& points) {
    int k = points.size();
    long long result = 0;

    for (int i = 0; i < k; i++) {
        long double term = points[i].second;

        for (int j = 0; j < k; j++) {
            if (j != i) {
                term *= -points[j].first;
                term /= (points[i].first - points[j].first);
            }
        }
        result += round(term);
    }
    return result;
}

void processTestCase(const string& filename) {
    ifstream inFile(filename);
    json data;
    inFile >> data;

    int n = data["keys"]["n"];
    int k = data["keys"]["k"];

    vector<pair<int, long long>> points;

    // Use iterator to iterate over the JSON object instead of structured bindings
    for (json::iterator it = data.begin(); it != data.end(); ++it) {
        string key = it.key();
        if (key == "keys") continue;  // Skip the "keys" part

        int x = stoi(key);
        int base = stoi(it.value()["base"].get<string>());
        string valStr = it.value()["value"];
        long long y = baseToDecimal(valStr, base);

        points.emplace_back(x, y);
    }

    // Take first k points only
    vector<pair<int, long long>> firstK(points.begin(), points.begin() + k);

    long long c = lagrangeInterpolationAtZero(firstK);
    cout << "Secret for Test Case (" << filename << "): " << c << endl;
}

int main() {
    // Process both test cases
    processTestCase("testcase1.json");
    processTestCase("testcase2.json");
    return 0;
}
