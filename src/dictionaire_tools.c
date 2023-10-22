
#include "../include/dictionaire_tools.h"

makevectfunction(str)
void str_fprintf(str* v,FILE* fp) { fprintf(fp,"%s ",*v); }
int str_equal(str* pv1, str* pv2) { 
  return strcoll(*pv1,*pv2); 
  }

int lecture(FILE* fp, char* mot) {
   char inter[512];
   int j,c;
   j=0;
   do {
     c=fgetc(fp);
     if (c==EOF) return 0;
     if (c=='\n' || c=='\r') break;
     if (!ispunct(c) && !isspace(c)) inter[j++]=c;
     else if (c=='\'') j=0;
     else if (c==' ') {
       if (j && ispunct(inter[j-1])) j--;
       else break;
     }
     else  break;
   } while (1);
   if (j && inter[j-1]=='-') j--;
   inter[j]=0;
   strcpy(mot,inter);
   if(!strcoll(mot,"–")) strcpy(mot,"");
   return 1;
 }

  vect_str_t loadtable(vect_str_t t1,char* dico, int k){

  FILE* file = fopen(dico, "r");
  int word_count = 0;

  do {
    char* buffer=calloc(31,sizeof(*buffer));
    if(lecture(file,buffer)==0) {
      free(buffer);
      break;  
      }
    vect_str_append(buffer,t1); 
  word_count ++;
  printf(" count word :%d\n",word_count);
  
  }while(word_count<k);

  fclose(file);
  return t1 ;
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
