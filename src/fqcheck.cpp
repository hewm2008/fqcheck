#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <vector>
#include <iomanip>
#include <math.h>
#include <omp.h>
#include <cstdlib>
#include <unistd.h>
#include "gzstream/gzstream.c"
#include "libglue_rapidgzip.hpp"

#define MAX_L 512
#define MAX_Q 128
typedef  long long LLongA;
typedef  long long int LLongB;
using namespace std;

//  #define _A 0 #define _C 1    #define _G 2   #define _T 3   #define _N 4

static const int BR_BUF_SIZE = 524288;

struct BatchReader {
	FILE *fp;
	char *buf;
	int pos, end;

	BatchReader(FILE *f) : fp(f), pos(0), end(0) { buf = new char[BR_BUF_SIZE]; }
	~BatchReader() { delete[] buf; }

	bool get_line(char *out, int max_len) {
		for (;;) {
			if (pos >= end) {
				end = fread(buf, 1, BR_BUF_SIZE, fp);
				if (end <= 0) return false;
				pos = 0;
			}
			char *nl = (char*)memchr(buf + pos, '\n', end - pos);
			if (nl) {
				int len = nl - (buf + pos) + 1;
				if (len > max_len - 1) len = max_len - 1;
				memcpy(out, buf + pos, len);
				out[len] = '\0';
				pos = nl - buf + 1;
				return true;
			}
			int rem = end - pos;
			if (rem >= max_len - 1) {
				memcpy(out, buf + pos, max_len - 1);
				out[max_len - 1] = '\0';
				pos = end;
				return true;
			}
			memmove(buf, buf + pos, rem);
			int more = fread(buf + rem, 1, BR_BUF_SIZE - rem, fp);
			if (more <= 0) {
				memcpy(out, buf, rem);
				out[rem] = '\0';
				pos = end = 0;
				return rem > 0;
			}
			end = rem + more;
			pos = 0;
		}
	}

	bool read_one(char *id, char *seq, char *qid, char *qual) {
		return get_line(id, MAX_L) && get_line(seq, MAX_L) &&
		       get_line(qid, MAX_L) && get_line(qual, MAX_L);
	}
};

int  print_usage_FqCheck()
{
	cout <<""
		"\n"
		"Usage:fqcheck  -InFq1 <in.fq>  -OutStat1 <out.fqcheck>  [options]\n"
		"\n"
		"\t\t-InFq1        <str>   File name of InFq Input\n"
		"\t\t-OutStat1     <str>   Prefix of OUT File name\n"
		"\n"
		"\t\t-InFq2        <str>   File name of InFq2 Input\n"
		"\t\t-OutStat2     <str>   Prefix of OUT File2 name\n"
		"\n"
		"\t\t-Adapter1     <str>   Input adapters fa file\n"
		"\t\t-Adapter2     <str>   Input adapters2 fa file\n"
		"\n"
		"\t\t-SelfFigO     <str>   Prefix of output figure files\n"
		"\n"
		"\t\t-help                 show this help[hewm2008 v2.09]\n"
		"\n";
	return 1;
}

string &  replace_all(string &  str,const  string &  old_Avalue,const string &  new_Avalue)
{
	while(true)   {
		string::size_type  pos(0);
		if(   (pos=str.find(old_Avalue))!=string::npos   )
			str.replace(pos,old_Avalue.length(),new_Avalue);
		else   break;
	}
	return   str;
}

string Int2Str (int A )
{
	stringstream   sstrm ;
	sstrm  <<  A ;
	return  sstrm.str();
}


void split(const string& str,vector<string>& tokens,  const string& delimiters = " ")
{
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	string::size_type pos     = str.find_first_of(delimiters, lastPos);
	while (string::npos != pos || string::npos != lastPos)
	{
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		lastPos = str.find_first_not_of(delimiters, pos);
		pos = str.find_first_of(delimiters, lastPos);
	}
}



class Para_A24 {
	public:
		string InFq1;
		string InFq2;
		string OutFq1;
		string OutFq2;

		string adapter1 ;
		string adapter2 ;

		int LowQint ;
		string FigPrefix ;

		Para_A24()
		{
			InFq1="";
			InFq2="";
			OutFq1="";
			OutFq2="";

			adapter1="NA" ;
			adapter2="NA" ;
			LowQint=64;
			FigPrefix="";

		}
}
;

inline void  LogLackArg( string  flag )
{
	cerr << "\t\tLack Argument for [ -"<<flag<<" ]"<<endl;
}


int parse_cmd_FqCheck(int argc, char **argv , Para_A24 * para ,   Para_A24 * P2In  )
{
	if (argc <=4  ) {print_usage_FqCheck();return 0;}

	for(int i = 1; i < argc; i++)
	{
		if(argv[i][0] != '-')
		{
			cerr << "command option error! please check." << endl;
			return 0;
		}
		string flag=argv[i] ;
		flag=replace_all(flag,"-","");

		if (flag  == "InFq1" || flag  == "r" )
		{
			if(i + 1 == argc) { LogLackArg( flag ) ; return 0;}
			i++;
			para->InFq1=argv[i];
		}
		else if (flag  ==  "Adapter1" )
		{
			if(i + 1 == argc) { LogLackArg( flag ) ; return 0;}
			i++;
			para->adapter1=argv[i];
		}
		else if (flag  ==  "OutStat1" || flag  == "c" )
		{
			if(i + 1 == argc) { LogLackArg( flag ) ; return 0;}
			i++;
			para->OutFq1=argv[i];
		}
		else if (flag  == "InFq2" )
		{
			if(i + 1 == argc) { LogLackArg( flag ) ; return 0;}
			i++;
			P2In->InFq1=argv[i];
		}
		else if (flag  ==  "Adapter2" )
		{
			if(i + 1 == argc) { LogLackArg( flag ) ; return 0;}
			i++;
			P2In->adapter1=argv[i];
		}
		else if (flag  ==  "OutStat2" )
		{
			if(i + 1 == argc) { LogLackArg( flag ) ; return 0;}
			i++;
			P2In->OutFq1=argv[i];
		}
		else if (flag  ==  "SelfFigO" )
		{
			if(i + 1 == argc) { LogLackArg( flag ) ; return 0;}
			i++;
			para->FigPrefix=argv[i];
		}
		else if (flag  == "help")
		{
			print_usage_FqCheck();return 0;
		}
		else
		{
			cerr << "UnKnow argument -"<<flag<<endl;
			return 0;
		}
	}

	if ( (para->OutFq1).empty() || (para->InFq1).empty() )
	{
		cerr<< "-InFq1 -OutStat1 lack argument for the must"<<endl;
		return 0;
	}

	if  ((P2In->OutFq1).empty() &&  (P2In->InFq1).empty())
	{
		return 1 ;
	}
	else if ( (P2In->OutFq1).empty() || (P2In->InFq1).empty() )
	{
		cerr<< "-InFq2 -OutStat2 must set togther"<<endl;
		return 0;
	}
	else
	{
		return  2 ;
	}
}



int RunSingleFq( string InFq, string OutStat,  Para_A24 * para  )
{

	LLongA base[5];//记录每种碱基总数
	LLongA qual[MAX_Q];//每个质量值的频数分布
	LLongA cycle_qual[MAX_L][MAX_Q];//每个cycle的碱基分布
	LLongA cycle_base[MAX_L][5];//每个cycle的碱基分布
	double   q_err[MAX_Q];//记录质量为q的碱基的错误率
	double   err = 0.;
	LLongA q20 = 0;
	LLongA q30 = 0;
	LLongA read_num = 0 ;

	int UC[256]={4};
	UC['A']=0; UC['C']=1;UC['G']=2;UC['T']=3 ; UC['N']=4;
	UC['a']=0; UC['c']=1;UC['g']=2;UC['t']=3 ; UC['n']=4;

	read_num = 0 ;
	q20 = 0;
	q30 = 0;
	err = 0.0;
	base[0]=0 ; base[1]=0 ; base[2]=0 ; base[3]=0 ; base[4]=0 ;

	memset(qual, 0, sizeof(qual));
	memset(q_err, 0, sizeof(q_err));
	memset(cycle_qual, 0, sizeof(cycle_qual));
	memset(cycle_base, 0, sizeof(cycle_base));

	//�A5;A?���п��ܵ�����ֵ��err
	for(int q=0;q<2;q++) {q_err[q]=1.0/(pow((double)10.0,(double)(q*0.1)));}
	for(int q=41;q<MAX_Q;q++) {q_err[q]=1.0/(pow((double)10.0,(double)(q*0.1)));}
	q_err[2]=0.121821900;
	q_err[3]=0.501187233;
	q_err[4]=0.398107170;
	q_err[5]=0.316227766;
	q_err[6]=0.251188643;
	q_err[7]=0.199526232;
	q_err[8]=0.158489319;
	q_err[9]=0.128647300;
	q_err[10]=0.10545720;
	q_err[11]=0.09508237;
	q_err[12]=0.07943282;
	q_err[13]=0.07539894;
	q_err[14]=0.03981071;
	q_err[15]=0.03162277;
	q_err[16]=0.02511886;
	q_err[17]=0.01995262;
	q_err[18]=0.01629471;
	q_err[19]=0.01584893;
	q_err[20]=0.01470216;
	q_err[21]=0.00910940;
	q_err[22]=0.00791782;
	q_err[23]=0.00601007;
	q_err[24]=0.00590479;
	q_err[25]=0.00559080;
	q_err[26]=0.00518726;
	q_err[27]=0.00437646;
	q_err[28]=0.00405721;
	q_err[29]=0.00393885;
	q_err[30]=0.00387256;
	q_err[31]=0.00341158;
	q_err[32]=0.00337891;
	q_err[33]=0.00296104;
	q_err[34]=0.00282249;
	q_err[35]=0.00261006;
	q_err[36]=0.00247926;
	q_err[37]=0.00227605;
	q_err[38]=0.00217025;
	q_err[39]=0.00215136;
	q_err[40]=0.00188753;

	//input file
	char read_id[MAX_L], read_se[MAX_L], qual_id[MAX_L], qual_se[MAX_L];
	int read_len;

	FILE *read_fs = glue_rapidgzip_fopen(InFq.c_str(), 0);
	if (read_fs==NULL)
	{
		cerr<<"Cann't open files\t"<<InFq<<endl;
		return 1;
	}

	fgets(read_id, MAX_L, read_fs);
	fgets(read_se, MAX_L, read_fs);
	read_len = strlen(read_se);
	if (read_len>0 && read_se[read_len-1]=='\n') read_len--;

	int detect_max=250;
	int detect_cnt=0;
	// Detect quality shift from first reads
	{
		int minQ=50000, maxQ=0;
		detect_cnt=1;
		fgets(qual_id, MAX_L, read_fs);
		fgets(qual_se, MAX_L, read_fs);
		for (int c=0;c<read_len;c++)
		{
			if(minQ>qual_se[c]) minQ=qual_se[c];
			if(maxQ<qual_se[c]) maxQ=qual_se[c];
		}
		while(detect_cnt<detect_max && fgets(read_id, MAX_L, read_fs))
		{
			fgets(read_se, MAX_L, read_fs);
			fgets(qual_id, MAX_L, read_fs);
			fgets(qual_se, MAX_L, read_fs);
			detect_cnt++;
			for(int c=0;c<read_len;c++)
			{
				if(minQ>qual_se[c]) minQ=qual_se[c];
				if(maxQ<qual_se[c]) maxQ=qual_se[c];
			}
		}
		if(minQ>=33 && minQ<=78 && maxQ>=33 && maxQ<=78) para->LowQint=33;
		else if(minQ>=64 && minQ<=108 && maxQ>=64 && maxQ<=108) para->LowQint=64;
		else para->LowQint=64;
	}

	fclose(read_fs);

	// Re-init arrays for main processing
	memset(base, 0, sizeof(base));
	read_num=0; q20=0; q30=0; err=0.0;
	memset(qual, 0, sizeof(qual));
	memset(cycle_qual, 0, sizeof(cycle_qual));
	memset(cycle_base, 0, sizeof(cycle_base));

	//output file
	ofstream  OUTFile;
	OUTFile.open (OutStat.c_str());
	if (!OUTFile.good())
	{
		cout<<"Cann't open files\t"<<OutStat<<endl;
		return 1 ;
	}

	read_fs = glue_rapidgzip_fopen(InFq.c_str(), 0);
	if (read_fs==NULL)
	{
		cerr<<"Cann't open files\t"<<InFq<<endl;
		return 1;
	}
	BatchReader br(read_fs);
	while (br.read_one(read_id, read_se, qual_id, qual_se)) 
	{
		++read_num;
		for (int c=0;c<read_len;c++) 
		{
			++cycle_base[c][UC[read_se[c]]];

			int q = qual_se[c]-(para->LowQint);
			q &= ~(q >> 31);
			++cycle_qual[c][q];
		}
	}
	fclose(read_fs);


	LLongB base_total = (LLongB) read_len * read_num;
	for(int c=0;c<read_len;c++)
	{
		base[0]+=cycle_base[c][0];
		base[1]+=cycle_base[c][1];
		base[2]+=cycle_base[c][2];
		base[3]+=cycle_base[c][3];
		base[4]+=cycle_base[c][4];

	}
	for(int c=0;c<read_len;c++)
	{
		for(int q=0;q<MAX_Q;q++)
		{
			qual[q]+=cycle_qual[c][q];
		}
	}
	int qMAX=0;
	for(int q=0;q<MAX_Q;q++)
	{
		if (qual[q])
		{
			qMAX=q;
			if (q>=30) q30+=qual[q];
			if (q>=20) q20+=qual[q];
			err+=q_err[q]*qual[q];
		}
	}

	OUTFile<<" the default quality shift value is: -"<<(para->LowQint)<<", "
		<<read_num<<" sequences, "
		<<base_total<<" total length, Max length:"
		<<read_len<<", average length:"
		<<setiosflags(ios::fixed)<<setprecision(2)<<(double)read_len<<endl;
	OUTFile<<"Standard deviations at 0.25:  total "<<100*(sqrt(0.25*(double)base_total)/base_total)
		<<"%, per base "<<100*(sqrt(0.25*(double)read_num)/read_num)
		<<"%"<<endl;
	OUTFile<<"             A     C     G     T     N ";

	for(int q=0;q<=qMAX;q++)
	{
		OUTFile<<setw(4)<<q<<' ';
	}

	OUTFile<<endl;
	OUTFile<<"Total    ";

	for(int b=0;b<=4;b++)  
	{
		OUTFile<<setw(5)<<setiosflags(ios::fixed)<<setprecision(1)<<100*(double)base[b]/base_total<<' ';
	}

	for(int q=0;q<=qMAX;q++) 
	{
		OUTFile<<setw(4)<<(int)(1000*((double)qual[q]/base_total))<<" ";
	}
	OUTFile<<endl;

	for(int c=0;c<read_len;c++)	
	{
		OUTFile<<"base "<<setw(3)<<c+1<<' ';
		for(int b=0;b<=4;b++)  {OUTFile<<setw(5)<<setiosflags(ios::fixed)<<setprecision(1)<<100*(double)cycle_base[c][b]/read_num<<' ';}
		for(int q=0;q<=qMAX;q++)
		{
			OUTFile<<setw(4)<<(int)(1000*((double)cycle_qual[c][q]/read_num))<<' ';
		}
		OUTFile<<endl;
	}

	OUTFile<<endl;
	OUTFile<<"Error Rate\t%GC\tQ20\tQ30"<<endl;
	OUTFile.precision(2);
	OUTFile<<(100.0*err)/(base_total)<<"\t"<<(100.0*(base[1]+base[2]))/(base_total-base[4])<<"\t"<<(100.0*q20)/base_total<<"\t"<<(100.0*q30)/base_total<<endl;
	OUTFile.close();


	return 0;
}



int RunSingleFqAdpter( string InFq, string OutStat,  Para_A24 * para  )
{

	LLongA base[5];//记录每种碱基总数
	LLongA qual[MAX_Q];//每个质量值的频数分布
	LLongA cycle_qual[MAX_L][MAX_Q];//每个cycle的质量分布
	LLongA cycle_base[MAX_L][5];//每个cycle的碱基分布
	double   q_err[MAX_Q];//记录质量为q的碱基的错误率
	double   err = 0.;
	LLongA q20 = 0;
	LLongA q30 = 0;
	LLongB read_num = 0;
	LLongB empt_num= 0;
	LLongB adpt_num =0;


	int UC[256]={4};
	UC['A']=0; UC['C']=1;UC['G']=2;UC['T']=3 ; UC['N']=4;
	UC['a']=0; UC['c']=1;UC['g']=2;UC['t']=3 ; UC['n']=4;

	read_num = 0;
	q20 = 0;
	q30 = 0;
	err = 0.0;
	base[0]=0 ; base[1]=0 ; base[2]=0 ; base[3]=0 ; base[4]=0 ;

	memset(qual, 0, sizeof(qual));
	memset(q_err, 0, sizeof(q_err));
	memset(cycle_qual, 0, sizeof(cycle_qual));
	memset(cycle_base, 0, sizeof(cycle_base));

	//�������п��ܵ�����ֵ��err
	for(int q=0;q<2;q++) {q_err[q]=1.0/(pow((double)10.0,(double)(q*0.1)));}
	for(int q=41;q<MAX_Q;q++) {q_err[q]=1.0/(pow((double)10.0,(double)(q*0.1)));}
	q_err[2]=0.121821900;
	q_err[3]=0.501187233;
	q_err[4]=0.398107170;
	q_err[5]=0.316227766;
	q_err[6]=0.251188643;
	q_err[7]=0.199526232;
	q_err[8]=0.158489319;
	q_err[9]=0.128647300;
	q_err[10]=0.10545720;
	q_err[11]=0.09508237;
	q_err[12]=0.07943282;
	q_err[13]=0.07539894;
	q_err[14]=0.03981071;
	q_err[15]=0.03162277;
	q_err[16]=0.02511886;
	q_err[17]=0.01995262;
	q_err[18]=0.01629471;
	q_err[19]=0.01584893;
	q_err[20]=0.01470216;
	q_err[21]=0.00910940;
	q_err[22]=0.00791782;
	q_err[23]=0.00601007;
	q_err[24]=0.00590479;
	q_err[25]=0.00559080;
	q_err[26]=0.00518726;
	q_err[27]=0.00437646;
	q_err[28]=0.00405721;
	q_err[29]=0.00393885;
	q_err[30]=0.00387256;
	q_err[31]=0.00341158;
	q_err[32]=0.00337891;
	q_err[33]=0.00296104;
	q_err[34]=0.00282249;
	q_err[35]=0.00261006;
	q_err[36]=0.00247926;
	q_err[37]=0.00227605;
	q_err[38]=0.00217025;
	q_err[39]=0.00215136;
	q_err[40]=0.00188753;


	igzstream ADF ((para->adapter1).c_str(),ifstream::in);
	if(!ADF.good())
	{
		cerr << "open Adpter File error: "<<para->adapter1<<endl;
		return 1;
	}
	string adpt_id, adpt_se;
	getline(ADF,adpt_id);	adpt_id.erase(0,1);
	getline(ADF,adpt_se);
	int adpt_len = adpt_se.length();
	ADF.close();

	string OUT_ListAdp=(para->OutFq1)+".adapter.list.gz";
	ogzstream OUT_List (OUT_ListAdp.c_str());
	string OUT_statAdp=(para->OutFq1)+".adapter.stat";
	ofstream  OUT_Stat (OUT_statAdp.c_str());


	string title = "#reads_id   reads_len   reads_start   reads_end   adapter_id   adapter_len   adapter_start   adapter_end   align_len   mismatch";
	OUT_List<<title<<endl;
	OUT_Stat<<"adapter_id\tpolluted_reads\tempty_reads\tadapter_sequence"<<endl;


	//input file
	char read_id[MAX_L], read_se[MAX_L], qual_id[MAX_L], qual_se[MAX_L];
	int read_len;

	FILE *read_fs = glue_rapidgzip_fopen(InFq.c_str(), 0);
	if (read_fs==NULL)
	{
		cerr<<"Cann't open files\t"<<InFq<<endl;
		return 1;
	}

	fgets(read_id, MAX_L, read_fs);
	fgets(read_se, MAX_L, read_fs);
	read_len = strlen(read_se);
	if (read_len>0 && read_se[read_len-1]=='\n') read_len--;

	int detect_max=250;
	int detect_cnt=0;
	// Detect quality shift from first reads
	{
		int minQ=50000, maxQ=0;
		detect_cnt=1;
		fgets(qual_id, MAX_L, read_fs);
		fgets(qual_se, MAX_L, read_fs);
		for (int c=0;c<read_len;c++)
		{
			if(minQ>qual_se[c]) minQ=qual_se[c];
			if(maxQ<qual_se[c]) maxQ=qual_se[c];
		}
		while(detect_cnt<detect_max && fgets(read_id, MAX_L, read_fs))
		{
			fgets(read_se, MAX_L, read_fs);
			fgets(qual_id, MAX_L, read_fs);
			fgets(qual_se, MAX_L, read_fs);
			detect_cnt++;
			for(int c=0;c<read_len;c++)
			{
				if(minQ>qual_se[c]) minQ=qual_se[c];
				if(maxQ<qual_se[c]) maxQ=qual_se[c];
			}
		}
		if(minQ>=33 && minQ<=78 && maxQ>=33 && maxQ<=78) para->LowQint=33;
		else if(minQ>=64 && minQ<=108 && maxQ>=64 && maxQ<=108) para->LowQint=64;
		else para->LowQint=64;
	}

	fclose(read_fs);

	// Re-init arrays for main processing
	memset(base, 0, sizeof(base));
	read_num=0; q20=0; q30=0; err=0.0;
	memset(qual, 0, sizeof(qual));
	memset(cycle_qual, 0, sizeof(cycle_qual));
	memset(cycle_base, 0, sizeof(cycle_base));

	//output file
	ofstream  OUTFile;
	OUTFile.open (OutStat.c_str());
	if (!OUTFile.good())
	{
		cout<<"Cann't open files\t"<<OutStat<<endl;
		return 1 ;
	}

	int Find_Len=read_len-9;

	read_fs = glue_rapidgzip_fopen(InFq.c_str(), 0);
	if (read_fs==NULL)
	{
		cerr<<"Cann't open files\t"<<InFq<<endl;
		return 1;
	}
	BatchReader br(read_fs);
	while (br.read_one(read_id, read_se, qual_id, qual_se)) 
	{
		read_num++;
		for (int c=0;c<read_len;c++)
		{
			++cycle_base[c][UC[read_se[c]]];
			int q = qual_se[c]-(para->LowQint);
			q &= ~(q >> 31);
			++cycle_qual[c][q];
		}


		bool find=true;
		int a1 = adpt_len-15;
		int r1 = 0;
		int len;
		int mis;

		for ( ;a1>0 ; a1--)
		{
			int len1 = adpt_len - a1;
			len=(len1<read_len)? len1:read_len;
			mis = 0;
			int map[MAX_L];
			map[0]=0;
			for (int c=0; c<len; c++)
			{
				if (adpt_se[a1+c]==read_se[c]) {map[mis]++;}
				else {mis++;map[mis]=0;}
			}			


			int max_map=map[0];
			for (int c=1; c<=mis; c++)
			{
				if (map[c]>max_map) {max_map=map[c];}
			}

			if ( (max_map>14)    || (len>(mis*5)) )
			{
				find = false ;
				break;
			}

		}

		if (find)
		{
			for (r1=0; r1<Find_Len; r1++)
			{
				int len2 = read_len - r1;
				len=(adpt_len<len2)? adpt_len:len2;
				mis = 0;
				int map[MAX_L];
				map[0]=0;
				for (int c=0; c<len; c++)
				{
					if (adpt_se[c]==read_se[r1+c]) {map[mis]++;}
					else {mis++;map[mis]=0;}
				}

				int max_map=map[0];
				for (int c=1; c<=mis; c++)
				{
					if (map[c]>max_map) {max_map=map[c];}
				}
				if ( (max_map>14)   ||  (len>(mis*5))  )
				{
					find = false;
					break;
				}
			}

		}

		if  (find) {}
		else
		{
			string RID=read_id;
			RID=RID.substr(1,RID.length()-2);
			OUT_List<<RID<<'\t'<<read_len<<'\t'<<r1<<'\t'<<r1+len-1<<'\t'<<adpt_id<<'\t'<<adpt_len<<'\t'<<a1<<'\t'<<a1+len-1<<'\t'<<len<<'\t'<<mis<<"\n";
			adpt_num++;
			if (r1<=3)
			{
				empt_num++;
			}
		}




	}
	fclose(read_fs);
	OUT_List.close();


	float adpt_percent = ((float) adpt_num)/read_num*100;
	float empt_percent = ((float) empt_num)/read_num*100;
	OUT_Stat<<adpt_id<<'\t'<<adpt_num<<" ("<<setprecision(3)<<fixed<<adpt_percent<<"%)"<<'\t'<<empt_num<<" ("<<empt_percent<<"%)"<<'\t'<<adpt_se<<endl;
	OUT_Stat<<"\ntotal_reads: "<<read_num<<endl;
	OUT_Stat.close();



	LLongB  base_total = (LLongB) read_len * read_num;
	for(int c=0;c<read_len;c++)
	{
		base[0]+=cycle_base[c][0];
		base[1]+=cycle_base[c][1];
		base[2]+=cycle_base[c][2];
		base[3]+=cycle_base[c][3];
		base[4]+=cycle_base[c][4];
	}

	for(int c=0;c<read_len;c++)
	{
		for(int q=0;q<MAX_Q;q++)
		{
			qual[q]+=(cycle_qual[c][q]);
		}
	}
	int qMAX=0;
	for(int q=0;q<MAX_Q;q++)
	{
		if (qual[q])
		{
			qMAX=q;
			if (q>=30) q30+=qual[q];
			if (q>=20) q20+=qual[q];
			err+=q_err[q]*qual[q];
		}
	}

	OUTFile<<" the default quality shift value is: -"<<(para->LowQint)<<", "
		<<read_num<<" sequences, "
		<<base_total<<" total length, Max length:"
		<<read_len<<", average length:"
		<<setiosflags(ios::fixed)<<setprecision(2)<<(double)read_len<<endl;
	OUTFile<<"Standard deviations at 0.25:  total "<<100*(sqrt(0.25*(double)base_total)/base_total)
		<<"%, per base "<<100*(sqrt(0.25*(double)read_num)/read_num)
		<<"%"<<endl;
	OUTFile<<"             A     C     G     T     N ";

	for(int q=0;q<=qMAX;q++)
	{
		OUTFile<<setw(4)<<q<<' ';
	}

	OUTFile<<endl;
	OUTFile<<"Total    ";

	for(int b=0;b<=4;b++)
	{
		OUTFile<<setw(5)<<setiosflags(ios::fixed)<<setprecision(1)<<100*(double)base[b]/base_total<<' ';
	}

	for(int q=0;q<=qMAX;q++)
	{
		OUTFile<<setw(4)<<(int)(1000*((double)qual[q]/base_total))<<" ";
	}
	OUTFile<<endl;

	for(int c=0;c<read_len;c++)	
	{
		OUTFile<<"base "<<setw(3)<<c+1<<' ';
		for(int b=0;b<=4;b++)  {OUTFile<<setw(5)<<setiosflags(ios::fixed)<<setprecision(1)<<100*(double)cycle_base[c][b]/read_num<<' ';}
		for(int q=0;q<=qMAX;q++)
		{
			OUTFile<<setw(4)<<(int)(1000*((double)cycle_qual[c][q]/read_num))<<' ';
		}
		OUTFile<<endl;
	}

	OUTFile<<endl;
	OUTFile<<"Error Rate\t%GC\tQ20\tQ30"<<endl;
	OUTFile.precision(2);
	OUTFile<<(100.0*err)/(base_total)<<"\t"<<(100.0*(base[1]+base[2]))/(base_total-base[4])<<"\t"<<(100.0*q20)/base_total<<"\t"<<(100.0*q30)/base_total<<endl;
	OUTFile.close();


	return 0;
}






int PlotfqCheck ( vector  <string> infile , string outfile,string ver, string figPrefix="" )
{
	string fp = figPrefix.empty() ? "" : figPrefix + "_";
	string CCC=outfile+"/"+fp+"base.tmp";
	string DDD=outfile+"/"+fp+"base.gnuplot";
	string PS=outfile+"/"+fp+"base.ps";
	string PNG=outfile+"/"+fp+"base.png";
	string PDF=outfile+"/"+fp+"base.pdf";
	ofstream OUTInfo (CCC.c_str());
	ofstream OUTPlot (DDD.c_str());


	string CCC_S=outfile+"/"+fp+"qual.tmp";
	string DDD_S=outfile+"/"+fp+"qual.gnuplot";
	string PS_S=outfile+"/"+fp+"qual.ps";
	string PNG_S=outfile+"/"+fp+"qual.png";
	string PDF_S=outfile+"/"+fp+"qual.pdf";
	ofstream OUTInfo_S (CCC_S.c_str());
	ofstream OUTPlot_S (DDD_S.c_str());


	int coutFlag=0;
	int FileNum=infile.size();
	string EndFlag="";
	for (int kk=0 ; kk< FileNum ; kk++)
	{
		igzstream IN (infile[kk].c_str(),ifstream::in);
		while (!IN.eof())
		{
			string line;
			getline(IN,line);
			if (line.length()<=0)  { continue ;}
			vector<string> infW;
			split(line,infW," \t");

			if  (infW[0] !="base")
			{
				continue  ;
			}
			coutFlag++;
			OUTInfo<<coutFlag<<"\t"<<infW[2]<<"\t"<<infW[3]<<"\t"<<infW[4]<<"\t"<<infW[5]<<"\t"<<infW[6]<<endl;
			int jjjk=0;
			int SizeVV=infW.size();
			for (int countFF=7 ; countFF<SizeVV ; countFF++)
			{
				OUTInfo_S<<coutFlag<<"\t"<<jjjk<<"\t"<<infW[countFF]<<endl;
				jjjk++;
			}

		}
		IN.close();
		if (kk!=(FileNum-1))
		{
			EndFlag=EndFlag+Int2Str(coutFlag)+" 50\n";
		}
	}
	OUTInfo.close();
	OUTInfo_S.close();

	double VerF=atof(ver.c_str());
	cout<<"Plot : by gnuplot Ver is "<<VerF<<endl;
	if  (VerF< 5.0)
	{
		OUTPlot<<"reset;"<<endl;
		OUTPlot<<"set out \'"<<PS<<"\';"<<endl;
		OUTPlot<<"set xlabel 'Position along reads';\nset grid front lc rgb 'gray';\nset terminal postscript portrait color size 8, 5;\nset ylabel 'Percent';\nset title 'Base percentage composition along reads';\nset yrange [-2:52];\nset xrange [0:"<<coutFlag<<"+1];\n"<<"plot '"<<CCC<<"\' u 1:2 w l lw 3 t 'A', '' u 1:3 w l lw 3 t 'C', '' u 1:4 w l lw 3 t 'G', '' u 1:5 w l lw 3 t 'T', '' u 1:6 w l lw 3 t 'N', '-' w i lc rgb 'blue' lw 3 notitle;\n"<<EndFlag<<"e\nreset;"<<endl;
		OUTPlot.close();
		string path="gnuplot  "+DDD+" ;   convert " +PS+"  "+PNG+"; convert "  +PS+"  "+PDF  +" ; rm -rf "+CCC+" "+DDD+" "+PS+"; ";

		pclose(popen(path.c_str(),"r"));

		OUTPlot_S<<"reset;"<<endl;
		OUTPlot_S<<"set out \'"<<PS_S<<"\';"<<endl;
		OUTPlot_S<<"set xlabel 'Position along reads';\nset cbrange [0:100];\nunset colorbox;\nset palette defined (0 '#ffffff', 10 '#00ff00', 30 '#ffff00', 50 '#ff0000', 70 '#800000', 100 '#000000');\nset grid front lc rgb 'gray';\nset terminal postscript portrait color size 8, 5;\nset ylabel 'Quality';\nset title 'Base percentage composition along reads';\nset yrange [-2:42];\nset key off;\nset xrange [0:"<<coutFlag<<"+1];\n"<<"plot '"<<CCC_S<<"\' u 1:2:($3/10) w image;\nreset;"<<endl;
		OUTPlot_S.close();
		path="gnuplot  "+DDD_S+" ;   convert " +PS_S+"  "+PNG_S+"; convert "  +PS_S+"  "+PDF_S  +" ; rm -rf "+CCC_S+" "+DDD_S+" "+PS_S+"; ";
		pclose(popen(path.c_str(),"r"));
	}
	else
	{
		OUTPlot<<"reset;"<<endl;
		OUTPlot<<"set out \'"<<PDF<<"\';"<<endl;
		OUTPlot<<"set xlabel 'Position along reads';\nset grid front lc rgb 'gray';\nset terminal pdfcairo;\nset ylabel 'Percent';\nset title 'Base percentage composition along reads';\nset yrange [-2:52];\nset xrange [0:"<<coutFlag<<"+1];\n"<<"plot '"<<CCC<<"\' u 1:2 w l lw 3 t 'A', '' u 1:3 w l lw 3 t 'C', '' u 1:4 w l lw 3 t 'G', '' u 1:5 w l lw 3 t 'T', '' u 1:6 w l lw 3 t 'N', '-' w i lc rgb 'blue' lw 3 notitle;\n"<<EndFlag<<"e\nreset;"<<endl;
		OUTPlot.close();
		string path="gnuplot   <<END  "+DDD+" \nEND\n ";
		pclose(popen(path.c_str(),"r"));

		OUTPlot_S<<"reset;"<<endl;
		OUTPlot_S<<"set out \'"<<PDF_S<<"\';"<<endl;
		OUTPlot_S<<"set xlabel 'Position along reads';\nset cbrange [0:100];\nunset colorbox;\nset palette defined (0 '#ffffff', 10 '#00ff00', 30 '#ffff00', 50 '#ff0000', 70 '#800000', 100 '#000000');\nset grid front lc rgb 'gray';\nset terminal pdfcairo;\nset ylabel 'Quality';\nset title 'Base percentage composition along reads';\nset yrange [-2:42];\nset key off;\nset xrange [0:"<<coutFlag<<"+1];\n"<<"plot '"<<CCC_S<<"\' u 1:2:($3/10) w image;\nreset;"<<endl;
		OUTPlot_S.close();
		path="gnuplot <<END  "+DDD_S+"\nEND\n" ;
		pclose(popen(path.c_str(),"r"));
		sleep(3);
		//pclose(popen(path.c_str(),"r")); // waste time for pdf done		
		path=" convert " +PDF+"  "+PNG+"  ; rm -rf "+CCC+" "+DDD+" "+PS+"; ";
		pclose(popen(path.c_str(),"r"));
		path=" convert " +PDF_S+"  "+PNG_S+" ;  rm -rf "+CCC_S+" "+DDD_S+" "+PS_S+"; ";
		pclose(popen(path.c_str(),"r"));


	}

	return  0;
}


//int FQ_Check_main(int argc, char **argv)
int main (int argc, char *argv[ ])
{

	Para_A24 * para_A24 = new Para_A24;
	Para_A24 * P2In = new Para_A24;
	para_A24->LowQint=64;	P2In->LowQint=64;

	if( parse_cmd_FqCheck(argc, argv, para_A24, P2In )==0)
	{
		delete  para_A24 ;
		delete P2In ;
		return 0 ;
	}

	vector<string> FqCheck;

	if (!(P2In->InFq1).empty())
	{
		string path1 = para_A24->OutFq1;
		{
			string ext =path1.substr(path1.rfind('.') ==string::npos ? path1.length() : path1.rfind('.') + 1);
			if (ext != "fqcheck")
				path1=path1+".fqcheck" ;
			else
				(para_A24->OutFq1)=path1.substr(0,path1.rfind('.') ==string::npos ? path1.length() : path1.rfind('.'));
		}

		string path2 = P2In->OutFq1;
		{
			string ext2 =path2.substr(path2.rfind('.') ==string::npos ? path2.length() : path2.rfind('.') + 1);
			if (ext2 != "fqcheck")
				path2=path2+".fqcheck" ;
			else
				(P2In->OutFq1)=path2.substr(0,path2.rfind('.') ==string::npos ? path2.length() : path2.rfind('.'));
		}

		FqCheck.push_back(path1);
		FqCheck.push_back(path2);

		#pragma omp parallel sections
		{
			#pragma omp section
			{
				if ( (para_A24->adapter1).empty() || para_A24->adapter1 == "NA" )
					RunSingleFq(  (para_A24->InFq1) , path1 , para_A24 );
				else
					RunSingleFqAdpter(  (para_A24->InFq1) , path1 , para_A24 );
			}
			#pragma omp section
			{
				if ( (P2In->adapter1).empty() || P2In->adapter1 == "NA" )
					RunSingleFq(  (P2In->InFq1) , path2 , P2In );
				else
					RunSingleFqAdpter(  (P2In->InFq1) , path2 , P2In );
			}
		}
	}
	else
	{
		string path=(para_A24->OutFq1);
		string ext =path.substr(path.rfind('.') ==string::npos ? path.length() : path.rfind('.') + 1);
		if (ext != "fqcheck")
		{
			path=path+".fqcheck" ;
		}
		else
		{
			(para_A24->OutFq1)=path.substr(0,path.rfind('.') ==string::npos ? path.length() : path.rfind('.'));
		}
		FqCheck.push_back(path);

		if ( (para_A24->adapter1).empty() || para_A24->adapter1 == "NA" )
		{
			RunSingleFq(  (para_A24->InFq1) , path , para_A24 );
		}
		else
		{
			RunSingleFqAdpter(  (para_A24->InFq1) , path  , para_A24   );
		}
	}

	if (FqCheck.size()<2)
	{
		cout <<"\twarning: only 1.fqchek , no Png Figure Out"<<endl;
		delete para_A24 ;
		delete P2In ;
		return (0);

	}

	FILE   *stream;
	char   buf[1024]={'\0'};
	string cc="which  gnuplot   2> /dev/null ";
	memset( buf, '\0', sizeof(buf) );
	stream=popen(cc.c_str(),"r") ;
	fread( buf, sizeof(char), sizeof(buf), stream);
	pclose(stream);
	string binPath=buf;
	if (binPath == "")
	{
		cout <<"\twarning::can't find the [gnuplot] in your $PATH ; no Png Figure Out"<<endl;
	}
	else
	{
		string cc="which  convert  2> /dev/null ";
		memset( buf, '\0', sizeof(buf) );
		stream=popen(cc.c_str(),"r") ;
		fread( buf, sizeof(char), sizeof(buf), stream);
		pclose(stream);
		binPath=buf;
		if (binPath == "" )
		{
			cout <<"\twarning:can't find the [convert] in your $PATH ; no png Figure Out"<<endl;
		}
		else
		{
			string path=(para_A24->OutFq1);
			if  (path.rfind('/') ==string::npos)
			{
				path="./";
			}
			else
			{
				path=path.substr(0, path.rfind('/') + 1);
			}
			cc="gnuplot --version |awk '{print $2}'   2> /dev/null ";
			memset( buf, '\0', sizeof(buf) );
			stream=popen(cc.c_str(),"r") ;
			fread( buf, sizeof(char), sizeof(buf), stream);
			pclose(stream);
			cc=buf;		

			PlotfqCheck (  FqCheck ,path ,cc , para_A24->FigPrefix );
		}
	}

	delete para_A24 ;
	delete P2In ;
	return (0);
}





