#ifndef LUM_OS_CARBON_FACTORIES_H
#define LUM_OS_CARBON_FACTORIES_H

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

#include <Lum/Base/Singleton.h>

#include <Lum/OS/Factories.h>

namespace Lum {
  namespace OS {
    namespace Carbon {

      class CarbonFactory : public Factory
      {
      public:
        CarbonFactory();

        ::Lum::OS::DisplayPtr CreateDisplay() const;
        ::Lum::OS::WindowPtr CreateWindow() const;
        ::Lum::OS::BitmapPtr CreateBitmap(size_t width, size_t height) const;
        ::Lum::OS::DrawInfoPtr CreateDrawInfo(::Lum::OS::WindowPtr window);
        ::Lum::OS::DrawInfoPtr CreateDrawInfo(::Lum::OS::BitmapPtr bitmap);
        ::Lum::OS::FontPtr CreateFont() const;
        ::Lum::OS::TrayPtr CreateTray() const;

        ::Lum::Images::Factory* CreateImageFactory(::Lum::OS::DisplayPtr display) const;
      };
    }
  }
}

#endif
