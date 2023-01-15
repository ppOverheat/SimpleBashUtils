#include "list.h"

int has_duplicate(list *elem, char *value) {
  int res = 0;
  list *tmp = elem;
  while (tmp != NULL) {
    if (strcmp(tmp->value, value) == 0) {
      res = 1;
    }
    tmp = tmp->next;
  }
  return res;
}

struct list *add_uniq(struct list *elem, char *value, int dynamic) {
  list *tmp = (list *)malloc(sizeof(list));
  tmp->value = value;
  tmp->del = dynamic;
  if (elem == NULL) {
    tmp->next = NULL;
    elem = tmp;
  } else {
    if (!has_duplicate(elem, value)) {
      list *p = elem;
      while (p->next != NULL) {
        p = p->next;
      }
      p->next = tmp;
      tmp->next = NULL;
    } else {
      if (tmp->del) free(tmp->value);
      free(tmp);
    }
  }
  return elem;
}

struct list *add(struct list *elem, char *value) {
  list *tmp = (list *)malloc(sizeof(list));
  tmp->value = value;
  tmp->del = 0;
  if (elem == NULL) {
    tmp->next = NULL;
    elem = tmp;
  } else {
    list *p = elem;
    while (p->next != NULL) {
      p = p->next;
    }
    p->next = tmp;
    tmp->next = NULL;
  }
  return elem;
}

void destroy(struct list *root) {
  while (root != NULL) {
    list *p = root;
    root = root->next;
    if (p->del) free(p->value);
    free(p);
  }
}
