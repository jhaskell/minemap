/** \file nbt.h
  * \brief Data structures and functions for Named Binary Tag \b (NBT) objects
  */
#ifndef NBT_H
#define NBT_H

#include <stdint.h>
#include <stdarg.h>
#include <wchar.h>
#include "linked_list.h"
#include "hashtable_private.h"
#include "hashtable.h"

/** \brief Details the different kinds of tags found in an NBT file. 
  */
enum nbt_tag_types
{
    TAG_Invalid = -1,   /**< \brief Used to indicate an invalid or unsupported
                          *         tag */
    TAG_End,            /**< \brief Used to indicate the end of a TAG_Compound
                          *         collection */
    TAG_Byte,           /**< \brief Holds a single signed byte of data */
    TAG_Short,          /**< \brief Holds a signed 16-bit integer */
    TAG_Int,            /**< \brief Holds a signed 32-bit integer */
    TAG_Long,           /**< \brief Holds a signed 64-bit integer */
    TAG_Float,          /**< \brief Holds a 32-bit floating point value */
    TAG_Double,         /**< \brief Holds a 64-bit floating point value */
    TAG_Byte_Array,     /**< \brief Holds a fixed-length unsigned char array */
    TAG_String,         /**< \brief Holds a wchar_t* utf-8 string */
    TAG_List,           /**< \brief Holds an ordered, fixed-length list of 
                          *         other tags */
    TAG_Compound,       /**< \brief Holds an unordered, variable length %list of
                          *         tags with unique names in a hashtable */
};

/** \brief Flags for print_hex().  */
enum hex_dump_flags
{
    HEX_CHAR,
    HEX_WCHAR_T,
};

/** \brief The smallest value for a valid tag. */
#define NBT_TAG_TYPE_MIN TAG_End
/** \brief The largest value for a valid tag. */
#define NBT_TAG_TYPE_MAX TAG_Compound
/** \brief The smallest value for a valid simple tag. */
#define NBT_TAG_TYPE_SIMPLE_MIN TAG_Byte
/** \brief The largest value for a valid simple tag. */
#define NBT_TAG_TYPE_SIMPLE_MAX TAG_Double

/** \brief The inital number of hash buckets to use for TAG_Compound tags. */
#define NBT_TAG_HASH_BUCKETS 16

/** \brief A void function pointer */
typedef void(*vfp)(void*);

/** \name Primitive Tag Creation macros
  * \return A new nbt_tag of the appropriate type, or NULL on error
  *
  * Individual functions for creating the primitive nbt_tag structures.
  */
/*@{*/
/** \brief Creates a new NBT_Byte tag */
#define nbt_new_byte_tag(name,name_len,payload) nbt_new_simple_tag(TAG_Byte, name, name_len, (void*)&payload)
/** \brief Creates a new NBT_Short tag */
#define nbt_new_short_tag(name,name_len,payload) nbt_new_simple_tag(TAG_Short, name, name_len, (void*)&payload)
/** \brief Creates a new NBT_Int tag */
#define nbt_new_int_tag(name,name_len,payload) nbt_new_simple_tag(TAG_Int, name, name_len, (void*)&payload)
/** \brief Creates a new NBT_Long tag */
#define nbt_new_long_tag(name,name_len,payload) nbt_new_simple_tag(TAG_Long, name, name_len, (void*)&payload)
/** \brief Creates a new NBT_Float tag */
#define nbt_new_float_tag(name,name_len,payload) nbt_new_simple_tag(TAG_Float, name, name_len, (void*)&payload)
/** \brief Creates a new NBT_Double tag */
#define nbt_new_double_tag(name,name_len,payload) nbt_new_simple_tag(TAG_Double, name, name_len, (void*)&payload)
/*@}*/

/** \brief Contains the payload for an nbt_tag.  */
typedef union {
    int8_t  byte_payload;        /**< \brief Payload for TAG_Byte tags. */
    int16_t short_payload;       /**< \brief Payload for TAG_Short tags. */
    int32_t int_payload;         /**< \brief Payload for TAG_Int tags. */
    int64_t long_payload;        /**< \brief Payload for TAG_Long tags. */
    float   float_payload;       /**< \brief Payload for TAG_Float tags. */
    double  double_payload;      /**< \brief Payload for TAG_Double tags. */
    uint8_t *byte_array_payload; /**< \brief Payload for TAG_Byte_Array tags. */
    wchar_t *string_payload;     /**< \brief Payload for TAG_String tags. */
    list    *list_payload;       /**< \brief Payload for TAG_List tags. */
    struct hashtable *compound_payload; /**< \brief Payload for TAG_Compound tags. */
} u_tag_payload;

/** \brief Contains meta-information for an nbt_tag. */
typedef union {
    int32_t length;         /**< \brief The length of a TAG_Byte_Array or TAG_String tag. */
    int8_t  child_tag_type; /**< \brief The type of child tags of a TAG_List tag. */
} u_tag_meta;

/** \brief Contains a single NBT tag entry. 
  */
typedef struct 
{
    uint8_t         type;     /**< \brief The tag type, which must be a member of #nbt_tag_types. */
    wchar_t        *name;     /**< \brief The name of the tag. Omitted for TAG_End tags. */
    uint16_t        name_len; /**< \brief The length of the name in characters */
    u_tag_meta     *meta;     /**< \brief The meta-information for a tag. */
    u_tag_payload  *payload;  /**< \brief The payload for this tag. */
} nbt_tag;

/** \brief Create a new nbt_tag. Do not call.
  * \param[in] tag_type       The type for the new tag. Must be a member of #nbt_tag_types. 
  * \param[in] name           The name of the tag. TAG_End types must pass null.
  * \param[in] name_len       The length of the name in characters
  * \param[in] meta           The meta-information for the tag.
  * \param[in] payload        Payload for the tag.
  * \return A new nbt_tag, or NULL if an error occured.
  *
  * Creates a new tag with the given parameters. You should not call this function directly; use the individual tag creation functions instead.
  */
nbt_tag *nbt_new_tag(uint8_t tag_type, wchar_t *name, uint16_t name_len, u_tag_meta *meta, u_tag_payload *payload);

/** \brief Frees a tag, its payload, and its metadata.
  * \param doomed The tag to free
  * \return void
  */
void nbt_free_tag(nbt_tag *doomed);

/** \brief Creates a new simple tag. Used in macros.
  * \param tag_type      The type of the new tag. Must be a member of #nbt_tag_types
  * \param name          The name of the new tag
  * \param name_len      The length of the name in characters
  * \param payload_value A pointer to the value of the new tag.
  *
  * Do not call this function directly; use the macros instead.
  */
nbt_tag *nbt_new_simple_tag(uint8_t tag_type, wchar_t *name, uint16_t name_len, void *payload_value);

/** \brief Generates a u_tag_payload for a simple type. Used in macros.
  * \param tag_type The type of the tag. Must be a member of #nbt_tag_types
  * \param payload_value A pointer to the payload.
  * \return A new u_tag_payload, or NULL if no memory could be allocated or if tag_type was out of bounds.
  *
  * Do not call this function directly; use the macros instead.
  */
u_tag_payload *nbt_new_simple_payload(uint8_t tag_type, void *payload_value);

/** \brief Gets the size of a payload for a simple tag type.
  * \param[in] tag_type The type of tag
  * \return The size of the payload for the tag, or 0 if an invalid type is passed
  */
uint8_t nbt_get_payload_size(uint8_t tag_type);

/** \brief Checks a tag type to see if it is valid
  * \param[in] tag_type The type of tag
  * \return 1 if the tag is valid, 0 if it is not
  */
uint8_t nbt_is_valid_tag_type(uint8_t tag_type);

/** \brief Checks a tag type to see if it is a valid simple tag type
  * \param[in] tag_type The type of tag
  * \return 1 if the tag is a valid simple type, 0 if it is not
  */
uint8_t nbt_is_simple_tag_type(uint8_t tag_type);

/** \name Complex Tag Creation functions
  * \return A new nbt_tag of the appropriate type, or NULL on error
  * 
  * Individual functions for creating more complex tags.
  */
/*@{*/
/** \brief Creates a new NBT_Byte_Array tag
  * \param[in] payload_value The payload for the tag
  * \param[in] name          The name of the tag
  * \param[in] name_len      The length of the name in characters
  * \param[in] length        The length of the payload, in bytes
  */
nbt_tag *nbt_new_byte_array_tag(uint8_t *payload_value, wchar_t *name, uint16_t name_len, int32_t length);

/** \brief Creates a new NBT_String tag
  * \param[in] payload_value The payload for the tag, as an UTF-8 string.
  * \param[in] name          The name of the tag
  * \param[in] name_len      The length of the name in characters
  * \param[in] length        The length of the string, in characters.
  */
nbt_tag *nbt_new_string_tag(wchar_t *payload_value, wchar_t *name, uint16_t name_len, int32_t length);

/** \brief Creates a new NBT_List tag
  * \param[in] name           The name of the tag
  * \param[in] name_len      The length of the name in characters
  * \param[in] child_tag_type The type of child tags in payload
  */
nbt_tag *nbt_new_list_tag(wchar_t *name, uint16_t name_len, int8_t child_tag_type);

/** \brief Creates a new NBT_Compound tag
  * \param[in] name The name of the tag
  * \param[in] name_len      The length of the name in characters
  */
nbt_tag *nbt_new_compound_tag(wchar_t *name, uint16_t name_len);
/*@}*/

/** \name NBT Accessor Functions
  * \brief Syntactical Sugar for accessing nbt_tag payloads
  */
/*@{*/
/** \brief Retrieve the payload
  * \param tag              The tag whose payload you wish to retrieve
  * \param expected_type    What type you expect this tag to be. See 
  *                         #nbt_tag_types
  * \return A pointer to the payload, or NULL if the type of the tag is not as
  *         expected or the payload is simply missing
  */
void *nbt_payload(nbt_tag *tag, uint8_t expected_type);

/** \brief Search a TAG_Compound hash for a given key
  * \param tag  The tag to search
  * \param key  The key to look for
  * \return The tag for the given key, NULL if the tag was not a TAG_Compound,
  *         or NULL if the key was not found
  */
nbt_tag *nbt_hash_search(nbt_tag *tag, wchar_t *key);
/*@}*/

/** \name NBT Pretty Print Functions
  * \brief Functions used to print out an NBT tag
  */
/*@{*/

/** \brief Prints an nbt_tag to stdout
  * \param[in] tag An nbt_tag.
  */
void nbt_print(nbt_tag *tag);

/** \brief Prints a single nbt_tag with a leading indent
  * \param[in] tag An nbt_tag
  * \param[in] indent A number of spaces to print before printing the tag
  */
void nbt_print_single(nbt_tag *tag, int indent);

/** \brief Prints multiple spaces
  * \param indent The number of spaces to print
  */
void print_indent(int indent);

/** \brief Prints a hex dump of a *char or a *wchar_t
  * \param string A char* or wchar_t* string to hex dump
  * \param string_length The length of the string
  * \param indent A number of spaces to print before each line
  * \param flags  Flag to tell function what kind of string is being passed in. See #hex_dump_flags.
  */
void hex_dump(void *string, int string_length, int indent, int flags);
/*@}*/

/** \name Hash functions
  * \brief Functions necessary to store nbt_tag data in a hashtable.
  */
/*@{*/
/** \brief Generates a hash value for a Named Binary Tag
  * \param name The name of a tag, cast as a void pointer
  * \return A hash value for the tag.
  */
unsigned int nbt_hash_fn(void *name);

/** \brief Compares two tags to see if their names match
  * \param name1 The first name to compare (wchar_t* cast as void*)
  * \param name2 The first name to compare (wchar_t* cast as void*)
  * \return 1 if the tags have the same name, 0 if they don't
  */
int nbt_name_eq(void *name1, void *name2);
/*@}*/

#endif
