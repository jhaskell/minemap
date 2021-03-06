/** \file config.h
  * \brief Routines to populate and free %configuration structs
  */
#ifndef CONFIG_H
#define CONFIG_H

/** \brief The version of the program */
#define MINEMAP_VERSION "0.0.1"
/** \brief How large of a string should be allocated for default output filenames. */
#define CONFIG_BUFFER_SIZE 64

/** \brief Error codes sent by parse_commandline_options when an error is found.
 */
enum config_errors 
{ 
    CONFIG_ERROR_NO_INPUT = 1, 
    CONFIG_ERROR_NO_MEM, 
    CONFIG_ERROR_PRINT_HELP, 
    CONFIG_ERROR_PRINT_VERSION,
    CONFIG_ERROR_LOCALTIME
};

/** \brief Contains %configuration information. 
  * \details Populate this struct with parse_commandline_options(), described in config.h. 
  *
  * Additionally, be sure to call free_config() before the program terminates, or memory leaks will occur.
  */
typedef struct 
{
    char          *output_filename;       /**< The name of the file which will hold the final rendered image. Will be autogenerated if missing. */
    unsigned char  free_output_filename;  /**< Internal flag to tell whether or not output_filename should be freed. */
    char          *input_path;            /**< Location of the directory containing the map chunks, as provided by the user. */
    int32_t       tile_x;                 /**< The X coordinate of the desired tile. */
    int32_t       tile_z;                 /**< The Z coordinate of the desired tile. */
} configuration;

/** \brief Parses commandline arguments and populates config struct.
 * \param[in] argc The number of arguments in argv.
 * \param[in] argv An array of strings representing each word in the argument.
 * \param[out] config A pointer to a configuration struct.
 * \return 0 on success, >0 on error. Error messages can be retrieved using config_error_message().
 *
 * Takes the argc and argv provided by int main() and populates the configuration struct based on the arguments provided. If an error occurs, the function returns an error code from #config_errors and the program should terminate.
 * 
 * Use free_config() to free memory allocated by this function.
 */
int parse_commandline_options(int argc, char **argv, configuration *config);

/** \brief Frees any memory that was allocated 
  * \param[in] config A pointer to a configuration struct.
  * \return void
  *
  * Frees any memory that was allocated in the process of populating the configuration struct. This must be called before the program terminates, or memory will leak. 
  *
  * \b Warning: This function does not free the struct itself, only memory allocated in the process of populating the struct. 
  */
void free_config(configuration *config);

/** \brief Returns a message associated with an error code
  * \param[in] error_code An error code, which must be a member of #config_errors
  * \return A pointer to the error message associated with error_code, or null if the error code is out of bounds
  */
char *config_error_message(int error_code);
#endif
