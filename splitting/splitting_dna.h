//
// Created by Enno Adler on 23.01.24.
//

#ifndef BWT_SPLITTING_DNA_H
#define BWT_SPLITTING_DNA_H

#include <stddef.h>
#include <stdbool.h>
#include "../collections/list.h"

struct DNASortEntry {
    long long start;
    long long end;
    long long previous_end;
    long long next_start;
    long long word_id;
    long long name; // field used for naming, equality, typing, additional zeros, etc.
};

typedef struct DNASortEntry DNASortEntry;

/**
 *
 */
List *dna_find_splits(char** words, const size_t *word_length, size_t word_count, size_t run_length_lowest_cut);
void copy_splitted_words_dna(List *cuts, char** words, bool dollar_at_position_0, char*** splitted_words, size_t** splitted_length);
void write_splitted_words_dna_to_file(List *cuts, char** words, bool dollar_at_position_0, char* filename);
#endif //BWT_SPLITTING_DNA_H
