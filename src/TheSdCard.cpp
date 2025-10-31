#include "TheSdCard.hpp"
#ifdef ESP32P4
#else
#include <string>
#endif

// *******************************************************************
// pure "C" code to load JPEG file

static FILE *myfile;
JPEGDEC myJpeg;

static void *myOpen(const char *filename, int32_t *size)
{
    printf("file open: %s, ", filename);
    myfile = (FILE *)fopen(filename, "rb");
    if (myfile == NULL)
    {
        printf(" ERROR opening file\n");
        return NULL;
    }
    fseek(myfile, 0, SEEK_END);
    *size = ftell(myfile);
    rewind(myfile);
    printf("size: %lu\n", *size);
    return &myfile;
}

static void myClose(void *handle)
{
    printf("file: close\n");
    fclose(myfile);
}

static int32_t myReadJPEG(JPEGFILE *handle, uint8_t *buffer, int32_t length)
{
    // printf("file must read: %d", length);
    size_t x = fread(buffer, 1, length, myfile);
    // printf(" and read: %d\n", x);
    return x;
}

static int32_t mySeekJPEG(JPEGFILE *handle, int32_t position)
{
    // printf("file seek: %d\n", position);
    return fseek(myfile, 0, position);
}

uint32_t tempWidth = 0;

int JPEGDrawToUser(JPEGDRAW *pDraw)
{
    // printf("jpeg draw: x,y=%d / %d, cx,cy = %d / %d\n", pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);
    uint32_t index = 0;
    for (uint16_t y = pDraw->y; y < pDraw->y + pDraw->iHeight; y++)
    {
        for (uint16_t x = pDraw->x; x < pDraw->x + pDraw->iWidth; x++)
        {
            uint32_t pos = y * tempWidth + x;
            THE_BACKGROUND_COLOR *p = static_cast<THE_BACKGROUND_COLOR *>(pDraw->pUser);
            p[pos] = pDraw->pPixels[index++];
        }
    }
    return 1;
}

// *******************************************************************
// *******************************************************************
// *******************************************************************

TheSdCard::TheSdCard() { isOk = false; }

// *******************************************************************

TheSdCard::~TheSdCard()
{
    MY_DEBUG(TAG, "destroyed")
    ClearBackground();
}

// *******************************************************************

void TheSdCard::Setup()
{
#ifdef ESP32P4
    ESP_LOGI(TAG, "Mounting SD card...");
    const esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 64 * 1024,
        .use_one_fat = false};
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.slot = SDMMC_HOST_SLOT_0;
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;

    sd_pwr_ctrl_ldo_config_t ldo_config = {
        .ldo_chan_id = 4,
    };
    sd_pwr_ctrl_handle_t pwr_ctrl_handle = NULL;
    ESP_LOGI(TAG, "sd_pwr_ctrl_new_on_chip_ldo");
    esp_err_t ret = sd_pwr_ctrl_new_on_chip_ldo(&ldo_config, &pwr_ctrl_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to create a new on-chip LDO power control driver");
        return;
    }
    host.pwr_ctrl_handle = pwr_ctrl_handle;
    const sdmmc_slot_config_t slot_config = {
        /* SD card is connected to Slot 0 pins. Slot 0 uses IO MUX, so not specifying the pins here */
        .cd = SDMMC_SLOT_NO_CD,
        .wp = SDMMC_SLOT_NO_WP,
        .width = 4,
        .flags = 0, // SDMMC_SLOT_FLAG_INTERNAL_PULLUP,
    };
    ESP_LOGI(TAG, "esp_vfs_fat_sdmmc_mount");
    ret = esp_vfs_fat_sdmmc_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &bsp_sdcard);
    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                          "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                          "Make sure SD card lines have pull-up resistors in place.",
                     esp_err_to_name(ret));
        }
        return;
    }
    ESP_LOGI(TAG, "Filesystem mounted");
    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, bsp_sdcard);
#ifdef DEBUG_LIST_DIR
    list_directory(MOUNT_POINT);
#endif
#endif
    isOk = true;
}

// *******************************************************************

void TheSdCard::ClearBackground()
{
    if (screenBackgroundWidth + screenBackgroundHeight > 0)
    {
        screenBackgroundWidth = 0;
        screenBackgroundHeight = 0;
        screenBackgroundMemorySize = 0;
        FREE(screenBackground);
    }
}

// *******************************************************************

#ifdef ESP32P4
#ifdef DEBUG_LIST_DIR
void TheSdCard::list_directory(const char *path)
{
    DIR *dir = opendir(path);
    if (!dir)
    {
        printf("Failed to open directory: %s\n", path);
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        printf("Name: %s ", entry->d_name);
        if (entry->d_type == DT_DIR)
        {
            printf("(Directory)\n");
        }
        else
        {
            printf("(File)\n");
        }
    }
    closedir(dir);
}
#endif
#endif

// *******************************************************************

bool TheSdCard::IsOk() { return isOk; }

// *******************************************************************

bool TheSdCard::LoadFile(std::string filename, unsigned char *toMemory, uint64_t size, uint64_t offset)
{
    if (!isOk)
    {
        MY_DEBUG(TAG, "SdCard not ready");
        return false;
    }
    if (toMemory == NULL || size == 0)
    {
        MY_DEBUG(TAG, "Invalid toMemory or size");
        return false;
    }
#ifdef ESP32P4
    std::string file_foo = std::string(MOUNT_POINT) + std::string("/") + filename;
    ESP_LOGI(TAG, "Loading file: %s", file_foo.c_str());
    FILE *file = fopen(file_foo.c_str(), "r");
    if (file == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return false;
    }
    uint8_t buffer[128];
    size_t bytes_read = fread(buffer, 1, sizeof(buffer), file);
    uint64_t index = 0;
    while (bytes_read > 0)
    {
        for (size_t i = 0; i < bytes_read; i++)
        {
            toMemory[index + i + offset] = buffer[i];
        }
        index += bytes_read;
        bytes_read = fread(buffer, 1, sizeof(buffer), file);
    }
    fclose(file);
#else
    std::string fullPath = std::string(PC_PATH) + "sdcard/" + filename;
    MY_DEBUG2TEXT(TAG, "Loading file... ", fullPath.c_str());
    const char *fname = fullPath.c_str();
    FILE *pFile = fopen(fname, "rb");
    if (pFile == NULL)
    {
        MY_DEBUG2TEXT(TAG, "File not found: ", filename.c_str());
        return false;
    }
    fseek(pFile, 0, SEEK_END);
    uint64_t lSize = ftell(pFile);
    rewind(pFile);
    if (size != lSize)
    {
        // MY_DEBUG(TAG, "File size mismatch: " + std::to_string(size) + " != " + std::to_string(lSize));
        fclose(pFile);
        return false;
    }
    unsigned char *buffer = (unsigned char *)malloc(sizeof(char) * lSize);
    if (buffer == NULL)
    {
        MY_DEBUG(TAG, "Memory malloc error");
        fclose(pFile);
        return false;
    }
    size_t result = fread(buffer, 1, lSize, pFile);
    fclose(pFile);
    if (result != lSize)
    {
        MY_DEBUG(TAG, "File reading error");
        FREE(buffer);
        return false;
    }
    MY_DEBUG2(TAG, "Transfert to:", offset);
    for (uint64_t i = 0; i < lSize; ++i)
    {
        toMemory[i + offset] = buffer[i];
    }
    FREE(buffer);
#endif
    return true;
}

// *******************************************************************

bool TheSdCard::LoadJpgFileToBackground(const char *filename)
{
    ClearBackground();
    screenBackgroundMemorySize = SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(THE_BACKGROUND_COLOR);
    screenBackground = (THE_BACKGROUND_COLOR *)malloc(screenBackgroundMemorySize);
    if (screenBackground == NULL)
    {
        MY_DEBUG(TAG, "Not enought memory to create background")
        screenBackgroundMemorySize = 0;
        return false;
    }
    screenBackgroundWidth = SCREEN_WIDTH;
    screenBackgroundHeight = SCREEN_HEIGHT;
    MY_DEBUG(TAG, "Cleaning memory background")
    memset(screenBackground, 0xFFFF, screenBackgroundMemorySize);
    return LoadJpgFileTo(screenBackground, filename, SCREEN_WIDTH);
}

// *******************************************************************

bool TheSdCard::LoadJpgFileTo(THE_BACKGROUND_COLOR *image, const char *filename, uint32_t lineWidth)
{
    const char *fname;
    std::string fullPath = std::string(MOUNT_POINT) + filename;
    fname = fullPath.c_str();
    MY_DEBUG2TEXT(TAG, "Loading JPG file:", fname);
    int rc = myJpeg.open(fname, myOpen, myClose, myReadJPEG, mySeekJPEG, JPEGDrawToUser);
    if (rc == JPEG_SUCCESS)
    {
        printf("Error opening JPEG\n");
        return false;
    }
    tempWidth = lineWidth;
    uint32_t size = myJpeg.getWidth() * myJpeg.getHeight() * sizeof(THE_BACKGROUND_COLOR);
    // printf("image specs: (%d x %d), %d bpp, pixel type: %d, memorySize: %lu\n", myJpeg.getWidth(), myJpeg.getHeight(), myJpeg.getBpp(), myJpeg.getPixelType(), size);
    memset(image, 0xFFFF, size);
    myJpeg.setUserPointer(image);
    rc = myJpeg.decode(0, 0, 0);
    if (rc == 0)
    {
        MY_DEBUG2(TAG, "JPEG decode failed with error code:", rc);
        myJpeg.close();
        return false;
    }
    myJpeg.close();
    return true;
}

#ifndef ESP32P4
FILE *TheSdCard::CreateJsonFileOnWiki(const char *filename)
{
    std::string fullPath = std::string(PC_PATH) + "wiki/" + filename + ".json";
    MY_DEBUG2TEXT(TAG, "Creating file... ", fullPath.c_str());
    const char *fname = fullPath.c_str();
    remove(fname);
    FILE *pFile = fopen(fname, "wb");
    if (pFile == NULL)
    {
        MY_DEBUG2TEXT(TAG, "Could not create file: ", fullPath.c_str());
        return NULL;
    }
    return pFile;
}

void TheSdCard::CloseFile(FILE *file)
{
    fclose(file);
}

void TheSdCard::FileWriteText(FILE *file, const char *text)
{
    fprintf(file, "%s", text);
}
#endif