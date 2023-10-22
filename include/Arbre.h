#ifndef ABRE_H
#define ABRE_H
#include <stdbool.h>
#define TAMANHO 30
  typedef struct _node {
      char value;
      struct _node *sons, *brothers;       
      } tree_t;
tree_t * createNode(char c);

tree_t* addLetter(char c, tree_t * t );

bool checkArbre(const char* word,tree_t* main_tree);

tree_t* checkNode(char c, tree_t * t);

tree_t* loadArbre();

tree_t* addHorizontal(char c,tree_t * t,tree_t*father);
int getSizeTree(tree_t* tree);
void resetSize();
void unloadArbre(tree_t* t);
void prettify(tree_t* a);
int filelenght(char* filename);
void updateValue();



#endif 