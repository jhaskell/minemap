/** \file read_nbt.h
  * Routines to read NBT files into memory
  */

#ifndef READ_NBT_H
#define READ_NBT_H

#include <zlib.h>
#include "nbt.h"

/** \brief Error codes returned by nbt_read() */
enum nbt_read_errors
{
    NBT_READ_OK,                /**< \brief Everything is OK. */
    NBT_READ_PREMATURE_EOF,     /**< \brief The file unexpectedly ran out of 
                                  *         input data. */
    NBT_READ_GZREAD_ERROR,      /**< \brief gzread threw an error, use gzerror 
                                  * in zlib.h to determine what is wrong */
    NBT_READ_OUT_OF_MEM,        /**< \brief nbt_read() ran out of memory to 
                                  *         allocate */
    NBT_READ_MALFORMED_INPUT,   /**< \brief An unexpected value was read out of
                                  *         the input file */
    NBT_READ_INVALID_UTF8,      /**< \brief An invalid utf-8 byte was detected
                                  */
};

/** \brief How big the generic, reusable buffer should be */
#define NBT_READ_BUFFER_SIZE 16

/** \brief Maximum size of a TAG_String */
#define NBT_READ_UTF8_BUFFER_SIZE 32768

/** \name UTF-8 Endpoints
  * \brief Constants used to test the validity of utf-8 data 
  */
/*@{*/
#define UTF8_SINGLE_MAX 0x7F
#define UTF8_CONT_MIN   0x80
#define UTF8_CONT_MAX   0xBF
#define UTF8_DOUBLE_MIN 0xC2
#define UTF8_DOUBLE_MAX 0xDF
#define UTF8_TRIPLE_MIN 0xE0
#define UTF8_TRIPLE_MAX 0xEF
#define UTF8_QUAD_MIN   0xF0
#define UTF8_QUAD_MAX   0xF4

#define UTF8_MASK_DOUBLE  0x1F
#define UTF8_MASK_TRIPLE  0x0F
#define UTF8_MASK_QUAD    0x07
#define UTF8_MASK_PAYLOAD 0x3F
/*@}*/

/** \brief Read an NBT file and place its information into an nbt_tag.
  * \param[in] file           A gzFile to read from
  * \param[in] force_tag_type Set to the child tag type if you are reading tags
  *                           for an NBT_List, and pass 0 in every other case
  * \return An nbt_tag holding the next tag in the file. Returns NULL on error
  *
  * If a TAG_Compound is found, this function will recurse and pick up all the
  * other tags that it contains.
  */
nbt_tag *nbt_read(gzFile file, int force_tag_type);

/** \brief Wraps gzread in zlib.h and does error checking
  * \param[in]  file   A gzFile to read from
  * \param[out] buffer The buffer to write to
  * \param[in]  length The number of bytes to read from file and write to 
  *                    buffer
  *
  * This function will continue to attempt to read from file until the total
  * number of bytes are read or EOF hits. Returns 0 on success, -1 if there's 
  * an error or the EOF occured prematurely.
  */
int nbt_read_gzread(gzFile file, void *buffer, int length);

/** \brief Reads a utf8 string from a gzFile
  * \param[in]  file         A gzFile to read from
  * \param[out] wchar_buffer A buffer to write to that's big enough to fit length characters
  * \param[in]  length       The number of utf8 characters to read
  * \return 0 on success, nonzero on error. Error code stored in nbt_read_error.
  */
int nbt_read_gzread_utf8(gzFile file, wchar_t *wchar_buffer, int length);

#endif
