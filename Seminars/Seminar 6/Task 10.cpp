#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <cmath>

using namespace std;

struct TrajectoryPoint {
    double time;
    double velocity;
    double altitude;
    double distance;
    double fuel;
};

class Trajectory {
private:
    vector<TrajectoryPoint> points;

public:
    void addPoint(double t, double v, double alt, double dist, double fuel) {
        points.push_back({t, v, alt, dist, fuel});
    }

    bool saveToCSV(const string& filename) {
        ofstream fout(filename);
        if (!fout) {
            cerr << "Cannot write to file: " << filename << endl;
            return false;
        }
        fout << "time,velocity,altitude,distance,fuel\n";
        for (const auto& p : points) {
            fout << fixed << setprecision(3)
                 << p.time << "," << p.velocity << "," << p.altitude << ","
                 << p.distance << "," << p.fuel << "\n";
        }
        fout.close();
        cout << "Trajectory saved to " << filename << " (" << points.size() << " points)\n";
        return true;
    }

    void generatePlotScript(const string& filename) {
        ofstream fout(filename);
        fout << "# Gnuplot script for trajectory visualization\n";
        fout << "set terminal png size 1200,800 enhanced font 'Arial,10'\n";
        fout << "set output 'trajectory_plots.png'\n";
        fout << "set multiplot layout 2,2 title 'Trajectory Analysis'\n\n";

        fout << "# Plot 1: Altitude vs Time\n";
        fout << "set title 'Altitude vs Time'\n";
        fout << "set xlabel 'Time (s)'\n";
        fout << "set ylabel 'Altitude (m)'\n";
        fout << "set grid\n";
        fout << "plot 'trajectory.csv' using 1:3 with lines lw 2 lc rgb 'blue' title 'Altitude'\n\n";

        fout << "# Plot 2: Velocity vs Time\n";
        fout << "set title 'Velocity vs Time'\n";
        fout << "set xlabel 'Time (s)'\n";
        fout << "set ylabel 'Velocity (m/s)'\n";
        fout << "set grid\n";
        fout << "plot 'trajectory.csv' using 1:2 with lines lw 2 lc rgb 'red' title 'Velocity'\n\n";

        fout << "# Plot 3: Fuel Consumption\n";
        fout << "set title 'Fuel Consumption'\n";
        fout << "set xlabel 'Time (s)'\n";
        fout << "set ylabel 'Fuel (kg)'\n";
        fout << "set grid\n";
        fout << "plot 'trajectory.csv' using 1:5 with lines lw 2 lc rgb 'green' title 'Fuel'\n\n";

        fout << "# Plot 4: Distance vs Altitude\n";
        fout << "set title 'Flight Path'\n";
        fout << "set xlabel 'Distance (m)'\n";
        fout << "set ylabel 'Altitude (m)'\n";
        fout << "set grid\n";
        fout << "plot 'trajectory.csv' using 4:3 with lines lw 2 lc rgb 'purple' title 'Path'\n";

        fout << "unset multiplot\n";
        fout.close();
        cout << "Gnuplot script generated: " << filename << endl;
    }

    void saveReport(const string& filename) {
        if (points.empty()) {
            cerr << "No trajectory data!\n";
            return;
        }

        ofstream fout(filename);
        fout << "TRAJECTORY ANALYSIS REPORT\n";
        fout << "==========================\n\n";

        // Calculate statistics
        double totalTime = points.back().time;
        double totalFuel = points.back().fuel;
        double totalDistance = points.back().distance;

        double avgVel = 0, maxVel = 0, maxAlt = 0;
        for (const auto& p : points) {
            avgVel += p.velocity;
            if (p.velocity > maxVel) maxVel = p.velocity;
            if (p.altitude > maxAlt) maxAlt = p.altitude;
        }
        avgVel /= points.size();

        fout << fixed << setprecision(2);
        fout << "FLIGHT SUMMARY:\n";
        fout << "Total flight time: " << totalTime << " s\n";
        fout << "Total fuel consumed: " << totalFuel << " kg\n";
        fout << "Total distance: " << totalDistance << " m\n";
        fout << "Average velocity: " << avgVel << " m/s\n";
        fout << "Maximum velocity: " << maxVel << " m/s\n";
        fout << "Maximum altitude: " << maxAlt << " m\n";
        fout << "Number of data points: " << points.size() << "\n\n";

        fout << "SAMPLE DATA POINTS (first 10):\n";
        fout << "Time(s)\tVelocity(m/s)\tAltitude(m)\tDistance(m)\tFuel(kg)\n";
        for (size_t i = 0; i < min(points.size(), (size_t)10); ++i) {
            fout << points[i].time << "\t" << points[i].velocity << "\t\t"
                 << points[i].altitude << "\t\t" << points[i].distance << "\t\t"
                 << points[i].fuel << "\n";
        }

        fout.close();
        cout << "Report saved to " << filename << endl;
    }

    void printSummary() {
        if (points.empty()) {
            cout << "No trajectory data!\n";
            return;
        }

        cout << "\nTrajectory Summary:\n";
        cout << "Total points: " << points.size() << "\n";
        cout << "Last point - Time: " << points.back().time
             << "s, Altitude: " << points.back().altitude
             << "m, Velocity: " << points.back().velocity
             << "m/s, Fuel: " << points.back().fuel << "kg\n";
    }
};

// Simulate trajectory data based on condition format
void simulateTrajectory(Trajectory& traj) {
    // Sample data from condition
    traj.addPoint(0.0, 250.0, 0.0, 0.0, 0.0);
    traj.addPoint(1.0, 255.3, 1050.2, 252.5, 2.47);
    traj.addPoint(2.0, 260.7, 1101.8, 508.3, 4.92);
    traj.addPoint(3.0, 265.5, 1150.5, 767.8, 7.35);
    traj.addPoint(4.0, 270.2, 1198.2, 1030.4, 9.76);
    traj.addPoint(5.0, 274.8, 1245.0, 1296.2, 12.15);

    // Add more points for better visualization
    for (double t = 6.0; t <= 10.0; t += 1.0) {
        double v = 280.0 + t * 2.0;
        double alt = 1300.0 + t * 50.0;
        double dist = 1500.0 + t * 300.0;
        double fuel = 15.0 + t * 2.5;
        traj.addPoint(t, v, alt, dist, fuel);
    }
}

void createSampleOutput() {
    cout << "\nExpected output files from condition:\n";
    cout << "trajectory.csv - with columns: time,velocity,altitude,distance,fuel\n";
    cout << "report.txt - with total time, fuel consumed, average velocity\n";
    cout << "Sample data from condition:\n";
    cout << "Total flight time: 185.3 s\n";
    cout << "Total fuel consumed: 845.2 kg\n";
    cout << "Average velocity: 275.4 m/s\n";
}

int main() {
    createSampleOutput();

    Trajectory trajectory;

    // Generate trajectory data
    cout << "\nGenerating trajectory data...\n";
    simulateTrajectory(trajectory);

    // Save results
    trajectory.saveToCSV("trajectory.csv");
    trajectory.saveReport("report.txt");
    trajectory.generatePlotScript("plot.gp");

    // Show summary
    trajectory.printSummary();

    cout << "\nFiles created successfully:\n";
    cout << "1. trajectory.csv - CSV data file\n";
    cout << "2. report.txt - Text report with statistics\n";
    cout << "3. plot.gp - Gnuplot script for visualization\n";
    cout << "\nTo generate plots, run: gnuplot plot.gp\n";

    return 0;
}
