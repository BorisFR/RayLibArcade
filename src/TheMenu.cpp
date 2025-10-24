#include "TheMenu.hpp"
#include "TheMenu.h"

TheMenu::TheMenu()
{
    MY_DEBUG(TAG, "created");
    allEntry = nullptr;
}

TheMenu::~TheMenu()
{
    MY_DEBUG(TAG, "destroyed")
    FREE(allEntry)
}
void TheMenu::Setup(TheDisplay &display, TheSdCard &sdCard)
{
    MY_DEBUG(TAG, "setup");
    TheGame::Setup(display, sdCard);
    display.CreateBackground();
    uint32_t deltaY = MENU_LEFT + MENU_TOP * SCREEN_WIDTH;
    uint32_t size = MENU_WIDTH * MENU_HEIGHT * sizeof(PNG_PTR_TYPE);
    allEntry = (PNG_PTR_TYPE *)malloc((MENU_MAX + 1) * size);
    if (allEntry == NULL)
    {
        MY_DEBUG(TAG, "Error allocating allEntry memory");
        return;
    }
    uint32_t index = 0;
    for (uint8_t i = 0; i < (MENU_MAX + 1); i++)
    {
        PNG_PTR_TYPE *pointer = &allEntry[index];
        bool r = sdCard.LoadJpgFileTo(pointer, "/_menu/frogger.jpg", MENU_WIDTH);
        if (r == false)
        {
            return;
        }
        if (i != MENU_MAX)
        {
            for (uint32_t y = 0; y < MENU_HEIGHT; y++)
            {
                uint32_t from = y * MENU_WIDTH;
                uint32_t to = deltaY + y * SCREEN_WIDTH;
                for (uint32_t x = 0; x < MENU_WIDTH; x++)
                {
                    pngImage[to + x] = pointer[from + x];
                    screenData[index + x + y * MENU_WIDTH] = pointer[from + x];
                    dirtybuffer[index + x + y * MENU_WIDTH] = DIRTY_YES;
                }
            }
        }
        index += MENU_WIDTH * MENU_HEIGHT;
        deltaY += MENU_HEIGHT * SCREEN_WIDTH;
    }
    CHECK_IF_DIRTY_XY(0, 0)
    CHECK_IF_DIRTY_XY(SCREEN_WIDTH, SCREEN_HEIGHT)
    x = 0;
    y = 0;
}

uint8_t gameX = 1;
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
    // if ((InputPorts[0] & (1 << 0x10)) != 0)
    if (InputPorts[0] > 0)
    {
        TheGame::Exit(gameX++);
        if (gameX >= countGames)
            gameX = 1;
        return;
    }
    if (display.TouchMoving())
    {
    }
}
