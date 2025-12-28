#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

class TelemetryFilter {
private:
    vector<vector<string>> data;

public:
    bool loadFromCSV(const string& filename) {
        ifstream fin(filename);
        if (!fin) {
            cerr << "Cannot open file: " << filename << endl;
            return false;
        }
        data.clear();
        string line;
        while (getline(fin, line)) {
            vector<string> row;
            stringstream ss(line);
            string cell;
            while (getline(ss, cell, ',')) {
                row.push_back(cell);
            }
            data.push_back(row);
        }
        fin.close();
        cout << "Loaded " << data.size() << " records from " << filename << endl;
        return true;
    }

    void filterData() {
        // Define validation lambdas from condition
        auto isValidAltitude = [](double alt) { return alt >= 0 && alt <= 20000; };
        auto isValidSpeed = [](double speed) { return speed >= 0 && speed <= 500; };

        vector<vector<string>> filtered;
        if (!data.empty()) {
            filtered.push_back(data[0]); // Keep header
        }

        int removed = 0;
        for (size_t i = 1; i < data.size(); ++i) {
            if (data[i].size() < 3) {
                removed++;
                continue;
            }

            try {
                double alt = stod(data[i][1]);
                double speed = stod(data[i][2]);

                if (isValidAltitude(alt) && isValidSpeed(speed)) {
                    filtered.push_back(data[i]);
                } else {
                    removed++;
                }
            } catch (...) {
                removed++;
            }
        }

        data = filtered;
        cout << "Filtered out " << removed << " invalid records.\n";
        cout << "Remaining valid records: " << data.size() - 1 << endl;
    }

    bool saveToCSV(const string& filename) {
        ofstream fout(filename);
        if (!fout) return false;
        for (const auto& row : data) {
            for (size_t j = 0; j < row.size(); ++j) {
                fout << row[j];
                if (j != row.size() - 1) fout << ",";
            }
            fout << "\n";
        }
        fout.close();
        cout << "Filtered data saved to " << filename << endl;
        return true;
    }

    void printFilteredStats() {
        if (data.size() <= 1) {
            cout << "No data or only header present.\n";
            return;
        }

        cout << "\nFiltered telemetry data:\n";
        cout << "Time\tAltitude\tSpeed\tHeading\tFuel\n";
        for (size_t i = 1; i < data.size(); ++i) {
            if (data[i].size() >= 5) {
                cout << data[i][0] << "\t" << data[i][1] << "\t\t"
                     << data[i][2] << "\t" << data[i][3] << "\t"
                     << data[i][4] << endl;
            }
        }

        // Calculate some statistics
        int validCount = data.size() - 1;
        cout << "\nStatistics:\n";
        cout << "Valid records: " << validCount << endl;
        cout << "Invalid records removed: " << 6 - data.size() << endl;
    }

    void setRemovedCount(int count) { removedCount = count; }

private:
    int removedCount = 0;
};

void createSampleCSVFile() {
    ofstream fout("telemetry.csv");
    if (!fout) {
        cerr << "Cannot create sample file!" << endl;
        return;
    }
    fout << "time,altitude,speed,heading,fuel\n";
    fout << "0,1000,250,45,80\n";
    fout << "1,1050,255,46,79\n";
    fout << "2,-50,260,47,78\n";        // Invalid: negative altitude
    fout << "3,1100,1000,48,77\n";      // Invalid: speed too high
    fout << "4,1150,265,49,76\n";
    fout.close();
    cout << "Sample file 'telemetry.csv' created with 5 records (2 invalid).\n";
}

int main() {
    // Create sample CSV file first
    createSampleCSVFile();

    TelemetryFilter filter;

    // Load data
    if (!filter.loadFromCSV("telemetry.csv")) {
        return 1;
    }

    // Apply filter
    cout << "\nApplying filters...\n";
    cout << "Valid altitude: -20000 m\n";
    cout << "Valid speed: -500 m/s\n";
    filter.filterData();

    // Save filtered data
    filter.saveToCSV("telemetry_filtered.csv");

    // Show filtered data
    filter.printFilteredStats();

    return 0;
}
