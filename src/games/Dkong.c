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

// the audio cpu is a mb8884 which in turn is 8048/49 compatible
struct i8048_state_S cpu_8048;
unsigned char colortable_select = 0;
// sound effects register between 8048 and z80
unsigned char dkong_sfx_index = 0x00;

#define DKONG_AUDIO_QUEUE_LEN 16
#define DKONG_AUDIO_QUEUE_MASK (DKONG_AUDIO_QUEUE_LEN - 1)

static unsigned char dkong_audio_assembly_buffer[64];
unsigned char dkong_audio_transfer_buffer[DKONG_AUDIO_QUEUE_LEN][64];
unsigned char dkong_audio_rptr = 0, dkong_audio_wptr = 0;

void dkong_doinit()
{
    i8048_reset(&cpu_8048);
    // audio_dkong_bitrate(machine == MCH_DKONG);
}

void dkong_doloop()
{
    if (((dkong_audio_wptr + 1) & DKONG_AUDIO_QUEUE_MASK) != dkong_audio_rptr)
    {
        i8048_step(&cpu_8048);
        i8048_step(&cpu_8048);
        i8048_step(&cpu_8048);
        i8048_step(&cpu_8048);
        i8048_step(&cpu_8048);
        i8048_step(&cpu_8048);
    }
}

void i8048_port_write(struct i8048_state_S *state, unsigned char port, unsigned char pos)
{
    if (port == 0)
        return;

    else if (port == 1)
    {
        static int bptr = 0;

        dkong_audio_assembly_buffer[bptr++] = pos;

        // buffer now full?
        if (bptr == 64)
        {
            bptr = 0;

            // It must never happen that we get here with no free transfer buffers
            // available. This would mean that the buffers were full and the
            // 8048 emulation was still running. It should be stoppped as long as the
            // buffers are full.
            if (((dkong_audio_wptr + 1) & DKONG_AUDIO_QUEUE_MASK) == dkong_audio_rptr)
            {
                // overflow
            }
            else
            {
                // copy data into transfer buffer
                memcpy(dkong_audio_transfer_buffer[dkong_audio_wptr], dkong_audio_assembly_buffer, 64);
                dkong_audio_wptr = (dkong_audio_wptr + 1) & DKONG_AUDIO_QUEUE_MASK;
            }
        }
    }
    else if (port == 2)
        state->p2_state = pos;
}

unsigned char i8048_port_read(struct i8048_state_S *state, unsigned char port)
{
    if (port == 2)
        return state->p2_state;
    return 0;
}

void i8048_xdm_write(struct i8048_state_S *state, unsigned char addr, unsigned char data) {}

unsigned char i8048_xdm_read(struct i8048_state_S *state, unsigned char addr)
{
    // inverted Z80 MUS register
    if (state->p2_state & 0x40)
        return dkong_sfx_index ^ 0x0f;

    return soundMemory[2048 + addr + 256 * (state->p2_state & 7)];
    //return dkong_rom_cpu2[2048 + addr + 256 * (state->p2_state & 7)];
}

// this is inlined in emulation.h
// unsigned char i8048_rom_read(struct i8048_state_S *state, unsigned short addr) {
//  return dkong_rom_cpu2[addr];
//}

int flipscreen = 0;
int gfx_bank = 0;
int palette_bank = 0;
const unsigned char *dkong_colors_codes;

#define ACTIVELOW_PORT_BIT(P, A, D) ((P & (~(1 << A))) | ((D ^ 1) << A))

WRITE_HANDLER(dkong_sh_sound3_w) {}                              // p[2] = ACTIVELOW_PORT_BIT(p[2], 5, data); }
WRITE_HANDLER(dkong_sh_sound4_w) {}                              // t[1] = ~data & 1; }
WRITE_HANDLER(dkong_sh_sound5_w) {}                              // t[0] = ~data & 1; }
WRITE_HANDLER(dkong_sh_tuneselect_w) { dkong_sfx_index = data; } // soundlatch_w(offset, data ^ 0x0f); }

WRITE_HANDLER(dkong_flipscreen_w) {}
WRITE_HANDLER(dkong_palettebank_w)
{
    if(offset == 0x7d86) {
      colortable_select &= ~1;
      colortable_select |= (data & 1);
    }
    
    if(offset == 0x7d87) {
      colortable_select &= ~2;
      colortable_select |= ((data<<1) & 2);
    }
/*        
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
    }*/
}
WRITE_HANDLER(dkong_sh_w) {cpu_8048.notINT = !(data & 1); }
WRITE_HANDLER(dkong_sh1_w)
{
    // trigger samples 0 (walk), 1 (jump) and 2 (stomp)
    //if ((offset & 0x0f) <= 2 && data)
    //    dkong_trigger_sound(offset & 0x0f);

    if ((offset & 0x0f) == 3)
    {
        if (data & 1)
            cpu_8048.p2_state &= ~0x20;
        else
            cpu_8048.p2_state |= 0x20;
    }

    if ((offset & 0x0f) == 4)
        cpu_8048.T1 = !(data & 1);

    if ((offset & 0x0f) == 5)
        cpu_8048.T0 = !(data & 1);
}

void dkong_vh_convert_color_prom(unsigned char *palette, unsigned short *colortable, const unsigned char *color_prom)
{
    int i;
// #define TOTAL_COLORS(gfxn) (Machine->gfx[gfxn]->total_colors * Machine->gfx[gfxn]->color_granularity)
#define TOTAL_COLORS(gfxn) (allGfx[gfxn]->total_colors * allGfx[gfxn]->color_granularity)
    // #define COLOR(gfxn,offs) (colortable[Machine->drv->gfxdecodeinfo[gfxn].color_codes_start + offs])
#define COLOR(gfxn, offs) (colortable[allGames[currentGame].video.decodeInfo->color_codes_start + offs])
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
    dkong_doinit();
    GameInitTilesAndSprites(DKONG_SPRITES_NUMBER, 16, 16, 0x400, 8, 8);
    visibleArea = allGames[currentGame].video.visibleArea;
}

void dkong_vh_screenrefresh()
{
    dkong_doloop();
    element = allGfx[0];
    int offs;

    /* for every character in the Video RAM, check if it has been modified */
    /* since last time and update it accordingly. */
    for (offs = videoram_size - 1; offs >= 0; offs--)
    {
        uint16_t tileX = offs % 32;
        uint16_t tileY = offs / 32;
        uint8_t onScreen = videoram[offs];
        uint32_t palette = offs % 32 + 32 * (offs / 32 / 4) + 0x10 * palette_bank;
        //if (gameTilesValue[offs] != onScreen || gameTilesColor[offs] != palette)
        {
            gameTilesValue[offs] = onScreen;
            gameTilesColor[offs] = palette;
            /* retrieve the character color from the PROM */
            // color = (dkong_colors_codes[offs % 32 + 32 * (offs / 32 / 4)] & 0x0f) + 0x10 * palette_bank;
            if (flipscreen)
            {
                tileX = 31 - tileX;
                tileY = 31 - tileY;
            }
            uint8_t *tile = gfxMemory;
            *tile += videoram[offs] + 256 * gfx_bank;
            //GameDrawTile(offs, *tile, palette , 8 * tileX, 8 * tileY, flipscreen, flipscreen);
            //return;
            // uint8_t *tile = &gfxMemory[videoram[offs] + 256 * gfx_bank];
            for (uint16_t y = 0; y < 8; y++)
            {
                uint16_t tempY = tileY * 8 + y;
                uint32_t index = tempY * screenGameWidth;
                unsigned short pix = *tile++;
                for (uint16_t x = 0; x < 8; x++)
                {
                    uint16_t tempX = x + tileX * 8;
                    if (pix & 3)
                    {
                        // uint16_t color = paletteColor[palette * 4 + (pix & 3)];
                        uint16_t color = colorMemory[palette + (pix & 3)];
                        DirtyAdd(screenGame, color, DIRTY_YES, tempX, tempY);
                    }
                }
            }
        }
    }

    element = allGfx[1];
    // dkong_draw_sprites();
}
