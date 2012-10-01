#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <endian.h>
#include <string.h>
#include <math.h>
#include "nbt.h"
#include "linked_list.h"
#include "main.h"

nbt_tag *nbt_new_tag(uint8_t tag_type, wchar_t *name, uint16_t name_len, u_tag_meta *meta, u_tag_payload *payload)
{
    nbt_tag *new; 

    new = malloc(sizeof(nbt_tag));
    if (new == NULL)
        return NULL;

    // Check to see if it's a valid tag type
    if (tag_type < NBT_TAG_TYPE_MIN || tag_type > NBT_TAG_TYPE_MAX)
    {
        new->type = TAG_Invalid;
    }
    else
    {
        new->type = tag_type;
    }

    new->name = name;
    new->name_len = name_len;
    new->meta = meta;
    new->payload = payload;

    return new;
}

void nbt_free_tag(nbt_tag *doomed)
{
    if (doomed == NULL)
        return;

    if (doomed->payload != NULL)
    {
        switch (doomed->type)
        {
            case TAG_End:
            case TAG_Byte:
            case TAG_Short:
            case TAG_Int:
            case TAG_Long:
            case TAG_Float:
            case TAG_Double:
                break;

            case TAG_Byte_Array:
                if (doomed->payload->byte_array_payload != NULL)
                    free(doomed->payload->byte_array_payload);
                break;
            case TAG_String:
                if (doomed->payload->string_payload != NULL)
                    free(doomed->payload->string_payload);
                break;

            case TAG_List:
                if (doomed->payload->list_payload != NULL)
                    list_free(doomed->payload->list_payload);
                break;

            case TAG_Compound:
                if (doomed->payload->compound_payload != NULL)
                    hashtable_destroy(doomed->payload->compound_payload, 1);
                break;
            default:
                // TODO: Handle unlisted tag type
                break;
        }

        free(doomed->payload);
    }

    if (doomed->meta != NULL)
    {
        free(doomed->meta);
    }

    if (doomed->name != NULL)
    {
        free(doomed->name);
    }

    free(doomed);
}

u_tag_payload *nbt_new_simple_payload(uint8_t tag_type, void *payload_value)
{
    u_tag_payload *payload;
    uint32_t float_temp;
    uint64_t double_temp;
    
    if (!nbt_is_simple_tag_type(tag_type))
        return NULL;

    payload = malloc(sizeof(u_tag_payload));
    if (payload == NULL)
        return NULL;

    switch (tag_type)
    {
        case TAG_Byte:
            payload->byte_payload = *((int8_t*)payload_value);
            break;
        case TAG_Short:
            payload->short_payload = be16toh(*((int16_t*)payload_value));
            break;
        case TAG_Int:
            payload->int_payload = be32toh(*((int32_t*)payload_value));
            break;
        case TAG_Long:
            payload->long_payload = be64toh(*((int64_t*)payload_value));
            break;
        case TAG_Float:
            float_temp = 0;
            float_temp = be32toh(*((uint32_t*)payload_value));
            memcpy((void*)&(payload->float_payload), (void*)&float_temp, sizeof(uint32_t));

            break;
        case TAG_Double:
            double_temp = 0;
            double_temp = be64toh(*((uint64_t*)payload_value));
            memcpy((void*)&(payload->double_payload), (void*)&double_temp, sizeof(uint64_t));
            break;
    }

    return payload;
}

nbt_tag *nbt_new_simple_tag(uint8_t tag_type, wchar_t *name, uint16_t name_len, void *payload_value)
{
    u_tag_payload *payload = nbt_new_simple_payload(tag_type, payload_value);

    if (payload == NULL)
        return NULL;

    return nbt_new_tag(tag_type, name, name_len, NULL, payload);
}

nbt_tag *nbt_new_byte_array_tag(uint8_t *payload_value, wchar_t *name, uint16_t name_len, int32_t length)
{
    u_tag_payload *payload;
    u_tag_meta    *meta;

    payload = malloc(sizeof(u_tag_payload));
    if (payload == NULL)
        return NULL;

    meta = malloc(sizeof(u_tag_meta));
    if (meta == NULL)
        return NULL;

    payload->byte_array_payload = payload_value;
    meta->length = length;

    return nbt_new_tag(TAG_Byte_Array, name, name_len, meta, payload);
}

nbt_tag *nbt_new_string_tag(wchar_t *payload_value, wchar_t *name, uint16_t name_len, int32_t length)
{
    u_tag_payload *payload;
    u_tag_meta    *meta;

    payload = malloc(sizeof(u_tag_payload));
    if (payload == NULL)
        return NULL;

    meta = malloc(sizeof(u_tag_meta));
    if (meta == NULL)
    {
        free(payload);
        return NULL;
    }

    payload->string_payload = payload_value;
    meta->length = length;

    return nbt_new_tag(TAG_String, name, name_len, meta, payload);
}

nbt_tag *nbt_new_list_tag(wchar_t *name, uint16_t name_len, int8_t child_tag_type)
{
    list          *new_list;
    u_tag_payload *payload;
    u_tag_meta    *meta;

    new_list = list_new((list_free_func)nbt_free_tag);
    if (new_list == NULL)
        return NULL;

    payload = malloc(sizeof(u_tag_payload));
    if (payload == NULL)
    {
        free(new_list);
        return NULL;
    }

    meta = malloc(sizeof(u_tag_meta));
    if (meta == NULL)
    {
        free(new_list);
        free(payload);
        return NULL;
    }

    meta->child_tag_type = child_tag_type;
    payload->list_payload = new_list;

    return nbt_new_tag(TAG_List, name, name_len, meta, payload);
}

nbt_tag *nbt_new_compound_tag(wchar_t *name, uint16_t name_len)
{
    struct hashtable *new_hash;
    u_tag_payload *payload;

    new_hash = create_hashtable(NBT_TAG_HASH_BUCKETS,
            nbt_hash_fn,
            nbt_name_eq,
            (vfp)nbt_free_tag);
    if (new_hash == NULL)
        return NULL;

    payload = malloc(sizeof(u_tag_payload));
    if (payload == NULL)
    {
        hashtable_destroy(new_hash, 1);
        return NULL;
    }

    payload->compound_payload = new_hash;

    return nbt_new_tag(TAG_Compound, name, name_len, NULL, payload);
}

uint8_t nbt_get_payload_size(uint8_t tag_type)
{
    static uint8_t tag_sizes[] = {
    0,  /**< \brief TAG_End payload size */
    1,  /**< \brief TAG_Byte payload size */
    2,  /**< \brief TAG_Short payload size */
    4,  /**< \brief TAG_Int payload size */
    8,  /**< \brief TAG_Long payload size */
    4,  /**< \brief TAG_Float payload size */
    8   /**< \brief TAG_Double payload size */
};

    if (!nbt_is_simple_tag_type(tag_type))
        return 0;

    return tag_sizes[tag_type];
}

uint8_t nbt_is_valid_tag_type(uint8_t tag_type)
{
    if (tag_type < NBT_TAG_TYPE_MIN || tag_type > NBT_TAG_TYPE_MAX)
        return 0;
    return 1;
}

uint8_t nbt_is_simple_tag_type(uint8_t tag_type)
{
    if (tag_type < NBT_TAG_TYPE_SIMPLE_MIN || tag_type > NBT_TAG_TYPE_SIMPLE_MAX)
        return 0;
    return 1;
}

void nbt_print(nbt_tag *tag)
{
    nbt_print_single(tag, 0);
}

void nbt_print_single(nbt_tag *tag, int indent)
{
    int i, j, rows;
    const static char *tag_names[] = {
        "TAG_End",
        "TAG_Byte",
        "TAG_Short",
        "TAG_Int",
        "TAG_Long",
        "TAG_Float",
        "TAG_Double",
        "TAG_Byte_Array",
        "TAG_String",
        "TAG_List",
        "TAG_Compound"
    };
    char *tag_name = NULL;
    list_node *iterator = NULL;
    struct hashtable_itr *hash_itr = NULL;
    nbt_tag *child = NULL;

    if (tag == NULL || tag->type == TAG_Invalid)
    {
        return;
    }

    print_indent(indent);

    if (nbt_is_valid_tag_type(tag->type))
        printf("%s", tag_names[tag->type]);
    else
        printf("TAG_UNKNOWN");

    if (tag->name != NULL)
        printf("(\"%ls\")", tag->name);
    printf(": ");

    if (tag->payload != NULL)
        switch (tag->type)
        {
            case TAG_Byte:
                printf("%i", tag->payload->byte_payload);
                break;
            case TAG_Short:
                printf("%i", tag->payload->short_payload);
                break;
            case TAG_Int:
                printf("%i", tag->payload->int_payload);
                break;
            case TAG_Long:
                printf("%lld", tag->payload->long_payload);
                break;
            case TAG_Float:
                printf("%.12f", tag->payload->float_payload);
                break;
            case TAG_Double:
                printf("%.24e", tag->payload->double_payload);
                break;
            case TAG_Byte_Array:
                if (tag->meta != NULL)
                {
                    printf("[%i bytes]", tag->meta->length);
                    /*
                    printf("\n");
                    hex_dump((void*)tag->payload->byte_array_payload, tag->meta->length, indent + 2, HEX_CHAR);
                    */
                }
                else
                    printf("[meta missing]");
                break;
            case TAG_String:
                printf("\"%ls\"", tag->payload->string_payload);
                /*
                printf("\n");
                hex_dump((void*)tag->payload->string_payload, tag->meta->length, indent + 2, HEX_WCHAR_T);
                */
                break;

            case TAG_List:
                printf("\n");
                print_indent(indent);
                printf("{\n");
                iterator = tag->payload->list_payload->start;
                while (iterator != NULL)
                {
                    nbt_print_single((nbt_tag*)iterator->data, indent + 2);
                    iterator = iterator->next;
                }
                print_indent(indent);
                printf("}");
                break;

            case TAG_Compound:
                hash_itr = (struct hashtable_itr*)hashtable_iterator(tag->payload->compound_payload);

                printf("\n");
                print_indent(indent);
                printf("{\n");
                do
                {
                    child = (nbt_tag*)hashtable_iterator_value(hash_itr);
                    if (child != NULL)
                        nbt_print_single(child, indent + 2);
                } while (hashtable_iterator_advance(hash_itr));

                print_indent(indent);
                printf("}");

                free(hash_itr);

                break;
        }
    printf("\n");
}

void print_indent(int indent)
{
    int i;
    if (indent == 0)
        return;

    for (i = 0; i < indent; i++)
        printf(" ");
}

void hex_dump(void *string, int string_length, int indent, int flags)
{
    int i, j;
    int bytes_per_row = flags == HEX_CHAR ? 16 : 8;
    int byte;

    for (i = 0; i < string_length; i++)
    {
        if ((i % bytes_per_row) == 0)
        {
            printf("\n");
            print_indent(indent);
            printf("%.6X: ", i);
        }

        if (flags == HEX_CHAR)
        {
            byte = *(((uint8_t*)string) + i);
            printf("%.2X", byte, byte);
        }
        else if (flags == HEX_WCHAR_T)
        {
            byte = *(((uint32_t*)string) + i);
            printf("%.4X", byte, byte);
        }

        if (flags == HEX_CHAR && (i % 2) == 1)
            printf(" ");
        else if (flags == HEX_WCHAR_T)
            printf(" ");

    }
    printf("\n");
}

unsigned int nbt_hash_fn(void *name)
{
    unsigned long hash = 5381;
    uint32_t c; 

    if (name != NULL)
    {
        while (c = *((uint32_t*)(name++)))
            hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

int nbt_name_eq(void *name1, void *name2)
{
    if (name1 == NULL || name2 == NULL)
        return -1;

    return wcscmp(name1, name2) == 0;
}

void *nbt_payload(nbt_tag *tag, uint8_t expected_type)
{
    if (tag == NULL)
        return NULL;

    if (tag->type != expected_type)
        return NULL;

    switch (tag->type)
    {
        case TAG_Byte:
            return &(tag->payload->byte_payload);
        case TAG_Short:
            return &(tag->payload->short_payload);
        case TAG_Int:
            return &(tag->payload->int_payload);
        case TAG_Long:
            return &(tag->payload->long_payload);
        case TAG_Float:
            return &(tag->payload->float_payload);
        case TAG_Double:
            return &(tag->payload->double_payload);
        case TAG_Byte_Array:
            return tag->payload->byte_array_payload;
        case TAG_String:
            return tag->payload->string_payload;
        case TAG_List:
            return tag->payload->list_payload;
        case TAG_Compound:
            return tag->payload->compound_payload;
        default:
            return NULL;

    }
}

nbt_tag *nbt_hash_search(nbt_tag *tag, wchar_t *key)
{
    struct hashtable *hash;

    if (tag == NULL || key == NULL)
        return NULL;

    hash = nbt_payload(tag, TAG_Compound);
    if (hash == NULL)
        return NULL;

    return hashtable_search(hash, key);
}
