#ifndef PACMAN_H
#define PACMAN_H

#pragma once

#include "../MyDefine.h"
#include "../TheMemory.h"
#include "../GameDefinition.h"

extern struct GfxLayout pacmanTileLayout;
extern struct GfxLayout pacmanSpriteLayout;

static struct GfxDecodeInfo pacmanGfxDecodeInfo[] =
    {
        {ROM_GFX, 0x0000, &pacmanTileLayout, 0, 32},
        {ROM_GFX, 0x1000, &pacmanSpriteLayout, 0, 32},
        {-1}};

#ifdef __cplusplus
extern "C"
{
#endif

    extern void pacman_videoram_w(int offset, int data);
    extern void pacman_colorram_w(int offset, int data);
    extern void pacman_interrupt_enable_w(int offset, int data);
    extern void pacman_sound_enable_w(int offset, int data);
    extern void pacman_flipscreen_w(int offset, int data);
    extern void pacman_osd_led_w(int offset, int data);
    extern void coin_counter_w(int offset, int data);
    extern void pacman_sound_w(int offset, int data);
    extern void pacman_watchdog_reset_w(int offset, int data);

    extern void pacman_interrupt_vector_w(int offset, int data);

    extern void PacmanRefreshScreen();

#ifdef __cplusplus
}
#endif

#define PACMAN {"pacman", "Pacman", {256, 288, {16, 256 - 16, 0, 288}, ORIENTATION_ROTATE_90, PacmanRefreshScreen, pacmanGfxDecodeInfo}, 3072000 / 60, {pacman_rom, NOTHING, pacman_readmem, pacman_writemem, pacman_input_ports, NOTHING, pacman_writeport}, MACHINE_Z80}

ROM_START(pacman_rom)
ROM_REGION(0x10000)
ROM_LOAD("pacman.6e", 0x0000, 0x1000, 0xc1e6ab10)
ROM_LOAD("pacman.6f", 0x1000, 0x1000, 0x1a6fb2d4)
ROM_LOAD("pacman.6h", 0x2000, 0x1000, 0xbcdd1beb)
ROM_LOAD("pacman.6j", 0x3000, 0x1000, 0x817d94e3)
// ROM_REGION_GFX(0x2000) // temporary space for graphics (disposed after conversion)
ROM_REGION_GFX(0x2000)
ROM_LOAD("pacman.5e", 0x0000, 0x1000, 0x0c944964)
// ROM_REGION_SPRITE(0x1000)
ROM_LOAD("pacman.5f", 0x1000, 0x1000, 0x958fedf9)
// ROM_REGION_COLOR(0x0120) // color PROMs
ROM_REGION_COLOR(0x020)
ROM_LOAD("82s123.7f", 0x0000, 0x0020, 0x2fc650bd)
ROM_REGION_PALETTE(0x0100)
ROM_LOAD("82s126.4a", 0x0000, 0x0100, 0x3eb3a8e4)
ROM_REGION_SND(0x0200) // sound PROMs
ROM_LOAD("82s126.1m", 0x0000, 0x0100, 0xa9cc86bf)
ROM_LOAD("82s126.3m", 0x0100, 0x0100, 0x77245b66) // timing - not used
ROM_END

static struct MemoryReadAddress pacman_readmem[] =
    {
        {0x0000, 0x3fff, MRA_ROM},
        {0x4000, 0x47ff, MRA_RAM},   /* video and color RAM */
        {0x4c00, 0x4fff, MRA_RAM},   /* including sprite codes at 4ff0-4fff */
        {0x5000, 0x503f, readPort0}, /* IN0 */
        {0x5040, 0x507f, readPort1}, /* IN1 */
        {0x5080, 0x50bf, readPort2}, /* DSW1 */
        {0x50c0, 0x50ff, readPort3}, /* DSW2 */
        {0x8000, 0xbfff, MRA_ROM},   /* Ms. Pac Man / Ponpoko only */
        {-1}};

static struct MemoryWriteAddress pacman_writemem[] =
    {
        {0x0000, 0x3fff, MWA_ROM},
        {0x4000, 0x43ff, pacman_videoram_w}, //, &videoram, &videoram_size },
        {0x4400, 0x47ff, pacman_colorram_w}, //, &colorram },
        {0x4c00, 0x4fef, MWA_RAM},
        {0x4ff0, 0x4fff, MWA_RAM}, //, &spriteram, &spriteram_size }, //8 pairs of two bytes:
                                   // the first byte contains the sprite image number (bits 2-7), Y flip (bit 0),
                                   // X flip (bit 1); the second byte the color
        {0x5000, 0x5000, pacman_interrupt_enable_w},
        {0x5001, 0x5001, pacman_sound_enable_w},
        {0x5002, 0x5002, MWA_NOP},
        {0x5003, 0x5003, pacman_flipscreen_w},
        {0x5004, 0x5005, pacman_osd_led_w},
        {0x5006, 0x5006, MWA_NOP},
        {0x5007, 0x5007, coin_counter_w},
        {0x5040, 0x505f, pacman_sound_w}, //, &pengo_soundregs },
        {0x5060, 0x506f, MWA_RAM},        //, &spriteram_2 }, //Sprite coordinates, x/y pairs for 8 sprites
        {0x50c0, 0x50c0, pacman_watchdog_reset_w},
        {0x8000, 0xbfff, MWA_ROM},           // Ms. Pac Man / Ponpoko only
        {0xc000, 0xc3ff, pacman_videoram_w}, // mirror address for video ram,
        {0xc400, 0xc7ef, pacman_colorram_w}, // used to display HIGH SCORE and CREDITS
        {0xffff, 0xffff, MWA_NOP},           // Eyes writes to this location to simplify code
        {-1}};

static struct IOWritePort pacman_writeport[] =
    {
        {0, 0, pacman_interrupt_vector_w}, /* Pac Man only */
        {-1}                               /* end of table */
};

INPUT_PORTS_START(pacman_input_ports)
PORT_START /* IN0 */
    PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP | IPF_4WAY)
        PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT | IPF_4WAY)
            PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_4WAY)
                PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN | IPF_4WAY)
                    PORT_DIPNAME(0x10, 0x10, "Rack Test", IP_KEY_NONE)
                        PORT_DIPSETTING(0x10, "Off")
                            PORT_DIPSETTING(0x00, "On")
                                PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_COIN1)
                                    PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_COIN2)
                                        PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_COIN3)

                                            PORT_START /* IN1 */
    PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP | IPF_4WAY | IPF_COCKTAIL)
        PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT | IPF_4WAY | IPF_COCKTAIL)
            PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_4WAY | IPF_COCKTAIL)
                PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN | IPF_4WAY | IPF_COCKTAIL)
                    PORT_DIPNAME(0x10, 0x10, "Service Mode", IP_KEY_NONE)
                        PORT_DIPSETTING(0x10, "Off")
                            PORT_DIPSETTING(0x00, "On")
                                PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_START1)
                                    PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_START2)
                                        PORT_DIPNAME(0x80, 0x80, "Cabinet", IP_KEY_NONE)
                                            PORT_DIPSETTING(0x80, "Upright")
                                                PORT_DIPSETTING(0x00, "Cocktail")

                                                    PORT_START /* DSW 1 */
    PORT_DIPNAME(0x03, 0x01, "Coinage", IP_KEY_NONE)
        PORT_DIPSETTING(0x03, "2 Coins/1 Credit")
            PORT_DIPSETTING(0x01, "1 Coin/1 Credit")
                PORT_DIPSETTING(0x02, "1 Coin/2 Credits")
                    PORT_DIPSETTING(0x00, "Free Play")
                        PORT_DIPNAME(0x0c, 0x08, "Lives", IP_KEY_NONE)
                            PORT_DIPSETTING(0x00, "1")
                                PORT_DIPSETTING(0x04, "2")
                                    PORT_DIPSETTING(0x08, "3")
                                        PORT_DIPSETTING(0x0c, "5")
                                            PORT_DIPNAME(0x30, 0x00, "Bonus Life", IP_KEY_NONE)
                                                PORT_DIPSETTING(0x00, "10000")
                                                    PORT_DIPSETTING(0x10, "15000")
                                                        PORT_DIPSETTING(0x20, "20000")
                                                            PORT_DIPSETTING(0x30, "Never")
                                                                PORT_DIPNAME(0x40, 0x40, "Difficulty", IP_KEY_NONE)
                                                                    PORT_DIPSETTING(0x40, "Normal")
                                                                        PORT_DIPSETTING(0x00, "Hard")
                                                                            PORT_DIPNAME(0x80, 0x80, "Ghost Names", IP_KEY_NONE)
                                                                                PORT_DIPSETTING(0x80, "Normal")
                                                                                    PORT_DIPSETTING(0x00, "Alternate")

                                                                                        PORT_START /* DSW 2 */
    // PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )

    INPUT_PORTS_END

#endif