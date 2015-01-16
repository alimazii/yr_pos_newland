#ifndef QRGENERATOR_H
#define QRGENERATOR_H

int generator_qrcode_to_bmp(void* gout, char* price ,void* gin);
void getSNoPre(char* pre); //in arm can't return unsigned long long, maybe because the arm ASM return r0; only can return int32;
//int alipay_query_single(unsigned long long queryNo);
//int alipay_query_24h(char* query_24h);
void getIMSIconfig();
int viewsingle(void* gout, char* serial_number );
int createrefund(void* gout, char* serial_number, char* refund_amount );
int preImsi(void* gout, int precreate_type);
int qTimemark(void* gout, char* time_mark);
int qMaxtime(void* gout, int max_time);
#endif
