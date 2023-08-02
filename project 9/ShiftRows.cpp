#include <iostream>
#include "AES.h"

void ShiftRows(uint8_t* state){ //LSJ
    uint8_t temp;
    uint8_t temp2;
    temp = state[1];
    state[1] = state[5];
    state[5] = state[9];
    state[9] = state[13];
    state[13] = temp;

    temp = state[2];
    state[2] = state[10];
    state[10] = temp;
    temp = state[6];
    state[6] = state[14];
    state[14] = temp;

    temp = state[3];
    temp2 = state[7];
    state[3] = state[15];
    state[7] = temp;
    temp = state[11];
    state[11] = temp2;
    state[15] = temp;
}
