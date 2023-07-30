#pragma once
void KeyExpansion(byte *, word *);
void AddRoundKey(byte *, byte *);
void SubBytes(byte*);
void ShiftRows(byte*);
void MixColumns(byte*);
void SubWord(word*);
void GenerateText(byte **, int);