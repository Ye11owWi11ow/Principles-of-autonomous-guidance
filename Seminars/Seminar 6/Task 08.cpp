#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <numeric>
#include <cmath>
#include <iomanip>

using namespace std;

class FuelAnalyzer {
private:
    vector<double> time_data;
    vector<double> fuel_data;
    vector<double> rpm_data;

public:
    bool loadData(const string& filename) {
        ifstream fin(filename);
        if (!fin) {
            cerr << "Cannot open file: " << filename << endl;
            return false;
        }
        time_data.clear();
        fuel_data.clear();
        rpm_data.clear();

        string header;
        getline(fin, header);

        double t, f, r;
        char comma;
        while (fin >> t >> comma >> f >> comma >> r) {
            time_data.push_back(t);
            fuel_data.push_back(f);
            rpm_data.push_back(r);
        }
        fin.close();
        cout << "Loaded " << time_data.size() << " data points from " << filename << endl;
        return true;
    }

    void detectAnomalies() {
        if (fuel_data.empty()) {
            cout << "No data to analyze.\n";
            return;
        }

        double sum = accumulate(fuel_data.begin(), fuel_data.end(), 0.0);
        double avg = sum / fuel_data.size();

        // Calculate standard deviation
        double variance = 0.0;
        for (double f : fuel_data) {
            variance += (f - avg) * (f - avg);
        }
        variance /= fuel_data.size();
        double stddev = sqrt(variance);

        // Set threshold (1.5 * average from condition)
        double threshold = avg + 1.5;

        auto isAnomaly = [avg, threshold](double consumption) {
            return consumption > threshold;
        };

        cout << "\nFuel consumption analysis:\n";
        cout << fixed << setprecision(1);
        cout << "Average consumption: " << avg << " kg/s\n";
        cout << "Standard deviation: " << stddev << " kg/s\n";
        cout << "Anomaly threshold (1.5 × avg): " << threshold << " kg/s\n\n";

        cout << "Detected anomalies:\n";
        cout << "Time\tConsumption\tRPM\tStatus\n";
        int anomalyCount = 0;
        for (size_t i = 0; i < fuel_data.size(); ++i) {
            bool anomaly = isAnomaly(fuel_data[i]);
            if (anomaly) {
                cout << time_data[i] << "\t" << fuel_data[i] << "\t\t"
                     << rpm_data[i] << "\tANOMALY\n";
                anomalyCount++;
            }
        }
        cout << "\nTotal anomalies detected: " << anomalyCount << " out of "
             << fuel_data.size() << " records\n";
    }

    double calculateAverageConsumption() {
        if (fuel_data.empty()) return 0.0;
        return accumulate(fuel_data.begin(), fuel_data.end(), 0.0) / fuel_data.size();
    }

    void generateReport(const string& filename) {
        ofstream fout(filename);
        fout << "Fuel Consumption Analysis Report\n";
        fout << "================================\n\n";

        fout << fixed << setprecision(1);
        fout << "Data points analyzed: " << fuel_data.size() << "\n";
        fout << "Average consumption: " << calculateAverageConsumption() << " kg/s\n";
        fout << "Total fuel consumed: " << accumulate(fuel_data.begin(), fuel_data.end(), 0.0)
             << " kg\n\n";

        // Find threshold
        double avg = calculateAverageConsumption();
        double threshold = avg * 1.5;

        fout << "Anomaly detection (threshold = 1.5 × average = " << threshold << " kg/s):\n";
        fout << "Time(s)\tConsumption(kg/s)\tRPM\tAnomaly\n";

        for (size_t i = 0; i < fuel_data.size(); ++i) {
            bool anomaly = fuel_data[i] > threshold;
            fout << time_data[i] << "\t" << fuel_data[i] << "\t\t\t"
                 << rpm_data[i] << "\t" << (anomaly ? "YES" : "NO") << "\n";
        }

        fout.close();
        cout << "Report saved to " << filename << endl;
    }
};

void createSampleFile() {
    ofstream fout("fuel_data.csv");
    if (!fout) {
        cerr << "Cannot create sample file!" << endl;
        return;
    }
    fout << "time,fuel_consumption,engine_rpm\n";
    fout << "0,120,8000\n";
    fout << "1,125,8100\n";
    fout << "2,130,8200\n";
    fout << "3,200,8300\n";    // Anomaly: high consumption
    fout << "4,128,8250\n";
    fout << "5,126,8150\n";
    fout.close();
    cout << "Sample file 'fuel_data.csv' created with 6 records (1 anomaly).\n";
}

int main() {
    // Create sample file first
    createSampleFile();

    FuelAnalyzer analyzer;

    // Load data
    if (!analyzer.loadData("fuel_data.csv")) {
        return 1;
    }

    // Analyze for anomalies
    analyzer.detectAnomalies();

    // Generate report
    analyzer.generateReport("fuel_analysis_report.txt");

    return 0;
}
