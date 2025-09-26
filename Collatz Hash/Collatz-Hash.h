#include<bits/stdc++.h>
#include "Shuffle.h"

using namespace std;

//----------------------------------------------------------------------define---------------------------------------------------------------

#define ull unsigned long long

#define CollatzHash_512_INPUT_REPRESENTATION_LENGTH 128
#define BLOCK_SIZE 1024

#define BUFFER_COUNT 10
#define WORD_LENGTH 32
#define WORD_COUNT 32
#define X_COUNT 32

using u64 = uint64_t;
using u8 = uint8_t;

u64 T1, T2;


using namespace std;

//-------------------------------------------------------------function------------------------------------------------------------------------

static inline uint32_t rotr16(uint32_t x, unsigned r) { return (x >> r) | (x << (16 - r)); }

static inline uint32_t rotl16(uint32_t x, unsigned r) { return (x << r) | (x >> (16 - r)); }

static inline uint32_t rotl32(uint32_t x, unsigned r) { return (x << r) | (x >> (32 - r)); }

static inline u64 rotl64(u64 x, unsigned r)
{
    return (x << r) | (x >> (64 - r));
}

uint32_t alpha(uint32_t x, uint32_t y, uint32_t z)
{
    // addition + rotation + xor: non-linear and mixes bits
    uint32_t t = (uint32_t)((uint32_t)x + (uint32_t)y + ((uint32_t)z << 1)); // mod 2^16
    // rotate t by 7 and XOR with x -> breaks linearity
    uint32_t r = rotr16(t, 7);
    return (uint32_t)((r ^ x) + (y ^ rotr16(z, 3)));
}

uint32_t beta(uint32_t x, uint32_t y, uint32_t z)
{
    // something similar to SHA-1's choose/majority but with additions
    uint32_t choose = (x & y) ^ ((~x) & z);
    uint32_t maj = (x & y) ^ (x & z) ^ (y & z);
    uint32_t t = (uint32_t)(((uint32_t)choose + (uint32_t)maj + rotr16(x ^ y, 5)) & 0xFFFF);
    return (uint32_t)(t ^ rotr16(z, 11));
}


void initialiseBuffers(vector<ull> &buffers)
{

    buffers = {
        0x6a09e667f3bcc908, 0xbb67ae8584caa73b, 0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,
        0x510e527fade682d1, 0x9b05688c2b3e6c1f, 0x1f83d9abfb41bd6b, 0x5be0cd19137e2179,
        0x0000000000000000, 0x0000000000000000};
}

uint64_t combine32to64(uint32_t parts[2])
{
    return ((uint64_t)parts[0] << 32) | (uint64_t)parts[1];
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

ull getUllFromString(string str)
{
    bitset<WORD_LENGTH> word(str);
    return word.to_ullong();
}


//------------------------------------------------------MD Padding-------------------------------------------------------------------------

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


//---------------------------------------------------------------------Hash Function------------------------------------------------------------


string Collatz_Hash_6(string inp)
{

    vector<ull> buffers(BUFFER_COUNT);
    initialiseBuffers(buffers);

    string input = inp;

    string key = input;

    vector<int> shuffled32 = deterministic_shuffle(key, 32);
    vector<int> shuffled10 = deterministic_shuffle(key, 10);

    string paddedInput = CollatzHash512Padding(input);

    // cout << "Padded Input:" << " " << paddedInput << endl
    //      << endl;

    for (int n = 0; n < paddedInput.size(); n += BLOCK_SIZE)
    {
        string currentBlock = paddedInput.substr(n, BLOCK_SIZE);
        vector<uint32_t> W(WORD_COUNT);



        ull a = buffers[0], b = buffers[1], c = buffers[2], d = buffers[3];
        ull e = buffers[4], f = buffers[5], g = buffers[6], h = buffers[7];

        uint32_t a_parts[2], b_parts[2], c_parts[2], d_parts[2];
        uint32_t e_parts[2], f_parts[2], g_parts[2], h_parts[2];


        a_parts[0] = (uint32_t)(a >> 32);
        b_parts[0] = (uint32_t)(a >> 32);
        c_parts[0] = (uint32_t)(a >> 32);
        d_parts[0] = (uint32_t)(a >> 32);
        e_parts[0] = (uint32_t)(a >> 32);
        f_parts[0] = (uint32_t)(a >> 32);
        g_parts[0] = (uint32_t)(a >> 32);
        h_parts[0] = (uint32_t)(a >> 32);

        a_parts[1] = (uint32_t)(a & 0xFFFFFFFFULL);
        b_parts[1] = (uint32_t)(a & 0xFFFFFFFFULL);
        c_parts[1] = (uint32_t)(a & 0xFFFFFFFFULL);
        d_parts[1] = (uint32_t)(a & 0xFFFFFFFFULL);
        e_parts[1] = (uint32_t)(a & 0xFFFFFFFFULL);
        f_parts[1] = (uint32_t)(a & 0xFFFFFFFFULL);
        g_parts[1] = (uint32_t)(a & 0xFFFFFFFFULL);
        h_parts[1] = (uint32_t)(a & 0xFFFFFFFFULL);



        for (int j = 0; j < WORD_COUNT; ++j)
            W[j] = getUllFromString(currentBlock.substr(j, WORD_LENGTH));
        

        // for (int j = 0; j < 32; ++j)
        // {
        //     cout << "Word-" << j << " : " << W[j] << " ";
        // }
        // cout << endl;

     

        vector<uint32_t> X(X_COUNT, 0);

        X[0] = a_parts[0]; X[4] = c_parts[0]; X[8] = e_parts[0];  X[12] = g_parts[0];
        X[1] = a_parts[1]; X[5] = c_parts[1]; X[9] = e_parts[1];  X[13] = g_parts[1];
        X[2] = b_parts[0]; X[6] = d_parts[0]; X[10] = f_parts[0]; X[14] = h_parts[0];
        X[3] = b_parts[1]; X[7] = d_parts[1]; X[11] = f_parts[1]; X[15] = h_parts[1];



        for (int i = 0; i < 16; i++)
        {
            uint32_t left = X[(i + 15) % 16];
            uint32_t right = X[(i + 1) % 16];
            // rotation offsets vary to avoid symmetry
            uint32_t mix = (uint32_t)((left + rotr16(right, (i & 3) + 3)) & 0xFFFF);
            // XOR + add for nonlinearity
            X[i] = (uint32_t)(X[i] ^ rotr16(mix, (i & 0xF)));
        }

        for (int w = 0; w <= 6; w++)
        {

            uint16_t x = (X[shuffled32[w]%16]>>(48-w%2*16)&0xFFFF);

            uint16_t x_=x;    
            // cout << "We take 'x' as X[" << i << "] value is :" << x << endl;

            while (x >= 2) // Chnage from CH-1.
            {

                if (x % 2 == 0)
                {

                    a_parts[0] = alpha(a_parts[0], W[shuffled32[0]], rotr16(W[shuffled32[16]], x % 7));
                    a_parts[1] = alpha(a_parts[1], W[shuffled32[1]], rotr16(W[shuffled32[17]], x % 7));

                    b_parts[0] = alpha(b_parts[0], W[shuffled32[2]], rotr16(W[shuffled32[18]], x % 7));
                    b_parts[1] = alpha(b_parts[1], W[shuffled32[3]], rotr16(W[shuffled32[19]], x % 7));

                    c_parts[0] = alpha(c_parts[0], W[shuffled32[4]], rotr16(W[shuffled32[20]], x % 7));
                    c_parts[1] = alpha(c_parts[1], W[shuffled32[5]], rotr16(W[shuffled32[21]], x % 7));

                    d_parts[0] = alpha(d_parts[0], W[shuffled32[6]], rotr16(W[shuffled32[22]], x % 7));
                    d_parts[1] = alpha(d_parts[1], W[shuffled32[7]], rotr16(W[shuffled32[23]], x % 7));

                    e_parts[0] = alpha(e_parts[0], W[shuffled32[8]], rotr16(W[shuffled32[24]], x % 7));
                    e_parts[1] = alpha(e_parts[1], W[shuffled32[9]], rotr16(W[shuffled32[25]], x % 7));

                    f_parts[0] = alpha(f_parts[0], W[shuffled32[10]], rotr16(W[shuffled32[26]], x % 7));
                    f_parts[1] = alpha(f_parts[1], W[shuffled32[11]], rotr16(W[shuffled32[27]], x % 7));

                    g_parts[0] = alpha(g_parts[0], W[shuffled32[12]], rotr16(W[shuffled32[28]], x % 7));
                    g_parts[1] = alpha(g_parts[1], W[shuffled32[13]], rotr16(W[shuffled32[29]], x % 7));

                    h_parts[0] = alpha(h_parts[0], W[shuffled32[14]], rotr16(W[shuffled32[30]], x % 7));
                    h_parts[1] = alpha(h_parts[1], W[shuffled32[15]], rotr16(W[shuffled32[31]], x % 7));

                    x = x / 2;
                }

                else
                {

                    a_parts[0] = beta(a_parts[0], W[shuffled32[0]], rotr16(W[shuffled32[16]], x % 13));
                    a_parts[1] = beta(a_parts[1], W[shuffled32[1]], rotr16(W[shuffled32[17]], x % 13));

                    b_parts[0] = beta(b_parts[0], W[shuffled32[2]], rotr16(W[shuffled32[18]], x % 13));
                    b_parts[1] = beta(b_parts[1], W[shuffled32[3]], rotr16(W[shuffled32[19]], x % 13));

                    c_parts[0] = beta(c_parts[0], W[shuffled32[4]], rotr16(W[shuffled32[20]], x % 13));
                    c_parts[1] = beta(c_parts[1], W[shuffled32[5]], rotr16(W[shuffled32[21]], x % 13));

                    d_parts[0] = beta(d_parts[0], W[shuffled32[6]], rotr16(W[shuffled32[22]], x % 13));
                    d_parts[1] = beta(d_parts[1], W[shuffled32[7]], rotr16(W[shuffled32[23]], x % 13));

                    e_parts[0] = beta(e_parts[0], W[shuffled32[8]], rotr16(W[shuffled32[24]], x % 13));
                    e_parts[1] = beta(e_parts[1], W[shuffled32[9]], rotr16(W[shuffled32[25]], x % 13));

                    f_parts[0] = beta(f_parts[0], W[shuffled32[10]], rotr16(W[shuffled32[26]], x % 13));
                    f_parts[1] = beta(f_parts[1], W[shuffled32[11]], rotr16(W[shuffled32[27]], x % 13));

                    g_parts[0] = beta(g_parts[0], W[shuffled32[12]], rotr16(W[shuffled32[28]], x % 13));
                    g_parts[1] = beta(g_parts[1], W[shuffled32[13]], rotr16(W[shuffled32[29]], x % 13));

                    h_parts[0] = beta(h_parts[0], W[shuffled32[14]], rotr16(W[shuffled32[30]], x % 13));
                    h_parts[1] = beta(h_parts[1], W[shuffled32[15]], rotr16(W[shuffled32[31]], x % 13));


                    x = ((3 * x) + 1) / 2;
                }
            }

            a = combine32to64(a_parts);
            b = combine32to64(b_parts);
            c = combine32to64(c_parts);
            d = combine32to64(d_parts);
            e = combine32to64(e_parts);
            f = combine32to64(f_parts);
            g = combine32to64(g_parts);
            h = combine32to64(h_parts);



            T1 = (a ^ W[(x_+3)%32]);
            b = a ^ W[x_%32];
            c = b ^ T1;
            d = c ^ rotl64(T1, 29);
            e = d ^ rotl64(T1, 41);
            T2 = (e ^ W[(x_+2)%32]);
            f = e ^ W[(x_+5)%32];
            g = f ^ T2;
            h = g ^ rotl64(T2, 53);
            a = h ^ rotl64(T2, 13);

            

            buffers[0] += a;
            buffers[1] += b;
            buffers[2] += c;
            buffers[3] += d;
            buffers[4] += e;
            buffers[5] += f;
            buffers[6] += g;
            buffers[7] += h;

            buffers[8] += T1;
            buffers[9] += T2;
        }


        if(m+BLOCK_SIZE==paddedInput.size())
        {
                T1 = (a ^ W[m%32]);
                b = a ^ W[(m+5)%32];
                c = b ^ T1;
                d = c ^ rotl64(T1, 29);
                e = d ^ rotl64(T1, 41);
                T2 = (e ^ W[(m+1)%32]);
                f = e ^ W[(m+7)%32];
                g = f ^ T2;
                h = g ^ rotl64(T2, 53);
                a = h ^ rotl64(T2, 13);

                
        }

            buffers[0] += a;
            buffers[1] += b;
            buffers[2] += c;
            buffers[3] += d;
            buffers[4] += e;
            buffers[5] += f;
            buffers[6] += g;
            buffers[7] += h;

            buffers[8] += T1;
            buffers[9] += T2;
    }


    cout<<"Digest Size: ";
    int S;
    cin>>S;

    if(S!=256 && S!=384 && S!=512)
    cout<<"Invalid input, it should be 256 or 384 or 512 "<<endl;


    string combined;
    for (int i = 0; i < S/64; ++i)
        combined += toHexString(buffers[shuffled10[i]]);

    cout<<endl;
    cout<<endl;    
    cout << "Output of Collatz Hash Algorithm: " << endl;

    cout << combined << endl;
    cout<<endl; 
    cout<<endl; 

    return combined;
}
