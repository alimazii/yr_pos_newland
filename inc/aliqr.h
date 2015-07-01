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
    char take_out_phone[30]; // for take out 
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
    char take_out_phone[30]; // for take out 
    char md5sum[32+1];
    char version[30+1];
#ifdef BAIDU_EN
    char pay_channel[8];
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

#define POSTPREORDER        "http://"ALISER":8080/qrcode/preorder/?"
#define POSTEXCHANGE        "http://"ALISER":8080/qrcode/exchange/?"
#define POSTEXCHANGEORDER   "http://"ALISER":8080/qrcode/exchangedorder/?"
#define POSTQUERY           "http://"ALISER":8080/qrcode/q/?"
#define POSTVIEW            "http://"ALISER":8080/qrcode/view/?"
#define POSTREFUND          "http://"ALISER":8080/qrcode/refund/?"
#define POSTTEMPLATEMD5     "http://"ALISER":8080/qrcode/template/md5/?"
#define POSTTEMPLATE        "http://"ALISER":8080/qrcode/template/?"
#define POSTLATESTMD5       "http://"ALISER":8080/qrcode/lastest/md5/?"
#define POSTLATEST          "http://"ALISER":8080/qrcode/lastest/?"
#ifdef  BARCODE_EN
#define CREATEANDPAY        "http://"ALISER":8080/qrcode/createandpay/?"
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
 *小票打印配置
 */
#ifdef RECEIPT_CONF
typedef struct
{
       char rcp_title_line1[16+1];      // 小票标题第一行，汉字最多8个，数字，英文最多16个
       char rcp_title_line2[16+1];      // 小票标题第二行，汉字最多8个，数字，英文最多16个
       char rcp_title_address[32+1];    // 小票标题地址，汉字最多16个，数字，英文最多32个
       char rcp_title_number[32+1];     // 小票标题电话，汉字最多16个，数字，英文最多32个
       char rcp_title_company[32+1];    // 小票标题公司，汉字最多16个，数字，英文最多32个
       char rcp_tech_company[8+1];      // 小票支持公司，汉字最多4个，数字，英文最多8个
       char rcp_tech_number[12+1];      // 小票支持电话，汉字最多6个，数字，英文最多12个
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
