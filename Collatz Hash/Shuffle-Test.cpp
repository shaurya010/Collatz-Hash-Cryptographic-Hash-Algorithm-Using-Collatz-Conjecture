#include<bits/stdc++.h>
#include"Shuffle.h"

using namespace std;



int main() {
    string key = "b";
    vector<int> shuffled = deterministic_shuffle(key,32);

    cout << "Deterministic shuffle for key \"" << key << "\":" << std::endl;
    for (int x : shuffled) {
        cout << x << " ";
    }
    cout << endl;
    return 0;
}