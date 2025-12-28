#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <algorithm>  // для max_element, min_element
#include <numeric>    // для accumulate

using namespace std;

class MotionAnalyzer {
private:
    vector<double> t, x, v, a;
public:
    // Create motion data file with values from condition
    void createDataFile() {
        ofstream fout("motion.csv");
        if (!fout) {
            cerr << "ERROR: Cannot create motion.csv" << endl;
            return;
        }

        fout << "t,x\n";
        // Data from condition:
        vector<vector<double>> motionData = {
            {0, 0},
            {1, 2.1},
            {2, 4.5},
            {3, 7.2},
            {4, 10.4},
            {5, 14.1},
            {6, 18.5}
        };

        for (const auto& row : motionData) {
            fout << row[0] << "," << row[1] << "\n";
        }
        fout.close();

        cout << "=== Created motion.csv ===" << endl;
        system("head -10 motion.csv 2>/dev/null || type motion.csv");
        cout << "\nCreated motion.csv with " << motionData.size() << " data points" << endl;
    }

    void loadFromFile(const string& filename) {
        cout << "\n=== Loading motion data ===" << endl;
        ifstream fin(filename);
        if (!fin) {
            cerr << "ERROR: Cannot open " << filename << endl;
            return;
        }

        string line;
        getline(fin, line); // header
        double time, coord;
        char comma;
        int count = 0;

        while (fin >> time >> comma >> coord) {
            t.push_back(time);
            x.push_back(coord);
            count++;
        }
        fin.close();

        cout << "Loaded " << count << " motion data points" << endl;
    }

    void computeVelocity() {
        if (t.size() < 2) {
            cerr << "ERROR: Not enough data for velocity computation" << endl;
            return;
        }

        v.resize(t.size());
        // Lambda for velocity calculation
        auto velLambda = [](double x1, double x2, double t1, double t2) {
            if (t2 - t1 == 0) return 0.0;
            return (x2 - x1) / (t2 - t1);
        };

        // Центральные разности для внутренних точек
        for (size_t i = 1; i < t.size() - 1; ++i) {
            v[i] = (x[i+1] - x[i-1]) / (t[i+1] - t[i-1]);
        }

        // Крайние точки: односторонние разности
        v[0] = (x[1] - x[0]) / (t[1] - t[0]);
        v.back() = (x.back() - x[x.size()-2]) / (t.back() - t[t.size()-2]);

        cout << "Velocity computed for " << v.size() << " points" << endl;
    }

    void computeAcceleration() {
        if (t.size() < 2 || v.size() < 2) {
            cerr << "ERROR: Not enough data for acceleration computation" << endl;
            return;
        }

        a.resize(t.size());
        // Lambda for acceleration calculation
        auto accLambda = [](double v1, double v2, double t1, double t2) {
            if (t2 - t1 == 0) return 0.0;
            return (v2 - v1) / (t2 - t1);
        };

        // Центральные разности для внутренних точек
        for (size_t i = 1; i < t.size() - 1; ++i) {
            a[i] = (v[i+1] - v[i-1]) / (t[i+1] - t[i-1]);
        }

        // Крайние точки: односторонние разности
        a[0] = (v[1] - v[0]) / (t[1] - t[0]);
        a.back() = (v.back() - v[v.size()-2]) / (t.back() - t[t.size()-2]);

        cout << "Acceleration computed for " << a.size() << " points" << endl;
    }

    void saveResults(const string& filename) {
        ofstream fout(filename);
        fout << "t,x,v,a\n";
        for (size_t i = 0; i < t.size(); ++i) {
            fout << t[i] << "," << x[i] << "," << v[i] << "," << a[i] << "\n";
        }
        fout.close();

        // Выводим таблицу в консоль
        cout << "\n=== Processed Motion Data ===" << endl;
        cout << "Time\tPosition\tVelocity\tAcceleration" << endl;
        cout << "(s)\t(m)\t\t(m/s)\t\t(m/s²)" << endl;
        cout << "----\t--------\t--------\t-----------" << endl;
        for (size_t i = 0; i < t.size(); ++i) {
            printf("%.1f\t%.2f\t\t%.3f\t\t%.3f\n", t[i], x[i], v[i], a[i]);
        }

        // Вычисляем статистику
        double max_v = *max_element(v.begin(), v.end());
        double min_v = *min_element(v.begin(), v.end());
        double max_a = *max_element(a.begin(), a.end());
        double min_a = *min_element(a.begin(), a.end());

        double avg_v = accumulate(v.begin(), v.end(), 0.0) / v.size();
        double avg_a = accumulate(a.begin(), a.end(), 0.0) / a.size();

        cout << "\n=== Statistics ===" << endl;
        cout << "Velocity - Max: " << max_v << " m/s, Min: " << min_v
             << " m/s, Avg: " << avg_v << " m/s" << endl;
        cout << "Acceleration - Max: " << max_a << " m/s², Min: " << min_a
             << " m/s², Avg: " << avg_a << " m/s²" << endl;

        cout << "\nProcessed data saved to " << filename << " as required" << endl;
    }

    void plotResults() {
        // Создаем файлы данных для GNUPlot
        ofstream pos("position_data.txt");
        ofstream vel("velocity_data.txt");
        ofstream acc("acceleration_data.txt");

        pos << "# Position data\n";
        vel << "# Velocity data\n";
        acc << "# Acceleration data\n";

        for (size_t i = 0; i < t.size(); ++i) {
            pos << t[i] << " " << x[i] << "\n";
            vel << t[i] << " " << v[i] << "\n";
            acc << t[i] << " " << a[i] << "\n";
        }

        pos.close(); vel.close(); acc.close();

        // Создаем скрипт GNUPlot
        ofstream script("plot_motion.gp");
        script << "# Motion analysis: position, velocity, acceleration\n";
        script << "set terminal qt size 1200,800\n";
        script << "set multiplot layout 3,1\n\n";

        // График 1: Положение
        script << "# Plot 1: Position\n";
        script << "set title 'Position vs Time'\n";
        script << "set xlabel 'Time (s)'\n";
        script << "set ylabel 'Position (m)'\n";
        script << "set grid\n";
        script << "set key top left\n";
        script << "plot 'position_data.txt' using 1:2 with linespoints pt 7 ps 1.5 lc rgb 'blue' title 'x(t)'\n\n";

        // График 2: Скорость
        script << "# Plot 2: Velocity\n";
        script << "set title 'Velocity vs Time'\n";
        script << "set xlabel 'Time (s)'\n";
        script << "set ylabel 'Velocity (m/s)'\n";
        script << "set grid\n";
        script << "set key top left\n";
        script << "plot 'velocity_data.txt' using 1:2 with linespoints pt 9 ps 1.5 lc rgb 'red' title 'v(t)'\n\n";

        // График 3: Ускорение
        script << "# Plot 3: Acceleration\n";
        script << "set title 'Acceleration vs Time'\n";
        script << "set xlabel 'Time (s)'\n";
        script << "set ylabel 'Acceleration (m/s²)'\n";
        script << "set grid\n";
        script << "set key top left\n";
        script << "plot 'acceleration_data.txt' using 1:2 with linespoints pt 5 ps 1.5 lc rgb 'green' title 'a(t)', \\\n";
        script << "     0 title '' lc rgb 'black'  # Zero line\n";

        script << "unset multiplot\n";
        script << "pause mouse\n";
        script.close();

        cout << "\n=== Created GNUPlot Script ===" << endl;
        cout << "File: plot_motion.gp" << endl;
        cout << "\nThe plot shows 3 graphs:" << endl;
        cout << "1. Position vs time (blue)" << endl;
        cout << "2. Velocity vs time (red)" << endl;
        cout << "3. Acceleration vs time (green)" << endl;

        // Запускаем GNUPlot
        cout << "\n=== Launching Plot ===" << endl;
        int result = system("gnuplot -p plot_motion.gp 2>/dev/null");
        if (result != 0) {
            cout << "Could not launch GNUPlot automatically." << endl;
            cout << "Please run manually: gnuplot -p plot_motion.gp" << endl;
        }
    }
};

int main() {
    cout << "===== TASK 5: MOTION ANALYSIS =====" << endl;
    cout << "Analyzing 1D motion: computing velocity and acceleration" << endl;
    cout << "Using central difference method for better accuracy" << endl;

    MotionAnalyzer analyzer;

    cout << "\n[1/4] Creating data file..." << endl;
    analyzer.createDataFile();

    cout << "\n[2/4] Loading data..." << endl;
    analyzer.loadFromFile("motion.csv");

    cout << "\n[3/4] Computing derivatives..." << endl;
    analyzer.computeVelocity();
    analyzer.computeAcceleration();

    cout << "\n[4/4] Saving results and plotting..." << endl;
    analyzer.saveResults("motion_processed.csv");
    analyzer.plotResults();

    cout << "\n===== TASK 5 COMPLETED =====" << endl;
    cout << "\nFiles created:" << endl;
    cout << "  motion.csv           - Original position data" << endl;
    cout << "  motion_processed.csv - Processed data (t,x,v,a)" << endl;
    cout << "  position_data.txt    - Position for GNUPlot" << endl;
    cout << "  velocity_data.txt    - Velocity for GNUPlot" << endl;
    cout << "  acceleration_data.txt- Acceleration for GNUPlot" << endl;
    cout << "  plot_motion.gp       - GNUPlot script with 3 plots" << endl;

    return 0;
}
