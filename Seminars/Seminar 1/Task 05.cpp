#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

struct Aircraft {
    double mass;
    double wingArea;
    double thrust;
    double CL;
    double CD;
};

int main() {
    const int N = 3;
    Aircraft planes[N];
    const double g = 9.81;
    double rho, V;

    cout << "Enter air density (kg/m^3): ";
    cin >> rho;
    cout << "Enter speed (m/s): ";
    cin >> V;

    for (int i = 0; i < N; i++) {
        cout << "\nAircraft " << i+1 << ":\n";
        cout << "Mass (kg): ";
        cin >> planes[i].mass;
        cout << "Wing area (m^2): ";
        cin >> planes[i].wingArea;
        cout << "Thrust (N): ";
        cin >> planes[i].thrust;
        cout << "Lift coefficient CL: ";
        cin >> planes[i].CL;
        cout << "Drag coefficient CD: ";
        cin >> planes[i].CD;
    }

    double bestTime = 1e9;
    int bestIndex = -1;

    for (int i = 0; i < N; i++) {
        double L = 0.5 * rho * V * V * planes[i].wingArea * planes[i].CL;
        double D = 0.5 * rho * V * V * planes[i].wingArea * planes[i].CD;
        double ay = (L - planes[i].mass * g) / planes[i].mass;

        if (ay <= 0) {
            cout << "Aircraft " << i+1 << " cannot climb (ay <= 0)." << endl;
            continue;
        }

        double h = 1000.0; // заданная высота из условия
        double t = sqrt(2 * h / ay);

        cout << fixed << setprecision(2);
        cout << "Aircraft " << i+1 << ": climb time = " << t << " s" << endl;

        if (t < bestTime) {
            bestTime = t;
            bestIndex = i;
        }
    }

    if (bestIndex != -1) {
        cout << "\nBest aircraft: " << bestIndex + 1
             << " with climb time = " << bestTime << " s" << endl;
    } else {
        cout << "No aircraft can climb." << endl;
    }

    return 0;
}
