/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2013, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at http://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/
/* Stream-parse a document using the streaming Expat parser.
 * Written by David Strauss
 *
 * Expat => http://www.libexpat.org/
 *
 * gcc -Wall -I/usr/local/include xmlstream.c -lcurl -lexpat -o xmlstream
 *
 */
#include <pthread.h>

#include "xmlparser.h"
#include "aliqr.h"
#ifdef ST_QRCODE
char stqrcode[QRRESULTSTR]={0};
#endif
char timemark[32]={0};
#if 0
char qrout_trade_no[65]={0};
#endif
static struct qr_result* st_query_result;  
extern char time_mark[32];
extern pthread_mutex_t prmutex;

int alipay_main(struct qr_result *query_result, struct payInfo* order_info, int order_type)
{
	  pthread_mutex_lock(&prmutex);
    CURL *curl;
    CURLcode res;
    char https_req[1024*3];
    int req_len = 0;

    XML_Parser parser;
    struct ParserStruct state;
    st_query_result = query_result;
    memset(query_result, 0, sizeof(struct qr_result));
    /* Initialize the state structure for parsing. */
    memset(&state, 0, sizeof(struct ParserStruct));
    state.ok = 1;

    /* Initialize a namespace-aware parser. */
    parser = XML_ParserCreateNS(NULL, '\0');
    XML_SetUserData(parser, &state);
    XML_SetElementHandler(parser, startElement, endElement);
    XML_SetCharacterDataHandler(parser, characterDataHandler);

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if(curl) {
        alipay_precreate(https_req, &req_len, order_info, order_type);
        printf("https_req:\n%s\n, len:%d\n", https_req, req_len);
        DebugErrorInfo("https_req:\n%s\n, len:%d\n", https_req, req_len);
        curl_easy_setopt(curl, CURLOPT_URL,alipay_postcreate(order_type));

        printf("https_req:\n%s\n",alipay_postcreate(order_type));
        DebugErrorInfo("https_req:\n%s\n",alipay_postcreate(order_type));
        curl_easy_setopt(curl, CURLOPT_POST,1);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS,https_req);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, req_len);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, parseStreamCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)parser);

#ifdef SKIP_PEER_VERIFICATION
        /*
         * If you want to connect to a site who isn't using a certificate that is
         * signed by one of the certs in the CA bundle you have, you can skip the
         * verification of the server's certificate. This makes the connection
         * A LOT LESS SECURE.
         *
         * If you have a CA cert for the server stored someplace else than in the
         * default bundle, then the CURLOPT_CAPATH option might come handy for
         * you.
         */
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

#ifdef SKIP_HOSTNAME_VERIFICATION
        /*
         * If the site you're connecting to uses a different host name that what
         * they have mentioned in their server certificate's commonName (or
         * subjectAltName) fields, libcurl will refuse to connect. You can skip
         * this check, but this will make the connection less secure.
         */
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
            DebugErrorInfo("curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));        
        } else if (state.ok) {
            /* Expat requires one final call to finalize parsing. */
            if (XML_Parse(parser, NULL, 0, 1) == 0) {
                int error_code = XML_GetErrorCode(parser);
                fprintf(stderr, "Finalizing parsing failed with error code %d (%s).\n",
                        error_code, XML_ErrorString(error_code));
                DebugErrorInfo("Finalizing parsing failed with error code %d (%s).\n",
                        error_code, XML_ErrorString(error_code));
            }
            else {
                printf("                     --------------\n");
                printf("                     %lu tags total\n", state.tags);
                DebugErrorInfo("                     --------------\n");
                DebugErrorInfo("                     %lu tags total\n", state.tags);
                //printf("                     %s tags total\n", state.characters.memory);
                #ifdef ST_QRCODE
                //memcpy(qr_result,stqrcode,strlen(stqrcode));      
                if(stqrcode[0] != '\0' && time_mark[0] != '\0') {
                memcpy(query_result->order,stqrcode,strlen(stqrcode));
                DebugErrorInfo("the qr_result is %s\n, the stqrcode is %s\n",query_result->order,stqrcode);
                }
                memset(stqrcode,0,QRRESULTSTR);
                #endif
                memcpy(query_result->time_mark,timemark,strlen(timemark));                
#if 0
                if(stqrcode[0] != '\0') {
                    memcpy(query_result->order,stqrcode,strlen(stqrcode));
                    printf("the qr_result is %s\n, the stqrcode is %s sizeof(stqrcode):%d\n",query_result->order,stqrcode,QRRESULTSTR);
                    DebugErrorInfo("the qr_result is %s\n, the stqrcode is %s sizeof(stqrcode):%d\n",query_result->order,stqrcode,QRRESULTSTR);
                    memset(stqrcode,0, QRRESULTSTR);
                }
                if(strlen(timemark) > 0) {
                    memcpy(query_result->time_mark,timemark,strlen(timemark));
                }
                if(strlen(qrout_trade_no) > 0) {
                    memcpy(query_result->out_trade_no,qrout_trade_no,strlen(qrout_trade_no));
                }
#endif
            }

        }
        /* Clean up. */
        free(state.characters.memory);
        XML_ParserFree(parser);
        /* always cleanup */
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    pthread_mutex_unlock(&prmutex);
    printf("exit alipay_main\n");
    DebugErrorInfo("exit alipay_main\n");
    return 0;
}

void startElement(void *userData, const XML_Char *name, const XML_Char **atts)
{
  struct ParserStruct *state = (struct ParserStruct *) userData;
  state->tags++;
  state->depth++;

  /* Get a clean slate for reading in character data. */
  free(state->characters.memory);
  state->characters.memory = NULL;
  state->characters.size = 0;
}

void characterDataHandler(void *userData, const XML_Char *s, int len)
{
  struct ParserStruct *state = (struct ParserStruct *) userData;
  struct MemoryStruct *mem = &state->characters;

  mem->memory = realloc(mem->memory, mem->size + len + 1);
  if(mem->memory == NULL) {
    /* Out of memory. */
    fprintf(stderr, "Not enough memory (realloc returned NULL).\n");
    state->ok = 0;
    return;
  }

  memcpy(&(mem->memory[mem->size]), s, len);
  mem->size += len;
  mem->memory[mem->size] = 0;
}

void endElement(void *userData, const XML_Char *name)
{
    struct ParserStruct *state = (struct ParserStruct *) userData;
    state->depth--;
    if(state->characters.size)
    printf("%5lu    %5lu   %10lu   %s %s\n",state->tags, state->depth, state->characters.size, name, state->characters.memory);
    DebugErrorInfo("%5lu    %5lu   %10lu   %s %s\n",state->tags, state->depth, state->characters.size, name, state->characters.memory);

    if( strcmp(name,"o") == 0) {//order
        if(state->characters.memory != NULL) {
            memcpy(st_query_result->order, state->characters.memory,state->characters.size);
        }
    }
    if( strcmp(name,"rfo") == 0) {//order
        if(state->characters.memory != NULL) {
            strncat(st_query_result->order, "|",1);
            strcat(st_query_result->order, state->characters.memory);
        }
    }
    if( strcmp(name,"is") == 0) { //is_success
        if(state->characters.memory != NULL) {
            memcpy(&st_query_result->is_success, state->characters.memory,state->characters.size);
        }
    }

    if( strcmp(name,"sn") == 0) { //serial_number
        if(state->characters.memory != NULL) {
            memcpy(st_query_result->serial_number, state->characters.memory,state->characters.size);
        }
    }
    if( strcmp(name,"on") == 0) {//out_trade_no
        if(state->characters.memory != NULL) {
            memcpy(st_query_result->out_trade_no, state->characters.memory,state->characters.size);
        }
    }
    if( strcmp(name,"tn") == 0) {//trade_no
        if(state->characters.memory != NULL) {
            memcpy(st_query_result->trade_no, state->characters.memory,state->characters.size);
        }
    }
    if( strcmp(name,"tf") == 0) {//total_fee
        if(state->characters.memory != NULL) {
            memcpy(st_query_result->total_fee, state->characters.memory,state->characters.size);
        }
    }
    if( strcmp(name,"ts") == 0) {//total_status
        if(state->characters.memory != NULL) {
            memcpy(st_query_result->total_status, state->characters.memory,state->characters.size);
        }
    }
    if( strcmp(name,"q") == 0) {//qrcode
        if(state->characters.memory != NULL) {
            memcpy(st_query_result->qrcode, state->characters.memory,state->characters.size);
        }
    }
    if( strcmp(name,"tm") == 0) {//time_mark
        if(state->characters.memory != NULL) {
            //memcpy(st_query_result->time_mark, state->characters.memory,state->characters.size);  
            memcpy(timemark,state->characters.memory,state->characters.size); 
            DebugErrorInfo("%5lu   %10lu   %s %s\n", state->depth, state->characters.size, name, state->characters.memory);
        }
    }
    if( strcmp(name,"rfa") == 0) {//refund_amount
        if(state->characters.memory != NULL) {
            memcpy(st_query_result->refund_amount, state->characters.memory,state->characters.size);
        }
    }
    if( strcmp(name,"rma") == 0) {//remain_amount
        if(state->characters.memory != NULL) {
            memcpy(st_query_result->remain_amount, state->characters.memory,state->characters.size);
        }
    }
    if( strcmp(name,"to") == 0) {//order_total
        if(state->characters.memory != NULL) {
            st_query_result->order_total =atoi(state->characters.memory);
        }
    }
    if( strcmp(name,"at") == 0) {//amount_total
        if(state->characters.memory != NULL) {
            memcpy(st_query_result->amount_total, state->characters.memory,state->characters.size);
        }
    }
    if( strcmp(name,"est") == 0) {//exchange_start_time
        if(state->characters.memory != NULL) {
            memcpy(st_query_result->exchange_start_time, state->characters.memory,state->characters.size);
        }
    }
    if( strcmp(name,"eet") == 0) {//exchange_end_time
        if(state->characters.memory != NULL) {
            memcpy(st_query_result->exchange_end_time, state->characters.memory,state->characters.size);
        }
    }
    if( strcmp(name,"tp") == 0) {//take out telephone number 
        if(state->characters.memory != NULL) {
            memcpy(st_query_result->take_out_phone, state->characters.memory,state->characters.size);
        }
    }
    if( strcmp(name,"m") == 0) {//md5sum
        if(state->characters.memory != NULL) {
            memcpy(st_query_result->md5sum, state->characters.memory,state->characters.size);
        }
    }
    if( strcmp(name,"v") == 0) {//version
        if(state->characters.memory != NULL) {
            memcpy(st_query_result->version, state->characters.memory,state->characters.size);
        }
    }
#if 0
    if( strcmp(name,"result") == 0) {
        if(strstr(state->characters.memory, "qr.alipay.com")) {
         //   memcpy(stqrcode,state->characters.memory,state->characters.size);
            memcpy(st_query_result->qrcode, state->characters.memory,state->characters.size);
            printf("qrgenerate:%5lu   %10lu   %s %s\n", state->depth, state->characters.size, name, state->characters.memory);
        }
    }
#endif
#if 0
    if( strcmp(name,"result") == 0) {
        //get the qrout_trade_no should be sync with server and request
        if(state->tags == 11) {
            memcpy(qrout_trade_no,state->characters.memory,state->characters.size);
            printf("qrout_trade_no:%5lu   %10lu   %s %s\n", state->depth, state->characters.size, name, state->characters.memory);
        }
        //get the qrcode 
        if(strstr(state->characters.memory, "qr.alipay.com")) {
            memcpy(stqrcode,state->characters.memory,state->characters.size);
            memcpy(st_query_result->qrcode, state->characters.memory,state->characters.size);
            printf("qrgenerate:%5lu   %10lu   %s %s\n", state->depth, state->characters.size, name, state->characters.memory);
        }
    }
#endif
}

void endElementPrint(void *userData, const XML_Char *name)
{
#if 0
    struct ParserStruct *state = (struct ParserStruct *) userData;
    char PrintBuff[100] = {0};
    state->depth--;
    if(strcmp(name,"size") == 0){
            printf("%5lu   %10lu   %s %s\n", state->depth, state->characters.size, name, state->characters.memory);
        SetPrintFont(atoi(state->characters.memory));    
    } else if(strcmp(name,"front") != 0) {
            printf("%5lu   %10lu   %s %s\n", state->depth, state->characters.size, name, state->characters.memory);
        memcpy(PrintBuff,state->characters.memory,state->characters.size);
        FillPrintBuff(PrintBuff);
    }
    
#endif
}
size_t parseStreamCallback(void *contents, size_t length, size_t nmemb, void *userp)
{
  XML_Parser parser = (XML_Parser) userp;
  size_t real_size = length * nmemb;
  struct ParserStruct *state = (struct ParserStruct *) XML_GetUserData(parser);

  /* Only parse if we're not already in a failure state. */
  if (state->ok && XML_Parse(parser, contents, real_size, 0) == 0) {
    int error_code = XML_GetErrorCode(parser);
    fprintf(stderr, "Parsing response buffer of length %lu failed with error code %d (%s).\n",
            real_size, error_code, XML_ErrorString(error_code));
    state->ok = 0;
  }

  return real_size;
}
