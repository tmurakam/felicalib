using System;
using System.Collections.Generic;
using System.Text;

namespace FelicaLib
{
    public class Sample
    {
        public static void Main()
        {
            FelicaLib f = new FelicaLib();

            f.Polling(0xfe00);

            for (int i = 0; ; i++)
            {
                byte[] data = f.ReadWithoutEncryption(0x564f, i);
                if (data == null) break;

                switch (data[0])
                {
                    case 0x47:
                    default:
                        Console.Write("éxï•     ");
                        break;
                    case 0x6f:
                        Console.Write("É`ÉÉÅ[ÉW ");
                        break;
                }

                int value = (data[9] << 24) + (data[10] << 16) + (data[11] << 8) + data[12];
                int year = (value >> 21) + 2000;
                int month = (value >> 17) & 0xf;
                int date = (value >> 12) & 0x1f;
                int hour = (value >> 6) & 0x3f;
                int min = value & 0x3f;

                Console.Write("ì˙ït {0}/{1:2}/{2:2} {3:2}:{4:2} ", year, month, date, hour, min);

                value = (data[1] << 24) + (data[2] << 16) + (data[3] << 8) + data[4];
                Console.Write("ã‡äz " + value.ToString() + "â~ ");

                value = (data[5] << 24) + (data[6] << 16) + (data[7] << 8) + data[8];
                Console.Write("écçÇ " + value.ToString() + "â~ ");

                value = (data[13] << 8) + data[14];
                Console.WriteLine("òAî‘ " + value);
            }
        }
    }
}
