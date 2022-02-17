# fqcheck
<b>fqcheck: A new simple and efficient software to stat base and quality distribution of fq file</b>

###  1) Install
------------
The <b>new version</b> will be updated and maintained in <b>[hewm2008/fqcheck](https://github.com/hewm2008/fqcheck)</b>, please click below Link to download the latest version
</br><p align="center"><b>[hewm2008/fqcheck](https://github.com/hewm2008/fqcheck)</b></p>
<b> [Download](https://github.com/hewm2008/fqcheck/archive/v2.07.tar.gz) </b>

For <b>linux/Unix </b> static
</br>you can use the statically compiled programs <i>directly</i>
<pre>
         chmod 755 ./bin/fqcheck
        ./bin/fqcheck </pre>

  </br> Just [make]  or [sh  make.sh ]  to compile this software.the final software can be found in the Dir <b>[bin/fqcheck]</b>
  </br> For <b>linux /Unix </b> and <b>macOS</b>
  <pre>
        tar -zxvf  fqcheckXXX.tar.gz             # if Link do not work ,Try <b>re-install</b> [zlib]library
        cd fqcheckXXX;                           # [zlib] and copy them to the library Dir
        make ; make clean                        # fqcheck-xx/src/include/zlib
        ./bin/fqcheck  </pre>
  
**Note:** If fail to link,try to <b>re-install</b> the libraries [**_zlib_**](https://zlib.net/)

###  2) Example of fqcheck
------------
* 1) Parameter description:
```php


Usage:fqcheck  -InFq1 <in.fq>  -OutStat1 <out.fqcheck>  [options]

		-InFq1        <str>   File name of InFq Input
		-OutStat1     <str>   Prefix of OUT File name

		-InFq2        <str>   File name of InFq2 Input
		-OutStat2     <str>   Prefix of OUT File2 name

		-Adapter1     <str>   Input adapters fa file
		-Adapter2     <str>   Input adapters2 fa file

		-help                 show this help[hewm2008 v2.07]

```

* 2) To Stat Fq file see the Dir [example]

```
# 2.1) Paire-End fq file
      ../bin/fqcheck	-InFq1	B_1.fq.gz	-InFq2	B_2.fq.gz	-OutStat1	1	-OutStat2	2.fqcheck

# 2.2) Singel-End  fq file
      ../bin/fqcheck	-r	B_1.fq.gz	-c	1 

# 2.3) Fq file withe adapters files
      ../bin/fqcheck	-InFq1	B_1.fq.gz	-OutStat1	1.fqcheck	-InFq2	B_2.fq.gz	-OutStat2	2	-Adapter1	iPE-3.fa	-Adapter2	iPE-5.fa
```


###  3) Results
------------
some stat file will ouput.  
Also if the system has <b> gnuplot</b> and <b> convert</b> commands installed, the following pictures will be output

![base.png](https://github.com/hewm2008/fqcheck/blob/main/Example/base.png)

![qual.png](https://github.com/hewm2008/fqcheck/blob/main/Example/qual.png)

###  4) Discussing
------------
- [:email:](https://github.com/hewm2008/fqcheck) hewm2008@gmail.com / hewm2008@qq.com
- join the<b><i> QQ Group : 125293663</b></i>


######################swimming in the sky and flying in the sea ########################### ##
