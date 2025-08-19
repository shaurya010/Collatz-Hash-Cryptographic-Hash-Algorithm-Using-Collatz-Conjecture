#include <bits/stdc++.h>
#include "Shuffle.h"

using namespace std;

#define ull unsigned long long

#define CollatzHash_512_INPUT_REPRESENTATION_LENGTH 128
#define BLOCK_SIZE 1024

#define BUFFER_COUNT 10
#define WORD_LENGTH 16
#define WORD_COUNT 64
#define X_COUNT 32

// #define f(x, y, z) ((x ^ y ^ z))

// #define g(x, y, z) ((x & y)^(x & z)^(y & z))

#define twoscompliment(x) ((~x) + 1)

uint16_t rotr16(uint16_t n, uint16_t c)
{
    const unsigned int mask = (CHAR_BIT * sizeof(n) - 1);
    c &= mask;
    return (n >> c) | (n << (twoscompliment(c) & mask));
}

uint64_t rotl64(uint64_t x, unsigned int n)
{
    return (x << n) | (x >> (64 - n));
}

// Rotate right for 64-bit
uint64_t rotr64(uint64_t x, unsigned int n)
{
    return (x >> n) | (x << (64 - n));
}

uint16_t f1(uint16_t x, uint16_t y, uint16_t z)
{
    // addition + rotation + xor: non-linear and mixes bits
    uint16_t t = (uint16_t)((uint32_t)x + (uint32_t)y + ((uint32_t)z << 1)); // mod 2^16
    // rotate t by 7 and XOR with x -> breaks linearity
    uint16_t r = rotr16(t, 7);
    return (uint16_t)((r ^ x) + (y ^ rotr16(z, 3)));
}

uint16_t g1(uint16_t x, uint16_t y, uint16_t z)
{
    // something similar to SHA-1's choose/majority but with additions
    uint16_t choose = (x & y) ^ ((~x) & z);
    uint16_t maj = (x & y) ^ (x & z) ^ (y & z);
    uint16_t t = (uint16_t)(((uint32_t)choose + (uint32_t)maj + rotr16(x ^ y, 5)) & 0xFFFF);
    return (uint16_t)(t ^ rotr16(z, 11));
}

using namespace std;

void initialiseBuffers(vector<ull> &buffers)
{

    buffers = {
        0x6a09e667f3bcc908, 0xbb67ae8584caa73b, 0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,
        0x510e527fade682d1, 0x9b05688c2b3e6c1f, 0x1f83d9abfb41bd6b, 0x5be0cd19137e2179,
        0x0000000000000000, 0x0000000000000000};
}

ull combine16to64(uint16_t parts[4])
{

    return ((uint64_t)parts[0] << 48) |
           ((uint64_t)parts[1] << 32) |
           ((uint64_t)parts[2] << 16) |
           ((uint64_t)parts[3]);
}

string toHex64(uint64_t value)
{
    ostringstream oss;
    oss << hex << setw(16) << setfill('0') << nouppercase << value;
    return oss.str();
}

string toHexString(uint64_t value)
{
    stringstream ss;
    ss << setfill('0') << setw(16) << hex << nouppercase << value;
    return ss.str();
}

string CollatzHash512Padding(string input)
{

    string finalPlainText = "";

    for (int i = 0; i < input.size(); ++i)
    {
        finalPlainText += bitset<8>((int)input[i]).to_string();
    }

    finalPlainText += '1';

    int plainTextSize = (int)input.size() * 8;
    int numberOfZeros = BLOCK_SIZE - ((plainTextSize + CollatzHash_512_INPUT_REPRESENTATION_LENGTH + 1) % BLOCK_SIZE);

    while (numberOfZeros--)
    {
        finalPlainText += '0';
    }

    finalPlainText += bitset<CollatzHash_512_INPUT_REPRESENTATION_LENGTH>(plainTextSize).to_string();

    cout << "Plain text length = " << plainTextSize << endl;
    cout << "Plain text length after padding = " << finalPlainText.length() << endl
         << endl;

    return finalPlainText;
}

ull getUllFromString(string str)
{
    bitset<WORD_LENGTH> word(str);
    return word.to_ullong();
}

string Collatz_Hash_5(string inp)
{

    vector<ull> buffers(BUFFER_COUNT);
    initialiseBuffers(buffers);

    string input = inp;

    string key = input;
    vector<int> shuffled64 = deterministic_shuffle(key, 64);
    vector<int> shuffled32 = deterministic_shuffle(key, 32);
    vector<int> shuffled10 = deterministic_shuffle(key, 10);

    string paddedInput = CollatzHash512Padding(input);

    cout << "Padded Input:" << " " << paddedInput << endl
         << endl;

    for (int i = 0; i < paddedInput.size(); i += BLOCK_SIZE)
    {
        string currentBlock = paddedInput.substr(i, BLOCK_SIZE);

        vector<uint16_t> w(WORD_COUNT);

        for (int j = 0; j < WORD_COUNT; ++j)
        {
            w[j] = getUllFromString(currentBlock.substr(j, WORD_LENGTH));
        }

        // for (int j = 0; j < 64; ++j)
        // {
        //     cout << "Word-" << j << " : " << w[j] << " ";
        // }
        // cout << endl;

        for (int j = 0; j < 64; ++j)
        {

            if (w[j] == 0)
            {

                uint16_t sigma1 = (rotr16(w[abs(j - 15)], 1)) ^ (rotr16(w[abs(j - 15)], 8)) ^ (w[abs(j - 15)] >> 7);
                uint16_t sigma2 = (rotr16(w[abs(j - 2)], 19)) ^ (rotr16(w[abs(j - 2)], 61)) ^ (w[abs(j - 2)] >> 6);

                w[j] = w[abs(j - 16)] + sigma1 + w[abs(j - 7)] + sigma2;
            }
        }

        ull a = buffers[0], b = buffers[1], c = buffers[2], d = buffers[3];
        ull e = buffers[4], f = buffers[5], g = buffers[6], h = buffers[7];

        ull s, p;

        uint16_t a_parts[4], b_parts[4], c_parts[4], d_parts[4];
        uint16_t e_parts[4], f_parts[4], g_parts[4], h_parts[4];

        uint16_t s_parts[4];
        uint16_t p_parts[4];

        for (int i = 0; i < 4; i++)
        {

            a_parts[i] = (a >> (48 - i * 16)) & 0xFFFF;
            b_parts[i] = (b >> (48 - i * 16)) & 0xFFFF;
            c_parts[i] = (c >> (48 - i * 16)) & 0xFFFF;
            d_parts[i] = (d >> (48 - i * 16)) & 0xFFFF;
            e_parts[i] = (e >> (48 - i * 16)) & 0xFFFF;
            f_parts[i] = (f >> (48 - i * 16)) & 0xFFFF;
            g_parts[i] = (g >> (48 - i * 16)) & 0xFFFF;
            h_parts[i] = (h >> (48 - i * 16)) & 0xFFFF;
        }

        vector<uint16_t> X(X_COUNT, 0);

        X[0] = a_parts[0];
        X[1] = a_parts[1];
        X[2] = a_parts[2];
        X[3] = a_parts[3];

        X[4] = b_parts[0];
        X[5] = b_parts[1];
        X[6] = b_parts[2];
        X[7] = b_parts[3];

        X[8] = c_parts[0];
        X[9] = c_parts[1];
        X[10] = c_parts[2];
        X[11] = c_parts[3];

        X[12] = d_parts[0];
        X[13] = d_parts[1];
        X[14] = d_parts[2];
        X[15] = d_parts[3];

        X[16] = e_parts[0];
        X[17] = e_parts[1];
        X[18] = e_parts[2];
        X[19] = e_parts[3];

        X[20] = f_parts[0];
        X[21] = f_parts[1];
        X[22] = f_parts[2];
        X[23] = f_parts[3];

        X[24] = g_parts[0];
        X[25] = g_parts[1];
        X[26] = g_parts[2];
        X[27] = g_parts[3];

        X[28] = h_parts[0];
        X[29] = h_parts[1];
        X[30] = h_parts[2];
        X[31] = h_parts[3];

        for (int i = 0; i < 32; i++)
        {
            uint16_t left = X[(i + 31) % 32];
            uint16_t right = X[(i + 1) % 32];
            // rotation offsets vary to avoid symmetry
            uint16_t mix = (uint16_t)((left + rotr16(right, (i & 3) + 3)) & 0xFFFF);
            // XOR + add for nonlinearity
            X[i] = (uint16_t)(X[i] ^ rotr16(mix, (i & 0xF)));
        }

        for (int i = 0; i <= 32; i++)
        {

            int x = X[i];

            // cout << "We take 'x' as X[" << i << "] value is :" << x << endl;

            while (x >= 2) // Chnage from CH-1.
            {

                if (x % 2 == 0)
                {

                    a_parts[0] = f1(a_parts[0], w[shuffled64[0]], rotr16(w[shuffled64[32]], x % 9));
                    a_parts[1] = f1(a_parts[1], w[shuffled64[1]], rotr16(w[shuffled64[33]], x % 9));
                    a_parts[2] = f1(a_parts[2], w[shuffled64[2]], rotr16(w[shuffled64[34]], x % 9));
                    a_parts[3] = f1(a_parts[3], w[shuffled64[3]], rotr16(w[shuffled64[35]], x % 9));

                    b_parts[0] = f1(b_parts[0], w[shuffled64[4]], rotr16(w[shuffled64[36]], x % 9));
                    b_parts[1] = f1(b_parts[1], w[shuffled64[5]], rotr16(w[shuffled64[37]], x % 9));
                    b_parts[2] = f1(b_parts[2], w[shuffled64[6]], rotr16(w[shuffled64[38]], x % 9));
                    b_parts[3] = f1(b_parts[3], w[shuffled64[7]], rotr16(w[shuffled64[39]], x % 9));

                    c_parts[0] = f1(c_parts[0], w[shuffled64[8]], rotr16(w[shuffled64[40]], x % 9));
                    c_parts[1] = f1(c_parts[1], w[shuffled64[9]], rotr16(w[shuffled64[41]], x % 9));
                    c_parts[2] = f1(c_parts[2], w[shuffled64[10]], rotr16(w[shuffled64[42]], x % 9));
                    c_parts[3] = f1(c_parts[3], w[shuffled64[11]], rotr16(w[shuffled64[43]], x % 9));

                    d_parts[0] = f1(d_parts[0], w[shuffled64[12]], rotr16(w[shuffled64[44]], x % 9));
                    d_parts[1] = f1(d_parts[1], w[shuffled64[13]], rotr16(w[shuffled64[45]], x % 9));
                    d_parts[2] = f1(d_parts[2], w[shuffled64[14]], rotr16(w[shuffled64[46]], x % 9));
                    d_parts[3] = f1(d_parts[3], w[shuffled64[15]], rotr16(w[shuffled64[47]], x % 9));

                    e_parts[0] = f1(e_parts[0], w[shuffled64[16]], rotr16(w[shuffled64[48]], x % 9));
                    e_parts[1] = f1(e_parts[1], w[shuffled64[17]], rotr16(w[shuffled64[49]], x % 9));
                    e_parts[2] = f1(e_parts[2], w[shuffled64[18]], rotr16(w[shuffled64[50]], x % 9));
                    e_parts[3] = f1(e_parts[3], w[shuffled64[19]], rotr16(w[shuffled64[51]], x % 9));

                    f_parts[0] = f1(f_parts[0], w[shuffled64[20]], rotr16(w[shuffled64[52]], x % 9));
                    f_parts[1] = f1(f_parts[1], w[shuffled64[21]], rotr16(w[shuffled64[53]], x % 9));
                    f_parts[2] = f1(f_parts[2], w[shuffled64[22]], rotr16(w[shuffled64[54]], x % 9));
                    f_parts[3] = f1(f_parts[3], w[shuffled64[23]], rotr16(w[shuffled64[55]], x % 9));

                    g_parts[0] = f1(g_parts[0], w[shuffled64[24]], rotr16(w[shuffled64[56]], x % 9));
                    g_parts[1] = f1(g_parts[1], w[shuffled64[25]], rotr16(w[shuffled64[57]], x % 9));
                    g_parts[2] = f1(g_parts[2], w[shuffled64[26]], rotr16(w[shuffled64[58]], x % 9 ));
                    g_parts[3] = f1(g_parts[3], w[shuffled64[27]], rotr16(w[shuffled64[59]], x % 9));

                    h_parts[0] = f1(h_parts[0], w[shuffled64[28]], rotr16(w[shuffled64[60]], x % 9));
                    h_parts[1] = f1(h_parts[1], w[shuffled64[29]], rotr16(w[shuffled64[61]], x % 9));
                    h_parts[2] = f1(h_parts[2], w[shuffled64[30]], rotr16(w[shuffled64[62]], x % 9));
                    h_parts[3] = f1(h_parts[3], w[shuffled64[31]], rotr16(w[shuffled64[63]], x % 9));

                    x = x / 2;
                }

                else
                {

                    a_parts[0] = g1(a_parts[0], w[shuffled64[0]], rotr16(w[shuffled64[32]], x % 9));
                    a_parts[1] = g1(a_parts[1], w[shuffled64[1]], rotr16(w[shuffled64[33]], x % 9));
                    a_parts[2] = g1(a_parts[2], w[shuffled64[2]], rotr16(w[shuffled64[34]], x % 9));
                    a_parts[3] = g1(a_parts[3], w[shuffled64[3]], rotr16(w[shuffled64[35]], x % 9));

                    b_parts[0] = g1(b_parts[0], w[shuffled64[4]], rotr16(w[shuffled64[36]], x % 9));
                    b_parts[1] = g1(b_parts[1], w[shuffled64[5]], rotr16(w[shuffled64[37]], x % 9));
                    b_parts[2] = g1(b_parts[2], w[shuffled64[6]], rotr16(w[shuffled64[38]], x % 9));
                    b_parts[3] = g1(b_parts[3], w[shuffled64[7]], rotr16(w[shuffled64[39]], x % 9));

                    c_parts[0] = g1(c_parts[0], w[shuffled64[8]], rotr16(w[shuffled64[40]], x % 9));
                    c_parts[1] = g1(c_parts[1], w[shuffled64[9]], rotr16(w[shuffled64[41]], x % 9));
                    c_parts[2] = g1(c_parts[2], w[shuffled64[10]], rotr16(w[shuffled64[42]], x % 9));
                    c_parts[3] = g1(c_parts[3], w[shuffled64[11]], rotr16(w[shuffled64[43]], x % 9));

                    d_parts[0] = g1(d_parts[0], w[shuffled64[12]], rotr16(w[shuffled64[44]], x % 9));
                    d_parts[1] = g1(d_parts[1], w[shuffled64[13]], rotr16(w[shuffled64[45]], x % 9));
                    d_parts[2] = g1(d_parts[2], w[shuffled64[14]], rotr16(w[shuffled64[46]], x % 9));
                    d_parts[3] = g1(d_parts[3], w[shuffled64[15]], rotr16(w[shuffled64[47]], x % 9));

                    e_parts[0] = g1(e_parts[0], w[shuffled64[16]], rotr16(w[shuffled64[48]], x % 9));
                    e_parts[1] = g1(e_parts[1], w[shuffled64[17]], rotr16(w[shuffled64[49]], x % 9));
                    e_parts[2] = g1(e_parts[2], w[shuffled64[18]], rotr16(w[shuffled64[50]], x % 9));
                    e_parts[3] = g1(e_parts[3], w[shuffled64[19]], rotr16(w[shuffled64[51]], x % 9));

                    f_parts[0] = g1(f_parts[0], w[shuffled64[20]], rotr16(w[shuffled64[52]], x % 9));
                    f_parts[1] = g1(f_parts[1], w[shuffled64[21]], rotr16(w[shuffled64[53]], x % 9));
                    f_parts[2] = g1(f_parts[2], w[shuffled64[22]], rotr16(w[shuffled64[54]], x % 9));
                    f_parts[3] = g1(f_parts[3], w[shuffled64[23]], rotr16(w[shuffled64[55]], x % 9));

                    g_parts[0] = g1(g_parts[0], w[shuffled64[24]], rotr16(w[shuffled64[56]], x % 9));
                    g_parts[1] = g1(g_parts[1], w[shuffled64[25]], rotr16(w[shuffled64[57]], x % 9));
                    g_parts[2] = g1(g_parts[2], w[shuffled64[26]], rotr16(w[shuffled64[58]], x % 9));
                    g_parts[3] = g1(g_parts[3], w[shuffled64[27]], rotr16(w[shuffled64[59]], x % 9));

                    h_parts[0] = g1(h_parts[0], w[shuffled64[28]], rotr16(w[shuffled64[60]], x % 9));
                    h_parts[1] = g1(h_parts[1], w[shuffled64[29]], rotr16(w[shuffled64[61]], x % 9));
                    h_parts[2] = g1(h_parts[2], w[shuffled64[30]], rotr16(w[shuffled64[62]], x % 9));
                    h_parts[3] = g1(h_parts[3], w[shuffled64[31]], rotr16(w[shuffled64[63]], x % 9));

                    x = ((3 * x) + 1) / 2;
                }
            }


            s_parts[0] = f1(a_parts[0], b_parts[0], rotr16(c_parts[0], 2));
            s_parts[1] = f1(a_parts[1], b_parts[1], rotr16(c_parts[1], 3));
            s_parts[2] = f1(a_parts[2], b_parts[2], rotr16(c_parts[2], 5));
            s_parts[3] = f1(a_parts[3], b_parts[3], rotr16(c_parts[3], 7));

            p_parts[0] = g1(f_parts[0], g_parts[0], rotr16(h_parts[0], 2));
            p_parts[1] = g1(f_parts[1], g_parts[1], rotr16(h_parts[1], 3));
            p_parts[2] = g1(f_parts[2], g_parts[2], rotr16(h_parts[2], 5));
            p_parts[3] = g1(f_parts[3], g_parts[3], rotr16(h_parts[3], 7));

            a = combine16to64(a_parts);
            b = combine16to64(b_parts);
            c = combine16to64(c_parts);
            d = combine16to64(d_parts);
            e = combine16to64(e_parts);
            f = combine16to64(f_parts);
            g = combine16to64(g_parts);
            h = combine16to64(h_parts);

            s = combine16to64(s_parts);
            p = combine16to64(p_parts);

            buffers[0] += a;
            buffers[1] += b;
            buffers[2] += c;
            buffers[3] += d;
            buffers[4] += e;
            buffers[5] += f;
            buffers[6] += g;
            buffers[7] += h;

            buffers[8] += s;
            buffers[9] += p;


        }
    }

    string combined;
    for (int i = 0; i < 8; ++i)
        combined += toHexString(buffers[shuffled10[i]]);

    cout << "Output of Collatz Hash Algorithm: " << endl;

    cout << combined << endl;

    return combined;

}