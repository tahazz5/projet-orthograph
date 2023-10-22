#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/list.h"

list_t list_new(void (*fprint_data)(void*,FILE*),void* (*delete_data)(void*),
          int (*equal_data)(void*,void*)) {
  list_t l = malloc(sizeof(struct _list));
  l->data = NULL;
  l->size = 0;
  l->delete_data = delete_data;
  l->fprint_data = fprint_data;
  l->equal_data = equal_data;
  
  return l;
}

int list_is_empty(list_t l) {
  return (l->size == 0);
}

int link_is_empty(link_t l) {
  return (l == NULL);
}

void* list_first(list_t l) {
  if (list_is_empty(l)) {
    fprintf(stderr, "Error: list is empty\n");
    exit(EXIT_FAILURE);
  }
  return l->data->data;
}

link_t link_add_first(void* e, link_t l) {
  link_t new_link = malloc(sizeof(struct _link));
  new_link->data = e;
  new_link->next = l;
  return new_link;
}

list_t list_add_first(void* e, list_t l) {
  l->data = link_add_first(e, l->data);
  l->size++;
  return l;
}

link_t link_del_first(link_t l , void* (*delete_data)(void*)) {
  link_t to_delete = l;
  l = l->next;
  if (delete_data) {
    delete(to_delete->data);
  }
  free(to_delete);
  return l;
}

list_t list_del_first(list_t l) {
  l->data = link_del_first(l->data, l->delete_data);
  l->size--;
  return l;
}

size_t list_length(list_t l) {
  return l->size;
}

void list_fprintf(list_t l, FILE* fp) {
  if (list_is_empty(l)) {
    fprintf(fp, "[]");
  } else {
    fprintf(fp, "[");
    link_t current = l->data;
    while (!link_is_empty(current)) {
      l->fprint_data(current->data);
      current = current->next;
      if (!link_is_empty(current)) {
        fprintf(fp, ", ");
      }
    }
    fprintf(fp, "]");
  }
}

void list_printf(list_t l) {
  list_fprintf(l, stdout);
}

list_t list_delete(list_t l) {
  while (!list_is_empty(l)) {
    l = list_del_first(l);
  }
  free(l);
  return list_new(NULL, NULL, NULL);
}

list_t list_add_last(void* e, list_t l) {
  l->data = link_add_last(e, l->data);
  l->size++;
  return l;
}

link_t link_add_last(void* e, link_t l) {
  if (link_is_empty(l)) {
    return link_add_first(e, l);
  } else {
    l->next = link_add_last(e, l->next);
    return l;
  }
}

list_t list_del_last(list_t l) {
  l->data = link_del_last(l->data, l->delete_data);
  l->size--;
  return l;
}

link_t link_del_last(link_t l, void* (*delete_data)(void*)) {
  if (link_is_empty(l)) {
    return NULL;
  } else if (link_is_empty(l->next)) {
    if (delete_data) {
      delete(l->data);
   
    }

  }
}

// Recherche un element selon le critere compare. Retourne le maillon trouve ou NULL
link_t list_lookup(void* elem, list_t l, int (*compare)(void* e1, void* e2)) {
  link_t current = l->data;
  while (!link_is_empty(current)) {
    if (compare(elem, current->data) == 0) {
      return current;
    }
    current = current->next;
  }
  return NULL;
}

// Recherche le premier maillon contenant e,
// au sens du equal_data ou de l'egalite de pointeurs si equal_data est NULL
// ou NULL si e n'est pas dans la liste
link_t list_find(void* e, list_t l) {
  link_t current = l->data;
  while (!link_is_empty(current)) {
    if (l->equal_data(current->data, e)) {
      return current;
    }
    current = current->next;
  }
  return NULL;
}

// Suppression de l'element param, au sens de equal_data
list_t list_remove(void* param, list_t l) {
  link_t previous = NULL;
  link_t current = l->data;

  while (!link_is_empty(current)) {
    if (l->equal_data(current->data, param)) {
      // Match found
      if (previous == NULL) {
        // First link matched
        l->data = current->next;
      } else {
        previous->next = current->next;
      }
      l->size--;
      l->delete_data(current->data);
      free(current);
      return l;
    }

    previous = current;
    current = current->next;
  }

  return l;
}

// Les visiteurs
void list_simple_visit(list_t l, void (*visit)(void*,void*), void* param) {
  link_t current = l->data;
  while (!link_is_empty(current)) {
    visit(current->data, param);
    current = current->next;
  }
}

void list_visit(list_t l, int (*exec_on_value)(void*,void*), int (*exec_on_link)(list_t,void*),void* param) {
  link_t current = l->data;
  while (!link_is_empty(current)) {
    if (exec_on_value != NULL) {
      exec_on_value(current->data, param);
    }
    if (exec_on_link != NULL) {
      exec_on_link(l, param);
    }
    current = current->next;
  }
}
