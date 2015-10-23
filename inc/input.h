#ifndef _INPUT_H_
#define _INPUT_H_

#ifdef __cplusplus
extern "C" 
{
#endif

int AmountInput(int nX, int nY, char* pszOut, int* pnOutLen, int nMinLen, int nMaxLen, int nTimeOut);

#ifdef __cplusplus
}  /* end extern "C" */
#endif

#endif