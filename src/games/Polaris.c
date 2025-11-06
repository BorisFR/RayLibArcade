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
    // if (videoram[offset] == data)
    //     return;
    videoram[offset] = data;
    /*uint16_t x = offset / 32;
    uint16_t y = screenGameHeight - 8 * (offset % 32);

    for (uint8_t bit = 0; bit < 8; bit++)
    {
        if (!(data & 0x01))
            GameClearPixel(x, y - bit - 1);
        else
            GamePlotPixel(x, y - bit - 1, myWhite);
        data >>= 1;
    }*/
}

uint8_t *m_scattered_colorram;

READ_HANDLER(polaris_scattered_colorram_r)
{
    return m_scattered_colorram[(offset & 0x1f) | ((offset & 0x1f80) >> 2)];
}

WRITE_HANDLER(polaris_scattered_colorram_w)
{
    m_scattered_colorram[(offset & 0x1f) | ((offset & 0x1f80) >> 2)] = data;
}

// taken from mame-master/src/lib/util/palette.h
uint8_t palexpand(uint8_t num, uint8_t bits)
{
    if (num == 1)
    {
        return (bits & 1) ? 0xff : 0x00;
    }
    if (num == 2)
    {
        bits &= 3;
        return (bits << 6) | (bits << 4) | (bits << 2) | bits;
    }
    if (num == 3)
    {
        bits &= 7;
        return (bits << 5) | (bits << 2) | (bits >> 1);
    }
    if (num == 4)
    {
        bits &= 0xf;
        return (bits << 4) | bits;
    }
    if (num == 5)
    {
        bits &= 0x1f;
        return (bits << 3) | (bits >> 2);
    }
    if (num == 6)
    {
        bits &= 0x3f;
        return (bits << 2) | (bits >> 4);
    }
    if (num == 7)
    {
        bits &= 0x7f;
        return (bits << 1) | (bits >> 6);
    }
    return bits;
}

uint8_t pal1bit(uint8_t bits) { return palexpand(1, bits); }
uint8_t pal2bit(uint8_t bits) { return palexpand(2, bits); }
uint8_t pal3bit(uint8_t bits) { return palexpand(3, bits); }
uint8_t pal4bit(uint8_t bits) { return palexpand(4, bits); }
uint8_t pal5bit(uint8_t bits) { return palexpand(5, bits); }
uint8_t pal6bit(uint8_t bits) { return palexpand(6, bits); }
uint8_t pal7bit(uint8_t bits) { return palexpand(7, bits); }

uint16_t rgb_t(uint8_t red, uint8_t green, uint8_t blue)
{
    uint16_t r = (red >> 3) & 0x1F;   // 5 bits for red
    uint16_t g = (green >> 2) & 0x3F; // 6 bits for green
    uint16_t b = (blue >> 3) & 0x1F;  // 5 bits for blue
    // Combine into a single 16-bit value
    return (r << 11) | (g << 5) | b;
    // 32bits (with alpha) return (255 << 24) | (red << 16) | (green << 8) | blue;
}

void PolarisInit()
{
    m_scattered_colorram = (uint8_t *)malloc(0x800);
    memset(m_scattered_colorram, 0, 0x800);
    // from mame-master/src/emu/emupal.cpp
    // for (int i = 0; i < 8; i++)
    //	palette.set_pen_color(i, rgb_t(pal1bit(i >> 0), pal1bit(i >> 2), pal1bit(i >> 1)));
    /*
    for (uint16_t i = 0; i < 0x0400; i++)
    {
        uint8_t value = colorMemory[i];
        colorRGB[i] = rgb_t(pal1bit(value >> 0), pal1bit(value >> 2), pal1bit(value >> 1));
    }
    colorRGB[0]= myWhite;
    colorRGB[1]= myYellow;
    colorRGB[2]= myRed;
    colorRGB[3]= myGreen;
    colorRGB[4]= myGreen;
    colorRGB[5]= myPurple;
    for (uint16_t i = 6; i < 0x0400; i++){
        colorRGB[i]=colorRGB[i-6];
    }*/
}

#define offs_t uint32_t
#define BIT(value, pos) (((value) >> (pos)) & 0x1)

void clear_pixel(uint16_t y, uint16_t x)
{
    GameClearPixel(y, screenGameHeight - x - 1);
}

void set_pixel(uint16_t y, uint16_t x, THE_COLOR color)
{
    GamePlotPixel(y, screenGameHeight - x - 1, color);
}

void set_8_pixels(uint16_t y, uint16_t x, uint8_t data, THE_COLOR fore_color) //, THE_COLOR back_color)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        // set_pixel(y, x, BIT(data, 0) ? fore_color : back_color);
        if (BIT(data, 0))
            set_pixel(y, x, fore_color);
        else
            clear_pixel(y, x);
        x += 1;
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
    // for (offs_t offs = 0; offs < m_main_ram.bytes(); offs++)
    for (offs_t offs = 0; offs < videoram_size; offs++)
    {
        uint8_t y = offs >> 5;
        uint8_t x = offs << 3;
        uint8_t data = videoram[offs];
        offs_t color_address = (offs >> 8 << 5) | (offs & 0x1f);
        /* for the background color, bit 0 of the map PROM is connected to green gun.
           red is 0 and blue is 1, giving cyan and blue for the background.  This
           is different from what the schematics shows, but it's supported
           by screenshots.  Bit 3 is connected to cloud enable, while
           bits 1 and 2 are marked 'not use' (sic) */

        // THE_COLOR back_color = (colorMemory[color_address] & 0x01) ? myRed : myYellow; // 6 : 2;
        uint8_t base_color = m_scattered_colorram[(offs & 0x1f) | ((offs & 0x1f80) >> 2)] & 0x07;
        THE_COLOR fore_color = myWhite;//base_color;//colorRGB[base_color];                                        // myRed;
        uint8_t cloud_y = y - m_polaris_cloud_pos;
        if ((colorMemory[color_address] & 0x08) || (cloud_y >= 64))
        {
            set_8_pixels(y, x, data, fore_color); //, back_color);
        }
        else
        {
            /* cloud appears in this part of the screen */
            for (uint8_t i = 0; i < 8; i++)
            {
                // THE_COLOR color;
                if (data & 0x01)
                {
                    // color = fore_color;
                    set_pixel(y, x, fore_color);
                }
                else
                {
                    int bit = 1 << (~x & 0x03);
                    offs_t cloud_gfx_offs = ((x >> 2) & 0x03) | ((~cloud_y & 0x3f) << 2);
                    // color = (user1Memory[cloud_gfx_offs] & bit) ? 7 : back_color;
                    if (user1Memory[cloud_gfx_offs] & bit)
                        set_pixel(y, x, myWhite);
                    else
                        clear_pixel(y, x);
                }
                // set_pixel(y, x, color);
                x = x + 1;
                data = data >> 1;
            }
        }
    }
}