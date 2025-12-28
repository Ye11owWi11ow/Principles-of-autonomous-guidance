#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>

using namespace std;

struct Waypoint {
    int id;
    double x, y, z;
    double speed;
    string desc;
};

class WaypointManager {
private:
    vector<Waypoint> waypoints;
    string filename = "waypoints.bd";
    int currentIndex = 0;

public:
    void addWaypoint(int id, double x, double y, double z, double speed, const string& desc) {
        waypoints.push_back({id, x, y, z, speed, desc});
    }

    bool saveRoute() {
        ofstream fout(filename);
        if (!fout) return false;
        fout << fixed << setprecision(1);
        for (const auto& w : waypoints) {
            fout << w.id << "," << w.x << "," << w.y << "," << w.z << ","
                 << w.speed << "," << w.desc << "\n";
        }
        fout.close();
        cout << "Route saved to " << filename << endl;
        return true;
    }

    bool loadRoute() {
        ifstream fin(filename);
        if (!fin) return false;
        waypoints.clear();
        int id;
        double x, y, z, speed;
        string desc;
        char comma;
        while (fin >> id >> comma >> x >> comma >> y >> comma >> z >> comma >> speed >> comma) {
            getline(fin, desc);
            waypoints.push_back({id, x, y, z, speed, desc});
        }
        fin.close();
        cout << "Route loaded from " << filename << endl;
        return true;
    }

    double calculateTotalDistance() {
        double total = 0.0;
        for (size_t i = 1; i < waypoints.size(); ++i) {
            double dx = waypoints[i].x - waypoints[i-1].x;
            double dy = waypoints[i].y - waypoints[i-1].y;
            double dz = waypoints[i].z - waypoints[i-1].z;
            total += sqrt(dx*dx + dy*dy + dz*dz);
        }
        return total;
    }

    Waypoint getNextWaypoint() {
        if (currentIndex < waypoints.size()) {
            return waypoints[currentIndex];
        }
        return waypoints.back();
    }

    bool checkWaypointReached(double x, double y, double z, double tolerance = 10.0) {
        if (currentIndex >= waypoints.size()) return false;
        const auto& w = waypoints[currentIndex];
        double dist = sqrt((x-w.x)*(x-w.x) + (y-w.y)*(y-w.y) + (z-w.z)*(z-w.z));
        if (dist <= tolerance) {
            cout << "Waypoint '" << w.desc << "' reached!\n";
            currentIndex++;
            return true;
        }
        return false;
    }

    void printRoute() {
        cout << "\nLoaded route: " << waypoints.size() << " waypoints\n";
        cout << fixed << setprecision(2);
        cout << "Total distance: " << calculateTotalDistance() << " m\n";
        if (!waypoints.empty()) {
            cout << "\nWaypoint Details:\n";
            cout << "ID\tX\tY\tZ\tSpeed\tDescription\n";
            for (const auto& w : waypoints) {
                cout << w.id << "\t" << w.x << "\t" << w.y << "\t" << w.z
                     << "\t" << w.speed << "\t" << w.desc << "\n";
            }

            cout << "\nCurrent waypoint: " << waypoints[currentIndex].desc
                 << " (" << waypoints[currentIndex].x << ", "
                 << waypoints[currentIndex].y << ", "
                 << waypoints[currentIndex].z << ")\n";
            if (currentIndex + 1 < waypoints.size()) {
                cout << "Next waypoint: " << waypoints[currentIndex+1].desc
                     << " (" << waypoints[currentIndex+1].x << ", "
                     << waypoints[currentIndex+1].y << ", "
                     << waypoints[currentIndex+1].z << ")\n";
            }
        }
    }
};

void createSampleFile() {
    ofstream fout("waypoints.bd");
    if (!fout) {
        cerr << "Cannot create sample file!" << endl;
        return;
    }
    fout << "1,0,0,100,25,Start\n";
    fout << "2,100,50,150,30,CheckpointA\n";
    fout << "3,200,100,200,35,CheckpointB\n";
    fout.close();
    cout << "Sample file 'waypoints.bd' created with 3 waypoints.\n";
}

int main() {
    // Create sample file first
    createSampleFile();

    WaypointManager manager;

    // Load existing route
    manager.loadRoute();
    manager.printRoute();

    // Simulate reaching waypoints
    cout << "\nSimulating waypoint reaching...\n";

    // Try to reach Start (should succeed)
    manager.checkWaypointReached(0, 0, 100);

    // Try to reach CheckpointA from wrong position (should fail)
    manager.checkWaypointReached(50, 25, 120);

    // Move to correct position for CheckpointA (should succeed)
    manager.checkWaypointReached(100, 50, 150);

    // Calculate distance
    cout << "\nTotal route distance: " << fixed << setprecision(1)
         << manager.calculateTotalDistance() << " meters\n";

    return 0;
}
