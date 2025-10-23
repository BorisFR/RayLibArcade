#include "TheMenu.hpp"
#include "TheMenu.h"

void TheMenu::Setup(TheDisplay &display, TheSdCard &sdCard)
{
    MY_DEBUG(TAG, "setup");
    TheGame::Setup(display, sdCard);
    // for (x = 0; x < screenLength; x++){
    //     screenData[x] = myBlack;
    //     dirtybuffer[x] = DIRTY_YES;
    // }
    CHECK_IF_DIRTY_XY(0, 0)
    CHECK_IF_DIRTY_XY(SCREEN_WIDTH, SCREEN_HEIGHT)

    display.CreateBackground();
    uint32_t deltaY = MENU_LEFT + MENU_TOP * SCREEN_WIDTH;
    uint32_t size = MENU_WIDTH * MENU_HEIGHT * sizeof(PNG_PTR_TYPE);
    allEntry = (PNG_PTR_TYPE *)malloc(MENU_MAX * size);
    uint32_t index = 0;
    for (uint8_t i = 0; i < MENU_MAX; i++)
    {
        PNG_PTR_TYPE *pointer = &allEntry[index];
        index += MENU_WIDTH * MENU_HEIGHT;
        bool r = sdCard.LoadJpgFileTo(pointer, "_menu/frogger.jpg", MENU_WIDTH);
        if (r == false)
        {
            return;
        }
        for (uint32_t y = 0; y < MENU_HEIGHT; y++)
        {
            uint32_t from = y * MENU_WIDTH;
            uint32_t to = deltaY + y * SCREEN_WIDTH;
            for (uint32_t x = 0; x < MENU_WIDTH; x++)
            {
                pngImage[to + x] = pointer[from + x];
            }
        }
        deltaY += MENU_HEIGHT * SCREEN_WIDTH;
    }
    // PNG_PTR_TYPE *ptr = &pngImage[200 + 500 * SCREEN_WIDTH];
    // bool r = sdCard.LoadJpgFileTo(ptr, "_menu/frogger.jpg", SCREEN_WIDTH);
    CHECK_IF_DIRTY_XY(0, 0)
    CHECK_IF_DIRTY_XY(SCREEN_WIDTH, SCREEN_HEIGHT)
}

void TheMenu::Loop(TheDisplay &display)
{
    y += 3;
    if (y >= (MENU_MAX * MENU_HEIGHT))
    {
        y -= (MENU_MAX * MENU_HEIGHT);
    }
    x += 7;
    if (x >= MENU_WIDTH)
        x -= MENU_WIDTH;
    GamePlotPixel(x, y, myRed);
    CHECK_IF_DIRTY_XY(x, y)
}
