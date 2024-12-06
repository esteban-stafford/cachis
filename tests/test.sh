#!/bin/sh
allPassed=1

# Function check and print errors
check_output() {
  if [ -n "$1" ]; then
    echo "Error, expected:"
    echo "$1" | grep "^>" | sed -s "s/>//g"
    echo "But got:"
    echo "$1" | grep "^<" | sed -s "s/<//g"

    allPassed=0
  else
    echo "OK"
  fi

  echo -e "\n"
}


echo -e "------------- Replacement Policy Tests -------------\n"

echo -e "----> Testing LRU:"
../cachis L1_LRU.ini -g 2>/dev/null | grep -e Writing -e Cycle -e Hit\ in > ./results/L1_LRU.tmp
check_output "$(diff  ./results/L1_LRU ./results/L1_LRU.tmp)"

echo -e "----> Testing LFU:"
../cachis L1_LFU.ini -g 2>/dev/null | grep -e Writing -e Cycle -e Hit\ in > ./results/L1_LFU.tmp
check_output "$(diff  ./results/L1_LFU ./results/L1_LFU.tmp)"

echo -e "----> Testing FIFO:"
../cachis L1_FIFO.ini -g 2>/dev/null | grep -e Writing -e Cycle -e Hit\ in > ./results/L1_FIFO.tmp
check_output "$(diff  ./results/L1_FIFO ./results/L1_FIFO.tmp)"


echo -e "------------- Write Policy Tests -------------\n"

if [ $allPassed -eq 1 ]; then
    echo "All tests passed"
else
    echo "Some tests have failed"
fi
