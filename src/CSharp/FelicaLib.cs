/*
 felicalib - FeliCa access wrapper library

 Copyright (c) 2007-2010, Takuya Murakami, All rights reserved.

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

///
/// Porting to x64 systems by DeForest(Hirokazu Hayashi)
///

using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Reflection;

namespace FelicaLib
{
    /// <summary>
    /// DLL遅延バインディングクラス
    /// </summary>
    public class BindDLL : IDisposable
    {
        [DllImport("kernel32", CharSet = CharSet.Unicode, SetLastError = true)]
        private static extern IntPtr LoadLibrary([MarshalAs(UnmanagedType.LPWStr)] string lpFileName);
        [DllImport("kernel32", SetLastError = true)]
        private static extern bool FreeLibrary(IntPtr hModule);
        [DllImport("kernel32", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = false)]
        private static extern IntPtr GetProcAddress(IntPtr hModule, [MarshalAs(UnmanagedType.LPStr)]  string lpProcName);

        private IntPtr _pModule;

        /// <summary>
        /// DLLのロード・オブジェクト生成
        /// </summary>
        /// <param name="szFilename">バインドするDLL名</param>
        public BindDLL(string szFilename)
        {
            _pModule = BindDLL.LoadLibrary(szFilename);
            if (_pModule != IntPtr.Zero)
            {
                return;
            }
            int nResult = Marshal.GetHRForLastWin32Error();
            throw Marshal.GetExceptionForHR(nResult);
        }

        /// <summary>
        /// 指定名のアンマネージ関数ポインタをデリゲートに変換
        /// </summary>
        /// <param name="szProcName">アンマネージ関数名</param>
        /// <param name="typDelegate">変換するデリゲートのType</param>
        /// <returns>変換したデリゲート</returns>
        public Delegate GetDelegate(string szProcName, Type typDelegate)
        {
            IntPtr pProc = BindDLL.GetProcAddress(_pModule, szProcName);
            if (pProc != IntPtr.Zero)
            {
                Delegate oDG = Marshal.GetDelegateForFunctionPointer(pProc, typDelegate);
                return oDG;
            }
            int nResult = Marshal.GetHRForLastWin32Error();
            throw Marshal.GetExceptionForHR(nResult);
        }

        #region IDisposable メンバ

        public void Dispose()
        {
            if (_pModule != IntPtr.Zero)
            {
                BindDLL.FreeLibrary(_pModule);
            }
        }

        #endregion
    }

    // システムコード
    enum SystemCode : int
    {
        Any = 0xffff,           // ANY
        Common = 0xfe00,        // 共通領域
        Cyberne = 0x0003,       // サイバネ領域

        Edy = 0xfe00,           // Edy (=共通領域)
        Suica = 0x0003,         // Suica (=サイバネ領域)
        QUICPay = 0x04c1,       // QUICPay
    }

    public class Felica : IDisposable
    {
        // 遅延ロード用Delegate定義
        private delegate IntPtr Pasori_open(String dummy);
        private delegate int Pasori_close(IntPtr p);
        private delegate int Pasori_init(IntPtr p);
        private delegate IntPtr Felica_polling(IntPtr p, ushort systemcode, byte rfu, byte time_slot);
        private delegate void Felica_free(IntPtr f);
        private delegate void Felica_getidm(IntPtr f, byte[] data);
        private delegate void Felica_getpmm(IntPtr f, byte[] data);
        private delegate int Felica_read_without_encryption02(IntPtr f, int servicecode, int mode, byte addr, byte[] data);

        // 遅延ロード用Delegate
        private Pasori_open pasori_open = null;
        private Pasori_close pasori_close = null;
        private Pasori_init pasori_init = null;
        private Felica_polling felica_polling = null;
        private Felica_free felica_free = null;
        private Felica_getidm felica_getidm = null;
        private Felica_getpmm felica_getpmm = null;
        private Felica_read_without_encryption02 felica_read_without_encryption02 = null;

        private string szDLLname = "";
        private BindDLL bdDLL = null;

        private IntPtr pasorip = IntPtr.Zero;
        private IntPtr felicap = IntPtr.Zero;

        /// <summary>
        /// コンストラクタ
        /// </summary>
        public Felica()
        {
            // x64対応 20100501 - DeForest
            try
            {
                // プラットフォーム別のロードモジュール名決定（x64/x86サポート、Iteniumはサポート外）
                if (System.IntPtr.Size >= 8)    // x64
                {
                    szDLLname = "felicalib64.dll";
                }
                else                // x86
                {
                    szDLLname = "felicalib.dll";
                }
                // DLLロード
                bdDLL = new BindDLL(szDLLname);
                // エントリー取得
                pasori_open = (Pasori_open)bdDLL.GetDelegate("pasori_open", typeof(Pasori_open));
                pasori_close = (Pasori_close)bdDLL.GetDelegate("pasori_close", typeof(Pasori_close));
                pasori_init = (Pasori_init)bdDLL.GetDelegate("pasori_init", typeof(Pasori_init));
                felica_polling = (Felica_polling)bdDLL.GetDelegate("felica_polling", typeof(Felica_polling));
                felica_free = (Felica_free)bdDLL.GetDelegate("felica_free", typeof(Felica_free));
                felica_getidm = (Felica_getidm)bdDLL.GetDelegate("felica_getidm", typeof(Felica_getidm));
                felica_getpmm = (Felica_getpmm)bdDLL.GetDelegate("felica_getpmm", typeof(Felica_getpmm));
                felica_read_without_encryption02 = (Felica_read_without_encryption02)bdDLL.GetDelegate("felica_read_without_encryption02", typeof(Felica_read_without_encryption02));
            }
            catch (Exception)
            {
                throw new Exception(szDLLname + " をロードできません");
            }

            pasorip = pasori_open(null);
            if (pasorip == IntPtr.Zero)
            {
                throw new Exception(szDLLname + " を開けません");
            }
            if (pasori_init(pasorip) != 0)
            {
                throw new Exception("PaSoRi に接続できません");
            }
        }

        #region IDisposable メンバ

        /// <summary>
        /// オブジェクト破棄時処理
        /// </summary>
        public void Dispose()
        {
            if (pasorip != IntPtr.Zero)
            {
                pasori_close(pasorip);
                pasorip = IntPtr.Zero;
            }
            if (bdDLL != null)
            {
                bdDLL = null;
            }
        }

        #endregion

        /// <summary>
        /// デストラクタ
        /// </summary>
        ~Felica()
        {
            Dispose();
        }

        /// <summary>
        /// ポーリング
        /// </summary>
        /// <param name="systemcode">システムコード</param>
        public void Polling(int systemcode)
        {
            felica_free(felicap);

            felicap = felica_polling(pasorip, (ushort)systemcode, 0, 0);
            if (felicap == IntPtr.Zero)
            {
                throw new Exception("カード読み取り失敗");
            }
        }

        /// <summary>
        /// IDm取得
        /// </summary>
        /// <returns>IDmバイナリデータ</returns>
        public byte[] IDm()
        {
            if (felicap == IntPtr.Zero)
            {
                throw new Exception("no polling executed.");
            }

            byte[] buf = new byte[8];
            felica_getidm(felicap, buf);
            return buf;
        }    

        /// <summary>
        /// PMm取得
        /// </summary>
        /// <returns>PMmバイナリデータ</returns>
        public byte[] PMm()
        {
            if (felicap == IntPtr.Zero)
            {
                throw new Exception("no polling executed.");
            }

            byte[] buf = new byte[8];
            felica_getpmm(felicap, buf);
            return buf;
        }    

        /// <summary>
        /// 非暗号化領域読み込み
        /// </summary>
        /// <param name="servicecode">サービスコード</param>
        /// <param name="addr">アドレス</param>
        /// <returns>データ</returns>
        public byte[] ReadWithoutEncryption(int servicecode, int addr)
        {
            if (felicap == IntPtr.Zero)
            {
                throw new Exception("no polling executed.");
            }

            byte[] data = new byte[16];
            int ret = felica_read_without_encryption02(felicap, servicecode, 0, (byte)addr, data);
            if (ret != 0)
            {
                return null;
            }
            return data;
        }
    }
}
