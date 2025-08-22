/*
 felicalib - FeliCa access wrapper library

 Copyright (c) 2007-2025, Takuya Murakami, All rights reserved.

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

  �����w�b�_
*/

#ifndef        _FELICAINT_H
#define        _FELICAINT_H

/** @brief Polling �\���� */
typedef struct {
    uint8* system_code;         ///< �V�X�e���R�[�h (2byte, �l�b�g���[�N�o�C�g�I�[�_)
    uint8 time_slot;            ///< �^�C���X���b�g (0x00, 0x01, 0x03, 0x07, 0x0f �̂����ꂩ)
} POLLING;

/** @brief �J�[�h���\���� */
typedef struct {
    uint8* card_idm;                    ///< �J�[�h�� IDm (8byte)
    uint8* card_pmm;                    ///< �J�[�h�� PMm (8byte)
} CARD_INFO;

/** @brief read_block_without_encryption ���͍\���� */
typedef struct {
    uint8 *card_idm;                    ///< IDm
    uint8 number_of_services;           ///< �T�[�r�X�R�[�h��
    uint8 *service_code_list;           ///< �T�[�r�X�R�[�h���X�g
    uint8 number_of_blocks;             ///< �u���b�N��
    uint8 *block_list;                  ///< �u���b�N���X�g
} INSTR_READ_BLOCK;

/** @brief read_block_without_encryption �o�͍\���� */
typedef struct {
    uint8 *status_flag_1;               ///< �X�e�[�^�X�t���O1
    uint8 *status_flag_2;               ///< �X�e�[�^�X�t���O2
    uint8 *result_number_of_blocks;     ///< �f�[�^�u���b�N��
    uint8 *block_data;                  ///< �f�[�^�u���b�N
} OUTSTR_READ_BLOCK;

/** @brief write_block_without_encryption ���͍\���� */
typedef struct {
    uint8 *card_idm;                    ///< IDm
    uint8 number_of_services;           ///< �T�[�r�X�R�[�h��
    uint8 *service_code_list;           ///< �T�[�r�X�R�[�h���X�g
    uint8 number_of_blocks;             ///< �u���b�N��
    uint8 *block_list;                  ///< �u���b�N���X�g
    uint8 *block_data;                  ///< �������݃f�[�^(16byte)
} INSTR_WRITE_BLOCK;                        

/** @brief read_block_without_encryption �o�͍\���� */
typedef struct {
    uint8 *status_flag_1;
    uint8 *status_flag_2;
} OUTSTR_WRITE_BLOCK;

/** @brief polling_and_request_system_code ���͍\���� */
typedef struct {
    uint8 *card_idm;                    ///< IDm
} INSTR_REQ_SYSTEM_CODE;

/** @brief polling_and_request_system_code �o�͍\���� */
typedef struct {
    uint8 number_of_system_codes;       ///< �V�X�e���R�[�h��
    uint8 *system_code_list;            ///< �V�X�e���R�[�h�z��
} OUTSTR_REQ_SYSTEM_CODE;

/** @brief polling_and_search_service_code ���͍\���� */
typedef struct {
    int buffer_size_of_area_codes;      ///< �G���A�R�[�h�o�b�t�@�T�C�Y
    int buffer_size_of_service_codes;   ///< �T�[�r�X�R�[�h�o�b�t�@�T�C�Y
    int offset_of_area_service_index;   ///< �G���A�T�[�r�X�I�t�Z�b�g(?)
} INSTR_SEARCH_SERVICE;

/** @brief polling_and_search_service_code �o�͍\���� */
typedef struct {
    int num_service_codes;              ///< �T�[�r�X�R�[�h��
    uint8 *service_code_list;           ///< �T�[�r�X�R�[�h�z��
    int num_area_codes;                 ///< �G���A�R�[�h��
    uint8 *area_code_list;              ///< �G���A�R�[�h�z��
    uint8 *end_service_code_list;       ///< �G���h�T�[�r�X�R�[�h�z��
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
typedef BOOL (*write_block_without_encryption_t)(INSTR_WRITE_BLOCK *, OUTSTR_WRITE_BLOCK *);

typedef BOOL (*set_polling_timeout_t)(unsigned long);
typedef BOOL (*set_retry_count_t)(unsigned long);

/* structures */
/**
   @brief PaSoRi �n���h��

   felica.dll �� DLL �n���h���A����� DLL �̃G���g���|�C���g��ێ�����B
*/
struct strpasori {
    HINSTANCE hInstDLL;         ///< DLL �n���h��

#define declare_entry(f)        f ## _t f
    declare_entry(initialize_library);
    declare_entry(dispose_library);
    declare_entry(open_reader_writer_auto);
    declare_entry(close_reader_writer);
    declare_entry(polling_and_get_card_information);
    declare_entry(polling_and_request_system_code);
    declare_entry(polling_and_search_service_code);
    declare_entry(read_block_without_encryption);
    declare_entry(write_block_without_encryption);
    declare_entry(set_polling_timeout);
    declare_entry(set_retry_count);
#undef declare_entry
};

#endif
