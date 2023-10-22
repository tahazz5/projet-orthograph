#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include "hashTable.h"
#include "Arbre.h"
#include "Trie.h"
#include "ArbreSuffixe.h"
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>

double calculate(const struct rusage* b, const struct rusage* a);


int main(int argc, char* argv[]) {

	char* arquivotexto = (argc == 2) ? argv[1] : "a_la_recherche_du_temps.txt";

	struct rusage before, after;
	double ti_load = 0.0, ti_check = 0.0, ti_unload = 0.0;
	
	getrusage(RUSAGE_SELF, &before);
	//hashltable_t* _table = loadHash();
	getrusage(RUSAGE_SELF, &after);

	node_t* root = loadTRIE();

	// calculate time to load dictionary
    ti_load = calculate(&before, &after);

	
	FILE* file = fopen(arquivotexto, "r");
	FILE *rapport_hash = fopen("rapport_HASHTABLE.dat","w");

	int index = 0;
	int erros = 0;
	int words = 0;
	char word[TAMANHO + 1];

	for (int c = fgetc(file); c != EOF; c = fgetc(file)) {
		if (isalpha(c)) {
			word[index] = c;
			index++;
			if (index > TAMANHO) {
				while ((c = fgetc(file)) != EOF && isalpha(c));
				index = 0;
			}
		}
		else if (isdigit(c)) {
			while ((c = fgetc(file)) != EOF && isalnum(c));
			index = 0;
		}
		else if (index > 0) {
			word[index] = '\0';
			index = 0;
			words++;

		

			getrusage(RUSAGE_SELF, &before);
			  if(!checkTRIE(word,root)){
                printf("%s\n", word);
                erros++;
              }
			fprintf(rapport_hash, "%s\n", word);
			getrusage(RUSAGE_SELF, &after);

			ti_check += calculate(&before, &after);


		}
	}

	fclose(rapport_hash);
	fclose(file);
	getrusage(RUSAGE_SELF, &before);
	//bool b = unloadHash(_table);
	getrusage(RUSAGE_SELF, &after);
	unloadTRIE(root);
	ti_unload = calculate(&before, &after);

	printf("\n------------------------------------\n");
	printf("\nNumber of words in text: %d", words);
	printf("\nNumber of misspelled words: %d", erros);
	printf("\nLoad time: %f", ti_load);
	printf("\nCheck time: %f", ti_check);
	printf("\nUnload time: %f\n", ti_unload);
	printf("\n------------------------------------\n");

	return 0;
}

double calculate(const struct rusage* b, const struct rusage* a)
{
    if (b == NULL || a == NULL)
    {
        return 0.0;
    }
    else
    {
        return ((((a->ru_utime.tv_sec * 1000000 + a->ru_utime.tv_usec) -
                 (b->ru_utime.tv_sec * 1000000 + b->ru_utime.tv_usec)) +
                ((a->ru_stime.tv_sec * 1000000 + a->ru_stime.tv_usec) -
                 (b->ru_stime.tv_sec * 1000000 + b->ru_stime.tv_usec)))
                / 1000000.0);
    }
}