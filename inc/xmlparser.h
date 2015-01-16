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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "expat/expat.h"
#include "curl/curl.h"

struct MemoryStruct {
  char *memory;
  size_t size;
};

struct ParserStruct {
  int ok;
  size_t tags;
  size_t depth;
  struct MemoryStruct characters;
};

void startElement(void *userData, const XML_Char *name, const XML_Char **atts);

void characterDataHandler(void *userData, const XML_Char *s, int len);

void endElement(void *userData, const XML_Char *name);
void endElement1(void *userData, const XML_Char *name);
void endElement2(void *userData, const XML_Char *name);
void endElement3(void *userData, const XML_Char *name);
void endElementPrint(void *userData, const XML_Char *name);

size_t parseStreamCallback(void *contents, size_t length, size_t nmemb, void *userp);
