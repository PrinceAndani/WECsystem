#include <iostream>
#include <unistd.h>

using namespace std;

int main() {
    int x = 10;
    x = x + 1;
    cout << "x = " << x << '\n';
    sleep(10);
    return 0;
}
