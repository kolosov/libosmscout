#ifndef LUM_MODEL_STRING_H
#define LUM_MODEL_STRING_H

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

#include <limits>
#include <string>

#include <Lum/Model/Simple.h>

namespace Lum {
  namespace Model {

    class LUMAPI String : public Simple<std::wstring>
    {
    private:
      size_t maxLength;

    public:
      String()
      : Simple<std::wstring>(),
        maxLength(std::numeric_limits<size_t>::max())
      {
        // no code
      }

      String(size_t maxLength)
      : Simple<std::wstring>(),
        maxLength(maxLength)
      {
        // no code
      }

      String(const std::wstring& value)
      : Simple<std::wstring>(value),
        maxLength(std::numeric_limits<size_t>::max())
      {
        // no code
      }

      String(const std::wstring& value, size_t maxLength)
      : Simple<std::wstring>(value),
        maxLength(maxLength)
      {
        // no code
      }

      void SetMaxLength(size_t maxLength);
      size_t GetMaxLength() const;

      void Set(const std::wstring& value);
      void Erase(size_t start, size_t length);
      void Insert(size_t pos, const std::wstring& text);
      size_t Length() const;
      bool Empty() const;
    };

#if defined(LUM_INSTANTIATE_TEMPLATES)
    LUM_EXPTEMPL template class LUMAPI Base::Reference<String>;
#endif

    typedef Base::Reference<String> StringRef;
  }
}

#endif
