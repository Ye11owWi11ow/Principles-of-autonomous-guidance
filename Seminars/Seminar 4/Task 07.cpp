#include <iostream>
#include <iomanip>

using namespace std;

class FlightComputer {
private:
    double altitude;
    double velocity;
    double fuel;
    double thrust;

public:
    FlightComputer(double alt, double vel, double f, double t)
        : altitude(alt), velocity(vel), fuel(f), thrust(t) {}

    void simulateStep(double dt) {
        if (fuel <= 0) return;
        velocity += (thrust / 1000 - 9.8) * dt;
        altitude += velocity * dt;
        fuel -= dt * 2;
        if (fuel < 0) fuel = 0;
    }

    void printStatus() const {
        cout << fixed << setprecision(1);
        cout << "Altitude: " << altitude << " m, Velocity: " << velocity
             << " m/s, Fuel: " << fuel << " kg" << endl;
    }
};

int main() {
    double alt, vel, fuel, thrust, dt;
    int steps;

    cout << "Enter initial altitude (m): ";
    cin >> alt;
    cout << "Enter initial velocity (m/s): ";
    cin >> vel;
    cout << "Enter initial fuel (kg): ";
    cin >> fuel;
    cout << "Enter thrust (N): ";
    cin >> thrust;
    cout << "Enter time step (s): ";
    cin >> dt;
    cout << "Enter number of simulation steps: ";
    cin >> steps;

    FlightComputer fc(alt, vel, fuel, thrust);
    for (int i = 0; i < steps; ++i) {
        fc.simulateStep(dt);
        cout << "Step " << i+1 << ": ";
        fc.printStatus();
    }

    return 0;
}
