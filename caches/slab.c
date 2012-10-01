#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "chunk.h"
#include "cache.h"
#include "caches/slab.h"

chunk_cache *cache_slab_new(uint32_t size)
{
    int i;
    chunk_cache *cache;
    cache_slab *slab;

    cache = cache_new();

    if (cache != NULL)
    {
        slab = malloc(sizeof(cache_slab));
        if (slab != NULL)
        {
            slab->chunks = malloc(size * sizeof(chunk*));
            slab->allocated = calloc(1, (int)ceil(size / 8));
            slab->size = size;

            if (slab->chunks != NULL && slab->allocated != NULL)
            {
                for (i = 0; i < size; i++)
                    slab->chunks[i] = NULL;

                cache->data = slab;
                cache->free = cache_slab_free;
                cache->set = cache_slab_set;
                cache->get = cache_slab_get;
                cache->empty = cache_slab_empty;
            }
            else
            {
                if (slab->chunks != NULL)
                    free(slab->chunks);
                if (slab->allocated != NULL)
                    free(slab->allocated);
                cache_slab_free(cache);
                cache_free(cache);
                cache = NULL;
            }
        }
        else
        {
            cache_free(cache);
            cache = NULL;
        }
    }

    return cache;
}

void cache_slab_free(void *_doomed)
{
    chunk_cache *doomed = (chunk_cache*)_doomed;
    cache_slab *slab;

    if (doomed != NULL && doomed->data != NULL)
    {
        slab = (cache_slab*)doomed->data;
        if (slab->chunks != NULL)
        {
            cache_slab_empty(doomed);
            free(slab->chunks);
        }
        if (slab->allocated != NULL)
            free(slab->allocated);
        free(doomed->data);
    }
}

int cache_slab_set(void *_cache, int64_t key, chunk *payload)
{
    chunk_cache *cache = (chunk_cache*)_cache;
    cache_slab *slab;

    if (cache == NULL || cache->data == NULL)
        return -1;

    slab = (cache_slab*)cache->data;
    if (slab->chunks == NULL || slab->allocated == NULL)
        return -1;

    slab->chunks[key] = payload;
    slab->allocated[key >> 3] |= 1 << (key % 8);

    return 0;
}

int cache_slab_get(void *_cache, int64_t key, chunk **out)
{
    chunk_cache *cache = (chunk_cache*)_cache;
    cache_slab *slab;

    if (cache == NULL || cache->data == NULL)
        return -1;

    slab = (cache_slab*)cache->data;
    if (slab->chunks == NULL || slab->allocated == NULL)
        return -1;

    if ((slab->allocated[key >> 3] & (1 << (key % 8))) == 0)
    {
        *out = NULL;
        return 1;
    }
    else
    {
        *out = slab->chunks[key];
        return 0;
    }
}

void cache_slab_empty(void *_cache)
{
    int i;
    cache_slab *slab;
    chunk *c;
    chunk_cache *cache = (chunk_cache*)_cache;

    if (cache == NULL || cache->data == NULL)
        return;

    slab = (cache_slab*)cache->data;
    if (slab->chunks == NULL || slab->allocated == NULL)
        return;

    for (i = 0; i < slab->size; i++)
    {
        c = slab->chunks[i];

        if (c != NULL)
            chunk_free(c);

        slab->chunks[i] = NULL;
    }

    memset(slab->allocated, 0, (int)ceil(slab->size / 8));
}
