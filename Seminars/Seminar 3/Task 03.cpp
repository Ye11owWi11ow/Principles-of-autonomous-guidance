#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>

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

    vector<double> times;
    vector<double> velocities;
    double t_fall = v0 / g;

    for (double t = 0.0; t <= t_fall; t += dt) {
        double v = v0 - g * t;
        times.push_back(t);
        velocities.push_back(v);
    }

    cout << fixed << setprecision(2);
    cout << setw(10) << "t (s)" << setw(15) << "v (m/s)" << endl;
    cout << string(25, '-') << endl;
    for (size_t i = 0; i < times.size(); ++i) {
        cout << setw(10) << times[i] << setw(15) << velocities[i] << endl;
    }

    return 0;
}
