#pragma once
#include <cstdint>
using namespace std;
void KeyExpansion(uint8_t *, uint32_t *);
void AddRoundKey(uint8_t *, uint8_t *);
void Subuint8_ts(uint8_t*);
void ShiftRows(uint8_t*);
void MixColumns(uint8_t*);
void SubWord(uint32_t*);
void GenerateText(uint8_t **, int);