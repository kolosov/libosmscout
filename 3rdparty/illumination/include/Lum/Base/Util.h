#ifndef LUM_BASE_UTIL_H
#define LUM_BASE_UTIL_H

/*
  This source is part of the Illumination library
  Copyright (C) 2004  Tim Teulings

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

#include <string>

#include <Lum/Private/ImportExport.h>

namespace Lum {
  namespace Base {

    template<typename A>
    A RoundRange(const A& value, const A& min, const A& max)
    {
      if (value<min) {
        return min;
      }
      else if (value>max) {
        return max;
      }
      else {
        return value;
      }
    }

    extern LUMAPI int RoundDiv(int a,int b);
    extern LUMAPI int RoundUpEven(int a);
    extern LUMAPI int UpDiv(int a, int b);

    extern LUMAPI bool CopyFile(const std::wstring& source, const std::wstring& dest);
  }
}

#endif
