#ifndef GAME_DEFINITION_H
#define GAME_DEFINITION_H

#pragma once

#include "TheMemory.h"

#ifdef __cplusplus
extern "C"
{
#endif

// Declaration moved after struct OneGame definition

#define IP_ACTIVE_HIGH 0x00
#define IP_ACTIVE_LOW 0xff

	enum
	{
		IPT_END = 1,
		IPT_PORT,
		/* use IPT_JOYSTICK for panels where the player has one single joystick */
		IPT_JOYSTICK_UP,
		IPT_JOYSTICK_DOWN,
		IPT_JOYSTICK_LEFT,
		IPT_JOYSTICK_RIGHT,
		/* use IPT_JOYSTICKLEFT and IPT_JOYSTICKRIGHT for dual joystick panels */
		IPT_JOYSTICKRIGHT_UP,
		IPT_JOYSTICKRIGHT_DOWN,
		IPT_JOYSTICKRIGHT_LEFT,
		IPT_JOYSTICKRIGHT_RIGHT,
		IPT_JOYSTICKLEFT_UP,
		IPT_JOYSTICKLEFT_DOWN,
		IPT_JOYSTICKLEFT_LEFT,
		IPT_JOYSTICKLEFT_RIGHT,
		IPT_BUTTON1,
		IPT_BUTTON2,
		IPT_BUTTON3,
		IPT_BUTTON4, /* action buttons */
		IPT_BUTTON5,
		IPT_BUTTON6,
		IPT_BUTTON7,
		IPT_BUTTON8,

		/* analog inputs */
		/* the "arg" field contains the default sensitivity expressed as a percentage */
		/* (100 = default, 50 = half, 200 = twice) */
		IPT_ANALOG_START,
		IPT_PADDLE,
		IPT_DIAL,
		IPT_TRACKBALL_X,
		IPT_TRACKBALL_Y,
		IPT_AD_STICK_X,
		IPT_AD_STICK_Y,
		IPT_ANALOG_END,

		IPT_COIN1,
		IPT_COIN2,
		IPT_COIN3,
		IPT_COIN4, /* coin slots */
		IPT_START1,
		IPT_START2,
		IPT_START3,
		IPT_START4, /* start buttons */
		IPT_SERVICE,
		IPT_TILT,
		IPT_DIPSWITCH_NAME,
		IPT_DIPSWITCH_SETTING,
		/* Many games poll an input bit to check for vertical blanks instead of using */
		/* interrupts. This special value allows you to handle that. If you set one of the */
		/* input bits to this, the bit will be inverted while a vertical blank is happening. */
		IPT_VBLANK,
		IPT_UNKNOWN
	};

//#define IPT_UNUSED IPF_UNUSED
#define IPT_UNUSED IPT_UNKNOWN

#define IPF_MASK 0xffff0000
#define IPF_UNUSED 0x80000000	 /* The bit is not used by this game, but is used */
								 /* by other games running on the same hardware. */
								 /* This is different from IPT_UNUSED, which marks */
								 /* bits not connected to anything. */
#define IPF_COCKTAIL IPF_PLAYER2 /* the bit is used in cocktail mode only */

#define IPF_CHEAT 0x40000000 /* Indicates that the input bit is a "cheat" key */
							 /* (providing invulnerabilty, level advance, and */
							 /* so on). MAME will not recognize it when the */
							 /* -nocheat command line option is specified. */

#define IPF_PLAYERMASK 0x00030000 /* use IPF_PLAYERn if more than one person can */
#define IPF_PLAYER1 0			  /* play at the same time. The IPT_ should be the same */
#define IPF_PLAYER2 0x00010000	  /* for all players (e.g. IPT_BUTTON1 | IPF_PLAYER2) */
#define IPF_PLAYER3 0x00020000	  /* IPF_PLAYER1 is the default and can be left out to */
#define IPF_PLAYER4 0x00030000	  /* increase readability. */

#define IPF_8WAY 0			/* Joystick modes of operation. 8WAY is the default, */
#define IPF_4WAY 0//0x00080000 /* it prevents left/right or up/down to be pressed at */
#define IPF_2WAY 0			/* the same time. 4WAY prevents diagonal directions. */
							/* 2WAY should be used for joysticks wich move only */
							/* on one axis (e.g. Battle Zone) */

#define IPF_IMPULSE 0x00100000 /* When this is set, when the key corrisponding to */
							   /* the input bit is pressed it will be reported as */
							   /* pressed for a certain number of video frames and */
							   /* then released, regardless of the real status of */
							   /* the key. This is useful e.g. for some coin inputs. */
							   /* The number of frames the signal should stay active */
							   /* is specified in the "arg" field. */
#define IPF_TOGGLE 0x00200000  /* When this is set, the key acts as a toggle - press */
							   /* it once and it goes on, press it again and it goes off. */
							   /* useful e.g. for sone Test Mode dip switches. */
#define IPF_REVERSE 0x00400000 /* By default, analog inputs like IPT_TRACKBALL increase */
							   /* when going right/up. This flag inverts them. */

#define IPF_CENTER 0x00800000 /* always preload in->default, autocentering the STICK/TRACKBALL */

#define IPF_CUSTOM_UPDATE 0x01000000 /* normally, analog ports are updated when they are accessed. */
									 /* When this flag is set, they are never updated automatically, */
									 /* it is the responsibility of the driver to call */
									 /* update_analog_port(int port). */

#define IPF_RESETCPU 0x02000000 /* when the key is pressed, reset the first CPU */

/* LBO - These 4 byte values are packed into the arg field and are typically used with analog ports */
#define IPF_SENSITIVITY(percent) (percent & 0xff)
#define IPF_CLIP(clip) ((clip & 0xff) << 8)
#define IPF_MIN(min) ((min & 0xff) << 16)
#define IPF_MAX(max) ((max & 0xff) << 24)

/* LBO - these fields are packed into in->keyboard & in->joystick for analog controls */
#define IPF_DEC(key) ((key & 0xff))
#define IPF_INC(key) ((key & 0xff) << 8)
#define IPF_DELTA(val) ((val & 0xff) << 16)

#define IP_NAME_DEFAULT ((const char *)-1)

#define IP_KEY_DEFAULT -1
#define IP_KEY_NONE -2
#define IP_KEY_PREVIOUS -3 /* use the same key as the previous input bit */

#define IP_JOY_DEFAULT -1
#define IP_JOY_NONE -2
#define IP_JOY_PREVIOUS -3 /* use the same joy as the previous input bit */

#define RGN_FRAC(num,den)       (0x80000000 | (((num) & 0x0f) << 27) | (((den) & 0x0f) << 23))

// https://github.com/Jean-MarcHarvengt/teensyMAME/blob/master/teensyMAMEClassic1/common.h
#define ROMFLAG_MASK 0xf0000000		 /* 4 bits worth of flags in the high nibble */
#define ROMFLAG_ALTERNATE 0x80000000 /* Alternate bytes, either even or odd */
#define ROMFLAG_DISPOSE 0x80000000	 /* Dispose of this region when done */
#define ROMFLAG_IGNORE 0x40000000	 /* BM: Ignored - drivers must load this region themselves */
#define ROMFLAG_WIDE 0x40000000		 /* 16-bit ROM; may need byte swapping */
#define ROMFLAG_SWAP 0x20000000		 /* 16-bit ROM with bytes in wrong order */

#define NO_ROM -1
#define ROM_CPU 0x00000001	   /* CPU ROM */
#define ROM_GFX 0x00000002	   /* Graphics ROM */
#define ROM_SOUND 0x00000004   /* Sound ROM */
#define ROM_COLOR 0x00000008   /* Color ROM */
#define ROM_PALETTE 0x00000010 /* Palette ROM */
							   // #define ROM_TILE 0x00000020	   /* Tile ROM */
							   // #define ROM_SPRITE 0x00000040  /* Sprite ROM */

	// change are made, I add ROM_CPU/GFX/SND at the end

#define ROM_START(name) static struct RomModule name[] = {
#define ROM_REGION(length) {0, length, 0, ROM_CPU},
#define ROM_REGION_GFX(length) {0, length, 0, ROM_GFX},
#define ROM_REGION_SND(length) {0, length, 0, ROM_SOUND},
#define ROM_REGION_COLOR(length) {0, length, 0, ROM_COLOR},
#define ROM_REGION_PALETTE(length) {0, length, 0, ROM_PALETTE},
// #define ROM_REGION_TILE(length) {0, length, 0, ROM_TILE},
// #define ROM_REGION_SPRITE(length) {0, length, 0, ROM_SPRITE},
#define ROM_REGION_DISPOSE(length) {0, length, 0, ROMFLAG_DISPOSE},
#define ROM_REGION_OPTIONAL(length) {0, length, 0, ROMFLAG_IGNORE},
#define ROM_LOAD(name, offset, length, crc) {name, offset, length, crc},
#define ROM_END { 0, 0, 0, 0 } } \
	;

	// https://github.com/Jean-MarcHarvengt/teensyMAME/blob/master/teensyMAMEClassic5/common.h

	/***************************************************************************
	Note that the memory hooks are not passed the actual memory address where
	the operation takes place, but the offset from the beginning of the block
	they are assigned to. This makes handling of mirror addresses easier, and
	makes the handlers a bit more "object oriented". If you handler needs to
	read/write the main memory area, provide a "base" pointer: it will be
	initialized by the main engine to point to the beginning of the memory block
	assigned to the handler. You may also provided a pointer to "size": it
	will be set to the length of the memory area processed by the handler.
	***************************************************************************/

#define MRA_ROM boardMemoryRead
#define MRA_RAM boardMemoryRead
#define MWA_ROM boardMemoryWriteNone
#define MWA_RAM boardMemoryWrite
#define MWA_NOP boardMemoryWriteNone

	// #define MRA_NOP 0						/* don't care, return 0 */
	// #define MRA_RAM ((int (*)(int)) - 1)	/* plain RAM location (return its contents) */
	// #define MRA_ROM ((int (*)(int)) - 2)	/* plain ROM location (return its contents) */
	// #define MRA_BANK1 ((int (*)(int)) - 10) /* bank memory */
	// #define MRA_BANK2 ((int (*)(int)) - 11) /* bank memory */
	// #define MRA_BANK3 ((int (*)(int)) - 12) /* bank memory */
	// #define MRA_BANK4 ((int (*)(int)) - 13) /* bank memory */
	// #define MRA_BANK5 ((int (*)(int)) - 14) /* bank memory */
	// #define MRA_BANK6 ((int (*)(int)) - 15) /* bank memory */
	// #define MRA_BANK7 ((int (*)(int)) - 16) /* bank memory */
	// #define MRA_BANK8 ((int (*)(int)) - 17) /* bank memory */

	// #define MWA_NOP 0						   /* do nothing */
	// #define MWA_RAM ((void (*)(int, int)) - 1) /* plain RAM location (store the value) */
	// #define MWA_ROM ((void (*)(int, int)) - 2) /* plain ROM location (do nothing) */
	/* RAM[] and ROM[] are usually the same, but they aren't if the CPU opcodes are */
	/* encrypted. In such a case, opcodes are fetched from ROM[], and arguments from */
	/* RAM[]. If the program dynamically creates code in RAM and executes it, it */
	/* won't work unless writes to RAM affects both RAM[] and ROM[]. */
	// #define MWA_RAMROM ((void (*)(int, int)) - 3) /* write to both the RAM[] and ROM[] array. */
	// #define MWA_BANK1 ((void (*)(int, int)) - 10) /* bank memory */
	// #define MWA_BANK2 ((void (*)(int, int)) - 11) /* bank memory */
	// #define MWA_BANK3 ((void (*)(int, int)) - 12) /* bank memory */
	// #define MWA_BANK4 ((void (*)(int, int)) - 13) /* bank memory */
	// #define MWA_BANK5 ((void (*)(int, int)) - 14) /* bank memory */
	// #define MWA_BANK6 ((void (*)(int, int)) - 15) /* bank memory */
	// #define MWA_BANK7 ((void (*)(int, int)) - 16) /* bank memory */
	// #define MWA_BANK8 ((void (*)(int, int)) - 17) /* bank memory */

	// from https://github.com/Jean-MarcHarvengt/teensyMAME/blob/master/teensyMAMEClassic3/memory.h

	// #define MAX_BANKS 8

	// https://github.com/Jean-MarcHarvengt/teensyMAME/blob/master/teensyMAMEClassic1/driver.h#L140

	/***************************************************************************
	IN and OUT ports are handled like memory accesses, the hook template is the
	same so you can interchange them. Of course there is no 'base' pointer for
	IO ports.
	***************************************************************************/

#define INPUT_PORTS_START(name) static struct InputPort name[] = {
#define PORT_START {0, 0, IPT_PORT, 0, 0, 0, 0},
#define INPUT_PORTS_END { 0, 0, IPT_END, 0, 0 } \
} \
	;
#define PORT_BIT(mask, default, type) {mask, default, type, IP_NAME_DEFAULT, IP_KEY_DEFAULT, IP_JOY_DEFAULT, 0},
#define PORT_DIPNAME(mask, default, name, key) {mask, default, IPT_DIPSWITCH_NAME, name, key, IP_JOY_NONE, 0},
#define PORT_DIPSETTING(default, name) {0, default, IPT_DIPSWITCH_SETTING, name, IP_KEY_NONE, IP_JOY_NONE, 0},

	// #define IORP_NOP 0 /* don't care, return 0 */
	// #define IOWP_NOP 0 /* do nothing */

	// https://github.com/Jean-MarcHarvengt/teensyMAME/blob/master/teensyMAMEClassic1/_unused/vidhrdw/driver.h#L404
#define	ORIENTATION_DEFAULT		0x00
#define	ORIENTATION_FLIP_X		0x01	/* mirror everything in the X direction */
#define	ORIENTATION_FLIP_Y		0x02	/* mirror everything in the Y direction */
#define ORIENTATION_SWAP_XY		0x04	/* mirror along the top-left/bottom-right diagonal */
#define	ORIENTATION_ROTATE_90	(ORIENTATION_SWAP_XY|ORIENTATION_FLIP_X)	/* rotate clockwise 90 degrees */
#define	ORIENTATION_ROTATE_180	(ORIENTATION_FLIP_X|ORIENTATION_FLIP_Y)	/* rotate 180 degrees */
#define	ORIENTATION_ROTATE_270	(ORIENTATION_SWAP_XY|ORIENTATION_FLIP_Y)	/* rotate counter-clockwise 90 degrees */
/* IMPORTANT: to perform more than one transformation, DO NOT USE |, use ^ instead. */
/* For example, to rotate 90 degrees counterclockwise and flip horizontally, use: */
/* ORIENTATION_ROTATE_270 ^ ORIENTATION_FLIP_X*/
/* Always remember that FLIP is performed *after* SWAP_XY. */

	// https://github.com/Jean-MarcHarvengt/teensyMAME/blob/master/teensyMAMEClassic4/common.h
	struct RomModule
	{
		const char *name;	 // name of the file to load
		unsigned int offset; // offset to load it to
		unsigned int length; // length of the file
		unsigned int crc;	 // standard CRC-32 checksum
	};

	struct MemoryReadAddress
	{
		int start, end;
		int (*handler)(int offset); // see special values below
		uint8_t **base;				// optional (see explanation above)
		int *size;					// optional (see explanation above)
	};

	struct MemoryWriteAddress
	{
		int start, end;
		void (*handler)(int offset, int data); // see special values below
		uint8_t **base;						   // optional (see explanation above)
		int *size;							   // optional (see explanation above)
	};

	struct InputPort
	{
		unsigned char mask;			 // bits affected
		unsigned char default_value; // default value for the bits affected
									 // you can also use one of the IP_ACTIVE defines below
		int type;					 // see defines below
		const char *name;			 // name to display
		int keyboard;				 // key affecting the input bits
		int joystick;				 // joystick command affecting the input bits
		int arg;					 // extra argument needed in some cases
	};

	struct IOReadPort
	{
		int start, end;
		int (*handler)(int offset); // see special values below
	};

	struct IOWritePort
	{
		int start, end;
		void (*handler)(int offset, int data); /* see special values below */
	};

	#define NOTHING 0

	struct TheVideo {
		const uint32_t screenWidth;
		const uint32_t screenHeight;
		const struct VisibleArea visibleArea;
		const uint8_t orientation;
		void (*drawDisplay)(); // must draw screen in screendata[]
		const struct GfxDecodeInfo *decodeInfo;
	};

	struct TheMachine {
		const struct RomModule *roms;
		void (*decode)();
		const struct MemoryReadAddress *readAddress;
		const struct MemoryWriteAddress *writeAddress;
		const struct InputPort *inputPorts;
		const struct IOReadPort *readPorts;
		const struct IOWritePort *writePorts;
	};

	struct OneGame
	{
		const char *folder;
		const char *name;
		const struct TheVideo video;
		// const uint32_t screenWidth;
		// const uint32_t screenHeight;
		// const uint8_t orientation;
		// void (*drawDisplay)(); // must draw screen in screendata[]
		// const struct GfxDecodeInfo *decodeInfo;
		const unsigned long frequency;
		const struct TheMachine machine;
		// const RomModule *roms;
		// const MemoryReadAddress *readAddress;
		// const MemoryWriteAddress *writeAddress;
		// const InputPort *inputPorts;
		// const IOReadPort *readPorts;
		// const IOWritePort *writePorts;
		const int8_t machineType;
	};

	extern const struct OneGame allGames[];

#ifdef __cplusplus
}
#endif

#endif