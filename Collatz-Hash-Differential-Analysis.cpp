#include <bits/stdc++.h>
#include "Collatz Hash\Collatz-Hash.h"
using namespace std;

// ===== Your Hash Function Declaration =====
// Make sure to define this somewhere else or link it during compile.
// It should accept std::string input and return std::string (hex digest).


// ===== Utilities =====
using Bytes = vector<uint8_t>;

static inline Bytes xor_bytes(const Bytes &a, const Bytes &b) {
    Bytes r(a.size());
    for (size_t i = 0; i < a.size(); ++i) r[i] = a[i] ^ b[i];
    return r;
}

static inline string to_hex(const Bytes &v) {
    static const char *hexd = "0123456789abcdef";
    string s; s.resize(v.size() * 2);
    for (size_t i = 0; i < v.size(); ++i) {
        s[2*i]   = hexd[v[i] >> 4];
        s[2*i+1] = hexd[v[i] & 0xF];
    }
    return s;
}

static inline Bytes from_hex(const string &h) {
    auto hexval = [](char c)->int {
        if ('0'<=c && c<='9') return c-'0';
        if ('a'<=c && c<='f') return c-'a'+10;
        if ('A'<=c && c<='F') return c-'A'+10;
        return -1;
    };
    if (h.size() % 2) throw runtime_error("Hex length must be even");
    Bytes out(h.size()/2);
    for (size_t i = 0; i < out.size(); ++i) {
        int hi = hexval(h[2*i]), lo = hexval(h[2*i+1]);
        if (hi<0 || lo<0) throw runtime_error("Invalid hex digit");
        out[i] = (uint8_t)((hi<<4)|lo);
    }
    return out;
}

static inline void flip_bit(Bytes &v, size_t bit_index) {
    size_t byte = bit_index / 8;
    size_t bit  = bit_index % 8;
    v[byte] ^= (uint8_t)(1u << bit);
}

static string bytes_to_string(const Bytes &data) {
    return string(data.begin(), data.end());
}

// ===== Hash Adapter =====
// Converts input bytes -> string -> calls Collatz_Hash_6 -> converts hex output -> bytes
using HashFunc = function<string(const Bytes &)>;
static Bytes hash_to_bytes(const HashFunc &H, const Bytes &msg) {
    string hex_out = H(msg);
    return from_hex(hex_out);
}

// ===== Differential Analysis =====
struct DifferentialResult {
    unordered_map<string, uint64_t> diff_hist;
    uint64_t samples = 0;
};

static DifferentialResult differential_distribution(
    const HashFunc &H, size_t in_len, const Bytes &delta, uint64_t samples, uint64_t seed = 0xC0FFEE)
{
    mt19937_64 rng(seed);
    uniform_int_distribution<uint32_t> dist(0, 255);
    DifferentialResult res; res.samples = samples;

    Bytes m(in_len), m2;
    for (uint64_t t = 0; t < samples; ++t) {
        for (size_t i = 0; i < in_len; ++i) m[i] = (uint8_t)dist(rng);
        m2 = xor_bytes(m, delta);
        Bytes h1 = hash_to_bytes(H, m);
        Bytes h2 = hash_to_bytes(H, m2);
        Bytes dy = xor_bytes(h1, h2);
        string key = to_hex(dy);
        ++res.diff_hist[key];
    }
    return res;
}

// ===== Avalanche Analysis =====
struct AvalancheStats {
    vector<vector<uint64_t>> flips;
    uint64_t trials_per_input_bit = 0;
    size_t in_bits = 0, out_bits = 0;
};

static AvalancheStats avalanche_matrix(
    const HashFunc &H, size_t in_len, uint64_t trials, uint64_t seed = 0xBEEFBABE)
{
    mt19937_64 rng(seed);
    uniform_int_distribution<uint32_t> dist(0, 255);

    Bytes probe(in_len, 0);
    size_t out_bits = hash_to_bytes(H, probe).size() * 8;
    size_t in_bits  = in_len * 8;

    AvalancheStats st;
    st.in_bits = in_bits;
    st.out_bits = out_bits;
    st.trials_per_input_bit = trials;
    st.flips.assign(in_bits, vector<uint64_t>(out_bits, 0));

    for (size_t b = 0; b < in_bits; ++b) {
        for (uint64_t t = 0; t < trials; ++t) {
            Bytes m(in_len);
            for (size_t i = 0; i < in_len; ++i) m[i] = (uint8_t)dist(rng);
            Bytes h1 = hash_to_bytes(H, m);
            Bytes m2 = m; flip_bit(m2, b);
            Bytes h2 = hash_to_bytes(H, m2);
            for (size_t ob = 0; ob < out_bits; ++ob) {
                size_t byte = ob / 8, bit = ob % 8;
                if ((h1[byte] ^ h2[byte]) & (1u << bit)) st.flips[b][ob]++;
            }
        }
    }
    return st;
}

// ===== Main =====
int main() {
    size_t in_len = 64;         // input length in bytes
    size_t samples = 100000;      // # samples for differential analysis
    size_t avalanche_trials = 100; // # trials for avalanche analysis

    // Create delta = 1-bit difference at byte 0
    Bytes delta(in_len, 0);
    delta[0] = 0x01;

    // Wrap Collatz_Hash_6 into our HashFunc
    HashFunc H = [](const Bytes &m) {
        return Collatz_Hash(bytes_to_string(m));
    };

    // Differential distribution
    auto diff = differential_distribution(H, in_len, delta, samples);
    cout << "Top differences (Δ=1 at byte 0):\n";
    int count = 0;
    for (auto &kv : diff.diff_hist) {
        cout << kv.first << " : " << kv.second << "\n";
        if (++count >= 10) break;
    }

    // Avalanche analysis
    auto av = avalanche_matrix(H, in_len, avalanche_trials);
    cout << "\nAvalanche matrix summary:\n";
    for (size_t i = 0; i < min<size_t>(8, av.in_bits); ++i) {
        uint64_t sum = 0;
        for (size_t j = 0; j < av.out_bits; ++j) sum += av.flips[i][j];
        double p = (double)sum / (av.trials_per_input_bit * av.out_bits);
        cout << "Input bit " << i << " -> Avg flip prob = " << fixed << setprecision(3) << p << "\n";
    }
    return 0;
}


