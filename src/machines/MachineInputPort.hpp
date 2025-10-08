#ifndef MACHINE_INPUT_PORT_H
#define MACHINE_INPUT_PORT_H

#pragma once

#include "../MyDefine.h"

#include "../TheMemory.h"
//#include "CpuZ80.h"

#define INPUT_PORT_SET_PORT_BIT(v, bit_pos, x) (v |= (x << bit_pos))
#define INPUT_PORT_SET_PORT_BIT_to1(v, bit_pos) (v |= (1 << bit_pos))
#define INPUT_PORT_SET_PORT_BIT_to0(v, bit_pos) (v &= ~(1 << bit_pos))


class MachineInputPort
{
public:
    MachineInputPort();
    ~MachineInputPort();

    uint8_t InputPortGetValueForBits(uint8_t bit0, uint8_t bit1, uint8_t bit2, uint8_t bit3, uint8_t bit4, uint8_t bit5, uint8_t bit6, uint8_t bit7);
    void InputPortSet(uint8_t port, uint8_t value);
    void InputPortUpdate(uint8_t port, uint8_t bit, uint8_t value);

private:
const char *TAG = "Machine InputPort";
};

#endif