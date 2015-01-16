#ifndef _UNICODE_H_
#define _UNICODE_H_

#define Unicode_Max   6768+106

struct Uni_Gb2312 
{
	char unicode[4+1];			//unicode
	char gbk[4+1];				//gb2312
};

extern struct Uni_Gb2312 uni_table[];

#endif
