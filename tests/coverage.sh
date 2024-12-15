#!/bin/sh

# ls | grep .gcda | sed -s "s/.gcda//g" | sed -s "s/^/gcov\ /"

cd ..

# All of the coverages get stored in a results file
gcov cachis-cachis > tests/results/coverage
gcov cachis-confparser >> tests/results/coverage
gcov cachis-datainterface >> tests/results/coverage
gcov cachis-datamanipulation >> tests/results/coverage
gcov cachis-datastore >> tests/results/coverage
gcov cachis-dictionary >> tests/results/coverage
#gcov cachis-gui >> tests/results/coverage
gcov cachis-iniparser >> tests/results/coverage
gcov cachis-mappingpolicy >> tests/results/coverage
gcov cachis-replacementpolicy >> tests/results/coverage
gcov cachis-simulator >> tests/results/coverage
gcov cachis-statistics >> tests/results/coverage
gcov cachis-traceparser >> tests/results/coverage
gcov cachis-writepolicy >> tests/results/coverage

echo -e "------------- Full coverage results -------------\n"

# The results get parsed and printed
RESULTS=$(grep -A 1 -e "File 'src.*" tests/results/coverage)
echo "$RESULTS"

# The numbers get extracted from gcov's output
NUMBERS=$(echo "$RESULTS" | grep -e "Lines ex" | sed -e "s/Lines executed://" | sed -e "s/\..*%.*//")

SUM=0
TOTAL_LINES=0
for file in *.gcov; do
	# The lines that have been processed get counted
	SUM=$((SUM + $(grep -v "#####" $file | wc -l)))

	# The total lines get counted
	TOTAL_LINES=$((TOTAL_LINES + $(wc -l $file | sed -e "s/ .*//")))
done

echo -e "\nCovered lines: $SUM"
echo "Total lines: $TOTAL_LINES"
echo -e "\nCurrent code coverage is at $(( (SUM * 100) / (TOTAL_LINES) ))%"
