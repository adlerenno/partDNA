//
// Created by Enno Adler on 23.01.24.
//

#include "splitting_dna.h"
//
// Created by Enno Adler on 29.11.23.
//

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "list.h"
#include "intlist.h"
// #include "sais.h" // Implementation shipped with this package. Did not work.
#include "sais64.h" // Implementation using the sais64 library of Yuta Mori.
#include "listutil.h"

size_t guess_length_dna(char** words, const size_t *word_length, size_t word_count, size_t number_of_effective_splits)
{
    // Phase 1a: Count the number of runs and find the highest run.
    IntList counts;
    intlist_init(&counts);
    size_t highest_run;
    for (int word_id = 0; word_id < word_count; word_count++)
    {
        size_t zero_run_length = 0;
        for (int char_pos = 0; char_pos < word_length[word_id]; char_pos++)
        {
            if (words[word_id][char_pos] == '1' && zero_run_length > 0)
            {
                listcounter(&counts, zero_run_length);
                highest_run = highest_run > zero_run_length ? highest_run : zero_run_length;
                zero_run_length = 0;
            }
            else
            if (words[word_id][char_pos] == '0') {
                zero_run_length++;
            }
        }
    }

    // Phase 1b: Determine the length of the lowest cut we need to get
    size_t run_length_lowest_cut, number_real_split_positions = 0;
    for (run_length_lowest_cut = highest_run; run_length_lowest_cut > 0; run_length_lowest_cut--)
    {
        number_real_split_positions += intlist_get(&counts, run_length_lowest_cut);
        if (number_real_split_positions > number_of_effective_splits)
        {
            break;
        }
    }
    intlist_destroy(&counts);

    return run_length_lowest_cut;
}

// Comparator used if sorting ascending.
int compare_sort_entries_asc(const void *a, const void* b)
{
    const DNASortEntry *sea = *(DNASortEntry **) a;
    const DNASortEntry *seb = *(DNASortEntry **) b;
    if (sea->name < seb->name)
        return -1;
    if (sea->name == seb->name)
        return 0;
    return 1;
}

// Comparator used if sorting descending.
int compare_sort_entries_desc(const void *a, const void* b)
{
    const DNASortEntry *sea = *(DNASortEntry **) a;
    const DNASortEntry *seb = *(DNASortEntry **) b;
    if (sea->name < seb->name)
        return 1;
    if (sea->name == seb->name)
        return 0;
    return -1;
}

void sort_entrys_by_name_dna(DNASortEntry **array, size_t size, int ascending) {
    if (ascending)
        qsort(array, size, sizeof(DNASortEntry *), compare_sort_entries_asc);
    else
        qsort(array, size, sizeof(DNASortEntry *), compare_sort_entries_desc);
}

void radix_for_splitting_dna(DNASortEntry **sorted_lowest_run_length_rotations, size_t length, char **words, const size_t *word_length, size_t word_count, size_t shift, bool *need_sais);

void radix_for_splitting_dna(DNASortEntry **sorted_lowest_run_length_rotations, size_t length, char **words, const size_t *word_length, size_t word_count, size_t shift, bool *need_sais)
{
    // termination
    if (length <= 1)
    {
        sorted_lowest_run_length_rotations[0]->name = 0;
        return;
    }

    List dollar_ret_list, ret_list,
    to_sort_rotations_A_list,
    to_sort_rotations_C_list,
    to_sort_rotations_G_list,
    to_sort_rotations_T_list,
    to_sort_rotations_dollars_list;

    List *dollar_ret = &dollar_ret_list, *ret = &ret_list,
    *to_sort_rotations_A = &to_sort_rotations_A_list,
    *to_sort_rotations_C = &to_sort_rotations_C_list,
    *to_sort_rotations_G = &to_sort_rotations_G_list,
    *to_sort_rotations_T = &to_sort_rotations_T_list,
    *to_sort_rotations_dollar = &to_sort_rotations_dollars_list;

    list_init(dollar_ret);
    list_init(ret);
    list_init(to_sort_rotations_A);
    list_init(to_sort_rotations_C);
    list_init(to_sort_rotations_G);
    list_init(to_sort_rotations_T);
    list_init(to_sort_rotations_dollar);

    // prepare for the next level.
    for (int i = 0; i < length; i++)
    {
        DNASortEntry *entry = sorted_lowest_run_length_rotations[i];
        if (entry->end + shift <= entry->next_start)
        {
            switch (words[entry->word_id][entry->end + shift])
            {
                case '$':
                    list_append(to_sort_rotations_dollar, entry);
                    break;
                case 'A':
                    list_append(to_sort_rotations_A, entry);
                    break;
                case 'C':
                    list_append(to_sort_rotations_C, entry);
                    break;
                case 'G':
                    list_append(to_sort_rotations_G, entry);
                    break;
                case 'T':
                    list_append(to_sort_rotations_T, entry);
                    break;
            }
        }
        else
        {
            if (entry->name < 0)
            {
                list_append(dollar_ret, entry);
            }
            else
            {
                list_append(ret, entry);
            }
        }
    }

    // store the number of equal substrings in the name field.
    sort_entrys_by_name_dna((DNASortEntry **) ret->array, list_size(ret), false);
    sort_entrys_by_name_dna((DNASortEntry **) dollar_ret->array, list_size(dollar_ret), false);
    list_extend(dollar_ret,ret); // The 0 with a $ following are smaller than the 0 with a 1.
    list_destroy(ret);

    if (list_size(dollar_ret) > 0)
    {
        for (size_t i= list_size(dollar_ret)-1; i > 0; i--)
        {
            DNASortEntry *current = list_get(dollar_ret, i);
            if (current->name == ((DNASortEntry*) list_get(dollar_ret, i-1))->name)
            {
                current->name = 1;
                *need_sais = true; //If at least two substrings are completely equal.
            }
            else
            {
                current->name = 0;
            }
        }
        ((DNASortEntry*) list_get(dollar_ret, 0))->name = 0;
    }

    if (list_size(to_sort_rotations_dollar) > 0)
    {
        for (int i = 0; i < list_size(to_sort_rotations_dollar); i++)
        {
            DNASortEntry *entry = list_get(to_sort_rotations_dollar, i);
            if (entry->next_start != word_length[entry->word_id])
            {
                list_append(dollar_ret, entry);
            }
        }
    }
    list_destroy(to_sort_rotations_dollar);

    // end Phase 1: Inline buckets to sorted_lowest_run_length_rotations:
    size_t i = 0, dollars = list_size(dollar_ret),
    As = list_size(to_sort_rotations_A),
    Cs = list_size(to_sort_rotations_C),
    Gs = list_size(to_sort_rotations_G),
    Ts = list_size(to_sort_rotations_T);
    memcpy(sorted_lowest_run_length_rotations + i, dollar_ret->array, sizeof (DNASortEntry *) * dollars);
    i+= dollars;
    memcpy(sorted_lowest_run_length_rotations + i, to_sort_rotations_A->array, sizeof (DNASortEntry *) * As);
    i+= As;
    memcpy(sorted_lowest_run_length_rotations + i, to_sort_rotations_C->array, sizeof (DNASortEntry *) * Cs);
    i+= Cs;
    memcpy(sorted_lowest_run_length_rotations + i, to_sort_rotations_G->array, sizeof (DNASortEntry *) * Gs);
    i+= Gs;
    memcpy(sorted_lowest_run_length_rotations + i, to_sort_rotations_T->array, sizeof (DNASortEntry *) * Ts);
    i+= Ts;
    list_destroy(dollar_ret);
    list_destroy(to_sort_rotations_A);
    list_destroy(to_sort_rotations_C);
    list_destroy(to_sort_rotations_G);
    list_destroy(to_sort_rotations_T);


    // Phase 2: Recursive sort
    if (As > 0)
    {
        radix_for_splitting_dna(sorted_lowest_run_length_rotations + dollars, As, words, word_length, word_count, shift + 1, need_sais);
    }
    // TODO: Can be parallelized
    if (Cs > 0)
    {
        radix_for_splitting_dna(sorted_lowest_run_length_rotations + dollars + As, Cs, words, word_length, word_count, shift + 1, need_sais);
    }
    if (Gs > 0)
    {
        radix_for_splitting_dna(sorted_lowest_run_length_rotations + dollars + As + Cs, Gs, words, word_length, word_count, shift + 1, need_sais);
    }
    if (Ts > 0)
    {
        radix_for_splitting_dna(sorted_lowest_run_length_rotations + dollars + As + Cs + Gs, Ts, words, word_length, word_count, shift + 1, need_sais);
    }
}

struct DNASortEntry *create_dna_sort_entry(size_t start, size_t end, size_t word_id) {
    DNASortEntry *entry = malloc(sizeof(DNASortEntry));
    entry->start = start;
    entry->end = end;
    entry->word_id = word_id;
    entry->name = 1;
    return entry;
}

void check_word_dna(char *word, const size_t length)
{
    for (int j = 0; j < length-1; j++)
    {
        if (word[j] != 'A' && word[j] != 'C' && word[j] != 'G' && word[j] != 'T') {
            panic("word pos %d is not 0 or 1: %c\n", j, word[j]);
        }
    }
    if (word[length-1] != '$'){
        panic("word has no $ at the end: %c\n", word[length-1]);
    }
    if (word[length] != '\0'){
        panic("word is not zero-terminated at the end.\n");
    }
}

List *dna_find_splits(char** words, const size_t *word_length, size_t word_count, size_t run_length_lowest_cut)
{
    // Check the correct word pattern. Searched too many errors regarding this...
    for (int i = 0; i < word_count; i++)
    {
        check_word_dna(words[i], word_length[i]);
    }

    // Phase 1c: Find all positions of 0-Runs of length exactly run_length_lowest_cut
    List *ret = malloc(sizeof(List));
    List to_sort_rotations;
    list_init(ret);
    list_init(&to_sort_rotations);
    size_t highest_run = 0;
    List help_sort_rotations;
    list_init(&help_sort_rotations);
    for (int word_id = 0; word_id < word_count; word_id++)
    {
        size_t zero_run_length = 0;
        DNASortEntry *last_entry = NULL;
        for (int position = 0; position < word_length[word_id]; position++)
        {
            switch (words[word_id][position]) {
                case 'A':
                    zero_run_length++;
                    break;
                case 'C': case 'G': case 'T':
                    if (zero_run_length >= run_length_lowest_cut)
                    {
                        DNASortEntry *entry = create_dna_sort_entry(position - zero_run_length, position, word_id);
                        list_append(&to_sort_rotations, entry);
                        highest_run = highest_run > zero_run_length ? highest_run : zero_run_length;
                        if (last_entry != NULL)
                        {
                            last_entry->next_start = entry->start;
                            entry->previous_end = last_entry->end;
                            last_entry->name = zero_run_length - run_length_lowest_cut;
                        }
                        else
                        {
                            entry->previous_end = 0;
                        }
                        last_entry = entry;
                    }
                    zero_run_length = 0;
                    break;
                case '$':;
                    DNASortEntry *last = NULL;
                    for (int run_length = (int) zero_run_length; run_length >= 0; run_length--)
                    {
                        DNASortEntry *e = create_dna_sort_entry(position - run_length, position - run_length, word_id);
                        if (last != NULL)
                        {
                            e->previous_end = last->end;
                            e->name = 0; //indicates a non-real split.
                        }
                        else if (last_entry != NULL)
                        {
                            e->previous_end = last_entry->end - run_length_lowest_cut; // Use last real cut for start of this word.
                            e->name = 1; // indicates a real split.
                        }
                        else
                        {
                            e->previous_end = 0;  // Use word start as start of this word.
                            e->name = 1; // indicates a real split.
                        }
                        listdict_add(&help_sort_rotations, run_length, e);
                        last = e;
                    }
                    DNASortEntry *entry = create_dna_sort_entry(position, position, word_id);
                    entry->name = word_id + 1;
                    entry->next_start = word_length[word_id];
                    if (last_entry != NULL)
                    {
                        last_entry->next_start = position - zero_run_length;
                        entry->previous_end = last_entry->end;
                        last_entry->name = -zero_run_length; // '-' indicates that a $ is following;
                    }
                    list_append(&to_sort_rotations, entry);
                    break;
                default:
                    break;
            }
        }
    }

    // Phase 2a: Consider $-Runs. Done in 1c now.
    // Phase 2b: Check if next 1-run is a $-Run: Done mostly in 1c.
    for (int run_length = 0; run_length < list_size(&help_sort_rotations); run_length++)
    {
        list_extend(ret, list_get(&help_sort_rotations, run_length));
    }
    // Phase 3a: Propagate previous and next values to entries. Done in 1c now.
    // Phase 3b: Sort ASC according to the substring

    bool need_sais = false;
    List *sorted_lowest_run_length_rotations = malloc(sizeof(List));
    list_init(sorted_lowest_run_length_rotations);
    for (size_t i = 0; i < list_size(&to_sort_rotations); i++) {
        list_append(sorted_lowest_run_length_rotations, list_get(&to_sort_rotations, i)); // TODO: Replace with list_clone command.
    }
    radix_for_splitting_dna((DNASortEntry **) sorted_lowest_run_length_rotations->array, list_size(&to_sort_rotations), words, word_length, word_count, 0, &need_sais);
    sorted_lowest_run_length_rotations->size -= word_count; // We omit during the sort exactly the SortEntries pointing towards a $-symbol

    if (need_sais)
    {
        // printf("using sa-is\n");
        // Phase 3c: Naming step
        long long name = word_count;
        for (size_t i = 0; i < list_size(sorted_lowest_run_length_rotations); i++)
        {
            if (((DNASortEntry *) list_get(sorted_lowest_run_length_rotations, i))->name == 0)
            {
                name++;
            }
            /*else
            {
                printf("name occurs multiple times: %zu\n", name);
            }*/
            ((DNASortEntry *) list_get(sorted_lowest_run_length_rotations, i))->name = name;
        }
        name++; // to ensure that the alphabet size is correct.

        // Phase 3d: create name string
        sa_int64_t *recursive_input_string = malloc((list_size(&to_sort_rotations) + 1) * sizeof(long long));
        sa_int64_t *suffix_array = malloc((list_size(&to_sort_rotations) + 1) * sizeof(long long));
        for (size_t i = 0; i < list_size(&to_sort_rotations); i++)
        {
            recursive_input_string[i] = ((DNASortEntry *) list_get(&to_sort_rotations, i))->name;
            //printf("%lld, ", recursive_input_string[i]);
        }
        recursive_input_string[list_size(&to_sort_rotations)] = 0; // Add the lowest symbol at the end.
        // printf("0\n");
        // printf("alphabet size: %zu, word length: %zu\n", name, list_size(&to_sort_rotations) + 1);

        // Phase 3e: use SA_IS to create the suffix array of the name string. Note that there are 'name' different symbols.
        // if (divsufsort64(recursive_input_string, suffix_array, list_size(&to_sort_rotations) + 1)) // use divsufsort library, size of alphabet to small
        // if (sais_size_t(recursive_input_string, suffix_array, (int) list_size(&to_sort_rotations) + 1, (int) name) != 0)  // uses local implementation, not working yet
        // using cast to sa_int64_t instead of long long
        if (sais64_i64(recursive_input_string, suffix_array, (sa_int64_t) list_size(&to_sort_rotations) + 1, (sa_int64_t) name) < 0) // use yuta moris sais implementation.
        {
            panic("Exception inside sa_is");
        }
/*        else
        {
            // Check the suffix array here
            for (int i = 2; i < list_size(&to_sort_rotations); i++)
            {
                DNASortEntry *se_prev = list_get(&to_sort_rotations,suffix_array[i-1]);
                DNASortEntry *se = list_get(&to_sort_rotations,suffix_array[i]);
                if (strcmp(&words[se_prev->word_id][se_prev->end], &words[se->word_id][se->end]) > 0)
                {
                    printf("Error in suffix array at pos %d:\n", i);
                    printf("%s\n", &words[se_prev->word_id][se_prev->end]);
                    printf("%s\n", &words[se->word_id][se->end]);
                }
            }
        }*/

        // Phase 3f: Use the result of SA_IS to complete the sort.
        for (size_t sa_position = 0; sa_position < list_size(sorted_lowest_run_length_rotations); sa_position++)
        {
            size_t suffix_array_entry = suffix_array[word_count + 1 + sa_position]; // skip the word_count + 1 $-symbols.
            list_set(sorted_lowest_run_length_rotations, sa_position, list_get(&to_sort_rotations, suffix_array_entry));
        }
        free(suffix_array);
    }

    // Check the correct order.
    for (int i = 2; i < list_size(sorted_lowest_run_length_rotations) && i < 1000; i++)
    {
        DNASortEntry *se_prev = list_get(sorted_lowest_run_length_rotations, i-1);
        DNASortEntry *se = list_get(sorted_lowest_run_length_rotations, i);
        if (strcmp(&words[se_prev->word_id][se_prev->end], &words[se->word_id][se->end]) > 0)
        {
            printf("Error in sorting at pos %d:\n", i);
            printf("%s\n", &words[se_prev->word_id][se_prev->end]);
            printf("%s\n", &words[se->word_id][se->end]);
        }
    }

    // Phase 4: Induced Adding
    List *next_rotations = malloc(sizeof (List)), *to_reverse_ret = malloc(sizeof(List));
    list_init(next_rotations);
    list_init(to_reverse_ret);
    list_reverse(sorted_lowest_run_length_rotations);
    for (int i = 0; i < list_size(sorted_lowest_run_length_rotations); i++)
    {
        DNASortEntry *e = ((DNASortEntry *) list_get(sorted_lowest_run_length_rotations, i));
        e->end -= run_length_lowest_cut; // Reduce end, so that if start < end, there are more zeros than run_length_lowest_cut.
        if (e->previous_end > 0)
        {
            e->previous_end -= run_length_lowest_cut; // Reduce end, so that if start < end, there are more zeros than run_length_lowest_cut.
        }
    }
    while (list_size(sorted_lowest_run_length_rotations) != 0)
    {
        for (int i = 0; i < list_size(sorted_lowest_run_length_rotations); i++)
        {
            DNASortEntry *entry = list_get(sorted_lowest_run_length_rotations, i);
            list_append(to_reverse_ret, entry);
            if (entry->start < entry->end)
            {
                entry->name = 0; // indicates a rotation that only adds a 0 to the BWT.
                DNASortEntry *next_entry = create_dna_sort_entry(entry->start, entry->end - 1, entry->word_id);
                next_entry->previous_end = entry->previous_end;
                entry->previous_end = next_entry->end;
                entry->start = entry->end;
                list_append(next_rotations, next_entry);
            }
            else
            {
                entry->name = 1; // indicates a real split
            }
        }
        void *help = sorted_lowest_run_length_rotations;
        sorted_lowest_run_length_rotations = next_rotations;
        next_rotations = help;
        list_clear(next_rotations);
        run_length_lowest_cut++;
    }
    list_reverse(to_reverse_ret);
    list_extend(ret, to_reverse_ret);

    list_destroy(next_rotations);
    free(next_rotations);
    list_destroy(sorted_lowest_run_length_rotations);
    free(sorted_lowest_run_length_rotations);
    list_destroy(to_reverse_ret);
    free(to_reverse_ret);

    return ret;
}

void copy_splitted_words_dna(List *cuts, char** words, bool dollar_at_position_0, char*** copied_words, size_t** copied_length)
{
    size_t splitted_word_count = list_size(cuts);
    char** splitted_words = *copied_words;
    size_t* splitted_length = *copied_length;
    for (int i = 0; i < splitted_word_count; i++) {
        DNASortEntry *se = list_get(cuts, i);
        if (se->start < se->previous_end)
        {
            panic("The end of the word is before its start during copying.");
        }
        if (dollar_at_position_0)
        {  // TODO: Check if this case works fine.
            splitted_length[i] = se->start - se->previous_end + 1; // + 1 for the $-sign.
            splitted_words[i] = malloc((splitted_length[i] + 1) * sizeof (char)); // +1 for \0
            memcpy(&(splitted_words[i][1]), &(words[se->word_id][se->previous_end]), (splitted_length[i] - 1) * sizeof(char));
            splitted_words[i][0]='$';
            splitted_words[i][splitted_length[i]]='\0';
        }
        else
        {
            splitted_length[i] = se->start - se->previous_end; // no $-sign here.
            splitted_words[i] = malloc((splitted_length[i] + 1) * sizeof (char)); // +1 for \0
            memcpy(&(splitted_words[i][0]), &(words[se->word_id][se->previous_end]), (splitted_length[i]) * sizeof(char));
            splitted_words[i][splitted_length[i]]='\0';
        }
        // splitted_words[i][splitted_length[i]]='\0';
        // printf("%zu, %s\n", splitted_length[i], splitted_words[i]);
    }
}