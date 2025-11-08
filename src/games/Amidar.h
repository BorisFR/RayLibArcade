#ifndef AMIDAR_H
#define AMIDAR_H

#pragma once

#include "../MyDefine.h"
#include "../TheMemory.h"
#include "../GameDefinition.h"

extern struct GfxLayout amidarTileLayout;
extern struct GfxLayout amidarSpriteLayout;

static struct GfxDecodeInfo amidarGfxDecodeInfo[] = {
    {ROM_GFX, 0x0000, &amidarTileLayout, 0, 8},
    {ROM_GFX, 0x1000, &amidarSpriteLayout, 0, 8},
    {-1}};

#ifdef __cplusplus
extern "C"
{
#endif

    WRITE_HANDLER(amidar_attributes_w);
    extern uint8_t *amidar_attributesram;
    WRITE_HANDLER(amidar_flipx_w);
    WRITE_HANDLER(amidar_flipy_w);
    WRITE_HANDLER(amidar_coina_w);
    WRITE_HANDLER(amidar_coinb_w);
    WRITE_HANDLER(amidar_soundlatch_w);
    WRITE_HANDLER(amidar_sh_irqtrigger_w);

    // READ_HANDLER(amidar_ppi8255_0_r);
    // READ_HANDLER(amidar_ppi8255_1_r);
    // WRITE_HANDLER(amidar_ppi8255_0_w);
    // WRITE_HANDLER(amidar_ppi8255_1_w);
    // WRITE_HANDLER(amidar_background_red_w);
    // WRITE_HANDLER(amidar_background_green_w);
    // WRITE_HANDLER(amidar_background_blue_w);

    extern void AmidarInit();
    extern void AmidarRefreshScreen();

#ifdef __cplusplus
}
#endif

#define AMIDAR {"amidar", "Amidar", {350, 32 * 8, 32 * 8, {2 * 8, 30 * 8 - 1, 0 * 8, 32 * 8 - 1}, ORIENTATION_ROTATE_90, AmidarRefreshScreen, amidarGfxDecodeInfo}, 3072000 / 60, {amidar_rom, NOTHING, amidar_readmem, amidar_writemem, amidar_input_ports, NOTHING, NOTHING, AmidarInit}, MACHINE_Z80}

ROM_START(amidar_rom)
ROM_REGION(0x10000)
ROM_LOAD("1.2c", 0x0000, 0x1000, 0x621b74de)
ROM_LOAD("2.2e", 0x1000, 0x1000, 0x38538b98)
ROM_LOAD("3.2f", 0x2000, 0x1000, 0x099ecb24)
ROM_LOAD("4.2h", 0x3000, 0x1000, 0xba149a93)
ROM_LOAD("5.2j", 0x4000, 0x1000, 0xeecc1abf)
ROM_MIRROR(0x8000, 0x87FF, 0X4000)
ROM_MIRROR(0x9000, 0x93FF, 0X4400)
ROM_MIRROR(0x9800, 0x98FF, 0X4700)
ROM_REGION_SND(0x10000)
ROM_LOAD("s1.5c", 0x0000, 0x1000, 0x8ca7b750)
ROM_LOAD("s2.5d", 0x1000, 0x1000, 0x9b5bdc0a)
ROM_REGION_GFX(0x1000)
ROM_LOAD("c2.5f", 0x0000, 0x0800, 0x2cfe5ede)
ROM_LOAD("c2.5d", 0x0800, 0x0800, 0x57c4fd0d)
ROM_REGION_COLOR(0x0020)
ROM_LOAD("amidar.clr", 0x0000, 0x0020, 0xf940dcc3)
ROM_END

// uint8_t turtles_ppi8255_0_r(offs_t offset){ return m_ppi8255[0]->read((offset >> 4) & 3); }
// uint8_t turtles_ppi8255_1_r(offs_t offset){ return m_ppi8255[1]->read((offset >> 4) & 3); }
// void turtles_ppi8255_0_w(offs_t offset, uint8_t data){ m_ppi8255[0]->write((offset >> 4) & 3, data); }
// void turtles_ppi8255_1_w(offs_t offset, uint8_t data){ m_ppi8255[1]->write((offset >> 4) & 3, data); }

static struct MemoryReadAddress amidar_readmem[] =
    {
        {0x0000, 0x4fff, MRA_ROM},
        {0x8000, 0x87ff, MRA_RAM},
        {0x9000, 0x93ff, MRA_RAM},
        {0x9800, 0x985f, MRA_RAM},
        //{ 0xa800, 0xa800, watchdog_reset_r },
        //{0xb000, 0xb03f, amidar_ppi8255_0_r},
        {0xb000, 0xb000, input_port_0_r}, /* IN0 */
        {0xb010, 0xb010, input_port_1_r}, /* IN1 */
        {0xb020, 0xb020, input_port_2_r}, /* IN2 */
        {0xb820, 0xb820, input_port_3_r}, /* DSW */
        //{0xb800, 0xb83f, amidar_ppi8255_1_r},
        {-1} /* end of table */
};

static struct MemoryWriteAddress amidar_writemem[] =
    {
        {0x0000, 0x4fff, MWA_ROM},
        {0x8000, 0x87ff, MWA_RAM},
        {0x9000, 0x93ff, videoram_w, &videoram, &videoram_size},
        {0x9800, 0x983f, amidar_attributes_w, &amidar_attributesram},
        {0x9840, 0x985f, MWA_RAM, &spriteram, &spriteram_size},
        {0x9860, 0x987f, MWA_NOP},
        //{0xa000, 0xa000, amidar_background_red_w},
        {0xa008, 0xa008, Z80_interrupt_enable_w},
        {0xa010, 0xa010, amidar_flipy_w},
        {0xa018, 0xa018, amidar_flipx_w},
        //{0xa020, 0xa020, amidar_background_green_w},
        //{0xa028, 0xa028, amidar_background_blue_w},
        {0xa030, 0xa030, amidar_coina_w},
        {0xa038, 0xa038, amidar_coinb_w},
        //{0xb000, 0xb03f, amidar_ppi8255_0_w},
        //{0xb800, 0xb83f, amidar_ppi8255_1_w},
        //{0xb800, 0xb800, amidar_soundlatch_w},
        {0xb810, 0xb810, amidar_sh_irqtrigger_w},
        {-1} /* end of table */
};

INPUT_PORTS_START(amidar_input_ports)
PORT_START /* IN0 */
    PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP | IPF_4WAY | IPF_COCKTAIL)
        PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_UNKNOWN) /* probably space for button 2 */
    PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_COIN3)
        PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_BUTTON1)
            PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_4WAY)
                PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT | IPF_4WAY)
                    PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_COIN2)
                        PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_COIN1)

                            PORT_START /* IN1 */
    PORT_DIPNAME(0x03, 0x03, "Lives", IP_KEY_NONE)
        PORT_DIPSETTING(0x03, "3")
            PORT_DIPSETTING(0x02, "4")
                PORT_DIPSETTING(0x01, "5")
                    PORT_BITX(0, 0x00, IPT_DIPSWITCH_SETTING | IPF_CHEAT, "255", IP_KEY_NONE, IP_JOY_NONE, 0)
                        PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_UNKNOWN) /* probably space for player 2 button 2 */
    PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_COCKTAIL)
        PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_4WAY | IPF_COCKTAIL)
            PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT | IPF_4WAY | IPF_COCKTAIL)
                PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_START2)
                    PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_START1)

                        PORT_START /* IN2 */
    PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN | IPF_4WAY | IPF_COCKTAIL)
        PORT_DIPNAME(0x02, 0x00, "Demo Sounds", IP_KEY_NONE)
            PORT_DIPSETTING(0x02, "Off")
                PORT_DIPSETTING(0x00, "On")
                    PORT_DIPNAME(0x04, 0x00, "Bonus Life", IP_KEY_NONE)
                        PORT_DIPSETTING(0x00, "30000 70000")
                            PORT_DIPSETTING(0x04, "50000 80000")
                                PORT_DIPNAME(0x08, 0x00, "Cabinet", IP_KEY_NONE)
                                    PORT_DIPSETTING(0x00, "Upright")
                                        PORT_DIPSETTING(0x08, "Cocktail")
                                            PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_UP | IPF_4WAY)
                                                PORT_DIPNAME(0x20, 0x00, "Unknown 1", IP_KEY_NONE)
                                                    PORT_DIPSETTING(0x20, "Off")
                                                        PORT_DIPSETTING(0x00, "On")
                                                            PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN | IPF_4WAY)
                                                                PORT_DIPNAME(0x80, 0x00, "Unknown 2", IP_KEY_NONE)
                                                                    PORT_DIPSETTING(0x80, "Off")
                                                                        PORT_DIPSETTING(0x00, "On")

                                                                            PORT_START /* DSW */
    PORT_DIPNAME(0x0f, 0x0f, "Coin A", IP_KEY_NONE)
        PORT_DIPSETTING(0x04, "4 Coins/1 Credit")
            PORT_DIPSETTING(0x0a, "3 Coins/1 Credit")
                PORT_DIPSETTING(0x01, "2 Coins/1 Credit")
                    PORT_DIPSETTING(0x02, "3 Coins/2 Credits")
                        PORT_DIPSETTING(0x08, "4 Coins/3 Credits")
                            PORT_DIPSETTING(0x0f, "1 Coin/1 Credit")
                                PORT_DIPSETTING(0x0c, "3 Coins/4 Credits")
                                    PORT_DIPSETTING(0x0e, "2 Coins/3 Credits")
                                        PORT_DIPSETTING(0x07, "1 Coin/2 Credits")
                                            PORT_DIPSETTING(0x06, "2 Coins/5 Credits")
                                                PORT_DIPSETTING(0x0b, "1 Coin/3 Credits")
                                                    PORT_DIPSETTING(0x03, "1 Coin/4 Credits")
                                                        PORT_DIPSETTING(0x0d, "1 Coin/5 Credits")
                                                            PORT_DIPSETTING(0x05, "1 Coin/6 Credits")
                                                                PORT_DIPSETTING(0x09, "1 Coin/7 Credits")
                                                                    PORT_DIPSETTING(0x00, "Free Play")
                                                                        PORT_DIPNAME(0xf0, 0xf0, "Coin B", IP_KEY_NONE)
                                                                            PORT_DIPSETTING(0x40, "4 Coins/1 Credit")
                                                                                PORT_DIPSETTING(0xa0, "3 Coins/1 Credit")
                                                                                    PORT_DIPSETTING(0x10, "2 Coins/1 Credit")
                                                                                        PORT_DIPSETTING(0x20, "3 Coins/2 Credits")
                                                                                            PORT_DIPSETTING(0x80, "4 Coins/3 Credits")
                                                                                                PORT_DIPSETTING(0xf0, "1 Coin/1 Credit")
                                                                                                    PORT_DIPSETTING(0xc0, "3 Coins/4 Credits")
                                                                                                        PORT_DIPSETTING(0xe0, "2 Coins/3 Credits")
                                                                                                            PORT_DIPSETTING(0x70, "1 Coin/2 Credits")
                                                                                                                PORT_DIPSETTING(0x60, "2 Coins/5 Credits")
                                                                                                                    PORT_DIPSETTING(0xb0, "1 Coin/3 Credits")
                                                                                                                        PORT_DIPSETTING(0x30, "1 Coin/4 Credits")
                                                                                                                            PORT_DIPSETTING(0xd0, "1 Coin/5 Credits")
                                                                                                                                PORT_DIPSETTING(0x50, "1 Coin/6 Credits")
                                                                                                                                    PORT_DIPSETTING(0x90, "1 Coin/7 Credits")
                                                                                                                                        PORT_DIPSETTING(0x00, "Disable All Coins")
                                                                                                                                            INPUT_PORTS_END

#endif