#include "TheDisplay.hpp"

bool justTouch;

#ifdef ESP32P4

void touchCallBack(esp_lcd_touch_handle_t tp)
{
    // if (touchedInProgress)
    //     return;
    // esp_rom_printf("Touch interrupt callback\n");
    justTouch = true;
    // touchedInProgress = true;
}
#endif

// *******************************************************************

#ifdef ESP32P4
IRAM_ATTR static bool test_notify_refresh_ready(esp_lcd_panel_handle_t panel, esp_lcd_dpi_panel_event_data_t *edata, void *user_ctx)
{
    SemaphoreHandle_t refresh_finish = (SemaphoreHandle_t)user_ctx;
    BaseType_t need_yield = pdFALSE;
    xSemaphoreGiveFromISR(refresh_finish, &need_yield);
    return (need_yield == pdTRUE);
}
#endif

// *******************************************************************

TheDisplay::TheDisplay()
{
    MY_DEBUG(TAG, "created")
    mustExit = false;
}

// *******************************************************************

TheDisplay::~TheDisplay() { MY_DEBUG(TAG, "destroyed") }

// *******************************************************************

void TheDisplay::Setup()
{
    MY_DEBUG(TAG, "setup")
#ifdef ESP32P4
    ESP_LOGI(TAG, "Initialize LCD device");

    ESP_LOGI(TAG, "Turn on LCD backlight");
    gpio_config_t bk_gpio_config = {
        .pin_bit_mask = 1ULL << SCREEN_BACK_LIGHT_PIN,
        .mode = GPIO_MODE_OUTPUT};
    gpio_config(&bk_gpio_config);
    gpio_set_level((gpio_num_t)SCREEN_BACK_LIGHT_PIN, SCREEN_ON);
    ESP_LOGI(TAG, "MIPI DSI PHY Powered on");
    esp_ldo_channel_config_t ldo_mipi_phy_config = {
        .chan_id = SCREEN_MIPI_DSI_PHY_PWR_LDO_CHAN,
        .voltage_mv = SCREEN_MIPI_DSI_PHY_PWR_LDO_VOLTAGE_MV,
    };
    esp_ldo_acquire_channel(&ldo_mipi_phy_config, &ldo_mipi_phy);
    ESP_LOGI(TAG, "Initialize MIPI DSI bus");
    // esp_lcd_dsi_bus_config_t bus_config = JD9365_PANEL_BUS_DSI_2CH_CONFIG();
#define ILI9881C_PANEL_BUS_DSI_2CH_CONFIG() {    \
    .bus_id = 0,                                 \
    .num_data_lanes = 2,                         \
    .phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT, \
    .lane_bit_rate_mbps = 1400,                  \
} // before: lane_bit_rate_mbps=1000
    // formula: https://docs.espressif.com/projects/esp-iot-solution/en/latest/display/lcd/mipi_dsi_lcd.html
    esp_lcd_dsi_bus_config_t bus_config = ILI9881C_PANEL_BUS_DSI_2CH_CONFIG();
    esp_lcd_new_dsi_bus(&bus_config, &mipi_dsi_bus);
    ESP_LOGI(TAG, "Install panel IO");
    // esp_lcd_dbi_io_config_t dbi_config = JD9365_PANEL_IO_DBI_CONFIG();
#define ILI9881C_PANEL_IO_DBI_CONFIG() { \
    .virtual_channel = 0,                \
    .lcd_cmd_bits = 8,                   \
    .lcd_param_bits = 8,                 \
}
    esp_lcd_dbi_io_config_t dbi_config = ILI9881C_PANEL_IO_DBI_CONFIG();
    esp_lcd_new_panel_io_dbi(mipi_dsi_bus, &dbi_config, &mipi_dbi_io);
    ESP_LOGI(TAG, "Install JD9365S panel driver");
    esp_lcd_dpi_panel_config_t dpi_config = {
        .virtual_channel = 0,
        .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT,
        .dpi_clock_freq_mhz = 80,
        .pixel_format = SCREEN_MIPI_DPI_PX_FORMAT,
        .num_fbs = SCREEN_FRAME_BUFFER,
        .video_timing = {
            .h_size = 800,
            .v_size = 1280,
            .hsync_pulse_width = 40, // 20,
            .hsync_back_porch = 140, // 20,
            .hsync_front_porch = 40,
            .vsync_pulse_width = 4,
            .vsync_back_porch = 16,  // 8,
            .vsync_front_porch = 16, // 20,
        },
        .flags = {
            .use_dma2d = true,
        },
    };
    jd9365_vendor_config_t vendor_config = {
        .mipi_config = {
            .dsi_bus = mipi_dsi_bus,
            .dpi_config = &dpi_config,
            .lane_num = SCREEN_MIPI_DSI_LANE_NUM,
        },
    };
    const esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = SCREEN_RESET_PIN,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = SCREEN_BPP,
        .vendor_config = &vendor_config,
    };
    esp_lcd_new_panel_jd9365(mipi_dbi_io, &panel_config, &panel_handle);
    esp_lcd_panel_reset(panel_handle);
    esp_lcd_panel_init(panel_handle);
    esp_lcd_panel_disp_on_off(panel_handle, true);
    esp_lcd_panel_mirror(panel_handle, true, true);
    esp_lcd_dpi_panel_event_callbacks_t cbs = {
        .on_color_trans_done = test_notify_refresh_ready,
    };
    refresh_finish = xSemaphoreCreateBinary();
    esp_lcd_dpi_panel_register_event_callbacks(panel_handle, &cbs, refresh_finish);

    byte_per_pixel = (SCREEN_BPP + 7) / 8;
    // printf("Byte per pixel: %d\n", byte_per_pixel);
#if SCREEN_FRAME_BUFFER == 1
    esp_lcd_dpi_panel_get_frame_buffer(panel_handle, SCREEN_FRAME_BUFFER, (void **)&fbs[0]); // SCREEN_FRAME_BUFFER == 1
#else
#if SCREEN_FRAME_BUFFER == 2
    esp_lcd_dpi_panel_get_frame_buffer(panel_handle, SCREEN_FRAME_BUFFER, (void **)&fbs[0], (void **)&fbs[1]); // SCREEN_FRAME_BUFFER == 2
#endif
#endif
    currentFrameBuffer = 0;
    // ESP_LOGI(TAG, "Screen memory Need: %u", (unsigned int)(SCREEN_WIDTH * SCREEN_HEIGHT * byte_per_pixel));
#ifdef LIMIT_FPS
    msTimer = millis();
#endif
#ifndef NO_FPS
    fpsTimer = millis();
#endif
    // printf("Ready\n");

    // TOUCH
    touch = gsl3680_touch();
    touch.begin(I2C_SDA_PIN, I2C_SCL_PIN, TOUCH_RST, TOUCH_INT, touchCallBack);
    touch.set_rotation(TOUCH_ROTATION);
    // justTouch = false;
#else
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raylib Arcade");
    pixels = (Color *)malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Color));
    memset(pixels, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Color));
    fb_image.width = SCREEN_WIDTH;
    fb_image.height = SCREEN_HEIGHT;
    fb_image.mipmaps = 1;
    fb_image.data = pixels;
    fb_image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    fb_texture = LoadTextureFromImage(fb_image);
    srcRec = {0.0f, 0.0f, (float)fb_texture.width, (float)fb_texture.height};
    origin = {0.0f, 0.0f};
    actualScreenWidth = GetScreenWidth();
    actualScreenHeight = GetScreenHeight();
    dstRec = {0.0f, 0.0f, (float)actualScreenWidth, (float)actualScreenHeight};
#ifdef LIMIT_FPS
    SetTargetFPS(FPS_LIMIT);
#endif
#endif
    touchedInProgress = false;
    scrollUp = false;
    scrollDown = false;
    scrollLeft = false;
    scrollRight = false;
    scrollDistanceVertical = 0;
    scrollDistanceHorizontal = 0;
    scrollSpeedVertical = 0;
    scrollSpeedHorizontal = 0;
    oneClick = false;

    myWhite = Rgb888ToRgb565(255, 255, 255);
    myBlack = Rgb888ToRgb565(0, 0, 0);
    myGreen = Rgb888ToRgb565(0x20, 0xff, 0x20);
    myRed = Rgb888ToRgb565(0xff, 0x20, 0x20);
    myYellow = Rgb888ToRgb565(0xff, 0xff, 0x20);
    myCyan = Rgb888ToRgb565(0x20, 0xff, 0xff);
    myPurple = Rgb888ToRgb565(0xff, 0x20, 0xff);

    for (uint8_t k = 0; k < BUTTON_END; k++)
    {
        keyPressed[k] = false;
        keyChanged[k] = false;
        keyPort[k] = BUTTON_END; // if player use buttons which are not use by the game
        keyBit[k] = 0;
        keyValuePressed[k] = 0;
    }
}

// *******************************************************************

void TheDisplay::SetDisplayForGame(uint32_t zoomX, uint32_t zoomY, uint32_t atX, uint32_t atY)
{
    screenZoomX = zoomX;
    screenZoomY = zoomY;
    screenPosX = atX;
    screenPosY = atY;
}

void TheDisplay::SetVerticalPositionForGame(uint32_t y)
{
    screenPosY = y;
}

// *******************************************************************

uint32_t TheDisplay::GetMaxZoomX() { return SCREEN_WIDTH / screenWidth; }

// *******************************************************************

uint32_t TheDisplay::GetMaxZoomY() { return SCREEN_HEIGHT / screenHeight; }

// *******************************************************************

uint32_t TheDisplay::GetPaddingLeftForZoom(uint32_t zoomX) { return (SCREEN_WIDTH - (screenWidth * zoomX)) / 2; }

// *******************************************************************

#ifdef ESP32P4
THE_COLOR TheDisplay::ConvertRGB565ToRGB888(uint16_t color565)
{
    return color565;
}
#else
Color TheDisplay::ConvertRGB565ToRGB888(uint16_t color565)
{
    // Extract the red, green, and blue components
    unsigned char r = (color565 >> 11) & 0x1F; // 5 bits for red
    unsigned char g = (color565 >> 5) & 0x3F;  // 6 bits for green
    unsigned char b = color565 & 0x1F;         // 5 bits for blue

    // Scale them to 8-bit values (0-255)
    r = (r * 255) / 31;
    g = (g * 255) / 63;
    b = (b * 255) / 31;
    // Return as a raylib Color struct
    return (Color){r, g, b, 255}; // Alpha is set to 255 (fully opaque)
}

// Color TheDisplay::ConvertRGB888ToRGBA8888(uint32_t rgb888)
// {
//     // Extract the red, green, and blue components
//     uint8_t red = (rgb888 >> 16) & 0xFF;
//     uint8_t green = (rgb888 >> 8) & 0xFF;
//     uint8_t blue = rgb888 & 0xFF;
//     // Add the alpha channel (e.g., 255 for full opacity)
//     uint8_t alpha = 0xFF;
//     // Combine into RGBA8888 format
//     // uint32_t rgba8888 = (red << 24) | (green << 16) | (blue << 8) | alpha;
//     // return rgba8888;
//     return Color{red, green, blue, alpha};
// }
#endif

uint32_t TheDisplay::GetPaddingTopForZoom(uint32_t zoomY) { return (SCREEN_HEIGHT - (screenHeight * zoomY)) / 2; }

void TheDisplay::DisplayPng(uint32_t atX, uint32_t atY)
{
    for (uint32_t x = 0; x < pngWidth; x++)
    {
        for (uint32_t y = 0; y < pngHeight; y++)
        {
            uint32_t pos = atX + x + (atY + y) * SCREEN_WIDTH;
            uint32_t index = x + y * pngWidth;
#ifdef ESP32P4
            fbs[currentFrameBuffer][pos] = pngImage[index];
#else
            pixels[pos] = ConvertRGB565ToRGB888(pngImage[index]);
#endif
        }
    }
}

void TheDisplay::FillScreen(THE_COLOR color)
{
#ifdef ESP32P4
    THE_COLOR c = Rgb888ToRgb565(0, 100, 0); // color;
#else
    Color c = Color{0, 100, 0, 255}; // ConvertRGB565ToRGB888(color);
#endif
    for (uint32_t x = 0; x < SCREEN_WIDTH; x++)
    {
        for (uint32_t y = 0; y < SCREEN_HEIGHT; y++)
        {
            uint32_t pos = x + y * SCREEN_WIDTH;
#ifdef ESP32P4
            fbs[currentFrameBuffer][pos] = c;
#else
            pixels[pos] = c;
#endif
        }
    }
    // memset(screenData, color, screenLength);
}

bool TheDisplay::CreateBackground()
{
    if (pngMemorySize > 0)
    {
        MY_DEBUG(TAG, "Background already create")
        return false;
    }
    pngMemorySize = SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(PNG_PTR_TYPE);
    pngImage = (PNG_PTR_TYPE *)malloc(pngMemorySize);
    if (pngImage == NULL)
    {
        pngMemorySize = 0;
        MY_DEBUG(TAG, "Not enought memory to create background")
        return false;
    }
    memset(pngImage, 0x0, pngMemorySize);
    pngWidth = SCREEN_WIDTH;
    pngHeight = SCREEN_HEIGHT;
    // MY_DEBUG(TAG, "Background ready")
    return true;
}

uint32_t TheDisplay::CreateEmptyImage(PNG_PTR_TYPE *image, uint32_t width, uint32_t height)
{
    uint32_t length = width * height * sizeof(PNG_PTR_TYPE);
    image = (PNG_PTR_TYPE *)malloc(length);
    if (image == NULL)
    {
        MY_DEBUG2(TAG, "Not enought memory for image:", length)
        return 0;
    }
    memset(image, 0, length);
    return length;
}

bool TheDisplay::TouchMoving()
{
    return touchedInProgress;
}

bool TheDisplay::Clicked()
{
    bool temp = oneClick;
    oneClick = false;
    return temp;
}

uint16_t TheDisplay::ClickPositionX()
{
    return touchEndX;
}

uint16_t TheDisplay::ClickPositionY()
{
    return touchEndY;
}

bool TheDisplay::ScrollUp()
{
    bool temp = scrollUp;
    scrollUp = false;
    return temp;
}

bool TheDisplay::ScrollDown()
{
    bool temp = scrollDown;
    scrollDown = false;
    return temp;
}

bool TheDisplay::ScrollLeft()
{
    bool temp = scrollLeft;
    scrollLeft = false;
    return temp;
}

bool TheDisplay::ScrollRight()
{
    bool temp = scrollRight;
    scrollRight = false;
    return temp;
}

uint16_t TheDisplay::ScrollDistance()
{
    return 0;
}

uint8_t TheDisplay::ScrollSpeedHorizontal()
{
    return scrollSpeedHorizontal;
}

uint8_t TheDisplay::ScrollSpeedVertical()
{
    return scrollSpeedVertical;
}

// *******************************************************************

THE_COLOR TheDisplay::GetColorFromPalette(uint8_t colorIndex, uint8_t paletteIndex)
{
    return paletteColor[paletteIndex * 4 + colorIndex];
    // switch (colorIndex)
    //{
    // case 0:
    //     return paletteColor1[paletteIndex];
    // case 1:
    //     return paletteColor2[paletteIndex];
    // case 2:
    //     return paletteColor3[paletteIndex];
    // case 3:
    //     return paletteColor4[paletteIndex];
    // }
    // return paletteColor2[paletteIndex];
}

void TheDisplay::TouchMove(uint16_t x, uint16_t y)
{
    if (!touchedInProgress)
    {
        touchedInProgress = true;
        touchStart = millis();
        touchStartX = x;
        touchStartY = y;
        scrollUp = false;
        scrollDown = false;
        scrollLeft = false;
        scrollRight = false;
        scrollDistanceVertical = 0;
        scrollDistanceHorizontal = 0;
        scrollSpeedVertical = 0;
        scrollSpeedHorizontal = 0;
        oneClick = false;
        MY_DEBUG2(TAG, "Touch START:", touchStart)
        MY_DEBUG2(TAG, "Touch START x:", touchStartX)
        MY_DEBUG2(TAG, "Touch START y:", touchStartY)
    }
    if (x < SCREEN_WIDTH)
        touchEndX = x;
    if (y < SCREEN_HEIGHT)
        touchEndY = y;
}

void TheDisplay::TouchEnd()
{
    touchedInProgress = false;
    touchEnd = millis();
    MY_DEBUG2(TAG, "Touch END:", touchEnd)
    MY_DEBUG2(TAG, "Touch END x:", touchEndX)
    MY_DEBUG2(TAG, "Touch END y:", touchEndY)
    int elaps = (int)(touchEnd - touchStart);
    MY_DEBUG2(TAG, "Touch END duration:", elaps)
    if (elaps < TOUCH_DELAY_CLICK)
    {
        MY_DEBUG(TAG, "Touch CLICK")
        oneClick = true;
        return;
    }
    else
    // if (elaps > TOUCH_DELAY_SCROLL)
    {
        if (touchStartY > touchEndY)
        {
            scrollDistanceVertical = touchStartY - touchEndY;
            if (scrollDistanceVertical > TOUCH_MOVE_DISTANCE)
            {
                scrollSpeedVertical = (uint8_t)((1.0f * scrollDistanceVertical) / (1.0f * elaps));
                MY_DEBUG2(TAG, "Touch SCROLL UP:", scrollSpeedVertical)
                scrollUp = true;
            }
        }
        else
        {
            scrollDistanceVertical = touchEndY - touchStartY;
            if (scrollDistanceVertical > TOUCH_MOVE_DISTANCE)
            {
                scrollSpeedVertical = scrollDistanceVertical / elaps;
                MY_DEBUG2(TAG, "Touch SCROLL DOWN:", scrollSpeedVertical)
                scrollDown = true;
            }
        }
        if (touchStartX > touchEndX)
        {
            scrollDistanceHorizontal = touchStartX - touchEndX;
            if (scrollDistanceHorizontal > TOUCH_MOVE_DISTANCE)
            {
                scrollSpeedHorizontal = scrollDistanceVertical / elaps;
                MY_DEBUG2(TAG, "Touch SCROLL LEFT:", scrollSpeedHorizontal)
                scrollLeft = true;
            }
        }
        else
        {
            scrollDistanceHorizontal = touchEndX - touchStartX;
            if (scrollDistanceHorizontal > TOUCH_MOVE_DISTANCE)
            {
                scrollSpeedHorizontal = scrollDistanceVertical / elaps;
                MY_DEBUG2(TAG, "Touch SCROLL RIGHT:", scrollSpeedHorizontal)
                scrollRight = true;
            }
        }
    }
}

// *******************************************************************

void TheDisplay::Loop()
{
#ifdef ESP32P4
    if (justTouch)
    {
        justTouch = false;
        uint16_t tx;
        uint16_t ty;
        if (touch.getTouch(&tx, &ty))
        {
            lastTouch = millis();
            // std::string temp = std::to_string(tx) + " / " + std::to_string(ty);
            // MY_DEBUG2TEXT(TAG, "Moving X:", temp.c_str())
            TouchMove(tx, ty);
        }
        else
        {
            if (millis() - lastTouch > TOUCH_DELAY_RELEASED)
                TouchEnd();
        }
    }
    else
    {
        if (touchedInProgress)
        {
            if (millis() - lastTouch > TOUCH_DELAY_RELEASED)
                TouchEnd();
        }
    }
    /*if (!justTouch && touchedInProgress)
    {
        if (millis() - touchStart > TOUCH_DELAY_RELEASED)
        {
            touchedInProgress = false;
            // esp_rom_printf("Touch release\n");
            if (touchStartX < SCREEN_WIDTH / 2)
            {
                KEY_RELEASED(BUTTON_CREDIT)
            }
            else
            {
                KEY_RELEASED(BUTTON_START)
            }
        }
        else
        {
            uint16_t tx;
            uint16_t ty;
            if (touch.getTouch(&tx, &ty))
            {
                // std::string temp = std::to_string(touchStartX) + " / " + std::to_string(touchStartY);
                // MY_DEBUG2TEXT(TAG, "Moving X:", temp.c_str())
                TouchMove(tx, ty);
            }
        }
    }
    if (justTouch)
    {
        justTouch = false;
        uint16_t tx;
        uint16_t ty;
        if (touch.getTouch(&tx, &ty))
        {
            TouchMove(tx, ty);
            // std::string temp = std::to_string(touchStartX) + " / " + std::to_string(touchStartY);
            // MY_DEBUG2TEXT(TAG, "Touch X:", temp.c_str())
            if (touchStartX < SCREEN_WIDTH / 2)
            {
                KEY_PRESSED(BUTTON_CREDIT)
            }
            else
            {
                KEY_PRESSED(BUTTON_START)
            }
        }
    }
*/
    // if (touchInProgress && !justTouch)
    // {
    //     touchInProgress = false;
    //     if (touchStartX < SCREEN_WIDTH / 2)
    //     {
    //         KEY_RELEASED(BUTTON_CREDIT)
    //     }
    //     else
    //     {
    //         KEY_RELEASED(BUTTON_START)
    //     }
    // }
    // else
    // {
    //     if (justTouch)
    //     {
    //         justTouch = false;
    //         // esp_lcd_touch_read_data(tp);
    //         if (touch.getTouch(&touchStartX, &touchStartY))
    //         {
    //             if (!touchInProgress)
    //             {
    //                 touchInProgress = true;
    //                 if (touchStartX < SCREEN_WIDTH / 2)
    //                 {
    //                     KEY_PRESSED(BUTTON_CREDIT)
    //                 }
    //                 else
    //                 {
    //                     KEY_PRESSED(BUTTON_START)
    //                 }
    //             }
    //         }
    //     }
    // }
#ifdef LIMIT_FPS
    if (millis() - msTimer < 16)
    {
        vTaskDelay((16 - (millis() - msTimer)) / portTICK_PERIOD_MS);
    }
    msTimer = millis();
#endif
#endif
#ifndef NO_FPS
    frameCount++;
    if (millis() - fpsTimer > 1000)
    {
        if (lastFrameCount != frameCount)
        {
            lastFrameCount = frameCount;
#ifndef NO_FPS_ON_CONSOLE
            MY_DEBUG2(TAG, "fps:", (lastFrameCount * 1000 / (millis() - fpsTimer)));
#endif
        }
        frameCount = 0;
        fpsTimer = millis();
    }
#endif
#ifdef ESP32P4
#else
    if (WindowShouldClose())
    {
        mustExit = true;
        CloseWindow();
        return;
    }
    // TOUCH
    int tCount = GetTouchPointCount();
    if (tCount > MAX_TOUCH_POINTS)
        tCount = MAX_TOUCH_POINTS;
    for (int i = 0; i < tCount; ++i)
        touchPositions[i] = GetTouchPosition(i);
    if (tCount > 0)
    {
        // touchPositions[0] = GetTouchPosition(0);
        TouchMove(touchPositions[0].x, touchPositions[0].y);
    }
    else
    {
        // if (touchedInProgress)
        //     TouchEnd();
    }

    if (touchedInProgress)
    {
        Vector2 pos = GetMousePosition();
        TouchMove(pos.x, pos.y);
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        Vector2 pos = GetMousePosition();
        TouchMove(pos.x, pos.y);
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
        if (touchedInProgress)
            TouchEnd();
    }

    if (IsKeyPressed(KEY_Q))
    {
        nextGame = GAME_NUMBER_IS_MENU;
        exitGame = true;
    }
    if (IsKeyPressed(KEY_G))
    {
        gfxDebug = !gfxDebug;
    }
    if (IsKeyPressed(KEY_S))
    {
        std::string temp = std::string(PC_PATH) + "sdcard/ss/" + std::string(allGames[currentGame].folder) + ".png";
        Image screenshot = LoadImageFromScreen(); // Capture current framebuffer
        if (ExportImage(screenshot, temp.c_str()))
        {
            printf("Screenshot saved as %s\n", temp.c_str());
        }
        else
        {
            printf("Failed to save screenshot.\n");
        }
        UnloadImage(screenshot); // Free image memory
                                 // TakeScreenshot(temp.c_str());
    }
    IS_KEY_PRESSED(KEY_FIVE, BUTTON_CREDIT)
    IS_KEY_PRESSED(KEY_ONE, BUTTON_START)
    IS_KEY_PRESSED(KEY_LEFT, BUTTON_LEFT)
    IS_KEY_PRESSED(KEY_RIGHT, BUTTON_RIGHT)
    IS_KEY_PRESSED(KEY_UP, BUTTON_UP)
    IS_KEY_PRESSED(KEY_DOWN, BUTTON_DOWN)
    IS_KEY_PRESSED(KEY_LEFT_CONTROL, BUTTON_FIRE)
    if (screenHeight + screenWidth == 0)
        return;
    BeginDrawing();
    if (actualScreenHeight != GetScreenHeight() || actualScreenWidth != GetScreenWidth())
    {
        actualScreenHeight = GetScreenHeight();
        actualScreenWidth = actualScreenHeight * SCREEN_RATIO;
        if (actualScreenWidth > GetScreenWidth())
        {
            actualScreenWidth = GetScreenWidth();
            actualScreenHeight = actualScreenWidth / SCREEN_RATIO;
        }
        SetWindowSize(actualScreenWidth, actualScreenHeight);
        dstRec = {0.0f, 0.0f, (float)actualScreenWidth, (float)actualScreenHeight};
        ClearBackground(BLACK);
        // ClearBackground(GREEN);
    }
#endif
    if (ScrollLeft())
    {
        if (currentGame != 0)
        {
            nextGame = 0;
            exitGame = true;
        }
    }

    // if(screenDirtyMinX > 0) screenDirtyMinX--;
    // if(screenDirtyMinY > 0) screenDirtyMinY--;
    // screenDirtyMaxX++;
    // screenDirtyMaxY++;
    if (screenDirtyMaxX > screenWidth)
        screenDirtyMaxX = screenWidth;
    if (screenDirtyMaxY > screenHeight)
        screenDirtyMaxY = screenHeight;
    //  DRAW SCREEN
    //  currentFrameBuffer = (currentFrameBuffer + 1) % SCREEN_FRAME_BUFFER;
    uint32_t index = 0;
    for (uint32_t y = screenDirtyMinY; y < screenDirtyMaxY; y++)
    {
        uint16_t posY = screenPosY + y * screenZoomY;
        for (uint32_t x = screenDirtyMinX; x < screenDirtyMaxX; x++)
        {
            index = y * screenWidth + x;
            // #ifdef ESP32P4
            // if (screenData[index] != screenDataOld[index])
            if (dirtybuffer[index])
            {
                // #endif
                uint16_t posX = screenPosX + x * screenZoomX;
                THE_COLOR color = screenData[index];
#ifndef ESP32P4
                // Color pixelColor = GetColor(color);
                Color pixelColor = ConvertRGB565ToRGB888(color);
#endif
                for (uint16_t zx = 0; zx < screenZoomX; zx++)
                {
                    for (uint16_t zy = 0; zy < screenZoomY; zy++)
                    {
#ifdef ESP32P4
                        if (dirtybuffer[index] == DIRTY_TRANSPARENT)
                        {
                            uint32_t p = posX + zx + (posY + zy) * SCREEN_WIDTH;
                            fbs[currentFrameBuffer][posX + zx + (posY + zy) * SCREEN_WIDTH] = pngImage[p];
                        }
                        else
                        {
                            fbs[currentFrameBuffer][posX + zx + (posY + zy) * SCREEN_WIDTH] = color;
                        }
#else
                        if (dirtybuffer[index] == DIRTY_TRANSPARENT)
                        {
                            uint32_t p = posX + zx + (posY + zy) * SCREEN_WIDTH;
                            pixels[posX + zx + (posY + zy) * SCREEN_WIDTH] = ConvertRGB565ToRGB888(pngImage[p]);
                        }
                        else
                        {
                            pixels[posX + zx + (posY + zy) * SCREEN_WIDTH] = pixelColor;
                        }
#endif
                    }
                }
                // #ifdef ESP32P4
                dirtybuffer[index] = DIRTY_NOT;
            }
            // #endif
            index++;
        }
    }

    //
    /*
    if (hasSprites)
    {
        uint32_t addressPosition = 0x5060 + 15;
        uint32_t addressState = 0x4FFF;
        for (uint8_t index = 0; index < 8; index++)
        {
            int16_t posY = boardMemory[addressPosition];
            addressPosition--;
            int16_t posX = boardMemory[addressPosition];
            addressPosition--;
            uint8_t paletteIndex = boardMemory[addressState];
            addressState--;
            uint8_t state = boardMemory[addressState];
            addressState--;
            if (posX >= 16 && posX < screenWidth - 16 && posY >= 0 && posY < screenHeight)
            {
                bool flipX = (state & 0x02) == 0x02;
                bool flipY = (state & 0x01) == 0x01;
                uint8_t spriteIndex = (state & 0xFC) >> 2;
                uint32_t at = spriteIndex * spriteWidth * spriteHeight;
                for (uint16_t p = 0; p < spriteWidth * spriteHeight; p++)
                {
                    uint8_t colorIndex = spriteGfx[at];
                    if (colorIndex > 0) // black is transparent
                    {
                        uint32_t deltaX = (p % spriteWidth) * screenZoomX;
                        uint32_t deltaY = (p / spriteWidth) * screenZoomY;
                        // Adjust coordinates. The coordinates are (x, y) from the lower right corner of the screen.
                        // Additionally, we need to account for the fact that the Y axis of the sprites is offset
                        // by 16 (the background tiles can be drawn out another 2 tiles, which is 16 pixels).
                        int16_t convertedX = screenWidth - posX - 1;
                        int16_t convertedY = screenHeight - 16 - posY;
                        int16_t x = (screenPosX + convertedX * screenZoomX);
                        int16_t y = (screenPosY + convertedY * screenZoomY);
                        THE_COLOR c = GetColorFromPalette(colorIndex, paletteIndex);
                        for (uint16_t zx = 0; zx < screenZoomX; zx++)
                        {
                            for (uint16_t zy = 0; zy < screenZoomY; zy++)
                            {
#ifdef ESP32P4
                                fbs[currentFrameBuffer][x + zx + deltaX + (y + zy + deltaY) * SCREEN_WIDTH] = c;
#else
                                DrawPixel(x + zx + deltaX, y + zy + deltaY, GetColor(c));
#endif
                            }
                        }
                    }
                    at++;
                }
            }
        }
    }*/
    // memcpy(screenDataOld, screenData, screenLength);
    // memcpy(dirtybuffer, screenData, screenLength);
    screenDirtyMinX = screenWidth;
    screenDirtyMaxX = 0;
    screenDirtyMinY = screenHeight;
    screenDirtyMaxY = 0;
    if (gfxDebug)
    {
        uint32_t debugStartY = 0;
#ifdef DEBUG_DISPLAY_COLOR
        // DEBUG color
        if (hasColor)
        {
            uint16_t dimension = 30;
            uint16_t max = MIN(colorMemorySize, 40);
            debugStartY = ((max * dimension) / SCREEN_WIDTH) * dimension + dimension;
            for (uint16_t c = 0; c < max /*colorMemorySize*/; c++)
            {
                THE_COLOR color = colorRGB[c];
#ifndef ESP32P4
                Color pixelColor = GetColor(color);
#endif
                uint32_t posX = (c * dimension) % SCREEN_WIDTH;
                uint32_t posY = ((c * dimension) / SCREEN_WIDTH) * dimension;
                for (uint32_t x = 0; x < dimension; x++)
                {
                    for (uint32_t y = 0; y < dimension; y++)
                    {
#ifdef ESP32P4
                        fbs[currentFrameBuffer][posX + x + (posY + y) * SCREEN_WIDTH] = color;
#else
                        pixels[posX + x + (posY + y) * SCREEN_WIDTH] = pixelColor;
#endif
                    }
                }
            }
        }
#endif
#ifdef DEBUG_DISPLAY_PALETTE
        // DEBUG palettes
        if (hasPalette)
        {
            uint16_t dimension = 30; // 15;
            uint16_t max = MIN(paletteColorSize, 40);
            debugStartY += ((max * dimension) / SCREEN_WIDTH) * dimension + dimension;
            for (uint16_t p = 0; p < max /*paletteColorSize*/; p++)
            {
                THE_COLOR color = paletteColor[p];
#ifndef ESP32P4
                Color pixelColor = GetColor(color);
#endif
                uint32_t posX = (p * dimension) % SCREEN_WIDTH;
                uint32_t posY = debugStartY + ((p * dimension) / SCREEN_WIDTH) * dimension;

                for (uint32_t x = 0; x < dimension; x++)
                {
                    for (uint32_t y = 0; y < dimension; y++)
                    {
                        // Draw the color
#ifdef ESP32P4
                        fbs[currentFrameBuffer][posX + x + (posY + y) * SCREEN_WIDTH] = color;
#else
                        pixels[posX + x + (posY + y) * SCREEN_WIDTH] = pixelColor;

#endif
                    }
                }
            }
        }
#endif

#ifdef DEBUG_DISPLAY_GFX
        if (hasGfx)
        {
            for (uint8_t g = 0; g < countGfxElement; g++)
            {
                GfxElement *element = allGfx[g];
                for (uint16_t z = 0; z < element->total_elements; z++)
                // for (uint16_t z = 0; z < 4; z++)
                {
                    for (uint16_t y = 0; y < element->height; y++)
                    {
                        uint16_t posY = 280 + g * 150 + (z * element->width / screenWidth) * element->height * screenZoomY + y * screenZoomY;
                        uint32_t posX = ((z * element->width) % screenWidth) * screenZoomX;
                        uint8_t *pointerLine = element->gfxdata->line[y + z * element->height];
                        for (uint16_t x = 0; x < element->gfxdata->width; x++)
                        {
                            uint8_t pixel = pointerLine[x];
                            THE_COLOR color = colorRGB[pixel];
                            ;
                            // if (pixel == 0)
                            //     color = colorRGB[0];
                            // else
                            //{
                            //     color = colorRGB[2];
                            // }
#ifndef ESP32P4
                            Color pixelColor = GetColor(color);
#endif
                            for (uint16_t zx = 0; zx < screenZoomX; zx++)
                            {
                                for (uint16_t zy = 0; zy < screenZoomY; zy++)
                                {
#ifdef ESP32P4
                                    fbs[currentFrameBuffer][posX + x * screenZoomX + zx + (posY + zy) * SCREEN_WIDTH] = color;
#else
                                    pixels[posX + x * screenZoomX + zx + (posY + zy) * SCREEN_WIDTH] = pixelColor;
#endif
                                }
                            }
                        }
                    }
                }
            }
        }
#endif
#ifdef DEBUG_DISPLAY_TILES
        // DEBUG tiles
        if (hasTiles)
        {
            uint16_t theWidth = 1.5 * tileWidth;
            uint16_t theHeight = 1.5 * tileHeight;
            for (uint16_t n = 0; n < tilesCount; n++)
            {
                uint16_t posX = (n * theWidth) % SCREEN_WIDTH;
                uint16_t posY = 40 + 4 * 15 + 10 + ((n * theWidth) / SCREEN_WIDTH) * theHeight;

                uint32_t at = n * tileWidth * tileHeight;
                for (uint16_t p = 0; p < tileWidth * tileHeight; p++)
                {
                    uint8_t colorIndex = tileGfx[at];
#ifdef ESP32P4
                    fbs[currentFrameBuffer][posX + (p % tileWidth) + (posY + (p / tileWidth)) * SCREEN_WIDTH] = GetColor(GetColorFromPalette(colorIndex, 1));
#else
                    DrawPixel(posX + (p % tileWidth), posY + (p / tileWidth), GetColor(GetColorFromPalette(colorIndex, 1)));
#endif
                    at++;
                }
                DrawPixel(posX, posY, WHITE);
            }
        }
#endif
#ifdef DEBUG_DISPLAY_SPRITES
        // DEBUG sprites
        if (hasSprites)
        {
            uint16_t theWidth = 1.5 * spriteWidth;
            uint16_t theHeight = 1.5 * spriteHeight;
            for (uint16_t n = 0; n < spritesCount; n++)
            {
                uint16_t posX = (n * theWidth) % SCREEN_WIDTH;
                uint16_t posY = 200 + ((n * theWidth) / SCREEN_WIDTH) * theHeight;

                uint32_t at = n * spriteWidth * spriteHeight;
                for (uint16_t p = 0; p < spriteWidth * spriteHeight; p++)
                {
                    uint8_t colorIndex = spriteGfx[at];
#ifdef ESP32P4
                    fbs[currentFrameBuffer][posX + (p % spriteWidth) + (posY + (p / spriteWidth)) * SCREEN_WIDTH] = GetColor(GetColorFromPalette(colorIndex, 1));
#else
                    THE_COLOR color = colorRGB[colorIndex];
                    DrawPixel(posX + (p % spriteWidth), posY + (p / spriteWidth), GetColor(color)); // colorIndex));
#endif
                    at++;
                }

                DrawPixel(posX, posY, WHITE);
            }
        }
#endif
    } // gfxDebug
#ifdef ESP32P4
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, fbs[currentFrameBuffer]);
    xSemaphoreTake(refresh_finish, portMAX_DELAY);
#else
    UpdateTexture(fb_texture, pixels);
    // DrawTexture(fb_texture, 0, 0, WHITE);
    DrawTexturePro(fb_texture, srcRec, dstRec, origin, 0.0f, WHITE);
#ifndef NO_FPS
    // ClearRectangle(10, SCREEN_HEIGHT - 20, 30, 20);
    //  Print(std::to_string(lastFrameCount), 10, SCREEN_HEIGHT - 20);
    // ClearRectangle(SCREEN_WIDTH - 80, SCREEN_HEIGHT - 20, 80, 20);
    // DrawFPS(SCREEN_WIDTH - 80, SCREEN_HEIGHT - 20);
    ClearRectangle(actualScreenWidth - 80, 10, 80, 20);
    DrawFPS(actualScreenWidth - 80, 10);
#endif
    EndDrawing();
#endif
}

// *******************************************************************

#ifdef ESP32P4
#else
void TheDisplay::ChangeTitle(std::string text)
{
    std::string t = "RayLib Arcade - " + text;
    SetWindowTitle(t.c_str());
}

bool TheDisplay::MustExit()
{
    return mustExit;
}

// *******************************************************************

void TheDisplay::ClearRectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
#ifdef ESP32P4
    // gfx->fillRect(x, y, width, height, RGB565_BLACK);
#else
    DrawRectangle(x, y, width, height, Color{0, 0, 0, 120});
#endif
}

// *******************************************************************

#ifdef ESP32P4
#else
void TheDisplay::Print(std::string text, uint32_t x, uint32_t y)
{
#ifdef ESP32P4
    // gfx->setCursor(x, y);
    // gfx->setTextSize(2);
    // gfx->setTextColor(RGB565_WHITE);
    // gfx->println(text);
#else
    DrawText(text.c_str(), x, y, 20, WHITE);
#endif
}
#endif

// *******************************************************************

void TheDisplay::Clear()
{
#ifdef ESP32P4
    // gfx->fillScreen(RGB565_BLACK);
#endif
}

// *******************************************************************

void TheDisplay::BeginWrite()
{
#ifdef ESP32P4
    // gfx->startWrite();
#endif
}

// *******************************************************************

void TheDisplay::EndWrite()
{
#ifdef ESP32P4
    // gfx->endWrite();
#endif
}

// *******************************************************************

// void TheDisplay::DrawPng(uint8_t *pngImage, int16_t width, int16_t height)
//{
// #ifdef ESP32P4
//     //// gfx->draw24bitRGBBitmap(0, 0, pngImage, width, height);
//     // gfx->draw16bitRGBBitmap(0, 0, reinterpret_cast<uint16_t *>(pngImage), width, height);
// #endif
// }

// *******************************************************************

// void TheDisplay::Pixel(uint16_t x, uint16_t y, uint16_t color)
//{
// #ifdef ESP32P4
//     // gfx->writePixel(x, y, color);
//     //// gfx->drawPixel(x, y, color);
// #endif
// }
#endif

// *******************************************************************

THE_COLOR TheDisplay::Rgb888ToRgb565(uint8_t red, uint8_t green, uint8_t blue)
{
#ifdef ESP32P4
    // Shift and mask the RGB888 values to fit into RGB565 format
    uint16_t r = (red >> 3) & 0x1F;   // 5 bits for red
    uint16_t g = (green >> 2) & 0x3F; // 6 bits for green
    uint16_t b = (blue >> 3) & 0x1F;  // 5 bits for blue
    // Combine into a single 16-bit value
    return (r << 11) | (g << 5) | b;
#else
    // return (red << 24) | (green << 16) | (blue << 8) | 255;
    uint16_t r = (red >> 3) & 0x1F;   // 5 bits for red
    uint16_t g = (green >> 2) & 0x3F; // 6 bits for green
    uint16_t b = (blue >> 3) & 0x1F;  // 5 bits for blue
    // Combine into a single 16-bit value
    return (r << 11) | (g << 5) | b;
#endif
}