FLAGS=-Wall -Werror -Wextra
SOURCES=grep.c ../common/list.c

all: s21_grep test

s21_grep:
	gcc $(FLAGS) $(SOURCES) -o s21_grep

test: 
	bash test_grep.sh

rebuild: clean s21_grep

clean:
	rm s21_grep
