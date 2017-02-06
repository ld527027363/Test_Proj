
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const unsigned char p32i[] =
{
	16,  7, 20, 21,
	29, 12, 28, 17,
	 1, 15, 23, 26,
	 5, 18, 31, 10,
	 2,  8, 24, 14,
	32, 27,  3,  9,
	19, 13, 30,  6,
	22, 11,  4, 25
};
const int aip[] =
{
	58, 50, 42, 34, 26, 18, 10,  2,
	60, 52, 44, 36, 28, 20, 12,  4,
	62, 54, 46, 38, 30, 22, 14,  6,
	64, 56, 48, 40, 32, 24, 16,  8,
	57, 49, 41, 33, 25, 17,  9,  1,
	59, 51, 43, 35, 27, 19, 11,  3,
	61, 53, 45, 37, 29, 21, 13,  5,
	63, 55, 47, 39, 31, 23, 15,  7
};
const int afp[] =
{
	40,  8, 48, 16, 56, 24, 64, 32,
	39,  7, 47, 15, 55, 23, 63, 31,
	38,  6, 46, 14, 54, 22, 62, 30,
	37,  5, 45, 13, 53, 21, 61, 29,
	36,  4, 44, 12, 52, 20, 60, 28,
	35,  3, 43, 11, 51, 19, 59, 27,
	34,  2, 42, 10, 50, 18, 58, 26,
	33,  1, 41,  9, 49, 17, 57, 25
};
/* permuted choice table (key)  */
const unsigned char pc1[] =
{
	57, 49, 41, 33, 25, 17,  9,
	 1, 58, 50, 42, 34, 26, 18,
	10,  2, 59, 51, 43, 35, 27,
	19, 11,  3, 60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15,
	 7, 62, 54, 46, 38, 30, 22,
	14,  6, 61, 53, 45, 37, 29,
	21, 13,  5, 28, 20, 12,  4
};
const int totrot[] =
{
   1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1
};
const unsigned char pc2[] =
{
	14, 17, 11, 24,  1,  5,
	 3, 28, 15,  6, 21, 10,
	23, 19, 12,  4, 26,  8,
	16,  7, 27, 20, 13,  2,
	41, 52, 31, 37, 47, 55,
	30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53,
	46, 42, 50, 36, 29, 32
};
const int sbox[8][4][16]={
/* S[1] */
	14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
	 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
	 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
	15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13,
/* S[2] */
	15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
	 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
	 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
	13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9,
/* S[3] */
	10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
	13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
	13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
	 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12,
/* S[4] */
	 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
	13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
	10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
	 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14,
/* S[5] */
	 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
	14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
	 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
	11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3,
/* S[6] */
	12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
	10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
	 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
	 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13,
/* S[7] */
	 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
	13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
	 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
	 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12,
/* S[8] */
	13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
	 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
	 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
	 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
};
const int e[48] = {
	32,  1,  2,  3,  4,  5,
	 4,  5,  6,  7,  8,  9,
	 8,  9, 10, 11, 12, 13,
	12, 13, 14, 15, 16, 17,
	16, 17, 18, 19, 20, 21,
	20, 21, 22, 23, 24, 25,
	24, 25, 26, 27, 28, 29,
	28, 29, 30, 31, 32,  1
};
void char2bit(unsigned char datachar,int *databit)
{
	int i;
	for(i=0;i<8;i++) databit[i]=(datachar>>(7-i))&0x01;
	return;
}
void chartobit(unsigned char *sour,int data[])
{
	int i;
	for(i=0;i<8;i++){
		char2bit(sour[i],&data[i*8]);
	}
	return;
}
void boxer(int data01[48],int bits[32])
{
	int i=0;
	int row,line;
	int result[8];
	for(i=0;i<8;i++){
		row=data01[i*6+0]*2+data01[i*6+5]*1;
		line=data01[i*6+1]*8+data01[i*6+2]*4+data01[i*6+3]*2+
						data01[i*6+4]*1;
		result[i]=sbox[i][row][line];
	}
	for(i=0;i<8;i++){
		bits[i*4+0]=(result[i]&15)>>3;
		bits[i*4+1]=(result[i]& 7)>>2;
		bits[i*4+2]=(result[i]& 3)>>1;
		bits[i*4+3]=(result[i]& 1)>>0;
	}
	return;
}
void expand(int data01[],int data02[])
{
	int i;
	for(i=0;i<48;i++){
		data02[i]=data01[e[i]-1];
	}
	return;
}
void bitxor(int data01[],int data02[],int len)
{
	int i;
	for(i=0;i<len;i++){
		data01[i]=(data01[i]==data02[i]?0:1);
	}
	return;
}
void ip(int data01[])
{
	int temp[64];
	int i=0;
	for(i=0;i<64;i++){
		temp[i]=data01[aip[i]-1];
	}
	for(i=0;i<64;i++)data01[i]=temp[i];
	return;
}
void fp(int data01[])
{
	int temp[64];
	int i=0;
	for(i=0;i<64;i++){
		temp[i]=data01[afp[i]-1];
	}
	for(i=0;i<64;i++)data01[i]=temp[i];
	return;
}
void p(int data01[])
{
	int i;
	int temp[32];
	for(i=0;i<32;i++){
		temp[i]=data01[p32i[i]-1];
	}
	for(i=0;i<32;i++)data01[i]=temp[i];
	return;
}
void bittochar(int data01[],unsigned char *data)
{
	int i;
	for(i=0;i<8;i++){
		data[i]=data01[i*8]*128+data01[i*8+1]*64+data01[i*8+2]*32+
			data01[i*8+3]*16+data01[i*8+4]*8+data01[i*8+5]*4+
			data01[i*8+6]*2+data01[i*8+7]*1;
	}
	return;
}
void fpc1(unsigned char *key,int c[],int d[])
{
	int temp[64];
	int data01[56];
	int i;
	chartobit(key,temp);
	for(i=0;i<56;i++)data01[i]=temp[pc1[i]-1];
	for(i=0;i<28;i++){
		c[i]=data01[i];
		d[i]=data01[i+28];
	}
	return;
}
void fkey(unsigned char *key,int sk[16][48])
{
	int c[28];
	int d[28];
	int temp[56];
	int ctemp=0;
	int dtemp=0;
	int i=0;
	int j=0;
	int k=0;
	fpc1(key,c,d);
	for(i=0;i<16;i++){
		for(j=0;j<totrot[i];j++){
			ctemp=c[0];
			dtemp=d[0];
			for(k=0;k<27;k++){
				c[k]=c[k+1];
				d[k]=d[k+1];
			}
			c[27]=ctemp;
			d[27]=dtemp;
		}
		for(k=0;k<28;k++){
			temp[k]=c[k];
			temp[k+28]=d[k];
		}
		for(k=0;k<48;k++)sk[i][k]=temp[pc2[k]-1];
	}
	return;
}
void des(unsigned char *key,unsigned char *sour,unsigned char *dest,int m)
{
	int i,j;
	int data01[64];
	int l[32];
	int r[32];
	int data02[48];
	int data03[32];
	int k[16][48];
	fkey(key,k);
	chartobit(sour,data01);
	ip(data01);
	for(i=0;i<32;i++){
		l[i]=data01[i];
		r[i]=data01[i+32];
	}
	for(i=0;i<16;i++){
		expand(r,data02);
		if(m)bitxor(data02,k[i],48);
		else bitxor(data02,k[15-i],48);
		boxer(data02,data03);
		p(data03);
		bitxor(l,data03,32);
		for(j=0;j<32;j++){
			data03[j]=r[j];
			r[j]=l[j];
			l[j]=data03[j];
		}
	}
	for(j=0;j<32;j++){
		data01[j]=r[j];
		data01[j+32]=l[j];
	}
	fp(data01);
	bittochar(data01,dest);
	return;
}

//Triple DES
int tripledes(unsigned char *key,unsigned char *src,unsigned char *dest,int m)
{
	unsigned char	src0[18],key0[18];
	int		i,flag;
	unsigned char data[18];


	if (m == 0)
		flag=0;
	else
		flag=1;
	for (i=0;i<8;i++){
		src0[i] = src[i];
		key0[i] = key[i];
	}
	des(key0,src0,data,flag);


	if (flag== 0)
		flag=1;
	else
		flag=0;

	for (i=0;i<8;i++){
		src0[i] = data[i];
		key0[i] = key[i+8];
	}

	des(key0,src0,data,flag);

	if (flag== 0)
		flag=1;
	else
		flag=0;

	for (i=0;i<8;i++){
		src0[i] = data[i];
		key0[i] = key[i];
	}

	des(key0,src0,data,flag);

	memcpy(dest,data,8);

	return 0;
}


int creat_mac(unsigned char KeyLen,unsigned char *Key, int DataLen,
							unsigned char *Data, unsigned char *MacData)
{
	int mode, i, j;
	unsigned char InitData[8];
	unsigned char TempData[16];
	int data_a,data_b;
	unsigned char tmp[3];

	

	if((KeyLen != 8 ) &&(KeyLen != 16 ))
	{
		return -1;
	}
	data_a=DataLen/8;
	data_b=DataLen%8;
	memset(InitData,0,8);
	for(i=0;i<data_a;i++)
	{
		for(j=0;j<8;j++)
			InitData[j]=InitData[j]^Data[i*8+j];
	}
	for(j=0;j<data_b;j++)
		InitData[j]=InitData[j]^Data[i*8+j];

//	StdCharsToHexChars(InitData, TempData, 8);
	
	for(j=0;j<8;j++)
	{
		i=InitData[j];
		sprintf((char*)tmp,"%02X",i);
		TempData[2*j]=tmp[0];
		TempData[2*j+1]=tmp[1];
	}

	mode=1; //Encryption
	if(0x08==KeyLen)
		des(&Key[0],TempData,InitData,mode);
	else
		tripledes(&Key[0],TempData,InitData,mode);

	for(i=0;i<8;++i)
	{
		InitData[i] ^= TempData[i+8];
	}

	mode=1; //Encryption
	if(0x08==KeyLen)
		des(&Key[0],InitData,InitData,mode);
	else
		tripledes(&Key[0],TempData,InitData,mode);

//	StdCharsToHexChars(InitData, TempData, 8);
	for(j=0;j<8;j++)
	{
		i=InitData[j];
		sprintf((char*)tmp,"%02X",i);
		TempData[2*j]=tmp[0];
		TempData[2*j+1]=tmp[1];
	}

	memcpy(MacData,TempData,8);

	return 0;
}
