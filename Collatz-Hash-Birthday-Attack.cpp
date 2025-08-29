#include<bits/stdc++.h>
#include "Collatz Hash\Collatz-Hash.h"

using namespace std;


// === Return truncated version (simulate collision detection zone) ===
string truncatedHash(const string& fullHash, int bits = 24) {
    int hexLength = bits / 4; // 6 hex digits for 24 bits
    return fullHash.substr(0, hexLength);
}

// === Random string generator ===
string randomString(size_t length) {
    const string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static mt19937 rng(time(nullptr));
    static uniform_int_distribution<> dist(0, charset.size() - 1);

    string result;
    for (size_t i = 0; i < length; ++i)
        result += charset[dist(rng)];

    return result;
}

int main() {
    unordered_map<string, string> hashMap;
    const int maxAttempts = 1e6;
    const int hashBits = 512;  // You can set to 32 or 40, etc.
    const size_t inputLength = 4;

    cout << "Birthday Attack Simulation on 512-bit Hash\n";
    cout << "Truncating hash to " << hashBits << " bits for collision test\n";

    for (int i = 0; i < maxAttempts; ++i) {
        string input = randomString(inputLength);
        string fullHash = Collatz_Hash(input);
        string shortHash = truncatedHash(fullHash, hashBits);

        if (hashMap.count(shortHash) && hashMap[shortHash]!=input) {
            cout << "\nCollision Found after " << i << " attempts!\n";
            cout << " - Input 1: " << hashMap[shortHash] << "\n";
            cout << " - Input 2: " << input << "\n";
            cout << " - Truncated Hash (" << hashBits << " bits): " << shortHash << "\n";
            break;
        }

        hashMap[shortHash] = input;
    }

    return 0;
}
