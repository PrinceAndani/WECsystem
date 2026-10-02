#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    int x = 10;
    cout << "\nFirst :" << x << '\n';
    cout << "X + 1: " <<  x + 1 << endl;
    x += 2;
    cout << "Second :" << x << '\n';
    cout << "argc = " << argc << '\n';
    cout << "argv[0] = " << argv[0] << '\n';

    return 0;
}