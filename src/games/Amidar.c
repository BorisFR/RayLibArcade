#include "Amidar.h"

struct GfxLayout amidarTileLayout =
    {
        8, 8,             /* 8*8 characters */
        256,              /* 256 characters */
        2,                /* 2 bits per pixel */
        {0, 256 * 8 * 8}, /* the two bitplanes are separated */
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0 * 8, 1 * 8, 2 * 8, 3 * 8, 4 * 8, 5 * 8, 6 * 8, 7 * 8},
        8 * 8 /* every char takes 8 consecutive bytes */
};

struct GfxLayout amidarSpriteLayout =
    {
        16, 16,            /* 16*16 sprites */
        64,                /* 64 sprites */
        2,                 /* 2 bits per pixel */
        {0, 64 * 16 * 16}, /* the two bitplanes are separated */
        {0, 1, 2, 3, 4, 5, 6, 7, 8 * 8 + 0, 8 * 8 + 1, 8 * 8 + 2, 8 * 8 + 3, 8 * 8 + 4, 8 * 8 + 5, 8 * 8 + 6, 8 * 8 + 7},
        {0 * 8, 1 * 8, 2 * 8, 3 * 8, 4 * 8, 5 * 8, 6 * 8, 7 * 8, 16 * 8, 17 * 8, 18 * 8, 19 * 8, 20 * 8, 21 * 8, 22 * 8, 23 * 8},
        32 * 8 /* every sprite takes 32 consecutive bytes */
};

uint8_t *amidar_attributesram;

// *******************************************************************

WRITE_HANDLER(amidar_attributes_w)
{
    if ((offset & 1) && amidar_attributesram[offset] != data)
    {
        DirtyAll();
    }
    amidar_attributesram[offset] = data;
}

// *******************************************************************

WRITE_HANDLER(amidar_flipx_w)
{
    if (screenFlipWidth != (data & 1))
    {
        screenFlipWidth = data & 1;
        DirtyAll();
    }
}

// *******************************************************************

WRITE_HANDLER(amidar_flipy_w)
{
    if (screenFlipHeight != (data & 1))
    {
        screenFlipHeight = data & 1;
        DirtyAll();
    }
}

// *******************************************************************

WRITE_HANDLER(amidar_coina_w) {}

// *******************************************************************

WRITE_HANDLER(amidar_coinb_w) {}

// *******************************************************************

WRITE_HANDLER(amidar_soundlatch_w) {}

// *******************************************************************

WRITE_HANDLER(amidar_sh_irqtrigger_w)
{
    static int last;
    if (last == 0 && (data & 0x08) != 0)
    {
        Z80AskForNMI[Z80CurrentCpu] = true;
    }
    last = data & 0x08;
}

// *******************************************************************

void AmidarInit()
{
    GameInitTilesAndSprites(8, 16, 16, 0x400, 8, 8);
    visibleArea = allGames[currentGame].video.visibleArea;
}

// *******************************************************************

void AmidarRefreshScreen()
{
    // DirtyAll();
    element = allGfx[0];
    /* for every character in the Video RAM, check if it has been modified */
    /* since last time and update it accordingly. */
    for (int offset = videoram_size - 1; offset >= 0; offset--)
    {
        // if (dirtybuffer[offs])
        {
            // dirtybuffer[offs] = 0;
            int atX = offset % 32;
            int atY = offset / 32;
            if (screenFlipWidth)
                atX = 31 - atX;
            if (!screenFlipHeight)
                atY = 31 - atY;
            GameDrawTile(offset, videoram[offset], amidar_attributesram[2 * (offset % 32) + 1] & 0x07, atY * 8, atX * 8, false, false);
            /*drawgfx(tmpbitmap,Machine->gfx[0],
                    videoram[offset],
                    amidar_attributesram[2 * (offset % 32) + 1] & 0x07,
                    screenFlipWidth,screenFlipHeight,
                    8*atX,8*atY,
                    &Machine->drv->visible_area,TRANSPARENCY_NONE,0);*/
        }
    }

    element = allGfx[1];
    /* Draw the sprites. Note that it is important to draw them exactly in this */
    /* order, to have the correct priorities. */
    for (int offset = spriteram_size - 4; offset >= 0; offset -= 4)
    {
        int atX = (spriteram[offset + 3] + 1) & 0xff; /* ??? */
        int atY = 240 - spriteram[offset];
        int flipX = spriteram[offset + 1] & 0x40;
        int flipY = spriteram[offset + 1] & 0x80;
        if (screenFlipWidth)
        {
            atX = 241 - atX; /* note: 241, not 240 */
            flipX = !flipX;
        }
        if (!screenFlipHeight)
        {
            atY = 240 - atY;
            flipY = !flipY;
        }
        /* Sprites #0, #1 and #2 need to be offset one pixel to be correctly */
        /* centered on the ladders in Turtles (we move them down, but since this */
        /* is a rotated game, we actually move them left). */
        /* Note that the adjustement must be done AFTER handling flipscreen, thus */
        /* proving that this is a hardware related "feature" */
        if (offset <= 2 * 4)
            atY++;
        GameDrawSprite(offset / 4, spriteram[offset + 1] & 0x3f, spriteram[offset + 2] & 0x07, atX, atY, flipX, flipY);
        /*drawgfx(bitmap,Machine->gfx[1],
                spriteram[offset + 1] & 0x3f,
                spriteram[offset + 2] & 0x07,
                flipX,flipY,
                atX,atY,
                screenFlipWidth ? &spritevisibleareaflipx : &spritevisiblearea,TRANSPARENCY_PEN,0);*/
    }
}