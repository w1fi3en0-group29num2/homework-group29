#include <iostream>
#include "AES.h"

void AddRoundKey(uint8_t* roundkey, uint8_t* state){
    for (int i = 0; i < 16; i++){
        state[i] ^= roundkey[i];
    }
}