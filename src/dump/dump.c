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
   @file dump.c

   FeliCa ƒ_ƒ“ƒv
*/

#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <locale.h>

#include "felicalib.h"

static void printserviceinfo(uint16 s);
static void hexdump(uint8* addr, int n);

int _tmain(int argc, _TCHAR *argv[])
{
    pasori* p;
    felica  *f, *f2;
    int i, j, k;

    setlocale(LC_ALL, "Japanese");

    p = pasori_open(NULL);
    if (!p)
    {
        _ftprintf(stderr, _T("PaSoRi open failed.\n"));
        exit(1);
    }
    pasori_init(p);
    
    f = felica_polling(p, POLLING_ANY, 0, 0);
    if (!f)
    {
        _ftprintf(stderr, _T("Polling card failed.\n"));
        exit(1);
    }
    _tprintf(_T("# IDm: "));
    hexdump(f->IDm, 8);
    _tprintf(_T("\n"));
    _tprintf(_T("# PMm: "));
    hexdump(f->PMm, 8);
    _tprintf(_T("\n\n"));
    felica_free(f);

    f = felica_enum_systemcode(p);
    if (!f)
    {
        exit(1);
    }

    for (i = 0; i < f->num_system_code; i++)
    {
        _tprintf(_T("# System code: %04X\n"), N2HS(f->system_code[i]));
        f2 = felica_enum_service(p, N2HS(f->system_code[i]));
        if (!f2)
        {
            _ftprintf(stderr, _T("Enum service failed.\n"));
            exit(1);
        }
        
        _tprintf(_T("# Number of area = %d\n"), f2->num_area_code);
        for (j = 0; j < f2->num_area_code; j++)
        {
            _tprintf(_T("# Area: %04X - %04X\n"), f2->area_code[j], f2->end_service_code[j]);
        }            

        _tprintf(_T("# Number of service code = %d\n"), f2->num_service_code);
        for (j = 0; j < f2->num_service_code; j++)
        {
            uint16 service = f2->service_code[j];
            printserviceinfo(service);

            for (k = 0; k < 255; k++)
            {
                uint8 data[16];

                if (felica_read_without_encryption02(f2, service, 0, (uint8)k, data))
                {
                    break;
                }
                
                _tprintf(_T("%04X:%04X "), service, k);
                hexdump(data, 16);
                _tprintf(_T("\n"));
            }
        }
        _tprintf(_T("\n"));
        felica_free(f2);
    }

    felica_free(f);
    pasori_close(p);

    return 0;
}

static void printserviceinfo(uint16 s)
{
    TCHAR*  ident;

    switch ((s >> 1) & 0xf)
    {
        case 0: ident = _T("Area Code"); break;
        case 4: ident = _T("Random Access R/W"); break; 
        case 5: ident = _T("Random Access Read only"); break; 
        case 6: ident = _T("Cyclic Access R/W"); break; 
        case 7: ident = _T("Cyclic Access Read only"); break; 
        case 8: ident = _T("Purse (Direct)"); break;
        case 9: ident = _T("Purse (Cashback/decrement)"); break;
        case 10: ident = _T("Purse (Decrement)"); break;
        case 11: ident = _T("Purse (Read only)"); break;
        default: ident = _T("INVALID or UNKOWN"); break;
    }

    _tprintf(_T("# Serivce code = %04X : %s"), s, ident);
    if ((s & 0x1) == 0)
    {
        _tprintf(_T(" (Protected)"));
    }
    _tprintf(_T("\n"));
}

static void hexdump(uint8* addr, int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        _tprintf(_T("%02X "), addr[i]);
    }
}
