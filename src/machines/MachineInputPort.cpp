#include "MachineInputPort.hpp"

MachineInputPort::MachineInputPort(){MY_DEBUG(TAG, "created");}

MachineInputPort::~MachineInputPort(){MY_DEBUG(TAG, "destroyed");}

void MachineInputPort::InputPortSet(uint8_t port, uint8_t value)
{
    InputPorts[port] = value;
}

void MachineInputPort::InputPortUpdate(uint8_t port, uint8_t bit, uint8_t value)
{
    if (value == 0)
        INPUT_PORT_SET_PORT_BIT_to0(InputPorts[port], bit);
    else
        INPUT_PORT_SET_PORT_BIT_to1(InputPorts[port], bit);
}

uint8_t MachineInputPort::InputPortGetValueForBits(uint8_t bit7, uint8_t bit6, uint8_t bit5, uint8_t bit4, uint8_t bit3, uint8_t bit2, uint8_t bit1, uint8_t bit0)
{
    uint8_t v = 0;
    INPUT_PORT_SET_PORT_BIT(v, 0, bit0);
    INPUT_PORT_SET_PORT_BIT(v, 1, bit1);
    INPUT_PORT_SET_PORT_BIT(v, 2, bit2);
    INPUT_PORT_SET_PORT_BIT(v, 3, bit3);
    INPUT_PORT_SET_PORT_BIT(v, 4, bit4);
    INPUT_PORT_SET_PORT_BIT(v, 5, bit5);
    INPUT_PORT_SET_PORT_BIT(v, 6, bit6);
    INPUT_PORT_SET_PORT_BIT(v, 7, bit7);
    return v;
}