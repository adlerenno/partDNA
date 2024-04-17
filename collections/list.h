//
// Created by Enno Adler on 29.11.23.
//

#ifndef BWT_LIST_H
#define BWT_LIST_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "panic.h"

#define INITIAL_CAPACITY 10

typedef struct {
    void **array;
    int64_t size;
    int64_t capacity;
} List;

// Initialize an empty list
void list_init(List *list);

// Append an element to the end of the list
void list_append(List *list, void *value);

void *list_get(const List *list, size_t index);

// Set the value at a specified index in the list
void list_set(List *list, size_t index, void *value);

// Reverse the elements of the list
void list_reverse(List *list);

// Get the size of the list
size_t list_size(List *list);

// Extend the list by appending elements from another list
void list_extend(List *list, const List *other);

// Clear all elements from the list
void list_clear(List *list);

// Print the elements of the list
void list_print(const List *list, void (*printElement)(void *));

// Free the memory allocated for the list
void list_destroy(List *list);

#endif //BWT_LIST_H
