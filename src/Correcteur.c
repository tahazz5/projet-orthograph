#include "dictionary.h"
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void usage(FILE *out, const char *name) {
    fprintf(out, "Usage : %s [options] fichier.txt\n"
        "  -d, --dictionary FICHIER  Dictionnaire (text/dico1.txt)\n"
        "  -m, --method METHODE      hash (defaut), trie ou seq\n"
        "  -r, --report FICHIER      Rapport des mots inconnus\n"
        "  -s, --suggest            Jusqu'a 5 suggestions par mot\n"
        "  -h, --help               Afficher cette aide\n"
        "Utiliser - comme fichier pour lire l'entree standard.\n", name);
}
static bool same_file(const char *a, const char *b) {
    struct stat first, second;
    return !stat(a, &first) && !stat(b, &second) &&
           first.st_dev == second.st_dev && first.st_ino == second.st_ino;
}
static double elapsed(clock_t start) {
    return (double)(clock() - start) / CLOCKS_PER_SEC;
}
int main(int argc, char **argv) {
    const char *dictionary_path = "text/dico1.txt", *input = NULL, *report = NULL;
    const char *method_name = "hash";
    dictionary_method method = METHOD_HASH;
    bool suggest = false;
    for (int i = 1; i < argc; ++i) {
        const char *arg = argv[i];
        if (!strcmp(arg, "-h") || !strcmp(arg, "--help")) {
            usage(stdout, argv[0]); return 0;
        }
        if (!strcmp(arg, "-s") || !strcmp(arg, "--suggest")) { suggest = true; continue; }
        if (!strcmp(arg, "-d") || !strcmp(arg, "--dictionary") ||
            !strcmp(arg, "-m") || !strcmp(arg, "--method") ||
            !strcmp(arg, "-r") || !strcmp(arg, "--report")) {
            if (++i == argc) { fprintf(stderr, "Valeur manquante pour %s.\n", arg); return 1; }
            if (!strcmp(arg, "-d") || !strcmp(arg, "--dictionary")) dictionary_path = argv[i];
            else if (!strcmp(arg, "-r") || !strcmp(arg, "--report")) report = argv[i];
            else method_name = argv[i];
        } else if (arg[0] == '-' && strcmp(arg, "-")) {
            fprintf(stderr, "Option inconnue : %s\n", arg); return 1;
        } else if (input) {
            fprintf(stderr, "Un seul fichier texte est accepte.\n"); return 1;
        } else input = arg;
    }
    if (!input) { usage(stderr, argv[0]); return 1; }
    if (!strcmp(method_name, "trie")) method = METHOD_TRIE;
    else if (!strcmp(method_name, "seq")) method = METHOD_SEQ;
    else if (strcmp(method_name, "hash")) {
        fprintf(stderr, "Methode inconnue : %s (hash, trie, seq).\n", method_name); return 1;
    }
    if (!setlocale(LC_CTYPE, "C.UTF-8") && !setlocale(LC_CTYPE, "")) {
        fprintf(stderr, "Impossible d'initialiser la locale UTF-8.\n"); return 1;
    }
    if (report && (same_file(report, dictionary_path) ||
                   (strcmp(input, "-") && same_file(report, input)))) {
        fprintf(stderr, "Le rapport doit etre distinct du texte et du dictionnaire.\n");
        return 1;
    }
    clock_t start = clock();
    dictionary *dict = dictionary_load(dictionary_path, method);
    if (!dict) { fprintf(stderr, "Dictionnaire %s : %s\n", dictionary_path, strerror(errno)); return 1; }
    double load_time = elapsed(start);
    FILE *file = !strcmp(input, "-") ? stdin : fopen(input, "r");
    if (!file) {
        fprintf(stderr, "Texte %s : %s\n", input, strerror(errno)); dictionary_free(dict); return 1;
    }
    /* Buffer the report so a failed analysis preserves any previous report. */
    FILE *output = report ? tmpfile() : NULL;
    if (report && !output) {
        perror("Rapport temporaire"); if (file != stdin) fclose(file); dictionary_free(dict); return 1;
    }
    size_t words = 0, errors = 0, long_words = 0;
    char word[WORD_MAX + 1]; bool too_long; int status;
    start = clock();
    while ((status = read_word(file, word, &too_long)) > 0) {
        ++words;
        if (too_long) {
            ++long_words;
            fprintf(stderr, "Mot de plus de %d octets ignore.\n", WORD_MAX);
            continue;
        }
        if (dictionary_check(dict, word)) continue;
        ++errors;
        printf("%s", word);
        if (output) fprintf(output, "%s\n", word);
        if (suggest) {
            const char *results[SUGGEST_MAX];
            size_t n = dictionary_suggest(dict, word, results);
            if (n) printf(" -> ");
            for (size_t j = 0; j < n; ++j) printf("%s%s", j ? ", " : "", results[j]);
        }
        putchar('\n');
    }
    double check_time = elapsed(start);
    if (file != stdin) fclose(file);
    int result = 0;
    if (status < 0) { fprintf(stderr, "Erreur de lecture : texte UTF-8 attendu.\n"); result = 1; }
    if (output) {
        if (!result) {
            FILE *destination = fopen(report, "w");
            if (!destination) { perror("Rapport"); result = 1; }
            else {
                if (fflush(output) || ferror(output) || fseek(output, 0, SEEK_SET)) result = 1;
                char buffer[4096]; size_t n;
                while (!result && (n = fread(buffer, 1, sizeof(buffer), output)) > 0)
                    if (fwrite(buffer, 1, n, destination) != n) result = 1;
                if (ferror(output)) result = 1;
                if (fclose(destination)) result = 1;
                if (result) fprintf(stderr, "Erreur d'ecriture du rapport.\n");
            }
        }
        fclose(output);
    }
    start = clock();
    size_t count = dictionary_size(dict);
    dictionary_free(dict);
    double unload_time = elapsed(start);
    if (!result) printf("\nMethode : %s\nDictionnaire : %zu mots uniques\n"
        "Mots analyses : %zu\nMots inconnus : %zu\nMots trop longs : %zu\n"
        "Temps CPU (s) : chargement %.6f | analyse %.6f | liberation %.6f\n",
        method_name, count, words, errors, long_words, load_time, check_time, unload_time);
    if (fflush(stdout) || ferror(stdout)) result = 1;
    return result;
}
