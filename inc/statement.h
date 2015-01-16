#ifndef _STATEMENTB_H
#define _STATEMENTB_H

 struct pos_statement_of_account{
	    char ticket_number[32];
		char transaction_number[32];
		char account[32];
		char transaction_amount[16];
		char time_buf[9];
		char zone_buf[9];
};

unsigned char ToHex(char text);
int get_title(char** title,int *title_size);
#endif
