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

#include <Lum/OS/Win32/Bitmap.h>

#include <iostream>

#include <Lum/OS/Win32/Display.h>

namespace Lum {
  namespace OS {
    namespace Win32 {

      Bitmap::Bitmap(size_t width, size_t height)
      : OS::Base::Bitmap(width,height)
      {
        dc=::CreateCompatibleDC(dynamic_cast<Display*>(OS::display)->hdc);
        if (dc==0) {
          std::cerr << "Bitmap: CreateCompatibleDC" << std::endl;
          return;
        }

        ::SetGraphicsMode(dc,GM_ADVANCED);
        bitmap=::CreateCompatibleBitmap(dynamic_cast<Display*>(OS::display)->hdc,width,height);
        if (bitmap!=0) {
          /* ignore */ SelectObject(dc,bitmap);

          draw=OS::driver->CreateDrawInfo(this);
          draw->PushClip(0,0,width,height);
        }
      }

      Bitmap::~Bitmap()
      {
        if (draw!=NULL) {
          draw->PopClip();
          delete draw;
        }

        ::DeleteDC(dc);
        ::DeleteObject(bitmap);

        bitmap=0;
        dc=0;
      }
    }
  }
}
