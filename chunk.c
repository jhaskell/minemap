#include <zlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include <math.h>
#include <stddef.h>

#include "chunk.h"
#include "nbt.h"
#include "read_nbt.h"
#include "hashtable.h"
#include "maths.h"

int chunk_errno = 0;

chunk *chunk_new(char *filepath, int32_t coord_x, int32_t coord_z)
{
    gzFile file = Z_NULL;
    chunk *new;
    nbt_tag *tag;
    int i;
    wchar_t *tag_name;
    struct hashtable *hash;
    void *payload;
    int64_t height = -1;
    tag_name_addr_map *map;

    // We don't need these any more, but I'll keep them here in case we do
    /*
    static wchar_t *expected_tags[] = {
        L"SkyLight",
        L"LastUpdate",
        L"TerrainPopulated",
        L"Data",
        L"Blocks",
        L"Entities",
        L"HeightMap",
        L"TileEntities",
        L"xPos",
        L"zPos",
        L"BlockLight",
        NULL,
    };
    */
    
    // We're gonna do some tricky pointer math here to save us some unnecessary
    // hash lookups.
    static tag_name_addr_map tags_4bit_level_data[] = {
        {L"SkyLight", offsetof(chunk, skylight)},
        {L"Data", offsetof(chunk, blockdata)},
        {L"BlockLight", offsetof(chunk, blocklight)},
        {NULL, 0},
    };

    static tag_name_addr_map tags_8bit_level_data[] = {
        {L"Blocks", offsetof(chunk, blocks)},
        {NULL, 0},
    };

    extern int chunk_errno, nbt_read_error;

    // Allocate memory for the chunk
    new = calloc(1, sizeof(chunk));
    if (new == NULL)
    {
        chunk_errno = CHUNK_ERR_OOM;
        return NULL;
    }

    // Initialize the chunk
    new->height = -1;
    new->coord_x = coord_x;
    new->coord_z = coord_z;
    new->blocks = NULL;
    new->heightmap = NULL;
    new->blockdata = NULL;
    new->skylight = NULL;
    new->blocklight = NULL;

    // Open the file
    file = gzopen(filepath, "r");
    if (file == Z_NULL)
    {
        chunk_errno = CHUNK_ERR_INPUT;
        goto chunk_new_cleanup;
    }

    // Read the tag
    new->data = nbt_read(file, 0);

    gzclose(file);
    file = Z_NULL;

    if (new->data == NULL)
    {
        chunk_errno = CHUNK_ERR_TAG;
        goto chunk_new_cleanup;
    }

    // Get the hash out of the tag (and make sure the tag is a TAG_Compound)
    hash = nbt_payload(new->data, TAG_Compound);
    if (hash == NULL)
    {
        chunk_errno = CHUNK_ERR_TAG;
        goto chunk_new_cleanup;
    }

    // If the root node has a null name, grab its child
    if (wcsncmp(new->data->name, L"(null)", 7) == 0)
    {
        tag = hashtable_remove(hash, L"Level");
        if (tag == NULL)
        {
            chunk_errno = CHUNK_ERR_TAG;
            goto chunk_new_cleanup;
        }
        nbt_free_tag(new->data);
        new->data = tag;
    }

    // Make sure the root tag is named Level
    if (wcsncmp(new->data->name, L"Level", 6) != 0)
    {
        chunk_errno = CHUNK_ERR_TAG_FORMAT;
        goto chunk_new_cleanup;
    }

    if (chunk_get_coords_from_filename(filepath, &(new->coord_x), &(new->coord_z)))
        goto chunk_new_coord_error;

    // Ensure the coordinates found in the filename and the coordinates in the
    // level data match

    // Check X coordinate.
    tag = nbt_hash_search(new->data, L"xPos");
    payload = nbt_payload(tag, TAG_Int);
    if (payload == NULL || new->coord_x != *(int32_t*)payload)
    {
        chunk_errno = CHUNK_ERR_CONSIST;
        goto chunk_new_cleanup;
    }

    // Check Z coordinate. 
    tag = nbt_hash_search(new->data, L"zPos");
    payload = nbt_payload(tag, TAG_Int);
    if (payload == NULL || new->coord_z != *(int32_t*)payload)
    {
        chunk_errno = CHUNK_ERR_CONSIST;
        goto chunk_new_cleanup;
    }

    // Derive the height of the chunk and make sure the heights are consistent
    // between different types of data

    // 4-bit data types
    i = 0;
    for (;;)
    {
        map = tags_4bit_level_data + i++;
        if (map->key == NULL)
            break;

        tag = nbt_hash_search(new->data, map->key);
        if (tag->meta == NULL)
        {
            chunk_errno = CHUNK_ERR_HEIGHT;
            goto chunk_new_cleanup;
        }

        payload = nbt_payload(tag, TAG_Byte_Array);
        if (payload == NULL)
        {
            chunk_errno = CHUNK_ERR_HEIGHT;
            goto chunk_new_cleanup;
        }

        height = tag->meta->length / CHUNK_SIZE_AREA * 2;

        if (new->height == -1)
            new->height = height;
        else if (new->height != height)
        {
            chunk_errno = CHUNK_ERR_HEIGHT;
            goto chunk_new_cleanup;
        }

        // Assigns the payload to the appropriate member of the struct
        *(uint8_t**)((char*)new + map->offset) = payload;
    }

    // 8-bit data types
    i = 0;
    for (;;)
    {
        map = tags_8bit_level_data + i++;
        if (map->key == NULL)
            break;

        tag = nbt_hash_search(new->data, map->key);
        if (tag == NULL)
        {
            chunk_errno = CHUNK_ERR_HEIGHT;
            goto chunk_new_cleanup;
        }

        if (tag->meta == NULL)
        {
            chunk_errno = CHUNK_ERR_HEIGHT;
            goto chunk_new_cleanup;
        }

        payload = nbt_payload(tag, TAG_Byte_Array);
        if (payload == NULL)
        {
            chunk_errno = CHUNK_ERR_HEIGHT;
            goto chunk_new_cleanup;
        }

        height = tag->meta->length / CHUNK_SIZE_AREA;

        if (new->height == -1)
            new->height = height;
        else if (new->height != height)
        {
            chunk_errno = CHUNK_ERR_HEIGHT;
            goto chunk_new_cleanup;
        }

        // Assigns the payload to the appropriate member of the struct
        *(uint8_t**)((char*)new + map->offset) = payload;
    }

    /*
    new->blockdata = nbt_payload(nbt_hash_search(new->data, L"Data"), TAG_Byte_Array);
    new->skylight = nbt_payload(nbt_hash_search(new->data, L"SkyLight"), TAG_Byte_Array);
    new->blocklight = nbt_payload(nbt_hash_search(new->data, L"BlockLight"), TAG_Byte_Array);
    new->blocks = nbt_payload(nbt_hash_search(new->data, L"Blocks"), TAG_Byte_Array);
    */
    new->heightmap = nbt_payload(nbt_hash_search(new->data, L"HeightMap"), TAG_Byte_Array);

    chunk_errno = CHUNK_ERR_OK;
    return new;

chunk_new_coord_error:
    chunk_errno = CHUNK_ERR_COORDS;
    // XXX FALLTHROUGH

chunk_new_cleanup:
    if (file != Z_NULL)
        gzclose(file);
    
    if (new != NULL)
        chunk_free(new);

    return NULL;
}

void chunk_free(void *doomed)
{
    chunk *c = (chunk*)doomed;
    if (doomed == NULL)
        return;

    if (c->data != NULL)
        nbt_free_tag(c->data);

    free(c);
}

int chunk_get_coords_from_filename(char *filename, int32_t *x_coord, int32_t *z_coord)
{
    char *position_start, *position_end;
    int pos_len;
    extern int base36_errno;

    position_start = strchr(filename, '.');
    if (position_start == NULL)
        return -1;

    position_start++;
    if (*position_start == 0)
        return -1;

    position_end = strchr(position_start, '.');
    if (position_end == NULL)
        return -1;

    pos_len = position_end - position_start;

    *(x_coord) = base36tobase10(position_start, pos_len);
    if (base36_errno)
        return -1;

    position_start = position_end + 1;
    if (*position_start == 0)
        return -1;

    position_end = strchr(position_start, '.');
    if (position_end == NULL)
        return -1;

    pos_len = position_end - position_start;

    *(z_coord) = base36tobase10(position_start, pos_len);
    if (base36_errno)
        return -1;

    return 0;
}

uint64_t chunk_generate_key(chunk *c)
{
    if (c == NULL)
        return 0;

    return chunk_generate_key_from_coords(c->coord_x, c->coord_z);
}

uint64_t chunk_generate_key_from_coords(int32_t coord_x, int32_t coord_z)
{
    uint64_t key;

    key = (((uint64_t)coord_x) << 32) | ((uint32_t)coord_z);

    return key;
}

uint32_t chunk_hash(void *_c)
{
    chunk *c = (chunk*)_c;
    uint64_t key;

    key = chunk_generate_key(c);

    key = (~key) + (key << 18);
    key = key ^ uint64_ror(key, 31);
    key = (key + (key << 2)) + (key << 4);
    key = key ^ uint64_ror(key, 11);
    key = key + (key << 6);
    key = key ^ uint64_ror(key, 22);

    return (uint32_t) key;
}

int chunk_key_eqfn(void *key1, void *key2)
{
    return *((uint64_t*)key1) == *((uint64_t*)key2);
}

int32_t chunk_generate_8bit_offset(chunk *c, uint8_t coord_x, uint8_t coord_y, uint8_t coord_z, uint32_t max_offset)
{
    int32_t slice_offset;

    slice_offset = (coord_z * c->height) + (coord_x * c->height * CHUNK_SIZE_X) + coord_y;
    
    if (slice_offset >= max_offset)
        return -1;
    return slice_offset;
}

int32_t chunk_generate_4bit_offset(chunk *c, uint8_t coord_x, uint8_t coord_y, uint8_t coord_z, uint32_t max_offset)
{
    int32_t slice_offset;
    //slice_offset = floor(((coord_z * c->height) + (coord_x * c->height * CHUNK_SIZE_X) + coord_y) / 2);
    slice_offset = ((coord_z * c->height) + (coord_x * c->height * CHUNK_SIZE_X) + coord_y) / 2;

    if (slice_offset >= max_offset)
        return -1;
    return slice_offset;
}
