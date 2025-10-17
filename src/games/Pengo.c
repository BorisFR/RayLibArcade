#include "Pengo.h"


struct GfxLayout pengoTileLayout =
{
	8,8,	/* 8*8 characters */
    256,    /* 256 characters */
    2,  /* 2 bits per pixel */
    { 0, 4 },   /* the two bitplanes for 4 pixels are packed into one byte */
    { 8*8+0, 8*8+1, 8*8+2, 8*8+3, 0, 1, 2, 3 }, /* bits are packed in groups of four */
    { 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
    16*8    /* every char takes 16 bytes */
};


struct GfxLayout pengoSpriteLayout =
{
	16,16,	/* 16*16 sprites */
	64,	/* 64 sprites */
	2,	/* 2 bits per pixel */
	{ 0, 4 },	/* the two bitplanes for 4 pixels are packed into one byte */
	{ 8*8, 8*8+1, 8*8+2, 8*8+3, 16*8+0, 16*8+1, 16*8+2, 16*8+3,
			24*8+0, 24*8+1, 24*8+2, 24*8+3, 0, 1, 2, 3 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
			32*8, 33*8, 34*8, 35*8, 36*8, 37*8, 38*8, 39*8 },
	64*8	/* every sprite takes 64 bytes */
};

void pengo_interrupt_enable_w(int offset, int data)
{
    if (data & 1)
        Z80AskForNMI[Z80CurrentCpu] = true;
    else
        Z80AskForNMI[Z80CurrentCpu] = false;
}

void pengo_gfxbank_w(int offset, int data)
{
}

void PengoDecodeRoms()
{
	static const unsigned char xortable[32][4] =
	{
		/*       opcode                   data                     address      */
		/*  A    B    C    D         A    B    C    D                           */
		{ 0xa0,0x88,0x88,0xa0 }, { 0x28,0xa0,0x28,0xa0 },	/* ...0...0...0...0 */
		{ 0x28,0xa0,0x28,0xa0 }, { 0xa0,0x88,0x88,0xa0 },	/* ...0...0...0...1 */
		{ 0xa0,0x88,0x00,0x28 }, { 0xa0,0x88,0x00,0x28 },	/* ...0...0...1...0 */
		{ 0x08,0x20,0xa8,0x80 }, { 0xa0,0x88,0x88,0xa0 },	/* ...0...0...1...1 */
		{ 0x08,0x08,0xa8,0xa8 }, { 0x28,0xa0,0x28,0xa0 },	/* ...0...1...0...0 */
		{ 0xa0,0x88,0x00,0x28 }, { 0x08,0x08,0xa8,0xa8 },	/* ...0...1...0...1 */
		{ 0xa0,0x88,0x00,0x28 }, { 0xa0,0x88,0x00,0x28 },	/* ...0...1...1...0 */
		{ 0xa0,0x88,0x00,0x28 }, { 0x00,0x00,0x00,0x00 },	/* ...0...1...1...1 */
		{ 0x88,0x88,0x28,0x28 }, { 0xa0,0x88,0x00,0x28 },	/* ...1...0...0...0 */
		{ 0x88,0x88,0x28,0x28 }, { 0x00,0x00,0x00,0x00 },	/* ...1...0...0...1 */
		{ 0x08,0x20,0xa8,0x80 }, { 0x08,0x20,0xa8,0x80 },	/* ...1...0...1...0 */
		{ 0xa0,0x88,0x88,0xa0 }, { 0xa0,0x88,0x00,0x28 },	/* ...1...0...1...1 */
		{ 0x08,0x08,0xa8,0xa8 }, { 0x88,0x88,0x28,0x28 },	/* ...1...1...0...0 */
		{ 0x00,0x00,0x00,0x00 }, { 0x88,0x88,0x28,0x28 },	/* ...1...1...0...1 */
		{ 0x08,0x20,0xa8,0x80 }, { 0x08,0x20,0xa8,0x80 },	/* ...1...1...1...0 */
		{ 0x08,0x08,0xa8,0xa8 }, { 0xa0,0x88,0x00,0x28 }	/* ...1...1...1...1 */
	};

    int A;
	uint8_t *RAM = boardMemory; // Machine->memory_region[Machine->drv->cpu[0].memory_region];
    uint8_t * ROM = (uint8_t *) malloc(boardMemorySize);

	for (A = 0x0000;A < 0x8000;A++)
	{
		int row,col;
		unsigned char src;
		src = RAM[A];
		/* pick the translation table from bits 0, 4, 8 and 12 of the address */
		row = (A & 1) + (((A >> 4) & 1) << 1) + (((A >> 8) & 1) << 2) + (((A >> 12) & 1) << 3);
		/* pick the offset in the table from bits 3 and 5 of the source data */
		col = ((src >> 3) & 1) + (((src >> 5) & 1) << 1);
		/* the bottom half of the translation table is the mirror image of the top */
		if (src & 0x80) col = 3 - col;
		/* decode the opcodes */
		ROM[A] = src ^ xortable[2*row][col];
		/* decode the data */
		RAM[A] = src ^ xortable[2*row+1][col];
		if (xortable[2*row][col] == 0xff)	/* table incomplete! (for development) */
			ROM[A] = 0x00;
		if (xortable[2*row+1][col] == 0xff)	/* table incomplete! (for development) */
			RAM[A] = 0xee;
	}
	/* copy the opcodes from the not encrypted part of the ROMs */
	for (A = 0x8000;A < 0x10000;A++)
		ROM[A] = RAM[A];
}

void PengoInit()
{
}

void PengoRefreshScreen()
{
    element = allGfx[0];
    visibleArea = VISIBLE_AREA_FULL;
    for (int offs = 0x400 - 1; offs >= 0; offs--)
    {
        // int sx = offs % 32;
        // int sy = offs / 32;
        // int sx = (31 - offs / 32);
        // int sy = (offs % 32);
        // Because screen is rotate
        int sy = offs % 32;
        int sx = offs / 32;
       //if (!galaxianFlipX)
       //    sx = 31 - sx;
       //if (galaxianFlipY)
       //    sy = 31 - sy;
        int tileIndex = boardMemory[0x5000 + offs];
        int paletteIndex = boardMemory[0x5800 + 2 * (offs % 32) + 1] & 0x07;
        //GameDrawElement(screenBitmap, sx * 8, sy * 8, galaxianFlipX, galaxianFlipY, tileIndex, paletteIndex, TRANSPARENCY_NONE, TRANSPARENT_NONE_COLOR);
        GameDrawElement(screenBitmap, sx * 8, sy * 8, false, false, tileIndex, paletteIndex, TRANSPARENCY_NONE, TRANSPARENT_NONE_COLOR);
    }
    // scroll
    for (uint8_t l = 0; l < 32; l++)
    {
        uint8_t scroll = boardMemory[0x5800 + 2 * l] % screenWidth;
        GameScrollLine(l, scroll, 8);
    }
    /* Draw the bullets */
    element = allGfx[2];
    visibleArea = allGames[currentGame].video.visibleArea;
    for (int offs = 0; offs < 0x20; offs += 4)
    {
        int color = 6; // 1; /* white */
        if (offs == 7 * 4)
            color = 7;                                    // 0; /* yellow */
        int y = 255 - boardMemory[0x5860 + offs + 3] - 1; // Machine->drv->gfxdecodeinfo[2].gfxlayout->width;
        int x = boardMemory[0x5860 + offs + 1];
        //if (galaxianFlipY)
        //    y = 255 - y;
        //GameDrawElement(screenData, x, y, galaxianFlipX, galaxianFlipY, 0, color, TRANSPARENCY_NONE, TRANSPARENT_NONE_COLOR);
        GameDrawElement(screenData, x, y, false, false, 0, color, TRANSPARENCY_NONE, TRANSPARENT_NONE_COLOR);
    }
    /* Draw the sprites */
    element = allGfx[1];
    visibleArea = allGames[currentGame].video.visibleArea;
    for (int spriteNumber = 7; spriteNumber >= 0; spriteNumber--)
    {
        const uint8_t *base = &boardMemory[0x5840 + spriteNumber * 4];
        uint8_t sx = base[0];
        uint16_t code = base[1] & 0x3f;
        uint8_t flipx = base[1] & 0x40;
        uint8_t flipy = base[1] & 0x80;
        uint8_t color = base[2] & 7;
        uint8_t sy = base[3];
        GameDrawElement(screenData, sx, sy, flipx, flipy, code, color, TRANSPARENCY_BLACK, TRANSPARENT_NONE_COLOR);
    }    
}
