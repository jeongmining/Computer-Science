#include "aes.h"

uint32_t SubWord(uint32_t word)
{
    uint32_t result = 0;

    result |= sbox[word & 0xff];
    result |= sbox[(word >> 8) & 0xff] << 8;
    result |= sbox[(word >> 16) & 0xff] << 16;
    result |= sbox[(word >> 24) & 0xff] << 24;

    return result;
}

uint32_t RotWord(uint32_t word)
{
    uint32_t result;
    result = (word << 24) | (word >> 8);

    return result;
}

void KeyExpansion(const uint8_t *key, uint32_t *roundKey)
{
    for (int i = 0; i < Nk; ++i)
    {
        roundKey[i] = (key[4 * i]) | (key[4 * i + 1] << 8) | (key[4 * i + 2] << 16) | (key[4 * i + 3] << 24);
    }
    for (int i = Nk; i < RNDKEYLEN; ++i)
    {
        uint32_t temp = roundKey[i - 1];
        if (i % Nk == 0)
            temp = SubWord(RotWord(temp)) ^ Rcon[i / Nk];
        roundKey[i] = roundKey[i - Nk] ^ temp;
    }
}

void SubBytes(uint8_t *state, int mode)
{
    for (int i = 0; i < 16; ++i)
    {
        state[i] = mode == ENCRYPT ? sbox[state[i]] : isbox[state[i]];
    }
}

void AddRoundKey(uint8_t *state, const uint32_t *roundKey)
{
    for (int i = 0; i < 16; ++i)
    {
        state[i] ^= (((uint8_t *)roundKey)[i]);
    }
}

void ShiftRows(uint8_t *state, int mode)
{
    uint32_t tmp;
    if (mode == ENCRYPT)
    {
        tmp = state[1] | state[5] << 8 | state[9] << 16 | state[13] << 24;
        tmp = (tmp << 24) | (tmp >> 8);
        state[1] = ((uint8_t *)&tmp)[0];
        state[5] = ((uint8_t *)&tmp)[1];
        state[9] = ((uint8_t *)&tmp)[2];
        state[13] = ((uint8_t *)&tmp)[3];

        tmp = state[2] | state[6] << 8 | state[10] << 16 | state[14] << 24;
        tmp = (tmp << 16) | (tmp >> 16);
        state[2] = ((uint8_t *)&tmp)[0];
        state[6] = ((uint8_t *)&tmp)[1];
        state[10] = ((uint8_t *)&tmp)[2];
        state[14] = ((uint8_t *)&tmp)[3];

        tmp = state[3] | state[7] << 8 | state[11] << 16 | state[15] << 24;
        tmp = (tmp << 8) | (tmp >> 24);
        state[3] = ((uint8_t *)&tmp)[0];
        state[7] = ((uint8_t *)&tmp)[1];
        state[11] = ((uint8_t *)&tmp)[2];
        state[15] = ((uint8_t *)&tmp)[3];
    }
    else
    {
        tmp = state[1] | state[5] << 8 | state[9] << 16 | state[13] << 24;
        tmp = (tmp << 8) | (tmp >> 24);
        state[1] = ((uint8_t *)&tmp)[0];
        state[5] = ((uint8_t *)&tmp)[1];
        state[9] = ((uint8_t *)&tmp)[2];
        state[13] = ((uint8_t *)&tmp)[3];

        tmp = state[2] | state[6] << 8 | state[10] << 16 | state[14] << 24;
        tmp = (tmp << 16) | (tmp >> 16);
        state[2] = ((uint8_t *)&tmp)[0];
        state[6] = ((uint8_t *)&tmp)[1];
        state[10] = ((uint8_t *)&tmp)[2];
        state[14] = ((uint8_t *)&tmp)[3];

        tmp = state[3] | state[7] << 8 | state[11] << 16 | state[15] << 24;
        tmp = (tmp << 24) | (tmp >> 8);
        state[3] = ((uint8_t *)&tmp)[0];
        state[7] = ((uint8_t *)&tmp)[1];
        state[11] = ((uint8_t *)&tmp)[2];
        state[15] = ((uint8_t *)&tmp)[3];
    }
}
void MixColumns(uint8_t *state, int mode)
{
    uint8_t temp[16] = {};
    for (uint8_t i = 0; i < BLOCKLEN; ++i)
    {
        uint8_t x = i / Nb, y = i % Nb;
        for (int k = y * 4; k < y * 4 + 4; ++k)
        {
            uint8_t mul = (mode == ENCRYPT ? M[k] : IM[k]);
            if (mul == 1)
            {
                temp[x * 4 + y] ^= state[x * 4 + k % 4];
            }
            else if (mul <= 3)
            {
                temp[x * 4 + y] ^= XTIME(state[x * 4 + k % 4]);
                if (mul == 3)
                {
                    temp[x * 4 + y] ^= state[x * 4 + k % 4];
                }
            }
            else
            {
                temp[x * 4 + y] ^= XTIME(XTIME(XTIME(state[x * 4 + k % 4])));
                if (mul == 9 || mul == 11 || mul == 13)
                {
                    temp[x * 4 + y] ^= state[x * 4 + k % 4];
                }
                if (mul == 11 || mul == 14)
                {
                    temp[x * 4 + y] ^= XTIME(state[x * 4 + k % 4]);
                }
                if (mul == 13 || mul == 14)
                {
                    temp[x * 4 + y] ^= XTIME(XTIME(state[x * 4 + k % 4]));
                }
            }
        }
    }
    for (uint8_t i = 0; i < 16; i++)
        state[i] = temp[i];
}
void Cipher(uint8_t *state, const uint32_t *roundKeys, int mode)
{

    if (mode == ENCRYPT)
    {
        AddRoundKey(state, roundKeys);
        for (int i = 1; i < Nr; ++i)
        {
            SubBytes(state, mode);
            ShiftRows(state, mode);
            MixColumns(state, mode);
            AddRoundKey(state, roundKeys + i * Nk);
        }
        SubBytes(state, mode);
        ShiftRows(state, mode);
        AddRoundKey(state, roundKeys + Nr * Nk);
    }
    else
    {
        AddRoundKey(state, roundKeys + Nr * Nk);
        for (int i = Nr - 1; i >= 1; --i)
        {
            ShiftRows(state, mode);
            SubBytes(state, mode);
            AddRoundKey(state, roundKeys + i * Nk);
            MixColumns(state, mode);
        }
        ShiftRows(state, mode);
        SubBytes(state, mode);
        AddRoundKey(state, roundKeys);
    }
}