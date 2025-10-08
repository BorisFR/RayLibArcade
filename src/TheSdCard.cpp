#include "TheSdCard.hpp"
#ifdef ESP32P4
#else
#include <string>
#endif

// *******************************************************************

TheSdCard::TheSdCard() { isOk = false; }

// *******************************************************************

TheSdCard::~TheSdCard() {}

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
    // ESP_LOGI(TAG, "Loading file: %s", filename.c_str());
    // MY_DEBUG(TAG, "Loading file: " + filename);
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
    std::string fullPath = std::string(PC_PATH) + "roms/" + filename;
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
        //MY_DEBUG(TAG, "File size mismatch: " + std::to_string(size) + " != " + std::to_string(lSize));
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
        free(buffer);
        return false;
    }
    MY_DEBUG2(TAG, "Transfert to:", offset);
    for (uint64_t i = 0; i < lSize; ++i)
    {
        toMemory[i + offset] = buffer[i];
    }
    free(buffer);
#endif
    return true;
}

// *******************************************************************

bool TheSdCard::LoadPngFile(const char *filename)
{
#ifdef ESP32P4
// png.setBuffer(NULL);
// int rc = png.open(filename, cbPngOpen, cbPngClose, cbPngRead, cbPngSeek, cbPngData);
// if (rc != PNG_SUCCESS)
//{
//     ESP_LOGI(TAG, "Failed to open PNG file: %s", filename);
//     png.close();
//     return false;
// }
// pngMemorySize = png.getWidth() * png.getHeight(); // * sizeof(PNG_PTR_TYPE);
// ESP_LOGI(TAG, "image specs: (%d x %d), %d bpp, pixel type: %d, memorySize: %d", png.getWidth(), png.getHeight(), png.getBpp(), png.getPixelType(), pngMemorySize);
// pngImage = (PNG_PTR_TYPE *)malloc(pngMemorySize);
// if (pngImage == NULL)
//{
//     ESP_LOGI(TAG, "Not enough memory for loading png");
//     png.close();
//     return false;
// }
//// ESP_LOGI(TAG, "Malloc ok for loading png");
//// rc = png.decode(NULL, 0);
// rc = png.decode(pngImage, 0);
// if (rc != 0)
//{
//     Serial.printf("PNG decode failed with error code: %d\n", rc);
//     png.close();
//     return false;
// }
// png.close();
#endif
    return true;
}