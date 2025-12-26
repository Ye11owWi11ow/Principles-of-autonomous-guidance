#include <iostream>
#include <iomanip>

using namespace std;

int main() {
    int n;
    cout << "Enter number of aircraft: ";
    cin >> n;

    double bestAccel = -1e9;
    int bestIndex = -1;
    const double g = 9.81;

    for (int i = 0; i < n; i++) {
        double mass, wingArea, thrust, CL, CD, V, rho;

        cout << "\nAircraft " << i+1 << ":\n";
        cout << "Mass (kg): ";
        cin >> mass;
        cout << "Wing area (m^2): ";
        cin >> wingArea;
        cout << "Thrust (N): ";
        cin >> thrust;
        cout << "Lift coefficient CL: ";
        cin >> CL;
        cout << "Drag coefficient CD: ";
        cin >> CD;
        cout << "Speed (m/s): ";
        cin >> V;
        cout << "Air density (kg/m^3): ";
        cin >> rho;

        double L = 0.5 * rho * V * V * wingArea * CL;
        double D = 0.5 * rho * V * V * wingArea * CD;
        double ax = (thrust - D) / mass;
        double ay = (L - mass * g) / mass;

        cout << fixed << setprecision(2);
        cout << "Lift = " << L << " N, Drag = " << D << " N\n";
        cout << "Horizontal acceleration = " << ax << " m/s^2\n";
        cout << "Vertical acceleration = " << ay << " m/s^2\n";

        if (ax > bestAccel) {
            bestAccel = ax;
            bestIndex = i;
        }
    }

    if (bestIndex != -1) {
        cout << "\nBest aircraft: " << bestIndex + 1
             << " with horizontal acceleration = " << bestAccel << " m/s^2" << endl;
    }

    return 0;
}
