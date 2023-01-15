#ifndef SRC_CAT_CAT_H_
#define SRC_CAT_CAT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/list.h"

typedef struct options {
  int b, E, n, s, T, v;
} options;

options *get_extra_opt(options *option, char *opt, int *err);
int print(options *option, int ch, int empty, int ignore, int sym, int special,
          int i);
int print_non_printable(int *special, int ch);
void error_message(int *err, char opt);
void cat(char *file, options *option);
options *init(options *option);
void process(int argc, char *argv[]);
options *get_option(char *opt, options *option, int *err);

#endif  // SRC_CAT_CAT_H_
