/*
  This source is part of the Illumination library
  Copyright (C) 2007  Tim Teulings

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/

#include <Lum/Base/String.h>

#include <Lum/Private/Config.h>

#include <sys/types.h>
#include <stdlib.h>
#if defined(HAVE_UNISTD_H)
  #include <unistd.h>
#endif

#include <cmath>
#include <cwchar>
#include <cstring>
#include <iomanip>
#include <memory>
#include <sstream>

#include <locale.h>

#if defined(HAVE_ICONV)
  #include <Lum/Private/IconvHelper.h>
#endif
#include <iostream>
#if defined(EXPLICIT_WSTRING)
template class std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> >;
#endif

namespace Lum {
  namespace Base {

    static const char* valueChar="0123456789abcdef";
    static const wchar_t* valueWChar=L"0123456789abcdef";

    wchar_t GetDecimalPointWChar()
    {
      struct lconv *l;

      l=localeconv();
      if (l!=NULL && l->decimal_point!=NULL) {
        return StringToWString(l->decimal_point)[0];
      }
      else {
        return L'.';
      }
    }

    std::wstring GetDecimalPointWString()
    {
      struct lconv *l;

      l=localeconv();
      if (l!=NULL && l->decimal_point!=NULL) {
        return StringToWString(l->decimal_point);
      }
      else {
        return L".";
      }
    }

    bool GetDigitValue(wchar_t digit, size_t& result)
    {
      switch (digit) {
      case L'1':
        result=1;
        return true;
      case L'2':
        result=2;
        return true;
      case L'3':
        result=3;
        return true;
      case L'4':
        result=4;
        return true;
      case L'5':
        result=5;
        return true;
      case L'6':
        result=6;
        return true;
      case L'7':
        result=7;
        return true;
      case L'8':
        result=8;
        return true;
      case L'9':
        result=9;
        return true;
      case L'0':
        result=0;
        return true;
      case L'a':
      case L'A':
        result=10;
        return true;
      case L'b':
      case L'B':
        result=11;
        return true;
      case L'c':
      case L'C':
        result=12;
        return true;
      case L'd':
      case L'D':
        result=13;
        return true;
      case L'e':
      case L'E':
        result=14;
        return true;
      case L'f':
      case L'F':
        result=15;
        return true;
      default:
        return false;
      }
    }

    char GetValueCharDigit(int value)
    {
      assert(value<16);

      return valueChar[value];
    }

    wchar_t GetValueWCharDigit(int value)
    {
      assert(value<16);

      return valueWChar[value];
    }

    void ToUpper(std::string& string)
    {
      for (size_t i=0; i<string.length(); i++) {
        string[i]=toupper(string[i]);
      }
    }

    void ToLower(std::string& string)
    {
      for (size_t i=0; i<string.length(); i++) {
        string[i]=tolower(string[i]);
      }
    }

    void ToUpper(std::wstring& string)
    {
      for (size_t i=0; i<string.length(); i++) {
        string[i]=toupper(string[i]);
      }
    }

    void ToLower(std::wstring& string)
    {
      for (size_t i=0; i<string.length(); i++) {
        string[i]=tolower(string[i]);
      }
    }

    std::string ByteSizeToString(double value)
    {
      std::stringstream buffer;

      buffer.imbue(std::locale(""));
      buffer.setf(std::ios::fixed);
      buffer << std::setprecision(1);

      if (ceil(value)>=1024.0*1024*1024*1024*0.5) {
        buffer << value/(1024.0*1024*1024*1024) << " TiB";
      }
      else if (ceil(value)>=1024.0*1024*1024*0.5) {
        buffer << value/(1024.0*1024*1024) << " GiB";
      }
      else if (ceil(value)>=1024.0*1024*0.5) {
        buffer << value/(1024.0*1024) << " MiB";
      }
      else if (ceil(value)>=1024.0*0.5) {
        buffer << value/1024.0 << " KiB";
      }
      else {
        buffer << value << " B";
      }

      return buffer.str();
    }

    std::wstring ByteSizeToWString(double value)
    {
      return StringToWString(ByteSizeToString(value));
    }

    std::string TimeSpanToString(size_t seconds)
    {
      std::stringstream buffer;

      buffer.imbue(std::locale(""));
      buffer.setf(std::ios::fixed);

      buffer << seconds/60;
      buffer << ":";
      buffer << (seconds%60)/10;
      buffer << seconds%10;

      return buffer.str();
    }

    std::wstring TimeSpanToWString(size_t seconds)
    {
      return StringToWString(TimeSpanToString(seconds));
    }

    bool MatchWildcardInternal(const std::wstring& pattern, size_t pPos,
                               const std::wstring& string, size_t sPos,
                               bool caseMatch)
    {
      if (pPos>=pattern.length()) {
        return sPos>=string.length();
      }

      switch (pattern[pPos]) {
      case L'*':
        if (MatchWildcardInternal(pattern,pPos+1,string,sPos,caseMatch)) {
          return true;
        }
        else if (sPos<string.length()) {
          return MatchWildcardInternal(pattern,pPos,string,sPos+1,caseMatch);
        }
        else {
          return false;
        }
      case L'?':
        return sPos<string.length() && MatchWildcardInternal(pattern,pPos+1,string,sPos+1,caseMatch);
      default:
        if (sPos>=string.length()) {
          return false;
        }

        if (caseMatch) {
          if (pattern[pPos]==string[sPos]) {
            return MatchWildcardInternal(pattern,pPos+1,string,sPos+1,caseMatch);
          }
          else {
            return false;
          }
        }
        else {
          if (toupper(pattern[pPos])==toupper(string[sPos])) {
            return MatchWildcardInternal(pattern,pPos+1,string,sPos+1,caseMatch);
          }
          else {
            return false;
          }
        }
      }
    }

    bool MatchWildcard(const std::wstring& pattern, const std::wstring& string, bool caseMatch)
    {
      return MatchWildcardInternal(pattern,0,string,0,caseMatch);
    }

#if defined(HAVE_MBSRTOWCS)
    std::wstring StringToWString(const std::string& text)
    {
      mbstate_t    state;

      const char   *in=text.c_str();
      wchar_t      *out=new wchar_t[text.length()+2];
      size_t       size;
      std::wstring result;

      memset(&state,0,sizeof(state));

      size=mbsrtowcs(out,&in,text.length()+2,&state);
      if (size!=(size_t)-1 && in==NULL) {
        result=std::wstring(out,size);
      }

      delete [] out;
      return result;
    }
#elif defined(HAVE_ICONV)
    std::wstring StringToWString(const std::string& text)
    {
      std::wstring res;
      iconv_t      handle;

      handle=iconv_open(ICONV_WCHAR_T,ICONV_EMPTY_STRING);
      if (handle==(iconv_t)-1) {
        return L"";
      }

      // length+1+1 to handle a potential BOM if ICONV_WCHAR_T is UTF-16 and to convert of \0
      size_t inCount=text.length()+1;
      size_t outCount=(text.length()+2)*sizeof(wchar_t);

      char    *in=const_cast<char*>(text.data());
      wchar_t *out=new wchar_t[text.length()+2];

      char *tmpOut=(char*)out;
      size_t tmpOutCount=outCount;
      if (iconv(handle,(ICONV_CONST char**)&in,&inCount,&tmpOut,&tmpOutCount)==(size_t)-1) {
        iconv_close(handle);
        delete [] out;
        return L"";
      }

      iconv_close(handle);

      // remove potential byte order marks
      if (sizeof(wchar_t)==4) {
        // strip off potential BOM if ICONV_WCHAR_T is UTF-32
        if (out[0]==0xfeff) {
          res=std::wstring(out+1,(outCount-tmpOutCount)/sizeof(wchar_t)-2);
        }
        else {
          res=std::wstring(out,(outCount-tmpOutCount)/sizeof(wchar_t)-1);
        }
      }
      else if (sizeof(wchar_t)==2) {
        // strip off potential BOM if ICONV_WCHAR_T is UTF-16
        if (out[0]==0xfeff || out[0]==0xfffe) {
          res=std::wstring(out+1,(outCount-tmpOutCount)/sizeof(wchar_t)-2);
        }
        else {
          res=std::wstring(out,(outCount-tmpOutCount)/sizeof(wchar_t)-1);
        }
      }
      else {
        res=std::wstring(out,(outCount-tmpOutCount)/sizeof(wchar_t)-1);
      }

      delete [] out;

      return res;
    }
#else
  #error Missing string conversion method!
#endif

#if defined(HAVE_WCSRTOMBS)
    std::string WStringToString(const std::wstring& text)
    {
      mbstate_t    state;

      const wchar_t *in=text.c_str();
      char          *out=new char[text.length()*4+1];
      size_t        size;
      std::string   result;

      memset(&state,0,sizeof(state));

      size=wcsrtombs(out,&in,text.length()*4+1,&state);
      if (size!=(size_t)-1 && in==NULL) {
        result=std::string(out,size);
      }

      delete [] out;
      return result;
    }
#elif defined(HAVE_ICONV)
    std::string WStringToString(const std::wstring& text)
    {
      iconv_t handle;

      handle=iconv_open(ICONV_EMPTY_STRING,ICONV_WCHAR_T);
      if (handle==(iconv_t)-1) {
        return "";
      }

      // length+1 to get the result '\0'-terminated
      size_t inCount=(text.length()+1)*sizeof(wchar_t);
      size_t outCount=text.length()*4+1; // UTF-8 is a potential legacy local encoding

      char *in=const_cast<char*>((const char*)text.c_str());
      char *out=new char[outCount];

      char   *tmpOut=out;
      size_t tmpOutCount=outCount;

      if (iconv(handle,(ICONV_CONST char**)&in,&inCount,&tmpOut,&tmpOutCount)==(size_t)-1) {
        iconv_close(handle);
        delete [] out;
        return "";
      }

      std::string res=out;

      delete [] out;

      iconv_close(handle);

      return res;
    }
#else
  #error Missing string conversion method!
#endif
    /**
      The following string conversion code is a modified version of code copied
      from the source of the ConvertUTF tool, as can be found for example at
      http://www.unicode.org/Public/PROGRAMS/CVTUTF/

      It is free to copy and use.
    */

    static const int halfShift=10; /* used for shifting by 10 bits */

    static const unsigned long halfBase=0x0010000UL;
    static const unsigned long halfMask=0x3FFUL;

    #define UNI_SUR_HIGH_START  0xD800
    #define UNI_SUR_HIGH_END    0xDBFF
    #define UNI_SUR_LOW_START   0xDC00
    #define UNI_SUR_LOW_END     0xDFFF
    #define UNI_MAX_BMP         0x0000FFFF
    #define UNI_MAX_UTF16       0x0010FFFF
    #define UNI_MAX_LEGAL_UTF32 0x0010FFFF

    static const unsigned long offsetsFromUTF8[6] = { 0x00000000UL, 0x00003080UL, 0x000E2080UL,
                                                      0x03C82080UL, 0xFA082080UL, 0x82082080UL };

    static const unsigned char firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

    static const char trailingBytesForUTF8[256] = {
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
    };

    std::string WStringToUTF8(const std::wstring& text)
    {
      std::string         result;
      char                buffer[4];
      const unsigned long byteMask=0xBF;
      const unsigned long byteMark=0x80;

      result.reserve(text.length()*4);

#if SIZEOF_WCHAR_T==4

      for (size_t i=0; i<text.length(); i++) {
        wchar_t             ch=text[i];
        unsigned short      bytesToWrite = 0;

        /* UTF-16 surrogate values are illegal in UTF-32 */
        if (ch>=UNI_SUR_HIGH_START && ch<=UNI_SUR_LOW_END) {
          return result;
        }

        /*
         * Figure out how many bytes the result will require. Turn any
         * illegally large UTF32 things (> Plane 17) into replacement chars.
        */
        if (ch<0x80) {
          bytesToWrite=1;
        }
        else if (ch<0x800) {
          bytesToWrite=2;
        }
        else if (ch<0x10000) {
          bytesToWrite=3;
        }
        else if (ch<=UNI_MAX_LEGAL_UTF32) {
          bytesToWrite=4;
        }
        else {
          return result;
        }

        switch (bytesToWrite) { /* note: everything falls through. */
        case 4:
          buffer[3]=(char)((ch | byteMark) & byteMask);
          ch>>=6;
        case 3:
          buffer[2]=(char)((ch | byteMark) & byteMask);
          ch>>=6;
        case 2:
          buffer[1]=(char)((ch | byteMark) & byteMask);
          ch>>=6;
        case 1:
          buffer[0]=(char)(ch | firstByteMark[bytesToWrite]);
        }

        result.append(buffer,bytesToWrite);
      }

#elif SIZEOF_WCHAR_T==2

      const wchar_t* source=text.c_str();

      while (source!=text.c_str()+text.length()) {
        wchar_t             ch;
        unsigned short      bytesToWrite=0;

        ch=*source++;

        /* If we have a surrogate pair, convert to UTF32 first. */
        if (ch>=UNI_SUR_HIGH_START && ch<=UNI_SUR_HIGH_END) {
          if (source>=text.c_str()+text.length()) {
            return result;
          }
          /* If the 16 bits following the high surrogate are in the source buffer... */
          unsigned long ch2 = *source++;
          /* If it's a low surrogate, convert to UTF32. */
          if (ch2>=UNI_SUR_LOW_START && ch2<=UNI_SUR_LOW_END) {
            ch=(wchar_t)(((ch-UNI_SUR_HIGH_START) << halfShift) + (ch2-UNI_SUR_LOW_START) + halfBase);
          }
          else { /* it's an unpaired high surrogate */
            return result;
          }
        }
        else {
          /* UTF-16 surrogate values are illegal in UTF-32 */
          if (ch>=UNI_SUR_LOW_START && ch<=UNI_SUR_LOW_END) {
            return result;
          }
        }

        /* Figure out how many bytes the result will require */
        if (ch<0x80) {
          bytesToWrite=1;
        }
        else if (ch<0x800) {
          bytesToWrite=2;
        }
        else if (ch<0x10000) {
          bytesToWrite=3;
        }
        else if (ch<0x110000) {
          bytesToWrite=4;
        }
        else {
          return result;
        }

        switch (bytesToWrite) { /* note: everything falls through. */
        case 4:
          buffer[3]=(char)((ch | byteMark) & byteMask);
          ch>>=6;
        case 3:
          buffer[2]=(char)((ch | byteMark) & byteMask);
          ch>>=6;
        case 2:
          buffer[1]=(char)((ch | byteMark) & byteMask);
          ch>>=6;
        case 1:
          buffer[0]=(char)(ch | firstByteMark[bytesToWrite]);
        }

        result.append(buffer,bytesToWrite);
      }

#endif

      return result;
    }

    std::wstring UTF8ToWString(const std::string& text)
    {
      std::wstring result;

      result.reserve(text.length());

#if SIZEOF_WCHAR_T==4
      const char* source=text.c_str();

      while (source!=text.c_str()+text.length()) {
        unsigned long  ch = 0;
        unsigned short extraBytesToRead=trailingBytesForUTF8[(unsigned char)*source];

        /*
         * The cases all fall through. See "Note A" below.
         */
        switch (extraBytesToRead) {
        case 5:
          ch+=(unsigned char)(*source);
          source++;
          ch<<=6;
        case 4:
          ch+=(unsigned char)(*source);
          source++;
          ch<<=6;
        case 3:
          ch+=(unsigned char)(*source);
          source++;
          ch<<=6;
        case 2:
          ch+=(unsigned char)(*source);
          source++;
          ch<<=6;
        case 1:
          ch+=(unsigned char)(*source);
          source++;
          ch<<=6;
        case 0:
          ch+=(unsigned char)(*source);
          source++;
        }
        ch-=offsetsFromUTF8[extraBytesToRead];

        if (ch<=UNI_MAX_LEGAL_UTF32) {
          /*
           * UTF-16 surrogate values are illegal in UTF-32, and anything
           * over Plane 17 (> 0x10FFFF) is illegal.
           */
          if (ch>=UNI_SUR_HIGH_START && ch<=UNI_SUR_LOW_END) {
            return result;
          }
          else {
            result.append(1,(wchar_t)ch);
          }
        }
        else { /* i.e., ch > UNI_MAX_LEGAL_UTF32 */
          return result;
        }
      }

#elif SIZEOF_WCHAR_T==2

      size_t idx=0;

      while (idx<text.length()) {
        unsigned long  ch=0;
        unsigned short extraBytesToRead=trailingBytesForUTF8[(unsigned char)text[idx]];

        /*
         * The cases all fall through. See "Note A" below.
         */

        switch (extraBytesToRead) {
        case 5:
          ch+=(unsigned char)text[idx];
          idx++;
          ch<<=6; /* remember, illegal UTF-8 */
        case 4:
          ch+=(unsigned char)text[idx];
          idx++;
          ch<<=6; /* remember, illegal UTF-8 */
        case 3:
          ch+=(unsigned char)text[idx];
          idx++;
          ch<<=6;
        case 2:
          ch+=(unsigned char)text[idx];
          idx++;
          ch<<=6;
        case 1:
          ch+=(unsigned char)text[idx];
          idx++;
          ch<<=6;
        case 0:
          ch+=(unsigned char)text[idx];
          idx++;
        }
        ch-=offsetsFromUTF8[extraBytesToRead];

        if (ch <= UNI_MAX_BMP) { /* Target is a character <= 0xFFFF */
          /* UTF-16 surrogate values are illegal in UTF-32 */
          if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
            return result;
          }
          else {
            result.append(1,(wchar_t)ch); /* normal case */
          }
        }
        else if (ch > UNI_MAX_UTF16) {
          return result;
        }
        else {
          /* target is a character in range 0xFFFF - 0x10FFFF. */
          ch -= halfBase;
          result.append(1,(wchar_t)((ch >> halfShift) + UNI_SUR_HIGH_START));
          result.append(1,(wchar_t)((ch & halfMask) + UNI_SUR_LOW_START));
        }
      }

#endif

      return result;
    }

#if defined(HAVE_ICONV)
    char* WStringToChar16BE(const std::wstring& text, size_t& length)
    {
      iconv_t handle;

      handle=iconv_open("UTF-16BE",ICONV_WCHAR_T);
      if (handle==(iconv_t)-1) {
        return NULL;
      }

      char   *out,*in;
      size_t inCount=(text.length()+1)*sizeof(wchar_t);
      size_t outCount=(text.length()+1)*2;

      in=const_cast<char*>((const char*)text.c_str());
      out=new char[outCount];

      char   *tmpIn=in;
      char   *tmpOut=out;
      size_t tmpOutCount=outCount;
      if (iconv(handle,(ICONV_CONST char**)&tmpIn,&inCount,&tmpOut,&tmpOutCount)==(size_t)-1) {
        iconv_close(handle);
        delete [] out;
        return NULL;
      }

      length=(outCount-tmpOutCount)/2-1;

      iconv_close(handle);

      return out;
    }
#endif
  }
}
