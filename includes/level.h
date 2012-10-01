/** \file level.h
  * Functions to represent a %level from Minecraft
  */

#ifndef LEVEL_H
#define LEVEL_H

#include "nbt.h"
#include "chunk.h"

/** \brief How large the buffers used to hold a base 36 string should be. */
#define LEVEL_BASE_36_SIZE 16

/** \brief How large the buffer to hold the path to a chunk should be. */
#define LEVEL_BUFFER_SIZE 128

/** \brief Contains information about the %level. */
typedef struct 
{
    char *input_path;       /**< \brief The base path for all of the tag data
                              */
    nbt_tag *data;          /**< \brief The nbt_tag holding the %level's 
                              *         metadata */
    int32_t smallest_x;     /**< \brief The smallest X coordinate of the level
                              */
    int32_t smallest_z;     /**< \brief The smallest Z coordinate of the level
                              */
    int32_t largest_x;      /**< \brief The largest X coordinate of the level 
                              */
    int32_t largest_z;      /**< \brief The largest Z coordinate of the level 
                              */
    uint64_t chunk_count;   /**< \brief The number of chunks in the map */
} level;

/** \brief Loads the given minecraft %level.
  * \param path The path to the directory with the %level data.
  * \return A level containing the game map data, or NULL on error.
  */
level *level_load(char *path);

/** \brief Frees a level
  * \param doomed   The level to free
  *
  * This function does NOT free the input path.
  */
void level_free(level *doomed);

/** \brief Loads a chunk from the map
  * \param lvl      The level whose chunks you wish to find
  * \param coord_x  The x Coordinate of the chunk
  * \param coord_z  The z Coordinate of the chunk
  * \return The chunk corresponding to the coordinates, or NULL if it does not
  *         exist or on error.
  */
chunk *level_get_chunk_at(level *lvl, int coord_x, int coord_z);

/** \brief Scans a world folder and derives its dimensions 
  * \param[in]  lvl     The level to read
  * \return Fills parameters of the level passed.
  */
void level_get_dimensions(level *lvl);

#endif
