#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "colors.h"

color_map *color_map_new(png_byte *colors, uint16_t block_count, uint8_t color_depth, color_get_func get)
{
    color_map *new;

    new = calloc(1, sizeof(color_map));

    if (new != NULL)
    {
        new->colors = colors;
        new->block_count = block_count;
        new->color_depth = color_depth;
        new->get = get;
    }
    
    return new;
}

void color_map_free(color_map *doomed)
{
    if (doomed == NULL)
        return;

    if (doomed->colors != NULL)
        free(doomed->colors);

    free(doomed);
}


int color_map_write(color_map *map, png_byte *destination, uint16_t block_type)
{
    png_byte *color;

    if (map == NULL)
        return COLOR_MAP_ERR_NOMAP;

    if (destination == NULL)
        return COLOR_MAP_ERR_NODEST;

    if (block_type < 0 || block_type > map->block_count)
        return COLOR_MAP_ERR_BLOCK;

    color = map->get((void*)map, block_type);

    if (color == NULL)
        return COLOR_MAP_ERR_BLOCK;

    memcpy(destination, color, map->color_depth);

    return COLOR_MAP_OK;
}

png_byte *color_map_get(color_map *map, uint16_t block_type)
{
    if (map == NULL || block_type < 0 || block_type > 255)
        return NULL;
    return map->get(map, block_type);
}
