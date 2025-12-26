#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <cmath>

using namespace std;

void simulate(vector<double>& times, vector<double>& heights, double v0, double dt) {
    const double g = 9.81;
    times.clear();
    heights.clear();
    for (double t = 0.0; t <= 1000.0; t += dt) {
        double h = v0 * t - 0.5 * g * t * t;
        times.push_back(t);
        heights.push_back(h);
        if (h < 0) break;
    }
}

int main() {
    double v01, v02, dt;
    cout << "Enter initial velocity for rocket 1 (m/s): ";
    cin >> v01;
    cout << "Enter initial velocity for rocket 2 (m/s): ";
    cin >> v02;
    cout << "Enter time step (s): ";
    cin >> dt;

    vector<double> t1, h1, t2, h2;
    simulate(t1, h1, v01, dt);
    simulate(t2, h2, v02, dt);

    auto max_it1 = max_element(h1.begin(), h1.end());
    auto max_it2 = max_element(h2.begin(), h2.end());
    double max1 = *max_it1, max2 = *max_it2;

    auto land_it1 = find_if(h1.begin(), h1.end(), [](double h) { return h <= 0.0; });
    auto land_it2 = find_if(h2.begin(), h2.end(), [](double h) { return h <= 0.0; });
    double land_t1 = (land_it1 != h1.end()) ? t1[distance(h1.begin(), land_it1)] : -1;
    double land_t2 = (land_it2 != h2.end()) ? t2[distance(h2.begin(), land_it2)] : -1;

    cout << fixed << setprecision(2);
    cout << "Rocket 1: Max height = " << max1 << " m, Landing time = " << land_t1 << " s" << endl;
    cout << "Rocket 2: Max height = " << max2 << " m, Landing time = " << land_t2 << " s" << endl;

    if (max1 > max2) cout << "Rocket 1 reached higher altitude." << endl;
    else if (max2 > max1) cout << "Rocket 2 reached higher altitude." << endl;
    else cout << "Both reached the same altitude." << endl;

    if (land_t1 > land_t2) cout << "Rocket 1 stayed longer in air." << endl;
    else if (land_t2 > land_t1) cout << "Rocket 2 stayed longer in air." << endl;
    else cout << "Both stayed the same time in air." << endl;

    return 0;
}
