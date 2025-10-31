
#include "MyDefine.h"

const char *TAG = "Main";

#include "TheSdCard.hpp"
TheSdCard sdCard = TheSdCard();
#include "TheDisplay.hpp"
TheDisplay display = TheDisplay();
#include "TheMenu.hpp" // TheMenu is working as a game
#ifdef MACHINE_8080BW
#include "machines/MachineDriver8080bw.hpp"
#endif
#ifdef MACHINE_Z80
#include "machines/MachineTheZ80.hpp"
#endif
#ifdef MACHINE_xxx
#include "machines/MachineDriverxxx.hpp"
#endif
TheGame *game;

#ifndef ESP32P4
#include "export/Export.hpp"
#endif

// *******************************************************************

//
// Start a game
//
void StartGame()
{
#ifdef ESP32P4
#else
  display.ChangeTitle(GAME_NAME);
#endif
  switch (GAME_MACHINE)
  {
  case MACHINE_THEMENU:
    game = new TheMenu();
    break;
#ifdef MACHINE_8080BW
  case MACHINE_8080BW:
    game = new MachineDriver8080bw();
    break;
#endif
#ifdef MACHINE_Z80
  case MACHINE_Z80:
    game = new MachineTheZ80();
    break;
#endif
  }
  game->Setup(display, sdCard);
  if (!game->IsReady())
  {
    MY_DEBUG(TAG, "Error preparing game")
  }
  uint32_t zoomFactor = MIN(display.GetMaxZoomX(), display.GetMaxZoomY());
  //zoomFactor = 1;
  if (zoomFactor == 0)
    MY_DEBUG2TEXT(TAG, "*** ERROR ***", "Zoom is 0")
  // zoomFactor=1;
  display.SetDisplayForGame(zoomFactor, zoomFactor, display.GetPaddingLeftForZoom(zoomFactor), display.GetPaddingTopForZoom(zoomFactor));

  display.FillScreen(display.Rgb888ToRgb565(255, 80, 0));
  std::string temp = "/_background/" + std::string(GAME_FOLDER) + ".jpg";
  bool bgOk = sdCard.LoadJpgFileToBackground(temp.c_str());
  if (bgOk)
  {
    display.DisplayPng(0, 0);
  }
  else
  {
    display.CreateBackground();
    // display.FillScreen(myBlack);
  }
  display.SetVerticalPositionForGame(allGames[currentGame].video.top);

  // std::string temp = "background/" + std::string(GAME_FOLDER) + ".jpg";
  // bool bgOk = sdCard.LoadJpgFile(temp.c_str());
  // if (bgOk)
  //   display.DisplayPng(0, 0);
}

// *******************************************************************

void setup()
{
  MY_DEBUG(TAG, "*** ESP Arcade");
  display.Setup();
  sdCard.Setup();
  bool finish = false;
  while (!finish)
  {
    if (allGames[countGames].machineType == -1)
    {
      finish = true;
      continue;
    }
    MY_DEBUG2TEXT(TAG, "Available:", allGames[countGames].name);
    countGames++;
  }
  if (countGames == 0)
  {
    MY_DEBUG(TAG, "There is no game!")
    return;
  }

#ifndef ESP32P4
  Export ex = Export();
  ex.GenerateAll(sdCard);
  ex.~Export();
#endif

  currentGame = 4; // 0 menu, 1 si, ... 4 pacman
  StartGame();
}

// *******************************************************************

void loop()
{
  if (game->IsReady())
  {
    game->Loop(display);
  }
  display.Loop();
  if (display.Clicked())
  {
    keyPressed[BUTTON_START] = true;
    game->KeyChange(BUTTON_START);
  }
  for (uint8_t k = 0; k < BUTTON_END; k++)
  {
    if (IsKeyChanged(k))
      game->KeyChange(k);
  }
  if (exitGame)
  {
    MY_DEBUG2(TAG, "Change game to", nextGame)
    delete game;
    currentGame = nextGame;
    StartGame();
  }
}

// *******************************************************************

#ifdef ESP32P4
extern "C" void app_main(void)
{
  // printf("*** EspArcade\n");
  /* Print chip information */
  /*
  esp_chip_info_t chip_info;
  uint32_t flash_size;
  esp_chip_info(&chip_info);
  printf("This is %s chip with %d CPU core(s), %s%s%s%s, ",
         CONFIG_IDF_TARGET,
         chip_info.cores,
         (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
         (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
         (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
         (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");
  unsigned major_rev = chip_info.revision / 100;
  unsigned minor_rev = chip_info.revision % 100;
  printf("silicon revision v%d.%d, ", major_rev, minor_rev);
  if (esp_flash_get_size(NULL, &flash_size) != ESP_OK)
  {
    printf("Get flash size failed");
    return;
  }
  printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
         (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
  printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());
  fflush(stdout);
*/
  ESP_LOGI(TAG, "Free heap size: %lu", esp_get_free_heap_size());
  ESP_LOGI(TAG, "Free heap size (caps): %u", heap_caps_get_free_size(MALLOC_CAP_8BIT));
  ESP_LOGI(TAG, "Free heap size (caps) INTERNAL: %u", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
  ESP_LOGI(TAG, "Free heap size (caps) DMA: %u", heap_caps_get_free_size(MALLOC_CAP_DMA));
  ESP_LOGI(TAG, "Free heap size (caps) SPIRAM: %u", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
  setup();
  ESP_LOGI(TAG, "Free heap size: %lu", esp_get_free_heap_size());
  ESP_LOGI(TAG, "Free heap size (caps): %u", heap_caps_get_free_size(MALLOC_CAP_8BIT));
  ESP_LOGI(TAG, "Free heap size (caps) INTERNAL: %u", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
  ESP_LOGI(TAG, "Free heap size (caps) DMA: %u", heap_caps_get_free_size(MALLOC_CAP_DMA));
  ESP_LOGI(TAG, "Free heap size (caps) SPIRAM: %u", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
  while (true)
  {
    loop();
    //vTaskDelay(1 / portTICK_PERIOD_MS);
  }
  delete game;
}
#else
int main()
{
  setup();
  while (!display.MustExit())
  {
    loop();
  }
  delete game;
  return 0;
}
#endif
