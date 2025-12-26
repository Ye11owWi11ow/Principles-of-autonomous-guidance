#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    int n;
    cout << "Enter number of sensor readings: ";
    cin >> n;

    vector<double> data(n);
    for (int i = 0; i < n; ++i) {
        cout << "Reading " << i+1 << ": ";
        cin >> data[i];
    }

    int zero_count = count(data.begin(), data.end(), 0.0);
    cout << "Zero readings: " << zero_count << endl;

    return 0;
}
