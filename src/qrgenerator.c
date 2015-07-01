#include "aliqr.h"
#include "qrgenerator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <time.h>

#include "NDK.h"
#include "QR_Encode.h"


struct payInfo qrpay_info;
unsigned long long query_number = 0;
unsigned long long query_number_idx = 1;
unsigned long long old_query_number = 1;
char pos_imsi[20];
char jfkey[32+1] = {0};
char pos_ver[16+1] = {0};

void getIMSIconfig();
char szQrcodeString[QRRESULT] = {0};

//char* subject = "Alipay";
char subject[128+1] = {0};
char* defaultsubject = "e-richpay";
int getsubject(char* name,char* buf);
#ifdef BAIDU_EN
extern int payment_channel;
#endif

#if 0
int main(int argc, char** argv)
{
    int precreate_type = 1;//ALI_EXCHANGE  
    if(argc >=2)
        precreate_type = atoi(argv[1]);
    
    strcpy(qrpay_info.order_key,"11");
    switch (precreate_type)
    {
        case ALI_PREORDER:
            if(argc == 4) {
                query_number = atoll(argv[2]);
                generator_qrcode_to_bmp(-1, argv[3]);
            }
            break;
        case ALI_EXCHANGE:
        case ALI_EXCHANGEORDER:
        case ALI_TEMPLATEMD5:
        case ALI_TEMPLATE:
        case ALI_LASTESTMD5:
        case ALI_LASTEST:
        case ALI_QUERY_24H: /* query the recent 24h */
            preImsi(precreate_type);
            break;
        case ALI_QUERY_TIMEMARK: /* query the payment status of order with timemask */
            //*len = sprintf(common,PREQUERYTIMEMASK);
            if(argc == 3) {
                qTimemark(argv[2]);
            }
            break;
        case ALI_QUERY_MAXTIME: /* query the payment status of order with timemask */
            if(argc == 3) {
                qMaxtime(atoi(argv[2]));
            }
            break;
        case ALI_VIEW_SINGLE: /* query the with serial no */
            if(argc == 3) {
                query_number = atoll(argv[2]);
                viewsingle(0);
            }
            break;
        case ALI_REFUND:
            if(argc == 4) {
                query_number = atoll(argv[2]);
                createrefund(argv[2], argv[3] );
            }
            break;
        default:
        //    *len = sprintf(common,PREIMSI);
            break;
    }

    return 0;
    
}
#endif

int generator_qrcode_to_bmp(void* gout, char* price ,void* gin)
{
    DataInfo qrDataInfo;
    BmpInfo qrBmpBuff;
    	
    char* szSourceString = NULL;
    // code from d620d start
    int ret;
    int enc_mode;
    struct qr_result* out = (struct qr_result*)gout; 
    struct payInfo* in = (struct payInfo*)gin; 
    // code from d620d end

    int i;

    struct tm *ptr;
    time_t td;
    char ticket_number[13] = {0};
    char client_number[21] = {0};
    char order_time[15] = {0};
    client_number[0] = '1';//to avoid 0 atoi bug
    DebugErrorInfo("Get Before getIMSIconfig\n");
    getIMSIconfig();
    if(jfkey[0] == 0 && getPosKey() > 0)
         return 1;
    strcpy(qrpay_info.order_key,jfkey);

    /* Time for Normal Platform */
    time(&td);
    ptr = (struct tm *)localtime(&td);
    //if(query_number == 0) { //if query_number != 0 then time will nto changed, bug
        strftime(ticket_number,sizeof(ticket_number),"%y%m%d%H%M00",ptr);
        //sprintf(ticket_number,"%s%s%s%s%s00","14","10","10","10","10");
        /* use last 4-bit of IMSI */
        strncpy(client_number+1, &(qrpay_info.imsi[11]), 5);
        strcat(client_number, ticket_number);       
        query_number = (unsigned long long)atoll(client_number);
        if(old_query_number == query_number/100 ) {
            query_number = query_number + query_number_idx;
            query_number_idx++;
        } else {
            query_number_idx = 1;
            old_query_number = query_number/100; 
        }
    //}
    qrpay_info.order_number = query_number;    

    strcpy(qrpay_info.total_fee,price);
    //strcpy(qrpay_info.total_fee,"0.01");^M
    //strcpy(qrpay_info.order_subject,"ccc");
    memset(qrpay_info.order_subject,0, sizeof(qrpay_info.order_subject));
    if(in && strlen(in->order_subject) > 0)
        strcpy(qrpay_info.order_subject,in->order_subject);
#if 0        
    else {
        int subjectlen = 0;
        subjectlen = getsubject("/usr/local/D620D/subject.txt",subject);
        if(subjectlen > 0 ) {
            printf("subject:%d:%s",subjectlen,subject);
            strncpy(qrpay_info.order_subject,subject,subjectlen);
        }else{
            strcpy(qrpay_info.order_subject,defaultsubject);
        }
    }
#endif
    else    
    	  strcpy(qrpay_info.order_subject,defaultsubject); /* temp solution for newland */
    //strcpy(qrpay_info.order_subject,"%E5%88%86%E8%B4%A6%E6%B5%8B%E8%AF%95-sky");


    strftime(order_time,sizeof(order_time),"%Y%m%d%H%M%S",ptr);        
    strcpy(qrpay_info.order_time,order_time);

    memset(szQrcodeString, 0,sizeof(szQrcodeString)); 
    /* print the qr code from alipay */
    DebugErrorInfo("Get Before alipay_main,imsi is %s\n",qrpay_info.imsi);
    #ifdef BAIDU_EN
    if(payment_channel == 1)
    	strncpy(qrpay_info.pay_channel, "bai", 3);
    else if (payment_channel == 2) 
      strncpy(qrpay_info.pay_channel, "wei", 3);
    else
    	strncpy(qrpay_info.pay_channel, "ali", 3);	
    #endif
    alipay_main(out, &qrpay_info, ALI_PREORDER);
    szSourceString = szQrcodeString;

    //if(strstr(out->qrcode, "https://qr.alipay.com/")) {
    //strcpy(out->qrcode,"https://www.baifubao.com/o2o/0/s/0?tinyurl=cskUF5");
    if(strlen(out->qrcode) > 0) {  
        /* print QR code on D620D */
        //ret = PrintQR(10, 1, 2, szSourceString, 5, 5);
        //ret = PrintQR(6, 1, 2, szSourceString, 5, 7);
        ////ret = PrintQR(6, 3, 2, out->qrcode, 50, 7);
        qrDataInfo.nLen = strlen(out->qrcode);
        qrDataInfo.nLevel = QR_LEVEL_H;
        qrDataInfo.nMask = -1; /* default mask value */
        qrDataInfo.nVersion = QR_VRESION_S;
        strcpy(qrDataInfo.szInputData,out->qrcode);
        /* firstly created bmp buffer with qr string from alipay */
        EncodeDataAndGenerateBmp(&qrDataInfo, &qrBmpBuff);
        ret = NDK_PrnInit(0);
        if(ret != NDK_OK)
        	  return ret;
        NDK_PrnImage(qrBmpBuff.xsize, qrBmpBuff.ysize, 110, qrBmpBuff.bmpbuff); /* offset to 110 pixel */	
        ret = NDK_PrnStart(); 
        printf("qrcode:%s\n",out->qrcode);
        DebugErrorInfo("qrcode:%s\n",out->qrcode);

        if (0 > ret)
        {
            DebugErrorInfo("the PrintQR return value is %d\n",ret);
        }
    } else {
        ret = 1;
    }
    return ret;
}

int preImsi(void* gout, int precreate_type)
{

    struct qr_result* out = (struct qr_result*)gout; 
    getIMSIconfig();
    if(jfkey[0] == 0 && getPosKey() > 0)
         return 1;
    strcpy(qrpay_info.order_key,jfkey);
    alipay_main(out, &qrpay_info, precreate_type );

}
#if 1
int qTimemark(void* gout, char* time_mark)
{

    struct qr_result* out = (struct qr_result*)gout; 
    getIMSIconfig();
    strcpy(qrpay_info.time_mark, time_mark);
    memset(szQrcodeString, 0,sizeof(szQrcodeString)); 
    /* print the qr code from alipay */
    alipay_main((struct qr_result*)szQrcodeString, &qrpay_info, ALI_QUERY_TIMEMARK);

}
int qMaxtime(void* gout, int max_time)
{

    struct qr_result* out = (struct qr_result*)gout; 
    getIMSIconfig();
    qrpay_info.max_time = max_time;
    memset(szQrcodeString, 0,sizeof(szQrcodeString)); 
    /* print the qr code from alipay */
    alipay_main((struct qr_result*)szQrcodeString, &qrpay_info, ALI_QUERY_MAXTIME);

}
#endif
int createrefund(void* gout, char* serial_number, char* refund_amount )
{

    struct qr_result* out = (struct qr_result*)gout; 
    getIMSIconfig();
    if(jfkey[0] == 0 && getPosKey() > 0)
         return 1;
    strcpy(qrpay_info.order_key,jfkey);
    //qrpay_info.order_number = query_number;
    qrpay_info.order_number = atoll(serial_number);
    //strcpy(qrpay_info.refund_amount, refund_amount);
    //memset(szQrcodeString, 0,sizeof(szQrcodeString)); 
    /* print the qr code from alipay */
    /* support partial refund */
    strcpy(qrpay_info.refund_amount, refund_amount);
    alipay_main(out, &qrpay_info, ALI_REFUND);
    if(out->is_success == 'T' && strcmp(out->total_status,"TRADE_SUCCESS") == 0)
        return 1;
    else
        return 0;    

}

int viewsingle(void* gout,char* serial_number)
{
    struct qr_result* out = (struct qr_result*)gout; 
    getIMSIconfig();
    qrpay_info.order_number = atoll(serial_number);
    /* print the qr code from alipay */
    alipay_main(out, &qrpay_info, ALI_VIEW_SINGLE);
    if(out->is_success == 'T' && strcmp(out->total_status,"TRADE_SUCCESS") == 0)
        return 1;
    else
        return 0;
}

#if 1
//user don't need to input imsi and year month date, but hour,minutes,and serial no is needed,
void getSNoPre(char* prefix_str)
{
    struct tm *ptr;
    time_t td;    
    char ticket_number[13]={0};
    char client_number[21]={0};
    getIMSIconfig();
    memset(ticket_number, 0, 13);
    memset(client_number, 0, 21);
    client_number[0] = '1'; //to avoid atoi bug
    time(&td);
    ptr = (struct tm *)localtime(&td);    
    strftime(ticket_number,sizeof(ticket_number),"%y%m%d\0",ptr);    

    /* use last 4-bit of IMSI */
    strncpy(client_number+1, &(qrpay_info.imsi[11]), 5);
    strcat(client_number, ticket_number);
    memcpy(prefix_str, client_number,strlen(client_number));
    printf("ticket_number:%s, client_number:%s, prefix:%s\n",
            ticket_number, client_number, prefix_str);
}
#endif

int getsubject(char* name,char* buf)
{
    FILE* fp = NULL;
    int len = 0;
    fp = fopen(name,"r");
    if(fp == NULL) {
        printf("couldn't open config.txt\n");
        return 0; 
    }
    
    fread(buf, sizeof(qrpay_info.order_subject),1,fp);
    printf("%d,%s\n",strlen(buf),buf);
    fclose(fp);
    return strlen(buf)-1;
}

void getIMSIconfig()
{
#if 1	
    FILE *fp;
    int i, ret;
    char buffer[30];
    
    ret = NDK_WlGetInfo(WLM_INFO_IMSI, &pos_imsi, 20);
    if (ret == NDK_OK) 
    		DebugErrorInfo("the pos imsi from SIM is %s\n",pos_imsi);

    if (pos_imsi[0] == '\0'){

        /* get imsi from config.txt */
        fp = fopen("config.txt","r");
        if(fp == NULL)
        {
            DebugErrorInfo("couldn't open config.txt\n");
            return;
        }
        if( fgets(buffer, 30, fp) == NULL )
        {
            DebugErrorInfo("Error reading config\n");
            fclose(fp);
            return ;
        }
        for (i=0; i<30; i++) {
            if(buffer[i] == '\n' || buffer[i] == '\r') {
                buffer[i] = '\0';
                break;
            }
        }

        fclose(fp);
        /* copy after IMSI: */
        strcpy(pos_imsi,&buffer[5]);
        DebugErrorInfo("the pos imsi buffer string is %s\n",pos_imsi);
    }

    strcpy(qrpay_info.imsi,pos_imsi);
#endif 
    //strcpy(pos_imsi, "460040001805635");
    //strcpy(qrpay_info.imsi,"460040001805635"); /* temp solution for newland */    
}
#if 0

int alipay_query_single(unsigned long long queryNo)
{
    int ret = 0;
    getIMSIconfig();
    strcpy(qrpay_info.order_key,"11");
    qrpay_info.order_number = queryNo;
    memset(szQrcodeString, 0,sizeof(szQrcodeString)); 
    alipay_main((struct qr_result*)szQrcodeString, &qrpay_info, ALI_PRECREATE_QUERY_SINGLE);
    if(strcmp("TRADE_SUCCESS", szQrcodeString) == 0)
        ret = 1;
    else
        ret = 0;
    return ret;
}

int alipay_query_24h(char* query_24h)
{
    int ret = 0;
    getIMSIconfig();
    strcpy(qrpay_info.order_key,"11");
    alipay_main((struct qr_result*)query_24h, &qrpay_info, ALI_PRECREATE_QUERY_24H);
    return ret;
}
#endif

int getPosKey()
{
#ifdef CONFIG_INPUTKEY
        FILE *fp;
        int i;  

        /* get key from config.txt */
        fp = fopen("config.txt","r");
        if(fp == NULL)
        {       
            DebugErrorInfo("couldn't open config.txt in getPosKey\n");
            return 1; 
        }       
        fseek(fp, 22, SEEK_SET); /* 5 + 15 + 2 */  
        if( fgets(jfkey, sizeof(jfkey), fp) == NULL )
        {       
            DebugErrorInfo("Error reading jfkey in config\n");
            fclose(fp);
            return 1;
        }       
        for (i=0; i<sizeof(jfkey); i++) {
            if(jfkey[i] == '\n' || jfkey[i] == '\r') { 
                jfkey[i] = '\0'; 
                break;  
            }       
        }       

        fclose(fp);
        //memfrob(jfkey, strlen(jfkey));
#else
        strcpy(jfkey,"11"); /* temp solution for newland */
#endif        
        return 0;

}

int getPosVer()
{

        FILE *fp;
        int i; 
        char buffer[30]; 

        /* get version from config.txt */
        fp = fopen("config.txt","r");
        if(fp == NULL)
        {       
            DebugErrorInfo("couldn't open config.txt in getPosVer\n");
            return 1; 
        }       
        fseek(fp, 26, SEEK_SET); /* 5 + 15 + 2 + 2 + 2 */  
        if( fgets(buffer, sizeof(buffer), fp) == NULL )
        {       
            DebugErrorInfo("Error reading pos sw version in config\n");
            fclose(fp);
            return 1;
        }       
        for (i=0; i<sizeof(buffer); i++) {
            if(buffer[i] == '\n' || buffer[i] == '\r') { 
                buffer[i] = '\0'; 
                break;  
            }       
        }       

        strcpy(pos_ver,&buffer[4]);
        
        fclose(fp);
       
        return 0;
}

#ifdef CONFIG_INPUTKEY
int setPosKey()
{
        FILE *fp;
        int i,ret,ucKey;  

        /* set key to config.txt */
        fp = fopen("config.txt","r+");
        if(fp == NULL)
        {       
            DebugErrorInfo("couldn't open config.txt in setPosKey\n");
            return 1; 
        }
        fseek(fp, 22, SEEK_SET); /* 5 + 15 + 2 */ 
        	  
        if(jfkey[0] == 0 && fgets(jfkey, sizeof(jfkey), fp) != NULL){
        	  
        	  for (i=0; i<sizeof(jfkey); i++) {
               if(jfkey[i] == '\n' || jfkey[i] == '\r') { 
                   jfkey[i] = '\0'; 
                   break;  
               }       
            }
            DebugErrorInfo("update reading key %s from config.txt\n",jfkey);
        }
        
        NDK_ScrClrs();
        NDK_ScrDispString(0, 0, "请输入密钥(2-32位):\n",0);
        NDK_ScrRefresh();    
        
        ret = NDK_KbGetInput(jfkey, 2, 32, NULL, INPUTDISP_OTHER, 0, INPUT_CONTRL_NOLIMIT_ERETURN);
		    if(ret == NDK_ERR)
			     return 1;
			  if (fputs(jfkey, fp) == EOF) 
			  {
            DebugErrorInfo("couldn't write key to config.txt\n");
            return 1; 			  	
			  } 
			  fclose(fp);
        NDK_ScrClrs();
        NDK_ScrDispString(24, 12, "密钥更新成功",0);
        NDK_ScrDispString(24, 24, "需要重启机器",0);
        NDK_ScrRefresh(); 
        
        NDK_KbGetCode(2, &ucKey);
        NDK_SysReboot();
        return 0;			   
}
#endif

#ifdef BARCODE_EN
int create_and_pay(void* gout, char* price, char* dynamic_id ,void* gin)
{
    DataInfo qrDataInfo;
    	
    // code from d620d start
    int ret;
    struct qr_result* out = (struct qr_result*)gout; 
    struct payInfo* in = (struct payInfo*)gin; 
    // code from d620d end

    int i;

    struct tm *ptr;
    time_t td;
    char ticket_number[13] = {0};
    char client_number[21] = {0};
    char order_time[15] = {0};
    client_number[0] = '1';//to avoid 0 atoi bug
    DebugErrorInfo("Get Before getIMSIconfig\n");
    getIMSIconfig();
    if(jfkey[0] == 0 && getPosKey() > 0)
         return 1;
    strcpy(qrpay_info.order_key,jfkey);

    /* Time for Normal Platform */
    time(&td);
    ptr = (struct tm *)localtime(&td);
    //if(query_number == 0) { //if query_number != 0 then time will nto changed, bug
        strftime(ticket_number,sizeof(ticket_number),"%y%m%d%H%M00",ptr);
        //sprintf(ticket_number,"%s%s%s%s%s00","14","10","10","10","10");
        /* use last 4-bit of IMSI */
        strncpy(client_number+1, &(qrpay_info.imsi[11]), 5);
        strcat(client_number, ticket_number);       
        query_number = (unsigned long long)atoll(client_number);
        if(old_query_number == query_number/100 ) {
            query_number = query_number + query_number_idx;
            query_number_idx++;
        } else {
            query_number_idx = 1;
            old_query_number = query_number/100; 
        }
    //}
    qrpay_info.order_number = query_number;    

    strcpy(qrpay_info.total_fee,price);
    strcpy(qrpay_info.dynamic_id,dynamic_id);
    //strcpy(qrpay_info.total_fee,"0.01");^M
    //strcpy(qrpay_info.order_subject,"ccc");
    memset(qrpay_info.order_subject,0, sizeof(qrpay_info.order_subject));
#if 0    
    if(in && strlen(in->order_subject) > 0)
        strcpy(qrpay_info.order_subject,in->order_subject);
        
    else {
        int subjectlen = 0;
        subjectlen = getsubject("/usr/local/D620D/subject.txt",subject);
        if(subjectlen > 0 ) {
            printf("subject:%d:%s",subjectlen,subject);
            strncpy(qrpay_info.order_subject,subject,subjectlen);
        }else{
            strcpy(qrpay_info.order_subject,defaultsubject);
        }
    }

#endif    	  
    strftime(ticket_number,sizeof(ticket_number),"%Y%m%d%H%M00",ptr);
    sprintf(qrpay_info.order_subject,"1788-%s",ticket_number);


    strftime(order_time,sizeof(order_time),"%Y%m%d%H%M%S",ptr);        
    strcpy(qrpay_info.order_time,order_time);

    /* print the qr code from alipay */
    DebugErrorInfo("Get Before alipay_main,imsi is %s\n",qrpay_info.imsi);
    alipay_main(out, &qrpay_info, ALI_CREATEANDPAY);
    return 0;	
}	
#endif