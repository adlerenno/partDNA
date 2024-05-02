//
// Created by Enno Adler on 10.11.23.
//

#include "raw_input.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>  /* Many POSIX functions (but not all, by a large margin) */
#include <fcntl.h>   /* open(), creat() - and fcntl() */
#include "kseq.h"
#include "panic.h"
#include "constants.h"
#include <zlib.h>
#include <math.h>
#include <assert.h>
#include <sys/errno.h>

KSEQ_INIT(gzFile, gzread)


void get_meta(const char *filename, size_t *max_sequence_length, size_t *sequence_count, float *avg_length) { // TODO: Count number of symbols for statistic.
    // Find max_sequence_length and sequence_count.
    size_t length = 0, count = 0, sum_length = 0;
    kseq_t *seq;
    gzFile fp = gzopen(filename, "r"); // STEP 2: open the file handler
    seq = kseq_init(fp); // STEP 3: initialize seq
    while (kseq_read(seq) >= 0) {
        length = length > seq->seq.l ? length : seq->seq.l;
        sum_length += seq->seq.l;
        count++;
    }
    *max_sequence_length = length;
    *sequence_count = count;
    *avg_length = sum_length / count;
    kseq_destroy(seq);
    gzclose(fp);
}

// Function to read a FASTA file
void convert_genome(const char *filename, size_t max_sequence_length, size_t *sequence_length) {
    kseq_t *seq;
    gzFile fp = gzopen(filename, "r"); // STEP 2: open the file handler
    seq = kseq_init(fp); // STEP 3: initialize seq

    // STEP 4: read sequence
    size_t buffer_x = (size_t) ((double) max_sequence_length/32 + 1);
    size_t buffer_y = DEFAULT_WORD_INSERTION_BUFFER_LENGTH;
# ifndef ON_HEAP
    uint64_t buffer[buffer_x][buffer_y]; //Use blocks of length 32
#endif
#ifdef ON_HEAP
    uint64_t **buffer = malloc(buffer_x * buffer_y * sizeof(uint64_t));
#endif
    size_t buffer_position_y = 0;

    for (size_t i = 0; i < buffer_x; i++)
    {
        char output_filename[20];
        sprintf(output_filename, "%zu.bti", i);
        switch (remove(output_filename))
        {
            case 0:
                // Successful
                break;
            case EACCES:
                printf("Missing permission to delete file %s\n", output_filename);
                break;
            case EBUSY:
                printf("System prevents deletion of file %s\n", output_filename);
                break;
            case ENOENT:
                printf("File %s was removed before this routine. Don't interfere the routing, please.\n", output_filename);
                break;
            case EPERM:
                printf("The file %s is a directory now. Avoid playing!\n", output_filename);
                break;
            case EROFS:
                printf("Directory is read-only. Cannot remove %s\n", output_filename);
                break;
            case ELOOP:
                printf("System needs to many symbolic link lookups to find the file %s. Please use fewer links.\n", output_filename);
                break;
            case ENAMETOOLONG:
                printf("File name %s is longer than allowed from the system.\n", output_filename);
                break;
            default:
                printf("Unkown error removing file %s", output_filename);
                break;

            printf("Cannot remove temporary file %s", output_filename);
        }
    }

    size_t sequence_id = 0;
    while (kseq_read(seq) >= 0) { // TODO: Parallelize this step
        sequence_length[sequence_id] = 0;
        int step_counter = 0;
        uint64_t result = 0;
        size_t buffer_position_x = 0;
        for (int64_t i = (int64_t) seq->seq.l-1; i >= 0; i -= 1)  //convert data-block.
        {
            switch (seq->seq.s[i]) {
                case 'A':
                    result <<= 2;
                    // result |= 0; Does not change anything
                    step_counter++;
                    sequence_length[sequence_id]++;
                    break;
                case 'C':
                    result <<= 2;
                    result |= 2; //higher bit second.
                    step_counter++;
                    sequence_length[sequence_id]++;
                    break;
                case 'G':
                    result <<= 2;
                    result |= 1; //higher bit second.
                    step_counter++;
                    sequence_length[sequence_id]++;
                    break;
                case 'T':
                    result <<= 2;
                    result |= 3;
                    step_counter++;
                    sequence_length[sequence_id]++;
                    break;
                    case 'N':
                        // Implement the desired behavior here.
                    break;
                default:
                    panic("Not allowed character in input file: %c at sequence %s at position %lld", seq->seq.s[i], seq->name.s, i);
            }
            if (step_counter == 32)
            {
                step_counter = 0;
                buffer[buffer_position_x][buffer_position_y] = result;
                buffer_position_x++;
                result = 0;
            }
        }
        if (step_counter != 0) // The Case, that the sequence is not a multiple of 32.
        {
            result <<= (32-step_counter) * 2;
            buffer[buffer_position_x][buffer_position_y] = result;
        }

        buffer_position_y++;
        if (buffer_position_y == buffer_y) //buffer is filled.
        {
            for (size_t i = 0; i < buffer_x; i++)
            {
                char output_filename[20];
                sprintf(output_filename, "%zu.bti", i);
                FILE *file = fopen(output_filename, "a");
                size_t num_write = fwrite(buffer[i], sizeof(uint64_t), buffer_y, file);
                assert(num_write == buffer_y);
                fclose(file);
            }
            buffer_position_y = 0;
        }
        sequence_id++;
    }
    if (buffer_position_y > 0)
    {
        for (size_t i = 0; i < buffer_x; i++)
        {
            char output_filename[20];
            sprintf(output_filename, "%zu.bti", i);
            FILE *file = fopen(output_filename, "a");
            size_t num_write = fwrite(buffer[i], sizeof(uint64_t), buffer_position_y, file);
            assert(num_write == buffer_position_y);
            fclose(file);
        }
    }
#ifdef ON_HEAP
    free(buffer);
#endif
    kseq_destroy(seq); // STEP 5: destroy seq
    gzclose(fp); // STEP 6: close the file handler
}

/*
 * Remove all .bti files from the help directory, if the number is continuous from 0.
 */
void remove_help_files()
{
    int i = 0;
    char filename[20];
    sprintf(filename, "%d.bti", i);
    while (access(filename, F_OK) == 0) {
        remove(filename);
        sprintf(filename, "%d.bti", i);
        i++;
    }
}

// Function to read a FASTA file
void load_file_into_memory(const char *filename, size_t max_sequence_length, char **words, size_t *word_length, size_t word_count) {
    kseq_t *seq;
    gzFile fp = gzopen(filename, "r"); // STEP 2: open the file handler
    seq = kseq_init(fp); // STEP 3: initialize seq

    // STEP 4: read sequence

    size_t sequence_id = 0;
    while (kseq_read(seq) >= 0) {
        words[sequence_id] = malloc((2 * seq->seq.l + 2) * sizeof(char));
        words[sequence_id][0] = '$';
        words[sequence_id][2 * seq->seq.l + 1] = '\0';
        int64_t insert_position = 1; // Position 0 is the $.
        for (int64_t i = 0; i < (int64_t) seq->seq.l; i++)  //convert data-block.
        {
            switch (seq->seq.s[i]) {
                case 'A':
                    words[sequence_id][insert_position++] = '0';
                    words[sequence_id][insert_position++] = '0';
                    break;
                case 'C':
                    words[sequence_id][insert_position++] = '0';
                    words[sequence_id][insert_position++] = '1';
                    break;
                case 'G':
                    words[sequence_id][insert_position++] = '1';
                    words[sequence_id][insert_position++] = '0';
                    break;
                case 'T':
                    words[sequence_id][insert_position++] = '1';
                    words[sequence_id][insert_position++] = '1';
                    break;
                case 'N':
                    break;
                default:
                    panic("Not allowed character in input file: %c at sequence %s at position %lld", seq->seq.s[i], seq->name.s, i);
            }
        }
        word_length[sequence_id] = insert_position; //Number of next insert position is length
        sequence_id++;
    }

    kseq_destroy(seq); // STEP 5: destroy seq
    gzclose(fp); // STEP 6: close the file handler
}
