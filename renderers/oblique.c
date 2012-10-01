#include <png.h>
#include <stdlib.h>

#include "level.h"
#include "chunk.h"
#include "cache.h"
#include "nbt.h"
#include "colors.h"
#include "renderer.h"
#include "renderers/oblique.h"

renderer *renderer_oblique_new(level *lvl, color_map *map, char *output_path)
{
    renderer_funcs *funcs = NULL;
    chunk_cache *cache = NULL;

    if (lvl == NULL || map == NULL)
        return NULL;

    funcs = calloc(1, sizeof(renderer_funcs));
    if (funcs == NULL)
        return NULL;

    cache = (chunk_cache*)cache_slab_new(RENDERER_OBLIQUE_CACHE_SIZE);
    if (cache == NULL)
    {
        free(funcs);
        return NULL;
    }

    funcs->dimensions = renderer_oblique_dimensions;
    funcs->draw_row = renderer_oblique_draw_row;

    return renderer_new(lvl, map, funcs, cache);
}

int renderer_oblique_dimensions(int *width, int *height)
{
    *width = RENDERER_OBLIQUE_IMAGE_WIDTH;
    *height = RENDERER_OBLIQUE_IMAGE_HEIGHT;

    return 0;
}

void renderer_oblique_draw_row(void *_r, png_bytep buffer, int row_number)
{
    int row, column; 

    // Relative chunk coordinates inside a tile
    int chunk_x, chunk_z, chunk_z_start;

    // relative block coordinates inside a chunk
    int local_x, local_z;

    renderer *r = (renderer*)_r;
    chunk *current = NULL;

    if (r == NULL || r->map == NULL)
        return;

    // For this row, determine where the starting chunk's Z coordinate is.
    if (row_number < 256)
    {
        chunk_z_start = row_number / 16;
    }
    else
    {
        chunk_z_start = 15;
    }

    for (column = 0; column < RENDERER_OBLIQUE_IMAGE_WIDTH; column++)
    {
        chunk_x = column / 16;
        chunk_z = chunk_z_start;

        while (chunk_z >= 0)
        {
            current = renderer_oblique_get_chunk(r, chunk_x, chunk_z);
            if (current != NULL)
            {
            }

            chunk_z--;
        }
    }
}

chunk *renderer_oblique_get_chunk(renderer *r, int32_t coord_x, int32_t coord_z)
{
    chunk *c;
    int absolute_x, absolute_y, key;

    if (r == NULL)
        return NULL;

    key = coord_x + CHUNK_SIZE_Z * coord_z;

    if (cache_get(r->cache, key, &c))
    {
        absolute_x = r->tile_x * RENDERER_TILE_SIZE + coord_x;
        absolute_z = r->tile_z * RENDERER_TILE_SIZE + coord_z;

        c = level_get_chunk_at(r->lvl, absolute_x, absolute_z);

        cache_set(r->cache, key, c);
    }

    return c;
}
