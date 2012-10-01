/** \file cache.h
  * \brief The base object for all chunk caches
  */
#ifndef CACHE_H
#define CACHE_H

#include "chunk.h"

/** \brief The base object for a chunk cache.
  */
typedef struct
{
    /** \brief Holds the data structure for the cache. */
    void *data;

    /** \brief Frees the renderer cache for this renderer. */
    void (*free)(void*);

    /** \brief Set a value in the cache. */
    int (*set)(void*,int64_t,chunk*);

    /** \brief Get a value in the cache. */
    int (*get)(void*,int64_t,chunk**);

    /** \brief Remove all the items from this cache. */
    void (*empty)(void*);

} chunk_cache;


/** \name Cache Accessor Functions
  *
  * Use these functions to manipulate a chunk_cache.
  */

/*@{*/

/** \brief Free a cache and all its values
  * \param doomed   A chunk_cache to free
  * \note Functions implementing this routine do not need to worry about 
  *       freeing the chunk_cache itself; this function will do that.
  */
void cache_free(chunk_cache *doomed);

/** \brief Store a chunk in a chunk_cache
  * \param cache    The chunk_cache in which to store the chunk
  * \param key      The key to associate with the chunk in the cache
  * \param payload  A chunk to store
  * \return 0 on success, nonzero on error.
  */
int cache_set(chunk_cache *cache, int64_t key, chunk *payload);

/** \brief Retrieve a chunk from a chunk_cache.
  * \param cache    The chunk_cache from which to retrieve the chunk
  * \param key      The key to the chunk you wish to retrieve
  * \param[out] out Where to hold the pointer to the chunk in the requested 
  *                 slot. If NULL is given, check return code for status
  * \return 0 if the chunk was found in the cache, nonzero if no chunk was 
  *         found and a NULL was passed.
  */
int cache_get(chunk_cache *cache, int64_t key, chunk **out);

/** \brief Remove and free all values from a chunk_cache
  * \param cache    The chunk_cache to empty
  */
void cache_empty(chunk_cache *cache);

/*@}*/

/** \name Constructor
  *
  * Do not use this constructor; instead, use the constructor for the type of
  * cache that you want.
  */
/*@{*/
/** \brief Allocates a new cache.
  * \return A pointer to a new chunk_cache, or NULL if one could not be allocated.
  *
  * This function should not be called directly, but only by a child object 
  * implementing these routines.
  */
chunk_cache *cache_new();
/*@}*/

#endif
