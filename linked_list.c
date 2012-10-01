#include <stdlib.h>
#include "linked_list.h"

// List creation/destruction functions
list *list_new(list_free_func node_free_func)
{
    list *new = malloc(sizeof(list));

    if (new != NULL)
    {
        new->data_free_func = node_free_func;
        new->start = NULL;
        new->end = NULL;
    }

    return new;
}

void list_free(list *doomed)
{
    list_node *needle;

    while ((needle = list_pop_node(doomed)) != NULL)
    {
        list_node_free(needle, doomed->data_free_func);
    }

    free(doomed);
}

void list_node_free(list_node *doomed, list_free_func node_free_func)
{
    if (node_free_func != NULL)
        node_free_func(doomed->data);
    else
        free(doomed->data);

    free(doomed);
}

// List manipulation (automatic mem)
int list_push(list *haystack, void *data)
{
    list_node *needle;

    if (haystack == NULL)
        return E_LINKED_LIST_NULL_HAYSTACK;

    needle = malloc(sizeof(list_node));

    if (needle == NULL)
        return E_LINKED_LIST_OUT_OF_MEM;

    needle->data = data;
    needle->prev = NULL;
    needle->next = NULL;
    return list_push_node(haystack, needle);
}

void *list_pop(list *haystack)
{
    list_node *needle;
    void *data;

    if (haystack == NULL)
        return NULL;
   
    needle = list_pop_node(haystack);
    if (needle == NULL)
        return NULL;

    data = needle->data;
    free(needle);

    return data;
}

int list_unshift(list *haystack, void *data)
{
    list_node *needle;

    if (haystack == NULL)
        return E_LINKED_LIST_NULL_HAYSTACK;

    needle = malloc(sizeof(list_node));

    if (needle == NULL)
        return E_LINKED_LIST_OUT_OF_MEM;

    needle->data = data;
    needle->prev = NULL;
    needle->next = NULL;
    return list_unshift_node(haystack, needle);
}

void *list_shift(list *haystack)
{
    list_node *needle;
    void *data;

    if (haystack == NULL)
        return NULL;

    needle = list_shift_node(haystack);
    if (needle == NULL)
        return NULL;

    data = needle->data;
    free(needle);

    return data;
}

// List manipulation (manual mem)
int list_push_node(list *haystack, list_node *needle)
{
    list_node *last;

    if (haystack == NULL)
        return E_LINKED_LIST_NULL_HAYSTACK;
   
    last = haystack->end;

    if (needle == NULL)
        return E_LINKED_LIST_NULL_NEEDLE;

    needle->prev = last;
    needle->next = NULL;

    if (last != NULL)
        last->next = needle;

    haystack->end = needle;

    if (haystack->start == NULL)
        haystack->start = needle;

    return 0;
}

list_node *list_pop_node(list *haystack)
{
    list_node *last, *next_to_last;

    if (haystack == NULL)
        return NULL;

    last = haystack->end;

    if (last != NULL)
    {
        next_to_last = last->prev;

        if (next_to_last != NULL)
            next_to_last->next = NULL;
        else
            haystack->start = NULL;

        last->prev = NULL;
        last->next = NULL;

        haystack->end = next_to_last;
    }

    return last;
}

int list_unshift_node(list *haystack, list_node *needle)
{
    list_node *first;

    if (haystack == NULL)
        return E_LINKED_LIST_NULL_HAYSTACK;

    if (needle == NULL)
        return E_LINKED_LIST_NULL_NEEDLE;

    first = haystack->start;

    needle->prev = NULL;
    needle->next = first;

    if (first != NULL)
        first->prev = needle;

    haystack->start = needle;

    if (haystack->end == NULL)
        haystack->end = needle;

    return 0;
}

list_node *list_shift_node(list *haystack)
{
    list_node *first, *second;

    if (haystack == NULL)
        return NULL;

    first = haystack->start;

    if (first != NULL)
    {
        second = first->next;

        if (second != NULL)
            second->prev = NULL;
        else
            haystack->end = NULL;

        first->prev = NULL;
        first->next = NULL;

        haystack->start = second;
    }
    
    return first;

}
