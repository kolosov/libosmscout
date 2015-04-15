#ifndef LUM_BASE_STATUS_H
#define LUM_BASE_STATUS_H

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

#include <string>

#include <Lum/Private/ImportExport.h>

namespace Lum {
  namespace Base {

    class LUMAPI Status
    {
    private:
      bool         success;
      std::wstring description;

    public:
      Status();
      Status(const Status& status);

      void ClearStatus();

      void SetError(const std::wstring& text);
      void SetToCurrentErrno();
#if defined(__WIN32__) || defined(WIN32)
      void SetToCurrentW32Error();
#endif

      std::wstring GetDescription() const;

      Status operator=(const Status& status);
      operator bool() const;
    };
  }
}

#endif

