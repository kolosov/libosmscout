#ifndef LUM_PRIVATE_IMPORT_EXPORT_H
#define LUM_PRIVATE_IMPORT_EXPORT_H

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

#include <Lum/Features.h>

// Shared library support
#if defined(__WIN32__) || defined(WIN32)
  #if defined(LUMDLL) 
    #if defined(DLL_EXPORT) || defined(_WINDLL)
      #define LUMAPI __declspec(dllexport)
      #define LUM_EXPTEMPL
    #else
      #define LUMAPI
    #endif
  #else
    #define LUMAPI __declspec(dllimport)
    #define LUM_EXPTEMPL extern
  #endif

  #define LUMDLLLOCAL

#else
  #define LUMIMPORT
  #define LUM_EXPTEMPL
  
  #if defined(ILLUMINATION_EXPORT_SYMBOLS)
    #define LUMEXPORT __attribute__ ((visibility("default")))
    #define LUMDLLLOCAL __attribute__ ((visibility("hidden")))
  #else
    #define LUMEXPORT
    #define LUMDLLLOCAL
  #endif

  #if defined(LUMDLL)
    #define LUMAPI LUMEXPORT
  #else
    #define LUMAPI LUMIMPORT
  #endif

#endif

// Throwable classes must always be visible on GCC in all binaries
#if defined(__WIN32__) || defined(WIN32)
  #define LUMEXCEPTIONAPI(api) api
#elif defined(ILLUMINATION_EXPORT_SYMBOLS)
  #define LUMEXCEPTIONAPI(api) LUMEXPORT
#else
  #define LUMEXCEPTIONAPI(api)
#endif

#if defined(_MSC_VER)
  #define LUM_INSTANTIATE_TEMPLATES
#endif  
#endif

