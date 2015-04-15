#ifndef LUM_OS_BASE_DRAWINFO_H
#define LUM_OS_BASE_DRAWINFO_H

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

#include <Lum/OS/Bitmap.h>
#include <Lum/OS/Display.h>
#include <Lum/OS/DrawInfo.h>
#include <Lum/OS/Window.h>

namespace Lum {
  namespace OS {
    namespace Base {

      class LUMAPI DrawInfo : public OS::DrawInfo
      {
      protected:
        OS::Window* window;
        OS::Bitmap* bitmap;

      protected:
        DrawInfo(OS::Window* window);
        DrawInfo(OS::Bitmap* window);

        OS::Window* GetWindow() const;
        OS::Bitmap* GetBitmap() const;

        void SetNewSize(size_t, size_t);
        void OnMap();
        void OnUnmap();

        void PushForeground(OS::Color color);
        void PushForeground(Display::ColorIndex color);

        void DrawString(int x, int y, const std::wstring& string);
        void DrawString(int x, int y, const std::wstring& string, size_t start, size_t length) = 0;

        void DrawPolygon(const Point points[], size_t count);
      };
    }
  }
}

#endif
