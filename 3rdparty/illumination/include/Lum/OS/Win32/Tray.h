#ifndef LUM_OS_WIN32_TRAY_H
#define LUM_OS_WIN32_TRAY_H

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

#include <Lum/OS/Tray.h>
#include <Lum/OS/Event.h>

#include <Lum/OS/Win32/OSAPI.h>

namespace Lum {
  namespace OS {
    namespace Win32 {

      class Tray : public OS::Tray
      {
      private:
        TrayStatus       status;
        Model::ActionRef eventAction;
        Model::ActionRef openedAction;
        size_t           relWidth;
        size_t           relHeight;
        size_t           width;
        size_t           height;
        OS::Bitmap*      bitmap;
        OS::Window*      menu;         //< Pointer to menu
        NOTIFYICONDATA   nid;
        std::wstring     title;

      public:
        Tray();
        ~Tray();

        TrayStatus GetStatus() const;

        ::Lum::OS::Bitmap* GetBitmap() const;

        void SetRelativeSize(size_t width, size_t height);

        void SetMenu(OS::Window* window);

        bool Show();
        bool Hide();

        void Redraw();

        void SetTitle(const std::wstring& title);

        Model::Action* GetEventAction() const;
        Model::Action* GetOpenedAction() const;

        void HandleEvent(OS::Event* event);
      };
    }
  }
}

#endif
