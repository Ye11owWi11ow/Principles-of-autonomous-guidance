#include <iostream>
#include <vector>
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

    if (v0 <= 0) {
        cout << "Initial velocity must be positive." << endl;
        return 1;
    }

    vector<double> times;
    vector<double> heights;
    times.reserve(100);
    heights.reserve(100);

    for (double t = 0.0; t <= 1000.0; t += dt) {
        double h = v0 * t - 0.5 * g * t * t;
        if (h < 0) break;
        times.push_back(t);
        heights.push_back(h);
    }

    cout << fixed << setprecision(2);
    cout << setw(10) << "t (s)" << setw(15) << "h (m)" << endl;
    cout << string(25, '-') << endl;
    for (size_t i = 0; i < times.size(); ++i) {
        cout << setw(10) << times[i] << setw(15) << heights[i] << endl;
    }

    return 0;
}
