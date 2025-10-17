#include "TheDisplay.hpp"

#ifdef ESP32P4
bool touched;
unsigned long lastTouch = 0;
#define TOUCH_DELAY_RELEASED 100
bool touchedInProgress = false;

void touchCallBack(esp_lcd_touch_handle_t tp)
{
    lastTouch = millis();
    if (touchedInProgress)
        return;
    // esp_rom_printf("Touch interrupt callback\n");
    touched = true;
    touchedInProgress = true;
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
    MY_DEBUG(TAG, "created");
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
    .lane_bit_rate_mbps = 1000,                  \
}
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
    touched = false;
    touchedInProgress = false;
#else
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib Arcade");
    pixels = (Color *)malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Color));
    memset(pixels, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Color));
    fb_image.width = SCREEN_WIDTH;
    fb_image.height = SCREEN_HEIGHT;
    fb_image.mipmaps = 1;
    fb_image.data = pixels;
    fb_image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    fb_texture = LoadTextureFromImage(fb_image);
#ifdef LIMIT_FPS
    SetTargetFPS(FPS_LIMIT);
#endif
#endif
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

// *******************************************************************

void TheDisplay::Loop()
{
#ifdef ESP32P4
    if (!touched && touchedInProgress)
    {
        if (millis() - lastTouch > TOUCH_DELAY_RELEASED)
        {
            touchedInProgress = false;
            // esp_rom_printf("Touch release\n");
            if (touchX < SCREEN_WIDTH / 2)
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
            if (touch.getTouch(&touchX, &touchY))
            {
                // std::string temp = std::to_string(touchX) + " / " + std::to_string(touchY);
                // MY_DEBUG2TEXT(TAG, "Moving X:", temp.c_str())
            }
        }
    }
    if (touched)
    {
        touched = false;
        if (touch.getTouch(&touchX, &touchY))
        {
            // std::string temp = std::to_string(touchX) + " / " + std::to_string(touchY);
            // MY_DEBUG2TEXT(TAG, "Touch X:", temp.c_str())
            if (touchX < SCREEN_WIDTH / 2)
            {
                KEY_PRESSED(BUTTON_CREDIT)
            }
            else
            {
                KEY_PRESSED(BUTTON_START)
            }
        }
    }

    // if (touchInProgress && !touched)
    // {
    //     touchInProgress = false;
    //     if (touchX < SCREEN_WIDTH / 2)
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
    //     if (touched)
    //     {
    //         touched = false;
    //         // esp_lcd_touch_read_data(tp);
    //         if (touch.getTouch(&touchX, &touchY))
    //         {
    //             if (!touchInProgress)
    //             {
    //                 touchInProgress = true;
    //                 if (touchX < SCREEN_WIDTH / 2)
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
    IS_KEY_PRESSED(KEY_C, BUTTON_CREDIT)
    IS_KEY_PRESSED(KEY_S, BUTTON_START)
    IS_KEY_PRESSED(KEY_LEFT, BUTTON_LEFT)
    IS_KEY_PRESSED(KEY_RIGHT, BUTTON_RIGHT)
    IS_KEY_PRESSED(KEY_UP, BUTTON_UP)
    IS_KEY_PRESSED(KEY_DOWN, BUTTON_DOWN)
    IS_KEY_PRESSED(KEY_LEFT_CONTROL, BUTTON_FIRE)
    if (screenHeight + screenWidth == 0)
        return;
    BeginDrawing();
    // ClearBackground(BLACK);
    // ClearBackground(GREEN);
#endif
    // if(screenDirtyMinX > 0) screenDirtyMinX--;
    // if(screenDirtyMinY > 0) screenDirtyMinY--;
    //screenDirtyMaxX++;
    //screenDirtyMaxY++;
    //if (screenDirtyMaxX > screenWidth)
    //    screenDirtyMaxX = screenWidth;
    //if (screenDirtyMaxY > screenHeight)
    //    screenDirtyMaxY = screenHeight;
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
            if (screenData[index] != screenDataOld[index])
            {
                // #endif
                uint16_t posX = screenPosX + x * screenZoomX;
                THE_COLOR color = screenData[index];
#ifndef ESP32P4
                Color pixelColor = GetColor(color);
#endif
                for (uint16_t zx = 0; zx < screenZoomX; zx++)
                {
                    for (uint16_t zy = 0; zy < screenZoomY; zy++)
                    {
#ifdef ESP32P4
                        fbs[currentFrameBuffer][posX + zx + (posY + zy) * SCREEN_WIDTH] = color;
#else
                        // DrawPixel(posX + zx, posY + zy, pixelColor);
                        pixels[posX + zx + (posY + zy) * SCREEN_WIDTH] = pixelColor;
#endif
                    }
                }
                // #ifdef ESP32P4
            }
            // #endif
            index++;
        }
    }

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
                                DrawPixel(posX + x * screenZoomX + zx, posY + zy, pixelColor);
                            }
                        }
                    }
                }
            }
        }
    }
#endif
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
                                // DrawPixel(x + zx + (p % spriteWidth), y + zy + (p / spriteWidth), GetColor(c));
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
    memcpy(screenDataOld, screenData, screenLength);
    screenDirtyMinX = screenWidth;
    screenDirtyMaxX = 0;
    screenDirtyMinY = screenHeight;
    screenDirtyMaxY = 0;

#ifdef DEBUG_DISPLAY_COLOR
    // DEBUG color
    if (hasColor)
    {
        uint16_t dimension = 30;
        for (uint16_t c = 0; c < colorMemorySize; c++)
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
                    DrawPixel(posX + x, posY + y, pixelColor);
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
        // for (uint8_t p = 0; p < paletteColorSize * 4; p++)
        for (uint16_t p = 0; p < paletteColorSize; p++)
        {
            THE_COLOR color = paletteColor[p];
#ifndef ESP32P4
            Color pixelColor = GetColor(color);
#endif
            uint32_t posX = (p * dimension) % SCREEN_WIDTH;
            uint32_t posY = 40 + ((p * dimension) / SCREEN_WIDTH) * dimension;

            for (uint32_t x = 0; x < dimension; x++)
            {
                for (uint32_t y = 0; y < dimension; y++)
                {
                    // Draw the color
#ifdef ESP32P4
                    fbs[currentFrameBuffer][posX + x + (posY + y) * SCREEN_WIDTH] = color;
#else
                    DrawPixel(posX + x, posY + y, pixelColor);
#endif
                }
            }
            DrawPixel(posX, posY, WHITE);
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
                // DrawPixel(posX + (p % spriteWidth), posY + (p / spriteWidth), GetColor(GetColorFromPalette(colorIndex, 1)));
                THE_COLOR color = colorRGB[colorIndex];
                DrawPixel(posX + (p % spriteWidth), posY + (p / spriteWidth), GetColor(color)); // colorIndex));
#endif
                at++;
            }

            DrawPixel(posX, posY, WHITE);
        }
    }
#endif

#ifdef ESP32P4
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, fbs[currentFrameBuffer]);
    xSemaphoreTake(refresh_finish, portMAX_DELAY);
#else
#ifndef NO_FPS
    UpdateTexture(fb_texture, pixels);
    DrawTexture(fb_texture, 0, 0, WHITE);
    // ClearRectangle(10, SCREEN_HEIGHT - 20, 30, 20);
    //  Print(std::to_string(lastFrameCount), 10, SCREEN_HEIGHT - 20);
    // ClearRectangle(SCREEN_WIDTH - 80, SCREEN_HEIGHT - 20, 80, 20);
    // DrawFPS(SCREEN_WIDTH - 80, SCREEN_HEIGHT - 20);
    ClearRectangle(SCREEN_WIDTH - 80, 10, 80, 20);
    DrawFPS(SCREEN_WIDTH - 80, 10);
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

void TheDisplay::DrawPng(uint8_t *pngImage, int16_t width, int16_t height)
{
#ifdef ESP32P4
    //// gfx->draw24bitRGBBitmap(0, 0, pngImage, width, height);
    // gfx->draw16bitRGBBitmap(0, 0, reinterpret_cast<uint16_t *>(pngImage), width, height);
#endif
}

// *******************************************************************

void TheDisplay::Pixel(uint16_t x, uint16_t y, uint16_t color)
{
#ifdef ESP32P4
    // gfx->writePixel(x, y, color);
    //// gfx->drawPixel(x, y, color);
#endif
}
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
    return (red << 24) | (green << 16) | (blue << 8) | 255;
    // Color color = (Color){red, green, blue, 255};
    // return color;
#endif
}