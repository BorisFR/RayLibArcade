#include "GamesList.h"


    const struct OneGame allGames[] = {
#ifdef MACHINE_8080BW
        INVADERS,
        INVADPT2,
#endif
#ifdef MACHINE_Z80
        FROGGER,
        PACMAN,
        //CRUSH,
#endif
#ifdef MACHINE_xxx
        {"folder", "Name", \
            {width, height, visiblearea, orientation, drawscreen, decodegfx}, \
            frequency, \
            {roms, decode, read, write, inport, readport, outport}, MACHINE_xxx}},
#endif
        {"", "", {0, 0, VISIBLE_AREA_FULL, ORIENTATION_DEFAULT, NOTHING, NOTHING}, NOTHING, {NOTHING, NOTHING, NOTHING, NOTHING, NOTHING, NOTHING, NOTHING}, -1}};

