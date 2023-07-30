#include <iostream>
#include "TypeDefine.h"

byte mul2(byte n)
{
	byte res;
	if (n & 0x80){
		res=(n << 1) ^ 0x1B;
	}else{
		res = n << 1;
	}
	return res;
}
byte mul3(byte n)
{
	byte res;
	if (n & 0x80){
		res = (n << 1) ^ 0x1B;
		res ^= n;
	}else{
		res = n << 1;
		res ^= n;
	}
	return res;
}
void MixColumns(byte *state)
{
	byte temp[4 * 4];
	for (int i = 0; i < 4; i++)
	{
		temp[i*4] = mul2(state[i*4]) ^ mul3(state[i*4+1]) ^ state[i*4+2] ^ state[i*4+3];
		temp[i*4 + 1] = state[i*4] ^ mul2(state[i*4 + 1]) ^ mul3(state[i*4 + 2]) ^ state[i*4 + 3];
		temp[i*4 + 2] = state[i*4] ^ state[i*4 + 1] ^ mul2(state[i*4 + 2]) ^ mul3(state[i*4 + 3]);
		temp[i*4 + 3] = mul3(state[i*4]) ^ state[i*4 + 1] ^ state[i*4 + 2] ^ mul2(state[i*4 + 3]);
	}
	memcpy(state, temp, 16);
}