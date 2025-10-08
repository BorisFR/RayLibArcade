#include "GameFrogger.hpp"

// ***********************************************************

GameFrogger::GameFrogger() { MY_DEBUG(GAME_FOLDER, "created"); }

// ***********************************************************

GameFrogger::~GameFrogger()
{
    MY_DEBUG(GAME_FOLDER, "destroyed");
    if (dirtyMemoryTiles != NULL)
        free(dirtyMemoryTiles);
    if (dirtyMemorySprites != NULL)
        free(dirtyMemorySprites);
}

// ***********************************************************

void GameFrogger::Setup(TheDisplay &display, TheSdCard &sdCard)
{
    MY_DEBUG(GAME_FOLDER, "setup");
    dirtyMemoryTiles = (uint8_t *)malloc(0x1000 * sizeof(uint8_t));
    if (dirtyMemoryTiles == NULL)
    {
        MY_DEBUG(GAME_FOLDER, "problem allocating dirty memory tiles");
        return;
    }
    memset(dirtyMemoryTiles, 0, 0x1000);
    dirtyMemorySprites = (uint8_t *)malloc(FROGGER_SPRITE_COUNT * FROGGER_SPRITE_INFO * sizeof(uint8_t));
    if (dirtyMemorySprites == NULL)
    {
        MY_DEBUG(GAME_FOLDER, "problem allocating dirty memory sprites");
        return;
    }
    memset(dirtyMemorySprites, 0, FROGGER_SPRITE_COUNT * FROGGER_SPRITE_INFO);
    //if (!Initialize(display, sdCard))
    //{
    //    return;
    //}
    // need to decode some rom
    // the first ROM of the second CPU has data lines D0 and D1 swapped. Decode it.
    for (uint32_t address = 0; address < 0x0800; address++)
        soundMemory[address] = (soundMemory[address] & 0xfc) | ((soundMemory[address] & 1) << 1) | ((soundMemory[address] & 2) >> 1);
    // likewise, the first gfx ROM has data lines D0 and D1 swapped. Decode it.
    for (uint32_t address = 0x1000; address < 0x1800; address++)
        gfxMemory[address] = (gfxMemory[address] & 0xfc) | ((gfxMemory[address] & 1) << 1) | ((gfxMemory[address] & 2) >> 1);

    struct GfxDecodeInfo gfxDecodeInfo[] =
        {
            {1, 0x0000, &tileLayout, 0, 16},
            {1, 0x0000, &spriteLayout, 0, 8},
            {-1} /* end of array */
        };
    //DecodeAllGfx(gfxDecodeInfo);
    // PrepareTiles(tileLayout, 0, 16);
    // PrepareSprites(spriteLayout, 0, 8);
    //PrepareFroggerSprites(spriteLayout, 0, FROGGER_SPRITE_COUNT);
    /*
    PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP | IPF_4WAY | IPF_COCKTAIL )
    PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN ) // 1P shoot2 - unused
    PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN3 )
    PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN ) // 1P shoot1 - unused
    PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_4WAY )
    PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT | IPF_4WAY )
    PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN2 )
    PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN1 )
     * IN0 (all bits are inverted)
     * bit 7 : COIN 1
     * bit 6 : COIN 2
     * bit 5 : LEFT player 1
     * bit 4 : RIGHT player 1
     * bit 3 : SHOOT 1 player 1
     * bit 2 : CREDIT
     * bit 1 : SHOOT 2 player 1
     * bit 0 : UP player 2 (TABLE only)
     */
    machineInputPort.InputPortSet(0, machineInputPort.InputPortGetValueForBits(HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH));
    /*
    PORT_DIPNAME( 0x03, 0x00, "Lives", IP_KEY_NONE )
    PORT_DIPSETTING(    0x00, "3" )
    PORT_DIPSETTING(    0x01, "5" )
    PORT_DIPSETTING(    0x02, "7" )
    PORT_BITX( 0,       0x03, IPT_DIPSWITCH_SETTING | IPF_CHEAT, "256", IP_KEY_NONE, IP_JOY_NONE, 0 )
    PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN ) // 2P shoot2 - unused
    PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN ) // 2P shoot1 - unused
    PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_4WAY | IPF_COCKTAIL )
    PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT | IPF_4WAY | IPF_COCKTAIL )
    PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START2 )
    PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 )
     * IN1 (all bits are inverted)
     * bit 7 : START 1
     * bit 6 : START 2
     * bit 5 : LEFT player 2 (TABLE only)
     * bit 4 : RIGHT player 2 (TABLE only)
     * bit 3 : SHOOT 1 player 2 (TABLE only)
     * bit 2 : SHOOT 2 player 2 (TABLE only)
     * bit 1 :\ nr of lives
     * bit 0 :/ 00 = 3  01 = 5  10 = 7  11 = 256
     */
    machineInputPort.InputPortSet(1, machineInputPort.InputPortGetValueForBits(HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH));
    /*
    PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN | IPF_4WAY | IPF_COCKTAIL )
    PORT_DIPNAME( 0x06, 0x00, "Coinage", IP_KEY_NONE )
    PORT_DIPSETTING(    0x02, "A 2/1 B 2/1 C 2/1" )
    PORT_DIPSETTING(    0x04, "A 2/1 B 1/3 C 2/1" )
    PORT_DIPSETTING(    0x00, "A 1/1 B 1/1 C 1/1" )
    PORT_DIPSETTING(    0x06, "A 1/1 B 1/6 C 1/1" )
    PORT_DIPNAME( 0x08, 0x00, "Cabinet", IP_KEY_NONE )
    PORT_DIPSETTING(    0x00, "Upright" )
    PORT_DIPSETTING(    0x08, "Cocktail" )
    PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_UP | IPF_4WAY )
    PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
    PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN | IPF_4WAY )
    PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )
     * IN2 (all bits are inverted)
     * bit 7 : unused
     * bit 6 : DOWN player 1
     * bit 5 : unused
     * bit 4 : UP player 1
     * bit 3 : COCKTAIL or UPRIGHT cabinet (0 = UPRIGHT)
     * bit 2 :\ coins per play
     * bit 1 :/
     * bit 0 : DOWN player 2 (TABLE only)
     */
    machineInputPort.InputPortSet(2, machineInputPort.InputPortGetValueForBits(HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH));
    FroggerVBlankEnable = false;
    FroggerInterruptVector = 0;

    MachineTheZ80::Setup(display, sdCard); //, 2);
    Z80CurrentCpu = 0;
    cpu[Z80CurrentCpu].port_in = FroggerPortIn;
    cpu[Z80CurrentCpu].port_out = FroggerPortOut;
    cpu[Z80CurrentCpu].read_byte = FroggerMemoryRead;
    cpu[Z80CurrentCpu].write_byte = FroggerMemoryWrite;
    Z80CurrentCpu = 1;
    cpu[Z80CurrentCpu].port_in = FroggerPortInCPU1;
    cpu[Z80CurrentCpu].port_out = FroggerPortOutCPU1;
    cpu[Z80CurrentCpu].read_byte = FroggerSoundMemoryRead;
    cpu[Z80CurrentCpu].write_byte = FroggerSoundMemoryWrite;
}

// ***********************************************************

void GameFrogger::Loop(TheDisplay &display)
{
    unsigned long count = 0;
    while (count < FROGGER_FREQUENCY / 3)
    {
        unsigned long start = cpu[Z80CurrentCpu].cyc;
        Z80CurrentCpu = 0;
        z80_step(&cpu[Z80CurrentCpu]);
        count += cpu[Z80CurrentCpu].cyc - start;
        start = cpu[Z80CurrentCpu].cyc;
        z80_step(&cpu[Z80CurrentCpu]);
        count += cpu[Z80CurrentCpu].cyc - start;
        start = cpu[Z80CurrentCpu].cyc;
        z80_step(&cpu[Z80CurrentCpu]);
        count += cpu[Z80CurrentCpu].cyc - start;
        start = cpu[Z80CurrentCpu].cyc;
        z80_step(&cpu[Z80CurrentCpu]);
        count += cpu[Z80CurrentCpu].cyc - start;
        Z80CurrentCpu = 1;
        z80_step(&cpu[Z80CurrentCpu]);
        frogger_snd_icnt++;
        z80_step(&cpu[Z80CurrentCpu]);
        frogger_snd_icnt++;
        // "latch" interrupt until CPU has them enabled
        if (frogger_snd_irq_state & 2 && cpu[Z80CurrentCpu].iff1 & IFF_1)
        {
            z80_gen_int(&cpu[1], INT_RST38);
            frogger_snd_irq_state &= ~2; // clear flag
        }
    }
    if (FroggerVBlankEnable)
    {
        Z80CurrentCpu = 0;
        z80_gen_nmi(&cpu[Z80CurrentCpu]);
        // z80_gen_int(&cpu[Z80CurrentCpu], INT_NMI); // #define INT_NMI 0xFFFD // Non-maskable interrupt
    }

    for (int8_t spriteNumber = FROGGER_SPRITE_COUNT - 1; spriteNumber >= 0; spriteNumber--)
    {
        uint32_t address = 0xb05f - 4 * (spriteNumber + 1);
        if (boardMemory[address + 3] != 0)
        {
            uint8_t posX = boardMemory[address];
            posX = (((posX << 4) & 0xf0) | ((posX >> 4) & 0x0f)); // - 16;
            uint8_t posY = boardMemory[address + 3];
            uint8_t color = boardMemory[address + 2] & 7;
            color = ((color >> 1) & 0x03) | ((color << 2) & 0x04);
            uint8_t code = boardMemory[address + 1] & 0x3f; // ?
            bool flipx = boardMemory[address + 1] & 0x80;
            bool flipY = boardMemory[address + 1] & 0x40;
            // if ((posY > -16) && (posY < 288) && (posX > -16) && (posX < 224))
            {
                DIRTY_MIN(posX, screenDirtyMinX)
                DIRTY_MAX(posX + spriteWidth, screenDirtyMaxX)
                DIRTY_MIN(posY, screenDirtyMinY)
                DIRTY_MAX(posY + spriteHeight, screenDirtyMaxY)
                // CODE = spriteram[offs + 1] & 0x3f,
                // COLOR = col,
                // FLIPX = spriteram[offs + 1] & 0x80, FLIPY = spriteram[offs + 1] & 0x40,
                // SX = x, SY = spriteram[offs + 3],
                for (uint8_t x = 0; x < spriteWidth; x++)
                {
                    for (uint8_t y = 0; y < spriteHeight; y++)
                    {
                        uint32_t index = posX + x + (posY + y) * screenWidth;
                        // screenData[index] = colorRGB[color]; // TODO
                        //screenData[index] = colorRGB[spriteGfx[x + y * spriteWidth]];
                    }
                }
            }
        }
    }

    for (uint8_t row = 0; row < 36; row++)
    {
        for (uint8_t col = 0; col < 28; col++)
        {
        }
    }
}

// ***********************************************************

void GameFrogger::KeyChange(uint8_t button)
{
    if (keyPressed[button])
        switch (button)
        {
        case BUTTON_CREDIT:
            machineInputPort.InputPortUpdate(0, 7, LOW);
            break;
        case BUTTON_START:
            machineInputPort.InputPortUpdate(1, 7, LOW);
            break;
        case BUTTON_LEFT:
            machineInputPort.InputPortUpdate(0, 5, LOW);
            break;
        case BUTTON_RIGHT:
            machineInputPort.InputPortUpdate(0, 4, LOW);
            break;
        case BUTTON_UP:
            machineInputPort.InputPortUpdate(2, 4, LOW);
            break;
        case BUTTON_DOWN:
            machineInputPort.InputPortUpdate(2, 6, LOW);
            break;
        default:
            break;
        }
    else
        switch (button)
        {
        case BUTTON_CREDIT:
            machineInputPort.InputPortUpdate(0, 7, HIGH);
            break;
        case BUTTON_START:
            machineInputPort.InputPortUpdate(1, 7, HIGH);
            break;
        case BUTTON_LEFT:
            machineInputPort.InputPortUpdate(0, 5, HIGH);
            break;
        case BUTTON_RIGHT:
            machineInputPort.InputPortUpdate(0, 4, HIGH);
            break;
        case BUTTON_UP:
            machineInputPort.InputPortUpdate(2, 4, HIGH);
            break;
        case BUTTON_DOWN:
            machineInputPort.InputPortUpdate(2, 6, HIGH);
            break;
        default:
            break;
        }
}
/*
bool GameFrogger::PrepareFroggerSprites(GfxLayout layout, uint8_t colorStart, uint8_t totalColors)
{
    spriteWidth = layout.width;
    spriteHeight = layout.height;
    spritesCount = layout.total;
    MY_DEBUG2(GAME_FOLDER, "Sprite width:", spriteWidth);
    MY_DEBUG2(GAME_FOLDER, "Prepare sprites:", spritesCount);
    spriteGfx = (uint8_t *)malloc(spriteWidth * spriteHeight * spritesCount);
    if (spriteGfx == NULL)
    {
        spritesCount = 0;
        spriteWidth = 0;
        spriteHeight = 0;
        MY_DEBUG(GAME_FOLDER, "Failed to allocate spriteGfx memory to decode");
        return false;
    }
    uint8_t bytesPerSprite = layout.charincrement / 8;
    MY_DEBUG2(GAME_FOLDER, "Sprite bytesPerSprite:", bytesPerSprite);
    for (uint16_t spriteIndex = 0; spriteIndex < spritesCount; spriteIndex++)
    {
        uint32_t at = spriteIndex * spriteWidth * spriteHeight;
        uint8_t c;
        for (uint8_t y = 0; y < spriteHeight; y++)
        {
            for (uint8_t x = 0; x < spriteWidth; x++)
            {
                uint8_t ym = y & 7 | ((x & 8) ^ 8);
                uint8_t xm = x & 7 | (y & 8);
                c = 0;
                uint8_t b = (xm ^ 7) + ((ym & 8) << 1);
                uint8_t m = (0x80 >> (ym & 7));
                uint32_t address = spriteIndex * bytesPerSprite + b;
                if (spriteMemory[address] & m)
                {
                    c = 1;
                }
                if (spriteMemory[0x800 + address] & m)
                {
                    c += 2;
                }
                spriteGfx[at] = c;
                at++;
            }
        }
    }
    free(spriteMemory);
    spriteMemory = NULL;
    return true;
}*/