#include <iostream>
#include <iomanip>

using namespace std;

double D(double ro, double V, double S, double C) {
    return 0.5*ro*V*V*S*C;
}

void Check(double *x) {
    while (*x<0){
        cout << "Something went wrong. The value must be positive. Please try again: ";
        cin >> *x;
    }
}

int main()
{
    int Switch = 0, mode;
    while (Switch != 1) {
        double S, V, ro, C;
        cout << "Please enter the parameters of the aircraft whose aerodynamic drag you want to calculate. When entering, use the SI." << endl << "Wing area S = ";
        cin >> S;
        Check (&S);
        cout << "Flow speed V = ";
        cin >> V;
        Check (&V);
        cout << "Air density p = ";
        cin >> ro;
        Check (&ro);
        cout << "Drag coefficient C_D = ";
        cin >> C;
        Check (&C);
        cout << fixed << setprecision(3);
        cout << "Then the aerodynamic drag would be D = " << D(ro, V, S, C) << " N" << endl << endl << "Do you want to calculate the aerodynamic drag for another aircraft? If yes, enter 1, if not, enter 0: ";
        cin >> mode;
        while ((mode != 0)&&(mode != 1)) {
            cout << endl << "Something went wrong. Please try again." << endl << "Do you want to calculate the aerodynamic drag for another aircraft? If yes, enter 1, if not, enter 0: ";
            cin >> mode;
        }
        switch (mode) {
            case 1:
                Switch = 0;
                cout << endl;
                break;
            case 0:
                Switch = 1;
                break;
        }
    }

    cout << endl << "Thank you for using this program!" << endl << "Have a nice day!";

    return 0;
}
