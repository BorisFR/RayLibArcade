#ifndef THE_MENU_H
#define THE_MENU_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif
#define THE_MENU {"themenu", ">> Menu <<", {200, 500, 7 * 140, {0, 0, 500, 7 * 140}, ORIENTATION_DEFAULT, NOTHING, NOTHING}, NOTHING, {themenu_rom, NOTHING, themenu_readmem, themenu_writemem, themenu_input_ports, NOTHING, NOTHING}, MACHINE_THEMENU}

ROM_START(themenu_rom)
ROM_REGION(0x01)
ROM_END

static struct MemoryReadAddress themenu_readmem[] = {
    {-1}};

static struct MemoryWriteAddress themenu_writemem[] = {
    {-1}};

INPUT_PORTS_START(themenu_input_ports)
PORT_START
PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP)
PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT)
PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT)
PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN)
PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_START1)
INPUT_PORTS_END

#endif
