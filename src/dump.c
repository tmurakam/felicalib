#include <stdio.h>

#include "felicalib.h"
//#include "felicaint.h"

static void printserviceinfo(uint16 s);
static void hexdump(uint8 *addr, int n);

int main()
{
    pasori *p;
    felica *f, *f2;
    int i, j, k;

    p = pasori_open(NULL);
    if (!p) {
	exit(1);
    }
    pasori_init(p);
    
    f = felica_polling(p, 0xffff, 0, 0);
    if (!f) {
	exit(1);
    }
    printf("# IDm: ");
    hexdump(f->IDm, 8);
    printf("\n");
    printf("# PMm: ");
    hexdump(f->PMm, 8);
    printf("\n");
    free(f);

    f = enum_systemcode(p);
    if (!f) {
	exit(1);
    }

    for (i = 0; i < f->num_system_code; i++) {
	printf("# System code: %04X\n", N2HS(f->system_code[i]));
	f2 = enum_service(p, N2HS(f->system_code[i]));
	if (!f2) {
	    exit(1);
	}
	
	printf("# Number of area = %d\n", f2->num_area_code);
	for (j = 0; j < f2->num_area_code; j++) {
	    printf("# Area: %04X - %04X\n", f2->area_code[j], f2->end_service_code[j]);
	}	    

	printf("# Number of service code = %d\n", f2->num_service_code);
	for (j = 0; j < f2->num_service_code; j++) {
	    uint16 service = f2->service_code[j];
	    printserviceinfo(service);

	    for (k = 0; k < 255; k++) {
		uint8 data[16];

		if (felica_read_without_encryption02(f2, service, 0, (uint8)k, data)) {
		    break;
		}
		
		printf("%04X:%04X ", service, k);
		hexdump(data, 16);
		printf("\n");
	    }
	}
	printf("\n");
    }

    return 0;
}

static void printserviceinfo(uint16 s)
{
    char *ident;

    switch ((s >> 1) & 0xf) {
    case 0: ident = "Area Code"; break;
    case 4: ident = "Random Access R/W"; break; 
    case 5: ident = "Random Access Read only"; break; 
    case 6: ident = "Cyclic Access R/W"; break; 
    case 7: ident = "Cyclic Access Read only"; break; 
    case 8: ident = "Purse (Direct)"; break;
    case 9: ident = "Purse (Cashback/decrement)"; break;
    case 10: ident = "Purse (Decrement)"; break;
    case 11: ident = "Purse (Read only)"; break;
    default: ident = "INVALID or UNKOWN"; break;
    }

    printf("# Serivce code = %04X : %s", s, ident);
    if ((s & 0x1) == 0) {
	printf(" (Protected)");
    }
    printf("\n");
}

static void hexdump(uint8 *addr, int n)
{
    int i;
    for (i = 0; i < n; i++) {
	printf("%02X ", addr[i]);
    }
}
