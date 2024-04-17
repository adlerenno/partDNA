//
// Created by Enno Adler on 30.01.24.
//

#include "raw_input_dna.h"
#include <stdio.h>
#include <stdlib.h>
#include "kseq.h"
#include "panic.h"
#include <zlib.h>
#include <math.h>
#include <assert.h>
#include <sys/errno.h>

KSEQ_INIT(gzFile, gzread)

void load_multiline_file_into_memory_dna(const char *filename, size_t max_sequence_length, char **words, size_t *word_length, size_t word_count) {
    kseq_t *seq;
    gzFile fp = gzopen(filename, "r"); // STEP 2: open the file handler
    seq = kseq_init(fp); // STEP 3: initialize seq

    // STEP 4: read sequence

    size_t sequence_id = 0;
    while (kseq_read(seq) >= 0) {
        words[sequence_id] = malloc((seq->seq.l + 2) * sizeof(char));
        int64_t insert_position = 0; // Position 0 is the $.
        for (int64_t i = 0; i < (int64_t) seq->seq.l; i++)  //convert data-block.
        {
            switch (seq->seq.s[i]) {
                case 'A': case 'C': case 'G': case 'T':
                    words[sequence_id][insert_position++] = seq->seq.s[i];
                    break;
                case 'N':
                    break;
                default:
                    panic("Not allowed character in input file: %c at sequence %s at position %lld", seq->seq.s[i], seq->name.s, i);
            }
        }
        words[sequence_id][insert_position++] = '$';
        words[sequence_id][insert_position] = '\0';
        word_length[sequence_id] = insert_position; //Number of next insert position is length
        sequence_id++;
    }

    kseq_destroy(seq); // STEP 5: destroy seq
    gzclose(fp); // STEP 6: close the file handler
}

void load_singleline_file_into_memory_dna(const char *filename, char **word, size_t *word_length)
{
    FILE* fastaFile = fopen(filename, "r");

    if (fastaFile == NULL) {
        panic("Error opening file: %s\n", filename);
        return;
    }

    // Initialize variables
    char line[1000];
    unsigned long sequenceLength = 0;
    unsigned long current_max_length = 1000000;
    word[0] = (char*)malloc(current_max_length * sizeof(char));

    // Skip the first line (header line starting with '>')
    fgets(line, sizeof(line), fastaFile);

    // Read the DNA sequence from the file
    while (fgets(line, sizeof(line), fastaFile) != NULL) {
        // Remove newline characters from the line
        unsigned long lineLength = strlen(line);
        if (line[lineLength - 1] == '\n') {
            line[lineLength - 1] = '\0';
            lineLength--;
        }
        if (lineLength + sequenceLength > current_max_length)
        {
            current_max_length *= 2;
            void *new = realloc(word[0], current_max_length);
            if (new != NULL)
            {
                word[0] = new;
            }
            else
            {
                panic("Out of Memory. Cannot load DNA-string.");
            }
        }
        // Copy the line to the DNA sequence buffer
        // strcpy(word[0] + sequenceLength, line);
        // sequenceLength += lineLength - 1;  // Exclude the newline character
        for (int64_t i = 0; i < lineLength; i++)  //convert data-block.
        {
            if (line[i] == '>')
                break; // Skip lines with > at the first position.
            switch (line[i]) {
                case 'A': case 'C': case 'G': case 'T':
                    word[0][sequenceLength++] = line[i];
                    break;
                case 'a':
                    word[0][sequenceLength++] = 'A';
                    break;
                case 'c':
                    word[0][sequenceLength++] = 'C';
                    break;
                case 'g':
                    word[0][sequenceLength++] = 'G';
                    break;
                case 't':
                    word[0][sequenceLength++] = 'T';
                    break;
                    case 'N': case 'M': case 'R': case 'Y': case 'W': case 'K': case 'B': case 'S':
                    break;
                default:
                    panic("Not allowed character in input file: %c at position %lld", line[i], i);
            }
        }
    }
    // TODO: Check if not length exceeded here.
    word[0][sequenceLength++] = '$';
    word[0][sequenceLength] = '\0';
    *word_length = sequenceLength;
    fclose(fastaFile);
}
