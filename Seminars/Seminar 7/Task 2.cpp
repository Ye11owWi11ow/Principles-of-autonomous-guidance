#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <algorithm>

using namespace std;

class SensorData {
private:
    vector<double> t, h1, h2;
public:
    void createDataFile() {
        ofstream fout("sensors.txt");
        if (!fout) {
            cerr << "ERROR: Cannot create sensors.txt" << endl;
            return;
        }

        // Data from condition:
        fout << "0 1000 1002\n";
        fout << "1 1015 1012\n";
        fout << "2 1030 1028\n";
        fout << "3 1048 1042\n";
        fout << "4 1060 1058\n";

        fout.close();
        cout << "Created sensors.txt with 5 data points" << endl;
    }

    void loadFromFile(const string& filename) {
        cout << "\nLoading data from " << filename << endl;
        ifstream fin(filename);
        if (!fin) {
            cerr << "ERROR: Cannot open " << filename << endl;
            return;
        }

        double time, height1, height2;
        while (fin >> time >> height1 >> height2) {
            t.push_back(time);
            h1.push_back(height1);
            h2.push_back(height2);
        }
        fin.close();

        cout << "Successfully loaded " << t.size() << " points" << endl;
    }

    void computeDiffAndPlot() {
        if (t.size() < 2) {
            cerr << "ERROR: Need at least 2 data points" << endl;
            return;
        }

        vector<double> delta;
        for (size_t i = 0; i < t.size(); ++i) {
            delta.push_back(fabs(h1[i] - h2[i]));
        }

        cout << "\n=== Data Analysis ===" << endl;
        cout << "Time\tSensor1\tSensor2\tDelta" << endl;
        cout << "----\t-------\t-------\t-----" << endl;
        for (size_t i = 0; i < t.size(); ++i) {
            printf("%.0f\t%.0f\t%.0f\t%.2f\n", t[i], h1[i], h2[i], delta[i]);
        }

        double avg_delta = 0;
        for (double d : delta) avg_delta += d;
        avg_delta /= delta.size();

        double max_delta = *max_element(delta.begin(), delta.end());
        double min_delta = *min_element(delta.begin(), delta.end());

        cout << "\nStatistics:" << endl;
        cout << "Average delta: " << avg_delta << " m" << endl;
        cout << "Max delta: " << max_delta << " m" << endl;
        cout << "Min delta: " << min_delta << " m" << endl;

        ofstream s1("sensor1_data.txt");
        ofstream s2("sensor2_data.txt");
        ofstream diff("delta_data.txt");

        for (size_t i = 0; i < t.size(); ++i) {
            s1 << t[i] << " " << h1[i] << "\n";
            s2 << t[i] << " " << h2[i] << "\n";
            diff << t[i] << " " << delta[i] << "\n";
        }

        s1.close(); s2.close(); diff.close();

        ofstream script("plot_comparison.gp");
        script << "# Sensor data comparison plot\n";
        script << "set terminal qt size 900,700\n";
        script << "set multiplot layout 2,1\n";
        script << "set style data linespoints\n\n";

        script << "# Top plot: Both sensors\n";
        script << "set title 'Altitude Measurements from Two Sensors'\n";
        script << "set xlabel 'Time (s)'\n";
        script << "set ylabel 'Altitude (m)'\n";
        script << "set grid\n";
        script << "set key top left\n";
        script << "plot 'sensor1_data.txt' using 1:2 pt 7 ps 1.5 lc rgb 'blue' title 'Sensor 1', \\\n";
        script << "     'sensor2_data.txt' using 1:2 pt 9 ps 1.5 lc rgb 'green' title 'Sensor 2'\n\n";

        script << "# Bottom plot: Difference\n";
        script << "set title 'Difference Between Sensors (Δh)'\n";
        script << "set xlabel 'Time (s)'\n";
        script << "set ylabel 'Difference (m)'\n";
        script << "set grid\n";
        script << "set yrange [0:" << (max_delta * 1.2) << "]\n";
        script << "plot 'delta_data.txt' using 1:2 pt 5 ps 2 lc rgb 'red' lw 3 title 'Δh = |h1 - h2|'\n";

        script << "unset multiplot\n";
        script << "pause mouse 'Click to close'\n";
        script.close();

        cout << "\n=== GNUPlot Script Created ===" << endl;
        cout << "File: plot_comparison.gp" << endl;
        cout << "\nThe plot shows:" << endl;
        cout << "1. TOP: Both altitude sensors (1000-1060 m scale)" << endl;
        cout << "2. BOTTOM: Difference between them (0-" << max_delta << " m scale)" << endl;

        ofstream script2("plot_combined.gp");
        script2 << "# Combined plot with two y-axes\n";
        script2 << "set terminal qt\n";
        script2 << "set title 'Sensor Data with Delta (Right Axis)'\n";
        script2 << "set xlabel 'Time (s)'\n";
        script2 << "set ylabel 'Altitude (m)'\n";
        script2 << "set y2label 'Delta (m)'\n";
        script2 << "set ytics nomirror\n";
        script2 << "set y2tics\n";
        script2 << "set grid\n";
        script2 << "set yrange [990:1070]\n";
        script2 << "set y2range [0:" << (max_delta * 1.5) << "]\n";
        script2 << "plot 'sensor1_data.txt' using 1:2 with linespoints title 'Sensor 1', \\\n";
        script2 << "     'sensor2_data.txt' using 1:2 with linespoints title 'Sensor 2', \\\n";
        script2 << "     'delta_data.txt' using 1:2 axes x1y2 with lines lw 3 title 'Delta h'\n";
        script2 << "pause mouse\n";
        script2.close();

        cout << "\nAlternative plot: plot_combined.gp (delta on right axis)" << endl;

        cout << "\n=== Launching GNUPlot ===" << endl;
        cout << "Choose plot type:" << endl;
        cout << "1. Two separate plots (recommended): plot_comparison.gp" << endl;
        cout << "2. Combined with two y-axes: plot_combined.gp" << endl;

        int result = system("gnuplot -p plot_comparison.gp 2>/dev/null");
        if (result != 0) {
            cout << "\nGNUPlot not found or failed to launch." << endl;
            cout << "Please install GNUPlot or run manually:" << endl;
            cout << "  gnuplot -p plot_comparison.gp" << endl;
        }
    }
};

int main() {
    cout << "===== TASK 2: SENSOR DATA COMPARISON =====" << endl;
    cout << "Comparing two altitude sensors over time" << endl;
    cout << "Data: 5 time points from 0 to 4 seconds" << endl;

    SensorData sensors;

    cout << "\n[1/3] Creating data file..." << endl;
    sensors.createDataFile();

    cout << "\n[2/3] Loading data..." << endl;
    sensors.loadFromFile("sensors.txt");

    cout << "\n[3/3] Computing differences and plotting..." << endl;
    sensors.computeDiffAndPlot();

    cout << "\n===== TASK 2 COMPLETED =====" << endl;
    cout << "\nFiles created:" << endl;
    cout << "  sensors.txt          - Raw data" << endl;
    cout << "  sensor1_data.txt     - Sensor 1 data" << endl;
    cout << "  sensor2_data.txt     - Sensor 2 data" << endl;
    cout << "  delta_data.txt       - Difference data" << endl;
    cout << "  plot_comparison.gp   - Main plot script" << endl;
    cout << "  plot_combined.gp     - Alternative plot" << endl;

    cout << "\nTo view plots manually:" << endl;
    cout << "  gnuplot -p plot_comparison.gp" << endl;
    cout << "  gnuplot -p plot_combined.gp" << endl;

    return 0;
}
