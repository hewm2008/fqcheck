#!/bin/sh
#$ -S /bin/sh
#Version1.0	hewm@genomics.org.cn	2018-02-08
echo Start Time : 
date
../bin/fqcheck	-r	B_1.fq.gz	-c	1	
echo End Time : 
date
