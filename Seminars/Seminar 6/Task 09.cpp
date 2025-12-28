#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <algorithm>

using namespace std;

// Aircraft class for loading parameters
class Aircraft {
private:
    double mass;
    double wing_area;
    double Cx, Cy;
    double max_thrust;
    double specific_consumption;

public:
    Aircraft() : mass(0), wing_area(0), Cx(0), Cy(0),
                 max_thrust(0), specific_consumption(0) {}

    bool loadFromFile(const string& filename) {
        ifstream fin(filename);
        if (!fin.is_open()) {
            cerr << "Error: Cannot open aircraft parameters file: " << filename << endl;
            return false;
        }

        string line;
        while (getline(fin, line)) {
            if (line.empty()) continue;

            size_t eq_pos = line.find('=');
            if (eq_pos == string::npos) continue;

            string key = line.substr(0, eq_pos);
            string value_str = line.substr(eq_pos + 1);

            // Remove whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);

            try {
                double value = stod(value_str);

                if (key == "mass") mass = value;
                else if (key == "wing_area") wing_area = value;
                else if (key == "Cx") Cx = value;
                else if (key == "Cy") Cy = value;
                else if (key == "max_thrust") max_thrust = value;
                else if (key == "specific_consumption") specific_consumption = value;
            } catch (...) {
                cerr << "Warning: Invalid value in line: " << line << endl;
            }
        }

        fin.close();
        return true;
    }

    // Getters
    double getMass() const { return mass; }
    double getWingArea() const { return wing_area; }
    double getCx() const { return Cx; }
    double getCy() const { return Cy; }
    double getMaxThrust() const { return max_thrust; }
    double getSpecificConsumption() const { return specific_consumption; }

    void printParameters() const {
        cout << "\n=== AIRCRAFT PARAMETERS ===" << endl;
        cout << left << setw(25) << "Mass:" << mass << " kg" << endl;
        cout << left << setw(25) << "Wing area:" << wing_area << " m²" << endl;
        cout << left << setw(25) << "Drag coefficient (Cx):" << Cx << endl;
        cout << left << setw(25) << "Lift coefficient (Cy):" << Cy << endl;
        cout << left << setw(25) << "Max thrust:" << max_thrust << " N" << endl;
        cout << left << setw(25) << "Specific consumption:" << specific_consumption << endl;
    }
};

// Environment class for atmosphere table
class Environment {
private:
    struct AtmospherePoint {
        double altitude;
        double density;
        double pressure;
    };

    vector<AtmospherePoint> atmosphereTable;

public:
    bool loadAtmosphereTable(const string& filename) {
        ifstream fin(filename);
        if (!fin.is_open()) {
            cerr << "Error: Cannot open atmosphere table: " << filename << endl;
            return false;
        }

        atmosphereTable.clear();

        string line;
        getline(fin, line); // Skip header

        while (getline(fin, line)) {
            if (line.empty()) continue;

            stringstream ss(line);
            string token;
            vector<string> tokens;

            while (getline(ss, token, ',')) {
                tokens.push_back(token);
            }

            if (tokens.size() >= 3) {
                try {
                    AtmospherePoint point;
                    point.altitude = stod(tokens[0]);
                    point.density = stod(tokens[1]);
                    point.pressure = stod(tokens[2]);
                    atmosphereTable.push_back(point);
                } catch (...) {
                    cerr << "Warning: Invalid data: " << line << endl;
                }
            }
        }

        fin.close();
        return true;
    }

    double getDensity(double altitude) const {
        if (atmosphereTable.empty()) {
            cerr << "Warning: Atmosphere table is empty, using default density." << endl;
            return 1.225; // Sea level density
        }

        // Check boundaries
        if (altitude <= atmosphereTable.front().altitude) {
            return atmosphereTable.front().density;
        }

        if (altitude >= atmosphereTable.back().altitude) {
            return atmosphereTable.back().density;
        }

        // Find interval for interpolation
        for (size_t i = 0; i < atmosphereTable.size() - 1; i++) {
            if (altitude >= atmosphereTable[i].altitude &&
                altitude <= atmosphereTable[i+1].altitude) {

                double alt1 = atmosphereTable[i].altitude;
                double alt2 = atmosphereTable[i+1].altitude;
                double dens1 = atmosphereTable[i].density;
                double dens2 = atmosphereTable[i+1].density;

                // Linear interpolation
                double t = (altitude - alt1) / (alt2 - alt1);
                return dens1 + t * (dens2 - dens1);
            }
        }

        return atmosphereTable.back().density;
    }

    double getPressure(double altitude) const {
        if (atmosphereTable.empty()) {
            return 101325.0; // Sea level pressure
        }

        if (altitude <= atmosphereTable.front().altitude) {
            return atmosphereTable.front().pressure;
        }

        if (altitude >= atmosphereTable.back().altitude) {
            return atmosphereTable.back().pressure;
        }

        for (size_t i = 0; i < atmosphereTable.size() - 1; i++) {
            if (altitude >= atmosphereTable[i].altitude &&
                altitude <= atmosphereTable[i+1].altitude) {

                double alt1 = atmosphereTable[i].altitude;
                double alt2 = atmosphereTable[i+1].altitude;
                double press1 = atmosphereTable[i].pressure;
                double press2 = atmosphereTable[i+1].pressure;

                double t = (altitude - alt1) / (alt2 - alt1);
                return press1 + t * (press2 - press1);
            }
        }

        return atmosphereTable.back().pressure;
    }

    void printAtmosphereTable() const {
        if (atmosphereTable.empty()) {
            cout << "Atmosphere table is empty." << endl;
            return;
        }

        cout << "\n=== ATMOSPHERE TABLE ===" << endl;
        cout << "Altitude (m)\tDensity (kg/m³)\tPressure (Pa)" << endl;
        cout << "------------------------------------------------" << endl;

        for (const auto& point : atmosphereTable) {
            cout << point.altitude << "\t\t"
                 << fixed << setprecision(3) << point.density << "\t\t"
                 << setprecision(0) << point.pressure << endl;
        }
    }

    size_t getTableSize() const {
        return atmosphereTable.size();
    }
};

void createAircraftParamsFile() {
    ofstream fout("aircraft_params.txt");
    if (!fout) {
        cerr << "Error creating aircraft_params.txt" << endl;
        return;
    }

    fout << "mass=18400\n";
    fout << "wing_area=62\n";
    fout << "Cx=0.02\n";
    fout << "Cy=3.45\n";
    fout << "max_thrust=14500\n";
    fout << "specific_consumption=0.75\n";

    fout.close();
    cout << "Created: aircraft_params.txt" << endl;
}

void createAtmosphereFile() {
    ofstream fout("atmosphere.csv");
    if (!fout) {
        cerr << "Error creating atmosphere.csv" << endl;
        return;
    }

    fout << "altitude,density,pressure\n";
    fout << "0,1.225,101325\n";
    fout << "1000,1.112,89880\n";
    fout << "2000,1.007,79500\n";

    fout.close();
    cout << "Created: atmosphere.csv" << endl;
}

void demonstrateInterpolation(const Environment& env) {
    cout << "\n=== DENSITY INTERPOLATION DEMO ===" << endl;

    // Test at specific altitudes
    vector<double> test_altitudes = {0, 500, 1000, 1500, 2000, 2500};

    cout << "Altitude\tDensity\t\tPressure" << endl;
    cout << "(m)\t\t(kg/m³)\t\t(Pa)" << endl;
    cout << "----------------------------------------" << endl;

    for (double alt : test_altitudes) {
        double density = env.getDensity(alt);
        double pressure = env.getPressure(alt);

        cout << alt << "\t\t"
             << fixed << setprecision(3) << density << "\t\t"
             << setprecision(0) << pressure << endl;
    }

    cout << "\nEnter an altitude for interpolation (m): ";
    double user_alt;
    if (cin >> user_alt) {
        double density = env.getDensity(user_alt);
        double pressure = env.getPressure(user_alt);

        cout << "\nAt altitude " << user_alt << " m:" << endl;
        cout << "  Density: " << fixed << setprecision(4) << density << " kg/m³" << endl;
        cout << "  Pressure: " << fixed << setprecision(0) << pressure << " Pa" << endl;
    }
}

int main() {
    cout << "=== AIRCRAFT AND ENVIRONMENT PARAMETERS LOADER ===" << endl;

    // Step 1: Create files with data from condition
    cout << "\nCreating files with data from condition..." << endl;
    createAircraftParamsFile();
    createAtmosphereFile();

    // Step 2: Load aircraft parameters
    cout << "\nLoading aircraft parameters..." << endl;
    Aircraft aircraft;
    if (aircraft.loadFromFile("aircraft_params.txt")) {
        aircraft.printParameters();
    } else {
        cout << "Failed to load aircraft parameters." << endl;
    }

    // Step 3: Load atmosphere table
    cout << "\nLoading atmosphere table..." << endl;
    Environment environment;
    if (environment.loadAtmosphereTable("atmosphere.csv")) {
        cout << "Loaded " << environment.getTableSize() << " atmosphere points." << endl;
        environment.printAtmosphereTable();

        // Demonstrate interpolation
        demonstrateInterpolation(environment);
    } else {
        cout << "Failed to load atmosphere table." << endl;
    }

    cout << "\n=== TASK COMPLETED ===" << endl;
    cout << "This program demonstrates:" << endl;
    cout << "1. Loading aircraft parameters from aircraft_params.txt" << endl;
    cout << "2. Loading atmosphere data from atmosphere.csv" << endl;
    cout << "3. Linear interpolation of density and pressure at any altitude" << endl;

    return 0;
}
