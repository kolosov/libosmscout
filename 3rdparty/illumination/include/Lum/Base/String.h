#ifndef LUM_BASE_STRING_H
#define LUM_BASE_STRING_H

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

#include <assert.h>

#include <limits>
#include <string>

#include <Lum/Private/ImportExport.h>

namespace Lum {
  namespace Base {

    extern LUMAPI std::wstring StringToWString(const std::string& text);
    extern LUMAPI std::string WStringToString(const std::wstring& text);
    extern LUMAPI std::string WStringToUTF8(const std::wstring& text);
    extern LUMAPI std::wstring UTF8ToWString(const std::string& text);

#if defined(HAVE_ICONV)
    extern LUMAPI char* WStringToChar16BE(const std::wstring& text, size_t& length);
#endif

    extern LUMAPI wchar_t GetDecimalPointWChar();
    extern LUMAPI std::wstring GetDecimalPointWString();

    extern LUMAPI bool GetDigitValue(wchar_t digit, size_t& result);
    extern LUMAPI char GetValueCharDigit(int value);
    extern LUMAPI wchar_t GetValueWCharDigit(int value);

    template<typename A>
    size_t NumberDigits(const A& a,size_t base=10)
    {
      A      value(a);
      size_t res=0;

      if (value<0) {
        res++;
      }

      while (value!=0) {
        res++;
        value=value/base;
      }

      return res;
    }

    template<typename A>
    bool StringToNumber(const std::string& string, A& a, size_t base=10)
    {
      assert(base<=16);

      std::string::size_type pos=0;
      bool                   minus=false;

      a=0;

      if (string.empty()) {
        return false;
      }

      if (!std::numeric_limits<A>::is_signed && string[0]=='-') {
        return false;
      }

      /*
        Special handling for the first symbol/digit (could be negative)
       */
      if (base==10 && string[0]=='-') {
        minus=true;
        pos=1;
      }
      else {
        size_t digitValue;

        if (!GetDigitValue(string[pos],digitValue)) {
          return false;
        }

        if (digitValue>=base) {
          return false;
        }

        /*
          For signed values with base!=10 we assume a negative value
        */
        if (digitValue==base-1 &&
            std::numeric_limits<A>::is_signed &&
            string.length()==NumberDigits(std::numeric_limits<A>::max())) {
          minus=true;
          a=base/2;
        }
        else {
          a=digitValue;
        }

        pos=1;
      }

      while (pos<string.length()) {
        size_t digitValue;

        if (!GetDigitValue(string[pos],digitValue)) {
          return false;
        }

        if (digitValue>=base) {
          return false;
        }

        if (std::numeric_limits<A>::max()/(A)base-(A)digitValue<a) {
          return false;
        }

        a=a*base+digitValue;

        pos++;
      }

      if (minus) {
        a=-a;
      }

      return true;
    }

    template<typename A>
    bool WStringToNumber(const std::wstring& string, A& a, size_t base=10)
    {
      assert(base<=16);

      std::string::size_type pos=0;
      bool                   minus=false;

      a=0;

      if (string.empty()) {
        return false;
      }

      if (!std::numeric_limits<A>::is_signed && string[0]==L'-') {
        return false;
      }

      if (base==10 && string[0]==L'-') {
        minus=true;
        pos=1;
      }
      else {
        size_t digitValue;

        if (!GetDigitValue(string[pos],digitValue)) {
          return false;
        }

        if (digitValue>=base) {
          return false;
        }

        if (digitValue==base-1 && string.length()==NumberDigits(std::numeric_limits<A>::max())) {
          minus=true;
          a=base/2;
        }
        else {
          a=digitValue;
        }

        pos=1;
      }

      while (pos<string.length()) {
        size_t digitValue;

        if (!GetDigitValue(string[pos],digitValue)) {
          return false;
        }

        if (digitValue>=base) {
          return false;
        }

        if (std::numeric_limits<A>::max()/(A)base-(A)digitValue<a) {
          return false;
        }

        a=a*base+digitValue;

        pos++;
      }

      if (minus) {
        a=-a;
      }

      return true;
    }

    template<typename A>
    std::wstring NumberToWString(const A& a, size_t base=10)
    {
      assert(base<=16);

      std::wstring res;
      A            value(a);
      bool         negative=false;

      if (std::numeric_limits<A>::is_signed) {
        if (value<0) {
          negative=true;
          value=-value;
        }
      }

      res.reserve(20);

      while (value!=0) {
        res.insert(0,1,GetValueWCharDigit(value%base));
        value=value/base;
      }

      if (res.empty()) {
        res.insert(0,1,L'0');
      }

      if (negative) {
        res.insert(0,1,L'-');
      }

      return res;
    }

    template<typename A>
    std::string NumberToString(const A& a, size_t base=10)
    {
      assert(base<=16);

      std::string res;
      A           value(a);
      bool        negative=false;

      if (std::numeric_limits<A>::is_signed) {
        if (value<0) {
          negative=true;
          value=-value;
        }
      }

      res.reserve(20);

      while (value!=0) {
        res.insert(0,1,GetValueCharDigit(value%base));
        value=value/base;
      }

      if (res.empty()) {
        res.insert(0,1,'0');
      }

      if (negative) {
        res.insert(0,1,'-');
      }

      return res;
    }

    extern LUMAPI void ToUpper(std::string& string);
    extern LUMAPI void ToLower(std::string& string);

    extern LUMAPI void ToUpper(std::wstring& string);
    extern LUMAPI void ToLower(std::wstring& string);

    extern LUMAPI std::string ByteSizeToString(double size);
    extern LUMAPI std::wstring ByteSizeToWString(double size);

    extern LUMAPI std::string TimeSpanToString(size_t seconds);
    extern LUMAPI std::wstring TimeSpanToWString(size_t seconds);

    extern LUMAPI bool MatchWildcard(const std::wstring& pattern,
                                     const std::wstring& string,
                                     bool caseMatch);
  }
}

#endif
