#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <cmath>

using namespace std;

int main() {
    double v0, dt;
    const double g = 9.81;

    cout << "Enter initial velocity (m/s): ";
    cin >> v0;
    cout << "Enter time step (s): ";
    cin >> dt;

    vector<double> heights;
    for (double t = 0.0; t <= 1000.0; t += dt) {
        double h = v0 * t - 0.5 * g * t * t;
        if (h < 0) break;
        heights.push_back(h);
    }

    cout << "Original size: " << heights.size() << endl;

    auto new_end = remove_if(heights.begin(), heights.end(),
                             [](double h) { return h < 10.0; });
    heights.erase(new_end, heights.end());

    cout << "After removing heights < 10 m: " << heights.size() << endl;
    cout << "Remaining heights:" << endl;
    for (double h : heights) cout << fixed << setprecision(2) << h << " ";
    cout << endl;

    return 0;
}
