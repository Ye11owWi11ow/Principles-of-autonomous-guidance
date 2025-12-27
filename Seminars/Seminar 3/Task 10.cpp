#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;

class Rocket {
private:
    double x, y, z;
    double vx, vy, vz;
public:
    Rocket(double x0, double y0, double z0, double vx0, double vy0, double vz0)
        : x(x0), y(y0), z(z0), vx(vx0), vy(vy0), vz(vz0) {}

    void updatePosition(double dt) {
        x += vx * dt;
        y += vy * dt;
        z += vz * dt;
    }

    void printPosition() const {
        cout << fixed << setprecision(2);
        cout << "(" << x << ", " << y << ", " << z << ")";
    }
};

int main() {
    double dt, T;
    cout << "Enter time step dt (s): ";
    cin >> dt;
    cout << "Enter total simulation time T (s): ";
    cin >> T;

    vector<Rocket> squad;
    squad.emplace_back(0, 0, 0, 30, 10, 40);
    squad.emplace_back(0, 0, 0, 25, 15, 35);
    squad.emplace_back(0, 0, 0, 35, 5, 45);
    squad.emplace_back(0, 0, 0, 20, 20, 30);
    squad.emplace_back(0, 0, 0, 40, 0, 50);

    for (double t = 0; t <= T; t += dt) {
        for (auto& rocket : squad) {
            rocket.updatePosition(dt);
        }
    }

    cout << "\nFinal positions after " << T << " s:\n";
    for (size_t i = 0; i < squad.size(); ++i) {
        cout << "Rocket " << i+1 << ": ";
        squad[i].printPosition();
        cout << endl;
    }

    return 0;
}
