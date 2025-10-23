#ifndef GAMES_HPP
#define GAMES_HPP

#pragma once

#include "MyDefine.h"
#include "GamesList.h"

#include <cstdlib>
#ifdef ESP32P4
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#endif
#include "TheTools.hpp"
#include "TheDisplay.hpp"
#include "TheSdCard.hpp"
#include "machines/MachineInputPort.hpp"
#include <string>

class TheGame
{
public:
    TheGame();
    virtual ~TheGame();
    virtual void Setup(TheDisplay &display, TheSdCard &sdCard);
    virtual void Loop(TheDisplay &display);
    bool IsReady();

    virtual void KeyChange(uint8_t button);
    void Exit(uint8_t next);

    MachineInputPort machineInputPort;

protected:
    bool isReady;
    uint8_t gameIndex;

private:
    const char *TAG = "TheGame";
    TheDisplay *display;
    uint32_t crc32_table[256];
    void InitCrc32Table();
    uint32_t GetCrc32(uint16_t offset, uint16_t length, uint8_t *fromMemory);
    int16_t countInportPortReadFunction;
    int16_t countInportPortWriteFunction;
    int16_t countMemoryReadFunction;
    int16_t countMemoryWriteFunction;
    bool Initialize(TheDisplay &display, TheSdCard &sdCard);
    bool LoadOneRom(TheSdCard &sdCard, std::string folder, std::string filename, uint8_t *toMemory, uint16_t size, uint16_t offset = 0, uint32_t expectedCrc = 0);
    bool DecodeColors(TheDisplay &display);
    bool DecodePalette();
    bool GeneratePalette(TheDisplay &display);
    bool DecodeAllGfx(const GfxDecodeInfo info[]);
    struct GfxElement *DecodeGfxElement(const uint8_t *fromMemory, uint32_t offset, const struct GfxLayout *gfxLayout);
    void DecodeElement(struct GfxElement *element, uint16_t index, const uint8_t *fromMemory, uint32_t offset, const struct GfxLayout *gfxLayout);
    struct osd_bitmap *osd_new_bitmap(int width, int height, int depth);
    void osd_clearbitmap(struct osd_bitmap *bitmap);
};

#endif