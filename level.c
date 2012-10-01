#include <stdio.h>
#include <string.h>
#include <zlib.h>
#include <stdlib.h>
#include <wchar.h>
#include <math.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "level.h"
#include "chunk.h"
#include "maths.h"

level *level_load(char *path)
{
    gzFile file;
    level *new;
    char *filename_buffer;
    int filename_length = strlen(path) + 15;
    nbt_tag *tag;
    struct hashtable *hash;

    filename_buffer = calloc(filename_length, sizeof(char));
    if (filename_buffer == NULL)
        goto level_load_error;

    new = calloc(1, sizeof(level));
    if (new == NULL)
        goto level_load_error;

    new->input_path = path;

    if (snprintf(filename_buffer, filename_length, "%s/level.dat", new->input_path) == 0)
        goto level_load_error;

    file = gzopen(filename_buffer, "r");
    if (file == Z_NULL)
        goto level_load_error;

    new->data = (nbt_tag*)nbt_read(file, 0);
    if (new->data == NULL)
        goto level_load_error;

    free(filename_buffer);
    filename_buffer = NULL;

    gzclose(file);
    file = Z_NULL;

    if (wcsncmp(new->data->name, L"(null)", 7) == 0)
    {
        hash = (struct hashtable *)nbt_payload(new->data, TAG_Compound);
        if (hash == NULL)
            goto level_load_error;

        tag = hashtable_remove(hash, L"Data");
        if (tag == NULL)
            goto level_load_error;

        nbt_free_tag(new->data);
        new->data = tag;
    }

    return new;

level_load_error:
    if (filename_buffer != NULL)
        free(filename_buffer);

    if (file == Z_NULL)
        gzclose(file);

    if (new != NULL)
        level_free(new);

    return NULL;
}

void level_free(level *doomed)
{
    if (doomed->data != NULL)
        nbt_free_tag(doomed->data);

    free(doomed);
}

chunk *level_get_chunk_at(level *lvl, int coord_x, int coord_z)
{
    int directory_x, directory_z;
    char dir_x_base36[LEVEL_BASE_36_SIZE], dir_z_base36[LEVEL_BASE_36_SIZE];
    char coord_x_base36[LEVEL_BASE_36_SIZE], coord_z_base36[LEVEL_BASE_36_SIZE];
    char input_file[LEVEL_BUFFER_SIZE];
    struct stat st;

    if (base10tobase36(coord_x, coord_x_base36, LEVEL_BASE_36_SIZE))
        return NULL;

    if (base10tobase36(coord_z, coord_z_base36, LEVEL_BASE_36_SIZE))
        return NULL;

    directory_x = ((unsigned int)coord_x) % 64;
    directory_z = ((unsigned int)coord_z) % 64;

    if (base10tobase36(directory_x, dir_x_base36, LEVEL_BASE_36_SIZE))
        return NULL;

    if (base10tobase36(directory_z, dir_z_base36, LEVEL_BASE_36_SIZE))
        return NULL;

    if (snprintf(input_file, LEVEL_BUFFER_SIZE, 
            "%s/%s/%s/c.%s.%s.dat", 
            lvl->input_path,
            dir_x_base36,
            dir_z_base36,
            coord_x_base36,
            coord_z_base36
        ) == -1)
    {
        return NULL;
    }

    if (stat(input_file, &st))
    {
        return NULL;
    }

    return chunk_new(input_file, coord_x, coord_z);
}

void level_get_dimensions(level *lvl)
{
    DIR *world, *x_dir, *z_dir;
    struct dirent *world_entry, *x_entry, *z_entry;
    int smallest_x = 0, smallest_z = 0, largest_x = 0, largest_z = 0;
    int32_t chunk_x, chunk_z;
    uint64_t chunk_count = 0;
    struct stat st;
    char x_path[LEVEL_BUFFER_SIZE], z_path[LEVEL_BUFFER_SIZE], chunk_path[LEVEL_BUFFER_SIZE];
    const char *format = "%s/%s";
    extern int errno;

    if (lvl == NULL)
        return;

    stat(lvl->input_path, &st);
    if (!S_ISDIR(st.st_mode))
        return;

    world = opendir(lvl->input_path);
    if (world == NULL)
        return;

    while ((world_entry = readdir(world)) != NULL)
    {
        if (world_entry->d_name[0] == '.')
            continue;

        if (snprintf(x_path, LEVEL_BUFFER_SIZE, format, lvl->input_path, world_entry->d_name) == -1)
            continue;

        if (stat(x_path, &st))
            continue;

        if (S_ISDIR(st.st_mode))
        {
            x_dir = opendir(x_path);
            if (x_dir == NULL)
                continue;

            while ((x_entry = readdir(x_dir)) != NULL)
            {
                if (x_entry->d_name[0] == '.')
                    continue;

                if (snprintf(z_path, LEVEL_BUFFER_SIZE, format, x_path, x_entry->d_name) == -1)
                    continue;

                if (stat(z_path, &st))
                    continue;

                if (S_ISDIR(st.st_mode))
                {
                    z_dir = opendir(z_path);
                    if (z_dir == NULL)
                        continue;

                    while ((z_entry = readdir(z_dir)) != NULL)
                    {
                        if (z_entry->d_name[0] == '.')
                            continue;

                        if (snprintf(chunk_path, LEVEL_BUFFER_SIZE, format, z_path, z_entry->d_name) == -1)
                            continue;

                        if (stat(chunk_path, &st))
                            continue;

                        if (S_ISREG(st.st_mode) && 
                            chunk_get_coords_from_filename(chunk_path, &chunk_x, &chunk_z) == 0
                           )
                        {
                            chunk_count++;

                            if (chunk_x < smallest_x)
                                smallest_x = chunk_x;
                            if (chunk_z < smallest_z)
                                smallest_z = chunk_z;

                            if (chunk_x > largest_x)
                                largest_x = chunk_x;
                            if (chunk_z > largest_z)
                                largest_z = chunk_z;
                        }
                    }
                    closedir(z_dir);
                }
            }
            closedir(x_dir);
        }
    }
    closedir(world);

    lvl->smallest_x = smallest_x;
    lvl->smallest_z = smallest_z;
    lvl->largest_x = largest_x;
    lvl->largest_z = largest_z;
    lvl->chunk_count = chunk_count;
}
