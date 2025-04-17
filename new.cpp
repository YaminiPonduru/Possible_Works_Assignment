#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

// Decode a value from a given base to a long long integer
long long decode_y_value(const string& value, int base) {
    try {
        return stoll(value, nullptr, base);
    } catch (const exception& e) {
        throw runtime_error("Invalid value '" + value + "' for base " + to_string(base));
    }
}

// Lagrange interpolation to compute f(0) (constant term) given points (x, y)
long long lagrange_interpolation(const vector<pair<long long, long long>>& points) {
    long long result = 0;
    int n = points.size();

    for (int i = 0; i < n; ++i) {
        long long xi = points[i].first;
        long long yi = points[i].second;
        long double term = static_cast<long double>(yi); // Use long double for intermediate precision

        for (int j = 0; j < n; ++j) {
            if (i != j) {
                long long xj = points[j].first;
                if (xi == xj) {
                    throw runtime_error("Duplicate x values in points");
                }
                // Compute (0 - xj) / (xi - xj) for f(0)
                term *= static_cast<long double>(-xj) / (xi - xj);
            }
        }
        result += static_cast<long long>(term + 0.5); // Round to nearest integer
    }

    return result;
}

// Process a single test case and return the secret
long long find_secret(const json& test_case) {
    int n = test_case["keys"]["n"].get<int>();
    int k = test_case["keys"]["k"].get<int>();
    
    if (n < k) {
        throw runtime_error("Not enough roots provided: n < k");
    }

    vector<pair<long long, long long>> points;
    for (auto it = test_case.begin(); it != test_case.end(); ++it) {
        if (it.key() == "keys") continue; // Skip the "keys" field
        int x = stoi(it.key());
        string value = it.value()["value"].get<string>();
        int base = stoi(it.value()["base"].get<string>());
        long long y = decode_y_value(value, base);
        points.emplace_back(x, y);
    }

    if (points.size() < k) {
        throw runtime_error("Insufficient points extracted from test case");
    }

    // Use first k points for interpolation (as k points are sufficient)
    vector<pair<long long, long long>> selected_points(points.begin(), points.begin() + k);
    return lagrange_interpolation(selected_points);
}

int main() {
    try {
        // Read JSON input file
        ifstream input_file("testcase1.json");  // Use your test case file (e.g., "testcase1.json")
        if (!input_file.is_open()) {
            throw runtime_error("Failed to open input.json");
        }

        json data;
        input_file >> data;
        input_file.close();

        // Process each test case
        vector<long long> secrets;
        for (const auto& test_case : data["test_cases"]) {
            long long secret = find_secret(test_case);
            secrets.push_back(secret);
        }

        // Output secrets for all test cases
        cout << "Secrets for all test cases:" << endl;
        for (size_t i = 0; i < secrets.size(); ++i) {
            cout << "Test case " << (i + 1) << ": " << secrets[i] << endl;
        }

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
