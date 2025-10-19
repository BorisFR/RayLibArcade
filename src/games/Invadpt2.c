#include "Invadpt2.h"

#define INVADERS_INVERT_Y(value) 264 - value

WRITE_HANDLER(invadpt2_videoram_w)
{
    if (data == videoram[offset])
        return;
    videoram[offset] = data;
    uint16_t x = offset / 32;
    uint16_t y = screenHeight - 8 * (offset % 32);
    DIRTY_MIN(x, screenDirtyMinX)
    DIRTY_MAX(x + 1, screenDirtyMaxX)
    DIRTY_MIN(y - 8, screenDirtyMinY)
    DIRTY_MAX(y - 1 + 1, screenDirtyMaxY)
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
        if ((data & 0x01))
        {
            GamePlotPixel(x, y - bit - 1, c);
        }
        else
        {
            GameClearPixel(x, y - bit - 1);
        }
        data >>= 1;
    }
}