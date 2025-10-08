#ifndef GAME_FROGGER_H
#define GAME_FROGGER_H

#pragma once

#include "../MyDefine.h"

#include "../machines/CpuZ80.h"
#include "../TheMemory.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define IFF_1       0x01       // IFF1 flip-flop

extern uint8_t frogger_snd_irq_state;

extern uint8_t FroggerInterruptVector;
extern bool FroggerVBlankEnable;
#define FROGGER_SPRITE_COUNT 8
#define FROGGER_SPRITE_INFO 4 // x/y/palette/state
#ifdef __cplusplus
extern "C" {
#endif
uint8_t FroggerPortIn(z80 *const z, uint8_t port);
void FroggerPortOut(z80 *const z, uint8_t port, uint8_t value);
uint8_t FroggerPortInCPU1(z80 *const z, uint8_t port);
void FroggerPortOutCPU1(z80 *const z, uint8_t port, uint8_t value);
uint8_t FroggerMemoryRead(void *xx, uint16_t address);
void FroggerMemoryWrite(void *xx, uint16_t address, uint8_t value);
uint8_t FroggerSoundMemoryRead(void *xx, uint16_t address);
void FroggerSoundMemoryWrite(void *xx, uint16_t address, uint8_t value);
#ifdef __cplusplus
}
#endif

#endif