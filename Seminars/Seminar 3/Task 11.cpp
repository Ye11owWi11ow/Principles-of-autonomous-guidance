#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

using namespace std;

int main() {
    double v0, g, dt;

    cout << "Enter v0 (m/s): ";
    cin >> v0;
    cout << "Enter g (m/s^2): ";
    cin >> g;
    cout << "Enter dt (s): ";
    cin >> dt;

    vector<double> heights;
    for (double t = 0.0; t <= 100; t += dt) {
        double h = v0 * t - 0.5 * g * t * t;
        if (h < 0) break;
        heights.push_back(h);
    }

    if (heights.empty()) {
        cout << "No data." << endl;
        return 1;
    }

    auto max_it = max_element(heights.begin(), heights.end());
    double max_h = *max_it;
    double t_max = distance(heights.begin(), max_it) * dt;

    cout << fixed << setprecision(2);
    cout << "Max height: " << max_h << " m at t = " << t_max << " s" << endl;

    return 0;
}
