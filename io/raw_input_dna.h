//
// Created by Enno Adler on 30.01.24.
//

#ifndef BWT_RAW_INPUT_DNA_H
#define BWT_RAW_INPUT_DNA_H

#include <stddef.h>
void load_multiline_file_into_memory_dna(const char *filename, size_t max_sequence_length, char **words, size_t *word_length, size_t word_count);
void load_singleline_file_into_memory_dna(const char *filename, char **word, size_t *word_length);
#endif //BWT_RAW_INPUT_DNA_H