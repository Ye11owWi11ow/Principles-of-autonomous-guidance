#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <cmath>

using namespace std;

class Trajectory {
private:
    vector<double> t;
    vector<double> x;
public:
    void createDataFile() {
        ofstream fout("traj.csv");
        if (!fout) {
            cerr << "Error creating traj.csv" << endl;
            return;
        }
        fout << "t,x\n";
        // Data from task: t,x: 0,0; 1,2; 2,3.8; 3,6.1; 4,8.0
        fout << "0,0\n";
        fout << "1,2\n";
        fout << "2,3.8\n";
        fout << "3,6.1\n";
        fout << "4,8.0\n";
        fout.close();
        cout << "Created traj.csv with trajectory data" << endl;
    }

    void loadFromFile() {
        cout << "Reading trajectory from file: traj.csv" << endl;
        ifstream fin("traj.csv");
        if (!fin.is_open()) {
            cerr << "Error opening file: traj.csv" << endl;
            return;
        }
        string line;
        getline(fin, line); // skip header
        double time, coord;
        char comma;
        while (fin >> time >> comma >> coord) {
            t.push_back(time);
            x.push_back(coord);
        }
        fin.close();
        cout << "Loaded " << t.size() << " data points" << endl;
    }

    void computeAndPlot() {
        if (t.size() < 2) {
            cerr << "Not enough data points for computation" << endl;
            return;
        }

        // Compute velocity using lambda
        vector<double> v;
        auto computeV = [](double x1, double x2, double t1, double t2) {
            return (x2 - x1) / (t2 - t1);
        };

        for (size_t i = 0; i < t.size() - 1; ++i) {
            v.push_back(computeV(x[i], x[i+1], t[i], t[i+1]));
        }
        v.push_back(v.back()); // last value

        cout << "\nComputed velocities:" << endl;
        for (size_t i = 0; i < v.size(); ++i) {
            cout << "t=" << t[i] << "s: v=" << v[i] << " m/s" << endl;
        }

        // Plot with GNUPlot
        FILE* gp = popen("gnuplot -persistent", "w");
        if (!gp) {
            cerr << "Error launching GNUPlot" << endl;
            return;
        }

        fprintf(gp, "set title 'Trajectory x(t) and Velocity v(t)'\n");
        fprintf(gp, "set xlabel 'Time (s)'\n");
        fprintf(gp, "set ylabel 'Coordinate x (m)'\n");
        fprintf(gp, "set y2label 'Velocity v (m/s)'\n");
        fprintf(gp, "set y2tics\n");
        fprintf(gp, "set grid\n");
        fprintf(gp, "plot '-' using 1:2 with linespoints title 'x(t)', ");
        fprintf(gp, "'-' using 1:2 with lines axes x1y2 title 'v(t)'\n");

        // x(t) data
        for (size_t i = 0; i < t.size(); ++i) {
            fprintf(gp, "%f %f\n", t[i], x[i]);
        }
        fprintf(gp, "e\n");

        // v(t) data
        for (size_t i = 0; i < t.size(); ++i) {
            fprintf(gp, "%f %f\n", t[i], v[i]);
        }
        fprintf(gp, "e\n");

        fflush(gp);
        cout << "\nPlot displayed. Close GNUPlot window to continue..." << endl;
        pclose(gp);
    }
};

int main() {
    Trajectory traj;
    traj.createDataFile();
    traj.loadFromFile();
    traj.computeAndPlot();
    return 0;
}
