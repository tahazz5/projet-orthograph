#ifndef _TRIE
#define _TRIE
// Inclusion des listes génériques

#define TAMANHO 30
#define CHARACTER 26
#include <stdbool.h>

typedef struct node {
	bool palavras_i;
	struct node* sons[CHARACTER];
} node_t;

node_t * loadTRIE();
bool checkTRIE(const char* word,node_t* root);
void unloadNode(node_t* top);
void unloadTRIE(node_t* root);
int filelenght(char* filename);
#endif
