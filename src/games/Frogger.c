#include "Frogger.h"

struct GfxLayout froggerTileLayout =
    {
        8, 8,             /* 8*8 characters */
        256,              /* 256 characters */
        2,                /* 2 bits per pixel */
        {256 * 8 * 8, 0}, /* the two bitplanes are separated */
        {7 * 8, 6 * 8, 5 * 8, 4 * 8, 3 * 8, 2 * 8, 1 * 8, 0 * 8},
        {0, 1, 2, 3, 4, 5, 6, 7},
        8 * 8 /* every char takes 8 consecutive bytes */
};
struct GfxLayout froggerSpriteLayout =
    {
        16, 16,            /* 16*16 sprites */
        64,                /* 64 sprites */
        2,                 /* 2 bits per pixel */
        {64 * 16 * 16, 0}, /* the two bitplanes are separated */
        {23 * 8, 22 * 8, 21 * 8, 20 * 8, 19 * 8, 18 * 8, 17 * 8, 16 * 8, 7 * 8, 6 * 8, 5 * 8, 4 * 8, 3 * 8, 2 * 8, 1 * 8, 0 * 8},
        {0, 1, 2, 3, 4, 5, 6, 7, 8 * 8 + 0, 8 * 8 + 1, 8 * 8 + 2, 8 * 8 + 3, 8 * 8 + 4, 8 * 8 + 5, 8 * 8 + 6, 8 * 8 + 7},
        32 * 8 /* every sprite takes 32 consecutive bytes */
};
/*
// I/O line states
enum line_state
{
    CLEAR_LINE = 0, // clear (a fired or held) line
    ASSERT_LINE,    // assert an interrupt immediately
    HOLD_LINE       // hold interrupt line until acknowledged
};

// I/O line definitions
enum
{
    // input lines
    MAX_INPUT_LINES = 64 + 3,
    INPUT_LINE_IRQ0 = 0,
    INPUT_LINE_IRQ1 = 1,
    INPUT_LINE_IRQ2 = 2,
    INPUT_LINE_IRQ3 = 3,
    INPUT_LINE_IRQ4 = 4,
    INPUT_LINE_IRQ5 = 5,
    INPUT_LINE_IRQ6 = 6,
    INPUT_LINE_IRQ7 = 7,
    INPUT_LINE_IRQ8 = 8,
    INPUT_LINE_IRQ9 = 9,
    INPUT_LINE_NMI = MAX_INPUT_LINES - 3,

    // special input lines that are implemented in the core
    INPUT_LINE_RESET = MAX_INPUT_LINES - 2,
    INPUT_LINE_HALT = MAX_INPUT_LINES - 1
};
*/

// Z80InterruptEnable[Z80CurrentCpu] is m_irq_enabled
// https://github.com/mamedev/mame/blob/master/src/emu/diexec.h#L163
// void set_input_line(int linenum, int state) { assert(device().started()); m_input[linenum].set_state_synced(state); }
// int m_irq_line = INPUT_LINE_NMI;

void frogger_interrupt_enable_w(int offset, int data)
{
    if (data & 1)
        Z80AskForNMI[Z80CurrentCpu] = true;
    else
        Z80AskForNMI[Z80CurrentCpu] = false;
    // //Z80InterruptEnable[Z80CurrentCpu] = data;// & 1;
    // // if CLEAR is held low, we must make sure the interrupt signal is clear
    // if (data == 0)
    //     Z80AskForNMI = false;
    // else // TODO: not sure for the else ... DOES not work
    //    Z80AskForNMI = true;
    // // m_maincpu->set_input_line(m_irq_line, CLEAR_LINE);
}

// uint8_t froggerAttributes[0x0040];

// void frogger_attributes_w(int offset, int data)
//{
//     boardMemory[offset] = data;
//     // if (offset >= 0xb000 && offset < 0xb03f)
//  {
//      uint32_t address = offset - 0xb000;
//      if (offset & 1)
//      {
//          if (froggerAttributes[address] != data)
//          {
//              int sx = (31 - address / 32);
//              int sy = (address % 32);
//              froggerScreen[sx + sy * 32] = true;
//              // for (uint16_t i = address / 2; i < 0x400; i += 32)
//              //     froggerScreen[i] = true;
//          }
//      }
//      froggerAttributes[address] = data;
//  }
//  if ((offset & 1) && frogger_attributesram[offset] != data)
//{
//	for (int i = offset / 2;i < videoram_size;i += 32)
//		dirtybuffer[i] = 1;
//  }
//    // frogger_attributesram[offset] = data;
//}

bool froggerScreenDirty[0x0400];
int froggerScrollLine[32];

void frogger_videoram_w(int offset, int data)
{
    // update any video up to the current scanline
    // m_screen->update_partial(m_screen->vpos());
    // store the data and mark the corresponding tile dirty
    boardMemory[offset] = data;
    // m_bg_tilemap->mark_tile_dirty(offset - 0xa800);
    froggerScreenDirty[offset - 0xa800] = true;
}

void frogger_objram_w(int offset, int data)
{
    // update any video up to the current scanline
    // m_screen->update_partial(m_screen->vpos());
    // store the data
    boardMemory[offset] = data;
    uint32_t address = offset - 0xb000;
    // the first $40 bytes affect the tilemap
    if (address < 0x40)
    {
        // even entries control the scroll position
        if ((address & 0x01) == 0)
        {
            // Frogger: top and bottom 4 bits swapped entering the adder
            // if (m_frogger_adjust)
            froggerScrollLine[address >> 1] = ((data << 4) & 0xf0) | ((data >> 4) & 0x0f);
            // froggerScrollSpeed[data];
            //  if (!m_sfx_adjust)
            //  	m_bg_tilemap->set_scrolly(offset >> 1, data);
            //  else
            //  	m_bg_tilemap->set_scrollx(offset >> 1, m_x_scale*data);
        }
        // odd entries control the color base for the row
        else
        {
            for (address >>= 1; address < 0x0400; address += 32)
                froggerScreenDirty[address] = true;
            //    m_bg_tilemap->mark_tile_dirty(address);
        }
    }
    else
    {
        // sprite
    }
}

void frogger_sh_irqtrigger_w(int offset, int data)
{
    // found here: https://github.com/Jean-MarcHarvengt/teensyMAME/blob/master/teensyMAMEClassic1/sound_frogger.c
    // static int last;
    // if (last == 0 && (data & 0x08) != 0)
    // {
    // 	/* setting bit 3 low then high triggers IRQ on the sound CPU */
    // 	cpu_cause_interrupt(1,0xff);
    // }
    // last = data & 0x08;
}

void frogger_decode_rom()
{
    // uint8_t *rombase = memregion("gfx1")->base();
    //  the 2nd gfx ROM has data lines D0 and D1 swapped
    // for (uint32_t offs = 0x0800; offs < 0x1000; offs++)
    //	rombase[offs] = bitswap<8>(rombase[offs], 7,6,5,4,3,2,0,1);
    for (uint32_t address = 0x0800; address < 0x1000; address++)
    {
        gfxMemory[address] = (gfxMemory[address] & 0xfc) | ((gfxMemory[address] & 1) << 1) | ((gfxMemory[address] & 2) >> 1);
    }
    // uint8_t *rombase = memregion("audiocpu")->base();
    //  the first ROM of the sound CPU has data lines D0 and D1 swapped
    // for (uint32_t offs = 0; offs < 0x800; offs++)
    //	rombase[offs] = bitswap<8>(rombase[offs], 7,6,5,4,3,2,0,1);
    for (uint32_t address = 0x0800; address < 0x800; address++)
    {
        soundMemory[address] = (soundMemory[address] & 0xfc) | ((soundMemory[address] & 1) << 1) | ((soundMemory[address] & 2) >> 1);
    }
}

// uint8_t maxc = 0;
uint8_t maxc2 = 0;
// uint8_t last = 0;

void FroggerRefreshScreen()
{
    // memcpy(screenDataOld, screenData, screenLength);
    //for (uint32_t i = 0; i < screenLength; i++)
    //    screenDataOld[i] = screenData[i];
    // videoram_size = 0x03FF;
    // frogger_attributesram => boardMemory from 0xb000
    element = allGfx[0];
    visibleArea = VISIBLE_AREA_FULL;
    /* for every character in the Video RAM, check if it has been modified */
    /* since last time and update it accordingly. */
    for (int offset = 0x0400 - 1; offset >= 0; offset--)
    {
        // if (froggerScreen[offset])
        //{
        // froggerScreen[offset] = false;
        int sx = (31 - offset / 32);
        int sy = (offset % 32);
        int col = boardMemory[0xb000 + 2 * sy + 1] & 7;
        col = ((col >> 1) & 0x03) | ((col << 2) & 0x04);
        // if (col > maxc)
        //{
        //     maxc = col;
        //     MY_DEBUG2("CC", "Max tile:", maxc)
        // } // 7
        //  drawgfx(tmpbitmap,Machine->gfx[0],
        //  		videoram[offset],
        //  		col + (sy <= 15 ? 8 : 0),	// blue background in the upper 128 lines
        //  		0,0,8*sx,8*sy,
        //  		0,TRANSPARENCY_NONE,0);
        uint16_t tileAddress = 0xa800 + offset;
        uint8_t tileIndex = boardMemory[tileAddress];
        if (sy < 16)
        {
            GameDrawElement(screenData, 8 * sx, 8 * sy, false, false, tileIndex, col, TRANSPARENCY_REPLACE, froggerWater);
            // if (last != col)
            //{
            //     last = col;
            //     MY_DEBUG2("CC", "Palette:", last)
            // } // O ou 3
        }
        else
        {
            GameDrawElement(screenData, 8 * sx, 8 * sy, false, false, tileIndex, col, TRANSPARENCY_NONE, TRANSPARENT_NONE_COLOR);
            // if (last != col)
            //{
            //     last = col;
            //     MY_DEBUG2("CC", "Palette:", last)
            // } // 0 ou 4
        }
        //}
    }
    /*
        // copy the temporary bitmap to the screen
        {
            int scroll[32];
            for (int i = 0;i < 32;i++)
            {
                int s = frogger_attributesram[2 * i];
                scroll[i] = ((s << 4) & 0xf0) | ((s >> 4) & 0x0f);
            }
            copyscrollbitmap(bitmap,tmpbitmap,32,scroll,0,0,&Machine->drv->visible_area,TRANSPARENCY_NONE,0);
        }
*/
    // better to do completly by code!
    // TODO: not good cause visiility area is not active...
    // memcpy(screenData, screenDataOld, screenLength);
    //for (uint32_t i = 0; i < screenLength; i++)
    //    screenData[i] = screenDataOld[i];

    // for (uint8_t line = 0; line < 32; line++)
    //{
    //     int scroll = froggerScrollLine[line] % screenWidth;
    //     if (scroll > 0)
    //     {
    //         // for (int x = screenWidth - scroll; x >= 0; x--)
    //         for (int x = screenWidth - 1; x >= 0; x--)
    //         {
    //             if (x + scroll >= 2 * 8 && x + scroll < 30 * 81)
    //             {
    //                 for (uint8_t y = 0; y < 8; y++)
    //                 {
    //                     CHECK_IF_DIRTY_XY(x + scroll, line * 8 + y)
    //                     // uint32_t old = screenDataOld[(x + scroll) + (line * 8 + y) * screenWidth];
    //                     screenData[(x + scroll) + (line * 8 + y) * screenWidth] = screenDataOld[x + (line * 8 + y) * screenWidth];
    //                     // screenDataOld[x + (line * 8 + y) * screenWidth] = old;
    //                 }
    //             }
    //         }
    //     }
    // }

    // memcpy(screenDataOld, screenData, screenLength);

    // memcpy(screenData, screenDataOld, screenLength);
    //  Draw the sprites. Note that it is important to draw them exactly in this
    //  order, to have the correct priorities.
    element = allGfx[1];
    visibleArea = allGames[currentGame].video.visibleArea;
    /* old sprite draw
        for (int offset = 0x20 - 4; offset >= 0; offset -= 4)
        {
            if (boardMemory[0xb040 + offset + 3] != 0)
            {
                int x = boardMemory[0xb040 + offset];
                x = ((x << 4) & 0xf0) | ((x >> 4) & 0x0f);
                int col = boardMemory[0xb040 + offset + 2] & 7;
                col = ((col >> 1) & 0x03) | ((col << 2) & 0x04);
                if (col > maxc)
                {
                    maxc = col;
                    MY_DEBUG2("C2", "Max:", maxc)
                }
                // drawgfx(bitmap, Machine->gfx[1],
                //         spriteram[0xb040 + offset + 1] & 0x3f,
                //         col,
                //         spriteram[0xb040 + offset + 1] & 0x80, spriteram[0xb040 + offset + 1] & 0x40,
                //         x, spriteram[0xb040 + offset + 3],
                //         &Machine->drv->visible_area, TRANSPARENCY_PEN, 0);
                uint16_t tileAddress = 0xb040 + offset + 1;
                uint8_t tileIndex = boardMemory[tileAddress] & 0x3f;
                GameDrawElement(screenData, x, boardMemory[0xb040 + offset + 3], false, false, tileIndex, col, TRANSPARENCY_BLACK);
            }
        }
    */
    for (int spriteNumber = 7; spriteNumber >= 0; spriteNumber--)
    {
        const uint8_t *base = &boardMemory[0xb040 + spriteNumber * 4];
        if (base[3])
        {
            uint8_t base0 = ((base[0] >> 4) | (base[0] << 4));
            uint8_t sy = 240 - (base0 - (spriteNumber >= 3));
            uint16_t code = base[1] & 0x3f;
            uint8_t flipx = base[1] & 0x40;
            uint8_t flipy = base[1] & 0x80;
            uint8_t color = base[2] & 7;
            color = ((color >> 1) & 0x03) | ((color << 2) & 0x04);
            const int hoffset = 1;
            uint8_t sx = base[3] + hoffset;
            // sx = 240 - sx;
            sy = 240 - sy;
            GameDrawElement(screenData, sy, sx, flipx, flipy, code, color, TRANSPARENCY_BLACK, TRANSPARENT_NONE_COLOR);
            // if (last != color)
            // {
            //     last = color;
            //     MY_DEBUG2("CC", "Palette:", last)
            // } // O ou 3
            // if (color > maxc2)
            //{
            //    maxc2 = color;
            //    MY_DEBUG2("CC", "Max sprite:", maxc2)
            //} // 7
        }
    }
}