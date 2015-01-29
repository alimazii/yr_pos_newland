/**
* @file main.c
* @brief ��ģ�飨������ڣ�
* @version  1.0
* @author lubobj
* @date 2015-01-12
*/


#include <string.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/wait.h>

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>

#include <stdarg.h>
#include <pthread.h>
#include <signal.h>
#include "NDK.h"
#include "aliqr.h"

#define _DEBUG_
int cIsDebug = 1; /* debug switch */ 

static struct payInfo commTestIn;
static struct qr_result commTestOut;
extern unsigned long long query_number; 

/* from backend */
extern char jfkey[32+1];
extern char pos_imsi[20];
extern struct payInfo qrpay_info;
static unsigned int query_count;
static unsigned int iQuery;
char time_mark[32] = {0};
char* str_timemark = "1408001801550";

pthread_mutex_t prmutex;
void *thr_fn(void* arg);


/* timer to get alipay payment response -- sample code */ 
void payment_alarm_handler(int sig) {
    //struct payInfo qrpay_info;
    struct qr_result payquery_result;
    int nbytes;
    


    //strcpy(qrpay_info.imsi,"460006922139942");
    memset(payquery_result.order,0,QRRESULTSTR);
    memset(payquery_result.time_mark,0,32);
#if 1
    if (time_mark[0] == '\0'){
       /* time_mark is missing, using random one to require a new one */
       //printf("query parameter: time_mark is missing!\n");
       DebugErrorInfo("query parameter: time_mark is missing!\n");
       strcpy(qrpay_info.time_mark,"1408001801550");
       //alipay_main(&payquery_result, &qrpay_info);
       //alipay_main(&payquery_result, &qrpay_info, ALI_PRECREATE_QUERY);
       alipay_main(&payquery_result, &qrpay_info, ALI_QUERY_TIMEMARK);

       if(payquery_result.time_mark[0] == '\0')
           DebugErrorInfo("AT BEGINING,THE TIME_MARK is NULL!\n");
       strcpy(time_mark, payquery_result.time_mark);
       DebugErrorInfo("query parameter: new time_mark is %s\n",time_mark);
       alarm(10);
       return;
    }
#endif
    //strcpy(qrpay_info.timemark,"123456789012345");
    strcpy(qrpay_info.time_mark,time_mark);
    DebugErrorInfo("the query timer mark is %s\n",time_mark);
    DebugErrorInfo("alarm!\n");
    //memset(payquery_result.qr_string,0,1024);
    //memset(payquery_result.time_mark,0,32);
    //alipay_main(&payquery_result, &qrpay_info);
    alipay_main(&payquery_result, &qrpay_info, ALI_QUERY_TIMEMARK);
    if( payquery_result.time_mark[0] != '\0') 
       strcpy(time_mark, payquery_result.time_mark);
    else
       DebugErrorInfo("NO time_mark return from server!\n");
    if(payquery_result.order[0]){
#if 0    	
    struct sockaddr_un address;

    socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    if(socket_fd < 0)
    {
        printf("socket() failed\n");
        alarm(10);
        return ;
    }
    /* ��һ���ɾ��ĵ�ַ�ṹ��ʼ */ 
    memset(&address, 0, sizeof(struct sockaddr_un));
    address.sun_family = AF_UNIX;
    snprintf(address.sun_path, 20/*UNIX_PATH_MAX*/, "/tmp/demo_socket");

    if(connect(socket_fd, 
            (struct sockaddr *) &address, 
            sizeof(struct sockaddr_un)) != 0)
    {
        printf("connect() failed\n");
        alarm(10);
        return ;
    }
    
    nbytes = strlen(payquery_result.qr_string);
    write(socket_fd, payquery_result.qr_string, nbytes);
    close(socket_fd);
#endif    
    query_count = 0;
    alarm(0);

    DebugErrorInfo("stop alarm timer after query completed\n");
    return;
    }

    query_count--;
    if(query_count > 0)
        alarm(10);
    else {

        DebugErrorInfo("The query_server alarm timer is stopped!\n");
        alarm(0);
    }
   
}

int main(void)
{

    char dispVer[16];
    uint scr_width,scr_height;
    uint x,y,width,height;
    uint font_width,font_height;
    ST_APPINFO PayAppInfo;
    ST_PPP_CFG PPPDialCfg;
    int  nStatus;
    int  nErrCode;
    char szTmpBuf[30];
    int ucKey;
    int ret = 0;
    
    int err;
    pthread_t ntid;
    sigset_t sigset;
    
    struct sigaction act_handler;

#ifdef NLSCAN_EN    
    int nbytes;
    char buffer[128]; 
    
#else
    int nbytes;
    FILE *stream;
    FILE *wstream;
    int  k;
    char numBuf[10];    /* for money input */
    char buffer[3000];  /* huge respons for some cmd */
    char cmd[128];    
#endif       	  
    /* disable auto suspend */
    NDK_SysSetSuspend(0);
    
    /*��ʼ��Ӧ�ý���*/
    if(NDK_ScrInitGui()!=NDK_OK)
        return -1;
    /*����Ϊ���Զ�ˢ��*/
    NDK_ScrAutoUpdate(0,NULL);
    /*���ñ���ͼ*/
    //if(NDK_ScrSetbgPic("disp_bg.jpg")!=NDK_OK)
        //return -1;
    NDK_ScrClrs();

#ifdef NLSCAN_EN
    /* use serial port one for 1D Barcode Scanner */
    /* Serial Port Connection: Female to Female, 5<->5, 2<->3, 3<->2 */
    /* Port Configure: 9600-8-"No Parity"-"1 Stop Bit" */
    ret = NDK_PortOpen(PORT_NUM_COM1, "9600,8,N,1");  
#else      
    /* open the serial port to print log */
    ret = NDK_PortOpen(PORT_NUM_COM1, "115200,8,N,1");
#endif     


    if (ret == NDK_OK){
    	  NDK_ScrPrintf("Serial Port 1 Open OK!\n");
    }
    else{
    	  NDK_ScrPrintf("Serial Port 1 Open FAIL!\n");	
    }
    NDK_ScrRefresh();

    
#ifdef NLSCAN_EN     
    while(1){
    	
    /* for newland scan */
   
    memset(buffer, 128, 0);

    ret = NDK_PortRead(PORT_NUM_COM1, 128, buffer, 500, &nbytes); 
    
    if (ret == NDK_OK && nbytes > 0){
        NDK_ScrClrs();
        NDK_ScrPrintf("Serial Port 1 Read OK!\n");
        NDK_ScrPrintf("Number:%s\n",buffer);
    }
    else{
    	  
    	  //NDK_ScrPrintf("Serial Port 1 Read FAIL,%d\n",ret);	
    }
    NDK_ScrRefresh();
    
    }
    /* newland scan end */
#endif  

  
    memset((char *)&PPPDialCfg, 0, sizeof(ST_PPP_CFG));
    sprintf(szTmpBuf, "+CGDCONT=1,\"IP\",\"CMNET\"");
    PPPDialCfg.nDevType = 0; /* 2G Modem */
    PPPDialCfg.nPPPFlag = LCP_PPP_KEEP;
    PPPDialCfg.PPPIntervalTimeOut = 60;
    strcpy(PPPDialCfg.szApn, szTmpBuf);
    strcpy(PPPDialCfg.szDailNum,"*99***1#");
    NDK_PppSetCfg(&PPPDialCfg, sizeof(PPPDialCfg));
    ret = NDK_PppDial("card","card");
    if (ret == NDK_OK){
    	  
    	  DebugErrorInfo("PPP Dial Function Called\n");
    }
    else{
    	  
    	  DebugErrorInfo("PPP Dial Function Call FAILED,ret=%d\n",ret);	
    	  goto end;
    }  
    
    while(1){ 
        NDK_PppCheck(&nStatus, &nErrCode);
        NDK_ScrClrs();
        if (nStatus==PPP_STATUS_DISCONNECT) {
            NDK_ScrPrintf("PPP DATA DISCONNECT\n");
            NDK_ScrRefresh();
            NDK_KbGetCode(0,&ucKey);
            goto end;
        } else if (nStatus==PPP_STATUS_CONNECTED) {            
            NDK_ScrPrintf("PPP DATA CONNECTED\n");
            NDK_ScrRefresh();
            NDK_KbGetCode(0,&ucKey);
            break;
        } else {
            NDK_ScrPrintf(".");
            NDK_ScrRefresh();
            if(NDK_KbGetCode(1,&ucKey)==NDK_OK && ucKey==K_ESC) {
                goto end;
            }
        }
    }
#if 0     
    NDK_ScrClrs();    
    NDK_ScrGetVer(dispVer);
    NDK_ScrPrintf("��ʾģ��汾:%s\n",dispVer);
    NDK_ScrGetLcdSize(&scr_width,&scr_height);
    NDK_ScrPrintf("Һ���ߴ�:%dx%d\n",scr_width,scr_height);
    NDK_ScrGetViewPort(&x,&y,&width,&height);
    NDK_ScrPrintf("��ʾ����:%d-%d-%d-%d\n",x, y, width,height);
    NDK_ScrGetFontSize(&font_width, &font_height);
    NDK_ScrPrintf("��ǰ����ߴ�Ϊn%dx%d\n",font_width, font_height);
    NDK_ScrRefresh();
#endif

#if 0
    while(1){

        nbytes = 0;
        memset(buffer, 3000, 0);
        NDK_KbGetInput(buffer, 2, 100, NULL, INPUTDISP_NORMAL, 0, INPUT_CONTRL_NOLIMIT_ERETURN);
        if (ret == NDK_OK ){

        stream = popen(buffer, "r");
        memset(buffer, 3000, 0);
        nbytes = fread(buffer, sizeof(char), sizeof(buffer), stream);
        pclose(stream);

        if(nbytes > 2000)
        {	
        	DebugErrorInfo("respons: %s\n",&buffer[2000]);
        }	
        else
        	DebugErrorInfo(buffer);

        continue; 
        }
        else 
            continue;
        

    }
#endif
	  
	  
    /* process alarm signal callback in main loop */
    act_handler.sa_handler = payment_alarm_handler;
    act_handler.sa_flags = 0;
    sigemptyset(&act_handler.sa_mask);
    sigaction(SIGALRM, &act_handler, NULL);
    
               
    //pthread_mutex_init(&prmutex, NULL);
          
           
    ret = NDK_AppGetInfo(NULL,0,&PayAppInfo, sizeof(PayAppInfo));
    if (ret == NDK_OK){
    	  DebugErrorInfo("Get Info OK,name: %s,nSeriNo is %d\n",PayAppInfo.szAppName,PayAppInfo.nSeriNo);
    }
    else{
    	  DebugErrorInfo("Get Info FAIL,ret=%d\n",ret);	
    }	


	  getIMSIconfig();
    if(jfkey[0] == 0 && getPosKey() > 0){
    	DebugErrorInfo("Get POS KEY Error from thr_fn!\n");
      return 1;
	  }
	  strcpy(qrpay_info.imsi, pos_imsi);
	  strcpy(qrpay_info.order_key, jfkey);
    
    query_count = 5;        
    alarm(10); 

 
    while(1)
    {
    	  NDK_ScrClrs();
    	  //NDK_ScrStatusbar(STATUSBAR_DISP_ALL|STATUSBAR_POSITION_TOP);
    	  NDK_ScrDispString(40,0,"ӯ���ͨ",0);
    	  NDK_ScrDispString(4,12,"1.֧����",0);
    	  NDK_ScrDispString(4,24,"2.�𵥲�ѯ",0);
    	  NDK_ScrDispString(4,36,"3.�ս�",0);
    	  NDK_ScrDispString(66,12,"4.ǩ��",0);
    	  NDK_ScrDispString(66,24,"5.����ǩ��",0);
    	  
    #ifdef REFUND_EN
        NDK_ScrDispString(66,36,"6.�˻�",0);
    #endif	  
        NDK_ScrRefresh();
    	  NDK_KbGetCode(0, &ucKey);
    	  if(ucKey == K_ONE)
    	  	  DebugErrorInfo("Key 1 pressed!\n");
        switch(ucKey)
		    {
			    case K_ESC:
			    	break;
			    case K_ONE:


			    	    NDK_ScrClrs();
			    	    NDK_ScrDispString(4, 0, "��ӭ��֧����Ǯ��֧��",0);
				        NDK_ScrDispString(15, 24, "�밴OK��������",0);
				        NDK_ScrDispString(4, 36, "��CANCEL��BACK������",0);
				        NDK_ScrRefresh();
				        NDK_KbGetCode(0, &ucKey); 
				        switch(ucKey)
				        {
				        	  case K_ESC:
				        	  case K_BASP:
				        	  	break;
				        	  
				        	  case K_ENTER:	
				        	  	NDK_ScrClrs();
				        	  	NDK_ScrDispString(20, 0, "��������밴OK��",0);
				        	  	NDK_ScrDispString(4, 24, "��������:",0);      
				        	  	NDK_ScrRefresh();
				        	  	strncpy(numBuf,"0.00",5);
				        	  	/* FIX ME Later,Money Check */
				        	  	NDK_KbGetInput(numBuf, 4, 7, NULL, INPUTDISP_OTHER, 0, INPUT_CONTRL_LIMIT_ERETURN);
				        	  	DebugErrorInfo("The Input Money:%s\n",numBuf);
				        	  	
				        	  	err = pthread_create(&ntid, NULL, thr_fn, (void*)numBuf);
				        	  	print_logo();


                      if(err != 0)
                      DebugErrorInfo("!!!! query thread create failure-----\n");
			    	          DebugErrorInfo("We just switch KEY ONE,NOTHING!\n");
				        	  	
				        	  	break;
				        }	
				    break;

				    #if 0
				    pthread_mutex_lock(&prmutex);
				    
				    ret = generator_qrcode_to_bmp((void*)&commTestOut,"0.01",(void*)&commTestIn);
				    iQuery = 1; /* open the query trigger */
				    pthread_mutex_unlock(&prmutex);
				    break; 
				    #endif
            //ret = generator_qrcode_to_bmp((void*)&commTestOut,"0.01",(void*)&commTestIn);
			    	
#if 0			    	
			    case K_TWO:
			    	GprsTest();
			    	break;
			    case K_THREE:
			    	EthernetTest();
			    	break;
			    case K_FOUR:
			    	CDMACommTest();
			    	break;	
          case K_FIVE:
            WifiCommTest();
            break;
          case K_SIX:
            SetCommParam();
            break;
#endif                      
		     }
		     //pause();
        
    };
    
end:    
    /* close the serial port 1 */
    ret = NDK_PortClose(PORT_NUM_COM1);   
    if (ret == NDK_OK){
    	  NDK_ScrPrintf("Serial Port 1 Close OK!\n");
    }
    else{
    	  NDK_ScrPrintf("Serial Port 1 Close FAIL!\n");	
    }	 
    NDK_ScrRefresh();
    ret = NDK_PppHangup(1);
    NDK_ScrClrs();
    if (ret == NDK_OK){
    	  NDK_ScrPrintf("PPP Dial Close OK!\n");
    }
    else{
    	  NDK_ScrPrintf("PPP Dial Close FAIL!\n");	
    }  
    NDK_ScrRefresh();     
    return 0;

}

/**
* @fn DebugBufToAux
* @brief ���ڵ��Թ���
* @param in 
* @return ����
*/
int DebugBufToAux(const char *pszBuf, const int nBufLen)
{
#ifdef _DEBUG_
	static int snInitAux=0;
	//ar cDebugAux = AUX_PINPAD;
	//char cDebugAux = AUX1;

	if (cIsDebug == 1)
	{
		if(snInitAux == 0)
		{
			//initaux(cDebugAux, BPS115200, DB8 |STOP1 |NP);
			//clrportbuf(cDebugAux, 0);
			NDK_PortClrBuf(PORT_NUM_COM1);
			snInitAux = 1;
		}
		return NDK_PortWrite(PORT_NUM_COM1, nBufLen, pszBuf);
	}
	else
	{
		return NDK_OK;
	}
#endif
	return NDK_OK;
}



void DebugErrorInfo(char* lpszFormat, ...)
{
#ifdef _DEBUG_
	va_list args;
	int nBuf;
	char sbuf[2048]={0};
	
	if (cIsDebug == 1)
	{
		va_start(args, lpszFormat);
		nBuf=vsprintf(sbuf, lpszFormat, args);
		strcat(sbuf, "\r\n");
		DebugBufToAux(sbuf, strlen(sbuf));
		va_end(args);
	}
#endif
}


#if 0
int DebugSendHex(const void * psRecvBuf, int nLen)
{
#ifdef _DEBUG_
	char szBuf[8092];
	int i=0,nStrLen,nIndex;

	if (cIsDebug == YES)
	{
		memset(szBuf, 0, sizeof(szBuf));
		for(i=0; i<nLen; i++)
		{
			sprintf(szBuf+i*3, "%02X ", *((char*)psRecvBuf+i));
		}
		strcat(szBuf,"\r\n");
		nStrLen = strlen(szBuf);
		nIndex = 0;
		while(nIndex < nStrLen)
		{
			if ((nIndex+1024) < nStrLen)
			{
				DebugBufToAux(szBuf+nIndex,1024);
				nIndex += 1024;
			}
			else
			{
				DebugBufToAux(szBuf+nIndex,nStrLen - nIndex);
				nIndex = nStrLen;
			}
		}
	}
	return APP_SUCC;
#endif
	return APP_SUCC;
}
#endif

#if 0
int SetMoney()
{
    int ret;
    inu ucKey;
    char buff[128] = "";
    char buffshow[128] = "";
    int tempDataNum;


    memset(buff, 0, sizeof(buff));
    NDK_ScrClrs();
    SetScrFont(FONT20, WHITE);
    TextOut(0, 2, ALIGN_CENTER, "��������");
    TextOut(0, 8, ALIGN_CENTER, "��������밴OK��");
    SetScrFont(FONT20, RED);
    ret = InputMoney(8, 5, buff,20); 
    if(ret < 0)
    	goto FAILED;

	printf("\nbefore:%s\n", buff);
    Moneyformat(buff);
    printf("\nafter:%s\n", buff);
    pthread_mutex_lock(&prmutex);
    print_logo();
    //strcpy(commTestIn.order_subject,gRCP.rcp_title_company);
    //strcpy(commTestIn.order_subject,"��������������޹�˾�����ó��");
    //strcpy(commTestIn.order_subject,"%E5%88%86%E8%B4%A6%E6%B5%8B%E8%AF%95-sky");
    //strcpy(commTestIn.order_subject,"AliPay");
    ret = generator_qrcode_to_bmp((void*)&commTestOut,buff,(void*)&commTestIn);
	//system(buff);	 

    OkBeep();
    NDK_ScrClrs();
    SetScrFont(FONT20, WHITE);
    if(ret == 1)
        TextOut(2, 4, ALIGN_CENTER, "����֧����ʧ�ܣ���������");
    else {
        //TextOut(2, 4, ALIGN_CENTER, "input money OK!");
        TextOut(2, 4, ALIGN_CENTER, "�Եȣ����������ά��...");
        printTail(buff,commTestOut.out_trade_no);
    }
    pthread_mutex_unlock(&prmutex);
    /* send trigger to query_server to start timer */
#ifdef ALIPAY_FIFO
    write(pipe_fd, "START", 6);
#endif
    WaitKey(1000);
    return OK;

FAILED:

	SetScrFont(FONT20, WHITE);
    if(ret == -1)
        TextOut(2, 6, ALIGN_CENTER, "error input");
    else if (ret == -3) {
        //TextOut(2, 6, ALIGN_CENTER, "cancel");
        return ERROR;
    }
    else 
        TextOut(2, 6, ALIGN_CENTER, "unknow");
	WaitKey(2000);
	printf("SetMoney failed ret;%d\n", ret);
	return ERROR;
}
#endif

void print_logo()                                                                                                                                                                                                                                                                                                                                         
{                                                                                                                                                                                                                                                                                                                                                         
    char PrintBuff[40];
    int ret = 0;  
    EM_PRN_STATUS PrnStatus;  
    int ucKey;
                                                                                                                                                                                                                                                                                
START_PRINT:                                                                                                                                                                                                                                                                                                                     
    if(NDK_PrnInit(0) != NDK_OK) {                                                                                                                                                                                                                                                                                                
        DebugErrorInfo("the printer is not working well!\n");                                                                                                                                                                                                                                                                            
        goto end1;                                                                                                                                                                                                                                                                                                               
    }                                                                                                                                                                                                                                                                                                                            
                                                                                                                                                                                                                                                                                                                                 
                                                                                                                                                                                                                                                                                                                                 
    NDK_PrnStr("\n\n");
#if 1                                                                                                                                                                                                                                                                                                                            
#if 1                                                                                                                                                                                                                                                                                                                            
    memset(PrintBuff,0,sizeof(PrintBuff));                                                                                                                                                                                                                                                                                       
    NDK_PrnSetFont(PRN_HZ_FONT_48x48B, PRN_ZM_FONT_32x32B);                                                                                                                                                                                                                                                                       
#ifdef RECEIPT_CONF                                                                                                                                                                                                                                                                                                              
    strcpy(PrintBuff,gRCP.rcp_title_line1);                                                                                                                                                                                                                                                                                      
    NDK_PrnStr(PrintBuff);                                                                                                                                                                                                                                                                                                       
    NDK_PrnSetFont(48);                                                                                                                                                                                                                                                                                                          
    strcpy(PrintBuff,gRCP.rcp_title_line2);                                                                                                                                                                                                                                                                                      
    NDK_PrnStr(PrintBuff);                                                                                                                                                                                                                                                                                                       
    NDK_PrnSetFont(24);                                                                                                                                                                                                                                                                                                          
    strcpy(PrintBuff,gRCP.rcp_title_address);                                                                                                                                                                                                                                                                                    
    NDK_PrnStr(PrintBuff);                                                                                                                                                                                                                                                                                                       
    strcpy(PrintBuff,gRCP.rcp_title_number);                                                                                                                                                                                                                                                                                     
    NDK_PrnStr(PrintBuff);                                                                                                                                                                                                                                                                                                       
#else                                                                                                                                                                                                                                                                                                                            
    strcpy(PrintBuff,"  ��������\n");
    NDK_PrnStr(PrintBuff);
    strcpy(PrintBuff,"  GL Cafe\n");
    NDK_PrnStr(PrintBuff);
    NDK_PrnSetFont(PRN_HZ_FONT_24x24, PRN_ZM_FONT_12x24A);
    strcpy(PrintBuff,"  ��������·36�Ż�ó����D��1��\n");
    NDK_PrnStr(PrintBuff);
    strcpy(PrintBuff,"      ���͵绰��58257262\n");
    NDK_PrnStr(PrintBuff);
#endif
    
#else                                                                                                                                                                                                                                                                                                                                                     
    strcpy(PrintBuff,"  ��������·36�Ż�ó����D��1��");                                                                                                                                                                                                                                                                                                   
    printf("PrintBuff:%d,��������·36�Ż�ó����D��1��:%d",strlen(PrintBuff), strlen("��������·36�Ż�ó����D��1��"));                                                                                                                                                                                                                                     
    parseXML("/usr/local/logo.xml");                                                                                                                                                                                                                                                                                                                      
#endif                                                                                                                                                                                                                                                                                                                                                    
    strcpy(PrintBuff,"--------------------------------");
    NDK_PrnStr(PrintBuff);
    NDK_PrnStr("\n\n");
    NDK_PrnSetFont(PRN_HZ_FONT_32x32, PRN_ZM_FONT_16x32 ); 
    strcpy(PrintBuff,"    ֧����Ǯ��֧��");
    NDK_PrnStr(PrintBuff);
#else
    NDK_PrnSetFont(32);                                                                                                             
    strcpy(PrintBuff,"     ֧������Լ�̻�");                                                                                                                                                                                                                                                                                                              
    NDK_PrnStr(PrintBuff);                                                                                                                                                                                                                                                                                                                             
    strcpy(PrintBuff,"   �����ֻ�֧���³���");                                                                                                                                                                                                                                                                                                            
    NDK_PrnStr(PrintBuff);                                                                                                                                                                                                                                                                                                                             
    strcpy(PrintBuff,"-----------------------------------");                                                                                                                                                                                                                                                                                              
    NDK_PrnStr(PrintBuff);                                                                                                                                                                                                                                                                                                                             
#endif                                                                                                                                                                                                                                                                                                                                                    
                                                                                                                                                                                                                                                                                                                                                          
                                                                                                                                                                                                                                                                                                                                                          
                                                                                                                                                                                                                                                                                                                                                          
    //��ʼ��ӡ                                                                                                                                                                                                                                                                                                                                            
    ret = NDK_PrnStart();                                                                                                                                                                                                                                                                                                                                    
    DebugErrorInfo("print error code:[%d]\n", ret);                                                                                                                                                                                                                                                                                                             
    if(ret != NDK_OK)                                                                                                                                                                                                                                                                                                                                          
    {                                                                                                                                                                                                                                                                                                                                                     
        NDK_PrnGetStatus(&PrnStatus);
        if(PrnStatus & PRN_STATUS_BUSY)                                                                                                                                                                                                                                                                                                                           
            goto START_PRINT;                                                                                                                                                                                                                                                                                                                             
        else if(PrnStatus & PRN_STATUS_VOLERR)                                                                                                                                                                                                                                                                                                                        
            goto end2;                                                                                                                                                                                                                                                                                                                                    
        else if(PrnStatus & PRN_STATUS_NOPAPER || PrnStatus & PRN_STATUS_OVERHEAT)                                                                                                                                                                                                                                                                                                                         
            goto end1;                                                                                                                                                                                                                                                                                                                                    
    }                                                                                                                                                                                                                                                                                                                                                     
    return;  
                                                                                                                                                                                                                                                                                                                                                 
end1:
	                                                                                                                                                                                                                                                                                                                                                     
    NDK_SysBeep();                                                                                                                                                                                                                                                                                                                                           
    NDK_ScrClrs();                                                                                                                                                                                                                                                                                                                                      
    NDK_ScrDispString(24, 24, "�����ӡ��", 0);                                                                                                                                                                                                                                                                                                          
    NDK_ScrDispString(36, 36, "��ӡʧ��", 0); 
    NDK_ScrRefresh(); 
    NDK_KbGetCode(2, &ucKey);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
    return; 
                                                                                                                                                                                                                                                                                                                                              
                                                                                                                                                                                                                                                                                                                                                          
end2:
	                                                                                                                                                                                                                                                                                                                                                     
    NDK_SysBeep();                                                                                                                                                                                                                                                                                                                                           
    NDK_ScrClrs();                                                                                                                                                                                                                                                                                                                                      
    NDK_ScrDispString(36, 24, "��������",0);                                                                                                                                                                                                                                                                                                          
    NDK_ScrDispString(24, 36, "�޷�ִ�д�ӡ",0); 
    NDK_ScrRefresh(); 
    NDK_KbGetCode(2, &ucKey);                                                                                                                                                                                                                                                                                                                                       
    return;                                                                                                                                                                                                                                                                                                                                               
} 

void printAD()
{
	int ret;
	int ucKey;

  NDK_ScrClrs();
	NDK_ScrDispString(36, 24, "���ڴ�ӡ...",0);
	NDK_ScrRefresh();

  ret = NDK_PrnInit(0);
	ret = NDK_PrnPicture(0, "print.bmp");
	DebugErrorInfo("BMP Loading ret:[%d]\n", ret);
	
	ret = NDK_PrnStart();
	
  if(ret != NDK_OK)
  {
      DebugErrorInfo("PrintBMP ret:[%d]\n", ret); 	
      NDK_SysBeep();        
      NDK_ScrClrs();
	    NDK_ScrDispString(36, 24, "��ӡʧ��",0);
	    NDK_ScrRefresh();
      NDK_KbGetCode(2, &ucKey);
  }
  
  return;
}

void printTail(char* price, char* out_trade_no)
{
    int ret = 0;
    char printBuff[50];
    EM_PRN_STATUS PrnStatus;
    int ucKey;
    
START_PRINT:
    if(NDK_PrnInit(0) != NDK_OK) {                                                                                                                                                                                                                                                                                                
        DebugErrorInfo("the printer is not working well!\n");                                                                                                                                                                                                                                                                            
        goto end1;                                                                                                                                                                                                                                                                                                               
    }

    NDK_PrnSetFont(PRN_HZ_FONT_32x32, PRN_ZM_FONT_16x32 );
    sprintf(printBuff,"���к�:%lld\n",query_number);
    NDK_PrnStr(printBuff);
    if(strlen(out_trade_no) > 0) {
        strcpy(printBuff,"�̻�������:");
        strcat(printBuff,out_trade_no);
        NDK_PrnStr(printBuff);
        memset(out_trade_no,0, 65);
    }
    NDK_PrnStr("\n\n");

    NDK_PrnSetFont(PRN_HZ_FONT_32x32, PRN_ZM_FONT_16x32 );
    strcpy(printBuff,"���������ѽ��: ");
    strcat(printBuff,price);
    NDK_PrnStr(printBuff);
    NDK_PrnStr("\n\n");
    NDK_PrnSetFont(PRN_HZ_FONT_24x24, PRN_ZM_FONT_12x24A);
    strcpy(printBuff," ����Ʒ��ӯ���ͨ�ṩ����֧��\n");
    NDK_PrnStr(printBuff);
    NDK_PrnSetFont(PRN_HZ_FONT_24x24, PRN_ZM_FONT_12x24A);
    strcpy(printBuff,"     ��ϵ�绰��4008190900\n");
    NDK_PrnStr(printBuff);
    strcpy(printBuff,"--------------------------------");
    NDK_PrnStr(printBuff);
    strcpy(printBuff,"���¹��λ���̵绰��4008190900\n");
    NDK_PrnStr(printBuff);
    NDK_PrnStr("\n");

    



    //��ʼ��ӡ    
    ret = NDK_PrnStart();                                                                                                                                                                                                                                                                                                                                    
    DebugErrorInfo("print error code:[%d]\n", ret);                                                                                                                                                                                                                                                                                                             
    if(ret != NDK_OK)                                                                                                                                                                                                                                                                                                                                          
    {                                                                                                                                                                                                                                                                                                                                                     
        NDK_PrnGetStatus(&PrnStatus);
        if(PrnStatus & PRN_STATUS_BUSY)                                                                                                                                                                                                                                                                                                                           
            goto START_PRINT;                                                                                                                                                                                                                                                                                                                             
        else if(PrnStatus & PRN_STATUS_VOLERR)                                                                                                                                                                                                                                                                                                                        
            goto end2;                                                                                                                                                                                                                                                                                                                                    
        else if(PrnStatus & PRN_STATUS_NOPAPER || PrnStatus & PRN_STATUS_OVERHEAT)                                                                                                                                                                                                                                                                                                                         
            goto end1;                                                                                                                                                                                                                                                                                                                                    
    }                                                                                                                                                                                                                                                                                                                                                     
   
    printAD();


    NDK_PrnStr("\n\n\n");
    ret = NDK_PrnStart();
    DebugErrorInfo("print error code:[%d]\n", ret);
    if(ret != NDK_OK)                                                                                                                                                                                                                                                                                                                                          
    {                                                                                                                                                                                                                                                                                                                                                     
        NDK_PrnGetStatus(&PrnStatus);
        if(PrnStatus & PRN_STATUS_BUSY)                                                                                                                                                                                                                                                                                                                           
            goto START_PRINT;                                                                                                                                                                                                                                                                                                                             
        else if(PrnStatus & PRN_STATUS_VOLERR)                                                                                                                                                                                                                                                                                                                        
            goto end2;                                                                                                                                                                                                                                                                                                                                    
        else if(PrnStatus & PRN_STATUS_NOPAPER || PrnStatus & PRN_STATUS_OVERHEAT)                                                                                                                                                                                                                                                                                                                         
            goto end1;                                                                                                                                                                                                                                                                                                                                    
    }   

    return;

end1:
	                                                                                                                                                                                                                                                                                                                                                     
    NDK_SysBeep();                                                                                                                                                                                                                                                                                                                                           
    NDK_ScrClrs();                                                                                                                                                                                                                                                                                                                                      
    NDK_ScrDispString(24, 24, "�����ӡ��", 0);                                                                                                                                                                                                                                                                                                          
    NDK_ScrDispString(36, 36, "��ӡʧ��", 0); 
    NDK_ScrRefresh(); 
    NDK_KbGetCode(2, &ucKey);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
    return; 
                                                                                                                                                                                                                                                                                                                                              
                                                                                                                                                                                                                                                                                                                                                          
end2:
	                                                                                                                                                                                                                                                                                                                                                     
    NDK_SysBeep();                                                                                                                                                                                                                                                                                                                                           
    NDK_ScrClrs();                                                                                                                                                                                                                                                                                                                                      
    NDK_ScrDispString(36, 24, "��������",0);                                                                                                                                                                                                                                                                                                          
    NDK_ScrDispString(24, 36, "�޷�ִ�д�ӡ",0); 
    NDK_ScrRefresh(); 
    NDK_KbGetCode(2, &ucKey);                                                                                                                                                                                                                                                                                                                                       
    return;
}


 void *thr_fn(void* arg)                                                                                                    
{                                                                                                  
 	  int ret = 0;                                                                                     
	  char *total_fee = (char*)arg;                                                                    
	  	                                                                                               
	  getIMSIconfig();                                                                                 
                                                                                                      
    if(jfkey[0] == 0 && getPosKey() > 0){                                                            
                                                                                                     
      	DebugErrorInfo("Get POS KEY Error from thr_fn!\n");                                                                    
        return 1;                                                                                      
 	  }                                                                                                                        
    strcpy(qrpay_info.imsi, pos_imsi);                           
 	  strcpy(qrpay_info.order_key, jfkey);                                                             
                                                                                                    
                                                                                                    
    ret = generator_qrcode_to_bmp((void*)&commTestOut,total_fee,(void*)&commTestIn); 
    
    NDK_SysBeep();                                                                                                                                                                                                                                                                                                                                           
    NDK_ScrClrs();
           
    if(ret == 1){

        NDK_ScrDispString(24, 24, "����֧����ʧ��",0);
        NDK_ScrDispString(36, 36, "��������",0);
        NDK_ScrRefresh();
        return ret;
    }    
    else {
        //TextOut(2, 4, ALIGN_CENTER, "input money OK!");
        NDK_ScrDispString(0, 36, "�Եȣ����������ά��...",0);
        NDK_ScrRefresh();
        printTail(total_fee,commTestOut.out_trade_no);
    }              
 	                                                                                                 
    if(query_count == 0)                                                                                
            alarm(10);                                                                                  
    else                                                                                             
       query_count = 5;                                                                                 
                                                                                                        
    return ret;                                                                                          
 }	                                                                                                 
                                                                                                                            
                                                                                                                            
                                                                                                                            
                                                                                                                            
                                                                                                                                                                                                                                                                                                                                                        