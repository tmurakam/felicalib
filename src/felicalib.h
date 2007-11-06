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
  @file felicalib.h

  メインヘッダ
*/
/**
   @mainpage

   API 仕様については、 felicalib.h を参照のこと。
*/
#ifndef _FELICALIB_H
#define	_FELICALIB_H

#include <windows.h>

typedef unsigned char uint8;
typedef unsigned short int uint16;

typedef struct strpasori pasori;

#define	MAX_SYSTEM_CODE		8
#define	MAX_AREA_CODE		16
#define MAX_SERVICE_CODE	256

/**
   @brief FeliCa ハンドル

   FeliCa の情報を格納する構造体
*/
typedef struct strfelica {
    pasori *p;		/**< PaSoRi ハンドル */
    uint16 systemcode;	/**< システムコード */
    uint8 IDm[8];	/**< IDm */
    uint8 PMm[8];	/**< PMm */

    /* systemcode */
    uint8 num_system_code;			/**< 列挙システムコード数 */
    uint16 system_code[MAX_SYSTEM_CODE];	/**< 列挙システムコード */

    /* area/service codes */
    uint8 num_area_code;			/**< エリアコード数 */
    uint16 area_code[MAX_AREA_CODE];		/**< エリアコード */
    uint16 end_service_code[MAX_AREA_CODE];	/**< エンドサービスコード */

    uint8 num_service_code;			/**< サービスコード数 */
    uint16 service_code[MAX_SERVICE_CODE];	/**< サービスコード */
} felica;

/* constants */
/* システムコード (ネットワークバイトオーダ/ビックエンディアンで表記) */
#define	POLLING_ANY	0xffff
#define	POLLING_EDY	0xfe00		/**< システムコード: 共通領域 (Edy などが使用) */
#define	POLLING_SUICA	0x0003		/**< システムコード: サイバネ領域 */

/* endian */
/** ネットワークバイトオーダからホストバイトオーダへの変換(16bit) */
#define	N2HS(x)		(((x) >> 8) & 0xff | ((x) << 8) & 0xff00)
/** ホストバイトオーダからネットワークバイトオーダへの変換(16bit) */
#define	H2NS(x)		N2HS(x)

/* APIs */
pasori *pasori_open(char *);
void pasori_close(pasori *);

int pasori_init(pasori *);
felica* felica_polling(pasori *, uint16, uint8, uint8);
int felica_read_without_encryption02(felica *f, int servicecode, int mode, uint8 addr, uint8 *b);

felica * felica_enum_systemcode(pasori *p);
felica * felica_enum_service(pasori *p, uint16 systemcode);

#endif /* _FELICALIB_H */

