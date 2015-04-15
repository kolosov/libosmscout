#include <Lum/Model/String.h>

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

#include <assert.h>

namespace Lum {
  namespace Model {

    void String::SetMaxLength(size_t maxLength)
    {
      this->maxLength=maxLength;

      if (!IsNull() &&
          value.value.length()>maxLength) {
        SetNull();
      }

      this->Notify();
    }

    size_t String::GetMaxLength() const
    {
      return maxLength;
    }

    void String::Set(const std::wstring& value)
    {
      if (value.length()<=maxLength) {
        Simple<std::wstring>::Set(value);
      }
      else {
        SetNull();
      }
    }

    void String::Erase(size_t start, size_t length)
    {
      assert(!IsNull());

      value.value.erase(start,length);

      Notify();
    }

    void String::Insert(size_t pos, const std::wstring& text)
    {
      assert(!IsNull());

      if (value.value.length()+text.length()>maxLength) {
        return;
      }

      value.value.insert(pos,text);

      Notify();
    }

    size_t String::Length() const
    {
      return value.value.length();
    }

    bool String::Empty() const
    {
      return value.value.empty();
    }
  }
}
