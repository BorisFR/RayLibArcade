#ifndef MACHINE_DRIVER8080BW_HPP
#define MACHINE_DRIVER8080BW_HPP

#pragma once

#include "../MyDefine.h"
#include "../TheGame.hpp"

#define USE_CPU_I8085
#ifdef USE_CPU_I8085
#include "CpuI8085.h"
#else
#include "i8080.hpp" // /!\ this one does not work with Space Invaders Part II
#endif

class MachineDriver8080bw : public TheGame
{
public:
    MachineDriver8080bw();
    ~MachineDriver8080bw();
    void Setup(TheDisplay &display, TheSdCard &sdCard);
    void Loop(TheDisplay &display);

protected:
private:
    const char *TAG = "MachineDriver8080bw";
#ifdef USE_CPU_I8085
#else
    CPU_8080 cpu;
#endif
};

#endif