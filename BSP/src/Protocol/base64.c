#include "base64.h"


const char * base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void base64_encode1( unsigned char * bindata, unsigned char * base64, int binlength )
{
    int i, j;
    unsigned char current;

    for ( i = 0, j = 0 ; i < binlength ; i += 3 )
    {
        current = (bindata[i] >> 2) ;
        current &= (unsigned char)0x3F;
        base64[j++] = base64char[(int)current];

        current = ( (unsigned char)(bindata[i] << 4 ) ) & ( (unsigned char)0x30 ) ;
        if ( i + 1 >= binlength )
        {
            base64[j++] = base64char[(int)current];
            base64[j++] = '=';
            base64[j++] = '=';
            break;
        }
        current |= ( (unsigned char)(bindata[i+1] >> 4) ) & ( (unsigned char) 0x0F );
        base64[j++] = base64char[(int)current];

        current = ( (unsigned char)(bindata[i+1] << 2) ) & ( (unsigned char)0x3C ) ;
        if ( i + 2 >= binlength )
        {
            base64[j++] = base64char[(int)current];
            base64[j++] = '=';
            break;
        }
        current |= ( (unsigned char)(bindata[i+2] >> 6) ) & ( (unsigned char) 0x03 );
        base64[j++] = base64char[(int)current];

        current = ( (unsigned char)bindata[i+2] ) & ( (unsigned char)0x3F ) ;
        base64[j++] = base64char[(int)current];
    }
    base64[j] = '\0';
    //return base64;
}



void base64_encode(unsigned char *s,unsigned char *EncodeDataBuff)
{
	unsigned char *p = s, *e, *r;
	int len = strlen((char *)s);
	unsigned char unit[4], temp[3];
	unsigned flag = 0;
	
	e = s + len;
	len = len / 3 * 4 + 4 + 1;
	//r = _ret = (unsigned char *)malloc(len);
	r  =EncodeDataBuff;
	
	while (p < e) 
	{
		memset(temp,0,3);
		if (e-p >= 3) 
		{
			memcpy(temp,p,3);
			p += 3;
			flag = 1;
		} 
		else 
		{
			memcpy(temp,p,e-p);
			flag = 0;
			p = e;
		}
		
		unit[0] = temp[0] >> 2;   //0x30  -> 0x06
		unit[1] = temp[0] << 6;   
		unit[1] = (unit[1]>>2) | (temp[1]>>4); //0x03
		unit[2] = temp[1] << 4;
		unit[2] = (unit[2]>>2) | (temp[2]>>6);
		unit[3] = temp[2] << 2;
		unit[3] = unit[3] >> 2;
		*r++ = B64[unit[0]];
		*r++ = B64[unit[1]];
		if (flag == 1)
		{
		    *r++ = B64[unit[2]];
		    *r++ = B64[unit[3]];
		}
		else
	  {
		    *r++ = (unit[2] ? B64[unit[2]] : '=');
		    *r++ = (unit[3] ? B64[unit[3]] : '=');
		}
	}
	*r = 0;

	/*
	#if RETURNMALLOC == 0
	strcpy(s,_ret);
	free(_ret);
	_ret = s;
	#endif //RETURNMALLOC
	*/
	//return _ret;
}

void base64_encode_with_len(unsigned char *s,unsigned char *EncodeDataBuff,unsigned int len)
{
	unsigned char *p = s, *e, *r;
	unsigned char unit[4], temp[3];
	unsigned flag = 0;
	
	e = s + len;
	len = len / 3 * 4 + 4 + 1;
	//r = _ret = (unsigned char *)malloc(len);
	r  =EncodeDataBuff;
	
	while (p < e) 
	{
		memset(temp,0,3);
		if (e-p >= 3) 
		{
			memcpy(temp,p,3);
			p += 3;
			flag = 1;
		} 
		else 
		{
			memcpy(temp,p,e-p);
			flag = 0;
			p = e;
		}
		
		unit[0] = temp[0] >> 2;   //0x30  -> 0x06
		unit[1] = temp[0] << 6;   
		unit[1] = (unit[1]>>2) | (temp[1]>>4); //0x03
		unit[2] = temp[1] << 4;
		unit[2] = (unit[2]>>2) | (temp[2]>>6);
		unit[3] = temp[2] << 2;
		unit[3] = unit[3] >> 2;
		*r++ = B64[unit[0]];
		*r++ = B64[unit[1]];
		if (flag == 1)
		{
		    *r++ = B64[unit[2]];
		    *r++ = B64[unit[3]];
		}
		else
	  {
		    *r++ = (unit[2] ? B64[unit[2]] : '=');
		    *r++ = (unit[3] ? B64[unit[3]] : '=');
		}
	}
	*r = 0;

	/*
	#if RETURNMALLOC == 0
	strcpy(s,_ret);
	free(_ret);
	_ret = s;
	#endif //RETURNMALLOC
	*/
	//return _ret;
}


unsigned char *base64_decode(unsigned char *s)
{
	unsigned char *p = s, *e, *r, *_ret;
	int len = strlen((char *)s);
	unsigned char i, unit[4];
	
	e = s + len;
	len = len / 4 * 3 + 1;
	r = _ret = (unsigned char *)malloc(len);
	
	while (p < e) 
	{
		memcpy(unit,p,4);
		if (unit[3] == '=')
			unit[3] = 0;
		if (unit[2] == '=')
			unit[2] = 0;
		p += 4;
		
		for (i=0; unit[0]!=B64[i] && i<64; i++);
		unit[0] = i==64 ? 0 : i;
		for (i=0; unit[1]!=B64[i] && i<64; i++);
		unit[1] = i==64 ? 0 : i;
		for (i=0; unit[2]!=B64[i] && i<64; i++);
		unit[2] = i==64 ? 0 : i;
		for (i=0; unit[3]!=B64[i] && i<64; i++);
		unit[3] = i==64 ? 0 : i;
		*r++ = (unit[0]<<2) | (unit[1]>>4);
		*r++ = (unit[1]<<4) | (unit[2]>>2);
		*r++ = (unit[2]<<6) | unit[3];
	}
	*r = 0;
	

	
	return _ret;
}









