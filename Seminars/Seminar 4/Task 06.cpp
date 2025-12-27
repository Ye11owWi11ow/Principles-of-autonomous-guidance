#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

class RocketStage {
private:
    double thrust;
    double burnTime;
    double mass;

public:
    RocketStage(double t, double bt, double m) : thrust(t), burnTime(bt), mass(m) {}

    double getDeltaV() const {
        return (thrust * burnTime) / mass;
    }

    void printInfo() const {
        cout << fixed << setprecision(2);
        cout << "Thrust: " << thrust << " N, Burn time: " << burnTime
             << " s, Mass: " << mass << " kg, Delta V: " << getDeltaV() << " m/s" << endl;
    }
};

int main() {
    int n;
    cout << "Enter number of stages: ";
    cin >> n;

    vector<RocketStage> stages;
    for (int i = 0; i < n; ++i) {
        double t, bt, m;
        cout << "Stage " << i+1 << ":\n";
        cout << "  Thrust (N): ";
        cin >> t;
        cout << "  Burn time (s): ";
        cin >> bt;
        cout << "  Mass (kg): ";
        cin >> m;
        stages.emplace_back(t, bt, m);
    }

    double totalV = 0;
    for (const auto& stage : stages) {
        stage.printInfo();
        totalV += stage.getDeltaV();
    }

    cout << fixed << setprecision(2);
    cout << "Total rocket velocity: " << totalV << " m/s" << endl;

    return 0;
}
