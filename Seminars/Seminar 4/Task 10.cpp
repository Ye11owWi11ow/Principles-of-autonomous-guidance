#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

using namespace std;

class DroneFlight {
private:
    vector<double> x, y;
    double totalDistance;

public:
    DroneFlight() : totalDistance(0) {
        x.push_back(0);
        y.push_back(0);
    }

    void addPoint(double newX, double newY) {
        double dx = newX - x.back();
        double dy = newY - y.back();
        totalDistance += sqrt(dx*dx + dy*dy);
        x.push_back(newX);
        y.push_back(newY);
    }

    double getTotalDistance() const {
        return totalDistance;
    }

    void printPath() const {
        cout << "Route points:" << endl;
        for (size_t i = 0; i < x.size(); ++i) {
            cout << "(" << fixed << setprecision(2) << x[i] << ", " << y[i] << ")" << endl;
        }
    }
};

int main() {
    DroneFlight d;
    int n;
    cout << "Enter number of additional points: ";
    cin >> n;

    for (int i = 0; i < n; ++i) {
        double nx, ny;
        cout << "Point " << i+1 << " X: ";
        cin >> nx;
        cout << "Point " << i+1 << " Y: ";
        cin >> ny;
        d.addPoint(nx, ny);
    }

    d.printPath();
    cout << fixed << setprecision(2);
    cout << "Total distance: " << d.getTotalDistance() << " m" << endl;

    return 0;
}
