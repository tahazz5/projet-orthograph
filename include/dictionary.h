#ifndef DICTIONARY_H
#define DICTIONARY_H
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#define WORD_MAX 255
#define SUGGEST_MAX 5
/* All backends share the same normalized, deduplicated vocabulary. */
typedef enum { METHOD_HASH, METHOD_TRIE, METHOD_SEQ } dictionary_method;
typedef struct dictionary dictionary;
/* Returns 1 for a word, 0 for EOF, -1 for a read error. Overlong words
 * are consumed in full and reported through too_long. */
int read_word(FILE *file, char word[WORD_MAX + 1], bool *too_long);
dictionary *dictionary_load(const char *path, dictionary_method method);
bool dictionary_check(const dictionary *dict, const char *word);
size_t dictionary_size(const dictionary *dict);
size_t dictionary_suggest(const dictionary *dict, const char *word,
                          const char *results[SUGGEST_MAX]);
void dictionary_free(dictionary *dict);
#endif
