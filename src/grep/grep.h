#ifndef SRC_GREP_GREP_H_
#define SRC_GREP_GREP_H_

#include <regex.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/list.h"

#define ARRAY_SIZE(arr) (sizeof((arr)) / sizeof((arr)[0]))

typedef struct options {
  int e, i, v, c, l, n, h, s, f, o;
} options;

void find_template(char *template, char *line, int *match, int cflags);
int check_e(int argc, char *argv[]);
void print_vo(options *option, int show_filename, char *file, int n_str,
              list *templates, char *line, int cflag);
void print_other(options *option, int show_filename, int n_str, char *line,
                 char *file);
void print_o(options *option, int show_filename, char *file, int n_str,
             list *templates, char *line, int cflag);
void print_c_l(options *option, int found, int show_filename, char *file,
               int count);
void grep(char *file, list *templates, options *option, int show_filename);
list *get_templates_from_file(char *file, list *templates, int *err);
void process(int argc, char *argv[]);
options *init(options *option);
options *get_option(char *opt, char *argv, size_t i, options *option,
                    list **templates, int *err);
void read_files(list *files, list *templates, options *option,
                int show_filename);
void error_message(int *err, char opt);
options *check_str_e(char *argv, list **templates, options *option,
                     size_t index);
options *check_str_f(char *opt, list **templates, options *option, int *err,
                     size_t index);
void read_lines(FILE *f, char *file, list *templates, options *option,
                int show_filename);

#endif  // SRC_GREP_GREP_H_
