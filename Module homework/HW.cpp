#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <limits>

using namespace std;

const double M_PI = 3.14159265358979323846;

// ========== CONSTANTS FOR TU-134 ==========
const double TU134_MASS = 47000.0; // kg
const double TU134_WING_AREA = 127.0; // m²
const double TU134_NOMINAL_THRUST = 2 * 68000.0; // N (2x D-30 engines)
const double MAX_THRUST_PERCENT = 1.0; // 100% thrust
const double INITIAL_ALTITUDE = 300.0; // m
const double FINAL_ALTITUDE = 6000.0; // m
const double INITIAL_VELOCITY = 310.0 / 3.6; // m/s (310 km/h)
const double FINAL_VELOCITY = 700.0 / 3.6; // m/s (700 km/h)
const double GRAVITY = 9.81; // m/s²

// Aerodynamic coefficients
const double Cx0 = 0.022; // Zero-lift drag coefficient
const double K = 0.045; // Induced drag factor
const double Cl_alpha = 5.2; // Lift curve slope (per radian)
const double Cy_max = 1.4; // Maximum lift coefficient
const double Cp = 0.6 / 9.81; // Specific fuel consumption (kg/(N·h)) - CORRECTED!
const double phi_p = 2.0 * M_PI / 180.0; // Engine inclination

// Grid parameters
const int N = 30; // Number of grid points
const double MAX_VERTICAL_SPEED = 8.0; // m/s
const double MAX_CLIMB_ANGLE = 15.0 * M_PI / 180.0; // radians
const double MIN_CLIMB_SPEED = 300.0 / 3.6; // m/s

// Optimization criteria
enum OptimizationCriterion {
    MIN_TIME = 1,
    MIN_FUEL = 2
};

// Maneuver types
enum ManeuverType {
    ACCELERATION = 1,
    CLIMB = 2,
    COMBINED = 3
};

// ========== ATMOSPHERIC MODEL ==========
struct AtmosPoint {
    double H, rho, a;
};

static const AtmosPoint ATMOS_TABLE[] = {
    {0.0, 1.22500, 340.294},
    {500.0, 1.16727, 338.370},
    {1000.0, 1.11166, 336.435},
    {2000.0, 1.00655, 332.532},
    {3000.0, 0.909254, 328.584},
    {4000.0, 0.819347, 324.589},
    {5000.0, 0.736429, 320.545},
    {6000.0, 0.660111, 316.452},
    {7000.0, 0.590018, 312.306},
    {8000.0, 0.526783, 308.105},
    {9000.0, 0.467063, 303.848},
    {10000.0, 0.413510, 299.532}
};

const int ATMOS_N = 12;

void atmosphere(double H, double& rho, double& a_sound) {
    if (H <= ATMOS_TABLE[0].H) {
        rho = ATMOS_TABLE[0].rho;
        a_sound = ATMOS_TABLE[0].a;
        return;
    }
    if (H >= ATMOS_TABLE[ATMOS_N - 1].H) {
        rho = ATMOS_TABLE[ATMOS_N - 1].rho;
        a_sound = ATMOS_TABLE[ATMOS_N - 1].a;
        return;
    }

    for (int i = 0; i < ATMOS_N - 1; i++) {
        if (H >= ATMOS_TABLE[i].H && H < ATMOS_TABLE[i + 1].H) {
            double t = (H - ATMOS_TABLE[i].H) / (ATMOS_TABLE[i + 1].H - ATMOS_TABLE[i].H);
            rho = ATMOS_TABLE[i].rho + t * (ATMOS_TABLE[i + 1].rho - ATMOS_TABLE[i].rho);
            a_sound = ATMOS_TABLE[i].a + t * (ATMOS_TABLE[i + 1].a - ATMOS_TABLE[i].a);
            return;
        }
    }
}

// ========== AERODYNAMIC FUNCTIONS ==========
double getLiftCoefficient(double alpha) {
    return min(Cl_alpha * alpha, Cy_max);
}

double getDragCoefficient(double alpha) {
    double Cl = getLiftCoefficient(alpha);
    return Cx0 + K * Cl * Cl;
}

double computeLiftForce(double V, double h, double alpha, double mass) {
    double rho, a_sound;
    atmosphere(h, rho, a_sound);
    double q = 0.5 * rho * V * V;
    double Cl = getLiftCoefficient(alpha);
    double lift = Cl * TU134_WING_AREA * q;

    // For climb calculations, we need enough lift to support the aircraft
    double required_lift = mass * GRAVITY * cos(min(alpha, MAX_CLIMB_ANGLE));
    return max(lift, required_lift * 0.8); // Safety factor
}

double computeDragForce(double V, double h, double alpha) {
    double rho, a_sound;
    atmosphere(h, rho, a_sound);
    double q = 0.5 * rho * V * V;
    double Cd = getDragCoefficient(alpha);
    return Cd * TU134_WING_AREA * q;
}

// ========== THRUST AND FUEL FUNCTIONS ==========
double getThrustSetting(OptimizationCriterion criterion, double alt_progress = 0.0) {
    if (criterion == MIN_TIME) {
        return 1.0; // 100% thrust for minimum time
    } else {
        // Для экономии топлива: начинаем с 85%, уменьшаем до ~72% на высоте
        return 0.85 * (1.0 - alt_progress * 0.15);
    }
}

double getAlphaForCriterion(OptimizationCriterion criterion, double alt_progress = 0.5,
                           ManeuverType maneuver = COMBINED) {
    double base_angle;

    if (criterion == MIN_TIME) {
        switch (maneuver) {
            case ACCELERATION: base_angle = 2.0; break;
            case CLIMB: base_angle = 7.0; break;
            case COMBINED: base_angle = 4.5; break;
        }
        base_angle *= (1.0 - alt_progress * 0.2);
    } else {
        switch (maneuver) {
            case ACCELERATION: base_angle = 1.5; break;
            case CLIMB: base_angle = 5.0; break;
            case COMBINED: base_angle = 3.0; break;
        }
        base_angle *= (1.0 - alt_progress * 0.1);
    }

    base_angle = max(1.0, min(8.0, base_angle));
    return base_angle * M_PI / 180.0;
}

double computeFuelFlow(double thrust) {
    return thrust * Cp / 3600.0;
}

// ========== SEGMENT CALCULATIONS ==========
struct SegmentData {
    double time;
    double fuel;
    double dV_dt;
    double Vy;
    double theta;
    bool valid;

    SegmentData() : time(1e9), fuel(1e9), dV_dt(0), Vy(0), theta(0), valid(false) {}
};

SegmentData calculate_acceleration(double H, double V1, double V2, double mass,
                                  OptimizationCriterion criterion) {
    SegmentData result;

    if (V2 <= V1) return result;

    double V_avg = 0.5 * (V1 + V2);
    double alt_progress = (H - INITIAL_ALTITUDE) / (FINAL_ALTITUDE - INITIAL_ALTITUDE);

    double alpha = getAlphaForCriterion(criterion, alt_progress, ACCELERATION);
    double thrust_setting = getThrustSetting(criterion, alt_progress);
    double thrust = TU134_NOMINAL_THRUST * MAX_THRUST_PERCENT * thrust_setting;

    double drag = computeDragForce(V_avg, H, alpha);
    double min_dV_dt = (criterion == MIN_TIME) ? 0.01 : 0.005;

    double dV_dt = (thrust * cos(alpha + phi_p) - drag) / mass;
    if (dV_dt <= min_dV_dt) return result;

    double dt = (V2 - V1) / dV_dt;
    if (dt <= 0 || dt > 1000.0) return result;

    double fuel_flow = computeFuelFlow(thrust);
    double fuel = fuel_flow * dt;

    result.time = dt;
    result.fuel = fuel;
    result.dV_dt = dV_dt;
    result.Vy = 0;
    result.theta = 0;
    result.valid = true;

    return result;
}

SegmentData calculate_climb(double H1, double H2, double V, double mass,
                           OptimizationCriterion criterion) {
    SegmentData result;

    if (H2 <= H1) return result;

    double min_climb_speed = (criterion == MIN_TIME) ? MIN_CLIMB_SPEED : MIN_CLIMB_SPEED * 1.1;
    if (V < min_climb_speed) return result;

    double H_avg = 0.5 * (H1 + H2);
    double alt_progress = (H_avg - INITIAL_ALTITUDE) / (FINAL_ALTITUDE - INITIAL_ALTITUDE);

    double alpha = getAlphaForCriterion(criterion, alt_progress, CLIMB);
    double thrust_setting = getThrustSetting(criterion, alt_progress);
    double thrust = TU134_NOMINAL_THRUST * MAX_THRUST_PERCENT * thrust_setting;

    double lift = computeLiftForce(V, H_avg, alpha, mass);
    double drag = computeDragForce(V, H_avg, alpha);

    double thrust_vertical = thrust * sin(alpha + phi_p);
    double required_lift = mass * GRAVITY;
    double excess_power_vertical = thrust_vertical + (lift - required_lift);

    double min_excess = (criterion == MIN_TIME) ? mass * GRAVITY * 0.01 : mass * GRAVITY * 0.005;
    if (excess_power_vertical <= min_excess) return result;

    double sin_theta = min(excess_power_vertical / (mass * GRAVITY), sin(MAX_CLIMB_ANGLE));
    double min_sin_theta = (criterion == MIN_TIME) ? 0.02 : 0.015;
    sin_theta = max(sin_theta, min_sin_theta);

    double Vy = V * sin_theta;
    double max_vy = (criterion == MIN_TIME) ? MAX_VERTICAL_SPEED : MAX_VERTICAL_SPEED * 0.9;
    if (Vy > max_vy) {
        Vy = max_vy;
        sin_theta = Vy / V;
    }

    double dt = (H2 - H1) / Vy;
    if (dt <= 0 || dt > 2000.0) return result;

    double fuel_flow = computeFuelFlow(thrust);
    double fuel = fuel_flow * dt;

    result.time = dt;
    result.fuel = fuel;
    result.dV_dt = 0;
    result.Vy = Vy;
    result.theta = asin(sin_theta);
    result.valid = true;

    return result;
}

SegmentData calculate_combined(double H1, double H2, double V1, double V2, double mass,
                              OptimizationCriterion criterion) {
    SegmentData result;

    if (H2 <= H1 || V2 <= V1) return result;

    if (criterion == MIN_FUEL) {
        double dH = H2 - H1;
        double dV = V2 - V1;
        double max_dH_step = (FINAL_ALTITUDE - INITIAL_ALTITUDE) / N;
        double max_dV_step = (FINAL_VELOCITY - INITIAL_VELOCITY) / N;

        if (dH > max_dH_step * 1.5 || dV > max_dV_step * 1.5) {
            return result;
        }
    }

    double H_avg = 0.5 * (H1 + H2);
    double V_avg = 0.5 * (V1 + V2);
    double alt_progress = (H_avg - INITIAL_ALTITUDE) / (FINAL_ALTITUDE - INITIAL_ALTITUDE);

    double alpha = getAlphaForCriterion(criterion, alt_progress, COMBINED);
    double base_thrust_setting = getThrustSetting(criterion, alt_progress);
    double thrust_setting = (criterion == MIN_TIME) ? base_thrust_setting :
                           min(base_thrust_setting * 1.1, 0.9);

    double thrust = TU134_NOMINAL_THRUST * MAX_THRUST_PERCENT * thrust_setting;

    double lift = computeLiftForce(V_avg, H_avg, alpha, mass);
    double drag = computeDragForce(V_avg, H_avg, alpha);

    double min_dV_dt = (criterion == MIN_TIME) ? 0.01 : 0.003;

    double dV_dt = (thrust * cos(alpha + phi_p) - drag) / mass;
    if (dV_dt <= min_dV_dt) {
        if (criterion == MIN_FUEL && dV_dt > 0) {
            dV_dt = max(dV_dt, min_dV_dt);
        } else {
            return result;
        }
    }

    double thrust_vertical = thrust * sin(alpha + phi_p);
    double required_lift = mass * GRAVITY;
    double excess_power_vertical = thrust_vertical + (lift - required_lift);

    double min_excess = (criterion == MIN_TIME) ? mass * GRAVITY * 0.005 : mass * GRAVITY * 0.002;
    if (excess_power_vertical <= min_excess) return result;

    double sin_theta = min(excess_power_vertical / (mass * GRAVITY), sin(MAX_CLIMB_ANGLE * 0.6));
    double min_sin_theta = (criterion == MIN_TIME) ? 0.015 : 0.01;
    sin_theta = max(sin_theta, min_sin_theta);

    double Vy = V_avg * sin_theta;
    double max_vy_limit = (criterion == MIN_TIME) ? MAX_VERTICAL_SPEED * 1.2 : MAX_VERTICAL_SPEED;
    if (Vy > max_vy_limit) Vy = max_vy_limit;

    double dH = H2 - H1;
    double dV = V2 - V1;

    double time_for_climb = dH / Vy;
    double time_for_accel = dV / dV_dt;
    double dt = max(time_for_climb, time_for_accel);

    double max_dt = (criterion == MIN_TIME) ? 1500.0 : 2000.0;
    if (dt <= 0 || dt > max_dt) return result;

    Vy = dH / dt;
    dV_dt = dV / dt;

    double fuel_flow = computeFuelFlow(thrust);
    double fuel = fuel_flow * dt;

    result.time = dt;
    result.fuel = fuel;
    result.dV_dt = dV_dt;
    result.Vy = Vy;
    result.theta = asin(Vy / V_avg);
    result.valid = true;

    return result;
}

// ========== TRAJECTORY RESULT STRUCTURE ==========
struct TrajectoryResult {
    vector<pair<double, double>> path; // (altitude, velocity)
    vector<double> time_points;
    vector<double> mass_points;
    vector<double> fuel_points;
    vector<ManeuverType> maneuvers;
    double total_time;
    double total_fuel;
    double avg_climb_rate;
    int used_acceleration;
    int used_climb;
    int used_combined;

    TrajectoryResult() : total_time(0), total_fuel(0), avg_climb_rate(0),
                        used_acceleration(0), used_climb(0), used_combined(0) {}
};

// ========== GRID-BASED OPTIMIZATION ==========
TrajectoryResult solve_trajectory_grid(OptimizationCriterion criterion) {
    TrajectoryResult trajectory;

    cout << "\n========================================\n";
    if (criterion == MIN_TIME) {
        cout << "OPTIMIZATION CRITERION: MINIMUM TIME\n";
    } else {
        cout << "OPTIMIZATION CRITERION: MINIMUM FUEL\n";
    }
    cout << "========================================\n\n";

    double dH = (FINAL_ALTITUDE - INITIAL_ALTITUDE) / N;
    double dV = (FINAL_VELOCITY - INITIAL_VELOCITY) / N;

    vector<double> H_grid(N + 1);
    vector<double> V_grid(N + 1);

    for (int i = 0; i <= N; i++) {
        H_grid[i] = INITIAL_ALTITUDE + i * dH;
        V_grid[i] = INITIAL_VELOCITY + i * dV;
    }

    vector<vector<double>> cost(N + 1, vector<double>(N + 1, 1e9));
    vector<vector<double>> time(N + 1, vector<double>(N + 1, 0));
    vector<vector<double>> fuel(N + 1, vector<double>(N + 1, 0));
    vector<vector<double>> mass(N + 1, vector<double>(N + 1, TU134_MASS));
    vector<vector<int>> prev_i(N + 1, vector<int>(N + 1, -1));
    vector<vector<int>> prev_j(N + 1, vector<int>(N + 1, -1));
    vector<vector<ManeuverType>> maneuver(N + 1, vector<ManeuverType>(N + 1, ACCELERATION));

    cost[0][0] = 0;
    mass[0][0] = TU134_MASS;

    for (int i = 0; i <= N; i++) {
        for (int j = 0; j <= N; j++) {
            if (cost[i][j] >= 1e9) continue;

            double H1 = H_grid[i];
            double V1 = V_grid[j];
            double current_mass = mass[i][j];

            // Acceleration only
            if (j < N) {
                double V2 = V_grid[j + 1];
                SegmentData seg = calculate_acceleration(H1, V1, V2, current_mass, criterion);

                if (seg.valid) {
                    double new_mass = current_mass - seg.fuel;
                    if (new_mass < TU134_MASS * 0.85) continue;

                    double cost_inc = (criterion == MIN_TIME) ? seg.time : seg.fuel;
                    double new_cost = cost[i][j] + cost_inc;

                    if (new_cost < cost[i][j + 1]) {
                        cost[i][j + 1] = new_cost;
                        time[i][j + 1] = time[i][j] + seg.time;
                        fuel[i][j + 1] = fuel[i][j] + seg.fuel;
                        mass[i][j + 1] = new_mass;
                        prev_i[i][j + 1] = i;
                        prev_j[i][j + 1] = j;
                        maneuver[i][j + 1] = ACCELERATION;
                    }
                }
            }

            // Climb only
            if (i < N) {
                double H2 = H_grid[i + 1];
                SegmentData seg = calculate_climb(H1, H2, V1, current_mass, criterion);

                if (seg.valid) {
                    double new_mass = current_mass - seg.fuel;
                    if (new_mass < TU134_MASS * 0.85) continue;

                    double cost_inc = (criterion == MIN_TIME) ? seg.time : seg.fuel;
                    double new_cost = cost[i][j] + cost_inc;

                    if (new_cost < cost[i + 1][j]) {
                        cost[i + 1][j] = new_cost;
                        time[i + 1][j] = time[i][j] + seg.time;
                        fuel[i + 1][j] = fuel[i][j] + seg.fuel;
                        mass[i + 1][j] = new_mass;
                        prev_i[i + 1][j] = i;
                        prev_j[i + 1][j] = j;
                        maneuver[i + 1][j] = CLIMB;
                    }
                }
            }

            // Combined maneuver
            if (i < N && j < N) {
                double H2 = H_grid[i + 1];
                double V2 = V_grid[j + 1];
                SegmentData seg = calculate_combined(H1, H2, V1, V2, current_mass, criterion);

                if (seg.valid) {
                    double new_mass = current_mass - seg.fuel;
                    if (new_mass < TU134_MASS * 0.85) continue;

                    double cost_inc = (criterion == MIN_TIME) ? seg.time : seg.fuel;
                    double new_cost = cost[i][j] + cost_inc;

                    if (new_cost < cost[i + 1][j + 1]) {
                        cost[i + 1][j + 1] = new_cost;
                        time[i + 1][j + 1] = time[i][j] + seg.time;
                        fuel[i + 1][j + 1] = fuel[i][j] + seg.fuel;
                        mass[i + 1][j + 1] = new_mass;
                        prev_i[i + 1][j + 1] = i;
                        prev_j[i + 1][j + 1] = j;
                        maneuver[i + 1][j + 1] = COMBINED;
                    }
                }
            }
        }
    }

    if (cost[N][N] >= 1e9) {
        cout << "ERROR: No valid trajectory found!\n";
        cout << "Try increasing thrust settings or using more gradual maneuvers.\n";
        return trajectory;
    }

    int ci = N, cj = N;
    while (ci >= 0 && cj >= 0) {
        trajectory.path.push_back(make_pair(H_grid[ci], V_grid[cj]));
        trajectory.maneuvers.push_back(maneuver[ci][cj]);
        trajectory.mass_points.push_back(mass[ci][cj]);
        trajectory.fuel_points.push_back(fuel[ci][cj]);
        trajectory.time_points.push_back(time[ci][cj]);

        int pi = prev_i[ci][cj];
        int pj = prev_j[ci][cj];

        if (pi == -1) break;
        ci = pi;
        cj = pj;
    }

    reverse(trajectory.path.begin(), trajectory.path.end());
    reverse(trajectory.maneuvers.begin(), trajectory.maneuvers.end());
    reverse(trajectory.mass_points.begin(), trajectory.mass_points.end());
    reverse(trajectory.fuel_points.begin(), trajectory.fuel_points.end());
    reverse(trajectory.time_points.begin(), trajectory.time_points.end());

    for (size_t k = 1; k < trajectory.maneuvers.size(); k++) {
        if (trajectory.maneuvers[k] == ACCELERATION) trajectory.used_acceleration++;
        else if (trajectory.maneuvers[k] == CLIMB) trajectory.used_climb++;
        else if (trajectory.maneuvers[k] == COMBINED) trajectory.used_combined++;
    }

    trajectory.total_time = time[N][N];
    trajectory.total_fuel = fuel[N][N];
    trajectory.avg_climb_rate = (FINAL_ALTITUDE - INITIAL_ALTITUDE) / trajectory.total_time;

    cout << "Optimal trajectory:\n";
    cout << "--------------------------------------------------------\n";
    cout << "Point\tAltitude (m)\tVelocity (km/h)\tManeuver\n";
    cout << "--------------------------------------------------------\n";

    for (size_t k = 0; k < trajectory.path.size(); k++) {
        cout << k + 1 << "\t" << fixed << setprecision(0)
             << setw(8) << trajectory.path[k].first
             << "\t" << setw(12) << trajectory.path[k].second * 3.6;

        if (k > 0) {
            switch (trajectory.maneuvers[k]) {
                case ACCELERATION: cout << "\t[Acceleration]"; break;
                case CLIMB: cout << "\t[Climb]"; break;
                case COMBINED: cout << "\t[Combined]"; break;
            }
        } else {
            cout << "\t[Start]";
        }
        cout << "\n";
    }

    cout << "\n=============================================\n";
    cout << "Maneuvers used:\n";
    cout << "- Acceleration: " << trajectory.used_acceleration << " times\n";
    cout << "- Climb: " << trajectory.used_climb << " times\n";
    cout << "- Combined: " << trajectory.used_combined << " times\n";
    cout << "------------------------------------------------\n";
    cout << fixed << setprecision(1);
    cout << "Total time: " << trajectory.total_time << " s ("
         << trajectory.total_time / 60.0 << " min)\n";
    cout << "Total fuel: " << trajectory.total_fuel << " kg\n";
    cout << "Average climb rate: " << trajectory.avg_climb_rate
         << " m/s (" << trajectory.avg_climb_rate * 60.0 << " m/min)\n";
    cout << "=============================================\n";

    return trajectory;
}

// ========== GNUPLOT VISUALIZATION FUNCTIONS ==========
void create_single_plot(const TrajectoryResult& traj, OptimizationCriterion criterion) {
    string filename, plot_title, traj_name;

    if (criterion == MIN_TIME) {
        filename = "min_time_trajectory.dat";
        plot_title = "Minimum Time Trajectory for TU-134";
        traj_name = "Min Time";
    } else {
        filename = "min_fuel_trajectory.dat";
        plot_title = "Minimum Fuel Trajectory for TU-134";
        traj_name = "Min Fuel";
    }

    // Save trajectory data
    ofstream data_file(filename);
    data_file << "# Velocity(km/h) Altitude(m)\n";

    for (size_t k = 0; k < traj.path.size(); k++) {
        data_file << traj.path[k].second * 3.6 << " "
                 << traj.path[k].first << "\n";
    }
    data_file.close();

    // Create gnuplot script for single trajectory
    ofstream gp("single_trajectory_plot.gp");

    gp << "# TU-134 Trajectory Optimization Plot\n";
    gp << "set terminal wxt size 800,600 enhanced\n";
    gp << "set title '" << plot_title << "'\n";
    gp << "set xlabel 'Velocity (km/h)'\n";
    gp << "set ylabel 'Altitude (m)'\n";
    gp << "set grid\n";
    gp << "set key top left\n";
    gp << "set style line 1 lc rgb '#FF0000' lw 2 pt 7 ps 0.5\n";

    // Add statistics to plot
    gp << "stats '" << filename << "' using 1:2 nooutput\n";
    gp << "set label 1 at graph 0.05,0.90 sprintf('Time: %.1f s (%.1f min)', "
       << traj.total_time << ", " << traj.total_time/60.0 << ")\n";
    gp << "set label 2 at graph 0.05,0.85 sprintf('Fuel: %.1f kg', "
       << traj.total_fuel << ")\n";
    gp << "set label 3 at graph 0.05,0.80 sprintf('Climb rate: %.2f m/s', "
       << traj.avg_climb_rate << ")\n";

    gp << "# Plot H-V diagram\n";
    gp << "plot '" << filename << "' using 1:2 with linespoints ls 1 title '"
       << traj_name << "'\n";

    gp << "pause -1\n";
    gp.close();

    cout << "\n==============================================\n";
    cout << "Gnuplot files created:\n";
    cout << "- " << filename << "\n";
    cout << "- single_trajectory_plot.gp\n";
    cout << "\nTo view the plot, run: gnuplot single_trajectory_plot.gp\n";
    cout << "==============================================\n\n";
}

void create_comparison_plot(const TrajectoryResult& time_traj,
                           const TrajectoryResult& fuel_traj) {
    // Save trajectory data
    ofstream time_file("time_trajectory.dat");
    ofstream fuel_file("fuel_trajectory.dat");

    time_file << "# Velocity(km/h) Altitude(m)\n";
    fuel_file << "# Velocity(km/h) Altitude(m)\n";

    // Save time trajectory data
    for (size_t k = 0; k < time_traj.path.size(); k++) {
        time_file << time_traj.path[k].second * 3.6 << " "
                 << time_traj.path[k].first << "\n";
    }

    // Save fuel trajectory data
    for (size_t k = 0; k < fuel_traj.path.size(); k++) {
        fuel_file << fuel_traj.path[k].second * 3.6 << " "
                 << fuel_traj.path[k].first << "\n";
    }

    time_file.close();
    fuel_file.close();

    // Create gnuplot script for comparison
    ofstream gp("trajectory_comparison_plot.gp");

    gp << "# TU-134 Trajectory Optimization Comparison\n";
    gp << "set terminal wxt size 800,600 enhanced\n";
    gp << "set title 'Flight Trajectory Comparison H(V) for TU-134'\n";
    gp << "set xlabel 'Velocity (km/h)'\n";
    gp << "set ylabel 'Altitude (m)'\n";
    gp << "set grid\n";
    gp << "set key top left\n";
    gp << "set style line 1 lc rgb '#FF0000' lw 2 pt 7 ps 0.5\n";
    gp << "set style line 2 lc rgb '#0000FF' lw 2 pt 7 ps 0.5\n";

    gp << "# Plot H-V diagram\n";
    gp << "plot 'time_trajectory.dat' using 1:2 with linespoints ls 1 title 'Min Time', \\\n";
    gp << "     'fuel_trajectory.dat' using 1:2 with linespoints ls 2 title 'Min Fuel'\n";

    gp << "pause -1\n";
    gp.close();

    cout << "\n==============================================\n";
    cout << "Gnuplot files created:\n";
    cout << "- time_trajectory.dat\n";
    cout << "- fuel_trajectory.dat\n";
    cout << "- trajectory_comparison_plot.gp\n";
    cout << "\nTo view the plot, run: gnuplot trajectory_comparison_plot.gp\n";
    cout << "==============================================\n\n";
}

// ========== MAIN FUNCTION ==========
int main() {
    try {
        cout << "==========================================\n";
        cout << " TU-134 TRAJECTORY OPTIMIZATION\n";
        cout << " GRID-BASED METHOD\n";
        cout << "==========================================\n\n";

        cout << "AIRCRAFT PARAMETERS:\n";
        cout << " Total mass (with fuel): " << TU134_MASS << " kg\n";
        cout << " Wing area: " << TU134_WING_AREA << " m²\n";
        cout << " Thrust: " << TU134_NOMINAL_THRUST/1000 << " kN (2xD-30)\n";
        cout << " Initial altitude: " << INITIAL_ALTITUDE << " m\n";
        cout << " Final altitude: " << FINAL_ALTITUDE << " m\n";
        cout << " Initial velocity: " << INITIAL_VELOCITY*3.6 << " km/h\n";
        cout << " Final velocity: " << FINAL_VELOCITY*3.6 << " km/h\n";
        cout << " Grid size: N = " << N << " points\n\n";

        int choice;
        cout << "Select optimization criterion:\n";
        cout << "1 - Minimum time\n";
        cout << "2 - Minimum fuel consumption\n";
        cout << "3 - Compare both\n";
        cout << "Your choice (1, 2 or 3): ";
        cin >> choice;

        TrajectoryResult time_traj, fuel_traj;

        if (choice == 1) {
            time_traj = solve_trajectory_grid(MIN_TIME);

            char plot_choice;
            cout << "\nDo you want to create a plot for this trajectory? (y/n): ";
            cin >> plot_choice;

            if (plot_choice == 'y' || plot_choice == 'Y') {
                create_single_plot(time_traj, MIN_TIME);
            }
        }
        else if (choice == 2) {
            fuel_traj = solve_trajectory_grid(MIN_FUEL);

            char plot_choice;
            cout << "\nDo you want to create a plot for this trajectory? (y/n): ";
            cin >> plot_choice;

            if (plot_choice == 'y' || plot_choice == 'Y') {
                create_single_plot(fuel_traj, MIN_FUEL);
            }
        }
        else if (choice == 3) {
            cout << "\n=== MINIMUM TIME TRAJECTORY ===\n";
            time_traj = solve_trajectory_grid(MIN_TIME);

            cout << "\n=== MINIMUM FUEL TRAJECTORY ===\n";
            fuel_traj = solve_trajectory_grid(MIN_FUEL);

            // Comparison table
            if (!time_traj.path.empty() && !fuel_traj.path.empty()) {
                cout << "\n=== COMPARISON OF RESULTS ===\n";
                cout << left << setw(30) << "Parameter"
                     << setw(15) << "Min Time"
                     << setw(15) << "Min Fuel"
                     << setw(15) << "Difference" << "\n";
                cout << string(75, '-') << "\n";

                cout << left << setw(30) << "Time (s):"
                     << setw(15) << fixed << setprecision(1) << time_traj.total_time
                     << setw(15) << fuel_traj.total_time
                     << setw(15) << fuel_traj.total_time - time_traj.total_time << "\n";

                cout << left << setw(30) << "Time (min):"
                     << setw(15) << time_traj.total_time/60.0
                     << setw(15) << fuel_traj.total_time/60.0
                     << setw(15) << fixed << setprecision(1)
                     << (fuel_traj.total_time - time_traj.total_time)/60.0 << " min\n";

                cout << left << setw(30) << "Fuel (kg):"
                     << setw(15) << time_traj.total_fuel
                     << setw(15) << fuel_traj.total_fuel
                     << setw(15) << fuel_traj.total_fuel - time_traj.total_fuel << "\n";

                cout << left << setw(30) << "Climb rate (m/s):"
                     << setw(15) << time_traj.avg_climb_rate
                     << setw(15) << fuel_traj.avg_climb_rate
                     << setw(15) << fuel_traj.avg_climb_rate - time_traj.avg_climb_rate << "\n";

                if (time_traj.total_fuel > 0) {
                    double fuel_savings = (time_traj.total_fuel - fuel_traj.total_fuel)
                                        / time_traj.total_fuel * 100;
                    double time_increase = (fuel_traj.total_time - time_traj.total_time)
                                         / time_traj.total_time * 100;

                    cout << string(75, '-') << "\n";
                    cout << left << setw(30) << "Fuel savings (%):"
                         << setw(15) << "0%"
                         << setw(15) << fixed << setprecision(1) << fuel_savings << "%"
                         << setw(15) << fuel_savings << "%\n";

                    cout << left << setw(30) << "Time increase (%):"
                         << setw(15) << "0%"
                         << setw(15) << time_increase << "%"
                         << setw(15) << time_increase << "%\n";

                    if (fuel_traj.total_time > time_traj.total_time) {
                        double efficiency = (time_traj.total_fuel - fuel_traj.total_fuel) /
                                          ((fuel_traj.total_time - time_traj.total_time)/60.0);
                        cout << left << setw(30) << "Efficiency (kg/min):"
                             << setw(15) << "N/A"
                             << setw(15) << fixed << setprecision(2) << efficiency
                             << setw(15) << "kg/min\n";
                    }
                }

                cout << string(75, '=') << "\n";

                // Create comparison plot
                char plot_choice;
                cout << "\nDo you want to create a comparison plot? (y/n): ";
                cin >> plot_choice;

                if (plot_choice == 'y' || plot_choice == 'Y') {
                    create_comparison_plot(time_traj, fuel_traj);
                }
            }
        }
        else {
            cout << "\nInvalid choice!\n";
        }

    } catch (const exception& e) {
        cerr << "\nERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
