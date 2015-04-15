/*
  This source is part of the Illumination library
  Copyright (C) 2006  Tim Teulings

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

#include <Lum/OS/Probe.h>

#include <clocale>
#include <cstdlib>
#include <iostream>

#include <Lum/Private/Config.h>

#include <Lum/Base/L10N.h>
#include <Lum/Base/Singleton.h>
#include <Lum/Base/String.h>

#if defined(LUM_HAVE_LIB_CAIRO)
#include <Lum/OS/Cairo/Driver.h>
#endif

#if defined(LUM_HAVE_LIB_CARBON)
#include <Lum/OS/Carbon/Driver.h>
#endif

#if defined(LUM_HAVE_LIB_CURSES)
#include <Lum/OS/Curses/Driver.h>
#endif

#if defined(LUM_HAVE_LIB_WIN32)
#include <Lum/OS/Win32/Driver.h>
#include <Lum/OS/Win32/OSAPI.h>
#endif

namespace Lum {
  namespace OS {

    ProberImpl*                      singletonStore;
    Lum::Base::Singleton<ProberImpl> prober(&singletonStore);

    Prober::~Prober()
    {
      // no code
    }

    ProberImpl::ProberImpl()
    {

// Primary

#if defined(LUM_HAVE_LIB_CAIRO) && defined(CAIRO_HAS_WIN32_SURFACE)
      AddDriver(&Cairo::cairoWin32Driver);
#endif

#if defined(LUM_HAVE_LIB_CAIRO) && defined(CAIRO_HAS_XLIB_SURFACE)
      AddDriver(&Cairo::cairoX11Driver);
#endif

// Secondary

#if defined(LUM_HAVE_LIB_CARBON)
      AddDriver(&Carbon::carbonDriver);
#endif

#if defined(LUM_HAVE_LIB_WIN32)
      AddDriver(&Win32::win32Driver);
#endif

#if defined(LUM_HAVE_LIB_CURSES)
      AddDriver(&Curses::cursesDriver);
#endif

      char* tmp;

      tmp=setlocale(LC_ALL,"");

      if (tmp!=NULL) {
        Lum::Base::L10N::SetLocale(Lum::Base::StringToWString(tmp));
      }
    }

    void ProberImpl::AddDriver(Driver* driver)
    {
      assert(driver!=NULL);

      list.push_back(driver);
    }

    void ProberImpl::SetDriver(Driver* driver)
    {
      assert(driver!=NULL);

      delete display;

      Lum::OS::driver=driver;

      display=driver->CreateDisplay();
    }

    bool ProberImpl::Open(const std::wstring& appName,
                          const std::wstring& programName,
                          const std::vector<std::wstring>& args)
    {
      char *tmp;

      std::list<Driver*>::const_iterator iter;

      tmp=getenv("ILLUMINATION_DRIVER");

      //
      // Try to use forced driver first
      //

      if (tmp!=NULL && tmp[0]!='\0') {
        std::wstring driverName=Lum::Base::StringToWString(tmp);

        iter=list.begin();
        while (iter!=list.end()) {
          if ((*iter)->GetDriverName()==driverName) {
            SetDriver(*iter);

            if (display==NULL) {
              std::cerr << "Cannot initialize forced driver: '" << tmp << "'!" << std::endl;
              break;
            }

            display->SetAppName(appName);
            display->SetProgramName(programName);

            if (!display->Open()) {
              std::cerr << "Cannot open display for forced driver: '" << tmp << "'!" << std::endl;
              delete display;
              break;
            }

            return true;
          }
          ++iter;
        }

        if (iter==list.end()) {
          std::cerr << "Forced driver: '" << tmp << "' not found!" << std::endl;
        }
      }

      //
      // No driver forced or initialization of forced driver did not work out no simply scan
      // all available drivers...
      //

      iter=list.begin();
      while (iter!=list.end()) {
        SetDriver(*iter);

        display->SetAppName(appName);
        display->SetProgramName(programName);

        if (display->Open()) {
          return true;
        }
        ++iter;
      }

      std::cerr << "Cannot initialize any display driver!" << std::endl;

      return false;
    }

    /**
     * Opens a display by probing all installed driver in order of priority.
     *
     * Also set the C and C++ locale to the local set in the enviroment.
     */
    bool ProberImpl::Open(const std::wstring& appName, int &argc, char** &argv)
    {
      std::vector<std::wstring> args;

      for (int i=1; i<argc; i++) {
        args.push_back(Lum::Base::StringToWString(argv[i]));
      }

      return Open(appName,Lum::Base::StringToWString(argv[0]),args);
    }

    void ProberImpl::Close()
    {
      if (display!=NULL) {
        display->Close();
      }

      delete display;

      display=NULL;
    }

    extern "C" {
      Prober* GetProber()
      {
        return prober.Instance();
      }
    }
  }
}
