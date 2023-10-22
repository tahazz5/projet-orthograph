#ifndef ARBRE_H
#define ARBRE_H
#include <stdbool.h>
#include "Arbre.h"
//Function que charge les suffixes
tree_t* loadSuffixes();
//Functions que changent des suffixes
void pivot(tree_t* tree,tree_t* suf);
void visitPrefix(tree_t*now ,tree_t* father,tree_t*tree);
tree_t* compressTree(tree_t* tree,tree_t* target, tree_t* target_fils);
tree_t* visit(tree_t* target,tree_t* target_fils, tree_t* now, tree_t* before,tree_t* father, tree_t* node_ack, tree_t* clearTree);
tree_t*  addAtLast(tree_t* node, tree_t* addLast);
#endif 