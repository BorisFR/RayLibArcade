#include "Pengo.h"

struct GfxLayout pengoTileLayout =
	{
		8, 8,													  /* 8*8 characters */
		256,													  /* 256 characters */
		2,														  /* 2 bits per pixel */
		{0, 4},													  /* the two bitplanes for 4 pixels are packed into one byte */
		{8 * 8 + 0, 8 * 8 + 1, 8 * 8 + 2, 8 * 8 + 3, 0, 1, 2, 3}, /* bits are packed in groups of four */
		{0 * 8, 1 * 8, 2 * 8, 3 * 8, 4 * 8, 5 * 8, 6 * 8, 7 * 8},
		16 * 8 /* every char takes 16 bytes */
};

struct GfxLayout pengoSpriteLayout =
	{
		16, 16, /* 16*16 sprites */
		64,		/* 64 sprites */
		2,		/* 2 bits per pixel */
		{0, 4}, /* the two bitplanes for 4 pixels are packed into one byte */
		{8 * 8, 8 * 8 + 1, 8 * 8 + 2, 8 * 8 + 3, 16 * 8 + 0, 16 * 8 + 1, 16 * 8 + 2, 16 * 8 + 3, 24 * 8 + 0, 24 * 8 + 1, 24 * 8 + 2, 24 * 8 + 3, 0, 1, 2, 3},
		{0 * 8, 1 * 8, 2 * 8, 3 * 8, 4 * 8, 5 * 8, 6 * 8, 7 * 8, 32 * 8, 33 * 8, 34 * 8, 35 * 8, 36 * 8, 37 * 8, 38 * 8, 39 * 8},
		64 * 8 /* every sprite takes 64 bytes */
};

WRITE_HANDLER(pengo_interrupt_enable_w)
{
	if (data & 1)
		Z80AskForNMI[Z80CurrentCpu] = true;
	else
		Z80AskForNMI[Z80CurrentCpu] = false;
}

uint8_t pengo_gfx_bank = 0;

WRITE_HANDLER(pengo_gfxbank_w)
{
	/* the Pengo hardware can set independently the palette bank, color lookup */
	/* table, and chars/sprites. However the game always set them together (and */
	/* the only place where this is used is the intro screen) so I don't bother */
	/* emulating the whole thing. */
	if (pengo_gfx_bank != (data & 1))
	{
		pengo_gfx_bank = data & 1;
		memset(dirtybuffer, 1, videoram_size);
	}
}

static int flipscreen;

WRITE_HANDLER(pengo_flipscreen_w)
{
	if (flipscreen != (data & 1))
	{
		flipscreen = data & 1;
		memset(dirtybuffer, 1, videoram_size);
	}
}

void PengoDecodeRoms()
{
	return;
	static const unsigned char xortable[32][4] =
		{
			/*       opcode                   data                     address      */
			/*  A    B    C    D         A    B    C    D                           */
			{0xa0, 0x88, 0x88, 0xa0},
			{0x28, 0xa0, 0x28, 0xa0}, /* ...0...0...0...0 */
			{0x28, 0xa0, 0x28, 0xa0},
			{0xa0, 0x88, 0x88, 0xa0}, /* ...0...0...0...1 */
			{0xa0, 0x88, 0x00, 0x28},
			{0xa0, 0x88, 0x00, 0x28}, /* ...0...0...1...0 */
			{0x08, 0x20, 0xa8, 0x80},
			{0xa0, 0x88, 0x88, 0xa0}, /* ...0...0...1...1 */
			{0x08, 0x08, 0xa8, 0xa8},
			{0x28, 0xa0, 0x28, 0xa0}, /* ...0...1...0...0 */
			{0xa0, 0x88, 0x00, 0x28},
			{0x08, 0x08, 0xa8, 0xa8}, /* ...0...1...0...1 */
			{0xa0, 0x88, 0x00, 0x28},
			{0xa0, 0x88, 0x00, 0x28}, /* ...0...1...1...0 */
			{0xa0, 0x88, 0x00, 0x28},
			{0x00, 0x00, 0x00, 0x00}, /* ...0...1...1...1 */
			{0x88, 0x88, 0x28, 0x28},
			{0xa0, 0x88, 0x00, 0x28}, /* ...1...0...0...0 */
			{0x88, 0x88, 0x28, 0x28},
			{0x00, 0x00, 0x00, 0x00}, /* ...1...0...0...1 */
			{0x08, 0x20, 0xa8, 0x80},
			{0x08, 0x20, 0xa8, 0x80}, /* ...1...0...1...0 */
			{0xa0, 0x88, 0x88, 0xa0},
			{0xa0, 0x88, 0x00, 0x28}, /* ...1...0...1...1 */
			{0x08, 0x08, 0xa8, 0xa8},
			{0x88, 0x88, 0x28, 0x28}, /* ...1...1...0...0 */
			{0x00, 0x00, 0x00, 0x00},
			{0x88, 0x88, 0x28, 0x28}, /* ...1...1...0...1 */
			{0x08, 0x20, 0xa8, 0x80},
			{0x08, 0x20, 0xa8, 0x80}, /* ...1...1...1...0 */
			{0x08, 0x08, 0xa8, 0xa8},
			{0xa0, 0x88, 0x00, 0x28} /* ...1...1...1...1 */
		};
	/*
		int A;
		//unsigned char *rom = boardMemory; // memory_region(REGION_CPU1);
		int diff = 0x10000; //boardMemorySize / 2;	  // memory_region_length(REGION_CPU1) / 2;
		//memory_set_opcode_base(0, rom + diff);
		for (A = 0x0000; A < 0x8000; A++)
		{
			int row, col;
			unsigned char src;
			src = boardMemory[A];
			// pick the translation table from bits 0, 4, 8 and 12 of the address
			row = (A & 1) + (((A >> 4) & 1) << 1) + (((A >> 8) & 1) << 2) + (((A >> 12) & 1) << 3);
			// pick the offset in the table from bits 3 and 5 of the source data
			col = ((src >> 3) & 1) + (((src >> 5) & 1) << 1);
			// the bottom half of the translation table is the mirror image of the top
			if (src & 0x80)
				col = 3 - col;
			// decode the opcodes
			boardMemory[A + diff] = src ^ xortable[2 * row][col];
			// decode the data
			boardMemory[A] = src ^ xortable[2 * row + 1][col];
			if (xortable[2 * row][col] == 0xff) // table incomplete! (for development)
				boardMemory[A + diff] = 0x00;
			if (xortable[2 * row + 1][col] == 0xff) // table incomplete! (for development)
				boardMemory[A] = 0xee;
		}
		// copy the opcodes from the not encrypted part of the ROMs
		for (A = 0x8000; A < diff; A++)
			boardMemory[A + diff] = boardMemory[A];
			*/
	/*
		int A;
		uint8_t *RAM = boardMemory; // Machine->memory_region[Machine->drv->cpu[0].memory_region];
		uint8_t *ROM = (uint8_t *)malloc(boardMemorySize);

		for (A = 0x0000; A < 0x8000; A++)
		{
			int row, col;
			unsigned char src;
			src = RAM[A];
			// pick the translation table from bits 0, 4, 8 and 12 of the address
			row = (A & 1) + (((A >> 4) & 1) << 1) + (((A >> 8) & 1) << 2) + (((A >> 12) & 1) << 3);
			// pick the offset in the table from bits 3 and 5 of the source data
			col = ((src >> 3) & 1) + (((src >> 5) & 1) << 1);
			// the bottom half of the translation table is the mirror image of the top
			if (src & 0x80)
				col = 3 - col;
			// decode the opcodes
			ROM[A] = src ^ xortable[2 * row][col];
			// decode the data
			RAM[A] = src ^ xortable[2 * row + 1][col];
			if (xortable[2 * row][col] == 0xff) // table incomplete! (for development)
				ROM[A] = 0x00;
			if (xortable[2 * row + 1][col] == 0xff) // table incomplete! (for development)
				RAM[A] = 0xee;
		}
		// copy the opcodes from the not encrypted part of the ROMs
		for (A = 0x8000; A < 0x10000; A++)
			ROM[A] = RAM[A];
			*/

	int A;
	unsigned char *RAM = boardMemory; // Machine->memory_region[Machine->drv->cpu[0].memory_region];
	for (A = 0x0000; A < 0x8000; A++)
	{
		int row, col;
		unsigned char src;
		src = RAM[A];
		/* pick the translation table from bits 0, 4, 8 and 12 of the address */
		row = (A & 1) + (((A >> 4) & 1) << 1) + (((A >> 8) & 1) << 2) + (((A >> 12) & 1) << 3);
		/* pick the offset in the table from bits 3 and 5 of the source data */
		col = ((src >> 3) & 1) + (((src >> 5) & 1) << 1);
		/* the bottom half of the translation table is the mirror image of the top */
		if (src & 0x80)
			col = 3 - col;
		/* decode the opcodes */
		// ROM[A] = src ^ xortable[2*row][col];
		RAM[A + 0x10000] = src ^ xortable[2 * row][col];
		/* decode the data */
		RAM[A] = src ^ xortable[2 * row + 1][col];
		if (xortable[2 * row][col] == 0xff)
		{ /* table incomplete! (for development) */
			// ROM[A] = 0x00;
			RAM[A + 0x10000] = 0x00;
		}
		if (xortable[2 * row + 1][col] == 0xff) /* table incomplete! (for development) */
			RAM[A] = 0xee;
	}
	/* copy the opcodes from the not encrypted part of the ROMs */
	// for (A = 0x8000;A < 0x10000;A++)
	//	ROM[A] = RAM[A];
	for (A = 0x8000; A < 0x10000; A++)
		RAM[A + 0x10000] = RAM[A];
	for (A = 0x0000; A < 0x10000; A++)
		RAM[A] = RAM[A + 0x10000];
}

void PengoInit()
{
}

void PengoRefreshScreen()
{
	// return;
	element = allGfx[pengo_gfx_bank * 2];
	visibleArea = VISIBLE_AREA_FULL;
	int sx, sy;
	for (int offs = 0x400 - 1; offs >= 0; offs--)
	{
		int mx = offs % 32;
		int my = offs / 32;
		if (my < 2)
		{
			if (mx < 2 || mx >= 30)
				continue; /* not visible */
			sx = my + 34;
			sy = mx - 2;
		}
		else if (my >= 30)
		{
			if (mx < 2 || mx >= 30)
				continue; /* not visible */
			sx = my - 30;
			sy = mx - 2;
		}
		else
		{
			sx = mx + 2;
			sy = my - 2;
		}
		if (flipscreen)
		{
			sx = 35 - sx;
			sy = 27 - sy;
		}

		int tileIndex = boardMemory[0x8000 + offs];
		int paletteIndex = boardMemory[0x8400 + offs] & 0x17;
		GameDrawElement(screenData, sx * 8, sy * 8, flipscreen, flipscreen, tileIndex, paletteIndex, TRANSPARENCY_NONE, TRANSPARENT_NONE_COLOR);
	}
	// drawgfx(tmpbitmap,Machine->gfx[gfx_bank*2],
	//		videoram[offs],
	//		colorram[offs] & 0x1f,
	//		flipscreen,flipscreen,
	//		sx*8,sy*8,
	//		&Machine->drv->visible_area,TRANSPARENCY_NONE,0);

	/* Draw the sprites */
	element = allGfx[pengo_gfx_bank * 2 + 1];
	visibleArea = allGames[currentGame].video.visibleArea;
	for (int spriteNumber = 7; spriteNumber >= 0; spriteNumber--)
	{
		const uint8_t *base = &boardMemory[0x9020 + spriteNumber * 2];
		uint8_t sx = 272 - base[1];
		uint8_t sy = base[0];
		uint8_t temp = boardMemory[0x8ff0 + spriteNumber * 2];
		uint16_t code = temp >> 2;
		uint8_t color = boardMemory[0x8ff0 + spriteNumber * 2 + 1] & 0x1f;
		uint8_t flipx = temp & 1;
		uint8_t flipy = temp & 2;
		GameDrawElement(screenData, sx, sy, flipx, flipy, code, color, TRANSPARENCY_BLACK, TRANSPARENT_NONE_COLOR);
	}
}
