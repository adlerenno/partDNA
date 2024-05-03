#include <stdio.h>
#include <getopt.h>
#include <unistd.h>  /* Many POSIX functions (but not all, by a large margin) */
#include <fcntl.h>   /* open(), creat() - and fcntl() */
#include "eval/time_consumption.h"
#include "io/raw_input.h"
#include "io/raw_input_dna.h"
#include "splitting/splitting_dna.h"
#include "io/raw_output_dna.h"

/*
 * Generates a random bitstring.
 */
static char *rand_string(char *str, size_t size)
{
    const char charset[] = "01";
    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
    return str;
}

int compareDNAs(DNASortEntry **a, DNASortEntry **b)
{
    if ((*a)->word_id < (*b)->word_id)
    {
        return -1;
    }
    if ((*a)->word_id > (*b)->word_id)
    {
        return 1;
    }
    if ((*a)->previous_end < (*b)->previous_end)
    {
        return -1;
    }
    if ((*a)->previous_end > (*b)->previous_end)
    {
        return 1;
    }
    return (int) ((*a)->start - (*b)->start);
}

void perform_splitting(char *input_filename, bool multiline_fasta, char *output_filename, long run_length)
{
    void *data; double cpu_time, real_time;
#ifdef TIME_EVALUATION
    data = time_consumption_start();
#endif

#ifdef VERBOSE
    printf("Get file meta of file %s.\n", input_filename);
#endif
    size_t max_sequence_length = 0, word_count = 0; float char_count = 0;
    char** words_dna;
    size_t* length_dna;
    if (multiline_fasta)
    {
        get_meta(input_filename, &max_sequence_length, &word_count, &char_count);
        words_dna = calloc(word_count, sizeof (char*));
        length_dna = calloc(word_count, sizeof(size_t*));
        load_multiline_file_into_memory_dna(input_filename, max_sequence_length, words_dna, length_dna, word_count);
    }
    else
    {
        word_count = 1;
        words_dna = calloc(word_count, sizeof (char*));
        length_dna = calloc(word_count, sizeof(size_t*));
        load_singleline_file_into_memory_dna(input_filename, words_dna, length_dna);
        max_sequence_length = length_dna[0];
        char_count = (float) max_sequence_length;
    }

#ifdef VERBOSE
    printf("\tWord count before splitting: %zu\n", word_count);
    printf("\tMax length of words before splitting: %zu\n", max_sequence_length);
    printf("\tSum length of words before splitting: %f\n", char_count);
    printf("\tAvg length of words before splitting: %f\n", char_count / word_count);
    printf("Split the input now.\n");
#endif

    List *cuts = dna_find_splits(words_dna, length_dna, word_count, run_length);

/*  // Use the following code to test if there is a gap in the splitted words.
    qsort(cuts->array, list_size(cuts), sizeof(DNASortEntry*), (int (*)(const void *, const void *)) compareDNAs);
    for (int i = 0; i < list_size(cuts) - 1; i++)
    {
        DNASortEntry *e = list_get(cuts, i);
        DNASortEntry *e2 = list_get(cuts, i+1);
        if (e->start != e2->previous_end && e->word_id == e2->word_id)
        {
            printf("first  word %lld, prev %lld, start %lld, end %lld, next %lld\n", e->word_id, e->previous_end, e->start, e->end, e->next_start);
            printf("second word %lld, prev %lld, start %lld, end %lld, next %lld\n", e2->word_id, e2->previous_end, e2->start, e2->end, e2->next_start);
        }
    }
*/

#ifdef VERBOSE
    printf("\tWord count after splitting: %zu\n", list_size(cuts));
    printf("Copy words now.\n");
#endif
    size_t splitted_word_count = list_size(cuts);
    char** splitted_words = calloc(splitted_word_count, sizeof(char*));
    size_t* splitted_length = calloc(splitted_word_count, sizeof(size_t*));
    copy_splitted_words_dna(cuts, words_dna, false, &splitted_words, &splitted_length);
#ifdef VERBOSE
    size_t max_splitted_sequence_length = splitted_length[0];
    size_t sum_length = splitted_length[0];
    for (int i = 1; i < splitted_word_count; ++i) {
        if (splitted_length[i] > max_splitted_sequence_length) {
            // Update max if a larger element is found
            max_splitted_sequence_length = splitted_length[i];
        }
        sum_length += splitted_length[i];
    }
    printf("\tMax length of words after splitting: %zu\n", max_splitted_sequence_length);
    printf("\tSum length of words after splitting: %zu\n", sum_length);
    printf("\tAvg length of words after splitting: %f\n", ((float) sum_length) / splitted_word_count);
    printf("\tFinished.\n");
    printf("Write to output file %s.\n", output_filename);
#endif

    for (int i = 0; i < word_count; i++)
    {
        free(words_dna[i]);
    }
    free(words_dna);
    free(length_dna);
    for (int i = 0; i < list_size(cuts); i++)
    {
        free(list_get(cuts, i));
    }
    free(cuts);

    write_memory_to_file_dna(output_filename, splitted_words, splitted_word_count);

    for (int i = 0; i < splitted_word_count; i++)
    {
        free(splitted_words[i]);
    }
    free(splitted_words);
    free(splitted_length);

#ifdef VERBOSE
    printf("\tFinished.\n");
#endif
#ifdef TIME_EVALUATION
    time_consumption_stop(data, &real_time, &cpu_time);
    printf("Total runtime of splitting: %f seconds (%f seconds on cpu)\n", real_time, cpu_time);
#endif
}

int main(int argc, char **argv) {
    int opt;
    bool multiline_fasta = true;
    int run_length = 2;
    char * filename = "";
    char * output_filename = "";
    while ((opt = getopt(argc, argv, "hdi:o:r:")) != -1) {
        switch (opt) {
            case 'i':
                filename = optarg;
                if (access(filename, F_OK) != 0)
                {
                    printf("Invalid input file.");
                    return -1;
                }
                break;
            case 'o':
                output_filename = optarg;
                /*if (access(output_filename, F_OK) == 0)
                {
                    printf("Output file exists. Choose other.");
                    return -1;
                }*/
                break;
            case 'r':
                run_length = atoi(optarg);
                if (run_length < 1) {
                    printf("Invalid run-length.");
                    return -1;
                }
                break;
            case 'd':
                multiline_fasta = false;
                break;
            case 'h':
            default:
                printf("Usage: \n\t./exc [-i <input_file>] [-o <output_file>] [-r <run_length>] [-d] \nor\n\t./exc -h\n\n");
                printf("Default parameters: \nrun_length: %d\n\n", run_length);
                return 0;
        }
    }
    perform_splitting(filename, multiline_fasta, output_filename, run_length);
    return 0;
}
