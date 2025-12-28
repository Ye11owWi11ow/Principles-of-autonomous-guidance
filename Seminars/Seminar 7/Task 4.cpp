#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <algorithm>

using namespace std;

class Navigator {
private:
    vector<double> t, x, y;
public:
    // Create navigation data file with values from condition
    void createDataFile() {
        ofstream fout("navigation.csv");
        if (!fout) {
            cerr << "ERROR: Cannot create navigation.csv" << endl;
            return;
        }

        fout << "t,x,y\n";
        // Data from condition:
        vector<vector<double>> navData = {
            {0, 0, 0},
            {1, 5, 1},
            {2, 9, 3},
            {3, 12, 6},
            {4, 14, 10}
        };

        for (const auto& row : navData) {
            fout << row[0] << "," << row[1] << "," << row[2] << "\n";
        }
        fout.close();

        cout << "=== Created navigation.csv ===" << endl;
        system("cat navigation.csv 2>/dev/null || type navigation.csv 2>/dev/null");
        cout << "\nCreated navigation.csv with " << navData.size() << " points" << endl;
    }

    void loadFromFile(const string& filename) {
        cout << "\n=== Loading navigation data ===" << endl;
        ifstream fin(filename);
        if (!fin) {
            cerr << "ERROR: Cannot open " << filename << endl;
            return;
        }

        string line;
        getline(fin, line); // header
        double time, coordX, coordY;
        char comma;
        int count = 0;

        while (fin >> time >> comma >> coordX >> comma >> coordY) {
            t.push_back(time);
            x.push_back(coordX);
            y.push_back(coordY);
            count++;
        }
        fin.close();

        cout << "Loaded " << count << " navigation points" << endl;
    }

    void computeVelocityAndPlot() {
        if (t.size() < 2) {
            cerr << "ERROR: Not enough data points for velocity calculation" << endl;
            return;
        }

        // Compute speed magnitude using lambda
        vector<double> v;
        auto computeSpeed = [](double x1, double y1, double x2, double y2, double dt) {
            double dx = x2 - x1;
            double dy = y2 - y1;
            return sqrt(dx*dx + dy*dy) / dt;
        };

        for (size_t i = 0; i < t.size() - 1; ++i) {
            double dt = t[i+1] - t[i];
            if (dt > 0) {
                v.push_back(computeSpeed(x[i], y[i], x[i+1], y[i+1], dt));
            } else {
                v.push_back(0);
            }
        }
        v.push_back(v.back()); // last value

        // Display results
        cout << "\n=== Navigation Analysis ===" << endl;
        cout << "Time\tX\tY\tSpeed" << endl;
        cout << "----\t---\t---\t-----" << endl;
        for (size_t i = 0; i < t.size(); ++i) {
            printf("%.1f\t%.1f\t%.1f\t%.2f\n", t[i], x[i], y[i], v[i]);
        }

        // Statistics
        double max_v = *max_element(v.begin(), v.end());
        double min_v = *min_element(v.begin(), v.end());
        double avg_v = 0;
        for (double speed : v) avg_v += speed;
        avg_v /= v.size();

        cout << "\nSpeed Statistics:" << endl;
        cout << "Maximum speed: " << max_v << " m/s" << endl;
        cout << "Minimum speed: " << min_v << " m/s" << endl;
        cout << "Average speed: " << avg_v << " m/s" << endl;

        // Save velocity data as required
        string outfile = "velocity.csv";
        ofstream fout(outfile);
        fout << "t,v\n";
        for (size_t i = 0; i < t.size(); ++i) {
            fout << t[i] << "," << v[i] << "\n";
        }
        fout.close();
        cout << "\nVelocity data saved to " << outfile << " as required" << endl;

        // Create data files for GNUPlot
        ofstream traj("trajectory_data.txt");
        ofstream speed("speed_data.txt");

        traj << "# Trajectory data\n";
        speed << "# Speed data\n";

        for (size_t i = 0; i < t.size(); ++i) {
            traj << t[i] << " " << x[i] << " " << y[i] << "\n";
            speed << t[i] << " " << v[i] << "\n";
        }

        traj.close();
        speed.close();

        // Create GNUPlot script
        ofstream script("plot_navigation.gp");
        script << "# Navigation data analysis\n";
        script << "set terminal qt size 1000,700\n";
        script << "set multiplot layout 2,2\n\n";

        // График 1: Траектория в пространстве
        script << "# Plot 1: 2D Trajectory\n";
        script << "set title '2D Trajectory'\n";
        script << "set xlabel 'X position (m)'\n";
        script << "set ylabel 'Y position (m)'\n";
        script << "set grid\n";
        script << "set size square\n";
        script << "plot 'trajectory_data.txt' using 2:3 with linespoints pt 7 ps 1.5 lc rgb 'blue' title 'Path'\n\n";

        // График 2: X и Y отдельно
        script << "# Plot 2: Position vs Time\n";
        script << "set title 'Position Components vs Time'\n";
        script << "set xlabel 'Time (s)'\n";
        script << "set ylabel 'Position (m)'\n";
        script << "set grid\n";
        script << "plot 'trajectory_data.txt' using 1:2 with linespoints title 'X(t)', \\\n";
        script << "     '' using 1:3 with linespoints title 'Y(t)'\n\n";

        // График 3: Скорость
        script << "# Plot 3: Speed vs Time\n";
        script << "set title 'Speed Magnitude vs Time'\n";
        script << "set xlabel 'Time (s)'\n";
        script << "set ylabel 'Speed (m/s)'\n";
        script << "set grid\n";
        script << "set yrange [0:" << (max_v * 1.2) << "]\n";
        script << "plot 'speed_data.txt' using 1:2 with linespoints pt 5 ps 1.5 lc rgb 'red' lw 2 title 'v(t)'\n\n";

        // График 4: 3D траектория
        script << "# Plot 4: 3D View (time as Z)\n";
        script << "set title '3D Trajectory (Time as Z-axis)'\n";
        script << "set xlabel 'X (m)'\n";
        script << "set ylabel 'Y (m)'\n";
        script << "set zlabel 'Time (s)'\n";
        script << "set grid\n";
        script << "splot 'trajectory_data.txt' using 2:3:1 with linespoints pt 7 ps 1.5 title '3D Path'\n";

        script << "unset multiplot\n";
        script << "pause mouse\n";
        script.close();

        cout << "\n=== Created GNUPlot Script ===" << endl;
        cout << "File: plot_navigation.gp" << endl;
        cout << "\nThe plot shows 4 graphs:" << endl;
        cout << "1. 2D Trajectory (X-Y plane)" << endl;
        cout << "2. Position components vs time" << endl;
        cout << "3. Speed magnitude vs time" << endl;
        cout << "4. 3D trajectory with time as Z-axis" << endl;

        // Запускаем GNUPlot
        cout << "\n=== Launching Plot ===" << endl;
        int result = system("gnuplot -p plot_navigation.gp 2>/dev/null");
        if (result != 0) {
            cout << "Could not launch GNUPlot automatically." << endl;
            cout << "Please run manually: gnuplot -p plot_navigation.gp" << endl;
        }
    }
};

int main() {
    cout << "===== TASK 4: NAVIGATION DATA ANALYSIS =====" << endl;
    cout << "Analyzing 2D trajectory data and computing speed" << endl;

    Navigator nav;

    cout << "\n[1/3] Creating data file..." << endl;
    nav.createDataFile();

    cout << "\n[2/3] Loading data..." << endl;
    nav.loadFromFile("navigation.csv");

    cout << "\n[3/3] Computing velocity and plotting..." << endl;
    nav.computeVelocityAndPlot();

    cout << "\n===== TASK 4 COMPLETED =====" << endl;
    cout << "\nFiles created:" << endl;
    cout << "  navigation.csv     - Original trajectory data" << endl;
    cout << "  velocity.csv       - Computed speed data" << endl;
    cout << "  trajectory_data.txt- Formatted trajectory data" << endl;
    cout << "  speed_data.txt     - Formatted speed data" << endl;
    cout << "  plot_navigation.gp - GNUPlot script with 4 plots" << endl;

    return 0;
}
