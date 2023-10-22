#include <time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "../include/hashTable.h"



//il itere 100 fois 
//il lit les k premiers mots du dictionary 
//il calcule la taille de ce dictionary
//Il fait la verification et mesure le temps de verification
//Il écrit les données dans un fichier.
    


int main(int ac, char**av) {
  

  clock_t debut,fin,debut1,fin1;

    if (ac!=2 && ac!=3) 
    {     

        printf("Usage : %s ficher_dico fichier_a_verifier\n",av[0]); 
        exit(EXIT_FAILURE);
                
    } 

      


      // Nombre de mots du dico
      int nbwords=filelenght(av[1]);
      
      FILE *ftemps = fopen("temps_Hash.dat","w");
     


      hashltable_t* _table; 
      char buffer[TAMANHO +1];
      int length;

      for (int k=nbwords; k>1; k -= nbwords/100)
      {
        FILE* file = fopen(av[1], "r");
        _table = hashltable_new(CAPACITY);
        debut=clock();
        int word_count = 0;
        
        while (word_count < k && fscanf(file, "%s", buffer)>0)
        {
          
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

          word_count++;


        }
             fin=clock();

          double taille = ((sizeof(cellhashltable_t)*( (_table->total_number)) )+ _table->size*sizeof(void*))/1000000;
        
          debut1=clock();
          FILE* file2 = fopen(av[2], "r");
          int index = 0;
          int erros = 0;
          int words = 0;
          char word[TAMANHO +1];

          for (int c = fgetc(file2); c != EOF; c = fgetc(file2)) {
            
            if (isalpha(c)) {
              word[index] = c;
              index++;
              if (index > TAMANHO) {
                while ((c = fgetc(file2)) != EOF && isalpha(c));
                index = 0;
              }
            }
            else if (isdigit(c)) {
              while ((c = fgetc(file2)) != EOF && isalnum(c));
              index = 0;
            }
            else if (index > 0) {
              word[index] = '\0';
              index = 0;
              

              if(!checkTable(word,_table)){
                
              } 
            }

          }
          fin1=clock();
          
          bool a = unloadHash(_table);
    
          fprintf(ftemps,"%d %lf %lf\n",k,taille,((double)fin1-debut1)/CLOCKS_PER_SEC);

          fclose(file2);
          fclose(file);

      } 
      fclose(ftemps);
 
        /*
          Trace avec gnuplot des données enregistrée dans le fichier
          f(x) est la fonction théorique de taille mémoire en fonction du nombre de mots
          g(x) celle de la complexité
        */
       
       system("gnuplot -p -e \"f(x) = a * (x) + b ; fit f(x) 'temps_Hash.dat' u 1:2 via a,b;"
                   "set y2range [0:100];set y2tics nomirror;"
                   "plot 'temps_Hash.dat' u 1:2 with line lt 4 axis x1y2 title 'Taille Hash', a * (x) + b  with line lt 6 axis x1y2 title 'Fit Taille Hash';"
                   "g(x) = c * (x) + d ;fit g(x) 'temps_Hash.dat' u 1:3 via c,d;"
                   "replot 'temps_Hash.dat' u 1:3 with line lt 2 title 'Recherchep Hash', g(x)  with line lt 8 title 'Fit Recherche Hash'; quit\"");
    
     
    _exit(0);
    return 0;
}
