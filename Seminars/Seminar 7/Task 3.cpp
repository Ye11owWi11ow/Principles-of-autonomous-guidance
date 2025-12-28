#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstdlib>

using namespace std;

class AltitudeFilter {
private:
    vector<pair<double, double>> data;
public:
    // Create altitude data file with values from condition
    void createDataFile() {
        ofstream fout("altitude.csv");
        if (!fout) {
            cerr << "ERROR: Cannot create altitude.csv" << endl;
            return;
        }

        fout << "t,H\n";
        // Data from condition:
        vector<vector<double>> altitudeData = {
            {0, 1000},
            {1, 1005},
            {2, 1010},
            {3, 1500},  // outlier
            {4, 1020},
            {5, 1025}
        };

        for (const auto& row : altitudeData) {
            fout << row[0] << "," << row[1] << "\n";
        }
        fout.close();

        // Проверяем создание файла
        cout << "=== Created altitude.csv ===" << endl;
        system("cat altitude.csv 2>/dev/null || type altitude.csv 2>/dev/null");
        cout << "\nNote: Value at t=3 (1500 m) is an outlier (outside 900-1100 range)" << endl;
    }

    void loadFromFile(const string& filename) {
        cout << "\n=== Loading altitude data ===" << endl;
        ifstream fin(filename);
        if (!fin) {
            cerr << "ERROR: Cannot open " << filename << endl;
            return;
        }

        string line;
        getline(fin, line); // header
        double time, height;
        char comma;
        int count = 0;

        while (fin >> time >> comma >> height) {
            data.push_back({time, height});
            count++;
        }
        fin.close();

        cout << "Loaded " << count << " altitude readings" << endl;
    }

    void filterAndPlot() {
        if (data.empty()) {
            cerr << "ERROR: No data loaded" << endl;
            return;
        }

        // Use bounds from condition: 900 < H < 1100
        const double minAlt = 900;
        const double maxAlt = 1100;
        cout << "\nUsing filter bounds from condition: "
             << minAlt << " < H < " << maxAlt << endl;

        // Lambda for filtering
        auto isOutlier = [minAlt, maxAlt](double h) {
            return h <= minAlt || h >= maxAlt;
        };

        vector<pair<double, double>> filtered;
        // Используем copy_if с лямбдой
        copy_if(data.begin(), data.end(), back_inserter(filtered),
                [&](const pair<double, double>& p) {
                    return !isOutlier(p.second);
                });

        cout << "Filtered " << filtered.size() << " valid points from "
             << data.size() << " total points" << endl;

        if (data.size() > filtered.size()) {
            cout << "Removed " << (data.size() - filtered.size()) << " outlier(s)" << endl;
        }

        // Save filtered data
        ofstream fout("filtered.csv");
        fout << "t,H\n";
        for (const auto& p : filtered) {
            fout << p.first << "," << p.second << "\n";
        }
        fout.close();

        // Также создаем текстовые файлы для GNUPlot
        ofstream raw("raw_data.txt");
        ofstream filt("filtered_data.txt");

        raw << "# Raw altitude data\n";
        filt << "# Filtered altitude data (900-1100 m)\n";

        for (const auto& p : data) {
            raw << p.first << " " << p.second << "\n";
        }

        for (const auto& p : filtered) {
            filt << p.first << " " << p.second << "\n";
        }

        raw.close();
        filt.close();

        // Создаем скрипт GNUPlot
        ofstream script("plot_filtering.gp");
        script << "# Altitude data filtering\n";
        script << "set terminal qt size 900,600\n";
        script << "set multiplot layout 2,1\n\n";

        // Верхний график: исходные данные
        script << "# Top: Raw data with bounds\n";
        script << "set title 'Raw Altitude Data with Outliers'\n";
        script << "set xlabel 'Time (s)'\n";
        script << "set ylabel 'Altitude (m)'\n";
        script << "set grid\n";
        script << "set yrange [800:1600]\n";
        script << "set arrow from graph 0, first " << minAlt << " to graph 1, first " << minAlt << " nohead lc rgb 'red' lw 2\n";
        script << "set arrow from graph 0, first " << maxAlt << " to graph 1, first " << maxAlt << " nohead lc rgb 'red' lw 2\n";
        script << "plot 'raw_data.txt' using 1:2 with points pt 7 ps 1.5 lc rgb 'blue' title 'Raw Data'\n\n";

        // Нижний график: отфильтрованные данные
        script << "# Bottom: Filtered data\n";
        script << "set title 'Filtered Data (900-1100 m range)'\n";
        script << "set xlabel 'Time (s)'\n";
        script << "set ylabel 'Altitude (m)'\n";
        script << "set grid\n";
        script << "set yrange [" << (minAlt-10) << ":" << (maxAlt+10) << "]\n";
        script << "plot 'filtered_data.txt' using 1:2 with linespoints pt 5 ps 1.5 lc rgb 'green' title 'Filtered'\n";

        script << "unset multiplot\n";
        script << "pause mouse\n";
        script.close();

        cout << "\n=== Created GNUPlot Script ===" << endl;
        cout << "File: plot_filtering.gp" << endl;
        cout << "\nThe plot shows:" << endl;
        cout << "1. TOP: Raw data with red lines showing filter bounds" << endl;
        cout << "2. BOTTOM: Filtered data within 900-1100 m range" << endl;

        // Запускаем GNUPlot
        cout << "\n=== Launching Plot ===" << endl;
        int result = system("gnuplot -p plot_filtering.gp 2>/dev/null");
        if (result != 0) {
            cout << "Could not launch GNUPlot automatically." << endl;
            cout << "Please run manually: gnuplot -p plot_filtering.gp" << endl;
        }
    }
};

int main() {
    cout << "===== TASK 3: ALTITUDE DATA FILTERING =====" << endl;
    cout << "Filtering altitude data using lambda functions" << endl;
    cout << "Condition: Keep only values where 900 < H < 1100" << endl;

    AltitudeFilter filter;

    cout << "\n[1/3] Creating data file..." << endl;
    filter.createDataFile();

    cout << "\n[2/3] Loading data..." << endl;
    filter.loadFromFile("altitude.csv");

    cout << "\n[3/3] Filtering and plotting..." << endl;
    filter.filterAndPlot();

    cout << "\n===== TASK 3 COMPLETED =====" << endl;
    cout << "\nFiles created:" << endl;
    cout << "  altitude.csv       - Original CSV data" << endl;
    cout << "  filtered.csv       - Filtered CSV data" << endl;
    cout << "  raw_data.txt       - Raw data for GNUPlot" << endl;
    cout << "  filtered_data.txt  - Filtered data for GNUPlot" << endl;
    cout << "  plot_filtering.gp  - GNUPlot script" << endl;

    return 0;
}
