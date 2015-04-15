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

#include <Lum/Def/AppInfo.h>

#include <Lum/Base/L10N.h>

namespace Lum {
  namespace Def {

    AppInfo::AppInfo()
    : program(L"This Program"),
    version(L"0.0"),
    author(L"<anonymous>"),
    contact(L"<no contact info>"),
    copyright(L"<no copyright>"),
    license(L"<no license>"),
    description(L"A small but useful program")
    {
      // no code
    }

    void AppInfo::SetProgram(const std::wstring& program)
    {
      this->program=program;
    }

    void AppInfo::SetVersion(const std::wstring& version)
    {
      this->version=version;
    }

    void AppInfo::SetAuthor(const std::wstring& author)
    {
      this->author=author;
    }

    void AppInfo::SetContact(const std::wstring& contact)
    {
      this->contact=contact;
    }

    void AppInfo::SetCopyright(const std::wstring& copyright)
    {
      this->copyright=copyright;
    }

    void AppInfo::SetLicense(const std::wstring& license)
    {
      this->license=license;
    }

    void AppInfo::SetDescription(const std::wstring& description)
    {
      this->description=description;
    }

    std::wstring AppInfo::GetProgram() const
    {
      return program;
    }

    std::wstring AppInfo::GetVersion() const
    {
      return version;
    }

    std::wstring AppInfo::GetAuthor() const
    {
      return author;
    }

    std::wstring AppInfo::GetContact() const
    {
      return contact;
    }

    std::wstring AppInfo::GetCopyright() const
    {
      return copyright;
    }

    std::wstring AppInfo::GetLicense() const
    {
      return license;
    }

    std::wstring AppInfo::GetDescription() const
    {
      return description;
    }
  }
}

