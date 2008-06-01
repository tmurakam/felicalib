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
  @file suica.c

  suica 履歴ダンプ (入出場履歴は未対応)
*/

#include <stdio.h>

#include "felicalib.h"

static void suica_dump_history(uint8 *data);
static const char *consoleType(int ctype);
static const char *procType(int proc);
static int read4b(uint8 *p);
static int read2b(uint8 *p);

// サービスコード
#define SERVICE_SUICA_INOUT     0x108f
#define SERVICE_SUICA_HISTORY   0x090f


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
    
    f = felica_polling(p, POLLING_SUICA, 0, 0);
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
        if (felica_read_without_encryption02(f, SERVICE_SUICA_HISTORY, 0, (uint8)i, data)) {
            break;
        }
        suica_dump_history(data);
    }

    felica_free(f);
    pasori_close(p);

    return 0;
}

static void suica_dump_history(uint8 *data)
{
    int ctype, proc, date, time, balance, seq, region;
    int in_line, in_sta, out_line, out_sta;
    int yy, mm, dd;

    ctype = data[0];            // 端末種
    proc = data[1];             // 処理
    date = read2b(data + 4);    // 日付
    balance = read2b(data + 10);// 残高
    balance = N2HS(balance);
    seq = read4b(data + 12);
    region = seq & 0xff;        // Region
    seq >>= 8;                  // 連番

    out_line = -1;
    out_sta = -1;
    time = -1;

    switch (ctype) {
    case 0xC7:  // 物販
    case 0xC8:  // 自販機          
        time = read2b(data + 6);
        in_line = data[8];
        in_sta = data[9];
        break;

    case 0x05:  // 車載機
        in_line = read2b(data + 6);
        in_sta = read2b(data + 8);
        break;

    default:
        in_line = data[6];
        in_sta = data[7];
        out_line = data[8];
        out_sta = data[9];
        break;
    }

    printf("端末種:%s ", consoleType(ctype));
    printf("処理:%s ", procType(proc));

    // 日付
    yy = date >> 9;
    mm = (date >> 5) & 0xf;
    dd = date & 0x1f;
    printf("%02d/%02d/%02d ", yy, mm, dd);

    // 時刻
    if (time > 0) {
        int hh = time >> 11;
        int min = (time >> 5) & 0x3f;

        printf(" %02d:%02d ", hh, min);
    }
    
    printf("入:%x/%x ", in_line, in_sta);
    if (out_line != -1) {
        printf("出:%x/%x ", out_line, out_sta);
    }

    printf("残高:%d ", balance);
    printf("連番:%d\n", seq);
}

static const char *consoleType(int ctype)
{
    switch (ctype) {
    case 0x03: return "清算機";
    case 0x05: return "車載端末";
    case 0x08: return "券売機";
    case 0x12: return "券売機";
    case 0x16: return "改札機";
    case 0x17: return "簡易改札機";
    case 0x18: return "窓口端末";
    case 0x1a: return "改札端末";
    case 0x1b: return "携帯電話";
    case 0x1c: return "乗継清算機";
    case 0x1d: return "連絡改札機";
    case 0xc7: return "物販";
    case 0xc8: return "自販機";
    }
    return "???";
}

static const char *procType(int proc)
{
    switch (proc) {
    case 0x01: return "運賃支払";
    case 0x02: return "チャージ";
    case 0x03: return "券購";
    case 0x04: return "清算";
    case 0x07: return "新規";
    case 0x0d: return "バス";
    case 0x0f: return "バス";
    case 0x14: return "オートチャージ";
    case 0x46: return "物販";
    case 0x49: return "入金";
    case 0xc6: return "物販(現金併用)";
    }
    return "???";
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


