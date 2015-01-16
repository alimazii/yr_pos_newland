#include "md5.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "aliqr.h"


//char* jfsubject = "%E5%88%86%E8%B4%A6%E6%B5%8B%E8%AF%95-sky";
int alipay_precreate(char* precr, int* len, struct payInfo* order_info,int type)
{
    
	char https[1024];
	char encrypt[1024];
	char common[1024];
	md5_state_t state;
	md5_byte_t digest[16];
	char hex_output[16*2 + 1];
	int di;
#if 1
#else
    *len = sprintf(common,PREQUERY );
#endif
    switch (type)
    {
        case ALI_PREORDER:
            *len = sprintf(common,PREORDER);
            break;
        case ALI_EXCHANGE:
        case ALI_EXCHANGEORDER:
        case ALI_QUERY_24H: /* query the recent 24h */
            *len = sprintf(common,PREIMSI);
            break;
        case ALI_QUERY_TIMEMARK: /* query the payment status of order with timemask */
            *len = sprintf(common,PREQUERYTIMEMASK);
            break;
        case ALI_QUERY_MAXTIME: /* query the payment status of order with timemask */
            *len = sprintf(common,PREQUERYMAXTIME);
            break;
        case ALI_VIEW_SINGLE: /* query the with serial no */
            *len = sprintf(common,PREVIEW);
            break;
        case ALI_REFUND:
            *len = sprintf(common,PREREFUND);
            break;
        case ALI_TEMPLATEMD5:
        case ALI_TEMPLATE:
        case ALI_LASTESTMD5:
        case ALI_LASTEST:
            //no sign needed direct return
            *len = sprintf(common,PREIMSI);
            memset(precr, 0, *len+1);
            memcpy(precr, common, *len);
            return *len;
            //break;
        default:
            *len = sprintf(common,PREIMSI);
            break;
    }

    strcpy(encrypt,common);
    strcat(encrypt,"#");
	printf("order_info->order_key=%s,strlen:%d\n",order_info->order_key,strlen(order_info->order_key));
    if(strlen(order_info->order_key) <= 0)
        strcpy(order_info->order_key,ORDERKEY);
    strcat(encrypt,order_info->order_key);
	printf("\nMD5 input:encrypt=%s\n", encrypt);

	md5_init(&state);
	md5_append(&state, (const md5_byte_t *)encrypt, strlen(encrypt));
	md5_finish(&state, digest);
	for (di = 0; di < 16; ++di)
	    sprintf(hex_output + di * 2, "%02x", digest[di]);
	//printf("\nencrypt output:");
	//puts(hex_output);
    //printf("digest:%s\n",digest);
	*len = sprintf(https,"%s&s=%s", common,hex_output);
	//puts(https);
	memset(precr, 0, *len+1);
	memcpy(precr, https, *len);
	return *len;
}

char* alipay_postcreate(int type)
{
    switch (type)
    {
        case ALI_PREORDER:
            return POSTPREORDER;
        case ALI_EXCHANGE:
            return POSTEXCHANGE;
        case ALI_EXCHANGEORDER:
            return POSTEXCHANGEORDER;
        case ALI_QUERY_TIMEMARK: /* query the payment status of order with timemask */
        case ALI_QUERY_MAXTIME: /* query the payment status of order with timemask */
        case ALI_QUERY_24H: /* query the recent 24h */
            return POSTQUERY;
        case ALI_VIEW_SINGLE: /* query the with serial no */
            return POSTVIEW;
        case ALI_REFUND:
            return POSTREFUND;
        case ALI_TEMPLATEMD5:
            return POSTTEMPLATEMD5;
        case ALI_TEMPLATE:
            return POSTTEMPLATE;
        case ALI_LASTESTMD5:
            return POSTLATESTMD5;
        case ALI_LASTEST:
            return POSTLATEST;
        default:
            break;
    }
    return 0;
}
