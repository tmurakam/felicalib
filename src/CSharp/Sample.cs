using System;
using System.Collections.Generic;
using System.Text;
using FelicaLib;

namespace FelicaLib
{
    public class Nanaco
    {
        public static void Main()
        {
            try
            {
                using (Felica f = new Felica())
                {
                    readNanaco(f);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }

        private static void readNanaco(Felica f)
        {
            f.Polling((int)SystemCode.Common);
            byte[] data = f.ReadWithoutEncryption(0x558b, 0);
            if (data == null)
            {
                throw new Exception("nanaco ID ‚ª“Ç‚ÝŽæ‚ê‚Ü‚¹‚ñ");
            }
            Console.Write("Nanaco ID = ");
            for (int i = 0; i < 8; i++) {
                Console.Write(data[i].ToString("X2"));
            }
            Console.Write("\n");

            for (int i = 0; ; i++)
            {
                data = f.ReadWithoutEncryption(0x564f, i);
                if (data == null) break;

                switch (data[0])
                {
                    case 0x47:
                    default:
                        Console.Write("Žx•¥     ");
                        break;
                    case 0x6f:
                        Console.Write("ƒ`ƒƒ[ƒW ");
                        break;
                }

                int value = (data[9] << 24) + (data[10] << 16) + (data[11] << 8) + data[12];
                int year = (value >> 21) + 2000;
                int month = (value >> 17) & 0xf;
                int date = (value >> 12) & 0x1f;
                int hour = (value >> 6) & 0x3f;
                int min = value & 0x3f;

                Console.Write("{0}/{1:D2}/{2:D2} {3:D2}:{4:D2}", year, month, date, hour, min);

                value = (data[1] << 24) + (data[2] << 16) + (data[3] << 8) + data[4];
                Console.Write("  ‹àŠz {0,6}‰~", value);

                value = (data[5] << 24) + (data[6] << 16) + (data[7] << 8) + data[8];
                Console.Write("  Žc‚ {0,6}‰~", value);

                value = (data[13] << 8) + data[14];
                Console.WriteLine("  ˜A”Ô {0}", value);
            }
        }
    }
}
