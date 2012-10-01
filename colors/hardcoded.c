
#include <stdint.h>
#include <stdlib.h>
#include <png.h>

#include "colors.h"
#include "colors/hardcoded.h"

color_map *color_map_hardcoded_new(uint16_t block_count, uint8_t color_depth)
{
    png_byte *colors;
    color_map *new;

    colors = calloc(block_count, color_depth);
    if (colors == NULL)
        return NULL;

    color_map_hardcoded_assign(colors, 0,255,255,255,0);
    color_map_hardcoded_assign(colors, 1,120,120,120,255);
    color_map_hardcoded_assign(colors, 2,117,176,73,255);
    color_map_hardcoded_assign(colors, 3,134,96,67,255);
    color_map_hardcoded_assign(colors, 4,115,115,115,255);
    color_map_hardcoded_assign(colors, 48,115,115,115,255);
    color_map_hardcoded_assign(colors, 5,157,128,79,255);
    color_map_hardcoded_assign(colors, 6,120,120,120,0);
    color_map_hardcoded_assign(colors, 7,84,84,84,255);
    //color_map_hardcoded_assign(colors, 8,38,92,255,51);
    //color_map_hardcoded_assign(colors, 9,38,92,255,51);

    color_map_hardcoded_assign(colors, 8,38,92,255,128);
    color_map_hardcoded_assign(colors, 9,38,92,255,128);

    color_map_hardcoded_assign(colors, 10,255,90,0,255);
    color_map_hardcoded_assign(colors, 11,255,90,0,255);
    color_map_hardcoded_assign(colors, 12,218,210,158,255);
    color_map_hardcoded_assign(colors, 13,136,126,126,255);
    color_map_hardcoded_assign(colors, 14,143,140,125,255);
    color_map_hardcoded_assign(colors, 15,136,130,127,255);
    color_map_hardcoded_assign(colors, 16,115,115,115,255);
    color_map_hardcoded_assign(colors, 17,102,81,51,255);
    color_map_hardcoded_assign(colors, 18,60,192,41,100);
    color_map_hardcoded_assign(colors, 20,255,255,255,64); //glass
    color_map_hardcoded_assign(colors, 35,222,222,222,255); //Color(143,143,143,255); 
    color_map_hardcoded_assign(colors, 37,255,0,0,255);
    color_map_hardcoded_assign(colors, 38,255,255,0,0);
    color_map_hardcoded_assign(colors, 41,232,245,46,255);
    color_map_hardcoded_assign(colors, 42,191,191,191,255);
    color_map_hardcoded_assign(colors, 43,200,200,200,255);
    color_map_hardcoded_assign(colors, 44,200,200,200,255);
    color_map_hardcoded_assign(colors, 45,170,86,62,255);
    color_map_hardcoded_assign(colors, 46,160,83,65,255);
    color_map_hardcoded_assign(colors, 49,26,11,43,255);
    color_map_hardcoded_assign(colors, 50,245,220,50,200);
    color_map_hardcoded_assign(colors, 51,255,170,30,200);
    color_map_hardcoded_assign(colors, 53,157,128,79,255);
    color_map_hardcoded_assign(colors, 54,125,91,38,255);
    color_map_hardcoded_assign(colors, 56,129,140,143,255);
    color_map_hardcoded_assign(colors, 57,45,166,152,255);
    color_map_hardcoded_assign(colors, 58,114,88,56,255);
    color_map_hardcoded_assign(colors, 59,146,192,0,255);
    color_map_hardcoded_assign(colors, 60,95,58,30,255);
    color_map_hardcoded_assign(colors, 61,96,96,96,255);
    color_map_hardcoded_assign(colors, 62,96,96,96,255);
    color_map_hardcoded_assign(colors, 63,111,91,54,255);
    color_map_hardcoded_assign(colors, 64,136,109,67,255);
    color_map_hardcoded_assign(colors, 65,181,140,64,32);
    color_map_hardcoded_assign(colors, 66,150,134,102,180);
    color_map_hardcoded_assign(colors, 67,115,115,115,255);
    color_map_hardcoded_assign(colors, 71,191,191,191,255);
    color_map_hardcoded_assign(colors, 73,131,107,107,255);
    color_map_hardcoded_assign(colors, 74,131,107,107,255);
    color_map_hardcoded_assign(colors, 75,181,140,64,32);
    color_map_hardcoded_assign(colors, 76,255,0,0,200);
    color_map_hardcoded_assign(colors, 78,255,255,255,255);
    color_map_hardcoded_assign(colors, 79,83,113,163,51);
    color_map_hardcoded_assign(colors, 80,250,250,250,255);
    color_map_hardcoded_assign(colors, 81,25,120,25,255);
    color_map_hardcoded_assign(colors, 82,151,157,169,255);
    color_map_hardcoded_assign(colors, 83,193,234,150,255);

    color_map_hardcoded_assign(colors, 84,134,96,67,255);
    color_map_hardcoded_assign(colors, 85,134,96,67,255);

    return color_map_new(colors, block_count, color_depth, color_map_hardcoded_get);
}

png_byte *color_map_hardcoded_get(void *_map, uint16_t block_type)
{
    color_map *map = (color_map*)_map;

    return (png_byte*)(map->colors + (block_type * map->color_depth));
}

void color_map_hardcoded_assign(png_byte *colors, uint16_t block_type, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    uint32_t offset = block_type * 4;

    colors[offset] = red;
    colors[offset + 1] = green;
    colors[offset + 2] = blue;
    colors[offset + 3] = alpha;

}
