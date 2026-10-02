#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    int x = 10;
    x++;
    cout << "argc = " << argc << '\n';
    cout << "argv[0] = " << argv[0] << '\n';
    cout << "x = " << x << '\n';

    return 0;
}