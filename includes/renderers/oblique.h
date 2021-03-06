/** \file renderers/oblique.h
  * \brief Renders an oblique map
  */

#ifndef RENDERERS_OBLIQUE_H
#define RENDERERS_OBLIQUE_H

#include <png.h>

#include "level.h"
#include "renderer.h"
#include "colors.h"
#include "chunk.h"

/** \brief The width of an oblique renderer image in pixels. */
#define RENDERER_OBLIQUE_IMAGE_WIDTH    256
/** \brief The height of an oblique renderer image in pixels. */
#define RENDERER_OBLIQUE_IMAGE_HEIGHT   384
/** \brief The number of slots to use for the cache. */
#define RENDERER_OBLIQUE_CACHE_SIZE     256

/** \brief Creates a new Oblique %Renderer.
  * \param lvl          The level to render
  * \param map          A color_map to use while rendering the image
  * \param output_path  Where to store the image after it has been rendered
  * \return A new renderer, or NULL on error.
  */
renderer *renderer_oblique_new(level *lvl, color_map *map, char *output_path);

/** \brief Returns the dimensions of a tile generated by an oblique %renderer.
  * \param[out] width   The width of the tile
  * \param[out] height  The height of the tile
  * \return 0 on succes, -1 on error
  *
  * Implements renderer_funcs.dimensions.
  */
int renderer_oblique_dimensions(int *width, int *height);

/** \brief Draws a single row of image data.
  * \param      _r          A void pointer to a renderer
  * \param[out] buffer      The buffer to draw upon
  * \param      row_number  Which row to render
  *
  * Implements renderer_funcs.draw_row.
  */
void renderer_oblique_draw_row(void *_r, png_bytep buffer, int row_number);

/** \brief Retrieve a chunk at the given coordinates
  * \param r        The renderer whose level's chunks you want
  * \param coord_x  The X Coordinate of the chunk you want
  * \param coord_z  The Z Coordinate of the chunk you want
  * \return The chunk corresponding to the coordinates, or NULL if it is
  *         missing or there is an error.
  */
chunk *renderer_oblique_get_chunk(renderer *r, int32_t coord_x, int32_t coord_z);

#endif
