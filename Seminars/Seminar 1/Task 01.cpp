#include <iostream>
#include <iomanip>

using namespace std;

int main()
{
    double S, V, ro, C;
    cout << "Please enter the parameters of the aircraft whose lift force you want to calculate. When entering, use the SI." << endl << "Wing area S = ";
    cin >> S;
    cout << "Flow speed V = ";
    cin >> V;
    cout << "Air density p = ";
    cin >> ro;
    cout << "Lift coefficient C_L = ";
    cin >> C;
    cout << fixed << setprecision(3);
    cout << "Then the lift force would be L = " << 0.5*ro*V*V*S*C << " N" << endl << "Thank you for using this program!" << endl << "Have a nice day!";

    return 0;
}
