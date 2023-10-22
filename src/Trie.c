#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "../include/Trie.h"



int _size = 0;
int filelenght(char* filename){

    FILE *file = fopen(filename, "r");

    char index;
	int words = 0;
	
        while((index = fgetc(file)) != EOF){  
            //Counts each word  
            if(index ==' ' || index == '\n')  
                words++;  
        }  

    fclose(file);
    return words; 

}

node_t* loadTRIE() {
	node_t* root = malloc(sizeof(*root));
	if (root == NULL) {
		return false;
	}
	root->palavras_i = false;
	for (int i = 0; i < CHARACTER; i++) {
		root->sons[i] = NULL;
	}

	FILE* file = fopen("dico1.txt", "r");
	if (file == NULL) {
		unloadTRIE(root);
		return false;
	}

	char word[TAMANHO + 1];
	int idx;

	while (fscanf(file, "%s", word) != EOF) {
		node_t* son = root;
		for (int i = 0, len = strlen(word); i < len; i++) {
			idx = (int)word[i] - (int)'a';
			if (son->sons[idx] == NULL) { 
				son->sons[idx] = malloc(sizeof(node_t));
				if (son->sons[idx] == NULL) { 
					return false;
				}
				son->sons[idx]->palavras_i = false;
				for (int j = 0; j < CHARACTER; j++) 
				{
					son->sons[idx]->sons[j] = NULL;
				}
			}

			son = son->sons[idx];
		}
		son->palavras_i = true;
		_size++;
	}

	fclose(file);

	return root;
}


bool checkTRIE(const char* word,node_t* root) {
	int idx;
	node_t* son = root;
	for (int i = 0, len = strlen(word); i < len; i++) {
		idx = (int)tolower(word[i]) - (int)'a';

		son = son->sons[idx];
		if (son == NULL) {
			return false;
		}
	}
	return son->palavras_i;
}

void unloadNode(node_t* top) {
	if (top == NULL)
		return;
	for (int i = 0; i < CHARACTER; i++) {
		if (top->sons[i] != NULL) {
			unloadNode(top->sons[i]);
		}
	}
	free(top);
}

void unloadTRIE(node_t* root) {
	unloadNode(root);
}