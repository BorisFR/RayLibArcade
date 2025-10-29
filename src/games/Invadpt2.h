#ifndef INVADERS_PART2_H
#define INVADERS_PART2_H

#pragma once

#include "../MyDefine.h"
#include "../TheMemory.h"
#include "../GameDefinition.h"

#ifdef __cplusplus
extern "C"
{
#endif

    WRITE_HANDLER(invadpt2_videoram_w);

#ifdef __cplusplus
}
#endif

#define INVADPT2 {"invadpt2", "Space Invaders part II", {386, 224, 256, VISIBLE_AREA_FULL, ORIENTATION_DEFAULT, NOTHING, NOTHING}, 3000000 / 60, {invadpt2_rom, NOTHING, invaders_readmem, invadpt2_writemem, invadpt2_input_ports, invadpt2_readport, invadpt2_writeport}, MACHINE_8080BW}

ROM_START(invadpt2_rom)
ROM_REGION(0x10000)
ROM_LOAD("pv01", 0x0000, 0x0800, 0x7288a511)
ROM_LOAD("pv02", 0x0800, 0x0800, 0x097dd8d5)
ROM_LOAD("pv03", 0x1000, 0x0800, 0x1766337e)
ROM_LOAD("pv04", 0x1800, 0x0800, 0x8f0e62e0)
ROM_LOAD("pv05", 0x4000, 0x0800, 0x19b505e9)
// ROM_REGION( 0x0800, REGION_PROMS )		// color maps player 1/player 2
ROM_REGION_COLOR(0x0800)
ROM_LOAD("pv06.1", 0x0000, 0x0400, 0xa732810b)
ROM_LOAD("pv07.2", 0x0400, 0x0400, 0x2c5b91cb)
ROM_END

static struct MemoryWriteAddress invadpt2_writemem[] = {
	{0x2000, 0x23ff, MWA_RAM},
	{0x2400, 0x3fff, invadpt2_videoram_w, &videoram, &videoram_size },
	{0x0000, 0x1fff, MWA_ROM},
	{0x4000, 0x57ff, MWA_ROM},
	{-1}};

static struct IOReadPort invadpt2_readport[] = {
	{0x00, 0x00, readPort0},
	{0x01, 0x01, readPort1},
	{0x02, 0x02, readPort2},
	{0x03, 0x03, mb14241_shift_result_r},
	{-1}};

static struct IOWritePort invadpt2_writeport[] = {
	{0x02, 0x02, mb14241_shift_count_w},
	{0x03, 0x03, writePort3},
	{0x04, 0x04, mb14241_shift_data_w},
	{0x05, 0x05, writePort5},
	{0x06, 0x06, writePortNone},
	{-1}};

INPUT_PORTS_START(invadpt2_input_ports)
PORT_START /* IN0 */
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_UNKNOWN)
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_UNKNOWN)
			PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_UNKNOWN)
				PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_UNKNOWN)
					PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_UNKNOWN)
						PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
							PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_UNKNOWN) /* N ? */
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNKNOWN)
		PORT_START /* IN1 */
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_COIN1)
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_START2)
			PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_START1)
				PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_UNKNOWN)
					PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_BUTTON1)
						PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT | IPF_2WAY)
							PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT | IPF_2WAY)
								PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNKNOWN)
									PORT_START /* DSW0 */
	PORT_DIPNAME(0x01, 0x00, "Lives", IP_KEY_NONE)
		PORT_DIPSETTING(0x00, "3")
			PORT_DIPSETTING(0x01, "4")
				PORT_DIPNAME(0x02, 0x00, "Unknown", IP_KEY_NONE)
					PORT_DIPSETTING(0x00, "Off")
						PORT_DIPSETTING(0x02, "On")
							PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN)
								PORT_DIPNAME(0x08, 0x00, "Preset Mode", IP_KEY_NONE)
									PORT_DIPSETTING(0x00, "Off")
										PORT_DIPSETTING(0x08, "On")
											PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 | IPF_COCKTAIL)
												PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT | IPF_2WAY | IPF_COCKTAIL)
													PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT | IPF_2WAY | IPF_COCKTAIL)
														PORT_DIPNAME(0x80, 0x00, "Coin Info", IP_KEY_NONE)
															PORT_DIPSETTING(0x00, "On")
																PORT_DIPSETTING(0x80, "Off")
																	PORT_START /* BSR */
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_UNKNOWN)
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_UNKNOWN)
			PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN)
				PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_UNKNOWN)
					PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_UNKNOWN)
						PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN)
							PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN)
								PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN)
									PORT_START /* Dummy port for cocktail mode */
	PORT_DIPNAME(0x01, 0x00, "Cabinet", IP_KEY_NONE)
		PORT_DIPSETTING(0x00, "Upright")
			PORT_DIPSETTING(0x01, "Cocktail")
				INPUT_PORTS_END

#endif