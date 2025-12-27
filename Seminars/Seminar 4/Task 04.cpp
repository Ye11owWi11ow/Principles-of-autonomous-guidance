#include <iostream>
#include <iomanip>

using namespace std;

class Gyroscope {
private:
    double angularVelocity;
    bool calibrationRequired;

public:
    Gyroscope(double av, bool cr) : angularVelocity(av), calibrationRequired(cr) {}

    void calibrate() {
        calibrationRequired = false;
    }

    void printStatus() const {
        cout << fixed << setprecision(1);
        cout << "Angular velocity: " << angularVelocity << " °/s | Calibration "
             << (calibrationRequired ? "required" : "not required") << endl;
    }
};

int main() {
    double av;
    double cr_temp;
    bool cr;
    cout << "Enter angular velocity (°/s): ";
    cin >> av;
    cout << "Calibration required? (1 for yes, 0 for no): ";
    cin >> cr_temp;
    while (cr_temp!=0&&cr_temp!=1){
        cout << "Incorrect input. Calibration required? (1 for yes, 0 for no): ";
        cin >> cr_temp;
    }

    cr=cr_temp;

    Gyroscope g1(av, cr);
    g1.printStatus();

    if (cr_temp == 1) {
        cout << "Calibrating..." << endl;
        g1.calibrate();
        g1.printStatus();
        return 0;
    }

    else {
        return 0;
    }
}
