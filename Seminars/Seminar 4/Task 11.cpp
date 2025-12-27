#include <iostream>
#include <iomanip>

using namespace std;

// 1. Engine class
class Engine {
private:
    double thrust;
    double fuelFlow;
    double fuel;

public:
    Engine(double t, double ff, double f) : thrust(t), fuelFlow(ff), fuel(f) {}

    double getThrust() const { return thrust; }

    bool hasFuel() const { return fuel > 0; }

    void burn(double dt) {
        if (hasFuel()) {
            double consumed = fuelFlow * dt;
            if (consumed > fuel) consumed = fuel;
            fuel -= consumed;
        }
    }

    double getFuel() const { return fuel; }
};

// 2. Navigation class
class Navigation {
private:
    double altitude;
    double velocity;
    double acceleration;
    double mass;

public:
    Navigation(double alt, double vel, double m) : altitude(alt), velocity(vel), mass(m), acceleration(0) {}

    void update(double thrust, double dt) {
        acceleration = thrust / mass - 9.81;
        velocity += acceleration * dt;
        altitude += velocity * dt;
    }

    void printStatus(double time) const {
        cout << fixed << setprecision(2);
        cout << "t=" << time << "s | h=" << altitude << "m | v=" << velocity
             << "m/s | a=" << acceleration << "m/s²" << endl;
    }

    double getAltitude() const { return altitude; }
};

// 3. AutonomousFlightSystem class
class AutonomousFlightSystem {
private:
    Engine engine;
    Navigation nav;
    double time;

public:
    AutonomousFlightSystem(const Engine& e, const Navigation& n) : engine(e), nav(n), time(0) {}

    void simulate(double dt, double totalTime) {
        while (time < totalTime && engine.hasFuel()) {
            engine.burn(dt);
            nav.update(engine.getThrust(), dt);
            nav.printStatus(time);
            time += dt;
        }
    }

    void printSummary() const {
        cout << "--- Flight completed ---" << endl;
        cout << "Remaining fuel: " << engine.getFuel() << " kg" << endl;
        cout << "Final altitude: " << nav.getAltitude() << " m" << endl;
    }
};

int main() {
    double thrust, fuelFlow, fuel, alt, vel, mass, dt, totalTime;

    cout << "=== Engine parameters ===" << endl;
    cout << "Thrust (N): ";
    cin >> thrust;
    cout << "Fuel flow (kg/s): ";
    cin >> fuelFlow;
    cout << "Initial fuel (kg): ";
    cin >> fuel;

    cout << "\n=== Navigation parameters ===" << endl;
    cout << "Initial altitude (m): ";
    cin >> alt;
    cout << "Initial velocity (m/s): ";
    cin >> vel;
    cout << "Rocket mass (kg): ";
    cin >> mass;

    cout << "\n=== Simulation parameters ===" << endl;
    cout << "Time step (s): ";
    cin >> dt;
    cout << "Total simulation time (s): ";
    cin >> totalTime;

    Engine engine(thrust, fuelFlow, fuel);
    Navigation nav(alt, vel, mass);
    AutonomousFlightSystem system(engine, nav);

    system.simulate(dt, totalTime);
    system.printSummary();

    return 0;
}
