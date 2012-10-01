/** \file renderer.h
  * \brief Base functions and structs for a map %renderer.
  */

#ifndef RENDERER_H
#define RENDERER_H

#include <stdio.h>
#include <png.h>

#include "level.h"
#include "hashtable.h"
#include "chunk.h"
#include "colors.h"
#include "cache.h"

/** \brief How many chunks should comprise a tile in both X and Z directions */
#define RENDERER_TILE_SIZE  16

/** \brief How many buckets the cache should initially contain */
#define RENDERER_CACHE_BUCKETS 32

/** \brief The offset of the alpha channel in a 32-bit color. */
#define COLOR_ALPHA_OFFSET 3

/** \brief Errors that can occur when renderer_perform() fails. */
enum renderer_errors
{
    RENDER_OK,              /**< \brief Everything is OK */
    RENDER_ERR_SANITY,      /**< \brief A renderer passed failed the sanity 
                              *         check */
    RENDER_ERR_FILE,        /**< \brief Could not open output file */
    RENDER_ERR_PNG,         /**< \brief libPNG returned an error */
    RENDER_ERR_DIM,         /**< \brief Could not retrieve the dimensions of
                              *         the tile */
    RENDER_MEM_PNGWRITE,    /**< \brief Couldn't create a PNG write struct */
    RENDER_MEM_PNGINFO,     /**< \brief Couldn't create a PNG info struct */
    RENDER_MEM_ROW,         /**< \brief Couldn't create a row of PNG data */
};

/** \brief Contains pointers to all the functions that a renderer must 
  *        implement */
typedef struct 
{
    /** \brief Return the expected dimensions of the tile being rendered */
    int (*dimensions)(int*,int*);

    /** \brief Write a row of image data. */
    void (*draw_row)(void*,png_bytep,int);

} renderer_funcs;

/** \brief Holds information about a %renderer.
  */
typedef struct 
{
    level *lvl;             /**< \brief The level to render. */
    char *output_path;      /**< \brief The place where the image should be 
                              *         stored.  */
    renderer_funcs *funcs;  /**< \brief A renderer_funcs instance for this 
                             *          renderer. */
    int32_t tile_x;         /**< \brief The X coordinate of the tile being
                              *         rendered */
    int32_t tile_z;         /**< \brief The Z coordinate of the tile being
                              *         rendered */
    chunk_cache *cache;     /**< \brief A chunk_cache */

    color_map *map;         /**< \brief A color_map used by this %renderer */
#ifdef DO_BLOCK_COUNT
    uint8_t block_count[256]; 
#endif
} renderer;

/** \brief Creates a new renderer.
  * \param lvl          A level object to render
  * \param map          A color_map used by this renderer
  * \param funcs        A render_funcs used by this particular renderer
  * \param cache        A chunk_cache used by this particular renderer
  * \return A new renderer, or NULL on error.
  *
  * This function should not be called directly; instead, the constructors for
  * the particular %renderer you wish to use should be called, which will call
  * this function and return its value.
  */
renderer *renderer_new(level *lvl, color_map *map, renderer_funcs *funcs, chunk_cache *cache);

/** \brief Perform the render
  * \param r        The renderer to use to perform the rendering
  * \param tile_x   The X coordinate of the tile you wish to render
  * \param tile_z   The Z coordinate of the tile you wish to render
  * \param output_path  The place where the image should be stored.
  * \return 0 on success, nonzero if an error occured. No image is created if 
  *         the renderer fails.
  */
int renderer_perform(renderer *r, int32_t tile_x, int32_t tile_z, char *output_path);

/** \brief Sanity checks the renderer, ensuring that vital information exists
  * \param r    A renderer to check.
  * \return 0 on succes, -1 if there are missing functions.
  */
int renderer_sanity_check(renderer *r);

/** \brief Free a renderer's allocated memory
  * \param doomed   The renderer to destroy
  */
void renderer_free(renderer *doomed);

/** \brief Blend color2 into color1
  * \param[out] pixel1  A 32-bit color to be blended against. Result is written
  *                     to this pixel.
  * \param[in]  pixel2  A 32-bit color to blend into pixel1. Is not modified.
  * \param      gamma   A factor from 0.0 to 1.0 to darken the result by.
  */
void renderer_blend_color(png_bytep pixel1, png_bytep pixel2, float gamma);

/** \brief Calculates the gamma for a block based on the map's light values
  * \param c                The chunk that the block resides on
  * \param coord_x          The X coordinate of the block
  * \param coord_y          The Y coordinate of the block
  * \param coord_z          The Z coordinate of the block
  * \param sky_percent      How much sky lighting should contribute to the 
  *                         total gamma (0.0 = none, 1.0 = full)
  * \param block_percent    How much block lighting should contribute to the
  *                         total gamma (0.0 = none, 1.0 = full)
  * \return A gamma value for the block, between 0.0 (fully dark) to 
  *         1.0 (fully lit)
  */
float renderer_calc_gamma(chunk *c, int16_t coord_x, int16_t coord_y, int16_t coord_z, float sky_percent, float block_percent);

#endif












