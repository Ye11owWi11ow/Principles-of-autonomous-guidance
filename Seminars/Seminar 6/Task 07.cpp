#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <iomanip>

using namespace std;

class WaypointSorter {
private:
    struct Waypoint {
        int id;
        double x, y, z;
        string name;
        double distance;
    };
    vector<Waypoint> waypoints;
    int removedCount = 0;

public:
    bool loadWaypoints(const string& filename) {
        ifstream fin(filename);
        if (!fin) {
            cerr << "Cannot open file: " << filename << endl;
            return false;
        }
        waypoints.clear();
        int id;
        double x, y, z;
        string name;
        char comma;
        while (fin >> id >> comma >> x >> comma >> y >> comma >> z >> comma) {
            getline(fin, name);
            waypoints.push_back({id, x, y, z, name, 0.0});
        }
        fin.close();
        cout << "Loaded " << waypoints.size() << " waypoints from " << filename << endl;
        return true;
    }

    void calculateDistances(double current_x, double current_y, double current_z) {
        for (auto& w : waypoints) {
            double dx = w.x - current_x;
            double dy = w.y - current_y;
            double dz = w.z - current_z;
            w.distance = sqrt(dx*dx + dy*dy + dz*dz);
        }
    }

    void sortByDistance() {
        sort(waypoints.begin(), waypoints.end(),
             [](const Waypoint& a, const Waypoint& b) { return a.distance < b.distance; });
        cout << "Waypoints sorted by distance.\n";
    }

    void saveSortedWaypoints(const string& filename) {
        ofstream fout(filename);
        if (!fout) {
            cerr << "Cannot write to file: " << filename << endl;
            return;
        }
        fout << fixed << setprecision(1);
        for (const auto& w : waypoints) {
            fout << w.id << "," << w.x << "," << w.y << "," << w.z << ","
                 << w.name << "," << w.distance << "\n";
        }
        fout.close();
        cout << "Sorted waypoints saved to " << filename << endl;
    }

    void printSorted() {
        cout << "\nWaypoints sorted by distance from current position:\n";
        cout << "ID\tName\t\t\tDistance (m)\tPosition\n";
        for (const auto& w : waypoints) {
            cout << w.id << "\t" << w.name;
            if (w.name.length() < 8) cout << "\t";
            cout << "\t\t" << fixed << setprecision(1) << w.distance
                 << "\t\t(" << w.x << ", " << w.y << ", " << w.z << ")\n";
        }
    }
};

void createSampleFile() {
    ofstream fout("waypoints.bd");
    if (!fout) {
        cerr << "Cannot create sample file!" << endl;
        return;
    }
    fout << "1,1000,500,2000,CheckpointA\n";
    fout << "2,2000,1500,2500,CheckpointB\n";
    fout << "3,500,1000,1500,CheckpointC\n";
    fout << "4,3000,2000,3000,CheckpointD\n";
    fout.close();
    cout << "Sample file 'waypoints.bd' created with 4 waypoints.\n";
}

int main() {
    // Create sample file first
    createSampleFile();

    WaypointSorter sorter;

    // Load waypoints
    if (!sorter.loadWaypoints("waypoints.bd")) {
        return 1;
    }

    // Current position from condition
    double currX = 800, currY = 600, currZ = 1800;
    cout << "\nCurrent position from condition:\n";
    cout << "X: " << currX << " m\n";
    cout << "Y: " << currY << " m\n";
    cout << "Z: " << currZ << " m\n";

    // Calculate distances
    sorter.calculateDistances(currX, currY, currZ);

    // Sort by distance
    sorter.sortByDistance();

    // Save sorted waypoints
    sorter.saveSortedWaypoints("waypoints_sorted.bd");

    // Print results
    sorter.printSorted();

    return 0;
}
