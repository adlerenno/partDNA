//
// Created by Enno Adler on 29.11.23.
//

#include "list.h"

// Initialize an empty list
void list_init(List *list) {
    list->array = (void **)malloc(INITIAL_CAPACITY * sizeof(void *));
    list->size = 0;
    list->capacity = INITIAL_CAPACITY;
}

// Append an element to the end of the list
void list_append(List *list, void *value) {
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

void *list_get(const List *list, size_t index) {
    if (index < list->size) {
        return list->array[index];
    } else {
        panic("Index out of bounds");
        return NULL;
    }
}

// Set the value at a specified index in the list
void list_set(List *list, size_t index, void *value) {
    if (index < list->size) {
        list->array[index] = value;
    } else {
        panic("Index out of bounds");
    }
}

// Reverse the elements of the list
void list_reverse(List *list) {
    if (list_size(list) <= 1)
        return;
    size_t left = 0;
    int64_t right =  (int64_t) list->size - 1;

    while (left < right) {
        // Swap elements at left and right indices
        void *temp = list->array[left];
        list->array[left] = list->array[right];
        list->array[right] = temp;

        left++;
        right--;
    }
}

void list_clear(List *list) {
    free(list->array);
    list->size = 0;
    list->capacity = 0;

    // Reinitialize the list with an empty array
    list->array = (void **)malloc(INITIAL_CAPACITY * sizeof(void *));
    list->capacity = INITIAL_CAPACITY;
}

// Get the size of the list
size_t list_size(List *list) {
    return list->size;
}

// Extend the list by appending elements from another list
void list_extend(List *list, const List *other) {
    for (size_t i = 0; i < other->size; i++) {
        list_append(list, other->array[i]);
    }
}

// Print the elements of the list
void list_print(const List *list, void (*printElement)(void *)) {
    printf("[ ");
    for (size_t i = 0; i < list->size; i++) {
        printElement(list->array[i]);
        printf(" ");
    }
    printf("]\n");
}

// Free the memory allocated for the list
void list_destroy(List *list) {
    free(list->array);
    list->size = 0;
    list->capacity = 0;
}