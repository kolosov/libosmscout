#ifndef LUM_OS_WIN32_DISPLAY_H
#define LUM_OS_WIN32_DISPLAY_H

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

#include <Lum/OS/Base/Display.h>
#include <Lum/OS/Base/DrawInfo.h>
#include <Lum/OS/Base/Font.h>

#include <Lum/OS/Win32/OSAPI.h>

namespace Lum {
  namespace OS {
    namespace Win32 {

      class Tray;
      class Window;
      class Display;

      extern WNDCLASS wClass;
      extern ATOM     wClassAtom;

#if defined(LUM_INSTANTIATE_TEMPLATES)
    LUM_EXPTEMPL template class std::list<Tray*>;
    LUM_EXPTEMPL template class std::list<Window*>;
#endif

      class LUMAPI Display : public OS::Base::Display
      {
      private:
        friend class Window;
        friend class Tray;

      private:
        std::list<Tray*>     trayList;
        std::list<Window*>   winList;
        Lum::Base::DataExchangeObject *selectObject;

      public:
        HINSTANCE            instance;
        HDC                  hdc;
        HWND                 appWindow;

      public:
        Display();

        double GetDPI() const;

        void Beep();

        bool AllocateColor(double red, double green, double blue,
                           OS::Color& color);
        bool AllocateNamedColor(const std::string& name, Color& color);
        void FreeColor(OS::Color color);

        FontRef GetFontInternal(FontType type, size_t size) const;

        bool RegisterSelection(Lum::Base::DataExchangeObject* object, OS::Window* window);
        void CancelSelection();
        Lum::Base::DataExchangeObject* GetSelectionOwner() const;
        bool QuerySelection(OS::Window* window, Lum::Base::DataExchangeObject* object);

        bool SetClipboard(const std::wstring& content);
        std::wstring GetClipboard() const;
        void ClearClipboard();
        bool HasClipboard() const;

        bool GetMousePos(int& x, int& y) const;

        void ReinitWindows();

        bool Open();
        void Close();

        void AddWindow(Window* w);
        void RemoveWindow(Window* w);
        Window* GetWindow(::HWND window);

        void AddTray(Tray* tray);
        void RemoveTray(Tray* tray);
        Tray* GetTray(UINT id);
      };
    }
  }
}

#endif
