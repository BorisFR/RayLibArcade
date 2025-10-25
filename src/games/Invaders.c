#include "Invaders.h"

#define INVADERS_INVERT_Y(value) 264 - value

WRITE_HANDLER(invaders_videoram_w)
{
    if (data == videoram[offset])
        return;
    videoram[offset] = data;
    uint16_t x = offset / 32;
    uint16_t y = screenGameHeight - 8 * (offset % 32);
    DIRTY_MIN(x - 0, screenDirtyMinX)
    DIRTY_MAX(x + 1, screenDirtyMaxX)
    DIRTY_MIN(y - 8 - 0, screenDirtyMinY)
    DIRTY_MAX(y - 1 + 1, screenDirtyMaxY)
    THE_COLOR c = myWhite;
    if (y >= INVADERS_INVERT_Y(72) && y < INVADERS_INVERT_Y(16))
    {
        c = myGreen;
    }
    if (y >= INVADERS_INVERT_Y(16) && x > 16 && x < 134)
    {
        c = myGreen;
    }
    if (y >= INVADERS_INVERT_Y(224) && y < INVADERS_INVERT_Y(192))
    {
        c = myRed;
    }
    for (uint8_t bit = 0; bit < 8; bit++)
    {
        if (!(data & 0x01))
        {
            GameClearPixel(x, y - bit - 1);
        }
        else
        {
            GamePlotPixel(x, y - bit - 1, c);
        }
        data >>= 1;
    }
}