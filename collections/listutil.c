//
// Created by Enno Adler on 12.02.24.
//

#include "listutil.h"
#include "list.h"
#include "intlist.h"

// Adds NULL values at the end of the list until the list is large as required
void listdict_add(List* list, size_t key, void *value) {
    while (list_size(list) <= key)
    {
        List *k = malloc(sizeof (List));
        list_init(k);
        list_append(list, k);
    }
    List *k = list_get(list, key);
    list_append(k, value);
}

// Adds NULL values at the end of the list until the list is large as required
void listcounter(IntList* list, size_t key) {
    while (intlist_size(list) < key)
    {
        //List *k = malloc(sizeof (List));
        //list_init(k);
        intlist_append(list, 0);
    }
    uint64_t k = intlist_get(list, key);
    intlist_set(list, key, k+1);
}
