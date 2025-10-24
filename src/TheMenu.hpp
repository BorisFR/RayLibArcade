#ifndef THE_MENU_HPP
#define THE_MENU_HPP

#pragma once

#include "MyDefine.h"
#include "TheGame.hpp"
#include "TheDisplay.hpp"

#define MENU_MAX 7
#define MENU_WIDTH 500
#define MENU_HEIGHT 140
#define MENU_LEFT ((SCREEN_WIDTH - MENU_WIDTH)/2)
#define MENU_TOP 200

class TheMenu : public TheGame
{

public:
    TheMenu();
    ~TheMenu();
    void Setup(TheDisplay &display, TheSdCard &sdCard);
    void Loop(TheDisplay &display);

protected:
private:
    const char *TAG = "TheMenu";
    uint16_t y;
    uint16_t x;
    PNG_PTR_TYPE fullScreen;
    PNG_PTR_TYPE *allEntry;
};

#endif