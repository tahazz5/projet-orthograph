#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../include/Arbre.h"
#include <stdbool.h>

// The Proposal here is for each node we will have only one way down, and if this node have more childs, each child is connected as a brother
// The search method isn't the best ( because it demands a lot of linear searchs)

tree_t * createNode(char c){
     tree_t *t = malloc( sizeof( *t) );
     if(t==NULL){
        return NULL;
     }
    (t)->value=c;
    (t)->sons=(tree_t *)NULL;
    (t)->brothers=(tree_t *)NULL;
    return t;

}
// We return the value of the new node/ node found to keep time for the next addition.
tree_t* addLetter(char c,tree_t * t ){
    if( t->sons==NULL){
        tree_t* new_node =createNode(c);

        t->sons = new_node;

        return new_node;}
    else if(t->sons->value == c){
 
        return t->sons;
    }
    else {
         // The value isn't in 't' but we have to look after his brothers.

        return addHorizontal(c,t->sons,t);
    }
}
tree_t* addHorizontal(char c,tree_t * t,tree_t*father){
    
    if(t->value==c){
        return t;
    }
    
    else if((void*)t->brothers==NULL){
        tree_t* new_node =createNode(c);
        if(c=='\0'){
            t->brothers= new_node;
            return t->brothers;
        }
        new_node->brothers= father->sons;
        father->sons = new_node;
        return new_node;
    }
    else {
        return addHorizontal(c,t->brothers,father);
    }
}

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
bool checkArbre(const char* word, tree_t * main_tree)
{  
    // vide Mot 
    tree_t * lastModified;
    lastModified=main_tree;
    char buffer[TAMANHO+1];
    int word_length = strlen(word);
    for(int i= 0; i< word_length; i++){
        if(isupper(word[i]))
        {
            buffer[i]=tolower(word[i]);
        }else{
            buffer[i]=word[i];
        }
         
        lastModified = checkNode(buffer[i],(lastModified->sons));
        
        if(lastModified==NULL){
            return false;
        } 
    }  
    if( checkNode('\0',(lastModified->sons))!=NULL){
        return true;
    }else{
        return false;
    }
}

tree_t* loadArbre(){

    FILE* file = fopen("dico1.txt", "r");

	if (file == NULL) {
		return NULL;
	}

    tree_t* main_tree = createNode(0);
    if(main_tree==NULL){
        exit(0);
    }
    tree_t * lastModified;
    
    lastModified=main_tree;
    
    char buffer[TAMANHO+1];
    while (fscanf(file, "%s", buffer) > 0){
        int word_length = strlen(buffer);
        for(int i= 0; i< word_length; i++){
            if(isupper(buffer[i]))
            {
                buffer[i]=tolower(buffer[i]);
            }
            lastModified = addLetter(buffer[i],lastModified);
        
            
        }    
        buffer[word_length] = '\0';
        lastModified=addLetter(buffer[word_length],(lastModified));
        lastModified=main_tree;

    }
    fclose(file);

    return main_tree;
}


tree_t* checkNode(char c, tree_t * t){
    
    
    if((void*)t==NULL){
        return t;
    }
    if(t->value == c){
        //check if the node is already the value
        return t;
    }
    else {
         // The value isn't in 't' but we have to look after his brothers.
        return checkNode(c,t->brothers);
    }
}

//This was a last moment way to keep the value of a normal tree but not the best way for a fixed tree, since the same node would be count 
//many times.
int* size_arbre;

void resetSize(){
    if(size_arbre!=NULL){
        free(size_arbre);
        size_arbre=NULL;
    }
}
void updateValue(){
   
        *size_arbre=*size_arbre+1;
}
// here we just visit each node of the tree and sum one into the size.
int getSizeTree(tree_t* tree){
    if(size_arbre==NULL){
        size_arbre= malloc(sizeof(*size_arbre));
        *size_arbre=0;    
    }
    updateValue();
    if(tree!=NULL){
    
    }
    if(tree->sons!=NULL){
        getSizeTree(tree->sons);
    }
    if(tree->brothers!=NULL){
        getSizeTree(tree->brothers);
    }    
    return *size_arbre;
    
}


void unloadArbre(tree_t* t)
{
    if(t==NULL){
        return;
    }
    if(t->sons!=NULL){
        unloadArbre(t->sons);
    }
    if(t->brothers!=NULL){
        unloadArbre(t->brothers);
    }
    free(t);
}

void prettify(tree_t* a){

    printf("\nNode %p:\n\tchar-> %c\n\tbrother -> %p \n\tsons->%p,",a,a->value,a->brothers,a->sons);

}
