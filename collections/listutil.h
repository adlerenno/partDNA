//
// Created by Enno Adler on 12.02.24.
//

#include "list.h"
#include "intlist.h"

#ifndef BWT_LISTDICT_H
#define BWT_LISTDICT_H

void listdict_add(List* list, size_t key, void *value);
void listcounter(IntList* list, size_t key);

#endif //BWT_LISTDICT_H
