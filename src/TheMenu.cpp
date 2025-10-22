#include "TheMenu.hpp"
#include "TheMenu.h"

void TheMenu::Setup(TheDisplay &display, TheSdCard &sdCard)
{
    MY_DEBUG(TAG, "setup");
    TheGame::Setup(display, sdCard);
    //for (x = 0; x < screenLength; x++){
    //    screenData[x] = myBlack;
    //    dirtybuffer[x] = DIRTY_YES;
    //}
    CHECK_IF_DIRTY_XY(0, 0)
    CHECK_IF_DIRTY_XY(SCREEN_WIDTH, SCREEN_HEIGHT)
    x = 0;
    y = 0;
    //uint32_t length = display.CreateEmptyImage(&fullScreen, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void TheMenu::Loop(TheDisplay &display)
{
    y+=3;
    if (y >= SCREEN_HEIGHT)
    {
        y -= SCREEN_HEIGHT;
    }
    x+=7;
    if (x >= SCREEN_WIDTH)
        x -= SCREEN_WIDTH;
    GamePlotPixel(x, y, myRed);
    CHECK_IF_DIRTY_XY(x, y)
}
