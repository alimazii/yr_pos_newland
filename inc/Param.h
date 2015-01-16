/***************************************************************************
    �ṹ�����ñ����Լ���Ķ���
    Li,Hengbo
    2009-11-05
***************************************************************************/
#ifndef _PARAM_H_
#define _PARAM_H_


#define APP_VERSION				"Version 14.06.19"

#define SIZE_TEL_NO					20

#define SIZE_IP						15
#define SIZE_PORT					5

#define SIZE_TPDU					4

#define YY_MM_DD					1
#define YY_MM						2
#define MM_DD						3
#define YY_MM_DD_HH_MM				4



/*
 * ��̫������
 */
typedef struct
{
	char szServerIP[SIZE_IP+1];							//��̫�� ����IP��ַ
	char szServerPort[SIZE_PORT+1];						//��̫�� ����PORT
	char szLocalIP[SIZE_IP+1];							//��̫�� ����IP��ַ
	char szMask[SIZE_IP+1];								//��̫�� ��������
	char szGateway[SIZE_IP+1];							//��̫�� ����
}T_ETHERNET;

typedef struct
{
	char szTPDU[4+1];									//TPDU
	char szOutNo;										//���ߺ���ǰ׺
	int delay_times;									//���ߺ�����绰����֮�䰮����ʱ����(���Ÿ���)
	char szTelNo[SIZE_TEL_NO+1];						//�绰����
	BOOL bOut;											//�����߿���
	UINT uiModemDataNum;								//ͨѶ������

	char szGprsServerIP[SIZE_IP+1];						//GPRS IP��ַ
	char szGprsServerPort[SIZE_PORT+1];					//GPRS PORT
        char szGprsApn[20+1];
	char szGprsUser[32 + 1];
	char szGprsPwd[32 + 1];
	UINT uiGprsDataNum;									//ͨѶ������

	T_ETHERNET tEthernet;								//��̫��IP��ַ
	UINT uiEthDataNum;									//ͨѶ������
        UINT uiWifiDataNum;                 //ͨѶ������

	char szMemKey[6+1];									//MEM��д����Կ
	char szM1KeyA[12+1];								//M1��������Կ
	char szM1KeyB[12+1];								//M1��д����Կ
	 T_WIFI  t_Wifi;
	BOOL  bIsDC;			// TRUE�ǵ¿�,FALSE�ǻ���ǽ�
	BOOL  bAutoIP;
	char szPos_sn[9];
	BOOL bAutoWifi;
}T_TERM;


extern T_TERM gTerm;
void InitTerm();

/*
 *SMS��������
 */
typedef struct
{
	char SCA[16];			// ����Ϣ�������ĺ���(SMSC��ַ)
	char TP_PID;			// �û���ϢЭ���ʶ(TP-PID)
	char TP_DCS;			// �û���Ϣ���뷽ʽ(TP-DCS)
	UINT uiMax;
}T_SMS;

extern T_SMS gSMS;
void InitSMS();

/*
 *СƱ��ӡ����
 */
#ifdef RECEIPT_CONF
typedef struct
{
       char rcp_title_line1[16+1];      // СƱ�����һ�У��������8�������֣�Ӣ�����16��
       char rcp_title_line2[16+1];      // СƱ����ڶ��У��������8�������֣�Ӣ�����16��
       char rcp_title_address[32+1];    // СƱ�����ַ���������16�������֣�Ӣ�����32��
       char rcp_title_number[32+1];     // СƱ����绰���������16�������֣�Ӣ�����32��
       char rcp_title_company[32+1];    // СƱ����绰���������16�������֣�Ӣ�����32��
}T_RECEIPT;

extern T_RECEIPT gRCP;
void InitReceipt();
#endif /*RECEIPT_CONF*/
#endif

