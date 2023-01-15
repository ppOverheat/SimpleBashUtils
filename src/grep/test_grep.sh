#!/bin/bash

SUCCESS=0
FAIL=0
DIFF=""
RED='\033[1;31m'
GR='\033[1;32m'
PUR='\033[1;35m'
NC='\033[0m'

declare -a tests=(
"s test_0_grep.txt VAR"
"for grep.c grep.h Makefile VAR"
"for grep.c VAR"
"-e for -e ^int grep.c grep.h Makefile VAR"
"-e for -e ^int grep.c VAR"
"-e regex -e ^print grep.c VAR -f test_ptrn_grep.txt"
"-e while -e void grep.c Makefile VAR -f test_ptrn_grep.txt"
)

declare -a special=(
"-n for test_1_grep.txt test_2_grep.txt"
"-n for test_1_grep.txt"
"-n -e ^\} test_1_grep.txt"
"-ce ^int test_1_grep.txt test_2_grep.txt"
"-e ^int test_1_grep.txt"
"-ni -vh = test_1_grep.txt test_2_grep.txt"
"-e"
"-ie INT test_5_grep.txt"
"-echar test_1_grep.txt test_2_grep.txt"
"-ne = -e out test_5_grep.txt"
"-iv int test_5_grep.txt"
"-in int test_5_grep.txt"
"-c -l aboba test_1.txt test_5_grep.txt"
"-v test_1_grep.txt -e ank"
"-l for test_1_grep.txt test_2_grep.txt"
"-o -e int test_4_grep.txt"
"-e = -e out test_5_grep.txt"
"-e ing -e as -e the -e not -e is test_6_grep.txt"
"-l for no_file.txt test_2_grep.txt"
"-f test_3_grep.txt test_5_grep.txt"
)

test_grep() {
    test=`echo $@ | sed "s/VAR/$var/"`
    ./s21_grep $test > test_21_grep.log
    grep $test > test_grep.log
    DIFF="$(diff -s test_21_grep.log test_grep.log)"
    if [ "$DIFF" == "Files test_21_grep.log and test_grep.log are identical" ]
    then
        ((SUCCESS++))
        echo -e "${GR}SUCCESS${NC}: grep $test"
    else
        ((FAIL++))
        echo -e "${RED}FAIL${NC}: grep $test"
    fi
    rm test_21_grep.log test_grep.log
}

test_leak() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; 
    then
        test=`echo $@ | sed "s/VAR/$var/"`
        valgrind --leak-check=yes --log-file="test_21_grep.log" ./s21_grep $test > tmp.txt
        leak=$(grep -A100000 leaks test_21_grep.log)
        if [[ $leak == *"0 errors from 0 contexts"* ]]
        then
        ((SUCCESS++))
            echo -e "${GR}SUCCESS${NC}: grep $test"
        else
        ((FAIL++))
            echo -e "${RED}FAIL${NC}: grep $test"
            echo "$leak"
        fi
        rm test_21_grep.log tmp.txt
    elif [[ "$OSTYPE" == "darwin"* ]]; 
    then
        test=`echo $@ | sed "s/VAR/$var/"`
        leaks -quiet -atExit -- ./s21_grep $test > test_21_grep.log
        leak=$(grep -A100000 leaks test_21_grep.log)
        if [[ $leak == *"0 leaks for 0 total leaked bytes"* ]]
        then
        ((SUCCESS++))
            echo -e "${GR}SUCCESS${NC}: grep $t"
        else
        ((FAIL++))
            echo -e "${RED}FAIL${NC}: grep $t"
            echo "$leak"
        fi
        rm test_21_grep.log
    fi
}

for i in "${special[@]}"
do
    var="-"
    test_grep $i
done

for var1 in v c l n h o
do
    for i in "${tests[@]}"
    do
        var="-$var1"
        test_grep $i
    done
done

for var1 in v c l n h o
do
    for var2 in v c l n h o
    do
        if [ $var1 != $var2 ]
        then
            for i in "${tests[@]}"
            do
                var="-$var1 -$var2"
                test_grep $i
            done
        fi
    done
done

for var1 in v c l n h o
do
    for var2 in v c l n h o
    do
        if [ $var1 != $var2 ]
        then
            for i in "${tests[@]}"
            do
                var="-$var1$var2"
                test_grep $i
            done
        fi
    done
done

echo -e "------------------------------------------------"

echo -e "${RED}FAIL${NC}: $FAIL"
echo -e "${GR}SUCCESS${NC}: $SUCCESS"

((FAIL=0))
((SUCCESS=0))

echo -e "${PUR}-------------MEMORY TEST------------------------${NC}"

for var1 in v c l n h o
do
    for i in "${tests[@]}"
    do
        var="-$var1"
        test_leak $i
    done
done

echo -e "${RED}FAIL${NC}: $FAIL"
echo -e "${GR}SUCCESS${NC}: $SUCCESS"
