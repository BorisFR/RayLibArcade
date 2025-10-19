#include "Pacman.h"

struct GfxLayout pacmanTileLayout = {
    8, 8, /* 8*8 characters */
    256,
    /* 256 characters */                                      // RGN_FRAC(1,2), // 256 characters
    2,                                                        /* 2 bits per pixel */
    {0, 4},                                                   /* the two bitplanes for 4 pixels are packed into one byte */
    {8 * 8 + 0, 8 * 8 + 1, 8 * 8 + 2, 8 * 8 + 3, 0, 1, 2, 3}, /* bits are packed in groups of four */
    {0 * 8, 1 * 8, 2 * 8, 3 * 8, 4 * 8, 5 * 8, 6 * 8, 7 * 8},
    16 * 8 /* every char takes 16 bytes */
};

struct GfxLayout pacmanSpriteLayout = {
    16, 16, /* 16*16 sprites */
    64,
    /* 64 sprites */ // RGN_FRAC(1,2), // 64 sprites
    2,               /* 2 bits per pixel */
    {0, 4},          /* the two bitplanes for 4 pixels are packed into one byte */
    {8 * 8, 8 * 8 + 1, 8 * 8 + 2, 8 * 8 + 3, 16 * 8 + 0, 16 * 8 + 1, 16 * 8 + 2, 16 * 8 + 3, 24 * 8 + 0, 24 * 8 + 1, 24 * 8 + 2, 24 * 8 + 3, 0, 1, 2, 3},
    {0 * 8, 1 * 8, 2 * 8, 3 * 8, 4 * 8, 5 * 8, 6 * 8, 7 * 8, 32 * 8, 33 * 8, 34 * 8, 35 * 8, 36 * 8, 37 * 8, 38 * 8, 39 * 8},
    64 * 8 /* every sprite takes 64 bytes */
};

// *******************************************************************

void pacman_interrupt_enable_w(int offset, int data) { Z80InterruptEnable[Z80CurrentCpu] = (data & 0x01) == 0x01; }

void pacman_sound_enable_w(int offset, int data)
{
    // Sound enable (bit 0: 0 = disabled, 1 = enabled)
    //_soundEnabled = (value & 0x01) == 0x01;
}

void pacman_flipscreen_w(int offset, int data)
{
    // Flip screen (bit 0: 0 = normal, 1 = flipped)
    //_flipScreen = value == 1 ? true : false;
}

void pacman_osd_led_w(int offset, int data)
{
    // 5004: 1 player start lamp (bit 0: 0 = on, 1 = off)
    // 5005: 2 player start lamp (bit 0: 0 = on, 1 = off)
}

void coin_counter_w(int offset, int data)
{
    // Coin counter (trigger by changing bit 0 from 0 to 1)
}

void pacman_sound_w(int offset, int data)
{
    // 5040-5044 sound voice 1 accumulator (nibbles) (used by the sound hardware only)
    // 5045      sound voice 1 waveform (nibble)
    // 5046-5049 sound voice 2 accumulator (nibbles) (used by the sound hardware only)
    // 504a      sound voice 2 waveform (nibble)
    // 504b-504e sound voice 3 accumulator (nibbles) (used by the sound hardware only)
    // 504f      sound voice 3 waveform (nibble)
    // 5050-5054 sound voice 1 frequency (nibbles)
    // 5055      sound voice 1 volume (nibble)
    // 5056-5059 sound voice 2 frequency (nibbles)
    // 505a      sound voice 2 volume (nibble)
    // 505b-505e sound voice 3 frequency (nibbles)
    // 505f      sound voice 3 volume (nibble)

    // Sound voice 1
    // 5040-5044 – accumulator (low nibbles, used by H/W only)
    // Sound voice 1
    // 5045 – waveform (low nibble)
    //_audio.Voice1Waveform = value;
    // Sound voice 2
    // 5046-5049 – accumulator (low nibbles, used by H/W only)
    // Sound voice 1
    // 504A – waveform (low nibble)
    //_audio.Voice2Waveform = value;
    // Sound voice 3
    // 504B-504E – accumulator (low nibbles, used by H/W only)
    // Sound voice 3
    // 504F – waveform (low nibble)
    //_audio.Voice3Waveform = value;
    // ...
}

void pacman_watchdog_reset_w(int offset, int data)
{
    // Watchdog reset (each byte has the same function)
    // This would write values that the watchdog would look for to determine if the game
    // code had locked up or not. Since I'm not implementing the watchdog hardware I don't
    // need to implement this.
}

void pacman_interrupt_vector_w(int offset, int data) { Z80InterruptVector[Z80CurrentCpu] = data; }

// *******************************************************************

void PacmanDrawTile(uint32_t offset, uint32_t atX, uint32_t atY)
{
    uint16_t tileAddress = 0x4000 + offset;
    uint16_t paletteAddress = 0x4400 + offset;
    uint8_t tileIndex = boardMemory[tileAddress];
    uint8_t paletteIndex = boardMemory[paletteAddress] & 0x7F;
    // not possible as we have to redraw behind sprite
    // if (dirtyMemoryTiles[tileAddress - 0x4000] == tileIndex && dirtyMemoryTiles[paletteAddress - 0x4000] == paletteIndex)
    //    return;
    // dirtyMemoryTiles[tileAddress - 0x4000] = tileIndex;
    // dirtyMemoryTiles[paletteAddress - 0x4000] = paletteIndex;
    GameDrawElement(screenData, atX, atY, false, false, tileIndex, paletteIndex, TRANSPARENCY_NONE, TRANSPARENT_NONE_COLOR);
}

// *******************************************************************

void PacmanRefreshScreen()
{
    element = allGfx[0];
    // visibleArea = VISIBLE_AREA_FULL;

    // The playfield uses addresses 040 through 3BF, increasing from top to bottom,
    // right to left, starting at the top right corner of the playfield.
    uint16_t originX = 29 * 8; // Column 30 (29 zero-indexed)
    uint16_t originY = 2 * 8;  // Row 3 (2 zero-indexed)
    uint16_t playfieldRow = 1;
    for (uint32_t i = 0x040; i <= 0x3BF; i++)
    {
        PacmanDrawTile(i, originX, originY);
        if (playfieldRow == 32)
        {
            // Next column (to the left) and back to the top.
            originX -= 8;
            originY = 2 * 8; // Row 3 (2 zero-indexed)
            playfieldRow = 1;
        }
        else
        {
            // Next row.
            originY += 8;
            playfieldRow++;
        }
    }
    // First row uses addresses 3DF through 3C0, decreasing from left to right.
    originX = 0; // Column 0
    originY = 0; // Row 0
    for (uint32_t i = 0x3DF; i >= 0x3C0; i--)
    {
        PacmanDrawTile(i, originX, originY);
        // Next column.
        originX += 8;
    }
    // Second row uses addresses 3FF through 3E0, decreasing from left to right.
    originX = 0;     // Column 0
    originY = 1 * 8; // Row 1 (0 zero-indexed)
    for (uint32_t i = 0x3FF; i >= 0x3E0; i--)
    {
        PacmanDrawTile(i, originX, originY);
        // Next column.
        originX += 8;
    }
    // First row uses addresses 01F through 000, decreasing from left to right.
    originX = 0;                             // Column 0
    originY = 34 * 8;                        // Row 35 (34 zero-indexed)
    for (int32_t i = 0x01F; i >= 0x000; i--) // could be negative
    {
        PacmanDrawTile(i, originX, originY);
        // Next column.
        originX += 8;
    }
    // Second row uses addresses 03F through 020, decreasing from left to right.
    originX = 0;      // Column 0
    originY = 35 * 8; // Row 36 (35 zero-indexed)
    for (uint32_t i = 0x03F; i >= 0x020; i--)
    {
        PacmanDrawTile(i, originX, originY);
        // Next column.
        originX += 8;
    }

    // sprites

    element = allGfx[1];

    for (uint8_t offset = 0; offset < 8; offset++) // 8 is sprites max for pacman
    {
        uint32_t addressPosition = 0x5060 + (15 - offset * 2);
        int16_t posY = boardMemory[addressPosition];
        addressPosition--;
        int16_t posX = boardMemory[addressPosition];
        int32_t atX = screenWidth - posX - 1;
        int32_t atY = screenHeight - element->height - posY;
        // if ((atX + element->width) < element->width || atX >= (screenWidth - element->width) || atY < 0 || atY >= screenHeight)
        //{
        //     // nothing to draw
        // }
        // else
        { // something to draw
            uint32_t addressState = 0x4FFF - offset * 2;
            uint8_t paletteIndex = boardMemory[addressState];
            addressState--;
            uint8_t state = boardMemory[addressState];
            bool flipX = (state & 0x02) == 0x02;
            bool flipY = (state & 0x01) == 0x01;
            uint8_t spriteIndex = (state & 0xFC) >> 2;
            GameDrawElement(screenData, atX, atY, flipX, flipY, spriteIndex, paletteIndex, TRANSPARENCY_BLACK, TRANSPARENT_NONE_COLOR);
            // GameDrawElement(screenData, atX, atY, flipX, flipY, spriteIndex, paletteIndex, TRANSPARENCY_NONE, TRANSPARENT_NONE_COLOR);
        } // something to draw
    }
}