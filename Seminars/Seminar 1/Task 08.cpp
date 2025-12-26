#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>

using namespace std;

struct Aircraft {
    double mass;
    double thrust;
    double CL;
    double CD;
    double wingArea;
    double climbTime;
};

int main() {
    int n;
    cout << "Enter number of aircraft: ";
    cin >> n;

    Aircraft planes[n];
    const double g = 9.81;
    double rho, V, h;

    cout << "Enter air density (kg/m^3): ";
    cin >> rho;
    cout << "Enter speed (m/s): ";
    cin >> V;
    cout << "Enter target height (m): ";
    cin >> h;

    for (int i = 0; i < n; i++) {
        cout << "\nAircraft " << i+1 << ":\n";
        cout << "Mass (kg): ";
        cin >> planes[i].mass;
        cout << "Thrust (N): ";
        cin >> planes[i].thrust;
        cout << "Lift coefficient CL: ";
        cin >> planes[i].CL;
        cout << "Drag coefficient CD: ";
        cin >> planes[i].CD;
        cout << "Wing area (m^2): ";
        cin >> planes[i].wingArea;

        double L = 0.5 * rho * V * V * planes[i].wingArea * planes[i].CL;
        double ay = (L - planes[i].mass * g) / planes[i].mass;

        if (ay <= 0) {
            planes[i].climbTime = 1e9; // очень большое время
        } else {
            planes[i].climbTime = sqrt(2 * h / ay);
        }
    }

    // Сортировка по времени
    for (int i = 0; i < n-1; i++) {
        for (int j = i+1; j < n; j++) {
            if (planes[j].climbTime < planes[i].climbTime) {
                swap(planes[i], planes[j]);
            }
        }
    }

    cout << "\nResults (sorted by climb time):\n";
    cout << "Aircraft | Climb Time (s)\n";
    cout << "-------------------------\n";
    for (int i = 0; i < n; i++) {
        if (planes[i].climbTime >= 1e9) {
            cout << setw(8) << i+1 << " | Cannot climb\n";
        } else {
            cout << setw(8) << i+1 << " | "
                 << fixed << setprecision(2) << planes[i].climbTime << endl;
        }
    }

    return 0;
}
