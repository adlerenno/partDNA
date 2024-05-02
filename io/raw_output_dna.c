//
// Created by Enno Adler on 30.01.24.
//

#include <stdio.h>
#include <unistd.h>  /* Many POSIX functions (but not all, by a large margin) */
#include <fcntl.h>   /* open(), creat() - and fcntl() */
#include "raw_output_dna.h"

void write_memory_to_file_dna(char *filename, char **words, long long word_count)
{
    FILE* fastaFile = fopen(filename, "w");

    if (fastaFile == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return;
    }

    for (int i = 0; i < word_count; ++i) {
        fprintf(fastaFile, ">S%d\n", i + 1); // Add a header for each sequence
        fprintf(fastaFile, "%s\n", words[i]);  // Write the DNA sequence
    }
    fclose(fastaFile);
}