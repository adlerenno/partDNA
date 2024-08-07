#include <stdio.h>
#include <getopt.h>
#include <unistd.h>  /* Many POSIX functions (but not all, by a large margin) */
#include <fcntl.h>   /* open(), creat() - and fcntl() */
#include <string.h>
#include "eval/time_consumption.h"
#include "io/raw_input.h"
#include "io/raw_input_dna.h"
#include "splitting/splitting_dna.h"
#include "io/raw_output_dna.h"
#include "constants.h"

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

void perform_splitting(char *input_filename, enum PARSER parser, char *output_filename, long run_length, enum DIVIDE_CRITERIA divide_criteria, int divide_criteria_arg)
{
    void *data; double cpu_time, real_time;
#ifdef TIME_EVALUATION
    data = time_consumption_start();
#endif

#ifdef VERBOSE
    printf("Get file meta of file %s.\n", input_filename);
#endif
    size_t max_sequence_length = 0, word_count = 0, char_count = 0;
    char** words_dna;
    size_t* length_dna;
    switch (parser) { // TODO: Avoid get_meta functions by using realloc within the read functions.
        case KSEQ:
            get_meta_kseq(input_filename, &max_sequence_length, &word_count, &char_count);
            words_dna = calloc(word_count, sizeof (char*));
            length_dna = calloc(word_count, sizeof(size_t*));
            load_multiline_file_into_memory_dna_kseq(input_filename, max_sequence_length, words_dna, length_dna, word_count);
            break;
        case SINGLE:
            word_count = 1;
            words_dna = calloc(word_count, sizeof (char*));
            length_dna = calloc(word_count, sizeof(size_t*));
            load_singleline_file_into_memory_dna(input_filename, words_dna, length_dna);
            max_sequence_length = length_dna[0];
            char_count = max_sequence_length;
            break;
        case MULTI:
            get_meta(input_filename, &max_sequence_length, &word_count, &char_count);
            words_dna = calloc(word_count, sizeof (char*));
            length_dna = calloc(word_count, sizeof(size_t*));
            load_multiline_file_into_memory_dna(input_filename, max_sequence_length, words_dna, length_dna, word_count);
    }

#ifdef VERBOSE
    printf("\tWord count before splitting: %zu\n", word_count);
    printf("\tMax length of words before splitting: %zu\n", max_sequence_length);
    printf("\tSum length of words before splitting: %zu\n", char_count);
    printf("\tAvg length of words before splitting: %f\n", (double) char_count / (double) word_count);
    printf("Split the input now.\n");
#endif
    if (word_count == 0)
    {
        printf("Input file is empty. I abort, because there is nothing to do for me. ");
        return;
    }

    List *cuts = dna_find_splits(words_dna, length_dna, word_count, run_length);

#ifdef TEST
    // Use the following code to test if there is a gap in the split words.
    qsort(cuts->array, list_size(cuts), sizeof(DNASortEntry*), (int (*)(const void *, const void *)) compareDNAs);
    DNASortEntry *e = list_get(cuts, 0);
    size_t consumed_length = 0;
    if (e->word_id != 0 || e->previous_end != 0)
        printf("first word is not starting at word id 0, position 0 but at word id %lld, position %lld", e->word_id, e->previous_end);
    for (int i = 0; i < list_size(cuts) - 1; i++)
    {
        DNASortEntry *e = list_get(cuts, i);
        DNASortEntry *e2 = list_get(cuts, i+1);
        consumed_length += e->start - e->previous_end;
        if (e->start != e2->previous_end)
        {
            printf("first  word input %lld, word output %d, prev %lld, start %lld, end %lld, next %lld\n", e->word_id, i, e->previous_end, e->start, e->end, e->next_start);
            printf("second word input %lld, word output %d, prev %lld, start %lld, end %lld, next %lld\n", e2->word_id, i+1, e2->previous_end, e2->start, e2->end, e2->next_start);
        }
    }
    e = list_get(cuts, list_size(cuts)-1);
    consumed_length += e->start - e->previous_end;
    if (e->start != char_count)
    {
        printf("Last interval is not ending at the words ending: %lld != %zu\n", e->start, char_count);
    }
    printf("Symbols in words: %zu\n", consumed_length);
#endif //TEST

#ifdef VERBOSE
    printf("\tWord count after splitting: %zu\n", list_size(cuts));
    //printf("Copy words now.\n");
#endif
    size_t splitted_word_count = list_size(cuts);
    //char** splitted_words = calloc(splitted_word_count, sizeof(char*));
    //size_t* splitted_length = calloc(splitted_word_count, sizeof(size_t*));

    //copy_splitted_words_dna(cuts, words_dna, false, &splitted_words, &splitted_length);
#ifdef VERBOSE
    DNASortEntry *se0 = list_get(cuts, 0);
    size_t max_splitted_sequence_length = se0->start - se0->previous_end;
    size_t sum_length = se0->start - se0->previous_end;
    for (int i = 1; i < splitted_word_count; ++i) {
        DNASortEntry *se = list_get(cuts, i);
        if (se->start - se->previous_end> max_splitted_sequence_length) {
            // Update max if a larger element is found
            max_splitted_sequence_length = se->start - se->previous_end;
        }
        sum_length += se->start - se->previous_end;
    }
    printf("\tMax length of words after splitting: %zu\n", max_splitted_sequence_length);
    printf("\tSum length of words after splitting: %zu\n", sum_length);
    printf("\tAvg length of words after splitting: %f\n", ((double) sum_length) / ((double) splitted_word_count));
    // printf("\tFinished.\n");
#endif //VERBOSE
#ifdef TEST
    return;
#endif //TEST
#ifdef VERBOSE
    printf("Write to output file %s.\n", output_filename);
#endif
    write_splitted_words_dna_to_file(cuts, words_dna, false, output_filename, divide_criteria, divide_criteria_arg);

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

    //write_memory_to_file_dna(output_filename, splitted_words, splitted_word_count);

    //for (int i = 0; i < splitted_word_count; i++)
    //{
    //    free(splitted_words[i]);
    //}
    //free(splitted_words);
    //free(splitted_length);

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
    int run_length = 3;
    char * filename = "";
    char * output_filename = "";
    enum PARSER parser = MULTI;
    enum DIVIDE_CRITERIA dc = NONE;
    int dc_arg = 0;
    while ((opt = getopt(argc, argv, "hi:o:r:p:t:s:")) != -1) {
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
            case 'p':
                if (strcmp(optarg, "single") == 0)
                {
                    parser = SINGLE;
                }
                if (strcmp(optarg, "kseq") == 0)
                {
                    parser = KSEQ;
                }
                if (strcmp(optarg, "multi") == 0)
                {
                    parser = MULTI;
                }
                break;
            case 't':
                if (strcmp(optarg, "none") == 0)
                {
                    dc = NONE;
                }
                if (strcmp(optarg, "word_absolute") == 0)
                {
                    dc = WORD_ABSOLUTE;
                    if (dc_arg == 0)
                        dc_arg = 3000;
                }
                if (strcmp(optarg, "char_absolute") == 0)
                {
                    dc = CHAR_ABSOLUTE;
                    if (dc_arg == 0)
                        dc_arg = 3000000;
                }
                if (strcmp(optarg, "word_relative") == 0)
                {
                    dc = WORD_RELATIVE;
                    if (dc_arg == 0)
                        dc_arg = 5;
                }
                if (strcmp(optarg, "char_relative") == 0)
                {
                    printf("Unsupported in current implementation.");
                    return -1;
                    dc = CHAR_RELATIVE;
                    if (dc_arg == 0)
                        dc_arg = 5;
                }
                break;
            case 's':
                dc_arg = atoi(optarg);
                if (dc_arg <= 0)
                {
                    printf("Invalid argument for dividing output file.");
                    return -1;
                }
                break;
            case 'h':
            default:
                printf("Usage: \n\t./exc [-i <input_file>] [-o <output_file>] [-r <run_length>] [-parser {multi|kseq|single}] [-t] \nor\n\t./exc -h\n\n");
                printf("Default parameters: \nrun_length: %d\n\n", run_length);
                return 0;
        }
    }
    perform_splitting(filename, parser, output_filename, run_length, dc, dc_arg);
    return 0;
}
