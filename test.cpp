
#include <iostream>
using namespace std;
 
void add(int a, int b)
{
  cout << "sum = " << (a + b);
}
 
void add(int a, int b, int c)
{
    // sua function thu 2
    cout << endl << "sum = " << (a + b + c);
}
 
// Driver code
int main()
{
    add(10, 2);
    add(5, 6, 4);
 
    return 0;
}