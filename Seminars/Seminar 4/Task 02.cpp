#include <iostream>
#include <iomanip>

using namespace std;

class Trajectory {
private:
    double startX, startY;
    double angle;

public:
    Trajectory() : startX(0), startY(0), angle(0) {}

    Trajectory(double x, double y, double ang) : startX(x), startY(y), angle(ang) {}

    void printTrajectory() const {
        cout << "Start: (" << fixed << setprecision(2) << startX << ", " << startY
             << "), angle: " << angle << "°" << endl;
    }
};

int main() {
    Trajectory t1;
    cout << "Default trajectory:" << endl;
    t1.printTrajectory();

    double x, y, ang;
    cout << "Enter start X: ";
    cin >> x;
    cout << "Enter start Y: ";
    cin >> y;
    cout << "Enter angle (degrees): ";
    cin >> ang;

    Trajectory t2(x, y, ang);
    cout << "Custom trajectory:" << endl;
    t2.printTrajectory();

    return 0;
}
