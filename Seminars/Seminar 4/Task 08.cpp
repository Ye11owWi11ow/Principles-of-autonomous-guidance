#include <iostream>
#include <iomanip>

using namespace std;

class NavigationSystem {
private:
    double x, y;
    double velocityX, velocityY;
    bool gpsAvailable;

public:
    NavigationSystem(double x0, double y0, double vx, double vy, bool gps)
        : x(x0), y(y0), velocityX(vx), velocityY(vy), gpsAvailable(gps) {}

    void integratePosition(double dt) {
        x += velocityX * dt;
        y += velocityY * dt;
    }

    void correctGPS(double realX, double realY) {
        if (gpsAvailable) {
            x = (x + realX) / 2;
            y = (y + realY) / 2;
        }
    }

    void printPosition() const {
        cout << fixed << setprecision(2);
        cout << "Corrected coordinates: (" << x << ", " << y << ")" << endl;
    }
};

int main() {
    double x, y, vx, vy;
    bool gps;
    cout << "Enter initial X: ";
    cin >> x;
    cout << "Enter initial Y: ";
    cin >> y;
    cout << "Enter velocity X (m/s): ";
    cin >> vx;
    cout << "Enter velocity Y (m/s): ";
    cin >> vy;
    cout << "GPS available? (1 for yes, 0 for no): ";
    cin >> gps;

    NavigationSystem nav(x, y, vx, vy, gps);
    double dt;
    cout << "Enter integration time step (s): ";
    cin >> dt;
    nav.integratePosition(dt);

    double realX, realY;
    cout << "Enter real GPS X: ";
    cin >> realX;
    cout << "Enter real GPS Y: ";
    cin >> realY;
    nav.correctGPS(realX, realY);

    nav.printPosition();

    return 0;
}
