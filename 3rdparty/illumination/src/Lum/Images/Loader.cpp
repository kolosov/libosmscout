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

#include <Lum/Images/Loader.h>

#include <iostream>

#include <Lum/Private/Config.h>

#ifdef HAVE_LIB_PNG
#include <Lum/Images/Loader/PNG.h>
#endif

#ifdef HAVE_LIB_JPEG
#include <Lum/Images/Loader/JPEG.h>
#endif

#ifdef HAVE_LIB_TIFF
#include <Lum/Images/Loader/TIFF.h>
#endif

#ifdef HAVE_LIB_GIF
#include <Lum/Images/Loader/GIF.h>
#endif

#include <Lum/Base/Path.h>
#include <Lum/Base/String.h>

#include <Lum/OS/Display.h>
#include <Lum/OS/Theme.h>

namespace Lum {
  namespace Images {

    static Manager*          singletonStore;
    Base::Singleton<Manager> loader(&singletonStore);

    Loader::~Loader()
    {
      // no code
    }

    Manager::Manager()
    : lightThemePostfix(L"_light"),
      darkThemePostfix(L"_dark"),
      normalImagePostfix(L"_normal"),
      normalSelectedImagePostfix(L"_selected"),
      disabledImagePostfix(L"_disabled"),
      disabledSelectedImagePostfix(L"_disabled_selected"),
      activatedImagePostfix(L"_activated")
    {
#ifdef HAVE_LIB_PNG
      loader.push_back(new LoaderPNG);
#endif

#ifdef HAVE_LIB_JPEG
      loader.push_back(new LoaderJPEG);
#endif

#ifdef HAVE_LIB_TIFF
      loader.push_back(new LoaderTIFF);
#endif

#ifdef HAVE_LIB_GIF
      loader.push_back(new LoaderGIF);
#endif
    }

    Manager::~Manager()
    {
      for (std::list<Loader*>::iterator iter=loader.begin();
           iter!=loader.end();
           ++iter) {
        delete *iter;
      }
    }

    /**
     * Add the given path as application image search path. this paths will be used by the
     * family of LoadApplication methods.
     */
    Manager& Manager::AddApplicationSearchPath(const Lum::Base::Path& path)
    {
      applicationSearchPaths.push_back(path);

      return *this;
    }

    Manager& Manager::SetLightThemePostfix(const std::wstring& postfix)
    {
      lightThemePostfix=postfix;

      return *this;
    }

    Manager& Manager::SetDarkThemePostfix(const std::wstring& postfix)
    {
      darkThemePostfix=postfix;

      return *this;
    }

    Manager& Manager::SetNormalImagePostfix(const std::wstring& postfix)
    {
      normalImagePostfix=postfix;

      return *this;
    }

    Manager& Manager::SetNormalSelectedImagePostfix(const std::wstring& postfix)
    {
      normalSelectedImagePostfix=postfix;

      return *this;
    }

    Manager& Manager::SetDisabledImagePostfix(const std::wstring& postfix)
    {
      disabledImagePostfix=postfix;

      return *this;
    }

    Manager& Manager::SetDisabledSelectedImagePostfix(const std::wstring& postfix)
    {
      disabledSelectedImagePostfix=postfix;

      return *this;
    }

    Manager& Manager::SetActivatedImagePostfix(const std::wstring& postfix)
    {
      activatedImagePostfix=postfix;

      return *this;
    }

    /**
     * Loads the image fromt he given filesystem path.
     *
     * Returns: true if the image loading was successful, else false
     */
    bool Manager::Load(const std::wstring& filename,
                       ImageRef &image) const
    {
      assert(image.Valid());

      for (std::list<Loader*>::const_iterator iter=loader.begin();
           iter!=loader.end();
           ++iter) {
        if ((*iter)->CanLoad(filename)) {
          return (*iter)->Load(filename,image);
        }
      }

      return false;
    }

    /**
     * Loads an individual image from the filessytem using all configured application image
      search pathes for building an complete filesystem path.
     */
    bool Manager::LoadApplication(const std::wstring& basename,
                                  ImageRef &image,
                                  bool showError) const
    {
      assert(image.Valid());

      std::wstring            base,suffix,scheme;
      std::wstring::size_type pos;

      pos=basename.find_last_of('.');

      if (pos==std::wstring::npos) {
        base=basename;
        suffix=L"";
      }
      else {
        base=basename.substr(0,pos);
        suffix=basename.substr(pos);
      }

      if (OS::display->GetTheme()->IsLightTheme()) {
        scheme=lightThemePostfix;
      }
      else {
        scheme=darkThemePostfix;
      }

      for (std::list<Loader*>::const_iterator iter=loader.begin();
           iter!=loader.end();
           ++iter) {

        for (std::list<Base::Path>::const_iterator path=applicationSearchPaths.begin();
             path!=applicationSearchPaths.end();
             ++path) {
          Base::Path   p(*path);
          std::wstring filename;

          if (!scheme.empty()) {
            p.SetBaseName(base+scheme+suffix);
            filename=p.GetPath();

            if ((*iter)->CanLoad(filename) &&
                (*iter)->Load(filename,image)) {
              return true;
            }
          }

          p.SetBaseName(base+suffix);
          filename=p.GetPath();

          if ((*iter)->CanLoad(filename) &&
              (*iter)->Load(filename,image)) {
            return true;
          }

          p.AppendDir(Lum::Base::NumberToWString(OS::display->GetTheme()->GetIconWidth())+
                      L"x"+
                      Lum::Base::NumberToWString(OS::display->GetTheme()->GetIconHeight()));


          if (!scheme.empty()) {
            p.SetBaseName(base+scheme+suffix);
            filename=p.GetPath();

            if ((*iter)->CanLoad(filename) &&
                (*iter)->Load(filename,image)) {
              return true;
            }
          }

          p.SetBaseName(base+suffix);
          filename=p.GetPath();

          if ((*iter)->CanLoad(filename) &&
              (*iter)->Load(filename,image)) {
            return true;
          }
        }
      }

      if (showError) {
        for (std::list<Base::Path>::const_iterator path=applicationSearchPaths.begin();
             path!=applicationSearchPaths.end();
             ++path) {
          Base::Path   p(*path);

          if (!scheme.empty()) {
            p.SetBaseName(base+scheme+suffix);
            std::cerr << "Cannot load '" << Lum::Base::WStringToString(basename) << "' as '" << Lum::Base::WStringToString(p.GetPath()) << "'!" << std::endl;
          }

          p.SetBaseName(base+suffix);
          std::cerr << "Cannot load '" << Lum::Base::WStringToString(basename) << "' as '" << Lum::Base::WStringToString(p.GetPath()) << "'!" << std::endl;

          p.AppendDir(Lum::Base::NumberToWString(OS::display->GetTheme()->GetIconWidth())+
                      L"x"+
                      Lum::Base::NumberToWString(OS::display->GetTheme()->GetIconHeight()));

          if (!scheme.empty()) {
            p.SetBaseName(base+scheme+suffix);
            std::cerr << "Cannot load '" << Lum::Base::WStringToString(basename) << "' as '" << Lum::Base::WStringToString(p.GetPath()) << "'!" << std::endl;
          }

          p.SetBaseName(base+suffix);
          std::cerr << "Cannot load '" << Lum::Base::WStringToString(basename) << "' as '" << Lum::Base::WStringToString(p.GetPath()) << "'!" << std::endl;
        }
      }

      return false;
    }

    /**
     * Loads an individual image from the filessytem using all configured application image
      search pathes for building an complete filesystem path.
     */
    bool Manager::LoadApplication(const std::wstring& basename,
                                  ImageRef &image) const
    {
      return LoadApplication(basename,image,true);
    }

    /**
     * Loading an application image by using the configured application pathes as
     * possible filesystem pathes and using the configured application state images postfixes
     * as image postfix. Also support for light themed or dark themed variant of
     * images are also supported.
     *
     * Returns: True if the 'normal' images can be loaded, else false.
     *
     * Example: Loading an image 'help.png' with application path '/usr/local/share/app/'
     * and postfix "disabled" as postfix for an 'disabled' image and "_light"
     * as the postfix for a light theme will result in a
     * filesystem path
     * of '/usr/local/share/app/help_disabled_light.png'.
     */
    bool Manager::LoadApplication(const std::wstring& basename,
                                  StatefulImageRef &image) const
    {
      std::wstring            base,suffix;
      std::wstring::size_type pos;

      pos=basename.find_last_of('.');

      if (pos==std::wstring::npos) {
        base=basename;
        suffix=L"";
      }
      else {
        base=basename.substr(0,pos);
        suffix=basename.substr(pos);
      }

      ImageRef i=Lum::Images::Factory::factory->CreateImage();

      if (!LoadApplication(base+normalImagePostfix+suffix,i,true)) {
        return false;
      }

      image->SetNormalImage(i);

      i=Lum::Images::Factory::factory->CreateImage();

      if (!normalSelectedImagePostfix.empty() &&
          LoadApplication(base+normalSelectedImagePostfix+suffix,i,false)) {
        image->SetNormalSelectedImage(i);
        i=Lum::Images::Factory::factory->CreateImage();
      }

      if (!disabledImagePostfix.empty() &&
          LoadApplication(base+disabledImagePostfix+suffix,i,false)) {
        image->SetDisabledImage(i);
        i=Lum::Images::Factory::factory->CreateImage();
      }
      else {
        ImageRef t=image->GetNormalImage()->Clone();
        t->GreyOut(80);
        image->SetDisabledImage(t);
      }

      if (!disabledSelectedImagePostfix.empty() &&
          LoadApplication(base+disabledSelectedImagePostfix+suffix,i,false)) {
        image->SetDisabledSelectedImage(i);
        i=Lum::Images::Factory::factory->CreateImage();
      }
      else {
        ImageRef t=image->GetNormalImage()->Clone();
        t->GreyOut(80);
        image->SetDisabledSelectedImage(t);
      }

      if (!activatedImagePostfix.empty() &&
          LoadApplication(base+activatedImagePostfix+suffix,i,false)) {
        image->SetActivatedImage(i);
      }

      return true;
    }

    /**
     * Load an image using the theming path as full filesytem path.
     */
    bool Manager::LoadThemed(const std::wstring& filename,
                             ImageRef &image) const
    {
      assert(image.Valid());

      for (std::list<Loader*>::const_iterator iter=loader.begin();
           iter!=loader.end();
           ++iter) {
        Lum::Base::Path path;

        path.SetNativeDir(OS::display->GetThemePath());
        path.SetBaseName(filename);

        if ((*iter)->CanLoad(path.GetPath())) {
          return (*iter)->Load(path.GetPath(),image);
        }
      }

      return false;
    }
  }
}
