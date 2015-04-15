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

#include <Lum/Base/Status.h>

#include <cerrno>
#include <cstring>

#include <Lum/Base/String.h>

#if defined(__WIN32__) || defined(WIN32)
  #include <Lum/OS/Win32/OSAPI.h>
#endif

namespace Lum {
  namespace Base {
    Status::Status()
    : success(true),
      description(L"OK")
    {
    }

    Status::Status(const Status& status)
    {
      success=status.success;
      description=status.description;
    }

    void Status::ClearStatus()
    {
      success=true;
      description=L"OK";
    }

    void Status::SetError(const std::wstring& description)
    {
      success=false;
      this->description=description;
    }

    /**
     Set the status to "error" and try to retrieve an apropiate error text by evaulating the errno variable.

     NOTE
     Only call this method if you know that there is an error and the "errno" variable
     describes the cause of it.
    */
    void Status::SetToCurrentErrno()
    {
      success=false;
      description=StringToWString(strerror(errno));
    }

#if defined(__WIN32__) || defined(WIN32)
    /**
     Set the status to "error" and try to retrieve an apropiate error text by evaluating the result of GetLastError()

     NOTE
     Only call this method if you know that there is an error and GetLastError()
     describes the cause of it.
    */
    void Status::SetToCurrentW32Error()
    {
      LPWSTR buffer;

      success=false;

      FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,NULL,
                     GetLastError(),
                     MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
                     (LPWSTR)&buffer,
                     0,
                     NULL);

      description=buffer;

      LocalFree(buffer);
    }
#endif

    std::wstring Status::GetDescription() const
    {
      return description;
    }

    Status Status::operator=(const Status& status)
    {
      if (&status!=this) {
        success=status.success;
        description=status.description;
      }

      return *this;
    }

    Status::operator bool() const
    {
      return success;
    }
  }
}

