#include <bits/stdc++.h>
#include "Collatz-Hash.h"



int main()
{
    cout<<"Type Input: ";

    string input;
    getline(cin, input);

    string output=Collatz_Hash(input);
    
    return 0;
}

