#ifndef GAMES_LIST_H
#define GAMES_LIST_H

#pragma once

#include "MyDefine.h"
#include "GameDefinition.h"
#include "TheMemory.h"

#ifdef __cplusplus
extern "C"
{
#endif

    // *******************************************************************
    // 8080w
#ifdef MACHINE_8080BW
#include "games/Invaders.h"
#include "games/Invadpt2.h"
#endif

    // *******************************************************************
    // Z80
#ifdef MACHINE_Z80
#include "games/Frogger.h"
#include "games/Pacman.h"
#include "games/Galaxian.h"
#include "games/Pengo.h"
//#include "games/Crush.h"
#endif

    // *******************************************************************
    // xxx
#ifdef MACHINE_xxx
#include "games/x.h"
#endif

    // *******************************************************************

#define GAME_MACHINE allGames[currentGame].machineType
#define GAME_FOLDER allGames[currentGame].folder
#define GAME_NAME allGames[currentGame].name
#define GAME_SCREEN_WIDTH allGames[currentGame].video.screenWidth
#define GAME_SCREEN_HEIGHT allGames[currentGame].video.screenHeight
#define GAME_ORIENTATION allGames[currentGame].video.orientation
#define GAME_SCREEN_FREQUENCY allGames[currentGame].frequency
#define GAME_ROMS allGames[currentGame].machine.roms
#define GAME_INPUTPORTS allGames[currentGame].machine.inputPorts

#ifdef __cplusplus
}
#endif

#endif