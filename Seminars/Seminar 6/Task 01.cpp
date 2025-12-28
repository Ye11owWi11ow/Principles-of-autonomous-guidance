#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

class TrajectoryLogger {
private:
    string filename;
    struct Point {
        double x, y, z, speed, time;
    };
    vector<Point> points;

public:
    TrajectoryLogger(const string& filename) : filename(filename) {}

    void addPoint(double x, double y, double z, double speed, double time) {
        points.push_back({x, y, z, speed, time});
    }

    bool saveToCSV() {
        ofstream fout(filename);
        if (!fout) {
            cerr << "Error opening file for writing: " << filename << endl;
            return false;
        }
        fout << "time,x,y,z,speed\n";
        for (const auto& p : points) {
            fout << p.time << "," << p.x << "," << p.y << "," << p.z << "," << p.speed << "\n";
        }
        fout.close();
        cout << "Data saved to " << filename << endl;
        return true;
    }

    bool loadFromCSV() {
        ifstream fin(filename);
        if (!fin) {
            cerr << "Error opening file for reading: " << filename << endl;
            return false;
        }
        points.clear();
        string header;
        getline(fin, header);

        double t, x, y, z, s;
        char comma;
        while (fin >> t >> comma >> x >> comma >> y >> comma >> z >> comma >> s) {
            points.push_back({x, y, z, s, t});
        }
        fin.close();
        cout << "Loaded " << points.size() << " points from " << filename << endl;
        return true;
    }

    double calculateTotalDistance() {
        if (points.size() < 2) return 0.0;

        double total = 0.0;
        for (size_t i = 1; i < points.size(); ++i) {
            double dx = points[i].x - points[i-1].x;
            double dy = points[i].y - points[i-1].y;
            double dz = points[i].z - points[i-1].z;
            total += sqrt(dx*dx + dy*dy + dz*dz);
        }
        return total;
    }

    double findMaxSpeed() {
        if (points.empty()) return 0.0;

        double maxSpeed = points[0].speed;
        for (const auto& p : points) {
            if (p.speed > maxSpeed) maxSpeed = p.speed;
        }
        return maxSpeed;
    }

    void printStatistics() {
        cout << "\n=== Statistics ===" << endl;
        cout << "Number of points: " << points.size() << endl;
        cout << "Total distance: " << calculateTotalDistance() << " m" << endl;
        cout << "Maximum speed: " << findMaxSpeed() << " m/s" << endl;
    }
};

int main() {
    cout << "=== Trajectory Logger System ===" << endl;

    string filename;
    cout << "Enter filename for trajectory data (e.g., trajectory.csv): ";
    getline(cin, filename);

    TrajectoryLogger logger(filename);

    // Get number of points from user
    int numPoints;
    cout << "How many trajectory points do you want to add? ";
    cin >> numPoints;

    // Input points
    for (int i = 0; i < numPoints; ++i) {
        double x, y, z, speed, time;
        cout << "\nPoint " << (i + 1) << ":" << endl;
        cout << "X coordinate (m): ";
        cin >> x;
        cout << "Y coordinate (m): ";
        cin >> y;
        cout << "Z coordinate (m): ";
        cin >> z;
        cout << "Speed (m/s): ";
        cin >> speed;
        cout << "Time (s): ";
        cin >> time;

        logger.addPoint(x, y, z, speed, time);
    }

    // Save to file
    logger.saveToCSV();

    // Calculate and display statistics
    logger.printStatistics();

    // Load data back (optional)
    char choice;
    cout << "\nDo you want to load data from file? (y/n): ";
    cin >> choice;
    if (choice == 'y' || choice == 'Y') {
        logger.loadFromCSV();
        logger.printStatistics();
    }

    return 0;
}
