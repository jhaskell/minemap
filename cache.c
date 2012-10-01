#include <stdlib.h>

#include "chunk.h"
#include "cache.h"

chunk_cache *cache_new()
{
    chunk_cache *new;

    new = malloc(sizeof(chunk_cache));

    if (new != NULL)
    {
        new->data = NULL;
        new->free = NULL;
        new->set = NULL;
        new->get = NULL;
        new->empty = NULL;
    }

    return new;
}

void cache_free(chunk_cache *doomed)
{
    if (doomed != NULL)
    {
        if (doomed->free != NULL)
            doomed->free(doomed);
        free(doomed);
    }
}

int cache_set(chunk_cache *cache, int64_t key, chunk *payload)
{
    if (cache == NULL || cache->set == NULL)
        return -1;
    return cache->set(cache, key, payload);
}

int cache_get(chunk_cache *cache, int64_t key, chunk **out)
{
    if (cache == NULL || cache->get == NULL)
        return -1;
    return cache->get(cache, key, out);
}

void cache_empty(chunk_cache *cache)
{
    if (cache != NULL && cache->empty != NULL)
        cache->empty(cache);
}
