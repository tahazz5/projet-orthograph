#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "../include/hashTable.h"




cellhashltable_t* crete_item(void* key, void* data){

    cellhashltable_t *item = malloc(sizeof(cellhashltable_t));
    (item)->data = (char*) malloc(strlen(data) + 1);
    (item)->key= key;
    
    strcpy((item)->data,data);
    (item)->hashcode = *((int*) key);
    return item;

}

hashltable_t* hashltable_new(int n) {

    // Allocate memory for the hash table and initialize its fields
    // basically creates a new table 
    hashltable_t* _table = (hashltable_t*)malloc(sizeof(hashltable_t));
    (_table)->total_number = 0;
    (_table)->size = n;
    (_table)->data = calloc(n, sizeof(void*));


    return _table;


}

void free_item(cellhashltable_t* item)
{
    // Frees an item.
    free(item->key);
    free(item->data);
    free(item);
}




bool hashltable_put(void* key, void* data, hashltable_t * table) {
     // Compute the hash code of the key
    int* hashcode = (int*)key;
     
    // Check if the key already exists in the hash table
    link_t* link ;
    link= (link_t*)table->data[*hashcode];
    char charArray[TAMANHO+1];
    strcpy(charArray,data);
   
    if(checkHashTable(link,charArray)){
        return true;
    }
    // The key does not exist, so add a new link to the appropriate bucket
    link_t* new_link = (link_t*)malloc(sizeof(link_t));
  
    if (new_link == NULL) {
        return false;  // Error: unable to allocate memory for the new link
    }

   
    
    cellhashltable_t* cell = crete_item(key, data);
    new_link->data = cell;
    if(table->data[*hashcode]==NULL){
        table->data[*hashcode] = new_link;
        new_link->next = NULL;
        return true;
    }
    new_link->next = table->data[*hashcode];
    table->data[*hashcode] = new_link;
    table->total_number++;
    return true;
    
}

bool checkTable(void* word, hashltable_t* table){
    char charArray[TAMANHO+1];
     int length = strlen(word);
     strcpy(charArray,(char*)word);
    for(int i = 0; i< length; i++){
            if(isupper(charArray[i]))
            {   
                charArray[i]=tolower(charArray[i]);
            }
    }
    charArray[length]='\0';
    unsigned int hashcode = fhachage((char*)charArray);
   
    return checkHashTable((void*)(table)->data[hashcode],charArray);
}

bool checkHashTable(void* initialLink, void* data){
    
    
    if(initialLink == NULL){
        return false;
    }    
    cellhashltable_t *cell;
    cell = (cellhashltable_t *) (((link_t*) initialLink)->data); 
    
    char* str = (cell->data);
    
    if(strcmp(str,data)==0){
        return true;
    }

    return checkHashTable(((link_t*)initialLink)->next,data);
    
}

 unsigned int fhachage(char* str)
{  

    unsigned long sum = 0;
    int a= 1.4;
    for (int i = 0; str[i]; i++)
    {
        sum += str[i]*a;
        a=a*1.4;
    }
     return  sum % CAPACITY;

}


hashltable_t* loadHash(){
    
    FILE* file = fopen("dico1.txt", "r");
    if (file == NULL) {
        return false;
    }
    hashltable_t* _table = hashltable_new(CAPACITY);
    char buffer[TAMANHO +1];
    char word[TAMANHO +1];
    int length;
 

    while (fscanf(file, "%s", buffer) > 0){
        length = strlen(buffer);
        for(int i = 0; i< length; i++){
            if(isupper(buffer[i]))
            {   
                buffer[i]=tolower(buffer[i]);
            }

        }
        buffer[length]='\0';
        int* hashcode = malloc(sizeof(*hashcode));
          *hashcode= fhachage(buffer);
        bool b = hashltable_put(hashcode,buffer,_table);

        if(!b){
            printf("\nops problems with %s\n",buffer);
        }
    }
    return _table;


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

void delete_key(void* key) {
    free(key);
}

void delete_data(void* data) {
    cellhashltable_t* cell = (cellhashltable_t*) data;
    free(cell->key);
    free(cell->data);
    free(cell);
}


bool unloadHash(hashltable_t* table){

    for(int i = 0; i < table->size; i++){
        
        link_t* link =(link_t*) table->data[i];
        delete_line(link);
    }
    free(table->data);
    free(table);
    
    return true; 
}

void delete_line(link_t* t ){
    if(t!= NULL){
        delete_line(t->next);
        free(t);
    }
}