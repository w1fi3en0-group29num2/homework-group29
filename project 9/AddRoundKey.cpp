#include <iostream>
#include "TypeDefine.h"

void AddRoundKey(byte* roundkey, byte* state){
    for (int i = 0; i < 16; i++){
        state[i] ^= roundkey[i];
    }
}