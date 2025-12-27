#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>
#include <iomanip>

using namespace std;

int main() {
    int n;
    cout << "Enter number of y-coordinates: ";
    cin >> n;

    vector<double> y(n);
    for (int i = 0; i < n; ++i) {
        cout << "y[" << i << "]: ";
        cin >> y[i];
    }

    vector<double> diffs(n);
    adjacent_difference(y.begin(), y.end(), diffs.begin());

    double total_path = 0;
    for (size_t i = 1; i < diffs.size(); ++i) {
        total_path += abs(diffs[i]);
    }

    cout << fixed << setprecision(2);
    cout << "Total path length: " << total_path << " m" << endl;

    return 0;
}
