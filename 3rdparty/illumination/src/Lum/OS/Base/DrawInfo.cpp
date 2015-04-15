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

#include <Lum/OS/Base/DrawInfo.h>

namespace Lum {
  namespace OS {
    namespace Base {
      DrawInfo::DrawInfo(::Lum::OS::Window* window)
      : ::Lum::OS::DrawInfo(window),
        window(window),
        bitmap(NULL)
      {
        // no code
      }

      DrawInfo::DrawInfo(::Lum::OS::Bitmap* bitmap)
      : ::Lum::OS::DrawInfo(window),
        window(NULL),
        bitmap(bitmap)
      {
        // no code
      }

      ::Lum::OS::Window* DrawInfo::GetWindow() const
      {
        return window;
      }

      ::Lum::OS::Bitmap* DrawInfo::GetBitmap() const
      {
        return bitmap;
      }

      void DrawInfo::SetNewSize(size_t /*width*/, size_t /*height*/)
      {
        // no code
      }

      void DrawInfo::OnMap()
      {
        // no code
      }

      void DrawInfo::OnUnmap()
      {
        // no code
      }

      void DrawInfo::PushForeground(OS::Color color)
      {
        // no code
      }

      void DrawInfo::PushForeground(Display::ColorIndex color)
      {
        PushForeground(OS::display->GetColor(color));
      }

      void DrawInfo::DrawString(int x, int y, const std::wstring& string)
      {
        DrawString(x,y,string,0,string.length());
      }

      void DrawInfo::DrawPolygon(const Point points[], size_t count)
      {
        assert(count>1);

        for (size_t x=1; x<count; x++) {
          DrawLine(points[x-1].x,points[x-1].y,points[x].x,points[x].y);
        }
          DrawLine(points[count-1].x,points[count-1].y,points[0].x,points[0].y);
      }
    }
  }
}

