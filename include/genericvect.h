#ifndef _GENERIC_VECT_H
#define _GENERIC_VECT_H
#include <stdio.h>
#include<assert.h>
#include <stdlib.h>
#include <string.h>
////////////////////////
#ifdef catid
#undef catid
#endif
#define catid(a,b) a##b

#ifdef makeid
#undef makeid
#endif
#define makeid(a,b) catid(a,b)

////////////////////////
// Generation de code pour des vecteurs redimensionnables

// API, avec type=double, complex, pcomplex, mystring, etc....

    /////////////////////////
// Macro pour definir les noms des fonctions et des types

// Definition de nom du type des vects, sous la forme vect_type_t
#define vecttype(type) makeid(makeid(vect_,type),_t)

// Definition du nom des fonctions, sous la forme vect_func
// func sera _new, _del_first, etc...
#define vectfunction(type,func) makeid(makeid(vect_,type),func)


////////////////////////
// Definition des structures des vects par la macro maketypevect
// Le nom de la structure est vect_type_t
// Elle comporte les champs
//    - data : un tableau de donnees alloue dynamiquement,
//    - max_size : la taille max du tableau
//    - actual_size : la taille actuelle du tableau

#define makevecttype(type) typedef struct makeid(_vect,type) { \
    type* data;\
    unsigned int max_size;\
    unsigned int actual_size;\
    void (*fprint_data)(type*,FILE*);\
    type* (*delete_data)(type*);\
    int (*equal_data)(type*,type*);\
  }  * makeid(makeid(vect_,type),_t);
/////////////////////////////////
// Definition des fonctions de base sur les vects
// Creation d'un vect de n elements vide
// Allocation du tableau de n elements, taille actuelle nulle.
#define makevectnew(type) \
  vecttype(type) vectfunction(type,_new)(unsigned int n, \
        void (*fprint_data)(type*,FILE*),\
        type* (*delete_data)(type*),\
        int (*equal_data)(type*,type*)) {\
      /* Définition d une variable de type vect_type_t et de nom table */\
    vecttype(type) table=NULL;\
    if ( (table=calloc(1,sizeof(*table))) ==NULL) return NULL;\
    if (n<=0) n=1;\
    if ( (table->data=calloc(n,sizeof(*(table->data)))) ==NULL) {free(table); return NULL;}\
    table->max_size=n;\
    table->actual_size=0;\
    table->fprint_data=fprint_data;\
    table->delete_data=delete_data;\
    table->equal_data=equal_data;\
    return table;\
  }\
// Ajout d'un element en derniere place.
// Augmente la taille du tableau si necessaire
#define makevectappend(type) \
vecttype(type) vectfunction(type,_append)(type e, vecttype(type) table)  {\
    if((2*(table->max_size)/3)<table->actual_size){\
      table->data=realloc(table->data,2*table->max_size*sizeof(type));\
      table->max_size=2*table->max_size;\
      }\
    table->data[table->actual_size++]= e;\
    return table;\
  }\
// Affichage du tableau dans un flux
#define makevectfprint(type) \
  void vectfunction(type,_fprintf)(vecttype(type) table,FILE* fp) {\
    int c;\
    for(c=0;c<table->actual_size;c++){\
      table->fprint_data(table->data+c,fp);\
    }\
  }\
// Affichage du tableau sur la sortie standart
#define makevectprint(type) \
  void vectfunction(type,_printf)( vecttype(type) table) {\
    int d;\
    for(d=0;d<table->actual_size;d++){\
      table->fprint_data(table->data+d,stdout);\
    }\
  }\

// Liberation de la table
#define makevectdelete(type) \
  void vectfunction(type,_delete)(vecttype(type) table) {int i; \
    if(table->delete_data)\
      for (i=0; i<table->max_size; i++)\
          table->delete_data(table->data+i);\
    table->actual_size=0;\
    free(table->data);\
    free(table);\
  }
//
// Recherche d'un element dans un vecteur
// Utilise la fonction equal_data si elle existe ou memcpy sinon
#define makevectfind(type)\
  int vectfunction(type,_find)(type param, vecttype(type) tab) { \
    int j;\
    if(tab->equal_data){\
      for(j=0;j<tab->actual_size;j++){\
        if(!tab->equal_data(tab->data+j,&param)){\
          return j;\
        }\
      }\
    }\
    else{\
      for(j=0;j<tab->actual_size;j++){\
        if(memcmp(tab->data+j,&param,sizeof(type))==0){\
          return j;\
        }\
      }\
    }\
    return -1;\
  }\
// Recherche dichotomique
#define makevectrechdic(type)\
  int vectfunction(type,_rechdic)(type param, vecttype(type) t1) { \
    if (t1->actual_size == 0) {\
        return -1;\
    }\
    int a = 0;\
    int b = t1->actual_size - 1;\
    for (; a <= b; ) {\
      int m =(a+b)/2;\
      int res= t1->equal_data(t1->data+m,&param);\
      if(res==0){\
         return m;\
      }\
       if(res<0){\
         a=m+1 ;\
        }\
        else\
        b=m-1;\
    }\
    return -1 ;\
  }\
// trirapide
#define makevectquicksort(type)\
void vectfunction(type,_swap)(type* param, type* t1) { \
    type t = *param;\
    *param = *t1;\
    *t1 = t;\
}\
void vectfunction(type,_quick_sort)(vecttype(type) t1,int a,int b) { \
    int p,j,i=0;\
    if (a < b) {\
        vectfunction(type,_swap)(t1->data + rand()%(b-a+1)+a, t1->data + b);\
        type param = t1->data[b];\
        i = a - 1;\
        for (j = a; j < b; j++) {\
            if (t1->equal_data(t1->data+j,&param)<0) {\
                i++;\
                vectfunction(type,_swap)(t1->data+i, t1->data+j);\
            }\
        }\
      p = i + 1;\
      vectfunction(type,_swap)(t1->data+i+1, t1->data+b);\
      vectfunction(type,_quick_sort)(t1, p + 1, b);\
      vectfunction(type,_quick_sort)(t1, a, p - 1);\
    } \
}\
void vectfunction(type,_qsort)(vecttype(type) t1) { \
   vectfunction(type,_quick_sort)(t1,0,t1->actual_size-1);\
}\
int vectfunction(type,_verif)(vecttype(type) t1) { \
  int i ;\
  for(i=0;i<t1->actual_size-1;i++){\
    if(t1->equal_data(t1->data+i,t1->data+i+1)>0)\
      return 1;\
     }\
     return 0;\
  }  
#define makevectinsert(type) \
vecttype(type) vectfunction(type,_insert)(type e, vecttype(type) table)  {\
    if(table->max_size>table->actual_size){\
       int i = table->actual_size- 1;\
       while (i > 0 && table->equal_data(table->data+i,&e)>0) {\
         table->data[i+1]=table->data[i];\
         i--;\
      }\
      table->data[i+1] = e;\
    }\
    if((2*(table->max_size)/3)<table->actual_size){\
      table->data=realloc(table->data,2*table->max_size*sizeof(type));\
       int p = table->actual_size- 1;\
       while (p > 0 && table->equal_data(table->data+p,&e)>0) {\
         table->data[p+1]=table->data[p];\
         p--;\
      }\
      table->data[p+1] = e;\
      table->max_size=2*table->max_size;\
    }\
    table->actual_size++;\
    return table;\
  }
////////////////////////////////
// Macro regroupant toutes les definitions de fonctions pour la creation de toutes
// les fonctions sur les vecteurs. A utiliser dans vect_type.c.
#define makevectfunction(type) \
makevectnew(type)\
makevectappend(type)\
makevectdelete(type)\
makevectfprint(type)\
makevectprint(type)\
makevectfind(type)\
makevectrechdic(type)\
makevectquicksort(type)\
makevectinsert(type)

// Macro regroupant toutes les declarations de fonctions
// Pour déclarer toutes les fonctions
#define makevectproto(type) \
vecttype(type) vectfunction(type,_new)(unsigned int n, \
      void (*fprint_data)(type*,FILE*),\
      type* (*delete_data)(type*),\
      int (*equal_data)(type*,type*)) ;\
vecttype(type) vectfunction(type,_append)(type e, vecttype(type) table)  ;\
void vectfunction(type,_printf)(vecttype(type) table);\
void vectfunction(type,_fprintf)( vecttype(type) table,FILE*);\
void vectfunction(type,_delete)(vecttype(type) table);\
int vectfunction(type,_find)(type param, vecttype(type) l) ;\
int vectfunction(type,_rechdic)(type param, vecttype(type) l) ;\
void vectfunction(type,_qsort)(vecttype(type) t1) ; \
int vectfunction(type,_verif)(vecttype(type) t1) ;  \
vecttype(type) vectfunction(type,_insert)(type e, vecttype(type) table) ;\

#endif
