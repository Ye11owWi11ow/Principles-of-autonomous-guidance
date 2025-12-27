#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;

int main() {
    srand(time(0));
    int n;
    cout << "Enter number of measurements: ";
    cin >> n;

    vector<double> h(n);
    for (int i = 0; i < n; ++i) {
        h[i] = 100 + (rand() % 201 - 100) * 0.1; // base 100 + noise -10..10
    }

    vector<double> smoothed(n);
    smoothed[0] = h[0];
    smoothed[n-1] = h[n-1];

    for (size_t i = 1; i + 1 < n; ++i) {
        smoothed[i] = (h[i-1] + h[i] + h[i+1]) / 3.0;
    }

    cout << fixed << setprecision(2);
    cout << setw(10) << "Index" << setw(15) << "Original" << setw(15) << "Smoothed" << endl;
    cout << string(40, '-') << endl;
    for (size_t i = 0; i < n; ++i) {
        cout << setw(10) << i << setw(15) << h[i] << setw(15) << smoothed[i] << endl;
    }

    return 0;
}
