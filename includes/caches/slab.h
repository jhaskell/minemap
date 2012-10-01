/** \file caches/slab.h
  * \brief Implements a simple chunk_cache using a slab of allocated memory
  */

#ifndef CACHES_SLAB_H
#define CACHES_SLAB_H

#include "cache.h"
#include "chunk.h"

/** \brief Holds a slab cache's data */
typedef struct
{
    chunk **chunks;   /**< \brief The slab of chunks */
    uint8_t *allocated; /**< \brief Holds information on which buckets are filled */
    uint32_t size;  /**< \brief How large the slab is */
} cache_slab;

/** \brief Creates a new slab chunk_cache.
  * \param size How many chunks large the slab should be
  * \return A new chunk_cache, or NULL on error.
  */
chunk_cache *cache_slab_new(uint32_t size);

/** \name Private Functions 
  *
  * Functions used internally by a chunk_cache. Do not call these functions 
  * directly; use the accessor functions defined in cache.h instead.
  */
/*@{*/
/** \brief Frees the slab of memory allocated for a slab cache
  * \param _doomed  A void pointer to a slab cache to obliterate
  *
  * Implements chunk_cache.free. Call using cache_free().
  */
void cache_slab_free(void *_doomed);

/** \brief Store a chunk in the slab cache
  * \param _cache   A void pointer to a slab cache in which to store the 
  *                 payload
  * \param key      The position in the slab in which to store the payload
  * \param payload  A chunk to be stored
  * \return 0 on success, nonzero on error.
  *
  * Implements chunk_cache.set. Call using cache_set().
  */
int cache_slab_set(void *_cache, int64_t key, chunk *payload);

/** \brief Retrieve a chunk from a slab cache
  * \param _cache   A void pointer to a slab cache from which to retrieve the 
  *                 payload
  * \param key      The position in the slab in which to retrieve the payload
  * \param[out] out Where to hold the pointer to the chunk in the requested 
  *                 slot. If NULL is given, check return code for status
  * \return 0 if the chunk was found in the cache, nonzero if no chunk was 
  *         found and a NULL was passed.
  *
  * Implements chunk_cache.get. Call using cache_get().
  */
int cache_slab_get(void *_cache, int64_t key, chunk **out);

/** \brief Remove all chunks from a slab cache.
  * \param _cache   A void pointer to a slab cache to empty
  */
void cache_slab_empty(void *_cache);

/*@}*/
#endif
