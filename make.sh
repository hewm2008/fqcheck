#!/bin/sh
#$ -S /bin/sh
#Version1.0	P_bc_rd@genomics.org.cn	2018-02-8
echo Start Time : 
date
echo g++	-O3	-g	src/fqcheck.cpp  	-L ./lib	-lz	-o	bin/fqcheck -static	
g++	-O3	-g	src/fqcheck.cpp	   -L	/zfssz5/BC_PUB/Software/01.Usr/lib/  -I ./include/ -L ./lib	-lz	-o	bin/fqcheck	 -static 
echo End Time : 
date
