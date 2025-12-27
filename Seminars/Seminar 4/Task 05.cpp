#include <iostream>
#include <iomanip>

using namespace std;

class Autopilot {
private:
    double courseAngle;
    double altitude;

public:
    Autopilot(double course, double alt) : courseAngle(course), altitude(alt) {}

    void changeCourse(double delta) {
        courseAngle += delta;
    }

    void changeAltitude(double delta) {
        altitude += delta;
    }

    void printStatus() const {
        cout << fixed << setprecision(2);
        cout << "Course: " << courseAngle << "°, altitude: " << altitude << " m" << endl;
    }
};

int main() {
    double course, alt;
    cout << "Enter initial course (°): ";
    cin >> course;
    cout << "Enter initial altitude (m): ";
    cin >> alt;

    Autopilot a1(course, alt);

    double deltaCourse, deltaAlt;
    cout << "Enter course change (°): ";
    cin >> deltaCourse;
    cout << "Enter altitude change (m): ";
    cin >> deltaAlt;

    a1.changeCourse(deltaCourse);
    a1.changeAltitude(deltaAlt);
    a1.printStatus();

    return 0;
}
