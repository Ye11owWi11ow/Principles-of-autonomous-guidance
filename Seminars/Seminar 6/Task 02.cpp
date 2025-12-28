#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>

using namespace std;

struct Target {
    int id;
    string name;
    double x, y, z;
    double priority;
    double distance;
};

class TargetManager {
private:
    vector<Target> targets;
    string filename = "targets.bd";

public:
    void addTarget(int id, const string& name, double x, double y, double z, double priority, double distance) {
        targets.push_back({id, name, x, y, z, priority, distance});
    }

    bool removeTarget(int target_id) {
        auto it = remove_if(targets.begin(), targets.end(),
                            [target_id](const Target& t) { return t.id == target_id; });
        if (it != targets.end()) {
            targets.erase(it, targets.end());
            cout << "Target " << target_id << " removed.\n";
            return true;
        }
        cout << "Target " << target_id << " not found.\n";
        return false;
    }

    void saveTargetsToFile() {
        ofstream fout(filename);
        if (!fout) {
            cerr << "Error saving to file!" << endl;
            return;
        }
        fout << fixed << setprecision(1);
        for (const auto& t : targets) {
            fout << t.id << "," << t.name << "," << t.x << "," << t.y << "," << t.z << ","
                 << t.priority << "," << t.distance << "\n";
        }
        fout.close();
        cout << "Targets saved to " << filename << endl;
    }

    void loadTargetsFromFile() {
        ifstream fin(filename);
        if (!fin) {
            cerr << "File " << filename << " not found!" << endl;
            return;
        }
        targets.clear();
        int id;
        string name;
        double x, y, z, pr, dist;
        char comma;
        while (fin >> id >> comma) {
            getline(fin, name, ',');
            fin >> x >> comma >> y >> comma >> z >> comma >> pr >> comma >> dist;
            targets.push_back({id, name, x, y, z, pr, dist});
        }
        fin.close();
        cout << "Targets loaded from " << filename << endl;
    }

    vector<Target> getHighPriorityTargets(double min_priority) {
        vector<Target> result;
        copy_if(targets.begin(), targets.end(), back_inserter(result),
                [min_priority](const Target& t) { return t.priority >= min_priority; });
        return result;
    }

    void sortByDistance() {
        sort(targets.begin(), targets.end(),
             [](const Target& a, const Target& b) { return a.distance < b.distance; });
    }

    void printTargets() {
        cout << "\nTargets in system:\n";
        cout << "ID\tName\t\tPosition\t\tPriority\tDistance\n";
        for (const auto& t : targets) {
            cout << t.id << "\t" << t.name << "\t\t("
                 << fixed << setprecision(1) << t.x << "," << t.y << "," << t.z << ")\t"
                 << t.priority << "\t\t" << t.distance << " m\n";
        }
    }
};

void createSampleFile() {
    ofstream fout("targets.bd");
    if (!fout) {
        cerr << "Cannot create sample file!" << endl;
        return;
    }
    fout << "1,Name1,100.5,200.3,50.0,0.8,1500.0\n";
    fout << "2,Name2,150.2,180.7,300.0,0.9,2000.0\n";
    fout << "3,Name3,50.0,50.0,10.0,0.1,500.0\n";
    fout.close();
    cout << "Sample file 'targets.bd' created with 3 targets.\n";
}

int main() {
    // Create sample file first
    createSampleFile();

    TargetManager manager;

    // Load existing targets
    manager.loadTargetsFromFile();
    manager.printTargets();

    // Add a new target (from condition)
    cout << "\nAdding new target from condition...\n";
    manager.addTarget(4, "Name4", 200.0, 100.0, 20.0, 0.7, 1800.0);

    // Save all targets
    manager.saveTargetsToFile();

    // Print updated list
    cout << "\nAfter adding new target:\n";
    manager.loadTargetsFromFile();
    manager.printTargets();

    // Show high-priority targets
    cout << "\nHigh-priority targets (priority >= 0.5):\n";
    vector<Target> high = manager.getHighPriorityTargets(0.5);
    for (const auto& t : high) {
        cout << t.name << " (prio=" << t.priority << ", dist=" << t.distance << " m)\n";
    }

    // Sort by distance
    cout << "\nTargets sorted by distance:\n";
    manager.sortByDistance();
    manager.printTargets();

    return 0;
}
