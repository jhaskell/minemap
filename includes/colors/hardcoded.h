/** \file colors/hardcoded.h
  * Hardcoded color definitions; useful for testing.
  */
#ifndef COLOR_MAP_HARDCODED_H
#define COLOR_MAP_HARDCODED_H

#include <stdint.h>
#include <png.h>

#include "colors.h"

/** \brief Returns a new color_map with hardcoded color definitions
  * \param block_count  The number of blocks to allocate space for
  * \param color_depth  How many bytes should be used per color
  * \return A new color_map, or NULL on error.
  */
color_map *color_map_hardcoded_new(uint16_t block_count, uint8_t color_depth);

/** \brief Function to retrieve a color from the map
  * \param _map         The color_map from which to fetch a color
  * \param block_type   The type of block whose color you want
  * \return A pointer to a position inside a color_map's color array 
  *         corresponding to the block type requested. 
  */
png_byte *color_map_hardcoded_get(void *_map, uint16_t block_type);

/** \brief Used internally to write a single color to the color buffer
  * \param colors       A buffer to write to
  * \param block_type   The block type to write
  * \param red          The red value for this block
  * \param blue         The blue value for this block
  * \param green        The green value for this block
  * \param alpha        The alpha value for this block
  */
void color_map_hardcoded_assign(png_byte *colors, uint16_t block_type, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);

#endif
