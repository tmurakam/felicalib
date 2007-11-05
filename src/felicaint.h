#ifndef	_FELICAINT_H
#define	_FELICAINT_H

typedef struct {
    uint8* system_code; // システムコード (2byte)
    uint8 time_slot; // タイムスロット (0x00, 0x01, 0x03, 0x07, 0x0f のいづれか)
} POLLING;

typedef struct {
    uint8* card_idm; // カードの IDm (8byte)
    uint8* card_pmm; // カードの PMm (8byte)
} CARD_INFO;

typedef struct {
    uint8 *card_idm;
    uint8 number_of_services;
    uint8 *service_code_list;
    uint8 number_of_blocks;
    uint8 *block_list;
} INSTR_READ_BLOCK;

typedef struct {
    uint8 *status_flag_1;
    uint8 *status_flag_2;
    uint8 *result_number_of_blocks;
    uint8 *block_data;
} OUTSTR_READ_BLOCK;

typedef struct {
    uint8 *card_idm;
} INSTR_REQ_SYSTEM_CODE;

typedef struct {
    uint8 number_of_system_codes;
    uint8 *system_code_list;
} OUTSTR_REQ_SYSTEM_CODE;

typedef struct {
    int buffer_size_of_area_codes;
    int buffer_size_of_service_codes;
    int offset_of_area_service_index;
} INSTR_SEARCH_SERVICE;

typedef struct {
    int num_service_codes;
    uint8 *service_code_list;
    int num_area_codes;
    uint8 *area_code_list;
    uint8 *end_service_code_list;
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
*/
struct strpasori {
    /** DLL ハンドル */
    HINSTANCE hInstDLL;

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
