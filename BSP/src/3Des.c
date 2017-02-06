#include "3Des.h"

//#include "Wmmp_comm.h"
#if   1 //{


uint8   ucaDesb[8],ucaDesx[8],tem_ri[8],tem_key[8],ucaKeyT[8],ucaDesTin[8],ucaDesTOut[8];
uint8   btCry1,btCryC1,btCryC2;
uint8   ucDesT1,ucDesT2,ucDesT3,ucDesT4,ucDesT5,ucDesT6,ucDesT7; //74个缓冲区

const   uint8 SB1[64]=
{
    14, 4,13, 1, 2,15,11, 8, 3,10, 6,12, 5, 9, 0, 7,
     0,15, 7, 4,14, 2,13, 1,10, 6,12,11, 9, 5, 3, 8,
     4, 1,14, 8,13, 6, 2,11,15,12, 9, 7, 3,10, 5, 0,
    15,12, 8, 2, 4, 9, 1, 7, 5,11, 3,14,10, 0, 6,13
};
const   uint8 SB2[64]=
{
    15, 1, 8,14, 6,11, 3, 4, 9, 7, 2,13,12, 0, 5,10,
     3,13, 4, 7,15, 2, 8,14,12, 0, 1,10, 6, 9,11, 5,
     0,14, 7,11,10, 4,13, 1, 5, 8,12, 6, 9, 3, 2,15,
    13, 8,10, 1, 3,15, 4, 2,11, 6, 7,12, 0, 5,14, 9
};
const   uint8 SB3[64]=
{
    10, 0, 9,14, 6, 3,15, 5, 1,13,12, 7,11, 4, 2, 8,
    13, 7, 0, 9, 3, 4, 6,10, 2, 8, 5,14,12,11,15, 1,
    13, 6, 4, 9, 8,15, 3, 0,11, 1, 2,12, 5,10,14, 7,
     1,10,13, 0, 6, 9, 8, 7, 4,15,14, 3,11, 5, 2,12
};
const   uint8 SB4[64]=
{
     7,13,14, 3, 0, 6, 9,10, 1, 2, 8, 5,11,12, 4,15,
    13, 8,11, 5, 6,15, 0, 3, 4, 7, 2,12, 1,10,14, 9,
    10, 6, 9, 0,12,11, 7,13,15, 1, 3,14, 5, 2, 8, 4,
     3,15, 0, 6,10, 1,13, 8, 9, 4, 5,11,12, 7, 2,14
};
const   uint8 SB5[64]=
{
     2,12, 4, 1, 7,10,11, 6, 8, 5, 3,15,13, 0,14, 9,
    14,11, 2,12, 4, 7,13, 1, 5, 0,15,10, 3, 9, 8, 6,
     4, 2, 1,11,10,13, 7, 8,15, 9,12, 5, 6, 3, 0,14,
    11, 8,12, 7, 1,14, 2,13, 6,15, 0, 9,10, 4, 5, 3
};
const   uint8 SB6[64]=
{
    12, 1,10,15, 9, 2, 6, 8, 0,13, 3, 4,14, 7, 5,11,
    10,15, 4, 2, 7,12, 9, 5, 6, 1,13,14, 0,11, 3, 8,
     9,14,15, 5, 2, 8,12, 3, 7, 0, 4,10, 1,13,11, 6,
     4, 3, 2,12, 9, 5,15,10,11,14, 1, 7, 6, 0, 8,13
};
const   uint8 SB7[64]=
{
     4,11, 2,14,15, 0, 8,13, 3,12, 9, 7, 5,10, 6, 1,
    13, 0,11, 7, 4, 9, 1,10,14, 3, 5,12, 2,15, 8, 6,
     1, 4,11,13,12, 3, 7,14,10,15, 6, 8, 0, 5, 9, 2,
     6,11,13, 8, 1, 4,10, 7, 9, 5, 0,15,14, 2, 3,12
};
const   uint8 SB8[64]=
{
    13, 2, 8, 4, 6,15,11, 1,10, 9, 3,14, 5, 0,12, 7,
     1,15,13, 8,10, 3, 7, 4,12, 5, 6,11, 0,14, 9, 2,
     7,11, 4, 1, 9,12,14, 2, 0, 6,10,13,15, 3, 5, 8,
     2, 1,14, 7, 4,10, 8,13,15,12, 9, 0, 3, 5, 6,11
};

const   uint8 Left_move[16]={ 1,1,2,2,2,2,2,2,  1,2,2,2,2,2,2,1};
const   uint8 right_move[16]={0,1,2,2,2,2,2,2,  1,2,2,2,2,2,2,1};

void   transposetem_ri(uint8 * tem_ri)
{//4 -> 6
    memcpy(ucaDesTin,tem_ri,4);
    memset((void *)ucaDesTOut, 0 ,6);
    ucaDesTOut[0] = (ucaDesTin[0] >> 1) & 0x7C;
    if((ucaDesTin[3]&0x01)!=0)  ucaDesTOut[0] |= 0x80;
    if((ucaDesTin[0]&0x10)!=0)  ucaDesTOut[0] |= 0x02;
    if((ucaDesTin[0]&0x08)!=0)  ucaDesTOut[0] |= 0x01;

    ucaDesTOut[1] =  ucaDesTin[0] << 5;
    ucaDesTOut[1]|=  ucaDesTin[1] >> 5;
    if((ucaDesTin[1]&0x80)!=0)  ucaDesTOut[1] |= 0x10;
    if((ucaDesTin[0]&0x01)!=0)  ucaDesTOut[1] |= 0x08;

    ucaDesTOut[2] = (ucaDesTin[1] << 1) & 0x3E;
    if((ucaDesTin[1]&0x10)!=0)  ucaDesTOut[2] |= 0x80;
    if((ucaDesTin[1]&0x08)!=0)  ucaDesTOut[2] |= 0x40;
    if((ucaDesTin[2]&0x80)!=0)  ucaDesTOut[2] |= 0x01;

    ucaDesTOut[3] = (ucaDesTin[2] >> 1) & 0x7C;
    if((ucaDesTin[1]&0x01)!=0)  ucaDesTOut[3] |= 0x80;
    if((ucaDesTin[2]&0x10)!=0)  ucaDesTOut[3] |= 0x02;
    if((ucaDesTin[2]&0x08)!=0)  ucaDesTOut[3] |= 0x01;

    ucaDesTOut[4] = ucaDesTin[2] << 5;
    ucaDesTOut[4]|= ucaDesTin[3] >> 5;
    if((ucaDesTin[3]&0x80)!=0)  ucaDesTOut[4] |= 0x10;
    if((ucaDesTin[2]&0x01)!=0)  ucaDesTOut[4] |= 0x08;

    ucaDesTOut[5] = (ucaDesTin[3] << 1) & 0x3E;
    if((ucaDesTin[3]&0x10)!=0)  ucaDesTOut[5] |= 0x80;
    if((ucaDesTin[3]&0x08)!=0)  ucaDesTOut[5] |= 0x40;
    if((ucaDesTin[0]&0x80)!=0)  ucaDesTOut[5] |= 0x01;
    memcpy(tem_ri,ucaDesTOut,6);
}


void   transposeIPTR(uint8 * srcdata)
{//
    uint8   ucWrt,ucRd,ucBitMask,ucRslt;
    const   uint8 SBz1_8[8]= {0x40, 0x10, 0x04,0x01, 0x80,0x20,0x08,0x02 };

    memset( (void *)ucaDesTOut,0,8);
    memcpy(ucaDesTin,srcdata,8); //4 0 5 1 6 2 7 3

#if 1  //zlq133
    for(ucWrt=0; ucWrt<8;ucWrt++)
    {
        ucBitMask = SBz1_8[ucWrt];
        ucRslt = 0;
        for( ucRd= 0; ucRd < 8; ucRd++)
        {
            if((ucaDesTin[ucRd] & ucBitMask) != 0)
                ucRslt +=  (0x01 << ucRd);
        }
        ucaDesTOut[ucWrt] = ucRslt;
    }
#else
    if((ucaDesTin[7]&0x40)!=0)  ucaDesTOut[0] |= 0x80;
    if((ucaDesTin[6]&0x40)!=0)  ucaDesTOut[0] |= 0x40;
    if((ucaDesTin[5]&0x40)!=0)  ucaDesTOut[0] |= 0x20;
    if((ucaDesTin[4]&0x40)!=0)  ucaDesTOut[0] |= 0x10;
    if((ucaDesTin[3]&0x40)!=0)  ucaDesTOut[0] |= 0x08;
    if((ucaDesTin[2]&0x40)!=0)  ucaDesTOut[0] |= 0x04;
    if((ucaDesTin[1]&0x40)!=0)  ucaDesTOut[0] |= 0x02;
    if((ucaDesTin[0]&0x40)!=0)  ucaDesTOut[0] |= 0x01;

    if((ucaDesTin[7]&0x10)!=0)  ucaDesTOut[1] |= 0x80;
    if((ucaDesTin[6]&0x10)!=0)  ucaDesTOut[1] |= 0x40;
    if((ucaDesTin[5]&0x10)!=0)  ucaDesTOut[1] |= 0x20;
    if((ucaDesTin[4]&0x10)!=0)  ucaDesTOut[1] |= 0x10;
    if((ucaDesTin[3]&0x10)!=0)  ucaDesTOut[1] |= 0x08;
    if((ucaDesTin[2]&0x10)!=0)  ucaDesTOut[1] |= 0x04;
    if((ucaDesTin[1]&0x10)!=0)  ucaDesTOut[1] |= 0x02;
    if((ucaDesTin[0]&0x10)!=0)  ucaDesTOut[1] |= 0x01;


    if((ucaDesTin[7]&0x04)!=0)  ucaDesTOut[2] |= 0x80;
    if((ucaDesTin[6]&0x04)!=0)  ucaDesTOut[2] |= 0x40;
    if((ucaDesTin[5]&0x04)!=0)  ucaDesTOut[2] |= 0x20;
    if((ucaDesTin[4]&0x04)!=0)  ucaDesTOut[2] |= 0x10;
    if((ucaDesTin[3]&0x04)!=0)  ucaDesTOut[2] |= 0x08;
    if((ucaDesTin[2]&0x04)!=0)  ucaDesTOut[2] |= 0x04;
    if((ucaDesTin[1]&0x04)!=0)  ucaDesTOut[2] |= 0x02;
    if((ucaDesTin[0]&0x04)!=0)  ucaDesTOut[2] |= 0x01;


    if((ucaDesTin[7]&0x01)!=0)  ucaDesTOut[3] |= 0x80;
    if((ucaDesTin[6]&0x01)!=0)  ucaDesTOut[3] |= 0x40;
    if((ucaDesTin[5]&0x01)!=0)  ucaDesTOut[3] |= 0x20;
    if((ucaDesTin[4]&0x01)!=0)  ucaDesTOut[3] |= 0x10;
    if((ucaDesTin[3]&0x01)!=0)  ucaDesTOut[3] |= 0x08;
    if((ucaDesTin[2]&0x01)!=0)  ucaDesTOut[3] |= 0x04;
    if((ucaDesTin[1]&0x01)!=0)  ucaDesTOut[3] |= 0x02;
    if((ucaDesTin[0]&0x01)!=0)  ucaDesTOut[3] |= 0x01;

    if((ucaDesTin[7]&0x80)!=0)  ucaDesTOut[4] |= 0x80;
    if((ucaDesTin[6]&0x80)!=0)  ucaDesTOut[4] |= 0x40;
    if((ucaDesTin[5]&0x80)!=0)  ucaDesTOut[4] |= 0x20;
    if((ucaDesTin[4]&0x80)!=0)  ucaDesTOut[4] |= 0x10;
    if((ucaDesTin[3]&0x80)!=0)  ucaDesTOut[4] |= 0x08;
    if((ucaDesTin[2]&0x80)!=0)  ucaDesTOut[4] |= 0x04;
    if((ucaDesTin[1]&0x80)!=0)  ucaDesTOut[4] |= 0x02;
    if((ucaDesTin[0]&0x80)!=0)  ucaDesTOut[4] |= 0x01;

    if((ucaDesTin[7]&0x20)!=0)  ucaDesTOut[5] |= 0x80;
    if((ucaDesTin[6]&0x20)!=0)  ucaDesTOut[5] |= 0x40;
    if((ucaDesTin[5]&0x20)!=0)  ucaDesTOut[5] |= 0x20;
    if((ucaDesTin[4]&0x20)!=0)  ucaDesTOut[5] |= 0x10;
    if((ucaDesTin[3]&0x20)!=0)  ucaDesTOut[5] |= 0x08;
    if((ucaDesTin[2]&0x20)!=0)  ucaDesTOut[5] |= 0x04;
    if((ucaDesTin[1]&0x20)!=0)  ucaDesTOut[5] |= 0x02;
    if((ucaDesTin[0]&0x20)!=0)  ucaDesTOut[5] |= 0x01;

    if((ucaDesTin[7]&0x08)!=0)  ucaDesTOut[6] |= 0x80;
    if((ucaDesTin[6]&0x08)!=0)  ucaDesTOut[6] |= 0x40;
    if((ucaDesTin[5]&0x08)!=0)  ucaDesTOut[6] |= 0x20;
    if((ucaDesTin[4]&0x08)!=0)  ucaDesTOut[6] |= 0x10;
    if((ucaDesTin[3]&0x08)!=0)  ucaDesTOut[6] |= 0x08;
    if((ucaDesTin[2]&0x08)!=0)  ucaDesTOut[6] |= 0x04;
    if((ucaDesTin[1]&0x08)!=0)  ucaDesTOut[6] |= 0x02;
    if((ucaDesTin[0]&0x08)!=0)  ucaDesTOut[6] |= 0x01;

    if((ucaDesTin[7]&0x02)!=0)  ucaDesTOut[7] |= 0x80;
    if((ucaDesTin[6]&0x02)!=0)  ucaDesTOut[7] |= 0x40;
    if((ucaDesTin[5]&0x02)!=0)  ucaDesTOut[7] |= 0x20;
    if((ucaDesTin[4]&0x02)!=0)  ucaDesTOut[7] |= 0x10;
    if((ucaDesTin[3]&0x02)!=0)  ucaDesTOut[7] |= 0x08;
    if((ucaDesTin[2]&0x02)!=0)  ucaDesTOut[7] |= 0x04;
    if((ucaDesTin[1]&0x02)!=0)  ucaDesTOut[7] |= 0x02;
    if((ucaDesTin[0]&0x02)!=0)  ucaDesTOut[7] |= 0x01;
#endif
    memcpy(srcdata,ucaDesTOut,8);
}


void   transposePC_1_Tr(uint8 *ucaKeyT)
{//8->7
    memcpy(ucaDesTin,ucaKeyT,8);
    memset((void *)ucaDesTOut, 0 , 7);

    if((ucaDesTin[7]&0x80)!=0)  ucaDesTOut[0] |= 0x80;
    if((ucaDesTin[6]&0x80)!=0)  ucaDesTOut[0] |= 0x40;
    if((ucaDesTin[5]&0x80)!=0)  ucaDesTOut[0] |= 0x20;
    if((ucaDesTin[4]&0x80)!=0)  ucaDesTOut[0] |= 0x10;
    if((ucaDesTin[3]&0x80)!=0)  ucaDesTOut[0] |= 0x08;
    if((ucaDesTin[2]&0x80)!=0)  ucaDesTOut[0] |= 0x04;
    if((ucaDesTin[1]&0x80)!=0)  ucaDesTOut[0] |= 0x02;
    if((ucaDesTin[0]&0x80)!=0)  ucaDesTOut[0] |= 0x01;

    if((ucaDesTin[7]&0x40)!=0)  ucaDesTOut[1] |= 0x80;
    if((ucaDesTin[6]&0x40)!=0)  ucaDesTOut[1] |= 0x40;
    if((ucaDesTin[5]&0x40)!=0)  ucaDesTOut[1] |= 0x20;
    if((ucaDesTin[4]&0x40)!=0)  ucaDesTOut[1] |= 0x10;
    if((ucaDesTin[3]&0x40)!=0)  ucaDesTOut[1] |= 0x08;
    if((ucaDesTin[2]&0x40)!=0)  ucaDesTOut[1] |= 0x04;
    if((ucaDesTin[1]&0x40)!=0)  ucaDesTOut[1] |= 0x02;
    if((ucaDesTin[0]&0x40)!=0)  ucaDesTOut[1] |= 0x01;

    if((ucaDesTin[7]&0x20)!=0)  ucaDesTOut[2] |= 0x80;
    if((ucaDesTin[6]&0x20)!=0)  ucaDesTOut[2] |= 0x40;
    if((ucaDesTin[5]&0x20)!=0)  ucaDesTOut[2] |= 0x20;
    if((ucaDesTin[4]&0x20)!=0)  ucaDesTOut[2] |= 0x10;
    if((ucaDesTin[3]&0x20)!=0)  ucaDesTOut[2] |= 0x08;
    if((ucaDesTin[2]&0x20)!=0)  ucaDesTOut[2] |= 0x04;
    if((ucaDesTin[1]&0x20)!=0)  ucaDesTOut[2] |= 0x02;
    if((ucaDesTin[0]&0x20)!=0)  ucaDesTOut[2] |= 0x01;


    if((ucaDesTin[7]&0x10)!=0)  ucaDesTOut[3] |= 0x80;
    if((ucaDesTin[6]&0x10)!=0)  ucaDesTOut[3] |= 0x40;
    if((ucaDesTin[5]&0x10)!=0)  ucaDesTOut[3] |= 0x20;
    if((ucaDesTin[4]&0x10)!=0)  ucaDesTOut[3] |= 0x10;

    if((ucaDesTin[7]&0x02)!=0)  ucaDesTOut[3] |= 0x08;
    if((ucaDesTin[6]&0x02)!=0)  ucaDesTOut[3] |= 0x04;
    if((ucaDesTin[5]&0x02)!=0)  ucaDesTOut[3] |= 0x02;
    if((ucaDesTin[4]&0x02)!=0)  ucaDesTOut[3] |= 0x01;

    if((ucaDesTin[3]&0x02)!=0)  ucaDesTOut[4] |= 0x80;
    if((ucaDesTin[2]&0x02)!=0)  ucaDesTOut[4] |= 0x40;
    if((ucaDesTin[1]&0x02)!=0)  ucaDesTOut[4] |= 0x20;
    if((ucaDesTin[0]&0x02)!=0)  ucaDesTOut[4] |= 0x10;

    if((ucaDesTin[7]&0x04)!=0)  ucaDesTOut[4] |= 0x08;
    if((ucaDesTin[6]&0x04)!=0)  ucaDesTOut[4] |= 0x04;
    if((ucaDesTin[5]&0x04)!=0)  ucaDesTOut[4] |= 0x02;
    if((ucaDesTin[4]&0x04)!=0)  ucaDesTOut[4] |= 0x01;

    if((ucaDesTin[3]&0x04)!=0)  ucaDesTOut[5] |= 0x80;
    if((ucaDesTin[2]&0x04)!=0)  ucaDesTOut[5] |= 0x40;
    if((ucaDesTin[1]&0x04)!=0)  ucaDesTOut[5] |= 0x20;
    if((ucaDesTin[0]&0x04)!=0)  ucaDesTOut[5] |= 0x10;

    if((ucaDesTin[7]&0x08)!=0)  ucaDesTOut[5] |= 0x08;
    if((ucaDesTin[6]&0x08)!=0)  ucaDesTOut[5] |= 0x04;
    if((ucaDesTin[5]&0x08)!=0)  ucaDesTOut[5] |= 0x02;
    if((ucaDesTin[4]&0x08)!=0)  ucaDesTOut[5] |= 0x01;

    if((ucaDesTin[3]&0x08)!=0)  ucaDesTOut[6] |= 0x80;
    if((ucaDesTin[2]&0x08)!=0)  ucaDesTOut[6] |= 0x40;
    if((ucaDesTin[1]&0x08)!=0)  ucaDesTOut[6] |= 0x20;
    if((ucaDesTin[0]&0x08)!=0)  ucaDesTOut[6] |= 0x10;

    if((ucaDesTin[3]&0x10)!=0)  ucaDesTOut[6] |= 0x08;
    if((ucaDesTin[2]&0x10)!=0)  ucaDesTOut[6] |= 0x04;
    if((ucaDesTin[1]&0x10)!=0)  ucaDesTOut[6] |= 0x02;
    if((ucaDesTin[0]&0x10)!=0)  ucaDesTOut[6] |= 0x01;

    memcpy(ucaKeyT,ucaDesTOut,7);
}

void    transposePC_2_Tr(uint8 *tem_key)         //临时密钥转换
{//7->6
    memcpy(ucaDesTin,tem_key,7);
    memset((void *)ucaDesTOut, 0 , 6);

    if((ucaDesTin[1]&0x04)!=0)  ucaDesTOut[0] |= 0x80;
    if((ucaDesTin[2]&0x80)!=0)  ucaDesTOut[0] |= 0x40;
    if((ucaDesTin[1]&0x20)!=0)  ucaDesTOut[0] |= 0x20;
    if((ucaDesTin[2]&0x01)!=0)  ucaDesTOut[0] |= 0x10;
    if((ucaDesTin[0]&0x80)!=0)  ucaDesTOut[0] |= 0x08;
    if((ucaDesTin[0]&0x08)!=0)  ucaDesTOut[0] |= 0x04;
    if((ucaDesTin[0]&0x20)!=0)  ucaDesTOut[0] |= 0x02;
    if((ucaDesTin[3]&0x10)!=0)  ucaDesTOut[0] |= 0x01;

    if((ucaDesTin[1]&0x02)!=0)  ucaDesTOut[1] |= 0x80;
    if((ucaDesTin[0]&0x04)!=0)  ucaDesTOut[1] |= 0x40;
    if((ucaDesTin[2]&0x08)!=0)  ucaDesTOut[1] |= 0x20;
    if((ucaDesTin[1]&0x40)!=0)  ucaDesTOut[1] |= 0x10;
    if((ucaDesTin[2]&0x02)!=0)  ucaDesTOut[1] |= 0x08;
    if((ucaDesTin[2]&0x20)!=0)  ucaDesTOut[1] |= 0x04;
    if((ucaDesTin[1]&0x10)!=0)  ucaDesTOut[1] |= 0x02;
    if((ucaDesTin[0]&0x10)!=0)  ucaDesTOut[1] |= 0x01;

    if((ucaDesTin[3]&0x40)!=0)  ucaDesTOut[2] |= 0x80;
    if((ucaDesTin[0]&0x01)!=0)  ucaDesTOut[2] |= 0x40;
    if((ucaDesTin[1]&0x01)!=0)  ucaDesTOut[2] |= 0x20;
    if((ucaDesTin[0]&0x02)!=0)  ucaDesTOut[2] |= 0x10;
    if((ucaDesTin[3]&0x20)!=0)  ucaDesTOut[2] |= 0x08;
    if((ucaDesTin[2]&0x10)!=0)  ucaDesTOut[2] |= 0x04;
    if((ucaDesTin[1]&0x08)!=0)  ucaDesTOut[2] |= 0x02;
    if((ucaDesTin[0]&0x40)!=0)  ucaDesTOut[2] |= 0x01;

    if((ucaDesTin[5]&0x80)!=0)  ucaDesTOut[3] |= 0x80;
    if((ucaDesTin[6]&0x10)!=0)  ucaDesTOut[3] |= 0x40;
    if((ucaDesTin[3]&0x02)!=0)  ucaDesTOut[3] |= 0x20;
    if((ucaDesTin[4]&0x08)!=0)  ucaDesTOut[3] |= 0x10;
    if((ucaDesTin[5]&0x02)!=0)  ucaDesTOut[3] |= 0x08;
    if((ucaDesTin[6]&0x02)!=0)  ucaDesTOut[3] |= 0x04;
    if((ucaDesTin[3]&0x04)!=0)  ucaDesTOut[3] |= 0x02;
    if((ucaDesTin[4]&0x01)!=0)  ucaDesTOut[3] |= 0x01;

    if((ucaDesTin[6]&0x20)!=0)  ucaDesTOut[4] |= 0x80;
    if((ucaDesTin[5]&0x08)!=0)  ucaDesTOut[4] |= 0x40;
    if((ucaDesTin[4]&0x80)!=0)  ucaDesTOut[4] |= 0x20;
    if((ucaDesTin[5]&0x01)!=0)  ucaDesTOut[4] |= 0x10;
    if((ucaDesTin[5]&0x10)!=0)  ucaDesTOut[4] |= 0x08;
    if((ucaDesTin[6]&0x80)!=0)  ucaDesTOut[4] |= 0x04;
    if((ucaDesTin[4]&0x02)!=0)  ucaDesTOut[4] |= 0x02;
    if((ucaDesTin[6]&0x01)!=0)  ucaDesTOut[4] |= 0x01;

    if((ucaDesTin[4]&0x40)!=0)  ucaDesTOut[5] |= 0x80;
    if((ucaDesTin[6]&0x08)!=0)  ucaDesTOut[5] |= 0x40;
    if((ucaDesTin[5]&0x04)!=0)  ucaDesTOut[5] |= 0x20;
    if((ucaDesTin[5]&0x40)!=0)  ucaDesTOut[5] |= 0x10;
    if((ucaDesTin[6]&0x40)!=0)  ucaDesTOut[5] |= 0x08;
    if((ucaDesTin[4]&0x10)!=0)  ucaDesTOut[5] |= 0x04;
    if((ucaDesTin[3]&0x08)!=0)  ucaDesTOut[5] |= 0x02;
    if((ucaDesTin[3]&0x01)!=0)  ucaDesTOut[5] |= 0x01;
    memcpy(tem_key,ucaDesTOut,6);
}

void  transposeP_Tr(uint8 *ucaDesx)
{//4->4
    memcpy(ucaDesTin,ucaDesx,4);
    memset((void *)ucaDesTOut, 0 , 4);

    if((ucaDesTin[1]&0x01)!=0)  ucaDesTOut[0] |= 0x80;
    if((ucaDesTin[0]&0x02)!=0)  ucaDesTOut[0] |= 0x40;
    if((ucaDesTin[2]&0x10)!=0)  ucaDesTOut[0] |= 0x20;
    if((ucaDesTin[2]&0x08)!=0)  ucaDesTOut[0] |= 0x10;

    if((ucaDesTin[3]&0x08)!=0)  ucaDesTOut[0] |= 0x08;
    if((ucaDesTin[1]&0x10)!=0)  ucaDesTOut[0] |= 0x04;
    if((ucaDesTin[3]&0x10)!=0)  ucaDesTOut[0] |= 0x02;
    if((ucaDesTin[2]&0x80)!=0)  ucaDesTOut[0] |= 0x01;

    if((ucaDesTin[0]&0x80)!=0)  ucaDesTOut[1] |= 0x80;
    if((ucaDesTin[1]&0x02)!=0)  ucaDesTOut[1] |= 0x40;
    if((ucaDesTin[2]&0x02)!=0)  ucaDesTOut[1] |= 0x20;
    if((ucaDesTin[3]&0x40)!=0)  ucaDesTOut[1] |= 0x10;

    if((ucaDesTin[0]&0x08)!=0)  ucaDesTOut[1] |= 0x08;
    if((ucaDesTin[2]&0x40)!=0)  ucaDesTOut[1] |= 0x04;
    if((ucaDesTin[3]&0x02)!=0)  ucaDesTOut[1] |= 0x02;
    if((ucaDesTin[1]&0x40)!=0)  ucaDesTOut[1] |= 0x01;

    if((ucaDesTin[0]&0x40)!=0)  ucaDesTOut[2] |= 0x80;
    if((ucaDesTin[0]&0x01)!=0)  ucaDesTOut[2] |= 0x40;
    if((ucaDesTin[2]&0x01)!=0)  ucaDesTOut[2] |= 0x20;
    if((ucaDesTin[1]&0x04)!=0)  ucaDesTOut[2] |= 0x10;

    if((ucaDesTin[3]&0x01)!=0)  ucaDesTOut[2] |= 0x08;
    if((ucaDesTin[3]&0x20)!=0)  ucaDesTOut[2] |= 0x04;
    if((ucaDesTin[0]&0x20)!=0)  ucaDesTOut[2] |= 0x02;
    if((ucaDesTin[1]&0x80)!=0)  ucaDesTOut[2] |= 0x01;

    if((ucaDesTin[2]&0x20)!=0)  ucaDesTOut[3] |= 0x80;
    if((ucaDesTin[1]&0x08)!=0)  ucaDesTOut[3] |= 0x40;
    if((ucaDesTin[3]&0x04)!=0)  ucaDesTOut[3] |= 0x20;
    if((ucaDesTin[0]&0x04)!=0)  ucaDesTOut[3] |= 0x10;

    if((ucaDesTin[2]&0x04)!=0)  ucaDesTOut[3] |= 0x08;
    if((ucaDesTin[1]&0x20)!=0)  ucaDesTOut[3] |= 0x04;
    if((ucaDesTin[0]&0x10)!=0)  ucaDesTOut[3] |= 0x02;
    if((ucaDesTin[3]&0x80)!=0)  ucaDesTOut[3] |= 0x01;

    memcpy(ucaDesx,ucaDesTOut,4);
}

void    transposeIP_1_Tr (uint8 *pucRslt)
{
    uint8   ucWrt,ucRd,ucRdSb,ucRslt,ucBitMask;
    const   uint8 SBz1_8[8]= {3,7,2,6,1,5,0,4};

    memcpy(ucaDesTin,pucRslt,8);
    memset((void *)ucaDesTOut, 0 , 8);
#if 1 //zlq133
    for(ucWrt=0; ucWrt<8;ucWrt++)
    {
        ucRslt = 0;
        ucBitMask = 0x01 << ucWrt;
        for( ucRd= 0; ucRd < 8; ucRd++)
        {
            ucRdSb = SBz1_8[ucRd];
            if((ucaDesTin[ucRdSb] & ucBitMask) != 0)
                ucRslt +=  (0x01 << ucRd);
        }
        ucaDesTOut[ucWrt] = ucRslt;
    }
#else

    if((ucaDesTin[4]&0x01)!=0)  ucaDesTOut[0] |= 0x80;
    if((ucaDesTin[0]&0x01)!=0)  ucaDesTOut[0] |= 0x40;
    if((ucaDesTin[5]&0x01)!=0)  ucaDesTOut[0] |= 0x20;
    if((ucaDesTin[1]&0x01)!=0)  ucaDesTOut[0] |= 0x10;
    if((ucaDesTin[6]&0x01)!=0)  ucaDesTOut[0] |= 0x08;
    if((ucaDesTin[2]&0x01)!=0)  ucaDesTOut[0] |= 0x04;
    if((ucaDesTin[7]&0x01)!=0)  ucaDesTOut[0] |= 0x02;
    if((ucaDesTin[3]&0x01)!=0)  ucaDesTOut[0] |= 0x01;

    if((ucaDesTin[4]&0x02)!=0)  ucaDesTOut[1] |= 0x80;
    if((ucaDesTin[0]&0x02)!=0)  ucaDesTOut[1] |= 0x40;
    if((ucaDesTin[5]&0x02)!=0)  ucaDesTOut[1] |= 0x20;
    if((ucaDesTin[1]&0x02)!=0)  ucaDesTOut[1] |= 0x10;
    if((ucaDesTin[6]&0x02)!=0)  ucaDesTOut[1] |= 0x08;
    if((ucaDesTin[2]&0x02)!=0)  ucaDesTOut[1] |= 0x04;
    if((ucaDesTin[7]&0x02)!=0)  ucaDesTOut[1] |= 0x02;
    if((ucaDesTin[3]&0x02)!=0)  ucaDesTOut[1] |= 0x01;

    if((ucaDesTin[4]&0x04)!=0)  ucaDesTOut[2] |= 0x80;
    if((ucaDesTin[0]&0x04)!=0)  ucaDesTOut[2] |= 0x40;
    if((ucaDesTin[5]&0x04)!=0)  ucaDesTOut[2] |= 0x20;
    if((ucaDesTin[1]&0x04)!=0)  ucaDesTOut[2] |= 0x10;
    if((ucaDesTin[6]&0x04)!=0)  ucaDesTOut[2] |= 0x08;
    if((ucaDesTin[2]&0x04)!=0)  ucaDesTOut[2] |= 0x04;
    if((ucaDesTin[7]&0x04)!=0)  ucaDesTOut[2] |= 0x02;
    if((ucaDesTin[3]&0x04)!=0)  ucaDesTOut[2] |= 0x01;

    if((ucaDesTin[4]&0x08)!=0)  ucaDesTOut[3] |= 0x80;
    if((ucaDesTin[0]&0x08)!=0)  ucaDesTOut[3] |= 0x40;
    if((ucaDesTin[5]&0x08)!=0)  ucaDesTOut[3] |= 0x20;
    if((ucaDesTin[1]&0x08)!=0)  ucaDesTOut[3] |= 0x10;
    if((ucaDesTin[6]&0x08)!=0)  ucaDesTOut[3] |= 0x08;
    if((ucaDesTin[2]&0x08)!=0)  ucaDesTOut[3] |= 0x04;
    if((ucaDesTin[7]&0x08)!=0)  ucaDesTOut[3] |= 0x02;
    if((ucaDesTin[3]&0x08)!=0)  ucaDesTOut[3] |= 0x01;

    if((ucaDesTin[4]&0x10)!=0)  ucaDesTOut[4] |= 0x80;
    if((ucaDesTin[0]&0x10)!=0)  ucaDesTOut[4] |= 0x40;
    if((ucaDesTin[5]&0x10)!=0)  ucaDesTOut[4] |= 0x20;
    if((ucaDesTin[1]&0x10)!=0)  ucaDesTOut[4] |= 0x10;
    if((ucaDesTin[6]&0x10)!=0)  ucaDesTOut[4] |= 0x08;
    if((ucaDesTin[2]&0x10)!=0)  ucaDesTOut[4] |= 0x04;
    if((ucaDesTin[7]&0x10)!=0)  ucaDesTOut[4] |= 0x02;
    if((ucaDesTin[3]&0x10)!=0)  ucaDesTOut[4] |= 0x01;

    if((ucaDesTin[4]&0x20)!=0)  ucaDesTOut[5] |= 0x80;
    if((ucaDesTin[0]&0x20)!=0)  ucaDesTOut[5] |= 0x40;
    if((ucaDesTin[5]&0x20)!=0)  ucaDesTOut[5] |= 0x20;
    if((ucaDesTin[1]&0x20)!=0)  ucaDesTOut[5] |= 0x10;
    if((ucaDesTin[6]&0x20)!=0)  ucaDesTOut[5] |= 0x08;
    if((ucaDesTin[2]&0x20)!=0)  ucaDesTOut[5] |= 0x04;
    if((ucaDesTin[7]&0x20)!=0)  ucaDesTOut[5] |= 0x02;
    if((ucaDesTin[3]&0x20)!=0)  ucaDesTOut[5] |= 0x01;

    if((ucaDesTin[4]&0x40)!=0)  ucaDesTOut[6] |= 0x80;
    if((ucaDesTin[0]&0x40)!=0)  ucaDesTOut[6] |= 0x40;
    if((ucaDesTin[5]&0x40)!=0)  ucaDesTOut[6] |= 0x20;
    if((ucaDesTin[1]&0x40)!=0)  ucaDesTOut[6] |= 0x10;
    if((ucaDesTin[6]&0x40)!=0)  ucaDesTOut[6] |= 0x08;
    if((ucaDesTin[2]&0x40)!=0)  ucaDesTOut[6] |= 0x04;
    if((ucaDesTin[7]&0x40)!=0)  ucaDesTOut[6] |= 0x02;
    if((ucaDesTin[3]&0x40)!=0)  ucaDesTOut[6] |= 0x01;

    if((ucaDesTin[4]&0x80)!=0)  ucaDesTOut[7] |= 0x80;
    if((ucaDesTin[0]&0x80)!=0)  ucaDesTOut[7] |= 0x40;
    if((ucaDesTin[5]&0x80)!=0)  ucaDesTOut[7] |= 0x20;
    if((ucaDesTin[1]&0x80)!=0)  ucaDesTOut[7] |= 0x10;
    if((ucaDesTin[6]&0x80)!=0)  ucaDesTOut[7] |= 0x08;
    if((ucaDesTin[2]&0x80)!=0)  ucaDesTOut[7] |= 0x04;
    if((ucaDesTin[7]&0x80)!=0)  ucaDesTOut[7] |= 0x02;
    if((ucaDesTin[3]&0x80)!=0)  ucaDesTOut[7] |= 0x01;
#endif

    memcpy(pucRslt,ucaDesTOut,8);
}

void _vKeyMoveL(uint8 *ucaKeyT)//56个位的密钥,按28个一组循环左移
{
    memcpy(ucaDesTin,ucaKeyT,7);
    if((ucaDesTin[0] &0x80)==0) btCry1  = 0; else  btCry1  = 1;
    if((ucaDesTin[3] &0x08)==0) btCryC1 = 0; else  btCryC1 = 1;
    ucDesT3 = 6;//7个数字
    do{
        if((ucaDesTin[ucDesT3] &0x80)==0)
            btCryC2 = 0;
        else
            btCryC2 = 1;
        ucaDesTin[ucDesT3] = ucaDesTin[ucDesT3]<<1;
        if(btCryC1 != 0)
            ucaDesTin[ucDesT3]++;
        btCryC1 = btCryC2;
    }while(ucDesT3-- !=0);
    ucaDesTin[3] &= 0xef;
    if(btCry1 != 0)
        ucaDesTin[3] |= 0x10;
    memcpy(ucaKeyT,ucaDesTin,7);
}

void _vKeyMoveR(uint8 *ucaKeyT)//56个位的密钥,按28个一组循环左移
{
    memcpy(ucaDesTin,ucaKeyT,7);
    if((ucaDesTin[6] &0x01)==0)btCry1 = 0; else  btCry1 = 1;
    if((ucaDesTin[3] &0x10)==0)btCryC1= 0; else  btCryC1 = 1;
    for(ucDesT3=0;ucDesT3<7;ucDesT3++)
    {
        if((ucaDesTin[ucDesT3] &0x01)==0)btCryC2 = 0; else  btCryC2 = 1;
        ucaDesTin[ucDesT3] = ucaDesTin[ucDesT3]>>1;
        if(btCryC1 != 0)
            ucaDesTin[ucDesT3] |= 0x80;
        btCryC1 = btCryC2;
    }
    ucaDesTin[3] &= 0xF7;
    if(btCry1 != 0)
        ucaDesTin[3] |= 0x08;
    memcpy(ucaKeyT,ucaDesTin,7);
}

void   vDes(uint8 mode, uint8 * pucData, uint8 * pucKey, uint8 * pucRslt)
{
    //KicKLazeDog();

    memcpy(ucaKeyT ,pucKey,8);      //使用缓冲密钥
    memcpy(pucRslt,pucData,8);            //原串到结果
    transposeIPTR((uint8 *)pucRslt);       //先转换结果中的原始数据
    transposePC_1_Tr((uint8 *)ucaKeyT); //转换密钥
    for (ucDesT1=0; ucDesT1<16; ucDesT1++)
    {//16次循环
        memcpy(ucaDesb,pucRslt,8);          //结果到缓冲区保存
        for (ucDesT2=0; ucDesT2<4; ucDesT2++)
            pucRslt[ucDesT2] = ucaDesb[ucDesT2+4];      //使用后半部分
        memcpy((void *)tem_ri,(void *)pucRslt,8);//输入到缓冲区
        transposetem_ri((uint8 *)tem_ri);
        //转换密钥
        if(mode == ENCRYPT)
        {
            ucDesT6 = Left_move[ucDesT1];
            for (ucDesT4=1;ucDesT4<=ucDesT6;ucDesT4++)
                _vKeyMoveL(ucaKeyT);
        }
        else
        {
            ucDesT6 = right_move[ucDesT1];
            for (ucDesT4=1;ucDesT4<=ucDesT6;ucDesT4++)
                _vKeyMoveR(ucaKeyT);
        }
        memcpy((void *)tem_key,(void *)ucaKeyT,8);//密钥放置到临时密钥
        transposePC_2_Tr((uint8 *)tem_key);         //临时密钥转换
        for (ucDesT4=0;ucDesT4<6;ucDesT4++) //临时数据和临时密钥异或-> 临时数据 (48)个
            tem_ri[ucDesT4] ^= tem_key[ucDesT4];
        memcpy(ucaDesTin,tem_ri,8);

        ucDesT7 = ucaDesTin[0];//&0xfc  //最高 6 位
        ucDesT5 = 0;//xxxx xx00  80 04 40 20 10 08
        if((ucDesT7&0x80) != 0) ucDesT5 |= 0x20;
        if((ucDesT7&0x04) != 0) ucDesT5 |= 0x10;
        if((ucDesT7&0x40) != 0) ucDesT5 |= 0x08;
        if((ucDesT7&0x20) != 0) ucDesT5 |= 0x04;
        if((ucDesT7&0x10) != 0) ucDesT5 |= 0x02;
        if((ucDesT7&0x08) != 0) ucDesT5 |= 0x01;
        ucaDesTOut[0] = SB1[ucDesT5] << 4;

        ucDesT7 = (ucaDesTin[0]&0x03) |(ucaDesTin[1]&0xF0);//&0xfc  //最高 6 位
        ucDesT5 = 0;//llll 00hh  02 10 01 80 40 20
        if((ucDesT7&0x02) != 0) ucDesT5 |= 0x20;
        if((ucDesT7&0x10) != 0) ucDesT5 |= 0x10;
        if((ucDesT7&0x01) != 0) ucDesT5 |= 0x08;
        if((ucDesT7&0x80) != 0) ucDesT5 |= 0x04;
        if((ucDesT7&0x40) != 0) ucDesT5 |= 0x02;
        if((ucDesT7&0x20) != 0) ucDesT5 |= 0x01;
        ucaDesTOut[0] |= SB2[ucDesT5];
        ucDesT7 = (ucaDesTin[1]&0x0F) |(ucaDesTin[2]&0xC0);//&0xfc  //最高 6 位
        ucDesT5 = 0;//ll00 hhhh  08 40 04 02 01 80
        if((ucDesT7&0x08) != 0) ucDesT5 |= 0x20;
        if((ucDesT7&0x40) != 0) ucDesT5 |= 0x10;
        if((ucDesT7&0x04) != 0) ucDesT5 |= 0x08;
        if((ucDesT7&0x02) != 0) ucDesT5 |= 0x04;
        if((ucDesT7&0x01) != 0) ucDesT5 |= 0x02;
        if((ucDesT7&0x80) != 0) ucDesT5 |= 0x01;
        ucaDesTOut[1] = SB3[ucDesT5] << 4;
        ucDesT7 = ucaDesTin[2];
        ucDesT5 = 0;//00xx xxxx  20 01 10 08 04 02
        if((ucDesT7&0x20) != 0) ucDesT5 |= 0x20;
        if((ucDesT7&0x01) != 0) ucDesT5 |= 0x10;
        if((ucDesT7&0x10) != 0) ucDesT5 |= 0x08;
        if((ucDesT7&0x08) != 0) ucDesT5 |= 0x04;
        if((ucDesT7&0x04) != 0) ucDesT5 |= 0x02;
        if((ucDesT7&0x02) != 0) ucDesT5 |= 0x01;
        ucaDesTOut[1] |= SB4[ucDesT5];
        ucDesT7 = ucaDesTin[3];//&0xfc  //最高 6 位
        ucDesT5 = 0;//xxxx xx00  80 04 40 20 10 08
        if((ucDesT7&0x80) != 0) ucDesT5 |= 0x20;
        if((ucDesT7&0x04) != 0) ucDesT5 |= 0x10;
        if((ucDesT7&0x40) != 0) ucDesT5 |= 0x08;
        if((ucDesT7&0x20) != 0) ucDesT5 |= 0x04;
        if((ucDesT7&0x10) != 0) ucDesT5 |= 0x02;
        if((ucDesT7&0x08) != 0) ucDesT5 |= 0x01;
        ucaDesTOut[2] = SB5[ucDesT5] << 4;
        ucDesT7 = (ucaDesTin[3]&0x03) |(ucaDesTin[4]&0xF0);//&0xfc  //最高 6 位
        ucDesT5 = 0;//llll 00hh  02 10 01 80 40 20
        if((ucDesT7&0x02) != 0) ucDesT5 |= 0x20;
        if((ucDesT7&0x10) != 0) ucDesT5 |= 0x10;
        if((ucDesT7&0x01) != 0) ucDesT5 |= 0x08;
        if((ucDesT7&0x80) != 0) ucDesT5 |= 0x04;
        if((ucDesT7&0x40) != 0) ucDesT5 |= 0x02;
        if((ucDesT7&0x20) != 0) ucDesT5 |= 0x01;
        ucaDesTOut[2] |= SB6[ucDesT5];
        ucDesT7 = (ucaDesTin[4]&0x0F) |(ucaDesTin[5]&0xC0);//&0xfc  //最高 6 位
        ucDesT5 = 0;//ll00 hhhh  08 40 04 02 01 80
        if((ucDesT7&0x08) != 0) ucDesT5 |= 0x20;
        if((ucDesT7&0x40) != 0) ucDesT5 |= 0x10;
        if((ucDesT7&0x04) != 0) ucDesT5 |= 0x08;
        if((ucDesT7&0x02) != 0) ucDesT5 |= 0x04;
        if((ucDesT7&0x01) != 0) ucDesT5 |= 0x02;
        if((ucDesT7&0x80) != 0) ucDesT5 |= 0x01;
        ucaDesTOut[3] = SB7[ucDesT5] << 4;
        ucDesT7 = ucaDesTin[5];
        ucDesT5 = 0;//00xx xxxx  20 01 10 08 04 02
        if((ucDesT7&0x20) != 0) ucDesT5 |= 0x20;
        if((ucDesT7&0x01) != 0) ucDesT5 |= 0x10;
        if((ucDesT7&0x10) != 0) ucDesT5 |= 0x08;
        if((ucDesT7&0x08) != 0) ucDesT5 |= 0x04;
        if((ucDesT7&0x04) != 0) ucDesT5 |= 0x02;
        if((ucDesT7&0x02) != 0) ucDesT5 |= 0x01;
        ucaDesTOut[3] |= SB8[ucDesT5];
        memcpy(ucaDesx,ucaDesTOut,8);

        transposeP_Tr((uint8 *)ucaDesx);  //再将结果转换
        for (ucDesT2=0; ucDesT2<4; ucDesT2++)
            pucRslt[ucDesT2+4] = ucaDesb[ucDesT2] ^ ucaDesx[ucDesT2];//后半部分再和结果比较.
    }
    memcpy(ucaDesTOut,pucRslt,4);
    memcpy(pucRslt,pucRslt+4,4);
    memcpy(pucRslt+4,ucaDesTOut,4);
    transposeIP_1_Tr ((uint8 *)pucRslt);

    //KicKLazeDog();
}

/*
void v3Des( uint8 ucMode,uint8 *DataIn,uint8 *DesKey,uint8 *DataOut )
{
    uint8 ucTemp1[8], ucTemp2[8];
    uint8 ucMode2;

    if (ucMode == ENCRYPT)
    {
        ucMode2 = DECRYPT;

        vDes(ucMode , DataIn , DesKey,      ucTemp1);
        vDes(ucMode2, ucTemp1, DesKey + 8,  ucTemp2);
        vDes(ucMode,  ucTemp2, DesKey + 16, DataOut);
    }
    else
    {
        ucMode2 = ENCRYPT;

        vDes(ucMode,  DataIn,  DesKey + 16, ucTemp1);
        vDes(ucMode2, ucTemp1, DesKey + 8,  ucTemp2);
        vDes(ucMode,  ucTemp2, DesKey,      DataOut);
    }
}
*/


void v3Des( uint8 ucMode,uint8 *DataIn,uint8 *DesKey,uint8 *DataOut )
{
    uint8 ucTemp1[8], ucTemp2[8];
    uint8 ucMode2;

    if (ucMode == ENCRYPT)
    {
        ucMode2 = DECRYPT;

        vDes(ucMode , DataIn , DesKey,      ucTemp1);
        vDes(ucMode2, ucTemp1, DesKey + 8,  ucTemp2);
        vDes(ucMode,  ucTemp2, DesKey, DataOut);
    }
    else
    {
        ucMode2 = ENCRYPT;

        vDes(ucMode,  DataIn,  DesKey, ucTemp1);
        vDes(ucMode2, ucTemp1, DesKey + 8,  ucTemp2);
        vDes(ucMode,  ucTemp2, DesKey,      DataOut);
    }
}


#endif


