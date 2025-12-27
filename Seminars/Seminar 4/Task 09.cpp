#include <iostream>
#include <iomanip>

using namespace std;

class AutonomousControl {
private:
    double altitude;
    double thrust;
    double targetAltitude;

public:
    AutonomousControl(double alt, double thr, double tgt)
        : altitude(alt), thrust(thr), targetAltitude(tgt) {}

    void updateControl() {
        if (altitude < targetAltitude) {
            thrust += 100;  // increase thrust
        } else {
            thrust -= 100;  // decrease thrust
        }
        if (thrust < 0) thrust = 0;
    }

    void simulateStep(double dt) {
        altitude += (thrust / 500) * dt;
    }

    void printStatus() const {
        cout << fixed << setprecision(2);
        cout << "Altitude: " << altitude << " m, thrust: " << thrust << " N" << endl;
    }
};

int main() {
    double alt, thrust, target, dt;
    int steps;

    cout << "Enter initial altitude (m): ";
    cin >> alt;
    cout << "Enter initial thrust (N): ";
    cin >> thrust;
    cout << "Enter target altitude (m): ";
    cin >> target;
    cout << "Enter time step (s): ";
    cin >> dt;
    cout << "Enter number of simulation steps: ";
    cin >> steps;

    AutonomousControl ac(alt, thrust, target);
    for (int i = 0; i < steps; ++i) {
        ac.updateControl();
        ac.simulateStep(dt);
        cout << "Step " << i+1 << ": ";
        ac.printStatus();
    }

    return 0;
}
