#include "TheMemory.h"

uint8_t countGames = 0;

uint8_t *videoram;
int videoram_size;
uint8_t *colorram;
uint8_t *spriteram;
int spriteram_size;
uint8_t *spriteram_2;
int spriteram_size_2;

THE_BACKGROUND_COLOR *screenBackground = NULL;
uint32_t screenBackgroundMemorySize;
uint16_t screenBackgroundWidth;
uint16_t screenBackgroundHeight;

// *******************************************************************

uint16_t screenPosX = 0;
uint16_t screenPosY = 0;

struct VisibleArea visibleArea;
THE_COLOR froggerWater;

bool GameTestSpriteOnTile(uint16_t spriteX, uint16_t spriteY, uint16_t spriteWidth, uint16_t spriteHeight, uint16_t tileX, uint16_t tileY, uint16_t tileWidth, uint16_t tileHeight)
{
    // X sprite start in the tile
    if (spriteX >= tileX && (spriteX) < (tileX + tileWidth))
    {
        if (spriteY >= tileY && spriteY < (tileY + tileHeight))
        {
            return true;
        }
        else
        {
            if ((spriteY + spriteHeight) >= tileY && (spriteY + spriteHeight) < (tileY + tileHeight))
            {
                return true;
            }
            else
            {
                if ((spriteY) < tileY && (spriteY + spriteHeight) > (tileY + tileHeight))
                {
                    return true;
                }
            }
        }
    }
    else
    {
        // X sprite end in the tile
        if ((spriteX + spriteWidth) >= tileX && (spriteX + spriteWidth) < (tileX + tileWidth))
        {
            if (spriteY >= tileY && spriteY < (tileY + tileHeight))
            {
                return true;
            }
            else
            {
                if ((spriteY + spriteHeight) >= tileY && (spriteY + spriteHeight) < (tileY + tileHeight))
                {
                    return true;
                }
                else
                {
                    if ((spriteY) < tileY && (spriteY + spriteHeight) > (tileY + tileHeight))
                    {
                        return true;
                    }
                }
            }
        }
        else
        {
            // X sprite start before and end after the tile
            if (spriteX < tileX && (spriteX + spriteWidth) > (tileX + tileWidth))
            {
                if (spriteY >= tileY && spriteY < (tileY + tileHeight))
                {
                    return true;
                }
                else
                {
                    if ((spriteY + spriteHeight) >= tileY && (spriteY + spriteHeight) < (tileY + tileHeight))
                    {
                        return true;
                    }
                    else
                    {
                        if ((spriteY) < tileY && (spriteY + spriteHeight) > (tileY + tileHeight))
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

void GameScrollLine(uint16_t line, uint16_t scroll, uint16_t height)
{
    for (uint8_t y = 0; y < height; y++)
    {
        uint32_t currentLine = (line * height + y) * screenGameWidth;
        for (int x = screenGameWidth - 1; x >= 0; x--)
        {
            uint32_t shiftX = (x + scroll) % screenGameWidth;
            if (shiftX >= visibleArea.minX && shiftX <= visibleArea.maxX)
            {
                THE_COLOR c = screenBitmap[x + currentLine];
                if (c == TRANSPARENCY_BLACK_COLOR)
                {
                    screenGameDirty[shiftX + currentLine] = DIRTY_TRANSPARENT;
                }
                else
                {
                    screenGame[shiftX + currentLine] = c;
                    screenGameDirty[shiftX + currentLine] = DIRTY_YES;
                }
            }
            else
            {
                screenGameDirty[shiftX + currentLine] = DIRTY_TRANSPARENT;
            }
        }
    }
}

void GamePlotPixel(uint16_t x, uint16_t y, THE_COLOR color)
{
    uint32_t index = x + y * screenGameWidth;
    screenGame[index] = color;
    screenGameDirty[index] = DIRTY_YES;
}

void GameClearPixel(uint16_t x, uint16_t y)
{
    uint32_t index = x + y * screenGameWidth;
    // uint32_t p = x + screenPosX + (y + screenPosY) * pngWidth;
    //  screenGame[index] = myBlack;
    // screenGame[index] = screenBackground[p];
    screenGameDirty[index] = DIRTY_TRANSPARENT;
}

void GameDrawElement(THE_COLOR *theScreen, uint16_t tileX, uint16_t tileY, bool flipX, bool flipY, uint16_t tileIndex, uint8_t paletteIndex, uint8_t blackIsTransparent, THE_COLOR replacedColor)
{
    if (flipX && flipY)
    { // FLIP X & FLIP Y
        for (uint16_t y = 0; y < element->height; y++)
        {
            uint16_t tempY = tileY + element->height - 1 - y;
            if (tempY >= visibleArea.minY && tempY < visibleArea.maxY)
            {
                uint8_t *pointerLine = element->gfxdata->line[y + tileIndex * element->height];
                for (uint16_t x = 0; x < element->width; x++)
                {
                    uint16_t tempX = tileX + element->width - x;
                    if (tempX >= visibleArea.minX && tempX < visibleArea.maxX)
                    {
                        CHECK_IF_DIRTY_XY(tempX, tempY)
                        uint8_t pixel = pointerLine[x];
                        THE_COLOR color = paletteColor[paletteIndex * 4 + pixel];
                        if (blackIsTransparent == TRANSPARENCY_REPLACE && color == TRANSPARENCY_BLACK_COLOR)
                        {
                            uint32_t index = tempX + tempY * screenGameWidth;
                            theScreen[index] = replacedColor;
                            screenGameDirty[index] = DIRTY_TRANSPARENT;
                        }
                        else if (!(blackIsTransparent == TRANSPARENCY_BLACK && color == TRANSPARENCY_BLACK_COLOR))
                        {
                            uint32_t index = tempX + tempY * screenGameWidth;
                            theScreen[index] = color;
                            screenGameDirty[index] = DIRTY_YES;
                        }
                    }
                }
            }
        }
        return;
    } // FLIP X & FLIP Y
    if (flipX)
    { // FLIP X
        for (uint16_t y = 0; y < element->height; y++)
        {
            uint16_t tempY = tileY + y;
            if (tempY >= visibleArea.minY && tempY < visibleArea.maxY)
            {
                uint8_t *pointerLine = element->gfxdata->line[y + tileIndex * element->height];
                for (uint16_t x = 0; x < element->width; x++)
                {
                    uint16_t tempX = tileX + element->width - x;
                    if (tempX >= visibleArea.minX && tempX < visibleArea.maxX)
                    {
                        CHECK_IF_DIRTY_XY(tempX, tempY)
                        uint8_t pixel = pointerLine[x];
                        THE_COLOR color = paletteColor[paletteIndex * 4 + pixel];
                        if (blackIsTransparent == TRANSPARENCY_REPLACE && color == TRANSPARENCY_BLACK_COLOR)
                        {
                            uint32_t index = tempX + tempY * screenGameWidth;
                            theScreen[index] = replacedColor;
                            screenGameDirty[index] = DIRTY_TRANSPARENT;
                        }
                        else if (!(blackIsTransparent == TRANSPARENCY_BLACK && color == TRANSPARENCY_BLACK_COLOR))
                        {
                            uint32_t index = tempX + tempY * screenGameWidth;
                            theScreen[index] = color;
                            screenGameDirty[index] = DIRTY_YES;
                        }
                    }
                }
            }
        }
        return;
    } // FLIP X
    if (flipY)
    { // FLIP Y
        for (uint16_t y = 0; y < element->height; y++)
        {
            uint16_t tempY = tileY + element->height - 1 - y;
            if (tempY >= visibleArea.minY && tempY < visibleArea.maxY)
            {
                uint8_t *pointerLine = element->gfxdata->line[y + tileIndex * element->height];
                for (uint16_t x = 0; x < element->width; x++)
                {
                    uint16_t tempX = x + tileX;
                    if (tempX >= visibleArea.minX && tempX < visibleArea.maxX)
                    {
                        CHECK_IF_DIRTY_XY(tempX, tempY)
                        uint8_t pixel = pointerLine[x];
                        THE_COLOR color = paletteColor[paletteIndex * 4 + pixel];
                        if (blackIsTransparent == TRANSPARENCY_REPLACE && color == TRANSPARENCY_BLACK_COLOR)
                        {
                            uint32_t index = tempX + tempY * screenGameWidth;
                            theScreen[index] = replacedColor;
                            screenGameDirty[index] = DIRTY_TRANSPARENT;
                        }
                        else if (!(blackIsTransparent == TRANSPARENCY_BLACK && color == TRANSPARENCY_BLACK_COLOR))
                        {
                            uint32_t index = tempX + tempY * screenGameWidth;
                            theScreen[index] = color;
                            screenGameDirty[index] = DIRTY_YES;
                        }
                    }
                }
            }
        }
        return;
    } // FLIP Y
    // NO FLIP
    for (uint16_t y = 0; y < element->height; y++)
    {
        uint16_t tempY = tileY + y;
        if (tempY >= visibleArea.minY && tempY < visibleArea.maxY)
        {
            uint8_t *pointerLine = element->gfxdata->line[y + tileIndex * element->height];
            for (uint16_t x = 0; x < element->width; x++)
            {
                uint16_t tempX = x + tileX;
                if (tempX >= visibleArea.minX && tempX < visibleArea.maxX)
                {
                    CHECK_IF_DIRTY_XY(tempX, tempY)
                    uint8_t pixel = pointerLine[x];
                    THE_COLOR color;
                    if (hasPalette)
                        color = paletteColor[paletteIndex * 4 + pixel];
                    else
                        color = colorRGB[paletteIndex * 4 + pixel];
                    if (blackIsTransparent == TRANSPARENCY_REPLACE && color == TRANSPARENCY_BLACK_COLOR)
                    {
                        uint32_t index = tempX + tempY * screenGameWidth;
                        theScreen[index] = replacedColor;
                        screenGameDirty[index] = DIRTY_TRANSPARENT;
                        // uint32_t bg = tempX + screenPosX + (tempY+screenPosY) * 800;
                        // theScreen[index] = screenBackground[bg];
                    }
                    else if (!(blackIsTransparent == TRANSPARENCY_BLACK && color == TRANSPARENCY_BLACK_COLOR))
                    {
                        uint32_t index = tempX + tempY * screenGameWidth;
                        theScreen[index] = color;
                        screenGameDirty[index] = DIRTY_YES;
                    }
                }
            }
        }
    }
}

uint8_t Z80InterruptVector[MAX_Z80_CPU];
bool Z80InterruptEnable[MAX_Z80_CPU];
uint8_t Z80CurrentRunningCpu;
uint8_t Z80CurrentCpu;
bool Z80AskForNMI[MAX_Z80_CPU];

ReadHandler *InputPortRead[BUTTON_END + 1];
WriteHandler *InputPortWrite[BUTTON_END + 1];
uint8_t InputPorts[BUTTON_END + 1];
uint8_t InputPortOut1;
uint8_t InputPortOut3;
uint8_t InputPortOut5;
uint8_t InputPortWatchdog;

uint8_t readPortHandler(uint8_t device_number)
{
    if (InputPortRead[device_number])
    {
        return InputPortRead[device_number]->handler(device_number);
    }
    return InputPorts[device_number];
}
int readPort(int port) { return InputPorts[port]; }
int readPort0(int offset) { return InputPorts[0]; }
int readPort1(int offset) { return InputPorts[1]; }
int readPort2(int offset) { return InputPorts[2]; }
int readPort3(int offset) { return InputPorts[3]; }
int readPort4(int offset) { return InputPorts[4]; }
int readPort5(int offset) { return InputPorts[5]; }
int readPort6(int offset) { return InputPorts[6]; }

void writePortHandler(uint8_t device_number, uint8_t data)
{
    if (InputPortWrite[device_number])
    {
        InputPortWrite[device_number]->handler(device_number, data);
        return;
    }
    // if (device_number <= BUTTON_END)
    {
        InputPorts[device_number] = data;
        // return;
    }
}
void writePort(int port, int value) { InputPorts[port] = value; }
void writePort0(int offset, int data) { InputPorts[0] = data; }
void writePort1(int offset, int data) { InputPorts[1] = data; }
void writePort2(int offset, int data) { InputPorts[2] = data; }
void writePort3(int offset, int data) { InputPorts[3] = data; }
void writePort4(int offset, int data) { InputPorts[4] = data; }
void writePort5(int offset, int data) { InputPorts[5] = data; }
void writePort6(int offset, int data) { InputPorts[6] = data; }
void writePortNone(int offset, int data) {}

ReadHandler *memoryReadHandler;
WriteHandler *memoryWriteHandler;

int readMemoryHandler(int address)
{
    if (address < 0)
    {
        MY_DEBUG2("MEMORY READ ERROR", "address neg:", address)
        // ESP_LOGE("MEMORY READ ERROR", "address neg: %x", address);
        return 0;
    }
    if (address >= boardMemorySize)
    {
        MY_DEBUG2("MEMORY READ ERROR", "address:", address)
        // ESP_LOGE("MEMORY READ ERROR", "address: %x", address);
        return 0;
    }
    if (memoryReadHandler == NULL)
    {
        MY_DEBUG2("MEMORY READ ERROR", "memoryReadHandler is NULL:", address)
        // ESP_LOGE("MEMORY READ ERROR", "memoryReadHandler NULL : %x", address);
        return 0;
    }
    if (memoryReadHandler[address].handler == NULL)
    {
        return boardMemory[address];
        // MY_DEBUG2("MEMORY READ ERROR", "memoryReadHandler handler is NULL:", address)
        // ESP_LOGE("MEMORY READ ERROR", "handler NULL : %x", address);
        // return 0;
    }
    else
    {
        return memoryReadHandler[address].handler(address);
    }
    // return boardMemory[address];
}

WRITE_HANDLER(videoram_w) { videoram[offset] = data; }
WRITE_HANDLER(colorram_w) { colorram[offset] = data; }

void writeMemoryHandler(int address, int value)
{
    // if (address < 0)
    // {
    //     MY_DEBUG2("MEMORY WRITE ERROR", "address neg:", address)
    //     return;
    // }
    // if (address >= boardMemorySize)
    // {
    //     MY_DEBUG2("MEMORY WRITE ERROR", "address:", address)
    //     return;
    // }
    // if (memoryWriteHandler == NULL)
    // {
    //     MY_DEBUG2("MEMORY WRITE ERROR", "memoryWriteHandler is NULL:", address)
    //     return;
    // }
    if (memoryWriteHandler[address].handler != NULL)
    {
        memoryWriteHandler[address].handler(address - memoryWriteHandler[address].toZero, value);
        return;
    }
    boardMemory[address] = value;
}

// Is button state has changed?
bool keyChanged[BUTTON_END + 1]; // = {BUTTON_CREDIT, BUTTON_START, BUTTON_LEFT, BUTTON_RIGHT, BUTTON_UP, BUTTON_DOWN, BUTTON_FIRE, BUTTON_2, BUTTON_END};
// Button is pushed or released?
bool keyPressed[BUTTON_END + 1];
// Input port number
uint8_t keyPort[BUTTON_END + 1];
// Input port bit number
uint8_t keyBit[BUTTON_END + 1];
// Input port default value
uint8_t keyValuePressed[BUTTON_END + 1];

bool IsKeyChanged(uint8_t button)
{
    if (keyChanged[button])
    {
        keyChanged[button] = false;
        return true;
    }
    return false;
}

THE_COLOR myWhite;
THE_COLOR myBlack;
THE_COLOR myGreen;
THE_COLOR myRed;
THE_COLOR myYellow;
THE_COLOR myCyan;
THE_COLOR myPurple;

bool hasColor;
bool hasPalette;
bool hasTiles;
bool hasSprites;
bool hasGfx;

uint8_t *boardMemory;
uint32_t boardMemorySize;
// uint32_t boardMemoryWriteMin;
// uint32_t boardMemoryWriteMax;
int boardMemoryRead0(int address) { return 0; }
int boardMemoryRead(int address) { return boardMemory[address]; }
int boardMemoryReadDecode(int address) { return boardMemory[address + 0x10000]; /*romptr[0][address];*/ }
void boardMemoryWrite(int address, int value)
{
    boardMemory[address] = value;
}
void boardMemoryWriteDecode(int address, int value)
{
    boardMemory[address] = value;
    boardMemory[address + 0x10000] = value;
}
void boardMemoryWriteNone(int address, int value)
{
    return;
}
/*
        public void Write16(int address, ushort value)
        {
            var lower = (byte)(value & 0x00FF);
            var upper = (byte)((value & 0xFF00) >> 8);
            Write(address, lower);
            Write(address + 1, upper);
        }
*/

// uint8_t *romptr[MAX_CPU];

void memory_set_opcode_base(int cpu, unsigned char *base)
{
    // romptr[cpu] = base;
}

uint8_t *gfxMemory;
uint32_t gfxMemorySize;
uint8_t *colorMemory;
uint32_t colorMemorySize;
uint8_t *soundMemory;
uint32_t soundMemorySize;
uint8_t *paletteMemory;
uint32_t paletteMemorySize;
uint8_t *tileMemory;
uint32_t tileMemorySize;
uint8_t *spriteMemory;
uint32_t spriteMemorySize;

uint8_t countGfxElement;
struct GfxElement *allGfx[MAX_GFX_ELEMENTS];

uint8_t *dirtyMemoryTiles;
uint8_t *dirtyMemorySprites;

THE_COLOR *colorRGB;
uint32_t paletteColorSize;
THE_COLOR *paletteColor;

struct GfxElement *element;

/*// uint8_t *tileGfx;
uint16_t tileWidth;
uint16_t tileHeight;
uint16_t tilesCount;
// uint8_t *spriteGfx;
uint16_t spriteWidth;
uint16_t spriteHeight;
uint16_t spritesCount;*/

THE_COLOR *screenGame = NULL;
uint8_t *screenGameDirty = NULL;
// THE_COLOR *screenGameOld = NULL;
THE_COLOR *screenBitmap = NULL;
uint16_t screenGameWidth;
uint16_t screenGameHeight;
uint32_t screenGameLength;
uint16_t screenDirtyMinX;
uint16_t screenDirtyMinY;
uint16_t screenDirtyMaxX;
uint16_t screenDirtyMaxY;
