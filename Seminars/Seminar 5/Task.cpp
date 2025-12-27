#include <iostream>
#include <cmath>
#include <iomanip>
#include <vector>
using namespace std;

// Base class Aircraft
class Aircraft {
protected:
    double mass;    // kg
    double x, y, z; // coordinates in meters
    double vx, vy, vz; // velocity components in m/s

public:
    // Constructor
    Aircraft(double m, double x0, double y0, double z0,
             double vx0 = 0, double vy0 = 0, double vz0 = 0)
        : mass(m), x(x0), y(y0), z(z0), vx(vx0), vy(vy0), vz(vz0) {}

    // Update position based on current velocity
    virtual void updatePosition(double dt) {
        x += vx * dt;
        y += vy * dt;
        z += vz * dt;
    }

    // Virtual method to display status
    virtual void printStatus() const {
        cout << fixed << setprecision(2);
        cout << "Aircraft Status:" << endl;
        cout << "  Mass: " << mass << " kg" << endl;
        cout << "  Position: (" << x << ", " << y << ", " << z << ") m" << endl;
        cout << "  Velocity: (" << vx << ", " << vy << ", " << vz << ") m/s" << endl;
    }

    // Virtual destructor for proper cleanup
    virtual ~Aircraft() {
        cout << "Aircraft object destroyed." << endl;
    }

    // Getter methods
    double getMass() const { return mass; }
    double getX() const { return x; }
    double getY() const { return y; }
    double getZ() const { return z; }
    double getVx() const { return vx; }
    double getVy() const { return vy; }
    double getVz() const { return vz; }

    // Setter methods
    void setVelocity(double new_vx, double new_vy, double new_vz) {
        vx = new_vx;
        vy = new_vy;
        vz = new_vz;
    }

    void setPosition(double new_x, double new_y, double new_z) {
        x = new_x;
        y = new_y;
        z = new_z;
    }
};

// Derived class JetAircraft
class JetAircraft : public Aircraft {
private:
    double thrust;          // engine thrust in Newtons
    double Cd;              // drag coefficient
    double wingArea;        // wing area in m²
    double airDensity;      // air density in kg/m³
    double fuel;            // remaining fuel in kg
    double fuelConsumption; // fuel consumption rate in kg/s
    const double g = 9.81;  // gravitational acceleration in m/s²

public:
    // Constructor
    JetAircraft(double m, double x0, double y0, double z0,
                double t, double cd, double area, double density,
                double initialFuel, double fuelRate,
                double vx0 = 0, double vy0 = 0, double vz0 = 0)
        : Aircraft(m, x0, y0, z0, vx0, vy0, vz0),
          thrust(t), Cd(cd), wingArea(area), airDensity(density),
          fuel(initialFuel), fuelConsumption(fuelRate) {}

    // Calculate aerodynamic drag force
    double computeDrag() const {
        // Calculate total speed
        double speed = sqrt(vx*vx + vy*vy + vz*vz);
        // Drag formula: F_drag = 0.5 * ρ * v² * Cd * A
        return 0.5 * airDensity * speed * speed * Cd * wingArea;
    }

    // Calculate total acceleration including thrust, drag, and gravity
    void computeAcceleration(double& ax, double& ay, double& az) {
        // Calculate drag force
        double drag = computeDrag();

        // Calculate direction of motion for drag application
        double speed = sqrt(vx*vx + vy*vy + vz*vz);

        if (speed > 0) {
            // Drag acts opposite to velocity direction
            double drag_x = -drag * (vx / speed);
            double drag_y = -drag * (vy / speed);
            double drag_z = -drag * (vz / speed);

            // Thrust acts in forward direction (along x-axis in this simplified model)
            double thrust_x = thrust;
            double thrust_y = 0;
            double thrust_z = 0;

            // Net forces
            double Fx = thrust_x + drag_x;
            double Fy = thrust_y + drag_y;
            double Fz = thrust_z + drag_z - mass * g; // Gravity acts downward

            // Accelerations (Newton's second law: F = ma)
            ax = Fx / mass;
            ay = Fy / mass;
            az = Fz / mass;
        } else {
            // If aircraft is stationary
            ax = thrust / mass;
            ay = 0;
            az = -g; // Only gravity
        }
    }

    // Simulate one time step
    void simulateStep(double dt) {
        // Check if we have fuel
        if (fuel <= 0) {
            thrust = 0; // Engine off if no fuel
        } else {
            // Consume fuel
            double fuelUsed = fuelConsumption * dt;
            if (fuelUsed > fuel) {
                fuelUsed = fuel;
            }
            fuel -= fuelUsed;

            // Reduce thrust if fuel is running low (optional)
            if (fuel < 10) {
                thrust *= 0.5; // Reduce thrust when fuel is very low
            }
        }

        // Calculate acceleration
        double ax, ay, az;
        computeAcceleration(ax, ay, az);

        // Update velocity
        vx += ax * dt;
        vy += ay * dt;
        vz += az * dt;

        // Update position
        updatePosition(dt);
    }

    // Override printStatus to include jet-specific information
    void printStatus() const override {
        Aircraft::printStatus();
        cout << fixed << setprecision(2);
        cout << "Jet Aircraft Specific:" << endl;
        cout << "  Thrust: " << thrust << " N" << endl;
        cout << "  Drag Coefficient: " << Cd << endl;
        cout << "  Wing Area: " << wingArea << " m²" << endl;
        cout << "  Air Density: " << airDensity << " kg/m³" << endl;
        cout << "  Remaining Fuel: " << fuel << " kg" << endl;
        cout << "  Fuel Consumption Rate: " << fuelConsumption << " kg/s" << endl;

        // Calculate and display additional metrics
        double speed = sqrt(vx*vx + vy*vy + vz*vz);
        double drag = computeDrag();
        cout << "  Current Speed: " << speed << " m/s" << endl;
        cout << "  Current Drag Force: " << drag << " N" << endl;

        // Check flight phase
        if (vz > 0) {
            cout << "  Flight Phase: CLIMBING" << endl;
        } else if (vz < 0) {
            cout << "  Flight Phase: DESCENDING" << endl;
        } else {
            cout << "  Flight Phase: LEVEL FLIGHT" << endl;
        }

        if (fuel <= 0) {
            cout << "  WARNING: OUT OF FUEL!" << endl;
        }
        cout << endl;
    }

    // Check if aircraft is still flying (above ground)
    bool isFlying() const {
        return z > 0;
    }

    // Check if aircraft has fuel
    bool hasFuel() const {
        return fuel > 0;
    }

    // Get remaining fuel
    double getFuel() const {
        return fuel;
    }

    // Get thrust
    double getThrust() const {
        return thrust;
    }
};

// Function to get user input with validation
double getInput(const string& prompt, double minVal = -1e9, double maxVal = 1e9) {
    double value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail() || value < minVal || value > maxVal) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input. Please enter a value between "
                 << minVal << " and " << maxVal << "." << endl;
        } else {
            cin.ignore(1000, '\n');
            return value;
        }
    }
}

int main() {
    cout << "=========================================" << endl;
    cout << "   JET AIRCRAFT FLIGHT SIMULATION" << endl;
    cout << "=========================================" << endl << endl;

    // Get aircraft parameters from user
    cout << "ENTER AIRCRAFT PARAMETERS:" << endl;
    cout << "----------------------------" << endl;

    double mass = getInput("Enter aircraft mass (kg): ", 1000, 500000);
    double thrust = getInput("Enter engine thrust (N): ", 1000, 1000000);
    double cd = getInput("Enter drag coefficient: ", 0.01, 1.0);
    double wingArea = getInput("Enter wing area (m²): ", 10, 500);
    double airDensity = getInput("Enter air density (kg/m³, sea level = 1.225): ", 0.1, 2.0);
    double initialFuel = getInput("Enter initial fuel (kg): ", 100, 100000);
    double fuelRate = getInput("Enter fuel consumption rate (kg/s): ", 0.1, 100);

    cout << endl << "ENTER INITIAL CONDITIONS:" << endl;
    cout << "----------------------------" << endl;
    double x0 = getInput("Enter initial x position (m): ", -10000, 10000);
    double y0 = getInput("Enter initial y position (m): ", -10000, 10000);
    double z0 = getInput("Enter initial altitude (m): ", 0, 20000);
    double vx0 = getInput("Enter initial x velocity (m/s): ", -500, 500);
    double vy0 = getInput("Enter initial y velocity (m/s): ", -500, 500);
    double vz0 = getInput("Enter initial z velocity (m/s, positive = up): ", -100, 100);

    cout << endl << "ENTER SIMULATION PARAMETERS:" << endl;
    cout << "-------------------------------" << endl;
    double dt = getInput("Enter time step (s): ", 0.01, 10);
    double totalTime = getInput("Enter total simulation time (s): ", 1, 3600);
    int outputInterval = getInput("Enter output interval (number of steps between status updates): ", 1, 1000);

    // Create the jet aircraft
    JetAircraft jet(mass, x0, y0, z0, thrust, cd, wingArea,
                    airDensity, initialFuel, fuelRate, vx0, vy0, vz0);

    cout << endl << "=========================================" << endl;
    cout << "   SIMULATION STARTING" << endl;
    cout << "=========================================" << endl << endl;

    // Display initial status
    cout << "INITIAL STATUS:" << endl;
    cout << "---------------" << endl;
    jet.printStatus();

    // Simulation loop
    double currentTime = 0;
    int step = 0;
    vector<double> altitudeHistory;
    vector<double> fuelHistory;
    vector<double> timeHistory;

    // Store initial values
    altitudeHistory.push_back(z0);
    fuelHistory.push_back(initialFuel);
    timeHistory.push_back(0);

    cout << endl << "SIMULATION PROGRESS:" << endl;
    cout << "-------------------" << endl;

    while (currentTime < totalTime && jet.isFlying()) {
        // Perform simulation step
        jet.simulateStep(dt);
        currentTime += dt;
        step++;

        // Store data for analysis
        altitudeHistory.push_back(jet.getZ());
        fuelHistory.push_back(jet.getFuel());
        timeHistory.push_back(currentTime);

        // Display status at specified intervals
        if (step % outputInterval == 0) {
            cout << endl << "Time: " << fixed << setprecision(1) << currentTime << " s" << endl;
            cout << "Altitude: " << fixed << setprecision(1) << jet.getZ() << " m" << endl;
            cout << "Fuel remaining: " << fixed << setprecision(1) << jet.getFuel() << " kg" << endl;

            if (!jet.hasFuel()) {
                cout << "WARNING: Aircraft is out of fuel!" << endl;
            }
        }

        // Check for ground impact
        if (!jet.isFlying()) {
            cout << endl << "IMPACT! Aircraft has reached the ground at time "
                 << fixed << setprecision(1) << currentTime << " s" << endl;
            break;
        }
    }

    // Display final results
    cout << endl << "=========================================" << endl;
    cout << "   SIMULATION COMPLETE" << endl;
    cout << "=========================================" << endl << endl;

    cout << "FINAL STATUS:" << endl;
    cout << "-------------" << endl;
    jet.printStatus();

    // Calculate and display statistics
    cout << "SIMULATION STATISTICS:" << endl;
    cout << "---------------------" << endl;
    cout << fixed << setprecision(2);
    cout << "Total simulation time: " << currentTime << " s" << endl;
    cout << "Number of simulation steps: " << step << endl;
    cout << "Final altitude: " << jet.getZ() << " m" << endl;
    cout << "Final fuel: " << jet.getFuel() << " kg" << endl;

    // Find maximum altitude
    double maxAltitude = altitudeHistory[0];
    double timeAtMaxAltitude = 0;
    for (size_t i = 0; i < altitudeHistory.size(); i++) {
        if (altitudeHistory[i] > maxAltitude) {
            maxAltitude = altitudeHistory[i];
            timeAtMaxAltitude = timeHistory[i];
        }
    }

    cout << "Maximum altitude reached: " << maxAltitude << " m at time "
         << timeAtMaxAltitude << " s" << endl;

    // Calculate average fuel consumption rate
    double totalFuelConsumed = initialFuel - jet.getFuel();
    double avgFuelRate = (currentTime > 0) ? totalFuelConsumed / currentTime : 0;
    cout << "Total fuel consumed: " << totalFuelConsumed << " kg" << endl;
    cout << "Average fuel consumption rate: " << avgFuelRate << " kg/s" << endl;

    // Determine flight outcome
    cout << endl << "FLIGHT OUTCOME: ";
    if (!jet.isFlying()) {
        cout << "CRASH LANDING" << endl;
    } else if (!jet.hasFuel()) {
        cout << "OUT OF FUEL IN FLIGHT" << endl;
    } else if (currentTime >= totalTime) {
        cout << "SIMULATION TIME COMPLETE" << endl;
    } else {
        cout << "UNKNOWN" << endl;
    }

    // Ask if user wants to see detailed trajectory
    cout << endl << "Would you like to see the detailed altitude history? (y/n): ";
    char response;
    cin >> response;

    if (response == 'y' || response == 'Y') {
        cout << endl << "TIME (s) | ALTITUDE (m) | FUEL (kg)" << endl;
        cout << "-----------------------------------" << endl;
        for (size_t i = 0; i < timeHistory.size(); i += max(1, (int)(timeHistory.size()/20))) {
            cout << fixed << setprecision(1) << setw(8) << timeHistory[i] << " | "
                 << setw(12) << altitudeHistory[i] << " | "
                 << setw(9) << fuelHistory[i] << endl;
        }
    }

    cout << endl << "Thank you for using the Jet Aircraft Flight Simulator!" << endl;

    return 0;
}
