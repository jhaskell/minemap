#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <zlib.h>
#include <dirent.h>
#include <string.h>
#include <locale.h>
#include <math.h>

#include "main.h"
#include "config.h"
#include "nbt.h"
#include "level.h"
#include "chunk.h"
#include "colors.h"
#include "colors/hardcoded.h"
#include "renderer.h"
#include "renderers/flat.h"

int main (int argc, char **argv)
{
    configuration config;
    level *l = NULL;
    color_map *map = NULL;
    renderer *r = NULL;
    int errcode = 0, i, tile_x, tile_z, tile_x_start, tile_x_end, tile_z_start, tile_z_end;
    char filename_buffer[256];

    if (!setlocale(LC_CTYPE, ""))
    {
        fprintf(stderr, "Can't set the specified local! Check LANG, LC_CTYPE, LC_ALL\n");
        return 1;
    }

    if ((errcode = parse_commandline_options(argc, argv, &config)) != 0)
    {
        printf("Error parsing configuration: %s\n", config_error_message(errcode));
        goto main_cleanup;
    }

    map = color_map_hardcoded_new(256, 4);
    if (map == NULL)
    {
        printf("Error loading color map\n");
        goto main_cleanup;
    }

    l = level_load(config.input_path);
    if (l == NULL)
    {
        printf("Error loading level\n");
        goto main_cleanup;
    }

    /*
    level_get_dimensions(l);

    tile_x_start = (int)floor(l->smallest_x / 16) - 1;
    tile_x_end = (int)floor(l->largest_x / 16);
    tile_z_start = (int)floor(l->smallest_z / 16) - 1;
    tile_z_end = (int)floor(l->largest_z / 16);
    */

    r = renderer_flat_new(l, map);
    if (r == NULL)
    {
        printf("Unable to initialize a new flat renderer\n");
        goto main_cleanup;
    }

    if (errcode = renderer_perform(r, config.tile_x, config.tile_z, config.output_filename))
    {
        printf("Rendering failed: %i\n", errcode);
    }

    /*
    for (tile_x = tile_x_start; tile_x <= tile_x_end; tile_x++)
        for (tile_z = tile_z_start; tile_z <= tile_z_end; tile_z++)
        {
            memset(filename_buffer, 0, 256);
            snprintf(filename_buffer, 256, "%s/tile_%i_%i.png", config.output_filename, tile_x, tile_z);
            if (errcode = renderer_perform(r, tile_x, tile_z, filename_buffer))
            {
                printf("Rendering failed: %i\n", errcode);
            }
        }
    */

main_cleanup:
    if (r != NULL)
        renderer_free(r);
    else
    {
        if (l != NULL)
            level_free(l);
        if (map != NULL)
            color_map_free(map);
    }

    free_config(&config);
}

void free_string(void *str)
{
    free(str);
}

void hex_print(void *hex, int len)
{
    int i;
    unsigned char c;

    for (i = 0; i < len; i++)
    {
        c = (unsigned char)*((unsigned char*)hex + i);
        printf("%.2X", c, i);
    }
}

void print_help(void)
{

}
