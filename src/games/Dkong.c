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

#define ACTIVELOW_PORT_BIT(P, A, D) ((P & (~(1 << A))) | ((D ^ 1) << A))

WRITE_HANDLER(dkong_sh_sound3_w) {}     // p[2] = ACTIVELOW_PORT_BIT(p[2], 5, data); }
WRITE_HANDLER(dkong_sh_sound4_w) {}     // t[1] = ~data & 1; }
WRITE_HANDLER(dkong_sh_sound5_w) {}     // t[0] = ~data & 1; }
WRITE_HANDLER(dkong_sh_tuneselect_w) {} // soundlatch_w(offset, data ^ 0x0f); }

WRITE_HANDLER(dkong_flipscreen_w) {}
WRITE_HANDLER(dkong_palettebank_w) {}
WRITE_HANDLER(dkong_sh_w) {}
WRITE_HANDLER(dkong_sh1_w) {}

void dkong_vh_convert_color_prom(unsigned char *palette, unsigned short *colortable, const unsigned char *color_prom)
{
}

int dkong_vh_start(void)
{
    return 0;
}

void dkong_vh_screenrefresh()
{
}
