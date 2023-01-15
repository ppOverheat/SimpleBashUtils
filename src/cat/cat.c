#include "cat.h"

int main(int argc, char *argv[]) {
  process(argc, argv);
  return 0;
}

void process(int argc, char *argv[]) {
  int err = -1;
  options *option = NULL;
  option = init(option);
  list *files = NULL;
  for (int i = 1; i < argc; i++) {
    char *opt = strdup(argv[i]);
    if (opt[0] == '-') {
      option = get_option(opt, option, &err);
    } else if (err == -1) {
      files = add(files, argv[i]);
    }
    if (opt) free(opt);
  }
  if (err != 1) {
    list *root = files;
    while (root != NULL) {
      cat(root->value, option);
      root = root->next;
    }
  }
  free(option);
  destroy(files);
}

options *get_option(char *opt, options *option, int *err) {
  switch (opt[1]) {
    case 'b':
      option->b = 1;
      break;
    case 'e':
      option->E = 1;
      option->v = 1;
      break;
    case 'E':
      option->E = 1;
      break;
    case 'n':
      option->n = 1;
      break;
    case 's':
      option->s = 1;
      break;
    case 't':
      option->T = 1;
      option->v = 1;
      break;
    case 'T':
      option->T = 1;
      break;
    case 'v':
      option->v = 1;
      break;
    case '-': {
      option = get_extra_opt(option, opt, err);
      break;
    }
    default:
      error_message(err, opt[1]);
  }
  return option;
}

options *get_extra_opt(options *option, char *opt, int *err) {
  if (strcmp(opt, "--number-nonblank") == 0)
    option->b = 1;
  else if (strcmp(opt, "--number") == 0)
    option->n = 1;
  else if (strcmp(opt, "--squeeze-blank") == 0)
    option->s = 1;
  else {
    error_message(err, opt[1]);
  }
  return option;
}

void error_message(int *err, char opt) {
  *err = 1;
  fprintf(stderr,
          "cat: illegal option -- %c\nusage: cat [-benstv] [file ...]\n", opt);
}

options *init(options *option) {
  option = (options *)malloc(sizeof(options));
  option->b = 0;
  option->E = 0;
  option->n = 0;
  option->s = 0;
  option->T = 0;
  option->v = 0;
  return option;
}

int print_non_printable(int *special, int ch) {
  *special = 1;
  if (ch > 127 && ch < 160) {
    printf("M-^%c", ch - 64);
  } else if (ch >= 0 && ch < 32 && ch != 9 && ch != 10) {
    printf("^%c", ch + 64);
  } else if (ch == 127) {
    printf("^?");
  } else
    *special = -1;
  return *special;
}

int print(options *option, int ch, int empty, int ignore, int sym, int special,
          int i) {
  if (!(option->s && empty > 1)) {
    if (((sym == 0 && ch != '\n' && option->b) ||
         (ignore != 1 && (sym == -1 || sym == 0) && option->n &&
          (!option->b)))) {
      printf("%6d\t", i);
      i++;
    }
    if (option->v) {
      special = print_non_printable(&special, ch);
    }
    if (ch == 10 && option->E) {
      printf("$\n");
    } else if (ch == '\t' && option->T) {
      printf("^I");
    } else if (special == -1) {
      printf("%c", ch);
    }
  }
  return i;
}

void cat(char *file, options *option) {
  FILE *f = fopen(file, "rb");
  if (f == NULL) {
    fprintf(stderr, "cat: %s: No such file or directory\n", file);
  } else {
    int sym = -1, i = 1, special = -1, ch = 0, empty = 0;
    while (ch != EOF) {
      ch = fgetc(f);
      if (ch != EOF) {
        if (ch == 10 && sym == -1)
          empty++;
        else
          empty = 0;
        int ignore = (ch == 10 && sym > -1) ? 1 : -1;
        if (ch == 10)
          sym = -1;
        else
          sym++;
        i = print(option, ch, empty, ignore, sym, special, i);
      }
    }
    fclose(f);
  }
}
