#include <iostream>
#include <iomanip>

using namespace std;

class Engine {
private:
    double thrust;
    double fuelFlow;

public:
    Engine(double T, double F) : thrust(T), fuelFlow(F) {}

    double getSpecificImpulse() const {
        if (fuelFlow == 0) return 0;
        return thrust / (fuelFlow * 9.81);
    }

    void printInfo() const {
        cout << fixed << setprecision(2);
        cout << "Thrust: " << thrust << " N | Fuel flow: " << fuelFlow
             << " kg/s | Specific impulse: " << getSpecificImpulse() << " s" << endl;
    }
};

int main() {
    double thrust, fuelFlow;
    cout << "Enter engine thrust (N): ";
    cin >> thrust;
    cout << "Enter fuel flow (kg/s): ";
    cin >> fuelFlow;

    Engine e1(thrust, fuelFlow);
    e1.printInfo();

    return 0;
}
