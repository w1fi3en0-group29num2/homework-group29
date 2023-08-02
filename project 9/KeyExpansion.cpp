#include <iostream>
#include "AES.h"
using namespace std;

typedef unsigned int uint32_t; // 字：一个字由4个字节组成，即32比特
typedef unsigned char uint8_t;

void fun_g(uint32_t *x){
    // 字循环
    uint32_t temp1 = *x >> 24;
    *x = *x << 8;
    *x ^= temp1;
    // 字代替
    SubWord(x);
}

// AES-128轮常量
uint32_t rcon[10] = {
    0x01000000UL, 0x02000000UL, 0x04000000UL, 0x08000000UL, 0x10000000UL,
    0x20000000UL, 0x40000000UL, 0x80000000UL, 0x1B000000UL, 0x36000000UL
};

void KeyExpansion(uint8_t* key, uint32_t* w){
    uint32_t temp;
    for (int i = 0; i < 4;i++){
        for (int j = 0; j <4;j++){
            memcpy((uint8_t *)w + i * 4 + 3 - j, key + i * 4 + j, 1);
        }
    }// 初始化w
    for (int i = 4; i < 44; i++)
    {
        temp = w[i - 1];
        if (i % 4 == 0){
            fun_g(&temp);
            temp ^= rcon[i / 4-1];
        }
        w[i] = w[i - 4] ^ temp;
    }
}