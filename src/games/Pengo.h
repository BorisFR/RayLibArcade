#ifndef PENGO_H
#define PENGO_H

#pragma once

#include "../MyDefine.h"
#include "../TheMemory.h"
#include "../GameDefinition.h"

extern struct GfxLayout pengoTileLayout;
extern struct GfxLayout pengoSpriteLayout;

static struct GfxDecodeInfo pengoGfxDecodeInfo[] =
	{
		{ROM_GFX, 0x0000, &pengoTileLayout, 0, 32}, /* first bank */
		{ROM_GFX, 0x1000, &pengoSpriteLayout, 0, 32},
		{ROM_GFX, 0x2000, &pengoTileLayout, 4 * 32, 32}, /* second bank */
		{ROM_GFX, 0x3000, &pengoSpriteLayout, 4 * 32, 32},
		{-1}};
// static struct GfxDecodeInfo pengoGfxDecodeInfo[] =
//	{
//		{ROM_GFX, 0x0000, &pengoTileLayout, 0, 128},
//		{ROM_GFX, 0x2000, &pengoSpriteLayout, 0, 128},
//		{-1}};

#ifdef __cplusplus
extern "C"
{
#endif

	WRITE_HANDLER(pengo_interrupt_enable_w);
	WRITE_HANDLER(pengo_gfxbank_w);
	WRITE_HANDLER(pengo_flipscreen_w);

	extern void PengoDecodeRoms();
	extern void PengoInit();
	extern void PengoRefreshScreen();

#ifdef __cplusplus
}
#endif

#define PENGO {"pengo", "Pengo", {350, 36 * 8, 28 * 8, {0 * 8, 36 * 8 - 1, 0 * 8, 28 * 8 - 1}, ORIENTATION_ROTATE_90, PengoRefreshScreen, pengoGfxDecodeInfo}, 3072000 / 60, {pengo_rom, PengoDecodeRoms, pengo_readmem, pengo_writemem, pengo_input_ports, NOTHING, NOTHING, PengoInit}, MACHINE_Z80}

ROM_START(pengo_rom)
ROM_REGION(2 * 0x10000)
ROM_LOAD("epr-5128.ic8", 0x0000, 0x1000, 0x3dfeb20e)
ROM_LOAD("epr-5129.ic7", 0x1000, 0x1000, 0x1db341bd)
ROM_LOAD("epr-5130.ic15", 0x2000, 0x1000, 0x7c2842d5)
ROM_LOAD("epr-5131a.ic14", 0x3000, 0x1000, 0x6e3c1f2f)
ROM_LOAD("epr-5132.ic21", 0x4000, 0x1000, 0x95f354ff)
ROM_LOAD("epr-5133.ic20", 0x5000, 0x1000, 0x0fdb04b8)
ROM_LOAD("epr-5134.ic32", 0x6000, 0x1000, 0xe5920728)
ROM_LOAD("epr-5135a.ic31", 0x7000, 0x1000, 0x13de47ed)

// ROM_REGION_DISPOSE(0x4000)	/* temporary space for graphics (disposed after conversion) */
ROM_REGION_GFX(0x4000)
ROM_LOAD("epr-1640.ic92", 0x0000, 0x2000, 0xd7eec6cd)
ROM_LOAD("epr-1695.ic105", 0x2000, 0x2000, 0x5bfd26e9)

// ROM_REGION(0x0420)	/* color PROMs */
ROM_REGION_COLOR(0x0420)
ROM_LOAD("pr1633.ic78", 0x0000, 0x0020, 0x3a5844ec)
ROM_LOAD("pr1634.ic88", 0x0020, 0x0400, 0x766b139b)

// ROM_REGION(0x0200)	/* sound PROMs */
ROM_REGION_SND(0x0200)
ROM_LOAD("pr1635.ic51", 0x0000, 0x0100, 0xc29dea27)
ROM_LOAD("pr1636.ic70", 0x0100, 0x0100, 0x77245b66) /* timing - not used */
ROM_END

static struct MemoryReadAddress pengo_readmem[] =
	{
		{0x0000, 0x7fff, MRA_ROM_DECODE},
		{0x8000, 0x8fff, MRA_RAM},		  /* video and color RAM, scratchpad RAM, sprite codes */
		{0x9000, 0x903f, input_port_3_r}, /* DSW1 */
		{0x9040, 0x907f, input_port_2_r}, /* DSW0 */
		{0x9080, 0x90bf, input_port_1_r}, /* IN1 */
		{0x90c0, 0x90ff, input_port_0_r}, /* IN0 */
		{-1}};

static struct MemoryWriteAddress pengo_writemem[] =
	{
		{0x0000, 0x7fff, MWA_ROM},
		{0x8000, 0x83ff, videoram_w, &videoram, &videoram_size},
		{0x8400, 0x87ff, colorram_w, &colorram},
		{0x8800, 0x8fef, MWA_RAMROM },
		{0x8ff0, 0x8fff, MWA_RAM, &spriteram, &spriteram_size},
		{0x9000, 0x901f, MWA_RAM}, // pengo_sound_w, &pengo_soundregs },
		{0x9020, 0x902f, MWA_RAM, &spriteram_2},
		{0x9040, 0x9040, pengo_interrupt_enable_w},
		{0x9041, 0x9041, MWA_RAM}, // pengo_sound_enable_w },
		{0x9042, 0x9042, MWA_NOP},
		{0x9043, 0x9043, pengo_flipscreen_w},
		{0x9044, 0x9046, MWA_NOP},
		{0x9047, 0x9047, pengo_gfxbank_w},
		{0x9070, 0x9070, MWA_NOP},
		{-1}};

INPUT_PORTS_START(pengo_input_ports)
PORT_START /* IN0 */
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP | IPF_4WAY)
		PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN | IPF_4WAY)
			PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT | IPF_4WAY)
				PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_4WAY)
	/* the coin input must stay low for no less than 2 frames and no more */
	/* than 9 frames to pass the self test check. */
	/* Moreover, this way we avoid the game freezing until the user releases */
	/* the "coin" key. */
	PORT_BITX(0x10, IP_ACTIVE_LOW, IPT_COIN1 | IPF_IMPULSE,
			  "Coin A", IP_KEY_DEFAULT, IP_JOY_DEFAULT, 2)
		PORT_BITX(0x20, IP_ACTIVE_LOW, IPT_COIN2 | IPF_IMPULSE,
				  "Coin B", IP_KEY_DEFAULT, IP_JOY_DEFAULT, 2)
	/* Coin Aux doesn't need IMPULSE to pass the test, but it still needs it */
	/* to avoid the freeze. */
	PORT_BITX(0x40, IP_ACTIVE_LOW, IPT_COIN3 | IPF_IMPULSE,
			  "Coin Aux", IP_KEY_DEFAULT, IP_JOY_DEFAULT, 2)
		PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_BUTTON1)

			PORT_START /* IN1 */
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP | IPF_4WAY | IPF_COCKTAIL)
		PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN | IPF_4WAY | IPF_COCKTAIL)
			PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT | IPF_4WAY | IPF_COCKTAIL)
				PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_4WAY | IPF_COCKTAIL)
					PORT_BITX(0x10, IP_ACTIVE_LOW, IPT_SERVICE, "Service Mode", OSD_KEY_F2, IP_JOY_NONE, 0)
						PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_START1)
							PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_START2)
								PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_COCKTAIL)

									PORT_START /* DSW0 */
	PORT_DIPNAME(0x01, 0x00, "Bonus Life", IP_KEY_NONE)
		PORT_DIPSETTING(0x00, "30000")
			PORT_DIPSETTING(0x01, "50000")
				PORT_DIPNAME(0x02, 0x00, "Demo Sounds", IP_KEY_NONE)
					PORT_DIPSETTING(0x02, "Off")
						PORT_DIPSETTING(0x00, "On")
							PORT_DIPNAME(0x04, 0x00, "Cabinet", IP_KEY_NONE)
								PORT_DIPSETTING(0x00, "Upright")
									PORT_DIPSETTING(0x04, "Cocktail")
										PORT_DIPNAME(0x18, 0x10, "Lives", IP_KEY_NONE)
											PORT_DIPSETTING(0x18, "2")
												PORT_DIPSETTING(0x10, "3")
													PORT_DIPSETTING(0x08, "4")
														PORT_DIPSETTING(0x00, "5")
															PORT_BITX(0x20, 0x20, IPT_DIPSWITCH_NAME | IPF_CHEAT, "Rack Test", OSD_KEY_F1, IP_JOY_NONE, 0)
																PORT_DIPSETTING(0x20, "Off")
																	PORT_DIPSETTING(0x00, "On")
																		PORT_DIPNAME(0xc0, 0x80, "Difficulty", IP_KEY_NONE)
																			PORT_DIPSETTING(0xc0, "Easy")
																				PORT_DIPSETTING(0x80, "Medium")
																					PORT_DIPSETTING(0x40, "Hard")
																						PORT_DIPSETTING(0x00, "Hardest")

																							PORT_START /* DSW1 */
	PORT_DIPNAME(0x0f, 0x0c, "A Coin/Cred", IP_KEY_NONE)
		PORT_DIPSETTING(0x00, "4/1")
			PORT_DIPSETTING(0x08, "3/1")
				PORT_DIPSETTING(0x04, "2/1")
					PORT_DIPSETTING(0x09, "2/1+Bonus each 5")
						PORT_DIPSETTING(0x05, "2/1+Bonus each 4")
							PORT_DIPSETTING(0x0c, "1/1")
								PORT_DIPSETTING(0x0d, "1/1+Bonus each 5")
									PORT_DIPSETTING(0x03, "1/1+Bonus each 4")
										PORT_DIPSETTING(0x0b, "1/1+Bonus each 2")
											PORT_DIPSETTING(0x02, "1/2")
												PORT_DIPSETTING(0x07, "1/2+Bonus each 5")
													PORT_DIPSETTING(0x0f, "1/2+Bonus each 4")
														PORT_DIPSETTING(0x0a, "1/3")
															PORT_DIPSETTING(0x06, "1/4")
																PORT_DIPSETTING(0x0e, "1/5")
																	PORT_DIPSETTING(0x01, "1/6")
																		PORT_DIPNAME(0xf0, 0xc0, "B Coin/Cred", IP_KEY_NONE)
																			PORT_DIPSETTING(0x00, "4/1")
																				PORT_DIPSETTING(0x80, "3/1")
																					PORT_DIPSETTING(0x40, "2/1")
																						PORT_DIPSETTING(0x90, "2/1+Bonus each 5")
																							PORT_DIPSETTING(0x50, "2/1+Bonus each 4")
																								PORT_DIPSETTING(0xc0, "1/1")
																									PORT_DIPSETTING(0xd0, "1/1+Bonus each 5")
																										PORT_DIPSETTING(0x30, "1/1+Bonus each 4")
																											PORT_DIPSETTING(0xb0, "1/1+Bonus each 2")
																												PORT_DIPSETTING(0x20, "1/2")
																													PORT_DIPSETTING(0x70, "1/2+Bonus each 5")
																														PORT_DIPSETTING(0xf0, "1/2+Bonus each 4")
																															PORT_DIPSETTING(0xa0, "1/3")
																																PORT_DIPSETTING(0x60, "1/4")
																																	PORT_DIPSETTING(0xe0, "1/5")
																																		PORT_DIPSETTING(0x10, "1/6")
																																			INPUT_PORTS_END

#endif