#ifndef THEZ80_HPP
#define THEZ80_HPP

#pragma once

#include "../MyDefine.h"
#include "../TheGame.hpp"
// https://github.com/Sandroid75/pac/blob/main/pac/deps/z80/z80.h
#include "CpuZ80.h"

class MachineTheZ80 : public TheGame
{
public:
    MachineTheZ80();
    ~MachineTheZ80();
    void Setup(TheDisplay &display, TheSdCard &sdCard);
    void Loop(TheDisplay &display);

    z80 cpu[MAX_Z80_CPU];

    void SwapRomD0D1(uint32_t offset, uint32_t length);

    /*
    // https://github.com/mamedev/mame/blob/master/src/lib/util/coretmpl.h
        // Data lines D0 and D6 swapped
        uint8_t *ROM = memregion("maincpu")->base();
        for (int i = 0; i < 0x8000; i++)
        {
            ROM[i] = bitswap<8>(ROM[i],7,0,5,4,3,2,1,6);
        }
            */

protected:
private:
    const char *TAG = "MachineTheZ80";

};

#endif