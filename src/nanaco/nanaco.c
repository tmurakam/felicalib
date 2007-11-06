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
  @file nanaco.c

  nanaco ƒ_ƒ“ƒv
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

    p = pasori_open(NULL);
    if (!p) {
	fprintf(stderr, "PaSoRi open failed.\n");
	exit(1);
    }
    pasori_init(p);
    
    f = felica_polling(p, 0xfe00, 0, 0);
    if (!f) {
	fprintf(stderr, "Polling card failed.\n");
	exit(1);
    }
#if 0
    printf("# IDm: ");
    hexdump(f->IDm, 8);
    printf("\n");
    printf("# PMm: ");
    hexdump(f->PMm, 8);
    printf("\n\n");
#endif

    for (i = 0; ; i++) {
	uint8 data[16];
	if (felica_read_without_encryption02(f, 0x564f, 0, (uint8)i, data)) {
	    break;
	}
	nanaco_dump(data);
    }

    return 0;
}

static void nanaco_dump(uint8 *data)
{
    int value, yy, mm, dd, hh, min;

    switch (data[0]) {
    case 0x47:
	printf("Žx•¥     ");
	break;
    case 0x6f:
	printf("ƒ`ƒƒ[ƒW ");
	break;
    default:
	printf("•s–¾     ");
	break;
    }

    value = read4b(data + 1);
    printf("‹àŠz:%-6d‰~ ", value);

    value = read4b(data + 5);
    printf("Žc‚:%-6d‰~ ", value);

    value = read4b(data + 9);
    yy = value >> 21;
    mm = (value >> 17) & 0xf;
    dd = (value >> 12) & 0x1f;
    hh = (value >> 6) & 0x3f;
    min = value & 0x3f;
    printf("%02d/%02d/%02d %02d:%02d ", yy, mm, dd, hh, min);

    value = read2b(data + 13);
    printf("˜A”Ô: %d\n", value);
}

static int read4b(uint8 *p)
{
    int v;
    v = (*p++) << 24;
    v |= (*p++) << 16;
    v |= (*p++) << 8;
    v |= *p;
    return v;
}

static int read2b(uint8 *p)
{
    int v;
    v = (*p++) << 8;
    v |= *p;
    return v;
}


