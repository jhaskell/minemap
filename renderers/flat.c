#include <png.h>
#include <math.h>
#include <stdlib.h>

#include "level.h"
#include "renderer.h"
#include "renderers/flat.h"
#include "chunk.h"
#include "colors.h"
#include "nbt.h"

renderer *renderer_flat_new(level *lvl, color_map *map)
{
    renderer_funcs *funcs = NULL;
    chunk_cache *cache = NULL;

    funcs = calloc(1, sizeof(renderer_funcs));
    if (funcs == NULL)
        return NULL;

    cache = (chunk_cache*)cache_slab_new(RENDERER_FLAT_CACHE_SIZE);
    if (cache == NULL)
    {
        free(funcs);
        return NULL;
    }

    funcs->dimensions = renderer_flat_dimensions;
    funcs->draw_row = renderer_flat_draw_row;

    return renderer_new(lvl, map, funcs, cache);
}

int renderer_flat_dimensions(int *width, int *height)
{
    *width = RENDERER_FLAT_IMAGE_WIDTH;
    *height = RENDERER_FLAT_IMAGE_HEIGHT;

    return 0;
}
void renderer_flat_draw_row(void *_r, png_bytep buffer, int row_number)
{
    int column, coord_y;
    uint8_t highest_block, chunk_coord_x, chunk_coord_z;
    uint8_t *slice;
    uint16_t block_type, blocks_length;
    uint32_t slice_offset;
    extern int chunk_errno;
    float gamma;

    nbt_tag *chunk_tag, *blocks_tag;
    renderer *r = (renderer*)_r;
    chunk *current = NULL;

    png_byte pixel_to_write[4];
    png_bytep pixel;

    if (r == NULL || r->map == NULL)
    {
        return;
    }

    chunk_coord_z = row_number % 16;

    // Every 16 rows, we need to load new chunks, so clobber the cache to
    // save memory
    if (chunk_coord_z == 0)
    {
        cache_empty(r->cache);
    }

    for (column = 0; column < RENDERER_FLAT_IMAGE_WIDTH; column++)
    {
        chunk_coord_x = column % 16;

        // Use the "air" block as the default pixel to write (transparent)
        color_map_write(r->map, pixel_to_write, 0);

        // Every 16 columns we need to switch to a new chunk
        // This block will also cleverly get called on the very first time
        // this loop iterates, initializing the necessary variables. :)
        if (chunk_coord_x == 0)
        {
            current = renderer_flat_get_chunk(r, floor(column / 16), floor(row_number / 16) );

            if (current != NULL)
            {
                chunk_tag = current->data;

                if (chunk_tag != NULL)
                {
                    //current->blockdata = nbt_payload(nbt_hash_search(chunk_tag, L"Data"), TAG_Byte_Array);
                    /*
                    current->skylight = nbt_payload(nbt_hash_search(chunk_tag, L"SkyLight"), TAG_Byte_Array);
                    current->heightmap = nbt_payload(nbt_hash_search(chunk_tag, L"HeightMap"), TAG_Byte_Array);
                    current->blocklight = nbt_payload(nbt_hash_search(chunk_tag, L"BlockLight"), TAG_Byte_Array);
                    blocks_tag = nbt_hash_search(chunk_tag, L"Blocks");
                    current->blocks = nbt_payload(blocks_tag, TAG_Byte_Array);
                    if (current->blocks != NULL)
                        blocks_length = blocks_tag->meta->length;
                        */
                }
            }
            // If this chunk is NULL, there's no point in looping 16 more times
            // so write out 16 blank pixels and continue.
            else
            {
                memset(buffer + column * r->map->color_depth, 0, 16 * r->map->color_depth);
                column += 15; 
                continue;
            }
        }

        if (!(current == NULL    || 
            current->heightmap == NULL    ||
            current->blocks == NULL       ||
            current->blocklight == NULL   ||
            current->skylight == NULL     
            //current->data == NULL         ||
           ))
        {
            slice_offset = chunk_generate_8bit_offset(current, chunk_coord_x, 0, chunk_coord_z, CHUNK_SIZE_AREA * current->height);
            highest_block = *(current->heightmap + (chunk_coord_z * 16 + chunk_coord_x));

            if (slice_offset != -1)
            {
                slice = current->blocks + slice_offset;

                // Drill down into the column until we hit a block with no 
                // transparency.
                for (coord_y = highest_block; coord_y > 0; coord_y--)
                {
                    block_type = *(slice + coord_y);
                    pixel = color_map_get(r->map, block_type);

                    // Array subscript 3 is the alpha value, and 255 is 100% 
                    // opacity
                    if (pixel[3] == 255)
                        break;
                }

                gamma = renderer_calc_gamma(current, chunk_coord_x, coord_y + 1, chunk_coord_z, RENDERER_FLAT_SKY_PERCENT, RENDERER_FLAT_BLOCK_PERCENT);
                memcpy(pixel_to_write, pixel, 4);
                renderer_blend_color(pixel_to_write, NULL, gamma);
                coord_y++;

                // Now go back up, blending each pixel found with blocks we 
                // find above.
                for (; coord_y <= highest_block; coord_y++)
                {
                    block_type = *(slice + coord_y);
                    pixel = color_map_get(r->map, block_type);
                    gamma = renderer_calc_gamma(current, chunk_coord_x, coord_y + 1, chunk_coord_z, RENDERER_FLAT_SKY_PERCENT, RENDERER_FLAT_BLOCK_PERCENT);
                    if (pixel[3] > 0)
                    {
                        renderer_blend_color(pixel_to_write, pixel, gamma);
                    }
                }
            }
        }

        // Write the blended color to the image.
        //color_map_write(r->map, buffer + (column * r->map->color_depth), block_type);
        memcpy(buffer + column * r->map->color_depth, pixel_to_write, r->map->color_depth);
    }
}

chunk *renderer_flat_get_chunk(renderer *r, int32_t coord_x, int32_t coord_z)
{
    chunk *c;
    int32_t absolute_x, absolute_z;

    if (r == NULL)
        return NULL;

    // Note: We're only using a 16 bucket slab cache here since we only ever
    // have to worry about 16 chunks at a time, so the key is the X coordinate

    if (cache_get(r->cache, coord_x, &c))
    {
        absolute_x = r->tile_x * RENDERER_TILE_SIZE + coord_x;
        absolute_z = r->tile_z * RENDERER_TILE_SIZE + coord_z;

        c = level_get_chunk_at(r->lvl, absolute_x, absolute_z);

        cache_set(r->cache, coord_x, c);
    }

    return c;
}
