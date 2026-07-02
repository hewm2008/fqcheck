#!/bin/sh
#$ -S /bin/sh
#Version1.0	hewm@genomics.org.cn	2018-02-08
echo Start Time : 
date
../bin/fqcheck	-InFq1	B_1.fq.gz	-InFq2	B_2.fq.gz	-OutStat1	1	-OutStat2	2.fqcheck	
echo End Time : 
date
