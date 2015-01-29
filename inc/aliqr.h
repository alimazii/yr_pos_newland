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
};

#define QRRESULT sizeof(struct qr_result)

#if 0
#define ALISER "182.92.173.31"
#else
//#define ALISER "182.92.8.2"
#define ALISER "182.92.21.76"
#endif

#define ORDERKEY "11"

#define POSTPREORDER        "http://"ALISER":80/qrcode/preorder/?"
#define POSTEXCHANGE        "http://"ALISER":80/qrcode/exchange/?"
#define POSTEXCHANGEORDER   "http://"ALISER":80/qrcode/exchangedorder/?"
#define POSTQUERY           "http://"ALISER":80/qrcode/q/?"
#define POSTVIEW            "http://"ALISER":80/qrcode/view/?"
#define POSTREFUND          "http://"ALISER":80/qrcode/refund/?"
#define POSTTEMPLATEMD5     "http://"ALISER":80/qrcode/template/md5/?"
#define POSTTEMPLATE        "http://"ALISER":80/qrcode/template/?"
#define POSTLATESTMD5       "http://"ALISER":80/qrcode/lastest/md5/?"
#define POSTLATEST          "http://"ALISER":80/qrcode/lastest/?"

#if 1
#define PREORDER "i=%s&ot=%s&sj=%s&sn=%lld&tf=%s", order_info->imsi, order_info->order_time,order_info->order_subject, order_info->order_number, order_info->total_fee
#else
#define PREORDER "i=%s&ot=%s&sj=%s&sn=%lld&tf=%s&tp=13810362150", order_info->imsi, order_info->order_time,order_info->order_subject, order_info->order_number, order_info->total_fee
#endif
#define PREQUERYTIMEMASK "i=%s&tm=%s", order_info->imsi, order_info->time_mark
#define PREQUERYMAXTIME "i=%s&mt=%d", order_info->imsi, order_info->max_time
#define PREIMSI "i=%s", order_info->imsi
#define PREVIEW "i=%s&sn=%lld", order_info->imsi, order_info->order_number
#if 0
#define PREREFUND "i=%s&rfa=%s&sn=%lld", order_info->imsi, order_info->refund_amount,order_info->order_number
#else
#define PREREFUND "i=%s&sn=%lld", order_info->imsi, order_info->order_number
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
};
struct receipt_info {
    char serial_number[24];
    char out_trade_no[16];
    char trade_no[32];
    char trade_status[16];
    char total_fee[16];
};

/* single query parameters for multi payment results */
struct queryInfo {
    char imsi[16+1];
    char timemark[32];
};


int alipay_precreate(char* precr, int* len, struct payInfo* order_info, int type);
char* alipay_postcreate(int type);
void DebugErrorInfo(char* lpszFormat, ...);
int getPosKey();
void getIMSIconfig();
int getsubject(char* name,char* buf);
#endif
