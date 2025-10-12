#include "Invadpt2.h"

#define INVADERS_INVERT_Y(value) 264 - value

void invadpt2_videoram_w(int offset, int data)
{
    if (data == boardMemory[offset])
        return;
    boardMemory[offset] = data;
    uint32_t address = offset - 0x2400;
    uint16_t x = address / 32;
    uint16_t y = screenHeight - 8 * (address % 32);
    DIRTY_MIN(x, screenDirtyMinX)
    DIRTY_MAX(x, screenDirtyMaxX)
    DIRTY_MIN(y - 8, screenDirtyMinY)
    DIRTY_MAX(y - 1, screenDirtyMaxY)
    THE_COLOR c = myWhite;
    if (y >= INVADERS_INVERT_Y(248) && y < INVADERS_INVERT_Y(240))
    {
        if (x < 72)
            c = myCyan;
        if (x >= 72 && x < 152)
            c = myRed;
        if (x >= 152)
            c = myYellow;
    }
    if (y >= INVADERS_INVERT_Y(240) && y < INVADERS_INVERT_Y(232))
    {
        if (x < 72)
            c = myCyan;
        if (x >= 72 && x < 160)
            c = myGreen;
        if (x >= 160)
            c = myYellow;
    }
    if (y >= INVADERS_INVERT_Y(232) && y < INVADERS_INVERT_Y(224))
    {
        if (x >= 72 && x < 160) // or 152?
            c = myGreen;
        if (x >= 160)
            c = myYellow;
    }
    if (y >= INVADERS_INVERT_Y(224) && y < INVADERS_INVERT_Y(216))
        c = myRed;
    if (y >= INVADERS_INVERT_Y(216) && y < INVADERS_INVERT_Y(192))
        c = myPurple;
    if (y >= INVADERS_INVERT_Y(192) && y < INVADERS_INVERT_Y(160))
        c = myGreen;
    if (y >= INVADERS_INVERT_Y(160) && y < INVADERS_INVERT_Y(128))
        c = myCyan;
    if (y >= INVADERS_INVERT_Y(128) && y < INVADERS_INVERT_Y(96))
        c = myPurple;
    if (y >= INVADERS_INVERT_Y(96) && y < INVADERS_INVERT_Y(64))
        c = myYellow;
    if (y >= INVADERS_INVERT_Y(64) && y < INVADERS_INVERT_Y(40))
        c = myRed;
    if (y >= INVADERS_INVERT_Y(40) && y < INVADERS_INVERT_Y(24))
        c = myCyan;
    if (y >= INVADERS_INVERT_Y(24) && y < INVADERS_INVERT_Y(16))
        c = myRed;
    if (y >= INVADERS_INVERT_Y(16))
    {
        if (x < 136)
            c = myCyan;
        if (x >= 136 && x < 192)
            c = myPurple;
        if (x >= 192)
            c = myCyan;
    }
    if (x == 223)
        c = myBlack;
    for (uint8_t bit = 0; bit < 8; bit++)
    {
        uint32_t index = x + (y - bit - 1) * screenWidth;
        if (!(data & 0x01))
        {
            screenData[index] = myBlack;
        }
        else
        {
            screenData[index] = c;
        }
        index++;
        data >>= 1;
    }
    // screenDirtyMaxX++;
    // screenDirtyMaxY++;
    // if (screenDirtyMaxX >= screenWidth)
    //     screenDirtyMaxX = screenWidth - 1;
    // if (screenDirtyMaxY >= screenHeight)
    //     screenDirtyMaxY = screenHeight - 1;
}