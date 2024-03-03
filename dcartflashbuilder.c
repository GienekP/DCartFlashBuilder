/*--------------------------------------------------------------------*/
/* DCartFlashBuilder                                                  */
/* by GienekP                                                         */
/* (c) 2023                                                           */
/*--------------------------------------------------------------------*/
#include <stdio.h>
/*--------------------------------------------------------------------*/
#include "imageDCart.h"
/*--------------------------------------------------------------------*/
typedef unsigned char U8;
/*--------------------------------------------------------------------*/
unsigned int find(const U8 *base, unsigned int size, U8 nob, U8 no)
{
	unsigned int i,ret=0;
	for (i=0; i<(size-8); i++)
	{
		if ((base[i]==0xAA) && (base[i+1]=='B') && (base[i+2]=='A') && (base[i+3]=='N') &&
		(base[i+4]=='K') && (base[i+5]==nob) && (base[i+6]==no) && (base[i+7]==0x55))
		{
			ret=i;
			i=size;
		}
	};
	return ret;
}
/*--------------------------------------------------------------------*/
void save(const char *filename, U8 *data, U8 nobanks, U8 *base, unsigned int outsize)
{
	unsigned int i,pos;
	U8 b;
	FILE *pf;
	pf=fopen(filename,"wb");
	if (pf)
	{
		printf("Prepare %ikB version\n",nobanks*8);
		for (b=0; b<nobanks; b++)
		{
			pos=find(base,outsize,nobanks,b);
			if (pos>0) {for (i=0; i<8192; i++) {base[pos+i]=data[8192*b+i];};};
		};
		fwrite(base,sizeof(U8),outsize,pf);
		fclose(pf);
		printf("Save XEX \"%s\"\n",filename);
	};	
}
/*--------------------------------------------------------------------*/
unsigned int load(const char *filename, U8 *data, unsigned int size)
{
	unsigned int ret=0;
	FILE *pf;
	pf=fopen(filename,"rb");
	if (pf)
	{
		ret=fread(data,sizeof(U8),size,pf);
		fclose(pf);
		printf("Load \"%s\"\n",filename);
	};
	return ret;
}
/*--------------------------------------------------------------------*/
void clear(U8 *data, unsigned int size)
{
	unsigned int i;
	for (i=0; i<size; i++) {data[i]=0xFF;};
}
/*--------------------------------------------------------------------*/
unsigned int toInt(const U8 *str)
{
	unsigned int i,ret=0;
	for (i=0; i<4; i++)
	{
		ret<<=8;
		ret|=(unsigned int)(str[i]);
	};
	return ret;
}
/*--------------------------------------------------------------------*/
unsigned int car(U8 *data, unsigned int size)
{
	unsigned int ret=size;
	if (toInt(&data[0])==0x43415254)
	{
		unsigned int i,sum=0,rs=toInt(&data[8]);
		for (i=16; i<size; i++) {sum+=data[i];};
		if (sum==rs)
		{
			for (i=0; i<(size-16); i++) {data[i]=data[i+16];};
			for (i=(size-16); i<size; i++) {data[i]=0xFF;};
			ret-=16;
			printf("Detect CAR header\n");
		};
	};
	return ret;
}
/*--------------------------------------------------------------------*/
void flashBuilder(const char *filein, const char *fileout)
{
	U8 data[512*1024+16];
	const unsigned int maxsize=sizeof(data);
	unsigned int size;
	clear(data,maxsize);
	size=load(filein,data,sizeof(data));
	if (size>0)
	{
		size=car(data,size);
		save(fileout,data,64,imageDCart,imageDCart_len);
	}
	else
	{
		printf("Unknown file \"%s\"\n",filein);
	};
}
/*--------------------------------------------------------------------*/
int main( int argc, char* argv[] )
{		
	printf("DCartFlashBuilder - ver: %s\n",__DATE__);
	switch (argc)
	{
		case 3:
		{
			flashBuilder(argv[1],argv[2]);

		} break;
		default:
		{
			printf("(c) GienekP\n");
			printf("use:\ndcartflashbuilder file.bincar file.xex\n");
		} break;
	};
	return 0;
}
/*--------------------------------------------------------------------*/
