#ifndef SRC_COMMON_LIST_H_
#define SRC_COMMON_LIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct list {
  char *value;
  int del;
  struct list *next;
} list;

struct list *add_uniq(struct list *elem, char *value, int dynamic);
void destroy(struct list *root);
struct list *add(struct list *elem, char *value);
int has_duplicate(list *elem, char *value);

#endif  // SRC_COMMON_LIST_H_
