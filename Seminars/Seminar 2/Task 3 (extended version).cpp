#include <iostream>
#include <iomanip>

using namespace std;

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
        const double g = 9.81;
        double m, L, D, T;
        cout << "Please enter the parameters of the aircraft whose along heading and vertical accelerations you want to calculate. When entering, use the SI." << endl << "Aircraft mass m = ";
        cin >> m;
        Check (&m);
        cout << "Lift force L = ";
        cin >> L;
        cout << "Aerodynamic drag D = ";
        cin >> D;
        Check (&D);
        cout << "Thrust T = ";
        cin >> T;
        cout << fixed << setprecision(3);
        cout << "Then the along heading acceleration would be a = " << (T - D)/m << " m/s^2, and the vertical acceleration would be a_y = " << (L - m*g)/m << " m/s^2" << endl << endl << "Do you want to calculate these accelerations for another aircraft? If yes, enter 1, if not, enter 0: ";
        cin >> mode;
        while ((mode != 0)&&(mode != 1)) {
            cout << endl << "Something went wrong. Please try again." << endl << "Do you want to calculate the along heading and vertical accelerations for another aircraft? If yes, enter 1, if not, enter 0: ";
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
