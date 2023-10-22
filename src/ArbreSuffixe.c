#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/Arbre.h"
#include "../include/ArbreSuffixe.h"
#include <stdbool.h>
tree_t* compressTree(tree_t* tree,tree_t* target, tree_t* target_fils){
    if(tree==NULL){
        return NULL;
    }
    //Given a tree first check the son.
    //after Check the brothers
    tree_t* node_ack=NULL;
    tree_t* clearTree=NULL;
    if(tree->sons!=NULL){
        node_ack= visit(target,target_fils,tree->sons,NULL,tree,node_ack,clearTree);        
    }
    unloadArbre(clearTree);
    return node_ack;
}
void pivot(tree_t* tree,tree_t* suf){
//Nous irons parcourir l'arbre en gardent le node actuel e ce superieur. 
    tree_t* deleteTree = NULL;
    visitPrefix(suf,NULL,tree);
}

//Nous vositons 
void visitPrefix(tree_t*now ,tree_t* father,tree_t*tree){
    tree_t* past = compressTree(tree,now,father);
 
    if(now->brothers!=NULL){
        visitPrefix(now->brothers,father,tree);
    }
    if(now->sons!=NULL){
        visitPrefix(now->sons,past,tree);
    }    
  
}


tree_t* addAtLast(tree_t* node, tree_t* addLast){
    if(node->brothers==NULL){
        node->brothers=addLast;
        return addLast;
    }
    else{   
        return addAtLast(node->brothers,addLast); 
    }
}
tree_t* visit(tree_t* target,tree_t* target_fils, tree_t* now, tree_t* before,tree_t* father, tree_t* node_ack, tree_t* clearTree){
    tree_t * brother,*hold;
    if(now!=NULL){
        //Et apres nous irons visiter toute l'arbre principale et regarde se le valeur est 
        //le target->value et si le fils du target est le pere de la arbre de suffixe.
        if(now->value==target->value && target_fils==now->sons && now->brothers==NULL){
            if(node_ack==NULL){
            // Si on trouve cette combinaison, on garde le noveaux adresse de target et apres on pointe tout les autres matches vers ce adresse.
                node_ack=now;
            }
            
            if(before==NULL ){
    
                    father->sons = node_ack;
                    if(clearTree==NULL){
                        clearTree=now;
                    }else{
                        now->brothers=clearTree;
                    }
            }
            else {
                if(clearTree==NULL){
                    clearTree=now;
                }else{
                    now->brothers=clearTree;
                }
                before->brothers= node_ack;
            }
                return node_ack;
        }
        if(now->sons !=NULL){
          
            node_ack = visit(target,target_fils,now->sons,NULL,now,node_ack,clearTree);
        }
        
        if(now->brothers!=NULL){

            node_ack =visit(target,target_fils,now->brothers,now,father,node_ack,clearTree);
        }    


    }
    return node_ack;
   
}


tree_t* loadSuffixes(){
    //Ici on garde une arbre que comence par \0 et a des suffixes au inverse (ex: a la place du ces\0 on a \0sec)
    FILE* file = fopen("text/suffixes.txt", "r");

	if (file == NULL) {
		return NULL;
	}

    tree_t* main_tree = createNode('\0');
    tree_t * lastModified;
    
    lastModified=main_tree;
    
    char buffer[TAMANHO+1];
    while (fscanf(file, "%s", buffer) > 0){
        int word_length = strlen(buffer);
        for(int i= word_length-1; -1< i; i--){
            if(isupper(buffer[i]))
            {
                buffer[i]=tolower(buffer[i]);
            }
            lastModified = addLetter(buffer[i],(lastModified));
            
        }  
        lastModified=main_tree;

    }
    fclose(file);
    return main_tree;
}
