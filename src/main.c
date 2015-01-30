/**
* @file main.c
* @brief 主模块（函数入口）
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
#include <time.h>
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

static char Defualtdata[] = "yy-mm-dd/hh:mm";
char* serial2date(char* serialNo)
{
    char* ret = &Defualtdata[0];
    Defualtdata[0] = serialNo[5];
    Defualtdata[1] = serialNo[6];

    Defualtdata[3] = serialNo[7];
    Defualtdata[4] = serialNo[8];

    Defualtdata[6] = serialNo[9];
    Defualtdata[7] = serialNo[10];

    Defualtdata[9] = serialNo[11];
    Defualtdata[10] = serialNo[12];

    Defualtdata[12] = serialNo[13];
    Defualtdata[13] = serialNo[14];
    return ret;
}

int SplitStr(char *buff, char *parr[], char *token) 
{
        char *pc = strtok(buff, token); 
        int i;  

        for(i=0; pc != NULL; i++)
        {       
                parr[i] = pc;
                pc = strtok(NULL, token); 
        }       
        
        return i;
}

/* timer to get alipay payment response -- sample code */ 
void payment_alarm_handler(int sig) {
    //struct payInfo qrpay_info;
    struct qr_result payquery_result;
    int ret,nbytes,ucKey,i;
    EM_PRN_STATUS PrnStatus;
    
    char buffer[1024];
    int trade_num;
    char *trade_ptr[100] = {NULL}; 
    char *trade_detail[5] = {NULL}; 
    struct receipt_info pos_receipt;
    char PrintBuff[30];
    //T_DATETIME tTime;    
    struct tm *ptr;
    time_t td;
    char pos_date[12];
    char pos_time[12];    
    


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
    /* 从一个干净的地址结构开始 */ 
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
    printf("MESSAGE FROM ALIPAY: %s\n", payquery_result.order);
    //nbytes = snprintf(buffer, 256, "hello from the server");
    //write(connection_fd, buffer, nbytes);
    /* start print out the payment query result */

    trade_num = SplitStr(payquery_result.order,trade_ptr,"|");

    //write(tty_data.posfd,alipay_receipt,sizeof(alipay_receipt));
    ///write(tty_data.posfd,"\n",1);
    /* get system time */
    time(&td);
    ptr = localtime(&td);
    strftime(pos_date,sizeof(pos_date),"%Y-%m-%d",ptr);
    strftime(pos_time,sizeof(pos_time),"%H:%M:%S",ptr);


    for (i=0; i<trade_num; i++){
        DebugErrorInfo("number %d trade:%s\n",i,trade_ptr[i]);
        SplitStr(trade_ptr[i],trade_detail,",");
        memset(pos_receipt.serial_number,0,24);
        memset(pos_receipt.out_trade_no,0,16);
        memset(pos_receipt.trade_no,0,32);
        memset(pos_receipt.total_fee,0,16);

        strcpy(pos_receipt.serial_number,trade_detail[0]);
        strcpy(pos_receipt.out_trade_no,trade_detail[1]);
        strcpy(pos_receipt.trade_no,trade_detail[2]);
        strcpy(pos_receipt.total_fee,trade_detail[3]);
        ///WritePayment(1, &pos_receipt);
        /// write(tty_data.posfd,"\n",1);
        ///write(tty_data.posfd,"\n",1);
        //pthread_mutex_lock(&prmutex);
START_PRINT:
        if(NDK_PrnInit(0) != NDK_OK) {                                                                                                                                                                                                                                                                                                
            DebugErrorInfo("the printer is not working well!\n");                                                                                                                                                                                                                                                                            
            goto end1;                                                                                                                                                                                                                                                                                                               
        } 
        

        memset(PrintBuff,0,30);

        NDK_PrnSetFont(PRN_HZ_FONT_32x32, PRN_ZM_FONT_16x32 );
        strcpy(PrintBuff,"支付宝交易凭条");
        NDK_PrnStr(PrintBuff);
        NDK_PrnStr("\n\n\n");

        strcpy(PrintBuff,"序列号：\n");
        NDK_PrnStr(PrintBuff);
        NDK_PrnStr(pos_receipt.serial_number);
        NDK_PrnStr("\n");

        strcpy(PrintBuff,"商户订单号：\n");
        NDK_PrnStr(PrintBuff);
        NDK_PrnStr(pos_receipt.out_trade_no);
        NDK_PrnStr("\n");

        strcpy(PrintBuff,"日期：\n");
        NDK_PrnStr(PrintBuff);
        NDK_PrnStr(pos_date);
        NDK_PrnStr("\n");

        strcpy(PrintBuff,"时间：\n");
        NDK_PrnStr(PrintBuff);
        NDK_PrnStr(pos_time);
        NDK_PrnStr("\n");

        strcpy(PrintBuff,"-------------------\n");
        NDK_PrnStr(PrintBuff);	   

        strcpy(PrintBuff,"支付宝当面付\n");
        NDK_PrnStr(PrintBuff);	   

        strcpy(PrintBuff,"交易号：\n");
        NDK_PrnStr(PrintBuff);
        NDK_PrnStr(pos_receipt.trade_no);
        NDK_PrnStr("\n");

        strcpy(PrintBuff,"金额：\n");
        NDK_PrnStr(PrintBuff);
        NDK_PrnStr(pos_receipt.total_fee);
        NDK_PrnStr("\n");

        strcpy(PrintBuff,"签名 \n");
        NDK_PrnStr(PrintBuff);
        NDK_PrnStr("\n");

        strcpy(PrintBuff,"本人同意上述交易\n");
        NDK_PrnStr(PrintBuff);

        //NDK_PrnStr("\n\n\n");
	 

        //开始打印            
        ret = NDK_PrnStart(); 
        //pthread_mutex_unlock(&prmutex);                                                                                                                                                                                                                                                                                                                                   
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
        NDK_PrnFeedPaper(PRN_FEEDPAPER_AFTER); 
        continue;
end1:

        NDK_SysBeep();                                                                                                                                                                                                                                                                                                                                           
        NDK_ScrClrs();                                                                                                                                                                                                                                                                                                                                      
        NDK_ScrDispString(24, 24, "请检查打印机", 0);                                                                                                                                                                                                                                                                                                          
        NDK_ScrDispString(36, 36, "打印失败", 0); 
        NDK_ScrRefresh(); 
        NDK_KbGetCode(0, &ucKey);


        goto START_PRINT;
end2:

        NDK_SysBeep();                                                                                                                                                                                                                                                                                                                                           
        NDK_ScrClrs();                                                                                                                                                                                                                                                                                                                                      
        NDK_ScrDispString(36, 24, "电量不足",0);                                                                                                                                                                                                                                                                                                          
        NDK_ScrDispString(24, 36, "无法执行打印",0); 
        NDK_ScrRefresh(); 
        NDK_KbGetCode(0, &ucKey);
        goto START_PRINT;  
    
    }



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
    
    /*初始化应用界面*/
    if(NDK_ScrInitGui()!=NDK_OK)
        return -1;
    /*设置为非自动刷新*/
    NDK_ScrAutoUpdate(0,NULL);
    /*设置背景图*/
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
            NDK_ScrPrintf("拨号成功\n");
            NDK_ScrRefresh();
            NDK_KbGetCode(2,&ucKey);
            break;
        } else {
            NDK_ScrPrintf("正在拨号...\n");
            NDK_ScrRefresh();
            if(NDK_KbGetCode(1,&ucKey)==NDK_OK && ucKey==K_ESC) {
                goto end;
            }
        }
    }
#if 0     
    NDK_ScrClrs();    
    NDK_ScrGetVer(dispVer);
    NDK_ScrPrintf("显示模块版本:%s\n",dispVer);
    NDK_ScrGetLcdSize(&scr_width,&scr_height);
    NDK_ScrPrintf("液晶尺寸:%dx%d\n",scr_width,scr_height);
    NDK_ScrGetViewPort(&x,&y,&width,&height);
    NDK_ScrPrintf("显示区域:%d-%d-%d-%d\n",x, y, width,height);
    NDK_ScrGetFontSize(&font_width, &font_height);
    NDK_ScrPrintf("当前字体尺寸为n%dx%d\n",font_width, font_height);
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
    	  NDK_ScrDispString(40,0,"盈润捷通",0);
    	  NDK_ScrDispString(4,12,"1.支付宝",0);
    	  NDK_ScrDispString(4,24,"2.逐单查询",0);
    	  NDK_ScrDispString(4,36,"3.日结",0);
    	  NDK_ScrDispString(66,12,"4.签到",0);
    	  NDK_ScrDispString(66,24,"5.结算签退",0);
    	  
    #ifdef REFUND_EN
        NDK_ScrDispString(66,36,"6.退货",0);
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
			    	    NDK_ScrDispString(4, 0, "欢迎用支付宝钱包支付",0);
				        NDK_ScrDispString(15, 24, "请按OK键输入金额",0);
				        NDK_ScrDispString(4, 36, "按CANCEL或BACK键返回",0);
				        NDK_ScrRefresh();
				        NDK_KbGetCode(0, &ucKey); 
				        switch(ucKey)
				        {
				        	  case K_ESC:
				        	  case K_BASP:
				        	  	break;
				        	  
				        	  case K_ENTER:	
				        	  	NDK_ScrClrs();
				        	  	NDK_ScrDispString(15, 0, "输入完成请按确认键",0);
				        	  	NDK_ScrDispString(4, 24, "请输入金额:",0);      
				        	  	NDK_ScrRefresh();
				        	  	//strncpy(numBuf,"0.00",5);
				        	  	/* FIX ME Later,Money Check */
				        	  	ret = NDK_KbGetInput(numBuf, 4, 7, NULL, INPUTDISP_NORMAL, 0, INPUT_CONTRL_LIMIT_ERETURN);
				        	  	if(ret == NDK_ERR)
				        	  		break;
				        	  	DebugErrorInfo("The Input Money:%s\n",numBuf);
				        	  	
				        	  	err = pthread_create(&ntid, NULL, thr_fn, (void*)numBuf);
				        	  	print_logo();
				        	  	err = pthread_join(ntid, NULL);


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
			    				    	
			    case K_TWO:
			    	querySingle();
			    	break;
#if 0			    	
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
* @brief 串口调试工具
* @param in 
* @return 返回
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
    TextOut(0, 2, ALIGN_CENTER, "请输入金额");
    TextOut(0, 8, ALIGN_CENTER, "输入完成请按OK键");
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
    //strcpy(commTestIn.order_subject,"北京金湖餐饮有限公司金湖环贸店");
    //strcpy(commTestIn.order_subject,"%E5%88%86%E8%B4%A6%E6%B5%8B%E8%AF%95-sky");
    //strcpy(commTestIn.order_subject,"AliPay");
    ret = generator_qrcode_to_bmp((void*)&commTestOut,buff,(void*)&commTestIn);
	//system(buff);	 

    OkBeep();
    NDK_ScrClrs();
    SetScrFont(FONT20, WHITE);
    if(ret == 1)
        TextOut(2, 4, ALIGN_CENTER, "链接支付宝失败，请检查网络");
    else {
        //TextOut(2, 4, ALIGN_CENTER, "input money OK!");
        TextOut(2, 4, ALIGN_CENTER, "稍等，正在输出二维码...");
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
    strcpy(PrintBuff,"  金湖茶餐厅\n");
    NDK_PrnStr(PrintBuff);
    strcpy(PrintBuff,"  GL Cafe\n");
    NDK_PrnStr(PrintBuff);
    NDK_PrnSetFont(PRN_HZ_FONT_24x24, PRN_ZM_FONT_12x24A);
    strcpy(PrintBuff,"  北三环东路36号环贸中心D栋1层\n");
    NDK_PrnStr(PrintBuff);
    strcpy(PrintBuff,"      订餐电话：58257262\n");
    NDK_PrnStr(PrintBuff);
#endif
    
#else                                                                                                                                                                                                                                                                                                                                                     
    strcpy(PrintBuff,"  北三环东路36号环贸中心D栋1层");                                                                                                                                                                                                                                                                                                   
    printf("PrintBuff:%d,北三环东路36号环贸中心D栋1层:%d",strlen(PrintBuff), strlen("北三环东路36号环贸中心D栋1层"));                                                                                                                                                                                                                                     
    parseXML("/usr/local/logo.xml");                                                                                                                                                                                                                                                                                                                      
#endif                                                                                                                                                                                                                                                                                                                                                    
    strcpy(PrintBuff,"--------------------------------");
    NDK_PrnStr(PrintBuff);
    NDK_PrnStr("\n\n");
    NDK_PrnSetFont(PRN_HZ_FONT_32x32, PRN_ZM_FONT_16x32 ); 
    strcpy(PrintBuff,"    支付宝钱包支付");
    NDK_PrnStr(PrintBuff);
#else
    NDK_PrnSetFont(32);                                                                                                             
    strcpy(PrintBuff,"     支付宝合约商户");                                                                                                                                                                                                                                                                                                              
    NDK_PrnStr(PrintBuff);                                                                                                                                                                                                                                                                                                                             
    strcpy(PrintBuff,"   引领手机支付新潮流");                                                                                                                                                                                                                                                                                                            
    NDK_PrnStr(PrintBuff);                                                                                                                                                                                                                                                                                                                             
    strcpy(PrintBuff,"-----------------------------------");                                                                                                                                                                                                                                                                                              
    NDK_PrnStr(PrintBuff);                                                                                                                                                                                                                                                                                                                             
#endif                                                                                                                                                                                                                                                                                                                                                    
                                                                                                                                                                                                                                                                                                                                                          
                                                                                                                                                                                                                                                                                                                                                          
                                                                                                                                                                                                                                                                                                                                                          
    //开始打印                                                                                                                                                                                                                                                                                                                                            
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
    NDK_ScrDispString(24, 24, "请检查打印机", 0);                                                                                                                                                                                                                                                                                                          
    NDK_ScrDispString(36, 36, "打印失败", 0); 
    NDK_ScrRefresh(); 
    NDK_KbGetCode(2, &ucKey);
    goto START_PRINT;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
    //return; 
                                                                                                                                                                                                                                                                                                                                              
                                                                                                                                                                                                                                                                                                                                                          
end2:
	                                                                                                                                                                                                                                                                                                                                                     
    NDK_SysBeep();                                                                                                                                                                                                                                                                                                                                           
    NDK_ScrClrs();                                                                                                                                                                                                                                                                                                                                      
    NDK_ScrDispString(36, 24, "电量不足",0);                                                                                                                                                                                                                                                                                                          
    NDK_ScrDispString(24, 36, "无法执行打印",0); 
    NDK_ScrRefresh(); 
    NDK_KbGetCode(2, &ucKey);
    goto START_PRINT;                                                                                                                                                                                                                                                                                                                                       
    //return;                                                                                                                                                                                                                                                                                                                                               
} 

void printAD()
{
	int ret;
	int ucKey;

  NDK_ScrClrs();
	NDK_ScrDispString(36, 24, "正在打印...",0);
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
	    NDK_ScrDispString(36, 24, "打印失败",0);
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
    sprintf(printBuff,"序列号:%lld\n",query_number);
    NDK_PrnStr(printBuff);
    if(strlen(out_trade_no) > 0) {
        strcpy(printBuff,"商户订单号:");
        strcat(printBuff,out_trade_no);
        NDK_PrnStr(printBuff);
        memset(out_trade_no,0, 65);
    }
    NDK_PrnStr("\n\n");

    NDK_PrnSetFont(PRN_HZ_FONT_32x32, PRN_ZM_FONT_16x32 );
    strcpy(printBuff,"您本次消费金额: ");
    strcat(printBuff,price);
    NDK_PrnStr(printBuff);
    NDK_PrnStr("\n\n");
    NDK_PrnSetFont(PRN_HZ_FONT_24x24, PRN_ZM_FONT_12x24A);
    strcpy(printBuff," 本产品由盈润捷通提供技术支持\n");
    NDK_PrnStr(printBuff);
    NDK_PrnSetFont(PRN_HZ_FONT_24x24, PRN_ZM_FONT_12x24A);
    strcpy(printBuff,"     联系电话：4008190900\n");
    NDK_PrnStr(printBuff);
    strcpy(printBuff,"--------------------------------");
    NDK_PrnStr(printBuff);
    strcpy(printBuff,"以下广告位招商电话：4008190900\n");
    NDK_PrnStr(printBuff);
    NDK_PrnStr("\n");

    



    //开始打印    
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

    NDK_PrnFeedPaper(PRN_FEEDPAPER_AFTER);   

    return;

end1:
	                                                                                                                                                                                                                                                                                                                                                     
    NDK_SysBeep();                                                                                                                                                                                                                                                                                                                                           
    NDK_ScrClrs();                                                                                                                                                                                                                                                                                                                                      
    NDK_ScrDispString(24, 24, "请检查打印机", 0);                                                                                                                                                                                                                                                                                                          
    NDK_ScrDispString(36, 36, "打印失败", 0); 
    NDK_ScrRefresh(); 
    NDK_KbGetCode(2, &ucKey);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
    return; 
                                                                                                                                                                                                                                                                                                                                              
                                                                                                                                                                                                                                                                                                                                                          
end2:
	                                                                                                                                                                                                                                                                                                                                                     
    NDK_SysBeep();                                                                                                                                                                                                                                                                                                                                           
    NDK_ScrClrs();                                                                                                                                                                                                                                                                                                                                      
    NDK_ScrDispString(36, 24, "电量不足",0);                                                                                                                                                                                                                                                                                                          
    NDK_ScrDispString(24, 36, "无法执行打印",0); 
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

        NDK_ScrDispString(24, 24, "链接支付宝失败",0);
        NDK_ScrDispString(36, 36, "请检查网络",0);
        NDK_ScrRefresh();
        return ret;
    }    
    else {
        //TextOut(2, 4, ALIGN_CENTER, "input money OK!");
        NDK_ScrDispString(0, 36, "稍等，正在输出二维码...",0);
        NDK_ScrRefresh();
        printTail(total_fee,commTestOut.out_trade_no);
    }              
 	                                                                                                 
    if(query_count == 0)                                                                                
            alarm(10);                                                                                  
    else                                                                                             
       query_count = 10;                                                                                 
                                                                                                        
    return ret;                                                                                          
 }
 
#if 0 	                                                                                                 
//user don't need to input imsi and year month date, but hour,minutes,and serial no is needed,
void getSNoPre(char* prefix_str)
{
    //T_DATETIME tTime;
    struct tm *ptr;
    time_t td;
    char ticket_number[13]={0};
    char client_number[21]={0};
    
    
    getIMSIconfig();
    //GetDateTime(&tTime);
    time(&td);
    ptr = localtime(&td);

    memset(ticket_number, 0, 13);
    memset(client_number, 0, 21);
    client_number[0] = '1'; //to avoid atoi bug
    //sprintf(ticket_number,"%s%s%s\0",
            //tTime.year, tTime.month, tTime.day);
    strftime(ticket_number,sizeof(ticket_number),"%Y-%m-%d",ptr);
    /* use last 4-bit of IMSI */
    strncpy(client_number+1, &(qrpay_info.imsi[11]), 5);
    strcat(client_number, ticket_number);
    memcpy(prefix_str, client_number,strlen(client_number));
    DebugErrorInfo("ticket_number:%s, client_number:%s, prefix:%s\n",
            ticket_number, client_number, prefix_str);
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
#endif
 
void querySingle(void)
{
    char prefix[12] = {0};
    unsigned long long prefixint;
    char hmno[7] = {0};
    char queryNo[18] = {0};
    int ret = 0; 
    EM_PRN_STATUS PrnStatus; 
    int ucKey;

    NDK_ScrClrs();                                                                                                                                                                                                                                                                                                                                      
    NDK_ScrDispString(0, 24, "请输入交易单上序列号的后6位",0);
    NDK_ScrDispString(0, 36, "查询当日交易",0);
    NDK_ScrRefresh();

    getSNoPre(prefix);
    DebugErrorInfo("queryNO prefixint:\n");   
    DebugErrorInfo("queryNO prefix:%s \n", prefix);   
    //sprintf(prefix, "%lld\0", prefixint);
    //TextOut(0, 5, ALIGN_LEFT, prefix);
    memcpy(queryNo,prefix,11);
    //ret = Input(0,5,queryNo,17,IME_NUMBER,WHITE, RED,FALSE,TRUE,FALSE);
		ret = NDK_KbGetInput(queryNo, 6, 17, NULL, INPUTDISP_OTHER, 0, INPUT_CONTRL_LIMIT_ERETURN);
		if(ret == NDK_ERR)
			return;

    //memcpy(queryNo,prefix,11);
    //memcpy(queryNo+11,hmno,6);
    DebugErrorInfo("queryNo:%s\n",queryNo);
    
    ret = viewsingle((void*)&commTestOut, queryNo);
    if(ret)
    {
        char PrintBuff[30];
        NDK_ScrClrs();
        NDK_ScrDispString(0, 24, "该单交易已成功",0);
        NDK_ScrDispString(0, 36, queryNo,0); 
START_PRINT:

        if(NDK_PrnInit(0) != NDK_OK) {                                                                                                                                                                                                                                                                                                
            DebugErrorInfo("the printer is not working well!\n");                                                                                                                                                                                                                                                                            
            goto end1;                                                                                                                                                                                                                                                                                                               
        }

        memset(PrintBuff,0,30);
        NDK_PrnSetFont(PRN_HZ_FONT_32x32, PRN_ZM_FONT_16x32 );
        strcpy(PrintBuff,"以下交易确已成功\n");
        NDK_PrnStr(PrintBuff);

        strcpy(PrintBuff,"序列号:\n");
        strcat(PrintBuff,queryNo);
        NDK_PrnStr(PrintBuff);
        NDK_PrnStr("\n");
                
        strcpy(PrintBuff,"交易时间:\n");
        strcat(PrintBuff, serial2date(queryNo));        
        NDK_PrnStr(PrintBuff);
        NDK_PrnStr("\n");	
           
        strcpy(PrintBuff,"商户订单号:\n");
        strcat(PrintBuff,commTestOut.out_trade_no);
        NDK_PrnStr(PrintBuff);	
        NDK_PrnStr("\n");
           
        strcpy(PrintBuff,"金额：\n");
        strcat(PrintBuff, commTestOut.total_fee);        
        NDK_PrnStr(PrintBuff);
        NDK_PrnStr("\n");

        //NDK_PrnStr("\n\n\n");	 
        ret = NDK_PrnStart();;

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
        NDK_PrnFeedPaper(PRN_FEEDPAPER_AFTER);   
        return;
end1:
	                                                                                                                                                                                                                                                                                                                                                     
        NDK_SysBeep();                                                                                                                                                                                                                                                                                                                                           
        NDK_ScrClrs();                                                                                                                                                                                                                                                                                                                                      
        NDK_ScrDispString(24, 24, "请检查打印机", 0);                                                                                                                                                                                                                                                                                                          
        NDK_ScrDispString(36, 36, "打印失败", 0); 
        NDK_ScrRefresh(); 
        NDK_KbGetCode(2, &ucKey);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
        return; 
                                                                                                                                                                                                                                                                                                                                              
                                                                                                                                                                                                                                                                                                                                                          
end2:
	                                                                                                                                                                                                                                                                                                                                                     
        NDK_SysBeep();                                                                                                                                                                                                                                                                                                                                           
        NDK_ScrClrs();                                                                                                                                                                                                                                                                                                                                      
        NDK_ScrDispString(36, 24, "电量不足",0);                                                                                                                                                                                                                                                                                                          
        NDK_ScrDispString(24, 36, "无法执行打印",0); 
        NDK_ScrRefresh(); 
        NDK_KbGetCode(2, &ucKey);                                                                                                                                                                                                                                                                                                                                       
        return;


    } else {
        NDK_ScrClrs();
        NDK_ScrDispString(24, 24, "该单交易失败", 0);
        NDK_ScrRefresh(); 
        NDK_KbGetCode(2, &ucKey);
    }

}                                                                                                                            
                                                                                                                            
                                                                                                                            
                                                                                                                                                                                                                                                                                                                                                        