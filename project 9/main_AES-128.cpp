#include <iostream>
#include <cstdio>
#include <cstring>
#include "AES.h"

const unsigned short Nr=10;

void show_buf(uint8_t *state);

int main(){
    uint8_t plaintext[17] = "2021004600340000";//明文
    // std::cout << "Please enter plaintext:";
    // std::cin >> plaintext;
    uint8_t key[17] = "2021004600340000";//种子密钥
    // std::cout << "Please enter key:";
    // std::cin >> key;

    // 1.初始化State，128比特分组长度，即16个字节
    uint8_t State[16]{};
    memcpy(State, plaintext, 16);

    // 2.编排轮密钥
    uint32_t w[44]{};
    KeyExpansion(key, w); // 44个字构成11个轮密钥
    uint8_t RoundKey[16]{};
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            memcpy(RoundKey + i * 4 + 3 - j, (uint8_t*)w + i * 4 + j, 1);
        }
    } // 初始化轮密钥

    // 3.第一次AddRoundKey，白化
    AddRoundKey(RoundKey, State);

    // 4.进行Nr=10轮加密，对前9轮中的每一轮:
    //                      >用S盒对State进行一次代换Subuint8_ts
    //                      >对State做一置换ShiftRows
    //                      >再对State做一次线性变换MixColumns
    //                      >然后进行AddRoundKey
    //最后一轮不用Mixcolumns
    for (int k = 0; k < Nr; k++){
        Subuint8_ts(State);
        ShiftRows(State);
        if(k!=Nr-1){
            MixColumns(State);
        }
        for (int i = 0; i < 4; i++){
            for (int j = 0; j < 4; j++){
                memcpy(RoundKey + i * 4 + 3 - j, (uint8_t *)w + (k+1) * 16 + i * 4 + j, 1);
            }
        } // 更新轮密钥
        AddRoundKey(RoundKey, State);
    }

    // 5.输出
    show_buf(State); // 4E D6 73 FF 01 91 F5 54 DE 5D 04 96 B0 39 37 78

    return 0;
}


void show_buf(uint8_t *state) // 输出state处16字节二进制数据
{
    for (int k = 0; k < 16; k++)
    {
        printf("%02X ", *(state + k));
    }
    printf("\n");
}