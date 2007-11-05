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

    /* service codes */
    uint8 num_area_code;
    uint16 area_code[MAX_AREA_CODE];
    uint16 end_service_code[MAX_AREA_CODE];
    uint8 num_service_code;
    uint16 service_code[MAX_SERVICE_CODE];
} felica;

/* constants */
/* system codes (Network Byte Order!) */
#define	POLLING_ANY	0xffff
#define	POLLING_SUICA	0x0003
#define	POLLING_EDY	0xfe00

/* endian */
#define	N2HS(x)		(((x) >> 8) & 0xff | ((x) << 8) & 0xff00)
#define	H2NS(x)		N2HS(x)

/* APIs */
pasori *pasori_open(char *);
void pasori_close(pasori *);

int pasori_init(pasori *);
felica* felica_polling(pasori *, uint16, uint8, uint8);
int felica_read_without_encryption02(felica *f, int servicecode, int mode, uint8 addr, uint8 *b);

felica * enum_systemcode(pasori *p);
felica * enum_service(pasori *p, uint16 systemcode);

