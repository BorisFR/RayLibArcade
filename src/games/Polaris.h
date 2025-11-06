#ifndef POLARIS_H
#define POLARIS_H

#pragma once

#include "../MyDefine.h"
#include "../TheMemory.h"
#include "../GameDefinition.h"

#ifdef __cplusplus
extern "C"
{
#endif

    READ_HANDLER(polaris_scattered_colorram_r);
    WRITE_HANDLER(polaris_scattered_colorram_w);
    WRITE_HANDLER(polaris_videoram_w);
    WRITE_HANDLER(polaris_sh_port_1_w);
    WRITE_HANDLER(polaris_sh_port_2_w);
    WRITE_HANDLER(polaris_sh_port_3_w);
    extern void PolarisInit();
    extern void PolarisRefreshScreen();

#ifdef __cplusplus
}
#endif

#define POLARIS {"polaris", "Polaris (latest version)", {307, 28*8, 32*8, { 0*8, 28*8-1, 0*8, 32*8-1 }, ORIENTATION_ROTATE_270, PolarisRefreshScreen, NOTHING}, 1996800 / 60, {polaris_rom, NOTHING, polaris_readmem, polaris_writemem, polaris_input_ports, polaris_readport, polaris_writeport, PolarisInit}, MACHINE_8080BW}

ROM_START(polaris_rom)
ROM_REGION(0x10000)
ROM_LOAD("ps01-1.ic71", 0x0000, 0x0800, 0x7d41007c)
ROM_LOAD("ps02-9.ic70", 0x0800, 0x0800, 0x9a5c8cb2)
ROM_LOAD("ps03-1.ic69", 0x1000, 0x0800, 0x21f32415)
ROM_LOAD("ps04-18.ic62", 0x1800, 0x0800, 0xd717aef3)
ROM_LOAD("ps05.ic61", 0x4000, 0x0800, 0x772e31f3)
ROM_LOAD("ps06-10.ic60", 0x4800, 0x0800, 0x3df77bac)
ROM_LOAD("ps26.ic60a", 0x5000, 0x0800, 0x9d5c3d50)
ROM_REGION_COLOR(0x400)
ROM_LOAD("ps08.1b", 0x0000, 0x0400, 0x164aa05d)
ROM_REGION_USER1(0x100)
ROM_LOAD("ps07.2c", 0x0000, 0x0100, 0x2953253b)
ROM_END

static struct MemoryReadAddress polaris_readmem[] =
{
	{ 0x0000, 0x1fff, MRA_ROM },
	{ 0x2000, 0x3fff, MRA_RAM },
	{ 0x4000, 0x4fff, MRA_ROM },
	{ 0xc000, 0xdfff, polaris_scattered_colorram_r },
	{ -1 }  // end of table
};

static struct MemoryWriteAddress polaris_writemem[] =
{
	{ 0x0000, 0x1fff, MWA_ROM },
	{ 0x2000, 0x23ff, MWA_RAM },
	{ 0x2400, 0x3fff, polaris_videoram_w, &videoram, &videoram_size },
	{ 0x4000, 0x4fff, MWA_ROM },
	{ 0xc000, 0xdfff, polaris_scattered_colorram_w },
	{ -1 }  // end of table
};


static struct IOReadPort polaris_readport[] =
{
	{ 0x00, 0x00, input_port_0_r },
	{ 0x01, 0x01, input_port_1_r },
	{ 0x02, 0x02, input_port_2_r },
	{ 0x03, 0x03, mb14241_shift_result_r },
	{ -1 }
};

static struct IOWritePort polaris_writeport[] =
{
	{ 0x00, 0x00, mb14241_shift_count_w },
	{ 0x02, 0x02, polaris_sh_port_1_w },
	{ 0x03, 0x03, mb14241_shift_data_w },
	{ 0x04, 0x04, polaris_sh_port_2_w },
	{ 0x05, 0x05, writePortNone }, // watchdog
	{ 0x06, 0x06, polaris_sh_port_3_w },
	{ -1 }
};


INPUT_PORTS_START( polaris_input_ports )
	PORT_START      // IN0
	//PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_DIPNAME( 0x01, 0x00, "Unused", IP_KEY_NONE ) // SW?:1
	PORT_DIPSETTING(    0x00, "Off" )
	PORT_DIPSETTING(    0x01, "On" )
	//PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_DIPNAME( 0x02, 0x00, "Unused", IP_KEY_NONE ) // SW?:2
	PORT_DIPSETTING(    0x00, "Off" )
	PORT_DIPSETTING(    0x02, "On" )
	// PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN )   // Tilt
	// PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	// PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	// PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	// PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	// PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN)

	PORT_START      // IN1
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT | IPF_8WAY )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN | IPF_8WAY )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT | IPF_8WAY )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP | IPF_8WAY )

	PORT_START      // IN2 / DSW0
	PORT_DIPNAME( 0x03, 0x00, "Lives", IP_KEY_NONE ) // SW1:1,2
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPSETTING(    0x02, "5" )
	PORT_DIPSETTING(    0x03, "6" )
	//PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_DIPNAME( 0x04, 0x00, "Cabinet", IP_KEY_NONE ) // SW1:3
	PORT_DIPSETTING(    0x00, "Upright" )
	PORT_DIPSETTING(    0x04, "Cocktail" )
	PORT_DIPNAME( 0x08, 0x00, "Invincible Test", IP_KEY_NONE ) // SW1:4
	PORT_DIPSETTING(    0x00, "Off" )
	PORT_DIPSETTING(    0x08, "On" )
	//PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_DIPNAME( 0x10, 0x10, "Demo sound", IP_KEY_NONE ) // SW1:5
	PORT_DIPSETTING(    0x00, "Off" )
	PORT_DIPSETTING(    0x10, "On" )
	//PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_DIPNAME( 0x20, 0x00, "Unused", IP_KEY_NONE ) // SW1:6
	PORT_DIPSETTING(    0x00, "Off" )
	PORT_DIPSETTING(    0x20, "On" )
	//PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_DIPNAME( 0x40, 0x00, "Unused", IP_KEY_NONE ) // SW1:7
	PORT_DIPSETTING(    0x00, "Off" )
	PORT_DIPSETTING(    0x40, "On" )
	PORT_DIPNAME( 0x80, 0x00, "High Score Preset Mode", IP_KEY_NONE ) // SW1:8
	PORT_DIPSETTING(    0x00, "Off" )
	PORT_DIPSETTING(    0x80, "On" )
INPUT_PORTS_END

/*

static struct MachineDriver polaris_machine_driver =
{
	{
		{
			CPU_8080,
			2000000,        // 2 Mhz?
			0,
			polaris_readmem,polaris_writemem,polaris_readport,polaris_writeport,
			polaris_interrupt,2    // two interrupts per frame
		}
	},
	60, DEFAULT_60HZ_VBLANK_DURATION,       // frames per second, vblank duration
	1,      // single CPU, no need for interleaving
	0,

	// video hardware
	32*8, 32*8, { 0*8, 32*8-1, 0*8, 28*8-1 },
	0,      // no gfxdecodeinfo - bitmapped display
	sizeof(palette)/3, 0,
	0,

	VIDEO_TYPE_RASTER|VIDEO_SUPPORTS_DIRTY,
	0,
	polaris_vh_start,
	polaris_vh_stop,
	polaris_vh_screenrefresh,

	// sound hardware
	0,0,0,0
}; */

#endif