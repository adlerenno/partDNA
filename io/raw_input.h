//
// Created by Enno Adler on 10.11.23.
//

#ifndef BWT_RAW_INPUT_H
#define BWT_RAW_INPUT_H

#include <stddef.h>

void get_meta(const char *filename, size_t *max_sequence_length, size_t *sequence_count, float *char_count);
void convert_genome(const char *filename, size_t max_sequence_length, size_t *sequence_length);
void load_file_into_memory(const char *filename, size_t max_sequence_length, char **words, size_t *word_length, size_t word_count);
#endif //BWT_RAW_INPUT_H
