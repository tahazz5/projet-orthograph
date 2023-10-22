#include <time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "../include/Trie.h"

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
      // int nbwords=100;

      FILE *ftemps = fopen("temps_Trie.dat","w");

      for (int k=nbwords; k>1; k -= nbwords/100)
      {
        node_t* root;
        int _size = 0;

        FILE* file = fopen(av[1], "r");
        debut=clock();
        int word_count = 0;

        	
        char wordss[TAMANHO + 1];
	      int idx;

          root = malloc(sizeof(*root));

          if (root == NULL) {
              return false;
          }
          root->palavras_i = false;
          for (int i = 0; i < CHARACTER; i++) {
              root->sons[i] = NULL; 
          }
        while (word_count < k && fscanf(file, "%s", wordss) != EOF)
        {  
          node_t* son = root;
          for (int i = 0, len = strlen(wordss); i < len; i++) 
          {
            idx = (int)(wordss[i]) - (int)'a';
            if (son->sons[idx] == NULL) 
            { 
              _size++;
              son->sons[idx] = malloc(sizeof(node_t));
              if (son->sons[idx] == NULL) 
              { 
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
          
          word_count++;
        }
            fin=clock();


          double taille = ((sizeof(node_t)*_size))/1000000;

          debut1=clock();
          FILE* file2 = fopen(av[2], "r");
          int index = 0;
          int erros = 0;
          int words = 0;
          char word[TAMANHO +1];
          int count = 0;
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
              

                if (!checkTRIE(word,root)) {
			          }  
            }
              
          } 
        fin1=clock();
          
          unloadTRIE(root);
    
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
       
       system("gnuplot -p -e \"f(x) = a * (x) + b ; fit f(x) 'temps_Trie.dat' u 1:2 via a,b;"
                   "set y2range [0:100];set y2tics nomirror;"
                   "plot 'temps_Trie.dat' u 1:2 with line lt 4 axis x1y2 title 'Taille Trie', a * (x) + b  with line lt 6 axis x1y2 title 'Fit Taille Trie';"
                   "g(x) = c * (x) + d ;fit g(x) 'temps_Trie.dat' u 1:3 via c,d;"
                   "replot 'temps_Trie.dat' u 1:3 with line lt 2 title 'Recherchep Tries', g(x)  with line lt 8 title 'Fit Recherche Tries'; quit\"");
    
     
    _exit(0);
    return 0;
}
