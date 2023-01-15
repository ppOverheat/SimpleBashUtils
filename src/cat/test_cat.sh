#!/bin/bash

SUCCESS=0
FAIL=0
DIFF=""
RED='\033[1;31m'
GR='\033[1;32m'
PUR='\033[1;35m'
NC='\033[0m'

declare -a tests=(
"VAR test_case_cat.txt"
"VAR no_file.txt"
)

declare -a special=(
"-s test_1_cat.txt"
"-b -e -n -s -t -v test_1_cat.txt"
"-t test_3_cat.txt"
"-n test_2_cat.txt"
"no_file.txt"
"-n -b test_1_cat.txt"
"-s -n -e test_4_cat.txt"
"test_1_cat.txt -n"
"-n test_1_cat.txt"
"-n test_1_cat.txt test_2_cat.txt"
"-v test_5_cat.txt"
)

test_cat() {
    test=`echo $@ | sed "s/VAR/$var/"`
    ./s21_cat $test > test_21_cat.log
    cat $test > test_cat.log
    DIFF="$(diff -s test_21_cat.log test_cat.log)"
    if [ "$DIFF" == "Files test_21_cat.log and test_cat.log are identical" ]
    then
        ((SUCCESS++))
        echo -e "${GR}SUCCESS${NC}: cat $test"
    else
        ((FAIL++))
        echo -e "${RED}FAIL${NC}: cat $test"
    fi
    rm test_21_cat.log test_cat.log
}

test_leak() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; 
    then
        test=`echo $@ | sed "s/VAR/$var/"`
        valgrind --leak-check=yes --log-file="test_21_cat.log" ./s21_cat $test > tmp.txt
        leak=$(grep -A100000 leaks test_21_cat.log)
        if [[ $leak == *"0 errors from 0 contexts"* ]]
        then
        ((SUCCESS++))
            echo -e "${GR}SUCCESS${NC}: cat $test"
        else
        ((FAIL++))
            echo -e "${RED}FAIL${NC}: cat $test"
            echo "$leak"
        fi
        rm test_21_cat.log tmp.txt
    elif [[ "$OSTYPE" == "darwin"* ]]; 
    then
        test=`echo $@ | sed "s/VAR/$var/"`
        leaks -quiet -atExit -- ./s21_cat $test > test_21_cat.log
        leak=$(grep -A100000 leaks test_21_cat.log)
        if [[ $leak == *"0 leaks for 0 total leaked bytes"* ]]
        then
        ((SUCCESS++))
            echo -e "${GR}SUCCESS${NC}: cat $test"
        else
        ((FAIL++))
            echo -e "${RED}FAIL${NC}: cat $test"
            echo "$leak"
        fi
        rm test_21_cat.log
    fi
}

for i in "${special[@]}"
do
    var="-"
    test_cat $i
done

for var1 in b e n s t v
do
    for i in "${tests[@]}"
    do
        var="-$var1"
        test_cat $i
    done
done

for var1 in b e n s t v
do
    for var2 in b e n s t v
    do
        if [ $var1 != $var2 ]
        then
            for i in "${tests[@]}"
            do
                var="-$var1 -$var2"
                test_cat $i
            done
        fi
    done
done

for var1 in b e n s t v
do
    for var2 in b e n s t v
    do
        for var3 in b e n s t v
        do
            if [ $var1 != $var2 ] && [ $var2 != $var3 ] && [ $var1 != $var3 ]
            then
                for i in "${tests[@]}"
                do
                    var="-$var1 -$var2 -$var3"
                    test_cat $i
                done
            fi
        done
    done
done

for var1 in b e n s t v
do
    for var2 in b e n s t v
    do
        for var3 in b e n s t v
        do
            for var4 in b e n s t v
            do
                if [ $var1 != $var2 ] && [ $var2 != $var3 ] \
                && [ $var1 != $var3 ] && [ $var1 != $var4 ] \
                && [ $var2 != $var4 ] && [ $var3 != $var4 ]
                then
                    for i in "${tests[@]}"
                    do
                        var="-$var1 -$var2 -$var3 -$var4"
                        test_cat $i
                    done
                fi
            done
        done
    done
done

echo -e "------------------------------------------------"

echo -e "${RED}FAIL${NC}: $FAIL"
echo -e "${GR}SUCCESS${NC}: $SUCCESS"

((FAIL=0))
((SUCCESS=0))

echo -e "${PUR}-------------MEMORY TEST------------------------${NC}"

for i in "${special[@]}"
do
    var="-"
    test_leak $i
done

for var1 in b e n s t v
do
    for i in "${tests[@]}"
    do
        var="-$var1"
        test_leak $i
    done
done

echo -e "${RED}FAIL${NC}: $FAIL"
echo -e "${GR}SUCCESS${NC}: $SUCCESS"
