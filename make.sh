#!/bin/sh
#$ -S /bin/sh
#Version1.0	P_bc_rd@genomics.org.cn	2018-02-8
echo Start Time : 
date
echo g++ -std=c++17 -O3 -g src/fqcheck.cpp -I ./src/rapidgzip/src -lz -o bin/fqcheck -static -fopenmp -march=native ./src/rapidgzip/src/libglue_rapidgzip.a
g++ -std=c++17 -O3 -g src/fqcheck.cpp -I ./src/rapidgzip/src -lz -o bin/fqcheck -static -fopenmp -march=native ./src/rapidgzip/src/libglue_rapidgzip.a
echo End Time : 
date
