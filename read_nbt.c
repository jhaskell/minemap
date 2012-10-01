#include <zlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <endian.h>
#include <wchar.h>
#include "nbt.h"
#include "read_nbt.h"
#include "linked_list.h"
#include "hashtable.h"
#include "hashtable_itr.h"
#include "utf8.h"

int nbt_read_error;

nbt_tag *nbt_read(gzFile file, int force_tag_type)
{
    // Different TAG_End tags don't really differ from one another, so we can
    // save a little memory here by only making one and just passing a pointer
    // to it when we need it.
    static nbt_tag end_tag = {TAG_End, NULL, 0, NULL, NULL};

    nbt_tag *tag, *child_tag;
    uint8_t tag_type = 0, child_tag_type = 0, payload_size = 0;
    int16_t name_length = 0, string_length = 0;
    int32_t byte_array_length = 0, i = 0, list_length = 0;
    wchar_t *name = NULL, *string_payload, *child_name = NULL;
    uint8_t *byte_array_payload, buffer[NBT_READ_BUFFER_SIZE];

    if (file == NULL)
    {
        return NULL;
    }

    memset(buffer, 0, NBT_READ_BUFFER_SIZE);

    // Read the tag type from the file
    if (force_tag_type > 0)
    {
        tag_type = force_tag_type;
    }
    else
    {
        if (nbt_read_gzread(file, (void*)&tag_type, sizeof(tag_type)))
            goto nbt_read_error;
    }

    // If it's a TAG_End, we can stop here
    if (tag_type == TAG_End)
        tag = &end_tag;
    else
    {
        if (force_tag_type == 0)
        {
            if (nbt_read_gzread(file, (void*)&name_length, sizeof(name_length)))
                goto nbt_read_error;

            // Convert big endian number to local format
            name_length = be16toh(name_length);
            if (name_length < 0)
            {
                //printf("Name length malformed\n");
                goto nbt_read_malformed;
            }
            else if (name_length == 0)
            {
                name_length = 6;
                name = calloc(name_length + 1, sizeof(wchar_t));
                if (name == NULL)
                    goto nbt_read_mem_error;
                wcsncpy(name, L"(null)", name_length);
            }
            else
            {
                name = calloc(name_length + 1, sizeof(wchar_t));
                if (name == NULL)
                    goto nbt_read_mem_error;

                if (nbt_read_gzread_utf8(file, name, name_length))
                    goto nbt_read_error;
            }
            //printf("Name: %ls\n", name);
        }

        if (nbt_is_simple_tag_type(tag_type))
        {
            payload_size = nbt_get_payload_size(tag_type);
            memset(buffer, 0, NBT_READ_BUFFER_SIZE);

            if (nbt_read_gzread(file, (void*)buffer, payload_size))
                goto nbt_read_error;

            tag = nbt_new_simple_tag(tag_type, name, name_length, buffer);
        }
        else
            switch (tag_type)
            {
                case TAG_Byte_Array:
                    if (nbt_read_gzread(file, (void*)&byte_array_length, sizeof(byte_array_length)))
                        goto nbt_read_error;

                    byte_array_length = be32toh(byte_array_length);
                    if (byte_array_length < 1)
                    {
                        //printf("Byte array length malformed\n");
                        goto nbt_read_malformed;
                    }

                    byte_array_payload = calloc(byte_array_length, sizeof(uint8_t));
                    if (byte_array_payload == NULL)
                        goto nbt_read_mem_error;

                    if (nbt_read_gzread(file, (void*)byte_array_payload, byte_array_length))
                        goto nbt_read_error;

                    tag = nbt_new_byte_array_tag(byte_array_payload, name, name_length, byte_array_length);
                    break;

                case TAG_String:
                    //printf("Reading TAG_String\n");
                    if (nbt_read_gzread(file, (void*)&string_length, sizeof(string_length)))
                        goto nbt_read_error;
                    
                    string_length = be16toh(string_length);
                    if (string_length < 0)
                    {
                        goto nbt_read_malformed;
                    }
                    
                    if (string_length > 0)
                    {
                        string_payload = calloc(string_length + 1, sizeof(wchar_t));
                        if (string_payload == NULL)
                            goto nbt_read_mem_error;

                        if (nbt_read_gzread_utf8(file, string_payload, string_length))
                            goto nbt_read_error;

                    }
                    else
                        string_payload = NULL;
                    tag = nbt_new_string_tag(string_payload, name, name_length, string_length);
                    break;

                case TAG_List:
                    if (nbt_read_gzread(file, (void*)&child_tag_type, sizeof(child_tag_type)))
                        goto nbt_read_error;

                    if (!nbt_is_valid_tag_type(child_tag_type))
                    {
                        //printf("List child tag type malformed\n");
                        goto nbt_read_malformed;
                    }

                    tag = nbt_new_list_tag(name, name_length, child_tag_type);

                    if (nbt_read_gzread(file, (void*)&list_length, sizeof(list_length)))
                        goto nbt_read_error;

                    list_length = be32toh(list_length);
                    if (list_length < 0)
                    {
                        //printf("List length: %i\n", list_length);
                        goto nbt_read_malformed;
                    }
                    
                    for (i = 0; i < list_length; i++)
                    {
                        child_tag = nbt_read(file, child_tag_type);
                        if (child_tag == NULL)
                        {
                            nbt_free_tag(tag);
                            tag = NULL;
                            break;
                        }
                        else if (child_tag->type == TAG_End)
                        {
                            //printf("Found TAG_End in a TAG_List's children\n");
                            goto nbt_read_malformed;
                        }
                        else
                            list_push(tag->payload->list_payload, child_tag);
                    }
                    break;

                case TAG_Compound:
                    tag = nbt_new_compound_tag(name, name_length);

                    do
                    {
                        child_tag = nbt_read(file, 0);
                        if (child_tag == NULL)
                        {
                            nbt_free_tag(tag);
                            tag = NULL;
                            break;
                        }

                        if (child_tag->type != TAG_End)
                        {
                            // Copy the child's name to use as a key
                            child_name = calloc(child_tag->name_len + 1, sizeof(wchar_t));
                            wcsncpy(child_name, child_tag->name, child_tag->name_len);

                            hashtable_insert(tag->payload->compound_payload, child_name, child_tag);
                        }

                    } while (child_tag != NULL && child_tag->type != TAG_End);

                    break;
            }
    }

    //nbt_print_single(tag, 2);
    return tag;

nbt_read_mem_error:
    nbt_read_error = NBT_READ_OUT_OF_MEM;
    goto nbt_read_error;

nbt_read_malformed:
    nbt_read_error = NBT_READ_MALFORMED_INPUT;
    goto nbt_read_error;

nbt_read_error:

    if (name != NULL)
        free(name);
    return NULL;
}

int nbt_read_gzread(gzFile file, void *buffer, int length)
{
    int total_bytes_read = 0, bytes_read, i, byte;

    do
    {
        bytes_read = gzread(file, buffer + total_bytes_read, length - total_bytes_read);

        for (i = 0; i < bytes_read; i++)
        {
            byte = *((uint8_t*)(buffer + total_bytes_read + i));
        }

        if (bytes_read > 0)
            total_bytes_read += bytes_read;

    } while (total_bytes_read < length && bytes_read > 0);

    if (total_bytes_read < length)
    {
        if (gzeof(file))
            nbt_read_error = NBT_READ_PREMATURE_EOF;
        else
            nbt_read_error = NBT_READ_GZREAD_ERROR;

        return (-1);
    }

    return 0;
}

int nbt_read_gzread_utf8(gzFile file, wchar_t *wchar_buffer, int length)
{
    static unsigned char utf8_data[NBT_READ_UTF8_BUFFER_SIZE];
    int bytes_read;

    if (length < 1)
        return 0;

    memset(utf8_data, 0, NBT_READ_UTF8_BUFFER_SIZE);

    if (nbt_read_gzread(file, utf8_data, length))
        return (-1);

    bytes_read = utf8_to_wchar(utf8_data, length, wchar_buffer, length, 0);
    if (bytes_read == 0)
    {
        nbt_read_error = NBT_READ_INVALID_UTF8;
        return (-1);
    }

    return 0;
}


