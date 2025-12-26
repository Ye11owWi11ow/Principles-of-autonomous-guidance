#include <iostream>
#include <iomanip>

using namespace std;

int main()
{
    const double g = 9.81;
    double m, L, D, T;
    cout << "Please enter the parameters of the aircraft whose along heading and vertical accelerations you want to calculate. When entering, use the SI." << endl << "Aircraft mass m = ";
    cin >> m;
    cout << "Lift force L = ";
    cin >> L;
    cout << "Aerodynamic drag D = ";
    cin >> D;
    cout << "Thrust T = ";
    cin >> T;
    cout << fixed << setprecision(3);
    cout << "Then the along heading acceleration would be a = " << (T - D)/m << " m/s^2, and the vertical acceleration would be a_y = " << (L - m*g)/m << " m/s^2";
    cout << endl << "Thank you for using this program!" << endl << "Have a nice day!";

    return 0;
}
