//
// Created by Enno Adler on 30.11.23.
//

#include <stdlib.h>
#include "intlist.h"
#include "panic.h"
#include "list.h"

// Initialize an empty IntList
void intlist_init(IntList *list) {
    list->array = malloc(INITIAL_CAPACITY * sizeof(uint64_t));
    list->size = 0;
    list->capacity = INITIAL_CAPACITY;
}

// Append an element to the end of the intList
void intlist_append(IntList *list, uint64_t value) {
    if (list->size == list->capacity) {
        // If the array is full, double its capacity
        list->capacity *= 2;
        void *new_array = realloc(list->array, list->capacity * sizeof(void *));
        if (new_array != NULL)
        {
            list->array = new_array;
        }
        else
        {
            panic("Cannot realloc list.");
        }
    }

    list->array[list->size++] = value;
}

uint64_t intlist_get(const IntList *list, size_t index) {
    if (index < list->size) {
        return list->array[index];
    } else {
        panic("Index out of bounds");
        return 0;
    }
}

// Set the value at a specified index in the list
void intlist_set(IntList *list, size_t index, uint64_t value) {
    if (index < list->size) {
        list->array[index] = value;
    } else {
        panic("Index out of bounds");
    }
}

// Reverse the elements of the list
void intlist_reverse(IntList *list) {
    size_t left = 0;
    size_t right = list->size - 1;

    while (left < right) {
        // Swap elements at left and right indices
        uint64_t temp = list->array[left];
        list->array[left] = list->array[right];
        list->array[right] = temp;

        left++;
        right--;
    }
}

void intlist_clear(IntList *list) {
    free(list->array);
    list->size = 0;
    list->capacity = 0;

    // Reinitialize the list with an empty array
    list->array = malloc(INITIAL_CAPACITY * sizeof(uint64_t));
    list->capacity = INITIAL_CAPACITY;
}

// Get the size of the list
size_t intlist_size(IntList *list) {
    return list->size;
}

// Extend the list by appending elements from another list
void intlist_extend(IntList *list, const IntList *other) {
    for (size_t i = 0; i < other->size; i++) {
        intlist_append(list, other->array[i]);
    }
}

// Print the elements of the list
void intlist_print(const IntList *list, void (*printElement)(uint64_t)) {
    printf("[ ");
    for (size_t i = 0; i < list->size; i++) {
        printElement(list->array[i]);
        printf(" ");
    }
    printf("]\n");
}

// Free the memory allocated for the list
void intlist_destroy(IntList *list) {
    free(list->array);
    list->size = 0;
    list->capacity = 0;
}