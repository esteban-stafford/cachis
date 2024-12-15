#!/bin/sh
cd traces

allPassed=1

# Function check and print errors
check_output() {
  if [ -n "$1" ]; then
    echo "Error, expected:"
    echo "$1" | grep "^<" | sed -s "s/<//g"
    echo "But got:"
    echo "$1" | grep "^>" | sed -s "s/>//g"

    allPassed=0
  else
    echo "OK"
  fi

  echo -e "\n"
}


echo -e "------------- Replacement Policy Tests -------------\n"

echo -e "----> Testing LRU:"
../../cachis L1_LRU.ini -g 2>/dev/null > ../results/L1_LRU.tmp
check_output "$(diff  ../results/L1_LRU ../results/L1_LRU.tmp)"

echo -e "----> Testing LFU:"
../../cachis L1_LFU.ini -g 2>/dev/null > ../results/L1_LFU.tmp
check_output "$(diff  ../results/L1_LFU ../results/L1_LFU.tmp)"

echo -e "----> Testing FIFO:"
../../cachis L1_FIFO.ini -g 2>/dev/null > ../results/L1_FIFO.tmp
check_output "$(diff  ../results/L1_FIFO ../results/L1_FIFO.tmp)"


echo -e "------------- Write Policy Tests -------------\n"

echo -e "----> Testing WB:"
../../cachis L1_WB.ini -g 2>/dev/null > ../results/L1_WB.tmp
check_output "$(diff  ../results/L1_WB ../results/L1_WB.tmp)"

echo -e "----> Testing WT:"
../../cachis L1_WT.ini -g 2>/dev/null > ../results/L1_WT.tmp
check_output "$(diff  ../results/L1_WT ../results/L1_WT.tmp)"

echo -e "------------- Associativity Tests -------------\n"

echo -e "----> Testing Direct Associativity:"
../../cachis L1_AssocDirect.ini -g 2>/dev/null > ../results/L1_AssocDirect.tmp
check_output "$(diff  ../results/L1_AssocDirect ../results/L1_AssocDirect.tmp)"

echo -e "----> Testing Full Associativity:"
../../cachis L1_AssocFull.ini -g 2>/dev/null > ../results/L1_AssocFull.tmp
check_output "$(diff  ../results/L1_AssocFull ../results/L1_AssocFull.tmp)"

echo -e "----> Testing 2-Way Set Associativity:"
../../cachis L1_AssocSet2.ini -g 2>/dev/null > ../results/L1_AssocSet2.tmp
check_output "$(diff  ../results/L1_AssocSet2 ../results/L1_AssocSet2.tmp)"

echo -e "----> Testing 4-Way Set Associativity:"
../../cachis L1_AssocSet4.ini -g 2>/dev/null > ../results/L1_AssocSet4.tmp
check_output "$(diff  ../results/L1_AssocSet4 ../results/L1_AssocSet4.tmp)"


echo -e "------------- Results -------------\n"
if [ $allPassed -eq 1 ]; then
    echo "All tests passed"
else
    echo "Some tests have failed"
fi
