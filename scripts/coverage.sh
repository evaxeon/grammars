#!/bin/bash
echo "This script generates coverage files."
g++ -I../headers ../src/iterated.cpp ../src/main.cpp --coverage -std=c++11 -o ../coverage/a.out


../coverage/a.out < ../tests/arrays.test
../coverage/a.out < ../tests/arraysstring.test
../coverage/a.out < ../tests/brackets.test
../coverage/a.out < ../tests/function.test
../coverage/a.out < ../tests/ints.test
../coverage/a.out < ../tests/manybrackets.test

mv ../scripts/main.gcno ../coverage/main.gcno
mv ../scripts/main.gcda ../coverage/main.gcda
mv ../scripts/iterated.gcno ../coverage/iterated.gcno
mv ../scripts/iterated.gcda ../coverage/iterated.gcda

../lcov/bin/lcov --capture --directory ../coverage --rc lcov_branch_coverage=1 --output-file ../html/coverage.info
../lcov/bin/genhtml ../html/coverage.info --branch-coverage --output-directory ../html
