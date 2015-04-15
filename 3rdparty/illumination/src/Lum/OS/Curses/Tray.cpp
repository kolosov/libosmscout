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

#include <Lum/OS/Curses/Tray.h>

namespace Lum {
  namespace OS {
    namespace Curses {

      Tray::Tray()
      : status(statusHidden)
      {
        // no code
      }

      Tray::~Tray()
      {
        if (status==statusOpening || status==statusShown) {
          Hide();
        }
      }

      Tray::TrayStatus Tray::GetStatus() const
      {
        return status;
      }

      ::Lum::OS::Bitmap* Tray::GetBitmap() const
      {
        return NULL;
      }

      void Tray::SetRelativeSize(size_t width, size_t height)
      {
        assert(width>0 && height>0);

        // no code
      }

      void Tray::SetMenu(::Lum::OS::Window* /*window*/)
      {
        // no code
      }

      bool Tray::Show()
      {
        assert(status==statusHidden);

        return false;
      }

      bool Tray::Hide()
      {
        assert(status==statusOpening || status==statusShown);

        status=statusHidden;

        return true;
      }


      void Tray::Redraw()
      {
        assert(status==statusShown);

        // no code
      }

      void Tray::SetTitle(const std::wstring& title)
      {
        // no code
      }

      ::Lum::Model::Action* Tray::GetEventAction() const
      {
        return NULL;
      }

      ::Lum::Model::Action* Tray::GetOpenedAction() const
      {
        return NULL;
      }

      void Tray::HandleEvent(::Lum::OS::Event* /*event*/)
      {
        // no code
      }
    }
  }
}

