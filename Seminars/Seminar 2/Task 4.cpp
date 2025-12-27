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
    double a, h;
    cout << "To calculate the climb time of the aircraft, please specify its vertical acceleration and the altitude you want to reach. When entering, use the SI." << endl << "Vertical acceleration a_y = ";
    cin >> a;
    Check (&a);
    cout << "Altitude h = ";
    cin >> h;
    Check (&h);
    cout << fixed << setprecision(3);
    cout << "Then the climb time would be t = " << sqrt((2*h)/a) << " s";
    cout << endl << "Thank you for using this program!" << endl << "Have a nice day!";

    return 0;
}
