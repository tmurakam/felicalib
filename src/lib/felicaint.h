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
  @file felicaint.h

  内部ヘッダ
*/

#ifndef	_FELICAINT_H
#define	_FELICAINT_H

/** @brief Polling 構造体 */
typedef struct {
    uint8* system_code;	///< システムコード (2byte, ネットワークバイトオーダ)
    uint8 time_slot;	///< タイムスロット (0x00, 0x01, 0x03, 0x07, 0x0f のいずれか)
} POLLING;

/** @brief カード情報構造体 */
typedef struct {
    uint8* card_idm; 			///< カードの IDm (8byte)
    uint8* card_pmm; 			///< カードの PMm (8byte)
} CARD_INFO;

/** @brief read_block_without_encryption 入力構造体 */
typedef struct {
    uint8 *card_idm;			///< IDm
    uint8 number_of_services;		///< サービスコード数
    uint8 *service_code_list;		///< サービスコードリスト
    uint8 number_of_blocks;		///< ブロック数
    uint8 *block_list;			///< ブロックリスト
} INSTR_READ_BLOCK;

/** @brief read_block_without_encryption 出力構造体 */
typedef struct {
    uint8 *status_flag_1;		///< ステータスフラグ1
    uint8 *status_flag_2;		///< ステータスフラグ2
    uint8 *result_number_of_blocks;	///< データブロック数
    uint8 *block_data;			///< データブロック
} OUTSTR_READ_BLOCK;

/** @brief polling_and_request_system_code 入力構造体 */
typedef struct {
    uint8 *card_idm;			///< IDm
} INSTR_REQ_SYSTEM_CODE;

/** @brief polling_and_request_system_code 出力構造体 */
typedef struct {
    uint8 number_of_system_codes;	///< システムコード数
    uint8 *system_code_list;		///< システムコード配列
} OUTSTR_REQ_SYSTEM_CODE;

/** @brief polling_and_search_service_code 入力構造体 */
typedef struct {
    int buffer_size_of_area_codes;	///< エリアコードバッファサイズ
    int buffer_size_of_service_codes;	///< サービスコードバッファサイズ
    int offset_of_area_service_index;	///< エリアサービスオフセット(?)
} INSTR_SEARCH_SERVICE;

/** @brief polling_and_search_service_code 出力構造体 */
typedef struct {
    int num_service_codes;		///< サービスコード数
    uint8 *service_code_list;		///< サービスコード配列
    int num_area_codes;			///< エリアコード数
    uint8 *area_code_list;		///< エリアコード配列
    uint8 *end_service_code_list;	///< エンドサービスコード配列
} OUTSTR_SEARCH_SERVICE;

/* DLL entries */
typedef BOOL (*initialize_library_t)(void);
typedef BOOL (*dispose_library_t)(void);
typedef BOOL (*open_reader_writer_auto_t)(void);
typedef BOOL (*close_reader_writer_t)(void);
typedef BOOL (*polling_and_get_card_information_t)(POLLING *, unsigned char *, CARD_INFO *);
typedef BOOL (*polling_and_request_system_code_t)(POLLING *, INSTR_REQ_SYSTEM_CODE *, 
						  CARD_INFO *, OUTSTR_REQ_SYSTEM_CODE *);
typedef BOOL (*polling_and_search_service_code_t)(POLLING *, INSTR_SEARCH_SERVICE *,
						  CARD_INFO *, OUTSTR_SEARCH_SERVICE *);
typedef BOOL (*read_block_without_encryption_t)(INSTR_READ_BLOCK *, OUTSTR_READ_BLOCK *);

/* structures */
/**
   @brief PaSoRi ハンドル

   felica.dll の DLL ハンドル、および DLL のエントリポイントを保持する。
*/
struct strpasori {
    HINSTANCE hInstDLL;	///< DLL ハンドル

#define declare_entry(f)	f ## _t f
    declare_entry(initialize_library);
    declare_entry(dispose_library);
    declare_entry(open_reader_writer_auto);
    declare_entry(close_reader_writer);
    declare_entry(polling_and_get_card_information);
    declare_entry(polling_and_request_system_code);
    declare_entry(polling_and_search_service_code);
    declare_entry(read_block_without_encryption);
#undef declare_entry
};

#endif
