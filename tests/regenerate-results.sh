#!/bin/sh
cd traces

../../cachis -i L1_FIFO.ini -g 2>/dev/null > ../results/L1_FIFO
../../cachis -i L1_LRU.ini -g 2>/dev/null > ../results/L1_LRU
../../cachis -i L1_LFU.ini -g 2>/dev/null > ../results/L1_LFU
../../cachis -i L1_RAND.ini -g 2>/dev/null > ../results/L1_RAND

../../cachis -i L1_WB.ini -g 2>/dev/null > ../results/L1_WB
../../cachis -i L1_WT.ini -g 2>/dev/null > ../results/L1_WT

../../cachis -i L1_AssocDirect.ini -g 2>/dev/null > ../results/L1_AssocDirect
../../cachis -i L1_AssocFull.ini -g 2>/dev/null > ../results/L1_AssocFull
../../cachis -i L1_AssocSet2.ini -g 2>/dev/null > ../results/L1_AssocSet2
../../cachis -i L1_AssocSet4.ini -g 2>/dev/null > ../results/L1_AssocSet4

../../cachis -i L12_Complex.ini -g 2>/dev/null > ../results/L12_Complex
../../cachis -i L12_WriteBack.ini -g 2>/dev/null > ../results/L12_WriteBack
