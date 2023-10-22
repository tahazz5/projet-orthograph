#include <time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "../include/Arbre.h"



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
      
      FILE *ftemps = fopen("temps_Arbre.dat","w");
     


      tree_t* main_tree ;
      char buffer[TAMANHO +1];
      int length;
      tree_t* lastModified;
      for (int k=nbwords; k>1; k -= nbwords/100)
      {
        FILE* file = fopen(av[1], "r");
        main_tree = createNode(0);
        lastModified= main_tree;

        debut=clock();
        int word_count = 0;
        char buffer[TAMANHO+1];
        while (word_count< k && fscanf(file, "%s", buffer) > 0)
        {
          int word_length = strlen(buffer);
          for(int i= 0; i< word_length; i++){
            if(isupper(buffer[i]))
            {
                buffer[i]=tolower(buffer[i]);
            }
            lastModified = addLetter(buffer[i],lastModified);              
          }    
          buffer[word_length] = '\0';
          lastModified  = addLetter(buffer[word_length],(lastModified));
          lastModified  = main_tree;
          word_count++;

        }
          fin=clock();
          resetSize();
          double taille = (getSizeTree(main_tree)*sizeof(*main_tree))/1000000;
          
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
              

              if(!checkArbre(word,main_tree)){
               // printf("%s\n", word);
                
              } 
            }
          }
          fin1=clock();
          
          unloadArbre(main_tree);
    
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
       
       system("gnuplot -p -e \"f(x) = a * (x) + b ; fit f(x) 'temps_Arbre.dat' u 1:2 via a,b;"
                   "set y2range [0:100];set y2tics nomirror;"
                   "plot 'temps_Arbre.dat' u 1:2 with line lt 4 axis x1y2 title 'Taille Arbre', a * (x) + b  with line lt 6 axis x1y2 title 'Fit Taille Arbre';"
                   "g(x) = c * (x) + d ;fit g(x) 'temps_Arbre.dat' u 1:3 via c,d;"
                   "replot 'temps_Arbre.dat' u 1:3 with line lt 2 title 'Recherchep Arbre', g(x)  with line lt 8 title 'Fit Recherche Arbre'; quit\"");
    
     
    _exit(0);
    return 0;
}
