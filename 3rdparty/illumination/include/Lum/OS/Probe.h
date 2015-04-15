#ifndef LUM_OS_PROBE_H
#define LUM_OS_PROBE_H

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

#include <Lum/Base/Singleton.h>

#include <Lum/OS/Driver.h>

namespace Lum {
  namespace OS {

    class LUMAPI Prober
    {
    public:
      virtual ~Prober();
      virtual bool Open(const std::wstring& appName, int &argc, char** &argv) = 0;
      virtual void Close() = 0;
    };

#if defined(LUM_INSTANTIATE_TEMPLATES)
    LUM_EXPTEMPL template class std::list< ::Lum::OS::Factory*>;
#endif

    class LUMAPI ProberImpl : public Prober
    {
    private:
      std::list<OS::Driver*> list;

    private:
      void AddDriver(OS::Driver* driver);
      void SetDriver(OS::Driver* driver);

    public:
      ProberImpl();
      bool Open(const std::wstring& appName,
                const std::wstring& progName,
                const std::vector<std::wstring>& args);

      bool Open(const std::wstring& appName, int &argc, char** &argv);

      void Close();
    };

#if defined(LUM_INSTANTIATE_TEMPLATES)
    LUM_EXPTEMPL template class LUMAPI ::Lum::Base::Singleton<ProberImpl>;
#endif

    extern LUMAPI ::Lum::Base::Singleton<ProberImpl> prober;

    extern "C" {
      Prober* GetProber();
    }
  }
}

#endif
