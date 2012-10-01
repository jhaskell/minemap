#include <stdlib.h>
#include <stdio.h>
#include <png.h>
#include <setjmp.h>
#include <math.h>

#include "renderer.h"
#include "level.h"
#include "hashtable.h"
#include "chunk.h"

renderer *renderer_new(level *lvl, color_map *map, renderer_funcs *funcs, chunk_cache *cache)
{
    renderer *new;

    new = calloc(1, sizeof(renderer));
    
    if (new != NULL)
    {
        new->lvl = lvl;
        new->funcs = funcs;
        new->map = map;
        new->cache = cache;
#ifdef DO_BLOCK_COUNT
        memset(new->block_count, 0, 256);
#endif
    }

    return new;
}

int renderer_perform(renderer *r, int32_t tile_x, int32_t tile_z, char *output_path)
{
    int err;
    FILE *file = NULL;
    png_structp png_ptr = NULL;
    png_infop   png_info = NULL;
    uint32_t width, height, row_number;
    png_bytep png_row = NULL;

    // Ensure the data is sane
    if (renderer_sanity_check(r))
        return RENDER_ERR_SANITY;

    r->tile_x = tile_x;
    r->tile_z = tile_z;

    // Open the output file
    file = fopen(output_path, "wb");
    if (file == NULL)
    {
        err = RENDER_ERR_FILE;
        goto renderer_perform_cleanup;
    }

    // Initialize the PNG image
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL)
    {
        err = RENDER_MEM_PNGWRITE;
        goto renderer_perform_cleanup;
    }

    // Init the PNG info struct
    png_info = png_create_info_struct(png_ptr);
    if (png_info == NULL)
    {
        err = RENDER_MEM_PNGINFO;
        goto renderer_perform_cleanup;
    }

    // If libPNG has an error, it will jump here to exit
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        err = RENDER_ERR_PNG;
        goto renderer_perform_cleanup;
    }

    // Show libPNG where to write the tile image
    png_init_io(png_ptr, file);

    // Retrieve the expected dimensions of the output image
    if (r->funcs->dimensions(&width, &height))
    {
        err = RENDER_ERR_DIM;
        goto renderer_perform_cleanup;
    }

    // Set the PNG metadata
    png_set_IHDR(png_ptr, png_info, width, height,
        8,                              // Color depth in bits
        PNG_COLOR_TYPE_RGB_ALPHA,       // Color type
        PNG_INTERLACE_NONE,             // Interlace type,
        PNG_COMPRESSION_TYPE_DEFAULT,   // Compression type,
        PNG_FILTER_TYPE_DEFAULT         // Filter type
    );

    png_write_info(png_ptr, png_info);
    //png_set_flush(png_ptr, 16);

    // TODO: Set text if desired (probably not for tiles)

    // Allocate one row of image data and reuse the crap out of it
    // We need 4 bytes per pixel (8 bits each for red, green, blue, and alpha)
    png_row = malloc(4 * width * sizeof(png_byte));
    if (png_row == NULL)
    {
        err = RENDER_MEM_ROW;
        goto renderer_perform_cleanup;
    }

    // Render the image a row at a time
    for (row_number = 0; row_number < height; row_number++)
    {
        r->funcs->draw_row(r, png_row, row_number);
        png_write_row(png_ptr, png_row);
    }

    // Close the PNG image and clean up
    png_write_end(png_ptr, NULL);

    err = RENDER_OK;

renderer_perform_cleanup:
    if (file != NULL)
        fclose(file);
    if (png_info != NULL)
        png_free_data(png_ptr, png_info, PNG_FREE_ALL, -1);
    if (png_ptr != NULL)
        png_destroy_write_struct(&png_ptr, &png_info);
    if (png_row != NULL)
        free(png_row);

    return err;
}

int renderer_sanity_check(renderer *r)
{
    if (r->funcs != NULL)
    {
        if (
            r->funcs->dimensions  == NULL ||
            r->funcs->draw_row    == NULL 
           )
            return -1;
    }
    else
        return -1;

    if (r->lvl == NULL)
        return -1;

    return 0;
}

void renderer_free(renderer *doomed)
{
    if (doomed == NULL)
        return;

    if (doomed->funcs != NULL)
        free(doomed->funcs);

    if (doomed->lvl != NULL)
        level_free(doomed->lvl);

    if (doomed->map != NULL)
        color_map_free(doomed->map);

    if (doomed->cache != NULL)
        cache_free(doomed->cache);

    free(doomed);
}

void renderer_blend_color(png_bytep pixel1, png_bytep pixel2, float gamma)
{
    int i;
    float alpha;
   
    if (pixel2 != NULL)
    {
        alpha = (float)pixel2[COLOR_ALPHA_OFFSET] / 255;
        for (i = 0; i < 3; i++)
            pixel1[i] = (1 - alpha) * pixel1[i] + alpha * pixel2[i] * gamma;

    }
    else
    {
        alpha = (float)pixel1[COLOR_ALPHA_OFFSET] / 255;
        for (i = 0; i < 3; i++)
            pixel1[i] *= alpha * gamma;
    }
    pixel1[COLOR_ALPHA_OFFSET] = 255;
}

float renderer_calc_gamma(chunk *c, int16_t coord_x, int16_t coord_y, int16_t coord_z, float sky_percent, float block_percent)
{
    float sky, block;
    float gamma;
    uint32_t offset;

    if (c == NULL || c->skylight == NULL || c->blocklight == NULL)
        return 1.0;

    if ( coord_x < 0 || coord_x > 15 ||
         coord_z < 0 || coord_z > 15 ||
         coord_y < 0 || coord_y >= c->height)
        return 1.0;

    if (sky_percent < 0 || sky_percent > 1.0 ||
        block_percent < 0 || block_percent > 1.0)
        return 1.0;

    offset = chunk_generate_4bit_offset(c, coord_x, coord_y, coord_z, CHUNK_SIZE_AREA * c->height / 2);
    if ((coord_y % 2) == 0)
    {
        sky = (float)(*(c->skylight + offset) & 0x0F);
        block = (float)(*(c->blocklight + offset) & 0x0F);
    }
    else
    {
        sky = (float)((*(c->skylight + offset) & 0xF0) >> 4);
        block = (float)((*(c->blocklight + offset) & 0xF0) >> 4);
    }

    sky = sky_percent * sky / 15;
    block = block_percent * block / 15;

    gamma = (sky > block ? sky : block);
    gamma = 0.75 * gamma + 0.25 * ((float)coord_y / c->height);

    return gamma;

}
