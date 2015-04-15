#ifndef LUM_IMAGES_LOADER_H
#define LUM_IMAGES_LOADER_H

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

#include <list>
#include <string>

#include <Lum/Private/ImportExport.h>

#include <Lum/Base/Path.h>
#include <Lum/Base/Singleton.h>

#include <Lum/Images/Image.h>
#include <Lum/Images/StatefulImage.h>

namespace Lum {
  namespace Images {

    class LUMAPI Loader
    {
    public:
      virtual ~Loader();

      virtual bool CanLoad(const std::wstring& filename) const = 0;

      virtual bool Load(const std::wstring& filename,
                        Image* image) const = 0;
    };

    class LUMAPI Manager
    {
    private:
      std::list<Loader*>    loader;
      std::list<Base::Path> applicationSearchPaths;
      std::wstring          lightThemePostfix;
      std::wstring          darkThemePostfix;
      std::wstring          normalImagePostfix;
      std::wstring          normalSelectedImagePostfix;
      std::wstring          disabledImagePostfix;
      std::wstring          disabledSelectedImagePostfix;
      std::wstring          activatedImagePostfix;

    private:
      bool LoadApplication(const std::wstring& basename,
                           ImageRef &image, bool showError) const;

    public:
      Manager();
      ~Manager();

      Manager& AddApplicationSearchPath(const Base::Path& path);
      Manager& SetLightThemePostfix(const std::wstring& postfix);
      Manager& SetDarkThemePostfix(const std::wstring& postfix);
      Manager& SetNormalImagePostfix(const std::wstring& postfix);
      Manager& SetNormalSelectedImagePostfix(const std::wstring& postfix);
      Manager& SetDisabledImagePostfix(const std::wstring& postfix);
      Manager& SetDisabledSelectedImagePostfix(const std::wstring& postfix);
      Manager& SetActivatedImagePostfix(const std::wstring& postfix);

      bool Load(const std::wstring& filename,
                ImageRef &image) const;
      bool LoadApplication(const std::wstring& basename,
                           ImageRef &image) const;
      bool LoadApplication(const std::wstring& basename,
                           StatefulImageRef &image) const;
      bool LoadThemed(const std::wstring& filename,
                      ImageRef &image) const;
    };

    extern LUMAPI Base::Singleton<Manager> loader;
  }
}

#endif
