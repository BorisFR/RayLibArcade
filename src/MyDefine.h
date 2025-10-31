#ifndef MY_DEFINE_H
#define MY_DEFINE_H

#pragma once
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"

//#define USE_DIRTY

// to comment for RayLib on PC :)
// #define ESP32P4 1

#include <stdint.h>

#define SCREEN_WIDTH (800)
#define SCREEN_HEIGHT (1280)

#define GAME_NUMBER_IS_MENU 0

#define MACHINE_THEMENU 1
#define MACHINE_8080BW 2
#define MACHINE_Z80 3
// #define MACHINE_xxx 3

enum Buttons
{
    BUTTON_CREDIT = 0,
    BUTTON_START = 1,
    BUTTON_LEFT = 2,
    BUTTON_RIGHT = 3,
    BUTTON_UP = 4,
    BUTTON_DOWN = 5,
    BUTTON_FIRE = 6,
    BUTTON_2 = 7,
    BUTTON_COIN2 = 8,
    BUTTON_COIN3 = 9,
    BUTTON_LEFT_P2 = 10,
    BUTTON_RIGHT_P2 = 11,
    BUTTON_UP_P2 = 12,
    BUTTON_DOWN_P2 = 13,
    BUTTON_END = 14
};

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define SIZEOF(arr) sizeof(arr) / sizeof(*arr)

extern uint8_t currentGame;
extern uint8_t exitGame;
extern uint8_t nextGame;

#define MAX_Z80_CPU 3

extern uint8_t GetBitFromNumber(uint8_t value);

#define DIRTY_MIN(value, minValue) \
    if (value < minValue)          \
    {                              \
        minValue = value;          \
    }
#define DIRTY_MAX(value, maxValue) \
    if (value > maxValue)          \
    {                              \
        maxValue = value;          \
    }

#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

#define PORT_SWITCH_VALUE // MY_DEBUG2(TAG, "Value:", allGames[currentGame].machine.inputPorts[i].default_value)
#define PORT_SWITCH_DEFAULT_VALUE(value) portValue += value;

#define PORT_BIT_VALUE                                                                           \
    if (allGames[currentGame].machine.inputPorts[i].default_value != IP_ACTIVE_LOW)              \
        portValue &= ~(1 << GetBitFromNumber(allGames[currentGame].machine.inputPorts[i].mask)); \
    else                                                                                         \
        portValue |= (1 << GetBitFromNumber(allGames[currentGame].machine.inputPorts[i].mask));

#define PORT_BIT_PARAM(name, key)                                                                        \
    keyPort[key] = portNumber;                                                                           \
    keyBit[key] = GetBitFromNumber(allGames[currentGame].machine.inputPorts[i].mask);                    \
    keyValuePressed[key] = allGames[currentGame].machine.inputPorts[i].default_value; \
    PORT_BIT_VALUE                                                                                       \
    // MY_DEBUG2(TAG, name, GetBitFromNumber(allGames[currentGame].machine.inputPorts[i].mask))

// #define PORT_NOT_USE(name) PORT_BIT_PARAM(name, BUTTON_NOT_USED)

#endif