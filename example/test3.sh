#!/bin/sh
#$ -S /bin/sh
#Version1.0	hewm@genomics.org.cn	2018-02-08
echo Start Time : 
date
../bin/fqcheck	-InFq1	B_1.fq.gz	-OutStat1	1.fqcheck	-InFq2	B_2.fq.gz	-OutStat2	2	-Adapter1	iPE-3.fa	-Adapter2	iPE-5.fa	
echo End Time : 
date
