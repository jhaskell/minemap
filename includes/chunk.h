/** \file chunk.h
  * \brief Structures to represent and functions to manipulate chunks of map
  *        data in Minecraft.
  */

#ifndef CHUNK_H
#define CHUNK_H

#include "nbt.h"
#include "maths.h"

/** \brief The length of a chunk along the X axis, in blocks. */
#define CHUNK_SIZE_X 16
/** \brief The length of a chunk along the Z axis, in blocks. */
#define CHUNK_SIZE_Z 16
/** \brief The area of a chunk along the XZ plane, in blocks. */
#define CHUNK_SIZE_AREA 256

/** \brief Describes error codes passed by chunk_new(). */
enum chunk_new_error_codes
{
    CHUNK_ERR_OK,       /**< \brief Everything is OK */
    CHUNK_ERR_OOM,      /**< \brief Could not allocate memory */
    CHUNK_ERR_INPUT,    /**< \brief Was not able to open the input file */
    CHUNK_ERR_TAG,      /**< \brief Parsing the tag failed */
    CHUNK_ERR_COORDS,   /**< \brief Could not read the chunk coordinates from
                          *         from the input file */
    CHUNK_ERR_CONSIST,  /**< \brief An inconsistency occured between the coords
                          *         derived from the map value and the info in
                          *         the tag */
    CHUNK_ERR_HEIGHT,   /**< \brief The length of map data was not consistent
                          *         and height data could not be reliably 
                          *         determined */
    CHUNK_ERR_TAG_FORMAT, /**<      The format of the tag was incorrect. */
};

/** \brief Contains data about a chunk. 
  * \note Of note here is that we are storing height data, despite the fact 
  * that the maximum height is currently the same for all chunks (128.) Notch 
  * has said that he has future plans to increase the height of chunks, so 
  * we'll calculate it for every chunk to stay future-proof.
  */
typedef struct
{
    nbt_tag *data;      /**< \brief Holds the tag data for this chunk */
    int32_t coord_x;    /**< \brief The X coordinate of this chunk */
    int32_t coord_z;    /**< \brief The Z coordinate of this chunk */
    uint64_t height;    /**< \brief How tall this chunk is, in blocks */

    uint8_t *blocks;        /**< \brief The block types on this chunk */
    uint8_t *heightmap;     /**< \brief The height map of this chunk */
    uint8_t *blockdata;     /**< \brief The block data of this chunk */
    uint8_t *skylight;      /**< \brief The skylight of this chunk */
    uint8_t *blocklight;    /**< \brief The blocklight of this chunk */
} chunk;

/** \brief Contains a map between tag names and members of a chunk */
typedef struct
{
    wchar_t *key;       /**< \brief A tag name. */
    uint64_t offset;    /**< \brief An offset in a chunk. */
} tag_name_addr_map;

/** \brief Reads a chunk from a file
  * \param filepath The path to a chunk
  * \param coord_x  The X coordinate of this chunk.
  * \param coord_z  The Z coordinate of this chunk.
  * \return A pointer to the new chunk, or NULL on error.
  */
chunk *chunk_new(char *filepath, int32_t coord_x, int32_t coord_z);

/** \brief Destroy a chunk, freeing its memory
  * \param doomed   The chunk to destroy
  */
void chunk_free(void *doomed);

/** \brief Parse a chunk's filename and retrieve the coordinates from it
  * \param      filename    The filename of the chunk to parse
  * \param[out] x_coord     A pointer to an integer to store the x coord
  * \param[out] z_coord     A pointer to an integer to store the z coord
  * \return 0 on success, -1 if an error occurs. The values of x_coord and 
  *         z_coord are undefined if an error occurs.
  */
int chunk_get_coords_from_filename(char *filename, int32_t *x_coord, int32_t *z_coord);

/** \brief Generate a 64-bit key for the chunk using its coordinates
  * \param c    A chunk
  * \return A 64-bit key, with the most significant 32 bits as the x coordinate
  *         and the least significant 32 bits as the z coordinate
  */
uint64_t chunk_generate_key(chunk *c);

/** \brief Generate a 64-bit key for a chunk using provided coordinates
  * \param coord_x  A 32-bit X coordinate
  * \param coord_z  A 32-bit Z coordinate
  * \return A 64-bit key, with the most significant 32 bits as the x coordinate
  *         and the least significant 32 bits as the z coordinate
  */
uint64_t chunk_generate_key_from_coords(int32_t coord_x, int32_t coord_z);

/** \brief Generate a hash key for a chunk.
  * \param _c   A chunk
  * \return A 32-bit hash value.
  *
  * Shamelessly taken from http://www.concentric.net/~Ttwang/tech/inthash.htm.
  */
uint32_t chunk_hash(void *_c);

/** \brief Determine if two chunk hash keys are equal
  * \param key1 A pointer to the first uint64_t key
  * \param key2 A pointer to the second uint64_t key
  * \return 1 if the keys are equal, 0 if they are not
  */
int chunk_key_eqfn(void *key1, void *key2);

/** \brief Generates an array offset for the 8-bit per block data arrays
  * \param c            The chunk with the data array requested
  * \param coord_x      The X coordinate (north-south) of the block requested, 
  *                     local to the chunk (0-15)
  * \param coord_y      The Y coordinate (altitude) of the requested block, 
  *                     local to the chunk (0-chunk.height)
  * \param coord_z      The Z coordinate (east-west) of the requested block, 
  *                     local to the chunk (0-15)
  * \param max_offset   The maximum value that the offset could be (typically 
  *                     the size of the TAG_Byte_Array being queried)
  * \return A relative address which corresponds to the location of the block 
  *         requested, or -1 on error.
  * \note   Passing 0 for the Y coordinate will give you an offset corresponding
  *         to the start of a column of map data, which you can then iterate 
  *         over manually in your routine. X and Z coordinates do not work this
  *         way.
  */
int32_t chunk_generate_8bit_offset(chunk *c, uint8_t coord_x, uint8_t coord_y, uint8_t coord_z, uint32_t max_offset);

/** \brief Generates an array offset for the 4-bit per block data arrays
  * \param c            The chunk with the data array requested
  * \param coord_x      The X coordinate (north-south) of the block requested, 
  *                     local to the chunk (0-15)
  * \param coord_y      The Y coordinate (altitude) of the requested block, 
  *                     local to the chunk (0-chunk.height)
  * \param coord_z      The Z coordinate (east-west) of the requested block, 
  *                     local to the chunk (0-15)
  * \param max_offset   The maximum value that the offset could be (typically 
  *                     the size of the TAG_Byte_Array being queried)
  * \return A relative address which corresponds to the location of the block 
  *         requested, or -1 on error.
  * \note   Passing 0 for the Y coordinate will give you an offset corresponding
  *         to the start of a column of map data, which you can then iterate 
  *         over manually in your routine. X and Z coordinates do not work this
  *         way.
  */
int32_t chunk_generate_4bit_offset(chunk *c, uint8_t coord_x, uint8_t coord_y, uint8_t coord_z, uint32_t max_offset);

#endif
