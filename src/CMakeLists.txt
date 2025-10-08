
idf_component_register(SRCS  "main.cpp"
"TheDisplay.cpp" "TheGame.cpp" "TheMemory.c" "TheSdCard.cpp" "TheTools.cpp" 
"CpuI8085.c" "CpuZ80.c"
"MachineDriver8080bw.cpp" "MachineI8080device.c" "MachineInputPort.cpp" "MachineTheZ80.cpp"
"GameCrush.cpp"
"GameFrogger.cpp"
"GameInvaders.cpp"
"GameInvadpt2.cpp"
"GamePacman.c"
"GamePacman.cpp"
"esp_lcd_gsl3680.c" "gsl_point_id.c" "gsl3680_touch.cpp"
PRIV_REQUIRES spi_flash fatfs sd_card sdmmc
INCLUDE_DIRS "" ".")
