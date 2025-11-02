#ifndef DKONG_H
#define DKONG_H

#pragma once

#include "../MyDefine.h"
#include "../TheMemory.h"
#include "../GameDefinition.h"

extern struct GfxLayout dkongTileLayout;
extern struct GfxLayout dkongSpriteLayout;

static struct GfxDecodeInfo dkongGfxDecodeInfo[] =
    {
        {ROM_GFX, 0x0000, &dkongTileLayout, 0, 64},
        {ROM_GFX, 0x0000, &dkongSpriteLayout, 0, 64},
        {-1}};

#ifdef __cplusplus
extern "C"
{
#endif

    WRITE_HANDLER(dkong_flipscreen_w);
    WRITE_HANDLER(dkong_palettebank_w);
    void dkong_vh_convert_color_prom(unsigned char *palette, unsigned short *colortable, const unsigned char *color_prom);
    int dkong_vh_start(void);
    void dkong_vh_screenrefresh();
    WRITE_HANDLER(dkong_sh_w);
    WRITE_HANDLER(dkong_sh1_w);
    WRITE_HANDLER(dkong_sh_sound3_w);
    WRITE_HANDLER(dkong_sh_sound4_w);
    WRITE_HANDLER(dkong_sh_sound5_w);
    WRITE_HANDLER(dkong_sh_tuneselect_w);

#ifdef __cplusplus
}
#endif

#define DKONG {"dkong", "Donkey Kong", {356, 32 * 8, 32 * 8, {0 * 8, 32 * 8 - 1, 2 * 8, 30 * 8 - 1}, ORIENTATION_ROTATE_270, dkong_vh_screenrefresh, dkongGfxDecodeInfo}, 3072000 / 60, {dkong_rom, NOTHING, dkong_readmem, dkong_writemem, dkong_input_ports, NOTHING, NOTHING}, MACHINE_Z80}

/*
static struct MachineDriver machine_driver_dkong =
{
    // basic machine hardware
    {
        {
            CPU_Z80,
            3072000,	// 3.072 Mhz (?)
            readmem,dkong_writemem,0,0,
            nmi_interrupt,1
        },
        {
            CPU_I8035 | CPU_AUDIO_CPU,
            6000000/15,	// 6Mhz crystal
            readmem_sound,writemem_sound,readport_sound,writeport_sound,
            ignore_interrupt,1
        }
    },
    60, DEFAULT_60HZ_VBLANK_DURATION,	// frames per second, vblank duration
    1,	// 1 CPU slice per frame - interleaving is forced when a sound command is written
    0,

    // video hardware
    32*8, 32*8, { 0*8, 32*8-1, 2*8, 30*8-1 },
    dkong_gfxdecodeinfo,
    256, 64*4,
    dkong_vh_convert_color_prom,

    VIDEO_TYPE_RASTER|VIDEO_SUPPORTS_DIRTY,
    0,
    dkong_vh_start,
    generic_vh_stop,
    dkong_vh_screenrefresh,

    // sound hardware
    0,0,0,0,
    {
        {
            SOUND_DAC,
            &dkong_dac_interface
        },
        {
            SOUND_SAMPLES,
            &dkong_samples_interface
        }
    }
}; */

ROM_START(dkong_rom)
ROM_REGION(0x10000 + 0x1000)
ROM_LOAD("c_5et_g.bin", 0x0000, 0x1000, 0xba70b88b)
ROM_LOAD("c_5ct_g.bin", 0x1000, 0x1000, 0x5ec461ec)
ROM_LOAD("c_5bt_g.bin", 0x2000, 0x1000, 0x1c97d324)
ROM_LOAD("c_5at_g.bin", 0x3000, 0x1000, 0xb9005ac0)
// space for diagnostic ROM

// ROM_REGION( 0x1000, REGION_CPU2 )	// sound
ROM_LOAD("s_3i_b.bin", 0x10000 + 0x0000, 0x0800, 0x45a4ed06)
ROM_LOAD("s_3j_b.bin", 0x10000 + 0x0800, 0x0800, 0x4743fe92)

ROM_REGION_GFX(0x1000 + 0x2000) //, REGION_GFX1 | REGIONFLAG_DISPOSE )
ROM_LOAD("v_5h_b.bin", 0x0000, 0x0800, 0x12c8c95d)
ROM_LOAD("v_3pt.bin", 0x0800, 0x0800, 0x15e9c5e9)
// ROM_REGION( 0x2000, REGION_GFX2 | REGIONFLAG_DISPOSE )
ROM_LOAD("l_4m_b.bin", 0x0000, 0x0800, 0x59f8054d)
ROM_LOAD("l_4n_b.bin", 0x0800, 0x0800, 0x672e4714)
ROM_LOAD("l_4r_b.bin", 0x1000, 0x0800, 0xfeaa59ee)
ROM_LOAD("l_4s_b.bin", 0x1800, 0x0800, 0x20f2ef7e)

ROM_REGION_COLOR(0x0300)                         //, REGION_PROMS )
ROM_LOAD("c-2k.bpr", 0x0000, 0x0100, 0xe273ede5) // palette low 4 bits (inverted)
ROM_LOAD("c-2j.bpr", 0x0100, 0x0100, 0xd6412358) // palette high 4 bits (inverted)
ROM_LOAD("v-5e.bpr", 0x0200, 0x0100, 0xb869b8f5) // character color codes on a per-column basis
ROM_END

static int mcustatus;

static READ_HANDLER(dkong_in2_r)
{
    return input_port_2_r(offset) | (mcustatus << 6);
}

static struct MemoryReadAddress dkong_readmem[] =
    {
        {0x0000, 0x5fff, MRA_ROM},        // DK: 0000-3fff
        {0x6000, 0x6fff, MRA_RAM},        // including sprites RAM
        {0x7400, 0x77ff, MRA_RAM},        // video RAM
        {0x7c00, 0x7c00, input_port_0_r}, // IN0
        {0x7c80, 0x7c80, input_port_1_r}, // IN1
        {0x7d00, 0x7d00, dkong_in2_r},    // IN2/DSW2
        {0x7d80, 0x7d80, input_port_3_r}, // DSW1
        {0x8000, 0x9fff, MRA_ROM},        // DK3 and bootleg DKjr only
        {-1}};

static struct MemoryWriteAddress dkong_writemem[] =
    {
        {0x0000, 0x5fff, MWA_ROM},
        {0x6000, 0x68ff, MWA_RAM},
        {0x6900, 0x6a7f, MWA_RAM, &spriteram, &spriteram_size},
        {0x6a80, 0x6fff, MWA_RAM},
        {0x7000, 0x73ff, MWA_RAM}, // ????
        {0x7400, 0x77ff, videoram_w, &videoram, &videoram_size},
        {0x7800, 0x7803, MWA_RAM}, // ????
        {0x7808, 0x7808, MWA_RAM}, // ????
        {0x7c00, 0x7c00, dkong_sh_tuneselect_w},
        //	{ 0x7c80, 0x7c80,  },
        {0x7d00, 0x7d02, dkong_sh1_w}, // walk/jump/boom sample trigger
        {0x7d03, 0x7d03, dkong_sh_sound3_w},
        {0x7d04, 0x7d04, dkong_sh_sound4_w},
        {0x7d05, 0x7d05, dkong_sh_sound5_w},
        {0x7d80, 0x7d80, dkong_sh_w},
        {0x7d81, 0x7d81, MWA_RAM}, // ???? */
        {0x7d82, 0x7d82, dkong_flipscreen_w},
        {0x7d83, 0x7d83, MWA_RAM},
        {0x7d84, 0x7d84, Z80_interrupt_enable_w},
        {0x7d85, 0x7d85, MWA_RAM},
        {0x7d86, 0x7d87, dkong_palettebank_w},
        {-1}};

INPUT_PORTS_START(dkong_input_ports)
PORT_START /* IN0 */
    PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT | IPF_8WAY)
        PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT | IPF_8WAY)
            PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP | IPF_8WAY)
                PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN | IPF_8WAY)
                    PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_BUTTON1)
                        PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN)
                            PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN)
                                PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN)

                                    PORT_START /* IN1 */
    PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_COCKTAIL)
        PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT | IPF_8WAY | IPF_COCKTAIL)
            PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP | IPF_8WAY | IPF_COCKTAIL)
                PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN | IPF_8WAY | IPF_COCKTAIL)
                    PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 | IPF_COCKTAIL)
                        PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN)
                            PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN)
                                PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN)

                                    PORT_START /* IN2 */
    PORT_BITX(0x01, IP_ACTIVE_HIGH, IPT_SERVICE, DEF_STR(Service_Mode), KEYCODE_F2, IP_JOY_NONE, 0)
    //	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Service_Mode ) )
    //	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
    //	PORT_DIPSETTING(    0x01, DEF_STR( On ) )
    PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_UNKNOWN)
        PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_START1)
            PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_START2)
                PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_UNKNOWN)
                    PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN)
                        PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN) /* status from sound cpu */
    PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_COIN1)

        PORT_START /* DSW0 */
    PORT_DIPNAME(0x03, 0x00, DEF_STR(Lives), IP_KEY_NONE)
        PORT_DIPSETTING(0x00, "3")
            PORT_DIPSETTING(0x01, "4")
                PORT_DIPSETTING(0x02, "5")
                    PORT_DIPSETTING(0x03, "6")
                        PORT_DIPNAME(0x0c, 0x00, DEF_STR(Bonus_Life), IP_KEY_NONE)
                            PORT_DIPSETTING(0x00, "7000")
                                PORT_DIPSETTING(0x04, "10000")
                                    PORT_DIPSETTING(0x08, "15000")
                                        PORT_DIPSETTING(0x0c, "20000")
                                            PORT_DIPNAME(0x70, 0x00, DEF_STR(Coinage), IP_KEY_NONE)
                                                PORT_DIPSETTING(0x70, DEF_STR(5C_1C))
                                                    PORT_DIPSETTING(0x50, DEF_STR(4C_1C))
                                                        PORT_DIPSETTING(0x30, DEF_STR(3C_1C))
                                                            PORT_DIPSETTING(0x10, DEF_STR(2C_1C))
                                                                PORT_DIPSETTING(0x00, DEF_STR(1C_1C))
                                                                    PORT_DIPSETTING(0x20, DEF_STR(1C_2C))
                                                                        PORT_DIPSETTING(0x40, DEF_STR(1C_3C))
                                                                            PORT_DIPSETTING(0x60, DEF_STR(1C_4C))
                                                                                PORT_DIPNAME(0x80, 0x80, DEF_STR(Cabinet), IP_KEY_NONE)
                                                                                    PORT_DIPSETTING(0x80, DEF_STR(Upright))
                                                                                        PORT_DIPSETTING(0x00, DEF_STR(Cocktail))
                                                                                            INPUT_PORTS_END

/*
static struct MemoryReadAddress readmem_sound[] =
{
    { 0x0000, 0x0fff, MRA_ROM },
    { -1 }
};

static struct MemoryWriteAddress writemem_sound[] =
{
    { 0x0000, 0x0fff, MWA_ROM },
    { -1 }
};

static struct IOReadPort readport_sound[] =
{
    { 0x00,     0xff,     dkong_sh_tune_r },
    { I8039_p1, I8039_p1, dkong_sh_p1_r },
    { I8039_p2, I8039_p2, dkong_sh_p2_r },
    { I8039_t0, I8039_t0, dkong_sh_t0_r },
    { I8039_t1, I8039_t1, dkong_sh_t1_r },
    { -1 }
};
static struct IOWritePort writeport_sound[] =
{
    { I8039_p1, I8039_p1, dkong_sh_p1_w },
    { I8039_p2, I8039_p2, dkong_sh_p2_w },
    { -1 }
};
*/

#endif