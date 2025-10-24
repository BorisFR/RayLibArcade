#include "TheGame.hpp"

// *******************************************************************

static uint8_t readbit(const uint8_t *src, uint32_t bitnum)
{
    return src[bitnum / 8] & (0x80 >> (bitnum % 8));
}

uint8_t GetBitFromNumber(uint8_t value)
{
    uint8_t res = 0;
    uint8_t temp = value >> 1;
    while (temp > 0)
    {
        res++;
        temp = temp >> 1;
    }
    return res;
}

// *******************************************************************
// *******************************************************************

TheGame::TheGame()
{
    MY_DEBUG2TEXT(TAG, GAME_NAME, "created");
    isReady = false;
    hasColor = false;
    hasPalette = false;
    hasTiles = false;
    hasSprites = false;
    hasGfx = false;
    InitCrc32Table();
    visibleArea = VISIBLE_AREA_FULL;
}

// *******************************************************************

TheGame::~TheGame()
{
    screenDirtyMaxX = 0;
    screenDirtyMinX = 0;
    screenDirtyMaxY = 0;
    screenDirtyMinY = 0;
    hasColor = false;
    hasPalette = false;
    hasTiles = false;
    hasSprites = false;
    hasGfx = false;
    paletteColorSize = 0;
    tilesCount = 0;
    spritesCount = 0;
    screenWidth = 0;
    screenHeight = 0;
    FREE(screenData);
    FREE(dirtybuffer);
    // FREE(screenDataOld);
    FREE(boardMemory);
    FREE(gfxMemory);
    FREE(colorMemory);
    FREE(paletteMemory);
    // FREE(tileMemory);
    // FREE(spriteMemory);
    FREE(soundMemory);
    FREE(colorRGB);
    FREE(paletteColor);
    // if (tileGfx != NULL)
    //     FREE(tileGfx);
    // if (spriteGfx != NULL)
    //     FREE(spriteGfx);
    FREE(memoryReadHandler);
    FREE(memoryWriteHandler);
    for (int16_t i = 0; i < countInportPortReadFunction; i++)
        delete InputPortRead[i];
    for (int16_t i = 0; i < countInportPortWriteFunction; i++)
        delete InputPortWrite[i];
    MY_DEBUG2TEXT(TAG, GAME_NAME, "destroyed");
}

// *******************************************************************

void TheGame::Setup(TheDisplay &display, TheSdCard &sdCard)
{
    this->display = &display;
    MY_DEBUG2TEXT(TAG, GAME_NAME, "setup");
    exitGame = false;
    // froggerWater = display.Rgb888ToRgb565(0x00, 0x00, 0x47);
    if (!Initialize(display, sdCard))
    {
        MY_DEBUG2TEXT(TAG, GAME_NAME, "could not be setup");
        return;
    }
    // ---------------------------------------------------------------
    // Init ports
    // Among other things, they are used for player's input : credit, start, left, fire, ...
    // First init them all to "nothing"
    for (uint8_t k = 0; k < BUTTON_END; k++)
    {
        keyPressed[k] = false;
        keyChanged[k] = false;
        keyPort[k] = BUTTON_END; // if player use buttons which are not use by the game
        keyBit[k] = 0;
        keyValuePressed[k] = 0;
    }
    // ---------------------------------------------------------------
    visibleArea = allGames[currentGame].video.visibleArea;
    // ---------------------------------------------------------------
    // then parse all the game's ports
    // INPUT_PORTS_START( gamename_input_ports )
    uint8_t i = 0;
    bool finish = false;
    int8_t portNumber = -1;
    uint8_t portValue = 0;
    while (!finish)
    {
        //MY_DEBUG2(TAG, "portValue=", portValue)
        switch (allGames[currentGame].machine.inputPorts[i].type)
        {
        case IPT_END:
            if (portNumber >= 0)
                machineInputPort.InputPortSet(portNumber, portValue);
            finish = true;
            //MY_DEBUG2(TAG, "==> Port ", portNumber)
            //MY_DEBUG2(TAG, "    with default value ", portValue)
            continue;
            break;
        case IPT_PORT:
            if (portNumber >= 0)
            {
                machineInputPort.InputPortSet(portNumber, portValue);
                //MY_DEBUG2(TAG, "=> Port ", portNumber)
                //MY_DEBUG2(TAG, "    with default value ", portValue)
            }
            portNumber++;
            portValue = 0;
            break;
        case IPT_COIN1:
            PORT_BIT_PARAM("*** BUTTON_CREDIT", BUTTON_CREDIT)
            break;
        case IPT_COIN2:
            PORT_BIT_PARAM("BUTTON_COIN2", BUTTON_COIN2)
            break;
        case IPT_COIN3:
            PORT_BIT_PARAM("BUTTON_COIN3", BUTTON_COIN3)
            break;
        case IPT_START1:
            PORT_BIT_PARAM("BUTTON_START", BUTTON_START)
            break;
        case IPT_START2:
            PORT_BIT_VALUE
            break;
        case IPT_JOYSTICK_LEFT:
            PORT_BIT_PARAM("*** BUTTON_LEFT", BUTTON_LEFT)
            break;
        case IPT_JOYSTICK_LEFT | IPF_PLAYER2:
            PORT_BIT_PARAM("BUTTON_LEFT for player 2", BUTTON_LEFT_P2)
            break;
        case IPT_JOYSTICK_RIGHT:
            PORT_BIT_PARAM("*** BUTTON_RIGHT", BUTTON_RIGHT)
            break;
        case IPT_JOYSTICK_RIGHT | IPF_PLAYER2:
            PORT_BIT_PARAM("BUTTON_RIGHT for player 2", BUTTON_RIGHT_P2)
            break;
        case IPT_JOYSTICK_UP:
            PORT_BIT_PARAM("*** BUTTON_UP", BUTTON_UP)
            break;
        case IPT_JOYSTICK_UP | IPF_PLAYER2:
            PORT_BIT_PARAM("BUTTON_UP for player 2", BUTTON_UP_P2)
            break;
        case IPT_JOYSTICK_DOWN:
            PORT_BIT_PARAM("*** BUTTON_DOWN", BUTTON_DOWN)
            break;
        case IPT_JOYSTICK_DOWN | IPF_PLAYER2:
            PORT_BIT_PARAM("BUTTON_DOWN for player 2", BUTTON_DOWN_P2)
            break;
        case IPT_BUTTON1:
            PORT_BIT_PARAM("*** BUTTON_FIRE", BUTTON_FIRE)
            break;
        case IPT_UNKNOWN:
            PORT_BIT_VALUE
            break;
        case IPT_TILT:
            PORT_BIT_VALUE
            break;
        case IPT_DIPSWITCH_NAME:
            //MY_DEBUG2TEXT(TAG, "IPT_DIPSWITCH_NAME", allGames[currentGame].machine.inputPorts[i].name)
            PORT_SWITCH_DEFAULT_VALUE(allGames[currentGame].machine.inputPorts[i].default_value)
            PORT_SWITCH_VALUE
            break;
        case IPT_DIPSWITCH_SETTING:
            //MY_DEBUG2TEXT(TAG, "IPT_DIPSWITCH_SETTING", allGames[currentGame].machine.inputPorts[i].name)
            PORT_SWITCH_VALUE
            break;
        default:
            PORT_BIT_VALUE
            MY_DEBUG2(TAG, "BUTTON ???", GetBitFromNumber(allGames[currentGame].machine.inputPorts[i].mask))
            break;
        }
        i++;
    }
    // ---------------------------------------------------------------
    //
    // parse static struct IOReadPort gamename_readport[]
    //
    finish = false;
    for (uint8_t p = 0; p < BUTTON_END + 1; p++)
    {
        InputPortRead[p] = new ReadHandler();
        InputPortRead[p]->handler = readPort;
    }
    countInportPortReadFunction = 0;
    if (allGames[currentGame].machine.readPorts != NULL)
        while (!finish)
        {
            if (allGames[currentGame].machine.readPorts[countInportPortReadFunction].start == -1)
            {
                finish = true;
                continue;
            }
            // MY_DEBUG2(TAG, "IOread Start:", allGames[currentGame].readPorts[countInportPortReadFunction].start)
            // MY_DEBUG2(TAG, "IOread End:", allGames[currentGame].readPorts[countInportPortReadFunction].end)
            for (uint8_t p = allGames[currentGame].machine.readPorts[countInportPortReadFunction].start; p <= allGames[currentGame].machine.readPorts[countInportPortReadFunction].end; p++)
            {
                InputPortRead[p] = new ReadHandler();
                InputPortRead[p]->handler = allGames[currentGame].machine.readPorts[countInportPortReadFunction].handler;
            }
            countInportPortReadFunction++;
        }
    // ---------------------------------------------------------------
    //
    // parse static struct IOWritePort gamename_readport[]
    //
    finish = false;
    for (uint8_t p = 0; p < BUTTON_END + 1; p++)
    {
        InputPortWrite[p] = new WriteHandler();
        InputPortWrite[p]->handler = writePort;
    }
    countInportPortWriteFunction = 0;
    if (allGames[currentGame].machine.writePorts != NULL)
        while (!finish)
        {
            if (allGames[currentGame].machine.writePorts[countInportPortWriteFunction].start == -1)
            {
                finish = true;
                continue;
            }
            // MY_DEBUG2(TAG, "IOwrite Start:", allGames[currentGame].writePorts[countInportPortWriteFunction].start)
            // MY_DEBUG2(TAG, "IOwrite End:", allGames[currentGame].writePorts[countInportPortWriteFunction].end)
            for (uint8_t p = allGames[currentGame].machine.writePorts[countInportPortWriteFunction].start; p <= allGames[currentGame].machine.writePorts[countInportPortWriteFunction].end; p++)
            {
                InputPortWrite[p] = new WriteHandler();
                InputPortWrite[p]->handler = allGames[currentGame].machine.writePorts[countInportPortWriteFunction].handler;
            }
            countInportPortWriteFunction++;
        }
    // ---------------------------------------------------------------
    //
    // parse static struct MemoryReadAddress gamename_readmem[]
    //
    finish = false;
    countMemoryReadFunction = 0;
    memoryReadHandler = (ReadHandler *)malloc(sizeof(ReadHandler) * boardMemorySize);
    if (memoryReadHandler == NULL)
    {
        MY_DEBUG(TAG, "Error allocating read memory handler");
        return;
    }
    memset(memoryReadHandler, 0, sizeof(ReadHandler) * boardMemorySize);
    for (uint32_t m = 0; m < boardMemorySize; m++)
    {
        memoryReadHandler[m].handler = boardMemoryRead;
    }
    while (!finish)
    {
        if (allGames[currentGame].machine.readAddress[countMemoryReadFunction].start == -1)
        {
            finish = true;
            continue;
        }
        if (allGames[currentGame].machine.readAddress[countMemoryReadFunction].size)
            *allGames[currentGame].machine.readAddress[countMemoryReadFunction].size = allGames[currentGame].machine.readAddress[countMemoryReadFunction].end - allGames[currentGame].machine.readAddress[countMemoryReadFunction].start;

        // MY_DEBUG2(TAG, "MemoryRead Start:", allGames[currentGame].readAddress[countMemoryReadFunction].start)
        // MY_DEBUG2(TAG, "MemoryRead End:", allGames[currentGame].readAddress[countMemoryReadFunction].end)
        for (uint32_t p = allGames[currentGame].machine.readAddress[countMemoryReadFunction].start; p <= allGames[currentGame].machine.readAddress[countMemoryReadFunction].end; p++)
        {
            memoryReadHandler[p].handler = allGames[currentGame].machine.readAddress[countMemoryReadFunction].handler;
        }
        countMemoryReadFunction++;
    }
    // ---------------------------------------------------------------
    //
    // parse static struct MemoryWriteAddress gamename_writemem[]
    //
    finish = false;
    countMemoryWriteFunction = 0;
    memoryWriteHandler = (WriteHandler *)malloc(sizeof(WriteHandler) * boardMemorySize);
    if (memoryWriteHandler == NULL)
    {
        MY_DEBUG(TAG, "Error allocating write memory handler");
        return;
    }
    memset(memoryWriteHandler, 0, sizeof(WriteHandler) * boardMemorySize);
    while (!finish)
    {
        if (allGames[currentGame].machine.writeAddress[countMemoryWriteFunction].start == -1)
        {
            finish = true;
            continue;
        }
        // point a user variable here
        if (allGames[currentGame].machine.writeAddress[countMemoryWriteFunction].base != NULL)
        {
            *allGames[currentGame].machine.writeAddress[countMemoryWriteFunction].base = &boardMemory[allGames[currentGame].machine.writeAddress[countMemoryWriteFunction].start];
        }
        // calculate the memory size of this area
        if (allGames[currentGame].machine.writeAddress[countMemoryWriteFunction].size != NULL)
        {
            *allGames[currentGame].machine.writeAddress[countMemoryWriteFunction].size = allGames[currentGame].machine.writeAddress[countMemoryWriteFunction].end - allGames[currentGame].machine.writeAddress[countMemoryWriteFunction].start;
        }
        // create the handler for the full area
        for (uint32_t p = allGames[currentGame].machine.writeAddress[countMemoryWriteFunction].start; p <= allGames[currentGame].machine.writeAddress[countMemoryWriteFunction].end; p++)
        {
            memoryWriteHandler[p].handler = allGames[currentGame].machine.writeAddress[countMemoryWriteFunction].handler;
            // if user put a variable, we adapt the offset
            if (allGames[currentGame].machine.writeAddress[countMemoryWriteFunction].base != NULL)
                memoryWriteHandler[p].toZero = allGames[currentGame].machine.writeAddress[countMemoryWriteFunction].start;
            else
                memoryWriteHandler[p].toZero = 0;
        }
        countMemoryWriteFunction++;
    }
    // uint8_t buttons[] = {BUTTON_RIGHT, BUTTON_LEFT, BUTTON_DOWN, BUTTON_UP};
    // for (uint8_t k = 0; k < 4; k++)
    //{
    //     keyPressed[buttons[k]] = true;
    //     KeyChange(buttons[k]);
    //     keyPressed[buttons[k]] = false;
    //     KeyChange(buttons[k]);
    // }
    isReady = true;
}

// *******************************************************************

bool TheGame::IsReady() { return isReady; }

// *******************************************************************

void TheGame::Loop(TheDisplay &display) {}

// *******************************************************************

void TheGame::InitCrc32Table()
{
    const uint32_t polynomial = 0xEDB88320;
    for (uint16_t i = 0; i < 256; ++i)
    {
        uint32_t crc = i;
        for (uint16_t j = 0; j < 8; ++j)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ polynomial;
            else
                crc >>= 1;
        }
        crc32_table[i] = crc;
    }
}

// *******************************************************************

uint32_t TheGame::GetCrc32(uint16_t offset, uint16_t length, uint8_t *fromMemory)
{
    uint32_t crc = 0xFFFFFFFF;
    for (uint16_t i = 0; i < length; ++i)
    {
        uint8_t index = (crc ^ fromMemory[i + offset]) & 0xFF;
        crc = (crc >> 8) ^ crc32_table[index];
    }
    return crc ^ 0xFFFFFFFF;
}

// *******************************************************************

bool TheGame::Initialize(TheDisplay &display, TheSdCard &sdCard)
{
    MY_DEBUG2TEXT(TAG, "Loading all ROMs from folder:", GAME_FOLDER)
    screenWidth = GAME_SCREEN_WIDTH;
    screenHeight = GAME_SCREEN_HEIGHT;
    screenDirtyMinX = screenWidth;
    screenDirtyMinY = screenHeight;
    screenDirtyMaxX = 0;
    screenDirtyMaxY = 0;
    screenLength = screenWidth * screenHeight * sizeof(THE_COLOR);
    screenData = (THE_COLOR *)malloc(screenLength);
    if (screenData == NULL)
    {
        MY_DEBUG(TAG, "Error allocating screen memory");
        return false;
    }
    memset(screenData, 0, screenLength);
    dirtybuffer = (uint8_t *)malloc(screenLength);
    if (dirtybuffer == NULL)
    {
        MY_DEBUG(TAG, "Error allocating dirtybuffer memory");
        return false;
    }
    memset(dirtybuffer, DIRTY_TRANSPARENT, screenLength);
    // screenDataOld = (THE_COLOR *)malloc(screenLength);
    // if (screenDataOld == NULL)
    // {
    //     MY_DEBUG(TAG, "Error allocating old screen memory");
    //     return false;
    // }
    // memset(screenDataOld, 0, screenLength);
    screenBitmap = (THE_COLOR *)malloc(screenLength);
    if (screenBitmap == NULL)
    {
        MY_DEBUG(TAG, "Error allocating bitmap screen memory");
        return false;
    }
    memset(screenBitmap, 0, screenLength);
    uint8_t *toMemory = NULL;
    uint8_t i = 0;
    bool finish = false;
    while (!finish)
    {
        if (GAME_ROMS[i].length == 0 && GAME_ROMS[i].offset == 0)
        {
            finish = true;
            continue;
        }
        if (GAME_ROMS[i].name == NULL)
        {
            switch (GAME_ROMS[i].crc)
            {
            case ROM_CPU:
                MY_DEBUG(TAG, "** Loading to CPU memory");
                boardMemorySize = GAME_ROMS[i].offset;
                boardMemory = (uint8_t *)malloc(boardMemorySize * sizeof(uint8_t));
                if (boardMemory == NULL)
                {
                    MY_DEBUG(TAG, "Error allocating board memory");
                    return false;
                }
                memset(boardMemory, 0, boardMemorySize);
                // romptr[0] = boardMemory;
                toMemory = boardMemory;
                break;
            case ROM_GFX:
                MY_DEBUG(TAG, "** Loading to GFX memory");
                gfxMemorySize = GAME_ROMS[i].offset;
                gfxMemory = (uint8_t *)malloc(gfxMemorySize * sizeof(uint8_t));
                if (gfxMemory == NULL)
                {
                    MY_DEBUG(TAG, "Error allocating gfx memory");
                    return false;
                }
                hasGfx = true;
                toMemory = gfxMemory;
                break;
            case ROM_COLOR:
                MY_DEBUG(TAG, "** Loading to COLOR memory");
                colorMemorySize = GAME_ROMS[i].offset;
                colorMemory = (uint8_t *)malloc(colorMemorySize * sizeof(uint8_t));
                if (colorMemory == NULL)
                {
                    MY_DEBUG(TAG, "Error allocating color memory");
                    return false;
                }
                hasColor = true;
                toMemory = colorMemory;
                break;
            case ROM_PALETTE:
                paletteMemorySize = GAME_ROMS[i].offset;
                paletteMemory = (uint8_t *)malloc(paletteMemorySize * sizeof(uint8_t));
                if (paletteMemory == NULL)
                {
                    MY_DEBUG(TAG, "Error allocating palette memory");
                    return false;
                }
                MY_DEBUG2(TAG, "** Loading to PALETTE memory: size", paletteMemorySize);
                hasPalette = true;
                toMemory = paletteMemory;
                break;
                /*
                            case ROM_TILE:
                                tileMemorySize = GAME_ROMS[i].offset;
                                MY_DEBUG(TAG, "Loading to TILE memory");
                                tileMemory = (uint8_t *)malloc(tileMemorySize * sizeof(uint8_t));
                                if (tileMemory == NULL)
                                {
                                    MY_DEBUG(TAG, "Error allocating tile memory");
                                    return false;
                                }
                                hasTiles = true;
                                toMemory = tileMemory;
                                break;
                            case ROM_SPRITE:
                                spriteMemorySize = GAME_ROMS[i].offset;
                                MY_DEBUG(TAG, "Loading to SPRITE memory");
                                spriteMemory = (uint8_t *)malloc(spriteMemorySize * sizeof(uint8_t));
                                if (spriteMemory == NULL)
                                {
                                    MY_DEBUG(TAG, "Error allocating sprite memory");
                                    return false;
                                }
                                hasSprites = true;
                                toMemory = spriteMemory;
                                break;
                */
            case ROM_SOUND:
                MY_DEBUG(TAG, "** Loading to SOUND memory");
                soundMemorySize = GAME_ROMS[i].offset;
                soundMemory = (uint8_t *)malloc(soundMemorySize * sizeof(uint8_t));
                if (soundMemory == NULL)
                {
                    MY_DEBUG(TAG, "Error allocating sound memory");
                    return false;
                }
                toMemory = soundMemory;
                break;
            default:
                MY_DEBUG(TAG, "** Loading to UNKNOW memory type!!");
                return false;
            }
            i++;
            continue;
        }
        std::string name = std::string(GAME_ROMS[i].name);
        // MY_DEBUG(TAG, "Number " + std::to_string(i) + " => " + name + " : offset " + std::to_string(GAME_ROMS[i].offset) + " - length " + std::to_string(GAME_ROMS[i].length));
        if (toMemory == NULL)
        {
            MY_DEBUG(TAG, "toMemory not initialized");
            return false;
        }
        if (!LoadOneRom(sdCard, GAME_FOLDER, name, toMemory, GAME_ROMS[i].length, GAME_ROMS[i].offset, GAME_ROMS[i].crc))
        {
            MY_DEBUG(TAG, "Error loading roms, exit");
            return false;
        }
        i++;
    } // while
    if (i == 0)
    {
        MY_DEBUG(TAG, "No ROM found");
        if (currentGame == GAME_NUMBER_IS_MENU)
            return true;
        return false;
    }
    // if we need to decode encrypt rom
    if (allGames[currentGame].machine.decode != NOTHING)
    {
        MY_DEBUG2TEXT(TAG, GAME_NAME, "Decoding roms");
        allGames[currentGame].machine.decode();
    }
    if (hasColor)
    {
        MY_DEBUG(TAG, "Decoding colors...");
        if (!DecodeColors(display))
        {
            MY_DEBUG(TAG, "Error decoding colors");
            return false;
        }
        if (hasPalette)
        {
            MY_DEBUG(TAG, "Decoding palette...");
            if (!DecodePalette())
            {
                MY_DEBUG(TAG, "Error decoding palette");
                return false;
            }
        }
    }
    else
    {
        if (hasPalette)
        {
            MY_DEBUG(TAG, "Could not decode palette, missing colors!");
            return false;
        }
    }
    if (hasGfx)
    {
        DecodeAllGfx(allGames[currentGame].video.decodeInfo);
        if (hasColor && !hasPalette)
        {
            // generate a palette
            element = allGfx[0];
            GeneratePalette(display);
        }
    }
    if (allGames[currentGame].machine.gameInit)
        allGames[currentGame].machine.gameInit();
    return true;
}

// *******************************************************************

bool TheGame::LoadOneRom(TheSdCard &sdCard, std::string folder, std::string filename, uint8_t *toMemory, uint16_t size, uint16_t offset, uint32_t expectedCrc)
{
    std::string fullPath = folder + "/" + filename;
    MY_DEBUG2TEXT(TAG, "Full ROM path:", fullPath.c_str());
    if (toMemory == NULL)
    {
        MY_DEBUG(TAG, "Memory not initialized");
        return false;
    }
    if (!sdCard.LoadFile(fullPath, toMemory, size, offset))
    {
        return false;
    }
    if (expectedCrc != 0 && expectedCrc != GetCrc32(offset, size, toMemory))
    {
        MY_DEBUG(TAG, "ROM CRC mismatch");
        return false;
    }
    // ESP_LOGI(TAG, "Loaded ROM: " + filename + " at offset: " + std::string(offset) + ", size: " + std::string(size) + ", crc: " + std::string(expectedCrc));
    return true;
}

// *******************************************************************

bool TheGame::DecodeColors(TheDisplay &display)
{
    MY_DEBUG2(TAG, "Color size:", colorMemorySize);
    colorRGB = (THE_COLOR *)malloc(colorMemorySize * sizeof(THE_COLOR));
    if (colorRGB == NULL)
    {
        MY_DEBUG(TAG, "Error allocating color buffers");
        return false;
    }
    for (uint32_t c = 0; c < colorMemorySize; c++)
    {
        uint8_t colorByte = colorMemory[c];
        uint8_t red = (uint8_t)((((colorByte >> 0U) & 0b1) * 0x21) + (((colorByte >> 1U) & 0b1) * 0x47) + (((colorByte >> 2U) & 0b1) * 0x97));
        uint8_t green = (uint8_t)((((colorByte >> 3U) & 0b1) * 0x21) + (((colorByte >> 4U) & 0b1) * 0x47) + (((colorByte >> 5U) & 0b1) * 0x97));
        uint8_t blue = (uint8_t)((((colorByte >> 6U) & 0b1) * 0x51) + (((colorByte >> 7U) & 0b1) * 0xAE));
        colorRGB[c] = display.Rgb888ToRgb565(red, green, blue);
        // std::string t = std::to_string(red) + "/" + std::to_string(green) + "/" + std::to_string(blue) + " => " + std::to_string(colorRGB[c]);
        // MY_DEBUG(TAG, t.c_str())
    }
    return true;
}

// *******************************************************************

bool TheGame::GeneratePalette(TheDisplay &display)
{
    // paletteColorSize = element->total_colors * element->color_granularity;
    paletteColorSize = colorMemorySize;
    MY_DEBUG2(TAG, "Generate palette:", paletteColorSize)
    paletteColor = (THE_COLOR *)malloc(paletteColorSize * sizeof(THE_COLOR));
    if (paletteColor == NULL)
    {
        MY_DEBUG(TAG, "Failed to allocate palette color memory to decode");
        paletteColorSize = 0;
        return false;
    }
    memset(paletteColor, 0, paletteColorSize);
    // uint8_t indexColor = 0;
    for (uint32_t i = 0; i < colorMemorySize; i++)
    {
        paletteColor[i] = colorRGB[i];
    }
    hasPalette = true;
    return true;
}

bool TheGame::DecodePalette()
{
    if (paletteMemory == NULL)
    {
        MY_DEBUG(TAG, "Palette memory not initialized");
        return false;
    }
    paletteColor = (THE_COLOR *)malloc(paletteMemorySize * sizeof(THE_COLOR));
    if (paletteColor == NULL)
    {
        MY_DEBUG(TAG, "Failed to allocate palette color memory to decode");
        return false;
    }
    memset(paletteColor, 0, paletteMemorySize);
    paletteColorSize = paletteMemorySize;
    for (uint32_t p = 0; p < paletteMemorySize; p++)
    {
        paletteColor[p] = colorRGB[paletteMemory[p]];
        // if (paletteColor[p] != 255)
        //     paletteColorSize = p + 1;
    }
    MY_DEBUG2(TAG, "Created palette color memory of size:", paletteColorSize);
    return true;
}

/***************************************************************************

  Function to convert the information stored in the graphic roms into a
  more usable format.

  Back in the early '80s, arcade machines didn't have the memory or the
  speed to handle bitmaps like we do today. They used "character maps",
  instead: they had one or more sets of characters (usually 8x8 pixels),
  which would be placed on the screen in order to form a picture. This was
  very fast: updating a character mapped display is, rougly speaking, 64
  times faster than updating an equivalent bitmap display, since you only
  modify groups of 8x8 pixels and not the single pixels. However, it was
  also much less versatile than a bitmap screen, since with only 256
  characters you had to do all of your graphics. To overcome this
  limitation, some special hardware graphics were used: "sprites". A sprite
  is essentially a bitmap, usually larger than a character, which can be
  placed anywhere on the screen (not limited to character boundaries) by
  just telling the hardware the coordinates. Moreover, sprites can be
  flipped along the major axis just by setting the appropriate bit (some
  machines can flip characters as well). This saves precious memory, since
  you need only one copy of the image instead of four.

  What about colors? Well, the early machines had a limited palette (let's
  say 16-32 colors) and each character or sprite could not use all of them
  at the same time. Characters and sprites data would use a limited amount
  of bits per pixel (typically 2, which allowed them to address only four
  different colors). You then needed some way to tell to the hardware which,
  among the available colors, were the four colors. This was done using a
  "color attribute", which typically was an index for a lookup table.

  OK, after this brief and incomplete introduction, let's come to the
  purpose of this section: how to interpret the data which is stored in
  the graphic roms. Unfortunately, there is no easy answer: it depends on
  the hardware. The easiest way to find how data is encoded, is to start by
  making a bit by bit dump of the rom. You will usually be able to
  immediately recognize some pattern: if you are lucky, you will see
  letters and numbers right away, otherwise you will see something which
  looks like letters and numbers, but with halves switched, dilated, or
  something like that. You'll then have to find a way to put it all
  together to obtain our standard one byte per pixel representation. Two
  things to remember:
  - keep in mind that every pixel has typically two (or more) bits
    associated with it, and they are not necessarily near to each other.
  - characters might be rotated 90 degrees. That's because many games used a
    tube rotated 90 degrees. Think how your monitor would look like if you
    put it on its side ;-)

  After you have successfully decoded the characters, you have to decode
  the sprites. This is usually more difficult, because sprites are larger,
  maybe have more colors, and are more difficult to recognize when they are
  messed up, since they are pure graphics without letters and numbers.
  However, with some work you'll hopefully be able to work them out as
  well. As a rule of thumb, the sprites should be encoded in a way not too
  dissimilar from the characters.

***************************************************************************/

void TheGame::DecodeElement(struct GfxElement *element, uint16_t index, const uint8_t *fromMemory, uint32_t delta, const struct GfxLayout *gfxLayout)
{
    int planebit = 1 << (gfxLayout->planes - 1);
    for (uint8_t plane = 0; plane < gfxLayout->planes; plane++)
    {
        uint32_t offset = index * gfxLayout->charincrement + gfxLayout->planeoffset[plane];
        for (uint32_t y = 0; y < element->height; y++)
        {
            // int yoffs = offset + gfxLayout->yoffset[y];
            uint8_t *pointerLine = element->gfxdata->line[index * element->height + y];
            for (uint32_t x = 0; x < element->width; x++)
            {
                int xoffs = x;
                int yoffs = y;
                if (GAME_ORIENTATION & ORIENTATION_FLIP_X)
                    xoffs = element->width - 1 - xoffs;
                if (GAME_ORIENTATION & ORIENTATION_FLIP_Y)
                    yoffs = element->height - 1 - yoffs;
                if (GAME_ORIENTATION & ORIENTATION_SWAP_XY)
                {
                    int temp;
                    temp = xoffs;
                    xoffs = yoffs;
                    yoffs = temp;
                }
                int coordinateInRomX = gfxLayout->xoffset[xoffs];
                int coordinateInRomY = gfxLayout->yoffset[yoffs];
                int positionInRom = offset + coordinateInRomX + coordinateInRomY;
                int result = fromMemory[delta + positionInRom / 8] & (0x80 >> (positionInRom % 8));
                if (result)
                {
                    pointerLine[x] |= planebit;
                }
                // if (index == 1)
                // {
                //     std::string t = std::to_string(x) + "x" + std::to_string(y) + "=" + std::to_string(pointerLine[x]);
                //     MY_DEBUG2TEXT(TAG, "change", t.c_str())
                // }
            }
        }
        planebit >>= 1;
    }
    if (element->pen_usage)
    {
        /* fill the pen_usage array with info on the used pens */
        element->pen_usage[index] = 0;
        for (uint32_t y = 0; y < element->height; y++)
        {
            uint8_t *pointerLine = element->gfxdata->line[index * element->height + y];
            for (uint32_t x = 0; x < element->width; x++)
            {
                element->pen_usage[index] |= 1 << pointerLine[x];
            }
        }
    }
}

// #define osd_create_bitmap(w, h) osd_new_bitmap((w), (h), 8) /* ASG 980209 */

GfxElement *TheGame::DecodeGfxElement(const uint8_t *fromMemory, uint32_t offset, const GfxLayout *gfxLayout)
{
    std::string t = std::to_string(gfxLayout->width) + "x" + std::to_string(gfxLayout->height);
    MY_DEBUG2TEXT(TAG, "Dimensions:", t.c_str())
    struct GfxElement *element = (struct GfxElement *)malloc(sizeof(struct GfxElement));
    if (element == 0)
        return 0;
    struct osd_bitmap *bitmap;
    if (GAME_ORIENTATION & ORIENTATION_SWAP_XY)
    {
        element->width = gfxLayout->height;
        element->height = gfxLayout->width;
        bitmap = osd_new_bitmap(gfxLayout->total * element->height, element->width, 16);
    }
    else
    {
        element->width = gfxLayout->width;
        element->height = gfxLayout->height;
        bitmap = osd_new_bitmap(element->width, gfxLayout->total * element->height, 8);
        if (bitmap == 0)
        {
            FREE(element);
            return 0;
        }
    }
    element->total_elements = gfxLayout->total;
    element->color_granularity = 1 << gfxLayout->planes;
    element->gfxdata = bitmap;
    element->pen_usage = 0;               /* need to make sure this is NULL if the next test fails) */
    if (element->color_granularity <= 32) /* can't handle more than 32 pens */
        element->pen_usage = (unsigned int *)malloc(element->total_elements * sizeof(int));
    /* no need to check for failure, the code can work without pen_usage */
    MY_DEBUG2(TAG, "color_granularity ", element->color_granularity)
    for (uint32_t index = 0; index < gfxLayout->total; index++)
    {
        // MY_DEBUG2(TAG, "GFX LAYOUT ", index)
        DecodeElement(element, index, fromMemory, offset, gfxLayout);
    }
    return element;
}

osd_bitmap *TheGame::osd_new_bitmap(int width, int height, int depth) // ASG 980209
{
    struct osd_bitmap *bitmap;
    MY_DEBUG2(TAG, "osd_new_bitmap. x:", width)
    MY_DEBUG2(TAG, "osd_new_bitmap. y:", height)
    if (GAME_ORIENTATION & ORIENTATION_SWAP_XY)
    {
        int temp;
        temp = width;
        width = height;
        height = temp;
    }
    bitmap = (osd_bitmap *)malloc(sizeof(struct osd_bitmap));
    if (bitmap != 0)
    {
        int i, rowlen, rdwidth;
        int safety;
        if (width > 32)
            safety = 8;
        else
            safety = 0; /* don't create the safety area for GfxElement bitmaps */
        if (depth != 8 && depth != 16)
            depth = 16;
        // depth = SCREEN_DEPTH;
        bitmap->depth = depth;
        bitmap->width = width;
        bitmap->height = height;
        bitmap->line = NULL;
        // bitmap->_private = NULL;
        rdwidth = (width + 7) & ~7; /* round width to a quadword */
        if (depth == 16)
            rowlen = 2 * (rdwidth + 2 * safety) * sizeof(uint8_t);
        else
            rowlen = (rdwidth + 2 * safety) * sizeof(uint8_t);
        uint8_t *bitmapLines = (uint8_t *)malloc((height + 2 * safety) * rowlen);
        if (bitmapLines == 0)
        {
            FREE(bitmap);
            return 0;
        }
        bitmap->line = (uint8_t **)malloc(height * sizeof(uint8_t *));
        if (bitmap->line == 0)
        {
            FREE(bitmapLines);
            FREE(bitmap);
            return 0;
        }
        for (i = 0; i < height; i++)
            bitmap->line[i] = &bitmapLines[(i + safety) * rowlen + safety];
        // bitmap->_private = bm;
        osd_clearbitmap(bitmap);
    }

    return bitmap;
}

void TheGame::osd_clearbitmap(osd_bitmap *bitmap)
{
    // if (bitmap != scrbitmap)
    {
        for (uint32_t i = 0; i < bitmap->height; i++)
        {
            if (bitmap->depth == 16)
                memset(bitmap->line[i], 0, 2 * bitmap->width);
            else
                memset(bitmap->line[i], 0, bitmap->width);
        }
    }
}

// *******************************************************************

bool TheGame::DecodeAllGfx(const GfxDecodeInfo info[])
{
    countGfxElement = 0;
    for (uint8_t i = 0; i < MAX_GFX_ELEMENTS; i++)
    {
        if (info[i].memory_region == -1)
            break;
        MY_DEBUG2(TAG, "GFX ELEMENT ", i)
        countGfxElement++;
        switch (info[i].memory_region)
        {
        case ROM_GFX:
            allGfx[i] = DecodeGfxElement(gfxMemory, info[i].start, info[i].gfxlayout);
            allGfx[i]->total_colors = info[i].total_color_codes;
            MY_DEBUG2(TAG, "total_colors ", allGfx[i]->total_colors)
            break;
        default:
            MY_DEBUG2(TAG, "*** GfxDecodeInfo, bad rom index:", info[i].memory_region)
        }
    }
    return true;
}

// *******************************************************************

void TheGame::KeyChange(uint8_t button)
{
    // MY_DEBUG2(TAG, "*** KEY port", keyPort[button])
    // MY_DEBUG2(TAG, "*** KEY before", InputPorts[keyPort[button]])
    if (keyPressed[button])
    {
        // MY_DEBUG2(TAG, "pressed", button)
        machineInputPort.InputPortUpdate(keyPort[button], keyBit[button], 0xff - keyValuePressed[button]);
    }
    else
    {
        // MY_DEBUG2(TAG, "released", button)
        machineInputPort.InputPortUpdate(keyPort[button], keyBit[button], keyValuePressed[button]);
    }
    // MY_DEBUG2(TAG, "KEY after", InputPorts[keyPort[button]])
}

void TheGame::Exit(uint8_t next)
{
    exitGame = true;
    nextGame = next;
}
