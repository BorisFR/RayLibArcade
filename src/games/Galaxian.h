#ifndef GALAXIAN_H
#define GALAXIAN_H

#pragma once

#include "../MyDefine.h"
#include "../TheMemory.h"
#include "../GameDefinition.h"

extern struct GfxLayout galaxianTileLayout;
extern struct GfxLayout galaxianSpriteLayout;
extern struct GfxLayout galaxianBulletlayout;

static struct GfxDecodeInfo galaxianGfxDecodeInfo[] = {
    {ROM_GFX, 0x0000, &galaxianTileLayout, 0, 8},
    {ROM_GFX, 0x0000, &galaxianSpriteLayout, 0, 8},
    {ROM_GFX, 0x0000, &galaxianBulletlayout, 8 * 4, 2},
    {-1}};

#ifdef __cplusplus
extern "C"
{
#endif

    extern void galaxian_interrupt_enable_w(int offset, int data);
    extern void galaxian_stars_w(int offset, int data);
    extern void galaxian_flipx_w(int offset, int data);
    extern void galaxian_flipy_w(int offset, int data);

    extern void GalaxianInit();
    extern void GalaxianRefreshScreen();

#ifdef __cplusplus
}
#endif

#define GALAXIAN {"galaxian", "Galaxian", {270, 32 * 8, 32 * 8, {0 * 8, 32 * 8 - 0, 2 * 8, 30 * 8 - 0}, ORIENTATION_ROTATE_90, GalaxianRefreshScreen, galaxianGfxDecodeInfo}, 3072000 / 60, {galaxian_rom, NOTHING, galaxian_readmem, galaxian_writemem, galaxian_input_ports, NOTHING, NOTHING, GalaxianInit}, MACHINE_Z80}

ROM_START(galaxian_rom)
ROM_REGION(0x10000)
ROM_LOAD("galmidw.u", 0x0000, 0x0800, 0x745e2d61)
ROM_LOAD("galmidw.v", 0x0800, 0x0800, 0x9c999a40)
ROM_LOAD("galmidw.w", 0x1000, 0x0800, 0xb5894925)
ROM_LOAD("galmidw.y", 0x1800, 0x0800, 0x6b3ca10b)
ROM_LOAD("7l", 0x2000, 0x0800, 0x1b933207)
// ROM_REGION_DISPOSE(0x1000)
ROM_REGION_GFX(0x1000)
ROM_LOAD("1h.bin", 0x0000, 0x0800, 0x39fb43a4)
ROM_LOAD("1k.bin", 0x0800, 0x0800, 0x7e3f56a2)
// ROM_REGION(0x20)	/* color prom */
ROM_REGION_COLOR(0x020)
ROM_LOAD("6l.bpr", 0x0000, 0x020, 0xc3ac9467)
ROM_END

static struct MemoryReadAddress galaxian_readmem[] = {
    {0x0000, 0x3fff, MRA_ROM},
    {0x4000, 0x43ff, MRA_RAM},
    {0x5000, 0x5fff, MRA_RAM},   // video RAM, screen attributes, sprites, bullets
    {0x6000, 0x6000, readPort0}, // IN0
    {0x6800, 0x6800, readPort1}, // IN1
    {0x7000, 0x7000, readPort2}, // DSW
    {0x7800, 0x7800, boardMemoryRead0}, //watchdog_reset_r},
    {-1}
};

static struct MemoryWriteAddress galaxian_writemem[] = {
    {0x0000, 0x3fff, MWA_ROM},
    {0x4000, 0x43ff, MWA_RAM},
    {0x5000, 0x53ff, MWA_RAM}, // galaxian_videoram_w},   //, &videoram, &videoram_size },
    {0x5800, 0x583f, MWA_RAM}, // galaxian_attributes_w}, //, &galaxian_attributesram },
    {0x5840, 0x585f, MWA_RAM}, //, &spriteram, &spriteram_size },
    {0x5860, 0x587f, MWA_RAM}, //, &galaxian_bulletsram, &galaxian_bulletsram_size },
    {0x7001, 0x7001, galaxian_interrupt_enable_w},
    //{ 0x7800, 0x7800, mooncrst_pitch_w },
    //{ 0x6800, 0x6800, mooncrst_background_w },
    //{ 0x6803, 0x6803, mooncrst_noise_w },
    //{ 0x6805, 0x6805, mooncrst_shoot_w },
    //{ 0x6806, 0x6807, mooncrst_vol_w },
    //{ 0x6000, 0x6001, osd_led_w },
    //{ 0x6004, 0x6007, mooncrst_lfo_freq_w },
    {0x7004, 0x7004, galaxian_stars_w},
    {0x7006, 0x7006, galaxian_flipx_w},
    {0x7007, 0x7007, galaxian_flipy_w},
    {-1}};

INPUT_PORTS_START(galaxian_input_ports)
PORT_START // IN0
    PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_COIN1)
        PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_COIN2)
            PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT | IPF_2WAY)
                PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT | IPF_2WAY)
                    PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_BUTTON1)
                        PORT_DIPNAME(0x20, 0x00, "Cabinet", IP_KEY_NONE)
                            PORT_DIPSETTING(0x00, "Upright")
                                PORT_DIPSETTING(0x20, "Cocktail")
                                    PORT_BITX(0x40, 0x00, IPT_DIPSWITCH_NAME | IPF_TOGGLE, "Service Mode", OSD_KEY_F2, IP_JOY_NONE, 0)
                                        PORT_DIPSETTING(0x00, "Off")
                                            PORT_DIPSETTING(0x40, "On")
                                                PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_COIN3)
                                                    PORT_START // IN1
    PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_START1)
        PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_START2)
            PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT | IPF_2WAY | IPF_COCKTAIL)
                PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT | IPF_2WAY | IPF_COCKTAIL)
                    PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 | IPF_COCKTAIL)
                        PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN) // probably unused
    PORT_DIPNAME(0xc0, 0x00, "Coinage", IP_KEY_NONE)
        PORT_DIPSETTING(0x40, "2 Coins/1 Credit")
            PORT_DIPSETTING(0x00, "1 Coin/1 Credit")
                PORT_DIPSETTING(0x80, "1 Coin/2 Credits")
                    PORT_DIPSETTING(0xc0, "Free Play")

                        PORT_START // DSW0
    PORT_DIPNAME(0x03, 0x00, "Bonus Life", IP_KEY_NONE)
        PORT_DIPSETTING(0x00, "7000")
            PORT_DIPSETTING(0x01, "10000")
                PORT_DIPSETTING(0x02, "12000")
                    PORT_DIPSETTING(0x03, "20000")
                        PORT_DIPNAME(0x04, 0x04, "Lives", IP_KEY_NONE)
                            PORT_DIPSETTING(0x00, "2")
                                PORT_DIPSETTING(0x04, "3")
                                    PORT_DIPNAME(0x08, 0x00, "Unknown", IP_KEY_NONE)
                                        PORT_DIPSETTING(0x00, "Off")
                                            PORT_DIPSETTING(0x08, "On")
                                                PORT_BIT(0xf0, IP_ACTIVE_HIGH, IPT_UNUSED)
                                                    INPUT_PORTS_END

/*

0000-3fff


4000-7fff
  4000-47ff -> RAM read/write (10 bits = 0x400)
  4800-4fff -> n/c
  5000-57ff -> /VRAM RD or /VRAM WR (10 bits = 0x400)
  5800-5fff -> /OBJRAM RD or /OBJRAM WR (8 bits = 0x100)
  6000-67ff -> /SW0 or /DRIVER
  6800-6fff -> /SW1 or /SOUND
  7000-77ff -> /DIPSW or LATCH
  7800-7fff -> /WDR or /PITCH

/DRIVER: (write 6000-67ff)
  D0 = data bit
  A0-A2 = decoder
  6000 -> 1P START
  6001 -> 2P START
  6002 -> COIN LOCKOUT
  6003 -> COIN COUNTER
  6004 -> 1M resistor (controls 555 timer @ 9R)
  6005 -> 470k resistor (controls 555 timer @ 9R)
  6006 -> 220k resistor (controls 555 timer @ 9R)
  6007 -> 100k resistor (controls 555 timer @ 9R)

/SOUND: (write 6800-6fff)
  D0 = data bit
  A0-A2 = decoder
  6800 -> FS1 (enables 555 timer at 8R)
  6801 -> FS2 (enables 555 timer at 8S)
  6802 -> FS3 (enables 555 timer at 8T)
  6803 -> HIT
  6804 -> n/c
  6805 -> FIRE
  6806 -> VOL1
  6807 -> VOL2

LATCH: (write 7000-77ff)
  D0 = data bit
  A0-A2 = decoder
  7000 -> n/c
  7001 -> NMI ON
  7002 -> n/c
  7003 -> n/c
  7004 -> STARS ON
  7005 -> n/c
  7006 -> HFLIP
  7007 -> VFLIP

/PITCH: (write 7800-7fff)
  loads latch at 9J

static struct MachineDriver galaxian_machine_driver =
{
    // basic machine hardware
    {
        {
            CPU_Z80,
            18432000/6,	// 3.072 Mhz
            0,
            readmem,galaxian_writemem,0,0,
            galaxian_vh_interrupt,1
        }
    },
    60, 2500,	// frames per second, vblank duration
    1,	// single CPU, no need for interleaving
    0,

    // video hardware
    32*8, 32*8, { 0*8, 32*8-1, 2*8, 30*8-1 },
    galaxian_gfxdecodeinfo,
    32+64,8*4+2*2,	// 32 for the characters, 64 for the stars
    galaxian_vh_convert_color_prom,

    VIDEO_TYPE_RASTER,
    0,
    galaxian_vh_start,
    generic_vh_stop,
    galaxian_vh_screenrefresh,

    // sound hardware
    0,0,0,0,
    {
        {
            SOUND_CUSTOM,
            &custom_interface
        }
    }
};


struct GameDriver galaxian_driver =
{
    __FILE__,
    0,
    "galaxian",
    "Galaxian (Namco)",
    "1979",
    "Namco",
    "Robert Anschuetz\nNicola Salmoria\nAndrew Scott\nMarco Cassili",
    0,
    &galaxian_machine_driver,
    0,

    galaxian_rom,
    0, 0,
    mooncrst_sample_names,
    0,	// sound_prom

    galaxian_input_ports,

    PROM_MEMORY_REGION(2), 0, 0,
    ORIENTATION_ROTATE_90,

    galaxian_hiload, galaxian_hisave
};

*/
#endif