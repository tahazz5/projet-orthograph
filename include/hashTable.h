
      #ifndef _HASHLTABLE
      #define  _HASHLTABLE
      // Inclusion des listes génériques
    
      #define TAMANHO 30
      #define CAPACITY 30000
      #include <stdbool.h>
      


      ///////// Les maillons des listes génériques contiennent des pointeurs vers des triplets clés, valeurs, hashcode.
      ///////// Le hashcode n'est utile que pour l'optimisation du redimensionnement

      typedef struct _link {
      void* data;               /* un élément de la liste*/
      struct _link *next;       /* l'adresse du maillon suivant */
      } link_t;

      typedef struct {
        void* key;
        void* data;
        unsigned int hashcode;
      } cellhashltable_t;

      ///////// Le type des tables d'association
     
    cellhashltable_t* crete_item(void* key, void* data);

      typedef struct {
        unsigned int total_number;
        unsigned int size;        
        void**  data;
        } hashltable_t;

      hashltable_t* getTable();
      bool checkTable(void* word, hashltable_t* table );
      bool checkHashTable(void* initialLink, void* data);
      hashltable_t* loadHash();
      hashltable_t* hashltable_new(int n);
      //hashltable_t* hashltable_delete(hashltable_t* table);
      bool hashltable_put(void* key, void* data, hashltable_t* table) ;
      //void* hashltable_get_value(void* key, hashltable_t* table) ;
      //void hashltable_remove_key(void* key, hashltable_t *table) ;

      bool unloadHash(hashltable_t* table);

      void delete_line(link_t* t );
      void bk();

      unsigned int fhachage(char* str);
      // Optionel, si Redimensionnement
      // sens =1 pour accroitre la table, -1 sinon
      hashltable_t* hashltable_delete(hashltable_t* table);
      int hashltable_new_length(int sens, hashltable_t* table);
      int filelenght(char* filename);
      void delete_key(void* key);
      void delete_data(void* data);
      #endif

