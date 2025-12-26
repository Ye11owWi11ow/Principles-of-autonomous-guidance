#include <iostream>
#include <iomanip>

using namespace std;

double D(double ro, double V, double S, double C) {
    return 0.5*ro*V*V*S*C;
}

int main()
{
    double S, V, ro, C;
    cout << "Please enter the parameters of the aircraft whose aerodynamic drag you want to calculate. When entering, use the SI." << endl << "Wing area S = ";
    cin >> S;
    cout << "Flow speed V = ";
    cin >> V;
    cout << "Air density p = ";
    cin >> ro;
    cout << "Drag coefficient C_D = ";
    cin >> C;
    cout << fixed << setprecision(3);
    cout << "Then the aerodynamic drag would be D = " << D(ro, V, S, C) << " N" << endl << "Thank you for using this program!" << endl << "Have a nice day!";

    return 0;
}
