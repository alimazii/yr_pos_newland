#ifndef aliqr_INCLUDED
#define aliqr_INCLUDED 
#include <stdio.h>


/* pay information struct (must) */
struct payInfo {
#if 1
    char imsi[16+1];
    char order_key[32+1];
    unsigned long long  order_number;
    char order_time[19+1];
    char order_subject[128+1];
    char total_fee[15+1];
    char refund_amount[9+1+2+1]; //refund
    int  max_time; // for query 120 = 2hours
    char time_mark[14+1]; // for query with time_mark
    char refund_status[30]; // for take out 
#endif
#ifdef BARCODE_EN
    char dynamic_id[18+1]; //payment id
#endif
#ifdef BAIDU_EN
  char pay_channel[8];   //payment channel:alipay,weixin,baidu etc.
#endif    
#if 0
    char *imsi;
    char *order_key;
    int  order_number;
    char *order_time;
    char *order_subject;
    char *total_fee;
#endif
}; 
#define QRRESULTSTR 102400
struct qr_result {
    char order[QRRESULTSTR];
    char is_success;
    char serial_number[64+1];
    char out_trade_no[65];
    char trade_no[64+1];
    char total_fee[9+1+2+1];
    char total_status[48+1];
    char qrcode[128+1];
    char time_mark[32];
    char refund_amount[10+1+2+1];
    char remain_amount[10+1+2+1];
    int order_total;
    char amount_total[10+1+2+1];
    char exchange_start_time[19+1];
    char exchange_end_time[19+1];
    char refund_status[30]; // for take out 
    char md5sum[32+1];
    char version[30+1];
#ifdef BAIDU_EN
    char pay_channel[8];
#endif
#ifdef ADVERTISEMENT_EN
    char receipt_ai[10];    //advertisement index
    char adv_qrcode[128+1]; //qrcode for advertisement
    char adv_text[60+1];    //advertisement text
#endif        
};
//#ifdef BAIDU_EN
//typedef struct {
//	  char amount[10];
//	  int  channel;
//}thr_data;	
//#endif
#define QRRESULT sizeof(struct qr_result)

#if 0
#define ALISER "182.92.173.31"
#else
//#define ALISER "182.92.8.2"
//#define ALISER "182.92.21.76"
#define ALISER "123.57.66.196"
#endif

#define ORDERKEY "11"
#define PORT 8080
#define SER_PORT "8080"

#define POSTPREORDER        "http://"ALISER":"SER_PORT"/qrcode/preorder/?"
#define POSTEXCHANGE        "http://"ALISER":"SER_PORT"/qrcode/exchange/?"
#define POSTEXCHANGEORDER   "http://"ALISER":"SER_PORT"/qrcode/exchangedorder/?"
#define POSTQUERY           "http://"ALISER":"SER_PORT"/qrcode/q/?"
#define POSTVIEW            "http://"ALISER":"SER_PORT"/qrcode/view/?"
#define POSTREFUND          "http://"ALISER":"SER_PORT"/qrcode/refund/?"
#define POSTTEMPLATEMD5     "http://"ALISER":"SER_PORT"/qrcode/template/md5/?"
#define POSTTEMPLATE        "http://"ALISER":"SER_PORT"/qrcode/template/?"
#define POSTLATESTMD5       "http://"ALISER":"SER_PORT"/qrcode/lastest/md5/?"
#define POSTLATEST          "http://"ALISER":"SER_PORT"/qrcode/lastest/?"
#ifdef  BARCODE_EN                             
#define CREATEANDPAY        "http://"ALISER":"SER_PORT"/qrcode/createandpay/?"
#endif
#ifdef  ADVERTISEMENT_EN
#define POSTINIT            "http://"ALISER":"SER_PORT"/qrcode/init/?"
#endif

#ifdef BAIDU_EN
#define PREORDER "i=%s&ot=%s&pc=%s&sj=%s&sn=%lld&tf=%s", order_info->imsi, order_info->order_time,order_info->pay_channel,order_info->order_subject, order_info->order_number, order_info->total_fee
#else
#define PREORDER "i=%s&ot=%s&sj=%s&sn=%lld&tf=%s", order_info->pay_channel, order_info->imsi, order_info->order_time,order_info->order_subject, order_info->order_number, order_info->total_fee
#endif
#define PREQUERYTIMEMASK "i=%s&tm=%s", order_info->imsi, order_info->time_mark
#define PREQUERYMAXTIME "i=%s&mt=%d", order_info->imsi, order_info->max_time
#define PREIMSI "i=%s", order_info->imsi
#define PREVIEW "i=%s&sn=%lld", order_info->imsi, order_info->order_number
#if 1
#define PREREFUND "i=%s&rfa=%s&sn=%lld", order_info->imsi, order_info->refund_amount,order_info->order_number
#else
#define PREREFUND "i=%s&sn=%lld", order_info->imsi, order_info->order_number
#endif
#ifdef BARCODE_EN
#define PRECREATE "di=%s&i=%s&ot=%s&sj=%s&sn=%lld&tf=%s", order_info->dynamic_id, order_info->imsi, order_info->order_time,order_info->order_subject, order_info->order_number, order_info->total_fee
#endif
#ifdef ADVERTISEMENT_EN
#define PREINIT "i=%s", order_info->imsi
#endif

enum precreate_type {
    ALI_PREORDER = 0, /* require an online order qrcode from alipay */
    ALI_EXCHANGE,
    ALI_EXCHANGEORDER,
    ALI_QUERY_TIMEMARK, /* query the payment status of order with timemask */
    ALI_QUERY_MAXTIME, /* query the payment status of order with timemask */
    ALI_QUERY_24H, /* query the recent 24h */
    ALI_VIEW_SINGLE, /* query the with serial no */
    ALI_REFUND,
    ALI_TEMPLATEMD5,
    ALI_TEMPLATE,
    ALI_LASTESTMD5,
    ALI_LASTEST,
#ifdef ADVERTISEMENT_EN
    ALI_PW_INIT,    /* init config and pic when power on */
#endif    
#ifdef BARCODE_EN    
    ALI_CREATEANDPAY
#endif    
};
struct receipt_info {
    char serial_number[24];
    char out_trade_no[16];
    char trade_no[32];
    char trade_status[16];
    char total_fee[16];
#ifdef BAIDU_EN
    char pay_channel[8];
#endif 
#ifdef REFUND_EN
    char refund_amount[14];
#endif    
};

/* single query parameters for multi payment results */
struct queryInfo {
    char imsi[16+1];
    char timemark[32];
};

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
       char rcp_title_company[32+1];    // СƱ���⹫˾���������16�������֣�Ӣ�����32��
       char rcp_tech_company[16+1];      // СƱ֧�ֹ�˾���������4�������֣�Ӣ�����8��
       char rcp_tech_number[12+1];      // СƱ֧�ֵ绰���������6�������֣�Ӣ�����12��
}T_RECEIPT;

void InitReceipt();
#endif /*RECEIPT_CONF*/

int alipay_precreate(char* precr, int* len, struct payInfo* order_info, int type);
char* alipay_postcreate(int type);
void DebugErrorInfo(char* lpszFormat, ...);
int getPosKey();
#ifdef CONFIG_INPUTKEY
int setPosKey();
#endif
void getIMSIconfig();
int getsubject(char* name,char* buf);
#endif

#ifdef ADVERTISEMENT_EN
/* configuration info request */
struct configInitResult {
    char is_success;
    char imsi[16+1];
    char pay_channel[8];    
    char init_adv_index[128];
    char init_del_index[128];

#ifdef BARCODE_EN
    char dynamic_id[18+1]; //payment id
#endif
 
};
#endif
/* configuration result */