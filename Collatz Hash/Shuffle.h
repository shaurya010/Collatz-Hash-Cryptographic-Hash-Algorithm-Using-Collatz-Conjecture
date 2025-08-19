#include <iostream>
#include <vector>
#include <string>

using namespace std;

// Derive step (odd, 1–63) and offset (0–63) from key characters
void derive_step_and_offset(const string& key, int& s, int& o, int UpperLimit) {
    int sum1 = 0, sum2 = 0;
    for (size_t i = 0; i < key.size(); ++i) {
        if (i % 2 == 0)
            sum1 += key[i];
        else
            sum2 += key[i];
    }

    // Ensure s is odd and between 1 and 63
    s = (sum1 % UpperLimit/2) * 2 + 1;  // gives odd number from 1 to 63
    o = sum2 % UpperLimit;            // offset from 0 to 63
}

// Perform modular shuffle of numbers 1 to 64 using s and o
vector<int> deterministic_shuffle(const string& key, int UpperLimit) {
    
    int s, o;

    derive_step_and_offset(key, s, o, UpperLimit );

    vector<int> result;
    for (int k = 0; k < UpperLimit; ++k) {
        int val = ((o + k * s) % UpperLimit) ;  // map to 1–64
        result.push_back(val);
    }
    return result;
}


