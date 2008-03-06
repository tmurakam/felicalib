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
  @file edy.c

  edy 履歴ダンプ
*/

#include <stdio.h>
#include <time.h>

#include "felicalib.h"

static void edy_dump(uint8 *data);
static void analyzeTime(int n, struct tm *t);
static int read4b(uint8 *p);
static int read2b(uint8 *p);

// サービスコード
#define SERVICE_EDY     0x170f


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
    
    f = felica_polling(p, POLLING_EDY, 0, 0);
    if (!f) {
        fprintf(stderr, "Polling card failed.\n");
        exit(1);
    }

    printf("IDm: ");
    for (i = 0; i < 8; i++) {
        printf("%02x", f->IDm[i]);
    }
	printf("\n");

    for (i = 0; ; i++) {
        if (felica_read_without_encryption02(f, SERVICE_EDY, 0, (uint8)i, data)) {
            break;
        }
        edy_dump(data);
    }

    return 0;
}

static void edy_dump(uint8 *data)
{
    int proc, time, value, balance, seq, v;
    struct tm tt;

    v = read4b(data + 0);
    proc = v >> 24;             // 処理
    seq  = v & 0xffffff;        // 連番
    time = read4b(data + 4);    // 時刻
    value = read4b(data + 8);   // 金額
    balance = read4b(data + 12);// 残高        

    // 日付/時刻
    analyzeTime(time, &tt);
    printf("%d/%02d/%02d %02d:%02d:%02d ",
           tt.tm_year, tt.tm_mon, tt.tm_mday,
           tt.tm_hour, tt.tm_min, tt.tm_sec);

    switch (proc) {
    case 0x02:
        printf("チャージ ");
        break;
    case 0x20:
        printf("支払い   ");
        break;
    default:
        printf("????     ");
        break;
    }

    printf("金額:%-5d ", value);
    printf("残高:%-5d ", balance);
    printf("連番:%d\n", seq);
}

static void analyzeTime(int n, struct tm *t)
{
    time_t tt;
    struct tm *t2;

    // calculate day
    memset(t, 0, sizeof(*t));
    t->tm_year  = 2000 - 1900;
    t->tm_mon   = 0;
    t->tm_mday  = 1;
    t->tm_hour  = 0;
    t->tm_min   = 0;
    t->tm_sec   = 0;
    t->tm_isdst = -1;

    tt = mktime(t);
    tt += (n >> 17) * 24 * 60 * 60;

    t2 = localtime(&tt);
    memcpy(t, t2, sizeof(*t));

    t->tm_year += 1900;
    t->tm_mon += 1;

    // calculate time
    n = n & 0x1ffff;
    t->tm_sec = n % 60;
    n /= 60;
    t->tm_min = n % 60;
    t->tm_hour = n / 60;
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


