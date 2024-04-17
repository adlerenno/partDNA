//
// Created by Enno Adler on 30.11.23.
//

#ifndef BWT_INTLIST_H
#define BWT_INTLIST_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint64_t *array;
    size_t size;
    size_t capacity;
} IntList;

// Initialize an empty list
void intlist_init(IntList *list);

// Append an element to the end of the list
void intlist_append(IntList *list, uint64_t value);

uint64_t intlist_get(const IntList *list, size_t index);

// Set the value at a specified index in the list
void intlist_set(IntList *list, size_t index, uint64_t value);

// Reverse the elements of the list
void intlist_reverse(IntList *list);

// Get the size of the list
size_t intlist_size(IntList *list);

// Extend the list by appending elements from another list
void intlist_extend(IntList *list, const IntList *other);

// Clear all elements from the list
void intlist_clear(IntList *list);

// Print the elements of the list
void intlist_print(const IntList *list, void (*printElement)(uint64_t));

// Free the memory allocated for the list
void intlist_destroy(IntList *list);

#endif //BWT_INTLIST_H
