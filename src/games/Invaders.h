#ifndef INVADERS_H
#define INVADERS_H

#pragma once

#include "../MyDefine.h"
#include "../TheMemory.h"
#include "../GameDefinition.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern void invaders_videoram_w(int offset, int data);

#ifdef __cplusplus
}
#endif

#define INVADERS {"invaders", "Space Invaders", \
    { 224, 256, VISIBLE_AREA_FULL, ORIENTATION_DEFAULT, NOTHING, NOTHING}, \
    3000000 / 60, \
    { invaders_rom, NOTHING, invaders_readmem, invaders_writemem, invaders_input_ports, invaders_readport, invaders_writeport }, \
    MACHINE_8080BW}

ROM_START(invaders_rom)
ROM_REGION(0x10000)                                // 64k for code
ROM_LOAD("invaders.h", 0x0000, 0x0800, 0x734f5ad8) // 9316b-0869_m739h.h1
ROM_LOAD("invaders.g", 0x0800, 0x0800, 0x6bfaca4a) // 9316b-0856_m739g.g1
ROM_LOAD("invaders.f", 0x1000, 0x0800, 0x0ccead96) // 9316b-0855_m739f.f1
ROM_LOAD("invaders.e", 0x1800, 0x0800, 0x14e538b0) // 9316b-0854_m739e.e1
ROM_END

static struct MemoryReadAddress invaders_readmem[] =
{
	{ 0x0000, 0x1fff, MRA_ROM },
	{ 0x2000, 0x3fff, MRA_RAM },
	{ 0x4000, 0x57ff, MRA_ROM },
	{ -1 }
};

static struct MemoryWriteAddress invaders_writemem[] =
{
	{ 0x0000, 0x1fff, MWA_ROM },
	{ 0x2000, 0x23ff, MWA_RAM },
	{ 0x2400, 0x3fff, invaders_videoram_w }, //, &invaders_videoram },
	{ 0x4000, 0x57ff, MWA_ROM },
	{ -1 }
};

static struct IOReadPort invaders_readport[] =
    {
        {0x01, 0x01, readPort0},
        {0x02, 0x02, readPort1},
        {0x03, 0x03, mb14241_shift},
        {-1}
};

static struct IOWritePort invaders_writeport[] =
    {
        {0x02, 0x02, mb14241_amount},
        {0x03, 0x03, writePort3},
        {0x04, 0x04, mb14241_data},
        {0x05, 0x05, writePort5},
        {0x06, 0x06, writePort6},
        {-1}
};

INPUT_PORTS_START(invaders_input_ports)
PORT_START // IN0
    PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_COIN1)
        PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_START2)
            PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_START1)
                PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_UNKNOWN)
                    PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_BUTTON1)
                        PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT | IPF_2WAY)
                            PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT | IPF_2WAY)
                                PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNKNOWN)
                                    PORT_START // DSW0
    PORT_DIPNAME(0x03, 0x00, "Lives", IP_KEY_NONE)
        PORT_DIPSETTING(0x00, "3")
            PORT_DIPSETTING(0x01, "4")
                PORT_DIPSETTING(0x02, "5")
                    PORT_DIPSETTING(0x03, "6")
                        PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_TILT)
                            PORT_DIPNAME(0x08, 0x00, "Bonus", IP_KEY_NONE)
                                PORT_DIPSETTING(0x00, "1500")
                                    PORT_DIPSETTING(0x08, "1000")
                                        PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 | IPF_COCKTAIL)
                                            PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT | IPF_2WAY | IPF_COCKTAIL)
                                                PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT | IPF_2WAY | IPF_COCKTAIL)
                                                    PORT_DIPNAME(0x80, 0x00, "Coin Info", IP_KEY_NONE)
                                                        PORT_DIPSETTING(0x00, "On")
                                                            PORT_DIPSETTING(0x80, "Off")
                                                                PORT_START // BSR
    PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_UNKNOWN)
        PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_UNKNOWN)
            PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN)
                PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_UNKNOWN)
                    PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_UNKNOWN)
                        PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN)
                            PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN)
                                PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN)
                                    PORT_START // Dummy port for cocktail mode
    PORT_DIPNAME(0x01, 0x00, "Cabinet", IP_KEY_NONE)
        PORT_DIPSETTING(0x00, "Upright")
            PORT_DIPSETTING(0x01, "Cocktail")
                INPUT_PORTS_END

#endif