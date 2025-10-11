#ifndef MACHINE_DRIVER8080BW_HPP
#define MACHINE_DRIVER8080BW_HPP

#pragma once

#include "../MyDefine.h"
#include "../TheGame.hpp"
#include "CpuI8085.h"
//#include "i8080.hpp"

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
    //CPU_8080 cpu;

};

#endif