#include<iostream>
#include <immintrin.h> // AVX指令集头文件
#include<chrono>
using namespace std;
using namespace chrono;

#define ROTATELEFT(X, n)  (((X)<<(n)) | ((X)>>(32-(n))))
typedef uint32_t word;
typedef uint8_t byte;

word* sm3(byte* plaintext, uint64_t textLen);
void CF(word* V, byte* Bi);
word P0(word X);
word P1(word X);
word T(int j);
word FF(int j, word X, word Y, word Z);
word GG(int j, word X, word Y, word Z);

word* sm3(byte* plaintext, uint64_t textLen) {
    word IV[] = { 0x7380166f, 0x4914b2b9, 0x172442d7, 0xda8a0600, 0xa96f30bc, 0x163138aa, 0xe38dee4d, 0xb0fb0e4e };
    word* state = new word[8];
    memcpy(state, IV, 32);
    byte buf[64];
    int inputPtr = 0;
    int bufPtr = 0;

    while (inputPtr < textLen) {
        /*buf[bufPtr] = plaintext[inputPtr];
        inputPtr++;
        bufPtr++;

        if (bufPtr == 64) {
            CF(state, buf);
            bufPtr = 0;
        }*/
        memcpy(buf, plaintext, 64);
        inputPtr += 64;
        CF(state, buf);
    }
    
    buf[bufPtr++] = 0x80;
    if (64 - bufPtr < 8) {
        //while (bufPtr < 64) buf[bufPtr++] = 0;
        memset(buf + 1, 0, 63);
        bufPtr = 0;
        CF(state, buf);
    }
    //while (bufPtr < 56) buf[bufPtr++] = 0;
    memset(buf + 1, 0, 55);

    textLen *= 8;
    
    buf[63] = static_cast<byte>(textLen);
    buf[62] = static_cast<byte>((textLen) >> 8);
    buf[61] = static_cast<byte>((textLen) >> 16);
    buf[60] = static_cast<byte>((textLen) >> 24);
    buf[59] = static_cast<byte>((textLen) >> 32);
    buf[58] = static_cast<byte>((textLen) >> 40);
    buf[57] = static_cast<byte>((textLen) >> 48);
    buf[56] = static_cast<byte>((textLen) >> 56);
    
    CF(state, buf);
    return state;
}

void CF(word* V, byte* Bi) {
    word W[68] = {}; // W
    word WW[64] = {}; // W'
    for (int i = 0; i < 16; ++i) {
        W[i] |= ((word)Bi[i * 4] << 24);
        W[i] |= ((word)Bi[i * 4 + 1] << 16);
        W[i] |= ((word)Bi[i * 4 + 2] << 8);
        W[i] |= ((word)Bi[i * 4 + 3]);
    }
    for (int i = 16; i <= 67; ++i) {
        W[i] = P1(W[i - 16] ^ W[i - 9] ^ ROTATELEFT(W[i - 3], 15)) ^ ROTATELEFT(W[i - 13], 7) ^ (W[i - 6]);
    }
    for (int i = 0; i <= 63; i+=8) {
        /*WW[i] = W[i] ^ W[i + 4];*/
        __m256i temp = _mm256_xor_epi32(_mm256_loadu_epi32(&W[i]), _mm256_loadu_epi32(&W[i + 4]));
        _mm256_storeu_epi32(&WW[i], temp);
    }
    constexpr int A = 0, B = 1, C = 2, D = 3, E = 4, F = 5, G = 6, H = 7;
    word reg[8] = {};

    /*for (int j = 0; j < 8; ++j) {
        reg[j] = V[j];
    }*/
    memcpy(reg, V, 32);

    word SS1, SS2, TT1, TT2;
    for (int j = 0; j <= 63; ++j) {
        /*word SS1, SS2, TT1, TT2;*/
        SS1 = ROTATELEFT(ROTATELEFT(reg[A], 12) + reg[E] + ROTATELEFT(T(j), j), 7);
        SS2 = SS1 ^ ROTATELEFT(reg[A], 12);
        TT1 = FF(j, reg[A], reg[B], reg[C]) + reg[D] + SS2 + WW[j];
        TT2 = GG(j, reg[E], reg[F], reg[G]) + reg[H] + SS1 + W[j];
        reg[D] = reg[C];
        reg[C] = ROTATELEFT(reg[B], 9);
        reg[B] = reg[A];
        reg[A] = TT1;
        reg[H] = reg[G];
        reg[G] = ROTATELEFT(reg[F], 19);
        reg[F] = reg[E];
        reg[E] = P0(TT2);

    }
    for (int i = 0; i < 8; ++i) {
        V[i] ^= reg[i];
    }
    /*__m256i temp = _mm256_xor_epi32(_mm256_loadu_epi32(&V[0]), _mm256_loadu_epi32(&reg[0]));
    _mm256_storeu_epi32(&V[0], temp);此处编译器已经做了循环展开的优化*/
}

word P0(word X) {
    return X ^ ROTATELEFT(X, 9) ^ ROTATELEFT(X, 17);
}

word P1(word X) {
    return X ^ ROTATELEFT(X, 15) ^ ROTATELEFT(X, 23);
}

word T(int j) {
    if (j <= 15) {
        return 0x79cc4519;
    }
    else {
        return 0x7a879d8a;
    }
}

word FF(int j, word X, word Y, word Z) {
    if (j <= 15) {
        return X ^ Y ^ Z;
    }
    else {
        return (X & Y) | (X & Z) | (Y & Z);
    }
}

word GG(int j, word X, word Y, word Z) {
    if (j <= 15) {
        return X ^ Y ^ Z;
    }
    else {
        return (X & Y) | ((~X) & Z);
    }
}

int main() {
    byte plaintext[] = { 0x61, 0x62, 0x63, 0x64, 0x61, 0x62, 0x63, 0x64, 0x61, 0x62, 0x63, 0x64, 0x61, 0x62, 0x63, 0x64,0x61, 0x62, 0x63, 0x64, 0x61, 0x62, 0x63, 0x64, 0x61, 0x62, 0x63, 0x64, 0x61, 0x62, 0x63, 0x64,0x61, 0x62, 0x63, 0x64, 0x61, 0x62, 0x63, 0x64, 0x61, 0x62, 0x63, 0x64, 0x61, 0x62, 0x63, 0x64,0x61, 0x62, 0x63, 0x64, 0x61, 0x62, 0x63, 0x64, 0x61, 0x62, 0x63, 0x64, 0x61, 0x62, 0x63, 0x64 };

    auto start = steady_clock::now();
    for (int i = 0; i < 10000; i++) {
        sm3(plaintext, 64);
    }
    auto end = steady_clock::now();

    auto last = duration_cast<milliseconds>(end - start);
    //milliseconds ms
    //microseconds us
    //nanoseconds ns

    word* cyphertext = sm3(plaintext, 64);
    for (int i = 0; i < 8; i++) {
        printf("%x ", cyphertext[i]);
    }
    cout << endl << "Optimized Encrypt Time: " << last.count() << "ms" << endl;
    return 0;
}
