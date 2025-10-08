#include "GameFrogger.h"

// *******************************************************************

uint8_t FroggerInterruptVector;
unsigned long frogger_snd_icnt;
uint8_t soundregs[15];
uint8_t frogger_snd_command;
uint8_t frogger_snd_ay_port;
uint8_t frogger_snd_irq_state;

uint8_t FroggerPortIn(z80 *const z, uint8_t port)
{
    // return InputPorts[InputPortsIndex[port]];
    return 0;
}

// { 0x40, 0x40, AY8910_read_port_0_r },
uint8_t FroggerPortInCPU1(z80 *const z, uint8_t port)
{
    static const unsigned char frogger_timer[20] = {
        0x00, 0x10, 0x00, 0x10, 0x08, 0x18, 0x08, 0x18, 0x40, 0x50,
        0x80, 0x90, 0x80, 0x90, 0x88, 0x98, 0x88, 0x98, 0xc0, 0xd0};
    // read AY data port
    if (port == 0x40)
    {
        if (frogger_snd_ay_port < 14)
            return soundregs[frogger_snd_ay_port];
        // port A, command register
        if (frogger_snd_ay_port == 14)
            return frogger_snd_command;
        // port B, timer
        if (frogger_snd_ay_port == 15)
        {
            // AY and Z80 run at 1.75 MHz, the counter runs at the same
            // speed. The cycle time thus is 570ns
            // using the instruction counter for the rate isn't perfect as it assumes all
            // instructions run the same time ... which they don't
            return frogger_timer[(frogger_snd_icnt / 20) % 20];
        }
        return 0;
    }
    return 0;
}

void FroggerPortOut(z80 *const z, uint8_t port, uint8_t value)
{
    // InputPorts[InputPortsIndex[port]] = value;
}

// { 0x80, 0x80, AY8910_control_port_0_w },
// { 0x40, 0x40, AY8910_write_port_0_w },
void FroggerPortOutCPU1(z80 *const z, uint8_t port, uint8_t value)
{
    // 40 is the ay data port
    if (port == 0x40)
    {
        soundregs[frogger_snd_ay_port] = value;
        return;
    }
    // 80 is the ay control port selecting the register
    if (port== 0x80)
    {
        frogger_snd_ay_port = value & 15;
        return;
    }
}

// *******************************************************************

bool FroggerVBlankEnable;
/*
    { 0x0000, 0x3fff, MRA_ROM },
    { 0x8000, 0x87ff, MRA_RAM },
    { 0x8800, 0x8800, watchdog_reset_r },
    { 0xa800, 0xabff, MRA_RAM },	// video RAM
    { 0xb000, 0xb05f, MRA_RAM },	// screen attributes, sprites
    { 0xe000, 0xe000, input_port_0_r },	// IN0
    { 0xe002, 0xe002, input_port_1_r },	// IN1
    { 0xe004, 0xe004, input_port_2_r },	// IN2
*/
uint8_t FroggerMemoryRead(void *xx, uint16_t address)
{
    // if (address < 0x4000)
    //    return boardMemory[address];
    if (address == 0x8800) // watchdog
        return 0x00;
    if (address == 0xe000) // IN0: Coin1,Coin0,L1,R1,Fire1A,CREDIT,Fire1B,U2
        return InputPorts[0];
    if (address == 0xe002) // IN1: START1,START2,L2,R2,Fire2A,Fire2B,LIVES1,LIVES0
        return InputPorts[1];
    if (address == 0xe004) // IN2: -,D1,-,U1,UPRIGHT,COINS_P_PLAY,D2
        return InputPorts[2];
    return boardMemory[address];
}

uint8_t FroggerSoundMemoryRead(void *xx, uint16_t address)
{
    // frogger audio cpu
    // if(address <  0x1800) return soundMemory[address];
    // main ram
    // if ((address & 0xfc00) == 0x4000)
    //    return soundMemory[address - 0x4000 + 0x1000];
    return soundMemory[address];
}

/*
    { 0x0000, 0x3fff, MWA_ROM },
    { 0x8000, 0x87ff, MWA_RAM },
    { 0xa800, 0xabff, videoram_w, &videoram, &videoram_size },
    { 0xb000, 0xb03f, frogger_attributes_w, &frogger_attributesram },
    { 0xb040, 0xb05f, MWA_RAM, &spriteram, &spriteram_size },
    { 0xb808, 0xb808, interrupt_enable_w },
    { 0xb818, 0xb818, coin_counter_w },
    { 0xb81c, 0xb81c, frogger_counterb_w },
    { 0xd000, 0xd000, soundlatch_w },
    { 0xd002, 0xd002, frogger_sh_irqtrigger_w },
*/

void FroggerMemoryWrite(void *xx, uint16_t address, uint8_t value)
{
    if (address < 0x4000)
        return;
    if (address == 0xd000)
    {
        // PA goes to AY port A and can be read by SND CPU through the AY
        frogger_snd_command = value;
        return;
    }
    if (address == 0xd002)
    {
        // rising edge on bit 3 sets audio irq
        // bit 4 is "am pm"
        // other bits go to connector
        // bit 0 = state written by CPU1
        if (!(frogger_snd_irq_state & 1) && (value & 8))
            frogger_snd_irq_state |= 1;
        // CPU1 writes 0
        if (!(value & 8) && (frogger_snd_irq_state & 1))
        {
            frogger_snd_irq_state |= 2;
            frogger_snd_irq_state &= 0xfe;
        }
        return;
    }
    // unknown
    if (address == 0x8805)
        return;
    if (address == 0xb808)
    {
        FroggerVBlankEnable = value & 1;
        return;
    }
    if (address == 0xb80c)
        return; // flip screen Y
    if (address == 0xb810)
        return; // flip screen X
    if (address == 0xb818)
        return; // coin counter 1
    if (address == 0xb81c)
        return; // coin counter 2
    // AY
    // if ((address & 0xc000) == 0xc000)
    {
        if (address == 0xd006)
            return; // ignore write to first 8255 control register
        if (address == 0xe006)
            return; // ignore write to second 8255 control register
    }

    boardMemory[address] = value;
    // // main ram
    // if ((address & 0xf800) == 0x8000)
    // {
    //     boardMemory[address - 0x8000] = value;
    //     return;
    // }
    // // tile ram
    // if ((address & 0xfc00) == 0xa800)
    // {
    //     // if (!game_started && address == 0xa800 + 800 && Value == 1)
    //     //     game_started = 1;
    //     boardMemory[address - 0xa800 + 0x800] = value; // map to 0x0800
    //     return;
    // }
    // // sprite ram
    // if ((address & 0xff00) == 0xb000)
    // {
    //     boardMemory[address - 0xb000 + 0xc00] = value; // map to 0x0c00
    //     return;
    // }
    // return;
}

void FroggerSoundMemoryWrite(void *xx, uint16_t address, uint8_t value)
{
    // if ((address & 0xfc00) == 0x4000)
    //{
    //     soundMemory[address - 0x4000 + 0x1000] = value;
    //     return;
    // }
    // if ((address & 0xf000) == 0x6000)
    //     return;
    soundMemory[address] = value;
}