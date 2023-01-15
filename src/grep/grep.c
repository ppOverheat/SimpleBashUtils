#include "grep.h"

int main(int argc, char *argv[]) {
  process(argc, argv);
  return 0;
}

int check_e(int argc, char *argv[]) {
  int res = -1;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-' && argv[i][1] == 'e') res = 1;
  }
  return res;
}

void process(int argc, char *argv[]) {
  int err = -1, has_e = -1;
  int show_filename = -1;
  options *option = NULL;
  option = init(option);
  list *files = NULL;
  list *templates = NULL;
  has_e = check_e(argc, argv);
  for (int i = 1; i < argc; i++) {
    char *opt = strdup(argv[i]);
    if (opt[0] == '-') {
      option->e = 0;
      option = get_option(opt, argv[i], 1, option, &templates, &err);
      if (opt[2] != '\0' && option->e != -1) {
        option = get_option(opt, argv[i], 2, option, &templates, &err);
      }
      if (option->e == -1) option->e = 0;
    } else if (err == -1) {
      if (option->f) {
        templates = get_templates_from_file(argv[i], templates, &err);
        option->f = 0;
      } else if ((templates == NULL && has_e != 1) || option->e) {
        templates = add_uniq(templates, argv[i], 0);
        option->e = 0;
      } else {
        files = add(files, argv[i]);
        show_filename++;
      }
    }
    if (opt) free(opt);
  }
  if (err != 1) read_files(files, templates, option, show_filename);
  free(option);
  destroy(files);
  destroy(templates);
}

void read_files(list *files, list *templates, options *option,
                int show_filename) {
  list *root = files;
  while (root != NULL) {
    grep(root->value, templates, option, show_filename);
    root = root->next;
  }
}

options *check_str_e(char *argv, list **templates, options *option,
                     size_t index) {
  option->e = 1;
  size_t size = strlen(argv);
  if (size > 1 + index) {
    size_t j = 0;
    for (j = 0; j < size; j++) {
      if (j < size - (1 + index))
        argv[j] = argv[j + 1 + index];
      else
        argv[j] = '\0';
    }
    *templates = add_uniq(*templates, argv, 0);
    option->e = -1;
  }
  return option;
}

options *check_str_f(char *opt, list **templates, options *option, int *err,
                     size_t index) {
  option->f = 1;
  size_t value = 1 + index;
  size_t size = strlen(opt);
  if (size > value) {
    size_t j = 0;
    for (j = 0; j < size; j++) {
      if (j < size - value)
        opt[j] = opt[j + value];
      else
        opt[j] = '\0';
    }
    *templates = get_templates_from_file(opt, *templates, err);
    option->f = 0;
  }
  return option;
}

options *get_option(char *opt, char *argv, size_t i, options *option,
                    list **templates, int *err) {
  switch (opt[i]) {
    case 'e': {
      option = check_str_e(argv, templates, option, i);
      break;
    }
    case 'i':
      option->i = 1;
      break;
    case 'v':
      option->v = 1;
      break;
    case 'c':
      option->c = 1;
      break;
    case 'l':
      option->l = 1;
      break;
    case 'n':
      option->n = 1;
      break;
    case 'h':
      option->h = 1;
      break;
    case 's':
      option->s = 1;
      break;
    case 'f': {
      option = check_str_f(argv, templates, option, err, i);
      break;
    }
    case 'o':
      option->o = 1;
      break;
    default:
      if (i == 1) error_message(err, opt[1]);
      break;
  }
  return option;
}

void error_message(int *err, char opt) {
  *err = 1;
  fprintf(stderr,
          "grep: illegal option -- %c\nusage: grep [-ivclnhso] [-e pattern] "
          "[-f file] [pattern] [file ...]\n",
          opt);
}

options *init(options *option) {
  option = (options *)malloc(sizeof(options));
  option->e = 0;
  option->i = 0;
  option->v = 0;
  option->c = 0;
  option->l = 0;
  option->n = 0;
  option->h = 0;
  option->s = 0;
  option->f = 0;
  option->o = 0;
  return option;
}

list *get_templates_from_file(char *file, list *templates, int *err) {
  FILE *f = fopen(file, "rb");
  if (f == NULL) {
    fprintf(stderr, "grep: %s: No such file or directory\n", file);
    *err = 1;
  } else {
    size_t len = 0;
    char *line = NULL;
    ssize_t read = getline(&line, &len, f);
    while (read != -1) {
      char *buf = strdup(line);
      size_t size = strlen(line);
      for (size_t i = 0; i < size; i++) {
        if (line[i] == '\n') buf[i] = '\0';
      }
      templates = add_uniq(templates, buf, 1);
      read = getline(&line, &len, f);
    }
    if (line) free(line);
    fclose(f);
  }
  return templates;
}

void find_template(char *template, char *line, int *match, int cflags) {
  const size_t nmatch = 1;
  regmatch_t pmatch[1];
  regex_t reg;
  regcomp(&reg, template, cflags);
  int status = regexec(&reg, line, nmatch, pmatch, 0);
  if (status == 0) {
    *match = 1;
  }
  regfree(&reg);
}

void read_lines(FILE *f, char *file, list *templates, options *option,
                int show_filename) {
  int cflags = REG_NOSUB;
  if (option->i) cflags = REG_ICASE;
  if (option->o) cflags = REG_NEWLINE;
  size_t len = 0;
  char *line = NULL;
  int n_str = 0, count = 0, found = 0;
  ssize_t read = getline(&line, &len, f);
  while (read != -1) {
    n_str++;
    list *template = templates;
    int match = -1;
    while (template != NULL) {
      find_template(template->value, line, &match, cflags);
      template = template->next;
    }
    if (match == 1)
      print_o(option, show_filename, file, n_str, templates, line, cflags);
    if ((match == 1 && !option->v) || (match != 1 && option->v)) {
      found = 1;
      count++;
      print_other(option, show_filename, n_str, line, file);
    }
    read = getline(&line, &len, f);
  }
  print_c_l(option, found, show_filename, file, count);
  if (line) free(line);
}

void print_other(options *option, int show_filename, int n_str, char *line,
                 char *file) {
  if (!option->o) {
    if (!option->l && !option->c) {
      if (show_filename > 0 && !option->h) printf("%s:", file);
      if (option->n) printf("%d:", n_str);
      size_t size = strlen(line);
      for (size_t i = 0; i < size; i++) {
        if (line[i] != '\n') printf("%c", line[i]);
      }
      printf("\n");
    }
  }
}

void print_o(options *option, int show_filename, char *file, int n_str,
             list *templates, char *line, int cflag) {
  if (option->o && !option->v && !option->l && !option->c) {
    list *template = templates;
    while (template != NULL) {
      char *s = line;
      regex_t regex = {};
      regmatch_t pmatch[1] = {};
      regcomp(&regex, template->value, cflag);
      for (int i = 0;; i++) {
        if (regexec(&regex, s, ARRAY_SIZE(pmatch), pmatch, 0)) break;
        if (show_filename > 0 && !option->h) printf("%s:", file);
        if (option->n) printf("%d:", n_str);
        regoff_t len = pmatch[0].rm_eo - pmatch[0].rm_so;
        printf("%.*s\n", (int)len, s + pmatch[0].rm_so);
        s += pmatch[0].rm_eo;
      }
      regfree(&regex);
      template = template->next;
    }
  }
}

void print_c_l(options *option, int found, int show_filename, char *file,
               int count) {
  if (option->c && !option->l) {
    if (show_filename > 0 && !option->h) printf("%s:", file);
    printf("%d\n", count);
  } else if (option->l && found == 1) {
    printf("%s\n", file);
  }
}

void grep(char *file, list *templates, options *option, int show_filename) {
  FILE *f = fopen(file, "rb");
  if (f == NULL) {
    if (!option->s) {
      fprintf(stderr, "grep: %s: No such file or directory\n", file);
    }
  } else {
    read_lines(f, file, templates, option, show_filename);
    fclose(f);
  }
}
