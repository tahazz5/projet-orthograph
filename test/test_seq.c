#include "../include/dictionaire_tools.h"


int main(int argc, char *argv[]) {


/*       if (argc!=2 && argc!=3) 
      
      { printf("Usage : %s ficher_dico fichier_a_verifier\n",argv[0]); 
      exit(EXIT_FAILURE);}   */  

      FILE* file = fopen(argv[2], "r");
      clock_t debut,fin,debut1,fin1;

      if (file==NULL) {
        printf("IMPOSSIBLE D OUVRIR LE FICHIER");
        exit(EXIT_FAILURE);
      }

      //int nbwords=filelenght(argv[1]);
      int nbwords=10000;

      FILE *ftemps = fopen("temps_Seq.dat","w");


      vect_str_t t1,t2;
      t1=vect_str_new(0,str_fprintf,NULL,str_equal);
      t2=vect_str_new(0,str_fprintf,NULL,str_equal);
      
      for (int k=nbwords; k>1; k -= nbwords/20)
      {

        int word_count1 = 0;
        debut=clock();
        loadtable(t1,argv[1],k);
        fin=clock();

        double taille = ((t1->max_size*sizeof(vect_str_t))/1000000);

        debut1=clock();

          int j=0 ;
          do {
            char* buffer=calloc(31,sizeof(*buffer));
            if(lecture(file,buffer)==0) {
              free(buffer);
              break;  
              }
              int i=vect_str_find(buffer,t1);
              j++;
              if(*buffer &&!isupper(*buffer) && !isdigit(*buffer)&& i==-1 && vect_str_find(buffer,t2)==-1) {
                //printf("num %d : %s : pas dans le dico \n ",j,buffer);
                vect_str_append(buffer,t2);
              } 
              word_count1++;

          }while(word_count1<k);

        fin1=clock();

        
        fprintf(ftemps,"%d %lf %lf\n",k,taille,((double)fin1-debut1)/CLOCKS_PER_SEC);

      }

      
      vect_str_delete(t1);
      vect_str_delete(t2);

      fclose(file); 
      fclose(ftemps);  

    /*
          Trace avec gnuplot des données enregistrée dans le fichier
          f(x) est la fonction théorique de taille mémoire en fonction du nombre de mots
          g(x) celle de la complexité
        */

      system("gnuplot -p -e \"f(x) = a * (x) + b ; fit f(x) 'temps_Seq.dat' u 1:2 via a,b;"
                   "set y2range [0:100];set y2tics nomirror;"
                   "plot 'temps_Seq.dat' u 1:2 with line lt 4 axis x1y2 title 'Taille Seq', a * (x) + b  with line lt 6 axis x1y2 title 'Fit Taille Seq';"
                   "g(x) = c * (x) + d ;fit g(x) 'temps_Seq.dat' u 1:3 via c,d;"
                   "replot 'temps_Seq.dat' u 1:3 with line lt 2 title 'Recherchep Seq', g(x)  with line lt 8 title 'Fit Recherche Seq'; quit\"");





}
