#include <iostream>
#include <windows.h>

using namespace std;

int main()
{
    char str[] = "hello";

    int exit = 0;
    string input;

    cout<<GetCurrentProcessId()<<endl<<str<<endl;

    while (!exit)
    {
        cin>>input;
        if (input == "out")
        {
            cout<<str<<endl;
        }
        if (input == "exit") {
            exit = 1;
        }
    }

    return 0;
}
