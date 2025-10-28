#ifndef FROGGER_H
#define FROGGER_H

#pragma once

#include "../MyDefine.h"
#include "../GameDefinition.h"

// https://github.com/mamedev/mame/blob/master/src/mame/galaxian/galaxian.cpp
// Frogger based hardware: 2nd Z80, AY-8910A, 2 8255 PPI for I/O, custom background

extern struct GfxLayout froggerTileLayout;
extern struct GfxLayout froggerSpriteLayout;

static struct GfxDecodeInfo froggerGfxDecodeInfo[] = {
    {ROM_GFX, 0x0000, &froggerTileLayout, 0, 16},
    {ROM_GFX, 0x0000, &froggerSpriteLayout, 0, 8},
    {-1}};

#ifdef __cplusplus
extern "C"
{
#endif

    extern void frogger_interrupt_enable_w(int offset, int data);
    // extern void frogger_attributes_w(int offset, int data);
    WRITE_HANDLER(frogger_videoram_w);
    //extern void frogger_videoram_w(int offset, int data);
    extern void frogger_objram_w(int offset, int data);
    extern void frogger_sh_irqtrigger_w(int offset, int data);

    extern void FroggerInit();
    extern void frogger_decode_rom();
    extern void FroggerRefreshScreen();

#ifdef __cplusplus
}
#endif

#define FROGGER {"frogger", "Frogger", {280, 32 * 8, 32 * 8, {2 * 8, 30 * 8 - 1, 0 * 8, 32 * 8 - 1}, ORIENTATION_DEFAULT, FroggerRefreshScreen, froggerGfxDecodeInfo}, 3072000 / 60, {frogger_rom, frogger_decode_rom, frogger_readmem, frogger_writemem, frogger_input_ports, NOTHING, NOTHING, FroggerInit}, MACHINE_Z80}

ROM_START(frogger_rom) // CPU 1
ROM_REGION(0x10000)    /* 64k for code */
ROM_LOAD("frogger.26", 0x0000, 0x1000, 0x597696d6)
ROM_LOAD("frogger.27", 0x1000, 0x1000, 0xb6e6fcc3)
ROM_LOAD("frsm3.7", 0x2000, 0x1000, 0xaca22ae0)
// ROM_REGION_GFX(0x1000) /* temporary space for graphics (disposed after conversion) */
ROM_REGION_GFX(0x1000)
ROM_LOAD("frogger.607", 0x0000, 0x0800, 0x05f7d883)
ROM_LOAD("frogger.606", 0x0800, 0x0800, 0xf524ee30)
ROM_REGION_COLOR(0x0020) /* color PROMs */
ROM_LOAD("pr-91.6l", 0x0000, 0x0020, 0x413703bf)
ROM_REGION_SND(0x10000) // CPU 2
ROM_LOAD("frogger.608", 0x0000, 0x0800, 0xe8ab0256)
ROM_LOAD("frogger.609", 0x0800, 0x0800, 0x7380a48f)
ROM_LOAD("frogger.610", 0x1000, 0x0800, 0x31d7eb27)
ROM_END

static struct MemoryReadAddress frogger_readmem[] = {
    {0x0000, 0x3fff, MRA_ROM},
    {0x8000, 0x87ff, MRA_RAM},
    {0x8800, 0x8800, boardMemoryRead0}, // watchdog_reset_r},
    {0xa800, 0xabff, MRA_RAM},          /* video RAM */
    {0xb000, 0xb0ff, MRA_RAM},   /* spriteram => screen attributes, sprites */
    {0xe000, 0xe000, readPort0}, /* IN0 */
    {0xe002, 0xe002, readPort1}, /* IN1 */
    {0xe004, 0xe004, readPort2}, /* IN2 */
    {-1}};

static struct MemoryWriteAddress frogger_writemem[] = {
    {0x0000, 0x3fff, MWA_ROM},
    {0x4400, 0x47ff, MWA_RAM},
    {0x8000, 0x87ff, MWA_RAM},
    //{0xa800, 0xabff, MWA_RAM},              // videoram_w, &videoram, &videoram_size},
    // map(0xa800, 0xabff).mirror(0x0400).ram().w(FUNC(galaxian_state::galaxian_videoram_w)).share("videoram");
    {0xa800, 0xabff, frogger_videoram_w, &videoram, &videoram_size},
    //{0xb000, 0xb03f, frogger_attributes_w}, //, &frogger_attributesram},
    //{0xb040, 0xb05f, MWA_RAM},              //, &spriteram, &spriteram_size},
    // map(0xb000, 0xb0ff).mirror(0x0700).ram().w(FUNC(galaxian_state::galaxian_objram_w)).share("spriteram");
    {0xb000, 0xb0ff, frogger_objram_w},
    {0xb808, 0xb808, frogger_interrupt_enable_w},
    {0xb818, 0xb818, MWA_ROM}, // coin_counter_w},
    {0xb81c, 0xb81c, MWA_ROM}, // frogger_counterb_w},
    {0xd000, 0xd000, MWA_ROM}, // soundlatch_w},
    {0xd002, 0xd002, frogger_sh_irqtrigger_w},
    {-1}};

INPUT_PORTS_START(frogger_input_ports)
PORT_START                                         // IN0
    PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP) // | IPF_4WAY | IPF_COCKTAIL)
    PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_UNKNOWN)     // 1P shoot2 - unused
    PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_COIN3)
        PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_UNKNOWN)    // 1P shoot1 - unused
    PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT) // | IPF_4WAY)
    PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT)  // | IPF_4WAY)
    PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_COIN2)
        PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_COIN1)

            PORT_START // IN1
    PORT_DIPNAME(0x03, 0x00, "Lives", IP_KEY_NONE)
        PORT_DIPSETTING(0x00, "3")
            PORT_DIPSETTING(0x01, "5")
                PORT_DIPSETTING(0x02, "7")
                    PORT_DIPNAME(0, 0x03, "256 (Cheat)", IP_KEY_NONE)
                        PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_UNKNOWN) // 2P shoot2 - unused
    PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_UNKNOWN)                     // 2P shoot1 - unused
    PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_4WAY | IPF_COCKTAIL)
        PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT | IPF_4WAY | IPF_COCKTAIL)
            PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_START2)
                PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_START1)

                    PORT_START // IN2
    PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN | IPF_4WAY | IPF_COCKTAIL)
        PORT_DIPNAME(0x06, 0x00, "Coinage", IP_KEY_NONE)
            PORT_DIPSETTING(0x02, "A 2/1 B 2/1 C 2/1")
                PORT_DIPSETTING(0x04, "A 2/1 B 1/3 C 2/1")
                    PORT_DIPSETTING(0x00, "A 1/1 B 1/1 C 1/1")
                        PORT_DIPSETTING(0x06, "A 1/1 B 1/6 C 1/1")
                            PORT_DIPNAME(0x08, 0x00, "Cabinet", IP_KEY_NONE)
                                PORT_DIPSETTING(0x00, "Upright")
                                    PORT_DIPSETTING(0x08, "Cocktail")
                                        PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_UP | IPF_4WAY)
                                            PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNUSED)
                                                PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN | IPF_4WAY)
                                                    PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNUSED)
                                                        PORT_START //("IN3")   // need for some PPI accesses
    PORT_BIT(0xff, 0x00, IPT_UNUSED)
        INPUT_PORTS_END

#endif