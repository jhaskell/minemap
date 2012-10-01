/** \file colors.h
  * \brief Generates colors that correspond with minecraft blocks
  */

#ifndef COLORS_H
#define COLORS_H

#include <png.h>
#include <stdio.h>
#include <stdint.h>

/** \brief The total number of blocks in Minecraft. */
#define BLOCK_COUNT 256

/** \brief The color depth of each pixel in bytes */
#define BLOCK_COLOR_DEPTH 4

/** \brief Error codes that can be returned by color_map_get(). */
enum color_map_errors
{
    COLOR_MAP_OK,           /**< \brief Everything is OK */
    COLOR_MAP_ERR_NOMAP,    /**< \brief No color_map was passed */
    COLOR_MAP_ERR_NODEST,   /**< \brief No destination was passed */
    COLOR_MAP_ERR_BLOCK,    /**< \brief An invalid block type was passed */
};

/** \brief The expected format for the get function of a color map 
  * \param void*    A void pointer to a color_map.
  * \param uint16_t The block type whose color you are requesting
  * \return A pointer to a position in the colors array of the color_map.
  *         Applications wishing to use this color must use the color_depth
  *         to determine how many bytes to copy out of this array.
  */
typedef png_byte *(*color_get_func )(void*,uint16_t);

/** \brief Holds a map of blocks to colors. */
typedef struct 
{
    png_byte *colors; /**< \brief Holds the color definition for this color map */

    uint16_t block_count; /**< \brief Holds the number of blocks that are defined in this map */

    uint8_t color_depth; /**< \brief Holds the color depth used by this color map. */

    color_get_func get; /**< \brief The function to use to get the color associated with a block */
} color_map;


/** \brief constructs a new color map.
  * \protected
  * \param colors       A png_byte array of colors 
  * \param block_count  How many blocks are in this color map
  * \param color_depth  How many bytes each color occupies
  * \param get          A function to use to retrieve a color from the map
  * \return A new color_map, or NULL on error.
  */
color_map *color_map_new(png_byte *colors, uint16_t block_count, uint8_t color_depth, color_get_func get);

/** \brief Frees a color map
  * \param doomed   The color_map to destroy
  */
void color_map_free(color_map *doomed);

/** \brief Finds the color of a requested block type and write it to a buffer
  * \param map          A color_map to get a color from
  * \param destination  The destination buffer to receive the color.
  * \param block_type   The block type whose color you want
  * \return 0 on success, a nonzero value in #color_map_errors on error.
  */
int color_map_write(color_map *map, png_byte *destination, uint16_t block_type);

/** \brief Retrieves a color from the color map.
  * \param map          A color_map to get a color from
  * \param block_type   The block_type whose color you want
  * \return A pointer to the location of your color, or NULL on error.
  */
png_byte *color_map_get(color_map *map, uint16_t block_type);
#endif
