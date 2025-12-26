#include <iostream>
#include <iomanip>

using namespace std;

int main() {
    int n;
    cout << "Enter number of steps: ";
    cin >> n;

    double V[n], rho[n];

    for (int i = 0; i < n; i++) {
        cout << "Step " << i+1 << ":\n";
        cout << "Speed (m/s): ";
        cin >> V[i];
        cout << "Air density (kg/m^3): ";
        cin >> rho[i];
    }

    double S, CL;
    cout << "Enter wing area (m^2): ";
    cin >> S;
    cout << "Enter lift coefficient CL: ";
    cin >> CL;

    cout << "\nStep | Speed (m/s) | Density (kg/m^3) | Lift (N)\n";
    cout << "-------------------------------------------------\n";
    for (int i = 0; i < n; i++) {
        double L = 0.5 * rho[i] * V[i] * V[i] * S * CL;
        cout << setw(4) << i+1 << " | "
             << fixed << setprecision(2) << setw(12) << V[i] << " | "
             << setw(16) << rho[i] << " | "
             << setw(9) << L << endl;
    }

    return 0;
}
