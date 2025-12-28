#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

struct TelemetryData {
    double time;
    double altitude;
    double speed;
    double heading;
    double fuel;
};

class TelemetryLogger {
private:
    string currentFile;
    int fileCounter = 1;
    const int maxFileSize = 1000;
    vector<TelemetryData> buffer;

public:
    TelemetryLogger() {
        currentFile = "telemetry_001.bin";
    }

    bool logData(double time, double altitude, double speed, double heading, double fuel) {
        buffer.push_back({time, altitude, speed, heading, fuel});
        if (buffer.size() >= maxFileSize) {
            rotateFileIfNeeded();
        }
        return true;
    }

    void rotateFileIfNeeded() {
        ofstream fout(currentFile, ios::binary | ios::app);
        if (!fout) {
            cerr << "Error opening file for writing!" << endl;
            return;
        }
        for (const auto& data : buffer) {
            fout.write(reinterpret_cast<const char*>(&data), sizeof(TelemetryData));
        }
        fout.close();
        buffer.clear();
        fileCounter++;
        char filename[50];
        sprintf(filename, "telemetry_%03d.bin", fileCounter);
        currentFile = filename;
        cout << "Rotated to new file: " << currentFile << endl;
    }

    vector<TelemetryData> readLogFile(const string& filename) {
        vector<TelemetryData> data;
        ifstream fin(filename, ios::binary);
        if (!fin) {
            cerr << "Error reading file: " << filename << endl;
            return data;
        }
        TelemetryData td;
        while (fin.read(reinterpret_cast<char*>(&td), sizeof(TelemetryData))) {
            data.push_back(td);
        }
        fin.close();
        return data;
    }

    void printLogSummary() {
        cout << "\nTelemetry Summary:\n";
        cout << "Current file: " << currentFile << endl;
        cout << "Records in buffer: " << buffer.size() << endl;
    }

    void printLastNRecords(int n) {
        int start = max(0, (int)buffer.size() - n);
        cout << "\nLast " << min(n, (int)buffer.size()) << " telemetry records:\n";
        cout << "Time\tAlt\tSpeed\tHeading\tFuel\n";
        for (size_t i = start; i < buffer.size(); ++i) {
            const auto& d = buffer[i];
            cout << fixed << setprecision(1)
                 << d.time << "\t" << d.altitude << "\t" << d.speed
                 << "\t" << d.heading << "\t" << d.fuel << endl;
        }
    }

    void saveBufferToFile() {
        ofstream fout(currentFile, ios::binary);
        if (!fout) {
            cerr << "Error opening file for writing!" << endl;
            return;
        }
        for (const auto& data : buffer) {
            fout.write(reinterpret_cast<const char*>(&data), sizeof(TelemetryData));
        }
        fout.close();
        cout << "Buffer saved to " << currentFile << endl;
    }
};

void createSampleBinaryFile() {
    // Create binary file with sample data
    ofstream fout("telemetry_001.bin", ios::binary);
    if (!fout) {
        cerr << "Cannot create sample file!" << endl;
        return;
    }

    TelemetryData sampleData[] = {
        {0.0, 100.0, 25.0, 45.0, 80.0},
        {1.0, 105.0, 27.0, 46.0, 79.5},
        {2.0, 110.0, 29.0, 47.0, 79.0}
    };

    for (const auto& data : sampleData) {
        fout.write(reinterpret_cast<const char*>(&data), sizeof(TelemetryData));
    }
    fout.close();
    cout << "Sample binary file 'telemetry_001.bin' created with 3 records.\n";
}

int main() {
    // Create sample binary file first
    createSampleBinaryFile();

    TelemetryLogger logger;

    // Load and display sample data
    cout << "\nReading sample telemetry data...\n";
    vector<TelemetryData> loaded = logger.readLogFile("telemetry_001.bin");

    cout << "\nLoaded telemetry records:\n";
    cout << "Time\tAlt\tSpeed\tHeading\tFuel\n";
    for (const auto& d : loaded) {
        cout << fixed << setprecision(1)
             << d.time << "\t" << d.altitude << "\t" << d.speed
             << "\t" << d.heading << "\t" << d.fuel << endl;
    }

    // Add the loaded data to buffer
    for (const auto& d : loaded) {
        logger.logData(d.time, d.altitude, d.speed, d.heading, d.fuel);
    }

    // Add more data
    cout << "\nAdding more telemetry data...\n";
    logger.logData(3.0, 115.0, 31.0, 48.0, 78.5);
    logger.logData(4.0, 120.0, 33.0, 49.0, 78.0);

    // Show summary
    logger.printLogSummary();
    logger.printLastNRecords(5);

    // Save buffer to file
    logger.saveBufferToFile();

    return 0;
}
