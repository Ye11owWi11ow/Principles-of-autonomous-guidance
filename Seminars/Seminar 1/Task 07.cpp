#include <iostream>

using namespace std;

int main() {
    double ax, ay;

    cout << "Enter horizontal acceleration (m/s^2): ";
    cin >> ax;
    cout << "Enter vertical acceleration (m/s^2): ";
    cin >> ay;

    if (ay > 0.5) {
        cout << "Mode: Climbing" << endl;
    } else if (ay >= 0 && ay <= 0.5) {
        cout << "Mode: Level flight" << endl;
    } else {
        cout << "Mode: Descent" << endl;
    }

    return 0;
}
