#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <cmath>

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
        times.push_back(t);
        heights.push_back(h);
        if (h < 0) break;
    }

    auto it = find_if(heights.begin(), heights.end(),
                      [](double h) { return h <= 0.0; });

    if (it == heights.end()) {
        cout << "No landing in the simulation." << endl;
    } else {
        size_t idx = distance(heights.begin(), it);
        cout << fixed << setprecision(2);
        cout << "Landing time: " << times[idx] << " s" << endl;
        cout << "Height at landing: " << heights[idx] << " m" << endl;
    }

    return 0;
}
