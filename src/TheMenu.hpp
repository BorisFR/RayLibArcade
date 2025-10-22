#ifndef THE_MENU_HPP
#define THE_MENU_HPP

#pragma once

#include "MyDefine.h"
#include "TheGame.hpp"
#include "TheDisplay.hpp"

class TheMenu : public TheGame
{

public:
    void Setup(TheDisplay &display, TheSdCard &sdCard);
    void Loop(TheDisplay &display);

protected:
private:
    const char *TAG = "TheMenu";
    uint32_t y;
    uint32_t x;
    PNG_PTR_TYPE fullScreen;
};

#endif