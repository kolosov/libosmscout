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

#include <iostream>

#include <Lum/Base/Path.h>
#include <Lum/Base/String.h>

struct Testcases
{
  std::wstring nativePath;
  std::wstring dir;
  std::wstring baseName;
};

static Testcases testcasesA[] = {
  { L"/",             L"/",            L"" },
  { L"hello",         L"",             L"hello" },
  { L"/hello",        L"/",            L"hello" },
  { L"//hello",       L"/",            L"hello" },
  { L"hello/",        L"hello",        L"" },
  { L"hello//",       L"hello",        L"" },
  { L"/hello/",       L"/hello",       L"" },
  { L"//hello//",     L"/hello",       L"" },
  { L"hello/hallo",   L"hello",        L"hallo" },
  { L"hello/hallo/",  L"hello/hallo",  L"" },
  { L"/hello/hallo",  L"/hello",       L"hallo" },
  { L"/hello/hallo/", L"/hello/hallo", L"" },
  { L"",L"",L"" }
};

static Testcases testcasesB[] = {
  { L"/",             L"/",            L"" },
  { L"/hello/hallo",  L"/hello",       L"hallo" },
  { L"",L"",L"" }
};

int main()
{
  size_t x=0;
  while (!testcasesA[x].nativePath.empty()) {
    Lum::Base::Path path;


    path.SetNativePath(testcasesA[x].nativePath);

    if (path.GetDir()!=testcasesA[x].dir) {
      std::cerr << "'" << Lum::Base::WStringToString(testcasesA[x].nativePath) << "'" << std::endl;
      std::cerr << "Dir:  '" << Lum::Base::WStringToString(path.GetDir()) << "' <-> '" << Lum::Base::WStringToString(testcasesA[x].dir) << "'" << std::endl;
      std::cerr << "Base: '" << Lum::Base::WStringToString(path.GetBaseName()) << "' <-> '" << Lum::Base::WStringToString(testcasesA[x].baseName) << "'" << std::endl;
      std::cerr << "ERROR!" << std::endl;
      return 1;
    }

    if (path.GetBaseName()!=testcasesA[x].baseName) {
      std::cerr << "'" << Lum::Base::WStringToString(testcasesA[x].nativePath) << "'" << std::endl;
      std::cerr << "Dir:  '" << Lum::Base::WStringToString(path.GetDir()) << "' <-> '" << Lum::Base::WStringToString(testcasesA[x].dir) << "'" << std::endl;
      std::cerr << "Base: '" << Lum::Base::WStringToString(path.GetBaseName()) << "' <-> '" << Lum::Base::WStringToString(testcasesA[x].baseName) << "'" << std::endl;
      std::cerr << "ERROR!" << std::endl;
      return 1;
    }

    x++;
  }

  x=0;
  while (!testcasesB[x].nativePath.empty()) {
    Lum::Base::Path path;


    path.SetNativeDir(testcasesB[x].dir);
    path.SetBaseName(testcasesB[x].baseName);

    if (path.GetPath()!=testcasesB[x].nativePath) {
      std::cerr << "'" << Lum::Base::WStringToString(testcasesB[x].dir) << "' + '" << Lum::Base::WStringToString(testcasesB[x].baseName) << "'" << std::endl;
      std::cerr << "NativePath:  '" << Lum::Base::WStringToString(path.GetPath()) << "' <-> '" << Lum::Base::WStringToString(testcasesB[x].nativePath) << "'" << std::endl;
      std::cerr << "ERROR!" << std::endl;
      return 1;
    }

    x++;
  }
}
