#ifndef IMAGE_SD_HPP
#define IMAGE_SD_HPP

#pragma once

#include "MyDefine.h"

#include "TheMemory.h"

#ifdef ESP32P4
#define CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4 
#define MOUNT_POINT CONFIG_BSP_SD_MOUNT_POINT // "/sdcard"
#define PIN_SDCARD_D0 39
#define PIN_SDCARD_D1 40
#define PIN_SDCARD_D2 41
#define PIN_SDCARD_D3 42
#define PIN_SDCARD_CLK 43
#define PIN_SDCARD_CMD 44

#include <string>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "sd_test_io.h"
#if SOC_SDMMC_IO_POWER_EXTERNAL
#include "sd_pwr_ctrl_by_on_chip_ldo.h"
#endif
#include <sys/stat.h>
#include <unistd.h>
#include <sys/dirent.h>
#include <algorithm>
#else
#endif
#include <string>

#include "libjpg/JPEGDEC.h"

//#define DEBUG_LIST_DIR 1

class TheSdCard
{
public:
    TheSdCard();
    ~TheSdCard();

    void Setup();
    void ClearBackground();
    bool IsOk();
    bool LoadFile(std::string filename, unsigned char *toMemory, uint64_t size, uint64_t offset = 0);
    bool LoadJpgFileToBackground(const char *filename);
    bool LoadJpgFileTo(THE_BACKGROUND_COLOR *image, const char *filename, uint32_t lineWidth);

private:
    const char *TAG = "TheSdCard";
#ifdef ESP32P4
    sdmmc_card_t *bsp_sdcard;
#ifdef DEBUG_LIST_DIR
    void list_directory(const char *path);
#endif
#endif    
    bool isOk;
};

#endif