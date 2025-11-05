#include "Polaris.h"

uint8_t m_polaris_cloud_pos = 0;
uint8_t m_polaris_cloud_speed = 0;

WRITE_HANDLER(polaris_sh_port_1_w)
{
    // m_discrete->write(POLARIS_MUSIC_DATA, data);
}
WRITE_HANDLER(polaris_sh_port_2_w) {}
WRITE_HANDLER(polaris_sh_port_3_w) {}

WRITE_HANDLER(polaris_videoram_w)
{
    if (videoram[offset] == data)
        return;
    videoram[offset] = data;
    uint16_t x = offset / 32;
    uint16_t y = screenGameHeight - 8 * (offset % 32);

    for (uint8_t bit = 0; bit < 8; bit++)
    {
        if (!(data & 0x01))
            GameClearPixel(x, y - bit - 1);
        else
            GamePlotPixel(x, y - bit - 1, myWhite);
        data >>= 1;
    }
}

void PolarisRefreshScreen()
{
    m_polaris_cloud_speed++;

    if (m_polaris_cloud_speed >= 4) // every 4 frames - this was verified against real machine
    {
        m_polaris_cloud_speed = 0;
        m_polaris_cloud_pos++;
    }
}