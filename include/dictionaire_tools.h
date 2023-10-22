#ifndef DICTIONARY
#define DICTIONARY

#include "genericvect.h"

#include <time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

typedef char* str;
makevecttype(str)
makevectproto(str)
void str_fprintf(str* v,FILE* fp);
int str_equal(str* pv1, str* pv2);
int lecture(FILE* fp, char* mot);
vect_str_t loadtable(vect_str_t t1,char* dico,int k);
int filelenght(char* filename);



#endif 
