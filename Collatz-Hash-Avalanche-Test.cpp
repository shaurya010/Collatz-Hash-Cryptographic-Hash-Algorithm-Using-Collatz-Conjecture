#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <bitset>
#include <numeric>
#include <cmath>
#include "Collatz Hash\Collatz-Hash.h"

using namespace std;


// Compute Hamming distance between two byte vectors
int hammingDistance(const string& a, const string& b)
{
    int count = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        count += bitset<8>(a[i] ^ b[i]).count();
    }
    return count;
}

// Generate a random 256-bit string (32 bytes)
string generateRandomString(mt19937_64& rng) {
    string result(32, '\0');
    uniform_int_distribution<uint8_t> dist(0, 255);
    for (char& c : result) {
        c = static_cast<char>(dist(rng));
    }
    return result;
}

// Flip one random bit in the string
string flipOneBit(const string& input, mt19937_64& rng) {
    string modified = input;
    uniform_int_distribution<int> byteDist(0, input.size() - 1);
    uniform_int_distribution<int> bitDist(0, 7);
    int byteIndex = byteDist(rng);
    int bitIndex = bitDist(rng);
    modified[byteIndex] ^= (1 << bitIndex);
    return modified;
}

int main() {
    const int N = 10000;
    mt19937_64 rng(random_device{}());

    vector<int> distances;
    distances.reserve(N);

    for (int i = 0; i < N; ++i) {
        string original = generateRandomString(rng);
        string modified = flipOneBit(original, rng);

        cout<<"Input: "<<original<<endl;
        cout<<"Modified Input: "<<modified<<endl;

        auto hash1 = Collatz_Hash(original);
        auto hash2 = Collatz_Hash(modified);

        distances.push_back(hammingDistance(hash1, hash2));
    }

    double sum = accumulate(distances.begin(), distances.end(), 0.0);
    double mean = sum / N;

    double variance = 0;
    for (int d : distances) {
        variance += (d - mean) * (d - mean);
    }
    variance /= N;
    double stddev = sqrt(variance);

    cout << "Mean Hamming distance: " << mean << " bits\n";
    cout << "Standard deviation: " << stddev << " bits\n";

    return 0;
}
//9552f349c26665fb01e314bb4da2228ba36de2a0968f92b9d8fa3960c48fbc044cf333890d58428c606f066d6a850c093a4b0184ae6037ad4cf333890d58428c49590dd061ea274f7f7d1a6074ff8181
