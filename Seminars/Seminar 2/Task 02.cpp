#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <algorithm>

using namespace std;

int main() {
    double v0, dt;
    const double g = 9.81;

    cout << "Enter initial velocity (m/s): ";
    cin >> v0;
    cout << "Enter time step (s): ";
    cin >> dt;

    vector<double> times;
    vector<double> heights;

    for (double t = 0.0; t <= 1000.0; t += dt) {
        double h = v0 * t - 0.5 * g * t * t;
        if (h < 0) break;
        times.push_back(t);
        heights.push_back(h);
    }

    if (heights.empty()) {
        cout << "No data." << endl;
        return 1;
    }

    auto it = max_element(heights.begin(), heights.end());
    size_t idx = distance(heights.begin(), it);

    cout << fixed << setprecision(2);
    cout << "Max height: " << heights[idx] << " m" << endl;
    cout << "Time at max height: " << times[idx] << " s" << endl;

    return 0;
}
