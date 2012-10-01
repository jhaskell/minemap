#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include "config.h"

int parse_commandline_options(int argc, char **argv, configuration *config)
{
    static struct option long_options[] =
    {
        {"help",    no_argument,       0, 'h'},
        {"output",  required_argument, 0, 'o'},
        {"version", no_argument,       0, 'v'},
        {0, 0, 0, 0}
    };
    int            option_index = 0, option, c;
    char           date[CONFIG_BUFFER_SIZE];
    int            dateSize, filenameSize;
    struct timeval tv;
    struct tm      *tm;
    int32_t tile_x, tile_z;

    // Set defaults for the configuration
    (*config).output_filename = (char*)0;
    (*config).free_output_filename = 0;

    while ((c = getopt_long(argc, argv, "ho:v", long_options, &option_index)) != -1)
    {
        switch (c)
        {
            case 'h':
                return CONFIG_ERROR_PRINT_HELP;
                break;
            case 'o':
                (*config).output_filename = (char*)optarg;
                break;
            case 'v':
                return CONFIG_ERROR_PRINT_VERSION;
                break;
        }
    }

    if (optind >= argc)
    {
        return CONFIG_ERROR_NO_INPUT;
    }
    else
    {
        (*config).input_path = argv[optind];
        if (argc - optind == 3)
        {
            sscanf(argv[optind + 1], "%d", &tile_x);
            sscanf(argv[optind + 2], "%d", &tile_z);

            config->tile_x = tile_x;
            config->tile_z = tile_z;
        }
        else
        {
            config->tile_x = 0;
            config->tile_z = 0;
        }

        if ((*config).output_filename == (char*)0)
        {
            gettimeofday(&tv, NULL);

            if ((tm = localtime(&tv.tv_sec)) != NULL)
            {
                strftime(date, sizeof date, "%Y%m%d_%H%M%S", tm);
                (*config).output_filename = malloc(sizeof(char) * CONFIG_BUFFER_SIZE);
                (*config).free_output_filename = 1;
                snprintf((*config).output_filename, CONFIG_BUFFER_SIZE, "images/minemap_%s_%i_%i.png", date, config->tile_x, config->tile_z);
            }
            else
            {
                return CONFIG_ERROR_LOCALTIME;
            }
        }
    }

    return 0;
}

void free_config(configuration *config)
{
    if ((*config).free_output_filename == 1)
    {
        free((*config).output_filename);
        (*config).output_filename = (char*)0;
    }
}

char *config_error_message(int error_code)
{
    static char *error_messages[] = {
        "No input path was specified.",
        "Could not allocate memory.",
        "HELP",
        "VERSION",
        "Could not retrieve timestamp",
    };

    if (error_code < 1 || error_code > 5)
        return (char*)0;

    return error_messages[error_code - 1];
}
