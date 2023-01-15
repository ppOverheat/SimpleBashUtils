FLAGS=-Wall -Werror -Wextra
SOURCES=cat.c ../common/list.c

all: s21_cat test

s21_cat:
	gcc $(FLAGS) $(SOURCES) -o s21_cat

rebuild: clean s21_cat

test: 
	bash test_cat.sh

clean:
	rm s21_cat
