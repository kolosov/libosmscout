#ifndef LUM_PRIVATE_ICONV_HELPER_H
#define LUM_PRIVATE_ICONV_HELPER_H

/*
  This source is part of the Illumination library
  Copyright (C) 2005  Tim Teulings

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

#include <Lum/Private/Config.h>

#include <iconv.h>

#if defined(ICONV_HAS_WCHAR_T)
#define ICONV_WCHAR_T "WCHAR_T"
#elif SIZEOF_WCHAR_T == 4
#define ICONV_WCHAR_T "UTF-32"
#elif SIZEOF_WCHAR_T == 2
#if defined(__WIN32__) || defined(WIN32)
#define ICONV_WCHAR_T "UTF-16LE"
#else
#define ICONV_WCHAR_T "UTF-16"
#endif
#elif SIZEOF_WCHAR_T == 1
#define ICONV_WCHAR_T "UTF-8"
#endif

#if defined(ICONV_SUPPORTS_EMPTY_STRING)
#define ICONV_EMPTY_STRING ""
#elif defined(HAVE_HEADER_LANGINFO)
#include <langinfo.h>
#define ICONV_EMPTY_STRING nl_langinfo(CODESET)
#else
  #if defined(__WIN32__) || defined(WIN32)
    #define ICONV_EMPTY_STRING "WINDOWS-1252"
  #else
    #define ICONV_EMPTY_STRING "ISO-8859-1"
  #endif  
#endif

#endif

