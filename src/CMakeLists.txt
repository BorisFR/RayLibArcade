
idf_component_register(SRCS  "main.cpp"

"TheDisplay.cpp" "TheGame.cpp" "TheMemory.c" "TheSdCard.cpp" "TheTools.cpp" 
"GamesList.c" "MyDefine.c"
"esp_lcd_gsl3680.c" "gsl_point_id.c" "gsl3680_touch.cpp"
"gsl_point_id.c" "gsl3680_touch.cpp"

"machines/CpuI8085.c" "machines/CpuZ80.c"
"machines/i8080.cpp" "machines/i8080instructions.cpp"
"machines/MachineDriver8080bw.cpp" "machines/MachineInputPort.cpp" "machines/MachineTheZ80.cpp"
"machines/mb14241.c"

"games/Frogger.c"
"games/Invaders.c"
"games/Invadpt2.c"
"games/Pacman.c"

PRIV_REQUIRES spi_flash fatfs sd_card sdmmc
INCLUDE_DIRS "" ".")
