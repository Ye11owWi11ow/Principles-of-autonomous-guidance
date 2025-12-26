#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>

using namespace std;

struct Point {
    double x, y, z;
};

int main() {
    double vx, vy, vz, dt, t_max;
    cout << "Enter initial velocity vx (m/s): ";
    cin >> vx;
    cout << "Enter initial velocity vy (m/s): ";
    cin >> vy;
    cout << "Enter initial velocity vz (m/s): ";
    cin >> vz;
    cout << "Enter time step (s): ";
    cin >> dt;
    cout << "Enter max time (s): ";
    cin >> t_max;

    vector<Point> traj;
    double x = 0, y = 0, z = 0;

    for (double t = 0.0; t <= t_max; t += dt) {
        x += vx * dt;
        y += vy * dt;
        z += vz * dt;
        traj.push_back({x, y, z});
    }

    cout << fixed << setprecision(2);
    cout << setw(10) << "t (s)"
         << setw(15) << "x (m)"
         << setw(15) << "y (m)"
         << setw(15) << "z (m)" << endl;
    cout << string(55, '-') << endl;

    double t = 0.0;
    for (const auto& p : traj) {
        cout << setw(10) << t
             << setw(15) << p.x
             << setw(15) << p.y
             << setw(15) << p.z << endl;
        t += dt;
    }

    return 0;
}
