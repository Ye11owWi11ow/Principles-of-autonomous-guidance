#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    int n;
    cout << "Enter number of temperature readings: ";
    cin >> n;

    vector<double> temps(n);
    for (int i = 0; i < n; ++i) {
        cout << "Temp " << i+1 << " (°C): ";
        cin >> temps[i];
    }

    auto new_end = remove_if(temps.begin(), temps.end(),
                             [](double t) { return t < -100 || t > 200; });
    temps.erase(new_end, temps.end());

    cout << "\nCleaned temperatures:\n";
    for (double t : temps) cout << t << " ";
    cout << endl;

    return 0;
}
