#include <iostream>
#include <iomanip>
#include <cmath>
//cmath нужно или нет?

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
        double a, h;
        cout << "To calculate the climb time of the aircraft, please specify its vertical acceleration and the altitude you want to reach. When entering, use the SI." << endl << "Vertical acceleration a_y = ";
        cin >> a;
        Check (&a);
        cout << "Altitude h = ";
        cin >> h;
        Check (&h);
        cout << fixed << setprecision(3);
        cout << "Then the climb time would be t = " << sqrt((2*h)/a) << " s" << endl << endl << "Do you want to calculate the climb time for another aircraft? If yes, enter 1, if not, enter 0: ";
        cin >> mode;
        while ((mode != 0)&&(mode != 1)) {
            cout << endl << "Something went wrong. Please try again." << endl << "Do you want to calculate the climb time for another aircraft? If yes, enter 1, if not, enter 0: ";
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
