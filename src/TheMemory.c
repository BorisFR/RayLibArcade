#include "TheMemory.h"

// *******************************************************************
struct VisibleArea visibleArea;

uint8_t maxp = 0;

void GameDrawElement(THE_COLOR *screenData, uint32_t atX, uint32_t atY, bool flipX, bool flipY, uint16_t tileIndex, uint8_t paletteIndex, bool blackIsTransparent)
{
    DIRTY_MIN(atX, screenDirtyMinX)
    DIRTY_MAX(atX + element->width, screenDirtyMaxX)
    DIRTY_MIN(atY, screenDirtyMinY)
    DIRTY_MAX(atY + element->height, screenDirtyMaxY)
    if (flipX && flipY)
    { // FLIP X & FLIP Y
        for (uint16_t y = 0; y < element->height; y++)
        {
            uint16_t tempY = atY + element->height - 1 - y;
            if (tempY >= visibleArea.minY && tempY < visibleArea.maxY)
            {
                uint8_t *pointerLine = element->gfxdata->line[y + tileIndex * element->height];
                for (uint16_t x = 0; x < element->width; x++)
                {
                    uint16_t tempX = atX + element->width - x;
                    if (tempX >= visibleArea.minX && tempX < visibleArea.maxX)
                    {
                        uint8_t pixel = pointerLine[x];
                        THE_COLOR color = paletteColor[paletteIndex * 4 + pixel];
                        if (!blackIsTransparent || color != 255)
                        {
                            uint32_t index = tempX + tempY * screenWidth;
                            screenData[index] = color;
                        }
                    }
                }
            }
        }
    } // FLIP X & FLIP Y
    else
    { // else FLIP X & FLIP Y
        if (flipX)
        { // FLIP X
            for (uint16_t y = 0; y < element->height; y++)
            {
                uint16_t tempY = atY + y;
                if (tempY >= visibleArea.minY && tempY < visibleArea.maxY)
                {
                    uint8_t *pointerLine = element->gfxdata->line[y + tileIndex * element->height];
                    for (uint16_t x = 0; x < element->width; x++)
                    {
                        uint16_t tempX = atX + element->width - x;
                        if (tempX >= visibleArea.minX && tempX < visibleArea.maxX)
                        {
                            uint8_t pixel = pointerLine[x];
                            THE_COLOR color = paletteColor[paletteIndex * 4 + pixel];
                            if (!blackIsTransparent || color != 255)
                            {
                                uint32_t index = tempX + tempY * screenWidth;
                                screenData[index] = color;
                            }
                        }
                    }
                }
            }
        } // FLIP X
        else
        { // else FLIP X
            if (flipY)
            {
                for (uint16_t y = 0; y < element->height; y++)
                {
                    uint16_t tempY = atY + element->height - 1 - y;
                    if (tempY >= visibleArea.minY && tempY < visibleArea.maxY)
                    {
                        uint8_t *pointerLine = element->gfxdata->line[y + tileIndex * element->height];
                        for (uint16_t x = 0; x < element->width; x++)
                        {
                            uint16_t tempX = x + atX;
                            if (tempX >= visibleArea.minX && tempX < visibleArea.maxX)
                            {
                                uint8_t pixel = pointerLine[x];
                                THE_COLOR color = paletteColor[paletteIndex * 4 + pixel];
                                if (!blackIsTransparent || color != 255)
                                {
                                    uint32_t index = tempX + tempY * screenWidth;
                                    screenData[index] = color;
                                }
                            }
                        }
                    }
                }
            } // FLIP Y
            else
            { // NO FLIP
                for (uint16_t y = 0; y < element->height; y++)
                {
                    uint16_t tempY = atY + y;
                    if (tempY >= visibleArea.minY && tempY < visibleArea.maxY)
                    {
                        uint8_t *pointerLine = element->gfxdata->line[y + tileIndex * element->height];
                        for (uint16_t x = 0; x < element->width; x++)
                        {
                            uint16_t tempX = x + atX;
                            if (tempX >= visibleArea.minX && tempX < visibleArea.maxX)
                            {
                                uint8_t pixel = pointerLine[x];
                                if(pixel> maxp){
                                    maxp=pixel;
                                    MY_DEBUG2("PIX","Pixel:", maxp)
                                }
                                THE_COLOR color;
                                if (hasPalette)
                                    color = paletteColor[paletteIndex * 4 + pixel];
                                else
                                    color = colorRGB[paletteIndex * 4 + pixel];
                                if (!blackIsTransparent || color != 255)
                                {
                                    uint32_t index = tempX + tempY * screenWidth;
                                    screenData[index] = color;
                                }
                            }
                        }
                    }
                }
            } // NO FLIP
        } // else FLIP X
    } // else FLIP X & FLIP Y
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
    if (memoryReadHandler[address].handler != 0)
    {
        return memoryReadHandler[address].handler(address);
    }
    return boardMemory[address];
}

void writeMemoryHandler(int address, int value)
{
    if (memoryWriteHandler[address].handler != 0)
    {
        memoryWriteHandler[address].handler(address, value);
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
bool keyValuePressed[BUTTON_END + 1];

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
void boardMemoryWrite(int address, int value)
{
    boardMemory[address] = value;
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

struct GfxElement *allGfx[2];

uint8_t *dirtyMemoryTiles;
uint8_t *dirtyMemorySprites;

THE_COLOR *colorRGB;
uint16_t paletteColorSize;
THE_COLOR *paletteColor;

struct GfxElement *element;

// uint8_t *tileGfx;
uint16_t tileWidth;
uint16_t tileHeight;
uint16_t tilesCount;
// uint8_t *spriteGfx;
uint16_t spriteWidth;
uint16_t spriteHeight;
uint16_t spritesCount;

THE_COLOR *screenData = NULL;
THE_COLOR *screenDataOld = NULL;
uint32_t screenWidth;
uint32_t screenHeight;
uint32_t screenLength;
uint32_t screenDirtyMinX;
uint32_t screenDirtyMinY;
uint32_t screenDirtyMaxX;
uint32_t screenDirtyMaxY;
