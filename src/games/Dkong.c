#include "Dkong.h"

struct GfxLayout dkongTileLayout =
    {
        8, 8,                     /* 8*8 characters */
        256,                      /* 256 characters */
        2,                        /* 2 bits per pixel */
        {256 * 8 * 8, 0},         /* the two bitplanes are separated */
        {0, 1, 2, 3, 4, 5, 6, 7}, /* pretty straightforward layout */
        {0 * 8, 1 * 8, 2 * 8, 3 * 8, 4 * 8, 5 * 8, 6 * 8, 7 * 8},
        8 * 8 /* every char takes 8 consecutive bytes */
};

struct GfxLayout dkongSpriteLayout =
    {
        16, 16,                  /* 16*16 sprites */
        128,                     /* 128 sprites */
        2,                       /* 2 bits per pixel */
        {128 * 16 * 16, 0},      /* the two bitplanes are separated */
        {0, 1, 2, 3, 4, 5, 6, 7, /* the two halves of the sprite are separated */
         64 * 16 * 16 + 0, 64 * 16 * 16 + 1, 64 * 16 * 16 + 2, 64 * 16 * 16 + 3, 64 * 16 * 16 + 4, 64 * 16 * 16 + 5, 64 * 16 * 16 + 6, 64 * 16 * 16 + 7},
        {0 * 8, 1 * 8, 2 * 8, 3 * 8, 4 * 8, 5 * 8, 6 * 8, 7 * 8, 8 * 8, 9 * 8, 10 * 8, 11 * 8, 12 * 8, 13 * 8, 14 * 8, 15 * 8},
        16 * 8 /* every sprite takes 16 consecutive bytes */
};

int flipscreen = 0;
int gfx_bank = 0;
int palette_bank = 0;
const unsigned char *dkong_colors_codes;

#define ACTIVELOW_PORT_BIT(P, A, D) ((P & (~(1 << A))) | ((D ^ 1) << A))

WRITE_HANDLER(dkong_sh_sound3_w) {}     // p[2] = ACTIVELOW_PORT_BIT(p[2], 5, data); }
WRITE_HANDLER(dkong_sh_sound4_w) {}     // t[1] = ~data & 1; }
WRITE_HANDLER(dkong_sh_sound5_w) {}     // t[0] = ~data & 1; }
WRITE_HANDLER(dkong_sh_tuneselect_w) {} // soundlatch_w(offset, data ^ 0x0f); }

WRITE_HANDLER(dkong_flipscreen_w) {}
WRITE_HANDLER(dkong_palettebank_w)
{
    int newbank;
    newbank = palette_bank;
    if (data & 1)
        newbank |= 1 << offset;
    else
        newbank &= ~(1 << offset);
    if (palette_bank != newbank)
    {
        palette_bank = newbank;
        // memset(dirtybuffer,1,videoram_size);
    }
}
WRITE_HANDLER(dkong_sh_w) {}
WRITE_HANDLER(dkong_sh1_w) {}

void dkong_vh_convert_color_prom(unsigned char *palette, unsigned short *colortable, const unsigned char *color_prom)
{
    int i;
// #define TOTAL_COLORS(gfxn) (Machine->gfx[gfxn]->total_colors * Machine->gfx[gfxn]->color_granularity)
#define TOTAL_COLORS(gfxn) (allGfx[gfxn]->total_colors * allGfx[gfxn]->color_granularity)
    // #define COLOR(gfxn,offs) (colortable[Machine->drv->gfxdecodeinfo[gfxn].color_codes_start + offs])
#define COLOR(gfxn,offs) (colortable[allGames[currentGame].video.decodeInfo->color_codes_start + offs])
    uint8_t nbColors = allGames[currentGame].video.decodeInfo->total_color_codes;
    for (i = 0; i < nbColors; i++)
    {
        int bit0, bit1, bit2;
        /* red component */
        bit0 = (color_prom[nbColors] >> 1) & 1;
        bit1 = (color_prom[nbColors] >> 2) & 1;
        bit2 = (color_prom[nbColors] >> 3) & 1;
        *(palette++) = 255 - (0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2);
        /* green component */
        bit0 = (color_prom[0] >> 2) & 1;
        bit1 = (color_prom[0] >> 3) & 1;
        bit2 = (color_prom[nbColors] >> 0) & 1;
        *(palette++) = 255 - (0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2);
        /* blue component */
        bit0 = (color_prom[0] >> 0) & 1;
        bit1 = (color_prom[0] >> 1) & 1;
        *(palette++) = 255 - (0x55 * bit0 + 0xaa * bit1);
        color_prom++;
    }
    color_prom += nbColors;
    /* color_prom now points to the beginning of the character color codes */
    dkong_colors_codes = color_prom; /* we'll need it later */
    /* sprites use the same palette as characters */
    for (i = 0; i < TOTAL_COLORS(0); i++)
        COLOR(0, i) = i;
}

int dkong_vh_start(void)
{
    gfx_bank = 0;
    palette_bank = 0;
    return 0;
}

#define DKONG_SPRITES_NUMBER 96

void DkongInit()
{
    GameInitTilesAndSprites(DKONG_SPRITES_NUMBER, 16, 16, 0x400, 8, 8);
}

void dkong_vh_screenrefresh()
{
    element = allGfx[0];
    int offs;

    /* for every character in the Video RAM, check if it has been modified */
    /* since last time and update it accordingly. */
    for (offs = videoram_size - 1; offs >= 0; offs--)
    {
        // if (dirtybuffer[offs])
        //{
        int sx, sy;
        int charcode, color;
        // dirtybuffer[offs] = 0;
        sx = offs % 32;
        sy = offs / 32;
        charcode = videoram[offs] + 256 * gfx_bank;
        /* retrieve the character color from the PROM */
        color = offs % 32 + 32 * (offs / 32 / 4);
        //color = (dkong_colors_codes[offs % 32 + 32 * (offs / 32 / 4)] & 0x0f) + 0x10 * palette_bank;
        if (flipscreen)
        {
            sx = 31 - sx;
            sy = 31 - sy;
        }
        GameDrawTile(offs, charcode, color, 8 * sx, 8 * sy, flipscreen, flipscreen);
        /*drawgfx(tmpbitmap,Machine->gfx[0],
                charcode,color,
                flipscreen,flipscreen,
                8*sx,8*sy,
                &Machine->drv->visible_area,TRANSPARENCY_NONE,0);*/
        //}
    }

    element = allGfx[1];
    // dkong_draw_sprites();
}
