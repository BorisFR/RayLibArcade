#include "MachineDriver8080bw.hpp"

// ********************************************************

MachineDriver8080bw::MachineDriver8080bw() { MY_DEBUG2TEXT(TAG, GAME_NAME, "created"); }

// *******************************************************************

MachineDriver8080bw::~MachineDriver8080bw() { MY_DEBUG2TEXT(TAG, GAME_NAME, "destroyed"); }

// *******************************************************************

void MachineDriver8080bw::Setup(TheDisplay &display, TheSdCard &sdCard)
{
    MY_DEBUG(TAG, "setup");
    mb14241data = 0;
    mb14241amount = 0;
    I8085_Reset();
    //cpu.Initialize();
    TheGame::Setup(display, sdCard);
}

// *******************************************************************

void MachineDriver8080bw::Loop(TheDisplay &display)
{
    I8085_Execute(16666);
    I8085_Cause_Interrupt(0xcf);
    I8085_Execute(33333);
    I8085_Cause_Interrupt(0xd7);
    //cpu.EmulateCycles(16666);
    //cpu.Interrupt(0xcf); // RST 1
    //cpu.EmulateCycles(33333);
    //cpu.Interrupt(0xd7); // RST 2
}