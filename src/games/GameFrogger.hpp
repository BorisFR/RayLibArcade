#ifndef GAME_FROGGER_HPP
#define GAME_FROGGER_HPP

#pragma once

#include "../MyDefine.h"
#include "../GamesList.h"

#include "GameFrogger.h"
#include "../machines/MachineTheZ80.hpp"
#define FROGGER_FOLDER "frogger"
#define FROGGER_WIDTH 768
#define FROGGER_HEIGHT 224

#define FROGGER_FREQUENCY 3072000 / 60
#define FROGGER_FREQUENCY2 1789750 / 60

#include "../TheMemory.h"
#include "../machines/CpuZ80.h"

class GameFrogger : public MachineTheZ80
{
public:
    GameFrogger();
    ~GameFrogger() override;
    void Setup(TheDisplay &display, TheSdCard &sdCard) override;
    void Loop(TheDisplay &display) override;

    void KeyChange(uint8_t button) override;

private:
    //const char *TAG = "Game Frogger";
    unsigned long frogger_snd_icnt;
    //bool PrepareFroggerSprites(GfxLayout layout, uint8_t colorStart, uint8_t totalColors);

    struct GfxLayout tileLayout =
        {
            8, 8,             /* 8*8 characters */
            256,              /* 256 characters */
            2,                /* 2 bits per pixel */
            {256 * 8 * 8, 0}, /* the two bitplanes are separated */
            {7 * 8, 6 * 8, 5 * 8, 4 * 8, 3 * 8, 2 * 8, 1 * 8, 0 * 8},
            {0, 1, 2, 3, 4, 5, 6, 7},
            8 * 8 /* every char takes 8 consecutive bytes */
        };
    struct GfxLayout spriteLayout =
        {
            16, 16,            /* 16*16 sprites */
            64,                /* 64 sprites */
            2,                 /* 2 bits per pixel */
            {64 * 16 * 16, 0}, /* the two bitplanes are separated */
            {23 * 8, 22 * 8, 21 * 8, 20 * 8, 19 * 8, 18 * 8, 17 * 8, 16 * 8, 7 * 8, 6 * 8, 5 * 8, 4 * 8, 3 * 8, 2 * 8, 1 * 8, 0 * 8},
            {0, 1, 2, 3, 4, 5, 6, 7, 8 * 8 + 0, 8 * 8 + 1, 8 * 8 + 2, 8 * 8 + 3, 8 * 8 + 4, 8 * 8 + 5, 8 * 8 + 6, 8 * 8 + 7},
            32 * 8 /* every sprite takes 32 consecutive bytes */
        };

};

#endif