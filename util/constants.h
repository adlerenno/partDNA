/**
 * @file constants.h
 * @author Fabian Roethlinger
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

// Default buffer length for WordInsertions
#define DEFAULT_WORD_INSERTION_BUFFER_LENGTH (512)

enum PARSER {SINGLE, MULTI, KSEQ};
enum DIVIDE_CRITERIA {NONE, WORD_ABSOLUTE, CHAR_ABSOLUTE, WORD_RELATIVE, CHAR_RELATIVE};

// #define VERBOSE

// Magic number of the compressed graph file
#define MAGIC_GRAPH "UBVBWT1\x00"

#endif
