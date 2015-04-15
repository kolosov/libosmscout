#ifndef LUM_DEF_APPINFO_H
#define LUM_DEF_APPINFO_H

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

#include <string>

#include <Lum/Private/ImportExport.h>

namespace Lum {

  namespace Def {
    /**
      Holds some information about the current application.
    */
    class LUMAPI AppInfo
    {
    private:
      std::wstring program;
      std::wstring version;
      std::wstring author;
      std::wstring contact;
      std::wstring copyright;
      std::wstring license;
      std::wstring description;

    public:
      AppInfo();

      // Setter
      void SetProgram(const std::wstring& program);
      void SetVersion(const std::wstring& version);
      void SetAuthor(const std::wstring& author);
      void SetContact(const std::wstring& contact);
      void SetCopyright(const std::wstring& copyright);
      void SetLicense(const std::wstring& license);
      void SetDescription(const std::wstring& description);

      // Getter
      std::wstring GetProgram() const;
      std::wstring GetVersion() const;
      std::wstring GetAuthor() const;
      std::wstring GetContact() const;
      std::wstring GetCopyright() const;
      std::wstring GetLicense() const;
      std::wstring GetDescription() const;
    };
  }
}

#endif

