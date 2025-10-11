#include "MachineTheZ80.hpp"

uint8_t Z80PortIn(z80 *const z, uint8_t port)
{
    // if(allGames[currentGame].readPorts == NULL) return InputPorts[port];
    return InputPortRead[port]->handler(port);
}

void Z80PortOut(z80 *const z, uint8_t port, uint8_t value)
{
    // InputPorts[port] = value;
    InputPortWrite[port]->handler(port, value);
}

uint8_t Z80MemoryRead(void *xx, uint16_t address)
{
    return readMemoryHandler(address);
    //return memoryReadHandler[address].handler(address);
}

void Z80MemoryWrite(void *xx, uint16_t address, uint8_t value)
{
    writeMemoryHandler(address, value);
    //if (memoryWriteHandler[address].handler != NULL)
    //{
    //    memoryWriteHandler[address].handler(address, value);
    //    return;
    //}
    //boardMemory[address] = value;
}

// *******************************************************************

MachineTheZ80::MachineTheZ80() { MY_DEBUG(TAG, "created"); }

// *******************************************************************

MachineTheZ80::~MachineTheZ80() { MY_DEBUG(TAG, "destroyed"); }

// *******************************************************************

void MachineTheZ80::Setup(TheDisplay &display, TheSdCard &sdCard)
{
    MY_DEBUG(TAG, "setup");
    for (uint8_t i = 0; i < MAX_Z80_CPU; i++)
    {
        Z80InterruptEnable[i] = false;
        Z80InterruptVector[i] = 0;
        Z80AskForNMI[i] = false;
    }
    Z80CurrentRunningCpu = 1; // TODO: by game, 2 for frogger
    Z80CurrentCpu = 0;
    for (uint8_t i = 0; i < Z80CurrentRunningCpu; i++)
        z80_init(&cpu[i]);
    // https://github.com/Sandroid75/pac/blob/main/pac/src/pac.c#L393
    cpu[Z80CurrentCpu].port_in = Z80PortIn;
    cpu[Z80CurrentCpu].port_out = Z80PortOut;
    cpu[Z80CurrentCpu].read_byte = Z80MemoryRead;
    cpu[Z80CurrentCpu].write_byte = Z80MemoryWrite;
    TheGame::Setup(display, sdCard);
}

// *******************************************************************

void MachineTheZ80::Loop(TheDisplay &display)
{
    unsigned long count = 0;
    while (count < allGames[currentGame].frequency)
    {
        unsigned long start = cpu[Z80CurrentCpu].cyc;
        z80_step(&cpu[Z80CurrentCpu]);
        count += cpu[Z80CurrentCpu].cyc - start;
    }
    if (Z80AskForNMI[Z80CurrentCpu])
    {
        // Z80AskForNMI = false;
        z80_gen_nmi(&cpu[Z80CurrentCpu]);
    }
    if (Z80InterruptEnable[Z80CurrentCpu])
    {
        //Z80InterruptEnable[Z80CurrentCpu] = false;
        z80_gen_int(&cpu[Z80CurrentCpu], Z80InterruptVector[Z80CurrentCpu]);
    }
    allGames[currentGame].video.drawDisplay();
}

// *******************************************************************

void MachineTheZ80::SwapRomD0D1(uint32_t offset, uint32_t length)
{
    for (uint32_t address = 0; address < length; address++)
    {
        boardMemory[address] = (boardMemory[address + offset] & 0xfc) | ((boardMemory[address + offset] & 1) << 1) | ((boardMemory[address + offset] & 2) >> 1);
    }
}
