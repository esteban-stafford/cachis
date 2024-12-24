#!/bin/sh
cd traces

../../cachis L1_FIFO.ini -g 2>/dev/null > ../results/L1_FIFO
../../cachis L1_LRU.ini -g 2>/dev/null > ../results/L1_LRU
../../cachis L1_LFU.ini -g 2>/dev/null > ../results/L1_LFU
../../cachis L1_RAND.ini -g 2>/dev/null > ../results/L1_RAND

../../cachis L1_WB.ini -g 2>/dev/null > ../results/L1_WB
../../cachis L1_WT.ini -g 2>/dev/null > ../results/L1_WT

../../cachis L1_AssocDirect.ini -g 2>/dev/null > ../results/L1_AssocDirect
../../cachis L1_AssocFull.ini -g 2>/dev/null > ../results/L1_AssocFull
../../cachis L1_AssocSet2.ini -g 2>/dev/null > ../results/L1_AssocSet2
../../cachis L1_AssocSet4.ini -g 2>/dev/null > ../results/L1_AssocSet4

../../cachis L12_Complex.ini -g 2>/dev/null > ../results/L12_Complex
