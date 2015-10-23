#include "NDK.h"
#include "input.h"

/* nX,nY should greater than 1, pszOut value: 123 instead of 1.23(decimal point is not included),nMinLen and nMaxLen
   only count the digits  */
int AmountInput(int nX, int nY, char* pszOut, int* pnOutLen, int nMinLen, int nMaxLen, int nTimeOut)
{
	  char	cCurLetter;
	  int		i,j;
	  int		x = 0,y = 0;
	  int		nKey;	/*处理按键信息*/
	  int		nNum = 0;			/*字符的个数*/
	  int		numoffset = 0;	
	  int		nShowChange = 1;	/*判断是否有效按键按下*/
	  int		tableoffset = 0;
    //int		nAmoutFlag = 0;
	  int		nMaxHzLines;
	  long int	lnBigNum,lnSmallNum;
	  uint unX,unY;
	  uint width,height;
	  uint font_width,font_height;
	  char	szTmp[80];			/*临时变量*/
	  char	szGetBuf[80];	

	  x = nX - 1;
	  y = nY - 1;	
	    
	  nNum = strlen(pszOut);
	  if (nNum > nMaxLen)
	  {
	  	return NDK_ERR;
	  }
	
	  NDK_ScrGetViewPort(&unX, &unY, &width,&height);
	  NDK_ScrGetFontSize(&font_width, &font_height);
	  //nMaxHzLines = (unScrHeight+1)/(unFontHeight+UI_GetHspace());
	  memset (szTmp, 0, sizeof(szTmp));
	  memset (szGetBuf, 0 ,sizeof(szGetBuf));	
	  
		if (nNum > 0) //如果预先有内容
		{
			if(nNum > 9)
			{//atol 最大值2147483647 (10位)
				memcpy(szTmp,pszOut,nNum-2);
				sprintf( szGetBuf, "%ld%s", atol(szTmp),pszOut+nNum-2 );
			}
			else
			{
				sprintf( szGetBuf, "%ld", atol(pszOut) );
			}
			nNum = strlen(szGetBuf);
		}

		#ifdef LANG_EN
		NDK_ScrDispString(font_width * 2, height - font_height * 2, "Back KEY to Correct" ,0);
		#else
		NDK_ScrDispString(font_width * 2, height - font_height * 2, "输错请按[退格]键" ,0);	
		#endif
		NDK_ScrRefresh();
		NDK_KbHit( &nKey );  
		
		for (;;)
		{
		if (nShowChange == 1)
		{			
				//sprintf(szTmp, "%12ld.%02ld", atol(szGetBuf) / 100, atol(szGetBuf) % 100);
				if(strlen(szGetBuf)>9)
				{//atol 最大值2147483647 (10位)
					memset(szTmp,0,sizeof(szTmp));
					memcpy(szTmp,szGetBuf,strlen(szGetBuf)-2);
					lnBigNum = atol(szTmp);
					memset(szTmp,0,sizeof(szTmp));
					memcpy(szTmp,szGetBuf+strlen(szGetBuf)-2,2);
					lnSmallNum= atol(szTmp);
					memset(szTmp,0,sizeof(szTmp));
					sprintf(szTmp, "%12ld.%02ld", lnBigNum, lnSmallNum);
				}
				else
				{
					sprintf(szTmp, "%12ld.%02ld", atol(szGetBuf) / 100, atol(szGetBuf) % 100);
				}
				//BsDispBigAscStr (x+unScrWidth/(unFontWidth/2)-15, y, szTmp, 15);	
				DebugErrorInfo("the amount sting is %s,nNum is %d, szTmp len is %d\n",szTmp,nNum,strlen(szTmp));
				NDK_ScrDispString(0, y, szTmp ,0);	
				//NDK_ScrDispString(width/2 - (font_width/2)*((nNum+15)/2), y, szTmp ,0);
				NDK_ScrRefresh();	
		}
		nShowChange = 1;
		/*按键处理*/
		//nKey = PubGetKeyCode(nTimeOut);
		NDK_KbGetCode(nTimeOut, &nKey);
		switch (nKey)
		{
		case 0:
			return NDK_ERR_TIMEOUT;
		/**<字母键*/
		case K_ZMK:
			/*如果字符串输入功能建按下输入字母符号*/
			break;
		case K_DOT:
			/*非一般字串模式下不允许输入'.'*/
      break;
    case K_ZERO:  
		case K_ONE:
		case K_TWO:
		case K_THREE:
		case K_FOUR:
		case K_FIVE:
		case K_SIX:
		case K_SEVEN:
		case K_EIGHT:
		case K_NINE:
			if(nKey == K_ZERO)
			{/*金额输入不能以0开始*/
				if (nNum == 0)
				{
					#if 0
					if (nMaxHzLines >= 8)
					{//GP710,GP730
						//PubBeep(3);
						nAmoutFlag = 1;
						//NDK_ScrDispString(unFontWidth, (unFontHeight+UI_GetHspace())*(nMaxHzLines-2), "不允许零金额输入",0);
					  #ifdef LANG_EN
		        NDK_ScrDispString(font_width, height - font_height * 2, "No ZERO Input" ,0);
		        #else
		        NDK_ScrDispString(font_width, height - font_height * 2, "不允许零金额输入" ,0);	
		        #endif
					}
					#endif
					break;
				}
			}
//			if (nMaxHzLines >= 8)
//			{//GP710,GP730
//				if (1 == nAmoutFlag)
//				{
//					//NDK_ScrDispString(unFontWidth, 14*8, "                ",0);
//					//TODO:PubClearLine(nMaxHzLines-1,nMaxHzLines-1);
//				}
//			}
			if (nNum >= nMaxLen)
			{
				//PubBeep(1);
			}			
			else 
			{
				szGetBuf[nNum] = nKey;
				nNum++;
				if(strlen(szGetBuf)>9)
				{//atol 最大值2147483647 (10位)
					memset(szTmp,0,sizeof(szTmp));
					memcpy(szTmp,szGetBuf,strlen(szGetBuf)-2);
					lnBigNum= atol(szTmp);
					if(lnBigNum > 200000000)
					{
						szGetBuf[--nNum] = 0;
						//PubBeep(1);
					}
				}
			}
			break;
		case K_BASP:
			if (nNum > 0)
			{
				szGetBuf[--nNum] = 0;

			}
			break;
		case K_ENTER:
			if ((nNum >= nMinLen) && (nNum <= nMaxLen))
			{
				/*清光标*/
				if( nMinLen == 1 ) //如果金额输入不允许0,
				{
					if( atol(szGetBuf) == 0 )
					{
						#if 0
						if (nMaxHzLines >= 8)
						{//GP710,GP730
							//PubBeep(3);
							nAmoutFlag = 1;
							//NDK_ScrDispString(unFontWidth, (unFontHeight+UI_GetHspace())*(nMaxHzLines-2), "不允许零金额输入",0);
							#ifdef LANG_EN
		          NDK_ScrDispString(font_width, height - font_height * 2, "No ZERO Input" ,0);
		          #else
		          NDK_ScrDispString(font_width, height - font_height * 2, "不允许零金额输入" ,0);	
		          #endif
		          NDK_ScrRefresh();
						}
						#endif
						break;
					}
				}
				//memcpy (pszOut, szGetBuf, nNum);
				//pszOut[nNum] = '\0'
				sprintf(pszOut, "%ld.%02ld", atol(szGetBuf) / 100, atol(szGetBuf) % 100);
				*pnOutLen = strlen(pszOut);
				return NDK_OK;
			}
			break;
		case K_ESC:
			/*清光标*/
//			if (nEditMask != INPUT_MODE_AMOUNT)
//			{
//				BsDispBigASC (x + nNum, y, POS_WHITE_CHAR);
//			}
			return NDK_ERR;

		case K_F1:
		case K_F2:
			nShowChange = 0;
			break;	
		default:
			nShowChange = 0;
			break;
		}
	}
}	