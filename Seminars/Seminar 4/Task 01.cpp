#include <iostream>
#include <iomanip>

using namespace std;

class Sensor {
private:
    double signalStrength;
    bool isActive;

public:
    Sensor(double strength) {
        setSignalStrength(strength);
        isActive = true;
    }

    void activate() {
        isActive = true;
    }

    void deactivate() {
        isActive = false;
        signalStrength = 0;
    }

    void setSignalStrength(double s) {
        signalStrength = (s < 0) ? 0 : s;
    }

    double getSignalStrength() const {
        return signalStrength;
    }

    void printStatus() const {
        cout << "Sensor " << (isActive ? "active" : "inactive")
             << " | Signal strength: " << fixed << setprecision(1)
             << signalStrength << " dB" << endl;
    }
};

int main() {
    double strength;
    cout << "Enter initial signal strength (dB): ";
    cin >> strength;

    Sensor s1(strength);
    s1.printStatus();

    cout << "Enter new signal strength: ";
    cin >> strength;
    s1.setSignalStrength(strength);
    s1.printStatus();

    s1.deactivate();
    s1.printStatus();

    return 0;
}
