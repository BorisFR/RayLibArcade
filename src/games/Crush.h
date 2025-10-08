/*#ifndef CRUSH_H
#define CRUSH_H

#pragma once

#include "../MyDefine.h"
#include "../TheMemory.h"
#include "../GameDefinition.h"
#include "Pacman.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

#define CRUSH {"crush", "Crush Roller", {256, 288, {16, 256 - 16, 0, 288}, ORIENTATION_ROTATE_90, PacmanRefreshScreen, pacmanGfxDecodeInfo}, 3072000 / 60, {crush_rom, NOTHING, crush_readmem, pacman_writemem, crush_input_ports, NOTHING, pacman_writeport}, MACHINE_Z80}

ROM_START(crush_rom)
ROM_REGION(0x10000) // 64k for code
ROM_LOAD("crushkrl.6e", 0x0000, 0x1000, 0xa8dd8f54)
ROM_LOAD("crushkrl.6f", 0x1000, 0x1000, 0x91387299)
ROM_LOAD("crushkrl.6h", 0x2000, 0x1000, 0xd4455f27)
ROM_LOAD("crushkrl.6j", 0x3000, 0x1000, 0xd59fc251)

// ROM_REGION_DISPOSE(0x2000)	// temporary space for graphics (disposed after conversion)
ROM_REGION_GFX(0x2000)
ROM_LOAD("maketrax.5e", 0x0000, 0x1000, 0x91bad2da)
// ROM_REGION_SPRITE(0x1000)
ROM_LOAD("maketrax.5f", 0x1000, 0x1000, 0xaea79f55)

// ROM_REGION(0x0120)	// color PROMs
ROM_REGION_COLOR(0x20)
ROM_LOAD("82s123.7f", 0x0000, 0x0020, 0x2fc650bd)
ROM_REGION_PALETTE(0x0100)
ROM_LOAD("2s140.4a", 0x0000, 0x0100, 0x63efb927) // 0x2bc5d339 )

ROM_REGION_SND(0x0200) // sound PROMs
ROM_LOAD("82s126.1m", 0x0000, 0x0100, 0xa9cc86bf)
ROM_LOAD("82s126.3m", 0x0100, 0x0100, 0x77245b66) // timing - not used
ROM_END

static struct MemoryReadAddress crush_readmem[] =
    {
        {0x0000, 0x3fff, MRA_ROM},
        {0x4000, 0x47ff, MRA_RAM},   // video and color RAM 
        {0x4c00, 0x4fff, MRA_RAM},   // including sprite codes at 4ff0-4fff 
        {0x5000, 0x503f, readPort0}, // IN0 
        {0x5040, 0x507f, readPort1}, // IN1 
        //{0x5080, 0x50bf, maketrax_special_port2_r), //readPort2}, // DSW1 
        //{0x50c0, 0x50ff, maketrax_special_r), //readPort3}, // DSW2 
        {0x8000, 0xbfff, MRA_ROM},   // Ms. Pac Man / Ponpoko only 
        {-1}};

INPUT_PORTS_START(crush_input_ports)
PORT_START // IN0
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP | IPF_4WAY)
		PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT | IPF_4WAY)
			PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_4WAY)
				PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN | IPF_4WAY)
					PORT_DIPNAME(0x10, 0x00, "Cabinet", IP_KEY_NONE)
						PORT_DIPSETTING(0x00, "Upright")
							PORT_DIPSETTING(0x10, "Cocktail")
								PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_COIN1)
									PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_COIN2)
										PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_COIN3)

											PORT_START // IN1
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP | IPF_4WAY | IPF_COCKTAIL)
		PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT | IPF_4WAY | IPF_COCKTAIL)
			PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_4WAY | IPF_COCKTAIL)
				PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN | IPF_4WAY | IPF_COCKTAIL)
					PORT_DIPNAME(0x10, 0x10, "Unknown 1", IP_KEY_NONE)
						PORT_DIPSETTING(0x10, "Off")
							PORT_DIPSETTING(0x00, "On")
								PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_START1)
									PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_START2)
										PORT_DIPNAME(0x80, 0x80, "Unknown 2", IP_KEY_NONE)
											PORT_DIPSETTING(0x80, "Off")
												PORT_DIPSETTING(0x00, "On")

													PORT_START // DSW 1
	PORT_DIPNAME(0x03, 0x01, "Coinage", IP_KEY_NONE)
		PORT_DIPSETTING(0x03, "2 Coins/1 Credit")
			PORT_DIPSETTING(0x01, "1 Coin/1 Credit")
				PORT_DIPSETTING(0x02, "1 Coin/2 Credits")
					PORT_DIPSETTING(0x00, "Free Play")
						PORT_DIPNAME(0x0c, 0x00, "Lives", IP_KEY_NONE)
							PORT_DIPSETTING(0x00, "3")
								PORT_DIPSETTING(0x04, "4")
									PORT_DIPSETTING(0x08, "5")
										PORT_DIPSETTING(0x0c, "6")
											PORT_DIPNAME(0x10, 0x10, "First Pattern", IP_KEY_NONE)
												PORT_DIPSETTING(0x10, "Easy")
													PORT_DIPSETTING(0x00, "Hard")
														PORT_DIPNAME(0x20, 0x20, "Teleport holes", IP_KEY_NONE)
															PORT_DIPSETTING(0x20, "Off")
																PORT_DIPSETTING(0x00, "On")
																	PORT_DIPNAME(0x40, 0x40, "Unknown 3", IP_KEY_NONE)
																		PORT_DIPSETTING(0x40, "Off")
																			PORT_DIPSETTING(0x00, "On")
																				PORT_DIPNAME(0x80, 0x80, "Unknown 4", IP_KEY_NONE)
																					PORT_DIPSETTING(0x80, "Off")
																						PORT_DIPSETTING(0x00, "On")

																							PORT_START // DSW 2
																									   // PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )
	INPUT_PORTS_END

#endif
*/
	/*
	// https://github.com/mamedev/mame/blob/master/src/mame/pacman/pacman.cpp
			   ROM        PARENT    MACHINE   INP       STATE          INIT
	GAME( 1981, crush,    0,        korosuke, maketrax, pacman_state,  init_maketrax, ROT90,  "Alpha Denshi Co. / Kural Samno Electric, Ltd.", "Crush Roller (set 1)",                   MACHINE_SUPPORTS_SAVE )


	extern struct GameDriver maketrax_driver;
	struct GameDriver crush_driver =
	{
		__FILE__,
		&maketrax_driver,
		"crush",
		"Crush Roller",
		"1981",
		"bootleg",
		BASE_CREDITS"\nGary Walton (color info)\nSimon Walls (color info)",
		0,
		&machine_driver,
		0,

		crush_rom,
		0, 0,
		0,
		0,	// sound_prom

		crush_input_ports,

		PROM_MEMORY_REGION(2), 0, 0,
		ORIENTATION_ROTATE_90,

		crush_hiload, crush_hisave
	};


	static struct MachineDriver maketrax_machine_driver =
	{
		/ basic machine hardware
		{
			{
				CPU_Z80,
				3072000,        / 3.072 Mhz
				0,
				maketrax_readmem,maketrax_writemem,0,writeport,
				pacman_interrupt,1
			}
		},
		60, DEFAULT_60HZ_VBLANK_DURATION,	/ frames per second, vblank duration
		1,      / single CPU, no need for interleaving
		pacman_init_machine,

		/video hardware
		36*8, 28*8, { 0*8, 36*8-1, 0*8, 28*8-1 },
		gfxdecodeinfo,
		16, 4*32,
		pacman_vh_convert_color_prom,

		VIDEO_TYPE_RASTER | VIDEO_SUPPORTS_DIRTY,
		0,
		pacman_vh_start,
		generic_vh_stop,
		pengo_vh_screenrefresh,

		/ sound hardware
		0,0,0,0,
		{
			{
				SOUND_NAMCO,
				&namco_interface
			}
		}
	};

	*/