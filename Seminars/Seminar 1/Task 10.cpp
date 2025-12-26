#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

int main() {
    double Tmin, Tmax, dT, m, wingArea, CL, CD, V, rho, h;
    const double g = 9.81;

    cout << "Enter mass (kg): ";
    cin >> m;
    cout << "Enter wing area (m^2): ";
    cin >> wingArea;
    cout << "Enter lift coefficient CL: ";
    cin >> CL;
    cout << "Enter drag coefficient CD: ";
    cin >> CD;
    cout << "Enter speed (m/s): ";
    cin >> V;
    cout << "Enter air density (kg/m^3): ";
    cin >> rho;
    cout << "Enter target height (m): ";
    cin >> h;
    cout << "Enter min thrust (N): ";
    cin >> Tmin;
    cout << "Enter max thrust (N): ";
    cin >> Tmax;
    cout << "Enter thrust step (N): ";
    cin >> dT;

    double bestTime = 1e9;
    double bestThrust = Tmin;

    for (double T = Tmin; T <= Tmax; T += dT) {
        double L = 0.5 * rho * V * V * wingArea * CL;
        double D = 0.5 * rho * V * V * wingArea * CD;
        double ay = (L - m * g) / m;

        if (ay <= 0) continue;

        double t = sqrt(2 * h / ay);

        if (t < bestTime) {
            bestTime = t;
            bestThrust = T;
        }
    }

    cout << fixed << setprecision(2);
    cout << "\nOptimal thrust: " << bestThrust << " N\n";
    cout << "Minimum climb time: " << bestTime << " s" << endl;

    return 0;
}
