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
#include "NDK.h"
#include "aliqr.h"

#define _DEBUG_
int cIsDebug = 1; /* debug switch */ 

static struct payInfo commTestIn;
static struct qr_result commTestOut;


int main(void)
{
	  //Mis_Menu();
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
    	  
    
    /*初始化应用界面*/
    if(NDK_ScrInitGui()!=NDK_OK)
        return -1;
    /*设置为非自动刷新*/
    NDK_ScrAutoUpdate(0,NULL);
    /*设置背景图*/
    //if(NDK_ScrSetbgPic("disp_bg.jpg")!=NDK_OK)
        //return -1;
    NDK_ScrClrs();
    
    /* open the serial port to print log */
    ret = NDK_PortOpen(PORT_NUM_COM1, "115200,8,N,1");   
    if (ret == NDK_OK){
    	  NDK_ScrPrintf("Serial Port 1 Open OK!\n");
    }
    else{
    	  NDK_ScrPrintf("Serial Port 1 Open FAIL!\n");	
    }
    NDK_ScrRefresh();
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
    NDK_ScrPrintf("显示模块版本:%s\n",dispVer);
    NDK_ScrGetLcdSize(&scr_width,&scr_height);
    NDK_ScrPrintf("液晶尺寸:%dx%d\n",scr_width,scr_height);
    NDK_ScrGetViewPort(&x,&y,&width,&height);
    NDK_ScrPrintf("显示区域:%d-%d-%d-%d\n",x, y, width,height);
    NDK_ScrGetFontSize(&font_width, &font_height);
    NDK_ScrPrintf("当前字体尺寸为n%dx%d\n",font_width, font_height);
    NDK_ScrRefresh();
#endif
    
    ret = NDK_AppGetInfo(NULL,0,&PayAppInfo, sizeof(PayAppInfo));
    if (ret == NDK_OK){
    	  //NDK_ScrPrintf("Get Info OK,name: %s\n",PayAppInfo.szAppName);
    	  DebugErrorInfo("Get Info OK,name: %s,nSeriNo is %d\n",PayAppInfo.szAppName,PayAppInfo.nSeriNo);
    }
    else{
    	  //NDK_ScrPrintf("Get Info FAIL,ret=%d\n",ret);
    	  DebugErrorInfo("Get Info FAIL,ret=%d\n",ret);	
    }	  
 
    while(1)
    {
    	  NDK_ScrClrs();
    	  NDK_ScrStatusbar(STATUSBAR_DISP_ALL|STATUSBAR_POSITION_TOP);
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
			    	#if 0
			    	NDK_ScrClrs();
			    	NDK_ScrDispString(4, 0, ALIGN_CENTER, "欢迎使用支付宝钱包支付");
				    NDK_ScrDispString(20, 24, ALIGN_CENTER, "请按OK键输入金额");
				    NDK_ScrDispString(4, 36, ALIGN_CENTER, "按CANCEL键或者BACK键返回");
				    NDK_ScrRefresh();
				    #endif
				    ret = generator_qrcode_to_bmp((void*)&commTestOut,"0.01",(void*)&commTestIn);
				    
			    	
			    	break;
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
    char buff[128] = "";
    char buffshow[128] = "";
    int tempDataNum;

    memset(buff, 0, sizeof(buff));
    Clear();
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
    Clear();
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


