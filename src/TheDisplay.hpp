#ifndef THE_DISPLAY_HPP
#define THE_DISPLAY_HPP

#pragma once

#include "MyDefine.h"

#include <cstdint>
#ifdef ESP32P4
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include <string>
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_ldo_regulator.h"
#include "esp_dma_utils.h"
#include "esp_system.h"
#include "esp_mac.h"

#include "esp_lcd_jd9365.h"
#include "bsp/esp-bsp.h"

#include "gsl3680_touch.hpp"
#define I2C_SDA_PIN 7
#define I2C_SCL_PIN 8
#define TOUCH_SPEED 100000
#define TOUCH_NUM I2C_NUM_0
#define TOUCH_INT 21
#define TOUCH_RST 22
// 38
#define TOUCH_ROTATION 2
// #define TOUCH_MAX_POINTS 5

#else
#endif
#include "TheMemory.h"
#include "TheTools.hpp"
#include "GameDefinition.h"
#include <string>

#ifdef ESP32P4
#define SCREEN_WIDTH (800)
#define SCREEN_HEIGHT (1280)
#define SCREEN_BPP (16)
#define SCREEN_RESET_PIN (27)
#define SCREEN_BACK_LIGHT_PIN (23) // set to -1 if not used
#define SCREEN_ON (1)
#define SCREEN_OFF !SCREEN_ON

#define SCREEN_MIPI_DSI_LANE_NUM (2)
#define SCREEN_MIPI_DSI_PHY_PWR_LDO_CHAN (3)
#define SCREEN_MIPI_DSI_PHY_PWR_LDO_VOLTAGE_MV (2500)
#if SCREEN_BPP == 24
#define SCREEN_MIPI_DPI_PX_FORMAT (LCD_COLOR_PIXEL_FORMAT_RGB888)
// #define SCREEN_MIPI_COLOR_FORMAT (LCD_COLOR_FMT_RGB888)
#elif SCREEN_BPP == 18
#define SCREEN_MIPI_DPI_PX_FORMAT (LCD_COLOR_PIXEL_FORMAT_RGB666)
// #define SCREEN_MIPI_COLOR_FORMAT (LCD_COLOR_FMT_RGB666)
#elif SCREEN_BPP == 16
#define SCREEN_MIPI_DPI_PX_FORMAT (LCD_COLOR_PIXEL_FORMAT_RGB565)
// #define SCREEN_MIPI_COLOR_FORMAT (lcd_color_format_t::LCD_COLOR_FMT_RGB565)
#endif

// 1 or 2
#define SCREEN_FRAME_BUFFER (1)
#else
#define SCREEN_WIDTH (800)
#define SCREEN_HEIGHT (1280)
#define SCREEN_RATIO (float)((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)
#define WINDOW_HEIGHT (float)(800.0f)
#define WINDOW_WIDTH ((float)WINDOW_HEIGHT * (float)SCREEN_RATIO)
#include "raylib.h"
#endif

#define MAX_TOUCH_POINTS 10

#define IS_KEY_PRESSED(key, button)     \
    if (IsKeyPressed(key))              \
    {                                   \
        if (!keyPressed[button])        \
        {                               \
            keyPressed[button] = true;  \
            keyChanged[button] = true;  \
        }                               \
    }                                   \
    if (IsKeyReleased(key))             \
    {                                   \
        if (keyPressed[button])         \
        {                               \
            keyPressed[button] = false; \
            keyChanged[button] = true;  \
        }                               \
    }

#define KEY_PRESSED(button)        \
    if (!keyPressed[button])       \
    {                              \
        keyPressed[button] = true; \
        keyChanged[button] = true; \
    }

#define KEY_RELEASED(button)        \
    if (keyPressed[button])         \
    {                               \
        keyPressed[button] = false; \
        keyChanged[button] = true;  \
    }

#define DEBUG_DISPLAY_COLOR
#define DEBUG_DISPLAY_PALETTE
#define DEBUG_DISPLAY_GFX
// #define DEBUG_DISPLAY_TILES
// #define DEBUG_DISPLAY_SPRITES

#ifdef ESP32P4
// #define LIMIT_FPS
//  #define NO_FPS
// #define NO_FPS_ON_CONSOLE
#else
#define FPS_LIMIT 60
// #define LIMIT_FPS
// #define NO_FPS
#define NO_FPS_ON_CONSOLE
#endif

#ifdef ESP32P4
#define TOUCH_DELAY_RELEASED 20
#else
#endif
#define TOUCH_DELAY_CLICK 80
//#define TOUCH_DELAY_SCROLL 150
#define TOUCH_MOVE_DISTANCE 150

class TheDisplay
{
public:
    TheDisplay();
    ~TheDisplay();
    void Setup();
    void Loop();
    void SetDisplayForGame(uint32_t zoomX, uint32_t zoomY, uint32_t atX = 0, uint32_t atY = 0);
    void SetVerticalPositionForGame(uint32_t y);
    uint32_t GetMaxZoomX();
    uint32_t GetMaxZoomY();
    uint32_t GetPaddingLeftForZoom(uint32_t zoomX);
    uint32_t GetPaddingTopForZoom(uint32_t zoomY);
    void DisplayPng(uint32_t atX, uint32_t atY);
    void FillScreen(THE_COLOR color);

    bool CreateBackground();
    uint32_t CreateEmptyImage(PNG_PTR_TYPE *image, uint32_t width, uint32_t height);

    bool TouchMoving();
    bool Clicked();
    uint16_t ClickPositionX();
    uint16_t ClickPositionY();
    bool ScrollUp();
    bool ScrollDown();
    bool ScrollLeft();
    bool ScrollRight();
    uint16_t ScrollDistance();
    uint8_t ScrollSpeedHorizontal();
    uint8_t ScrollSpeedVertical();

#ifdef ESP32P4
    THE_COLOR ConvertRGB565ToRGB888(uint16_t color565);
#else
    void ChangeTitle(std::string text);
    bool MustExit();
    void ClearRectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

    void Clear();
    void BeginWrite();
    void EndWrite();
    // void DrawPng(uint8_t *pngImage, int16_t width, int16_t height);
    // void Pixel(uint16_t x, uint16_t y, uint16_t color);

    void Print(std::string text, uint32_t x, uint32_t y);
    Color ConvertRGB565ToRGB888(uint16_t color565);
    // Color ConvertRGB888ToRGBA8888(uint32_t rgb888);

#endif
    THE_COLOR Rgb888ToRgb565(uint8_t red, uint8_t green, uint8_t blue);
    THE_COLOR GetColorFromPalette(uint8_t colorIndex, uint8_t paletteIndex);

protected:
private:
    const char *TAG = "TheDisplay";
    struct timeval theStartTime;
    bool gfxDebug = false;

    bool mustExit;
#ifdef LIMIT_FPS
    unsigned long msTimer = 0;
#endif
#ifndef NO_FPS
    unsigned long fpsTimer = 0;
    uint16_t frameCount = 0;
    uint16_t lastFrameCount = 0;
#endif
    uint16_t screenZoomX = 1;
    uint16_t screenZoomY = 1;

#ifdef ESP32P4
    uint8_t byte_per_pixel;
    uint16_t *color;
    uint16_t base_color = 0;
    uint16_t color_index = 0;

    esp_ldo_channel_handle_t ldo_mipi_phy = NULL;
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_dsi_bus_handle_t mipi_dsi_bus = NULL;
    esp_lcd_panel_io_handle_t mipi_dbi_io = NULL;
    SemaphoreHandle_t refresh_finish = NULL;
    uint16_t *fbs[SCREEN_FRAME_BUFFER];
    uint8_t currentFrameBuffer;
    // esp_lcd_panel_io_handle_t tp_io_handle = NULL;
    // esp_lcd_touch_handle_t touch_handle;
    gsl3680_touch touch;
    // uint16_t touchX;
    // uint16_t touchY;
#else
    Image fb_image;
    Color *pixels;
    Texture2D fb_texture;
    Rectangle srcRec;
    Rectangle dstRec;
    Vector2 origin;
    uint32_t actualScreenWidth;
    uint32_t actualScreenHeight;

    Vector2 touchPositions[MAX_TOUCH_POINTS] = {0};
#endif

    void TouchMove(uint16_t x, uint16_t y);
    void TouchEnd();

    bool touchedInProgress;
    unsigned long lastTouch;
    unsigned long touchStart;
    unsigned long touchEnd;
    uint16_t touchStartX;
    uint16_t touchStartY;
    uint16_t touchEndX;
    uint16_t touchEndY;
    bool scrollUp;
    bool scrollDown;
    bool scrollLeft;
    bool scrollRight;
    uint16_t scrollDistanceVertical;
    uint16_t scrollDistanceHorizontal;
    uint8_t scrollSpeedVertical;
    uint8_t scrollSpeedHorizontal;
    bool oneClick;
};

#endif // THE_DISPLAY_HPP