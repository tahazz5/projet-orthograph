#include "dictionary.h"
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

#define BUCKETS 65537

typedef struct edge {
    unsigned char letter;
    bool terminal;
    struct edge *child, *next;
} edge;
struct dictionary {
    dictionary_method method;
    char **words;
    size_t count, capacity;
    size_t *buckets, *next;
    edge root;
};

static const char *fold(wint_t c) {
    switch (c) {
    case L'à': case L'á': case L'â': case L'ä': case L'ã': case L'å': return "a";
    case L'ç': return "c";
    case L'è': case L'é': case L'ê': case L'ë': return "e";
    case L'ì': case L'í': case L'î': case L'ï': return "i";
    case L'ò': case L'ó': case L'ô': case L'ö': case L'õ': return "o";
    case L'ù': case L'ú': case L'û': case L'ü': return "u";
    case L'ÿ': return "y";
    case L'œ': return "oe";
    case L'æ': return "ae";
    default: return NULL;
    }
}

int read_word(FILE *file, char word[WORD_MAX + 1], bool *too_long) {
    size_t length = 0;
    bool started = false;
    *too_long = false;
    for (;;) {
        wint_t c = fgetwc(file);
        if (c == WEOF) {
            if (ferror(file)) return -1;
            break;
        }
        if (!iswalpha(c)) {
            if (started) break;
            continue;
        }
        started = true;
        c = towlower(c);
        const char *replacement = fold(c);
        char encoded[MB_LEN_MAX];
        size_t n;
        if (replacement) {
            n = strlen(replacement);
        } else {
            mbstate_t state = {0};
            n = wcrtomb(encoded, (wchar_t)c, &state);
            if (n == (size_t)-1) return -1;
            replacement = encoded;
        }
        if (length + n > WORD_MAX) *too_long = true;
        if (!*too_long) {
            memcpy(word + length, replacement, n);
            length += n;
        }
    }
    word[length] = '\0';
    return started ? 1 : 0;
}

static size_t hash_word(const char *s) {
    uint64_t h = UINT64_C(14695981039346656037);
    while (*s) { h ^= (unsigned char)*s++; h *= UINT64_C(1099511628211); }
    return (size_t)(h % BUCKETS);
}
static int compare_words(const void *a, const void *b) {
    return strcmp(*(const char *const *)a, *(const char *const *)b);
}
static void free_edges(edge *e) {
    while (e) { edge *next = e->next; free_edges(e->child); free(e); e = next; }
}
void dictionary_free(dictionary *d) {
    if (!d) return;
    for (size_t i = 0; i < d->count; ++i) free(d->words[i]);
    free(d->words); free(d->buckets); free(d->next);
    free_edges(d->root.child); free(d);
}
static bool build_index(dictionary *d) {
    if (d->method == METHOD_HASH) {
        d->buckets = calloc(BUCKETS, sizeof(*d->buckets));
        d->next = calloc(d->count, sizeof(*d->next));
        if (!d->buckets || !d->next) return false;
        for (size_t i = 0; i < d->count; ++i) {
            size_t h = hash_word(d->words[i]);
            d->next[i] = d->buckets[h]; d->buckets[h] = i + 1;
        }
    } else if (d->method == METHOD_TRIE) {
        for (size_t i = 0; i < d->count; ++i) {
            edge *node = &d->root;
            for (const unsigned char *p = (unsigned char *)d->words[i]; *p; ++p) {
                edge **child = &node->child;
                while (*child && (*child)->letter != *p) child = &(*child)->next;
                if (!*child) {
                    *child = calloc(1, sizeof(**child));
                    if (!*child) return false;
                    (*child)->letter = *p;
                }
                node = *child;
            }
            node->terminal = true;
        }
    }
    return true;
}
dictionary *dictionary_load(const char *path, dictionary_method method) {
    FILE *file = fopen(path, "r");
    if (!file) return NULL;
    dictionary *d = calloc(1, sizeof(*d));
    if (!d) { fclose(file); return NULL; }
    d->method = method;
    char word[WORD_MAX + 1]; bool too_long; int status;
    while ((status = read_word(file, word, &too_long)) > 0) {
        if (too_long) { errno = EOVERFLOW; goto fail; }
        if (d->count == d->capacity) {
            size_t capacity = d->capacity ? d->capacity * 2 : 1024;
            char **words = realloc(d->words, capacity * sizeof(*words));
            if (!words) goto fail;
            d->words = words; d->capacity = capacity;
        }
        char *copy = malloc(strlen(word) + 1);
        if (!copy) goto fail;
        strcpy(copy, word); d->words[d->count++] = copy;
    }
    if (status < 0) goto fail;
    if (!d->count) { errno = EINVAL; goto fail; }
    fclose(file); file = NULL;
    qsort(d->words, d->count, sizeof(*d->words), compare_words);
    size_t unique = 0;
    for (size_t i = 0; i < d->count; ++i) {
        if (unique && !strcmp(d->words[unique - 1], d->words[i])) free(d->words[i]);
        else d->words[unique++] = d->words[i];
    }
    d->count = unique;
    if (!build_index(d)) goto fail;
    return d;
fail: {
    int saved = errno;
    if (file) fclose(file);
    dictionary_free(d); errno = saved; return NULL;
}}
size_t dictionary_size(const dictionary *d) { return d ? d->count : 0; }
bool dictionary_check(const dictionary *d, const char *word) {
    if (!d || !word || !*word) return false;
    if (d->method == METHOD_HASH) {
        for (size_t i = d->buckets[hash_word(word)]; i; i = d->next[i - 1])
            if (!strcmp(word, d->words[i - 1])) return true;
        return false;
    }
    if (d->method == METHOD_TRIE) {
        const edge *node = &d->root;
        for (const unsigned char *p = (const unsigned char *)word; *p; ++p) {
            node = node->child;
            while (node && node->letter != *p) node = node->next;
            if (!node) return false;
        }
        return node->terminal;
    }
    for (size_t i = 0; i < d->count; ++i)
        if (!strcmp(word, d->words[i])) return true;
    return false;
}
/* Bounded Levenshtein distance; rows above the threshold stop early. */
static unsigned distance(const char *a, const char *b) {
    size_t n = strlen(a), m = strlen(b);
    if (n > m + 2 || m > n + 2) return 3;
    unsigned previous[WORD_MAX + 1], current[WORD_MAX + 1];
    for (size_t j = 0; j <= m; ++j) previous[j] = (unsigned)j;
    for (size_t i = 1; i <= n; ++i) {
        current[0] = (unsigned)i;
        unsigned minimum = current[0];
        for (size_t j = 1; j <= m; ++j) {
            unsigned v = previous[j] + 1;
            if (current[j - 1] + 1 < v) v = current[j - 1] + 1;
            unsigned substitution = previous[j - 1] + (a[i - 1] != b[j - 1]);
            if (substitution < v) v = substitution;
            current[j] = v;
            if (v < minimum) minimum = v;
        }
        if (minimum > 2) return 3;
        memcpy(previous, current, (m + 1) * sizeof(*previous));
    }
    return previous[m];
}
size_t dictionary_suggest(const dictionary *d, const char *word,
                          const char *results[SUGGEST_MAX]) {
    size_t count = 0;
    unsigned scores[SUGGEST_MAX];
    if (!d || !word || strlen(word) > WORD_MAX) return 0;
    for (size_t i = 0; i < d->count; ++i) {
        unsigned score = distance(word, d->words[i]);
        if (score > 2) continue;
        size_t pos = 0;
        while (pos < count && scores[pos] <= score) ++pos;
        if (pos == SUGGEST_MAX) continue;
        if (count < SUGGEST_MAX) ++count;
        for (size_t j = count - 1; j > pos; --j) {
            scores[j] = scores[j - 1]; results[j] = results[j - 1];
        }
        scores[pos] = score; results[pos] = d->words[i];
    }
    return count;
}
