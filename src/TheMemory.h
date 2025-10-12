#ifndef MEMORY_H
#define MEMORY_H

#pragma once

#include "MyDefine.h"

#define LOW 0
#define HIGH 1

#ifdef ESP32P4
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <sys/time.h>
#else
#endif
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "TheTools.hpp"

#ifdef ESP32P4
#define COLOR_TYPE uint16_t
#define THE_COLOR uint16_t
#else
#define COLOR_TYPE Color
#define THE_COLOR uint32_t
#endif

#define MAX_GFX_ELEMENTS 32
// #define MAX_MEMORY_REGIONS 10

// https://github.com/squidrpi/mame4all-pi/blob/bf71c5fdd2e2bbdccb36995e918b99ae7b01dc7a/src/memory.h#L253
// #define INLINE inline
#define INT8 int8_t
#define UINT8 uint8_t
#define UINT16 uint16_t
#define UINT32 uint32_t

typedef struct
{
	int (*handler)(int offset);
} ReadHandler;

typedef struct
{
	void (*handler)(int offset, int data);
} WriteHandler;

extern ReadHandler *InputPortRead[BUTTON_END + 1];
extern WriteHandler *InputPortWrite[BUTTON_END + 1];
extern uint8_t InputPorts[BUTTON_END + 1];
// extern uint8_t InputPortsIndex[7];
extern uint8_t InputPortOut1;
extern uint8_t InputPortOut3;
extern uint8_t InputPortOut5;
extern uint8_t InputPortWatchdog;

// CpuI8085.c line 33 & ...
extern ReadHandler *memoryReadHandler;
// CpuI8085.c line 44
extern WriteHandler *memoryWriteHandler;

// #define OP_ROM boardMemory
// #define OP_RAM boardMemory
// #define cpu_readop(A) (OP_ROM[A])
// #define cpu_readop_arg(A) (OP_RAM[A])
// extern void cpu_setOPbase16(uint32_t pc);
// #define change_pc16(pc) // setop(pc);

#include "machines/mb14241.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define VISIBLE_AREA_FULL (struct VisibleArea){0, 65535, 0, 65535}

	extern struct VisibleArea visibleArea;

	struct VisibleArea
	{
		uint32_t minX;
		uint32_t maxX;
		uint32_t minY;
		uint32_t maxY;
	};

#define TRANSPARENCY_NONE 0
#define TRANSPARENCY_BLACK 1
#ifdef ESP32P4
#define TRANSPARENCY_BLACK_COLOR 0
#else
#define TRANSPARENCY_BLACK_COLOR 255
#endif
#define TRANSPARENCY_REPLACE 2
#define TRANSPARENT_NONE_COLOR 0

	extern THE_COLOR froggerWater;

#define CHECK_IF_DIRTY_X(x) DIRTY_MIN(x, screenDirtyMinX) DIRTY_MAX(x, screenDirtyMaxX)
#define CHECK_IF_DIRTY_Y(y) DIRTY_MIN(y, screenDirtyMinY) DIRTY_MAX(y, screenDirtyMaxY)
#define CHECK_IF_DIRTY_XY(x, y) CHECK_IF_DIRTY_X(x) CHECK_IF_DIRTY_Y(y)

	extern void GameDrawElement(THE_COLOR *theScreen, uint32_t atX, uint32_t atY, bool flipX, bool flipY, uint16_t tileIndex, uint8_t paletteIndex, uint8_t blackIsTransparent, THE_COLOR replacedColor);

	extern uint8_t Z80InterruptVector[MAX_Z80_CPU];
	extern bool Z80InterruptEnable[MAX_Z80_CPU];
	extern uint8_t Z80CurrentRunningCpu;
	extern uint8_t Z80CurrentCpu;
	extern bool Z80AskForNMI[MAX_Z80_CPU];

	// use in CpuI8085cpu.h line 99/100
	extern uint8_t readPortHandler(uint8_t device_number);
	extern int readPort0(int offset);
	extern int readPort1(int offset);
	extern int readPort2(int offset);
	extern int readPort3(int offset);
	extern int readPort4(int offset);
	extern int readPort5(int offset);
	extern int readPort6(int offset);
	extern int readPort(int port);
	void writePortHandler(uint8_t device_number, uint8_t data);
	extern void writePort0(int offset, int data);
	extern void writePort1(int offset, int data);
	extern void writePort2(int offset, int data);
	extern void writePort3(int offset, int data);
	extern void writePort4(int offset, int data);
	extern void writePort5(int offset, int data);
	extern void writePort6(int offset, int data);
	extern void writePortNone(int offset, int data);
	extern void writePort(int port, int value);

	extern int readMemoryHandler(int address);
	extern void writeMemoryHandler(int address, int value);

	extern bool keyChanged[BUTTON_END + 1];
	extern bool keyPressed[BUTTON_END + 1];
	extern uint8_t keyPort[BUTTON_END + 1];
	extern uint8_t keyBit[BUTTON_END + 1];
	extern uint8_t keyValuePressed[BUTTON_END + 1];
	extern bool IsKeyChanged(uint8_t button);

	extern THE_COLOR myWhite;
	extern THE_COLOR myBlack;
	extern THE_COLOR myGreen;
	extern THE_COLOR myRed;
	extern THE_COLOR myYellow;
	extern THE_COLOR myCyan;
	extern THE_COLOR myPurple;

	extern bool hasColor;
	extern bool hasPalette;
	extern bool hasTiles;
	extern bool hasSprites;
	extern bool hasGfx;

	// All ROM stuff
	// main CPU memory
	extern uint8_t *boardMemory;
	extern uint32_t boardMemorySize;
	// extern uint32_t boardMemoryWriteMin;
	// extern uint32_t boardMemoryWriteMax;
	extern int boardMemoryRead0(int address);
	extern int boardMemoryRead(int address);
	extern void boardMemoryWrite(int address, int value);
	extern void boardMemoryWriteNone(int address, int value);

	// graphics memory
	extern uint8_t *gfxMemory;
	extern uint32_t gfxMemorySize;
	// color memory
	extern uint8_t *colorMemory;
	extern uint32_t colorMemorySize;
	// palette memory
	extern uint8_t *paletteMemory;
	extern uint32_t paletteMemorySize;
	// tile memory
	// extern uint8_t *tileMemory;
	extern uint32_t tileMemorySize;
	// sprite memory
	// extern uint8_t *spriteMemory;
	extern uint32_t spriteMemorySize;
	// sound memory
	extern uint8_t *soundMemory;
	extern uint32_t soundMemorySize;

	extern uint8_t *dirtyMemoryTiles;
	extern uint8_t *dirtyMemorySprites;

	extern THE_COLOR *colorRGB;
	extern uint16_t paletteColorSize;
	extern THE_COLOR *paletteColor;

	extern struct GfxElement *element;

	extern uint8_t *tileGfx;
	extern uint16_t tileWidth;
	extern uint16_t tileHeight;
	extern uint16_t tilesCount;
	extern uint8_t *spriteGfx;
	extern uint16_t spriteWidth;
	extern uint16_t spriteHeight;
	extern uint16_t spritesCount;

	extern struct GfxElement *allGfx[2];

	extern THE_COLOR *screenData;
	extern THE_COLOR *screenDataOld;
	extern uint32_t screenWidth;
	extern uint32_t screenHeight;
	extern uint32_t screenLength;
	extern uint32_t screenDirtyMinX;
	extern uint32_t screenDirtyMinY;
	extern uint32_t screenDirtyMaxX;
	extern uint32_t screenDirtyMaxY;

	// extern int readbit(const uint8_t *src,int bitnum);
	// static int readbit(const uint8_t *src, int bitnum)
	//{
	//	return (src[bitnum / 8] >> (7 - bitnum % 8)) & 1;
	//}

	// struct InputPort *input_ports;

	struct GameDriver
	{
		const char *source_file;		   /* set this to __FILE__ */
		const struct GameDriver *clone_of; /* if this is a clone, point to */
										   /* the main version of the game */
		const char *name;
		const char *description;
		const char *year;
		const char *manufacturer;
		const char *credits;
		int flags; /* see defines below */
		const struct MachineDriver *drv;
		void (*driver_init)(void); /* optional function to be called during initialization */
								   /* This is called ONCE, unlike Machine->init_machine */
								   /* which is called every time the game is reset. */

		const struct RomModule *rom;
		void (*rom_decode)(void);	 /* used to decrypt the ROMs after loading them */
		void (*opcode_decode)(void); /* used to decrypt the CPU opcodes in the ROMs, */
									 /* if the encryption is different from the above. */
		const char **samplenames;	 /* optional array of names of samples to load. */
									 /* drivers can retrieve them in Machine->samples */
		const uint8_t *sound_prom;

		/* if they are available, provide a dump of the color proms, or even */
		/* better load them from disk like the other ROMs. */
		/* If you load them from disk, you must place them in a memory region by */
		/* itself, and use the PROM_MEMORY_REGION macro below to say in which */
		/* region they are. */
		const uint8_t *color_prom;
		const uint8_t *palette;
		const unsigned short *colortable;
		int orientation; /* orientation of the monitor; see defines below */

		int (*hiscore_load)(void);	/* will be called every vblank until it */
									/* returns nonzero */
		void (*hiscore_save)(void); /* will not be called if hiscore_load() hasn't yet */
									/* returned nonzero, to avoid saving an invalid table */
	};

	struct GfxLayout
	{
		unsigned short width, height; /* width and height of chars/sprites */
		unsigned int total;			  /* total numer of chars/sprites in the rom */
		unsigned short planes;		  /* number of bitplanes */
		int planeoffset[8];			  /* start of every bitplane */
		int xoffset[32];			  /* coordinates of the bit corresponding to the pixel */
		int yoffset[32];			  /* of the given coordinates */
		short charincrement;		  /* distance between two consecutive characters/sprites */
	};

	struct GfxDecodeInfo
	{
		int memory_region; /* memory region where the data resides (usually 1) */
						   /* -1 marks the end of the array */
		int start;		   /* beginning of data to decode */
		struct GfxLayout *gfxlayout;
		int color_codes_start; /* offset in the color lookup table where color codes start */
		int total_color_codes; /* total number of color codes */
	};

	struct osd_bitmap
	{
		int width, height; /* width and height of the bitmap */
		int depth;		   /* bits per pixel - ASG 980209 */
		// void *_private; /* don't touch! - reserved for osdepend use */
		uint8_t **line; /* pointers to the start of each line */
	};

	// #define SCREEN_DEPTH 16

	struct GfxElement
	{
		int width, height;

		struct osd_bitmap *gfxdata;	 /* graphic data */
		unsigned int total_elements; /* total number of characters/sprites */

		int color_granularity;		/* number of colors for each color code */
									/* (for example, 4 for 2 bitplanes gfx) */
		unsigned short *colortable; /* map color codes to screen pens */
									/* if this is 0, the function does a verbatim copy */
		int total_colors;
		unsigned int *pen_usage; /* an array of total_elements ints. */
								 /* It is a table of the pens each character uses */
								 /* (bit 0 = pen 0, and so on). This is used by */
								 /* drawgfgx() to do optimizations like skipping */
								 /* drawing of a totally transparent characters */
	};

#ifdef __cplusplus
}
#endif

#endif
