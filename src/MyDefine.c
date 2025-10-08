#include "MyDefine.h"

uint8_t currentGame;

uint8_t GetBitFromNumber(uint8_t value)
{
    uint8_t res = 0;
    uint8_t temp = value >> 1;
    while (temp > 0) {
        res++;
        temp = temp >> 1;
    }
    return res;
}
