#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    int n;
    cout << "Enter number of x-coordinates: ";
    cin >> n;

    vector<double> x(n);
    for (int i = 0; i < n; ++i) {
        cout << "x[" << i << "]: ";
        cin >> x[i];
    }

    sort(x.begin(), x.end());
    auto last = unique(x.begin(), x.end());
    x.erase(last, x.end());

    cout << "\nUnique sorted coordinates:\n";
    for (double val : x) cout << val << " ";
    cout << endl;

    return 0;
}
