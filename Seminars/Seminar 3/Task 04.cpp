#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <numeric>

using namespace std;

int main() {
    double v0, dt, t_max;
    const double g = 9.81;

    cout << "Enter initial velocity (m/s): ";
    cin >> v0;
    cout << "Enter time step (s): ";
    cin >> dt;
    cout << "Enter max time (s): ";
    cin >> t_max;

    vector<double> velocities;
    double t_fall = v0 / g;

    for (double t = 0.0; t <= t_fall; t += dt) {
        double v = v0 - g * t;
        velocities.push_back(v);
    }

    if (velocities.empty()) {
        cout << "No data." << endl;
        return 1;
    }

    double sum = accumulate(velocities.begin(), velocities.end(), 0.0);
    double avg = sum / velocities.size();

    cout << fixed << setprecision(2);
    cout << "Average velocity: " << avg << " m/s" << endl;

    return 0;
}
