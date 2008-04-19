/*
 felicalib - FeliCa access wrapper library

 Copyright (c) 2007, Takuya Murakami, All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are
 met:

 1. Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer. 

 2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution. 

 3. Neither the name of the project nor the names of its contributors
    may be used to endorse or promote products derived from this software
    without specific prior written permission. 

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**
  @file testap.c
*/

#include <stdio.h>

#include "felicalib.h"

static void nanaco_dump(uint8 *data);
static int read4b(uint8 *p);
static int read2b(uint8 *p);

int _tmain(int argc, _TCHAR *argv[])
{
    pasori *p;
    felica *f;
    int i;
    uint8 data[16];

    p = pasori_open(NULL);
    if (!p) {
        fprintf(stderr, "PaSoRi open failed.\n");
        exit(1);
    }
    pasori_init(p);
    
    f = felica_polling(p, 0x80cd, 0, 0);
    if (!f) {
        fprintf(stderr, "Polling card failed.\n");
        exit(1);
    }

    for (i = 0; i < 16; i++) {
        data[i] = i;
    }

    if (felica_write_without_encryption(f, 0x1009, 0, data)) {
        fprintf(stderr, "write failed.\n");
        exit(1);
    }
    if (felica_read_without_encryption02(f, 0x1009, 0, 0, data)) {
        fprintf(stderr, "Can't read.\n");
        exit(1);
    }
    for (i = 0; i < 16; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
    return 0;
}


