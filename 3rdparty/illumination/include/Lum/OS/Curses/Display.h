#ifndef LUM_OS_CURSES_DISPLAY_H
#define LUM_OS_CURSES_DISPLAY_H

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

// std
#include <list>
#include <stack>
#include <string>

// Curses
#include <curses.h>

#include <Lum/OS/Base/Display.h>

#include <Lum/OS/Event.h>

#include <Lum/OS/Curses/Event.h>

namespace Lum {
  namespace OS {
    namespace Curses {

      class Window;
      class Display;

      class LUMAPI Display : public OS::Base::Display
      {
        friend class Window;

      private:
        std::list<Window*>        winList;
        std::stack<Window*>       currentWin;

        Lum::Base::DataExchangeObject *selectObject;
        /*
        Lum::Base::DnDObject      *querySelectObject;
        Lum::Base::DnDObject      *queryClipboardObject;
        bool                      selClearPend;
        */

        //Lum::OS::Display::Timer contextTimer;
        bool                      contextHelp;

        std::wstring              clipboard;

        double                    dpi;

        bool                      exit;

      public:
        ::WINDOW                  *screen;

      public:
        Display();

        double GetDPI() const;

        void Beep();

        bool AllocateColor(double red, double green, double blue,
                           OS::Color& color);
        bool AllocateNamedColor(const std::string& name,
                                OS::Color& color);
        void FreeColor(OS::Color color);

        bool Open();

        bool RegisterSelection(Lum::Base::DataExchangeObject* object, OS::Window* window);
        void CancelSelection();
        Lum::Base::DataExchangeObject* GetSelectionOwner() const;
        bool QuerySelection(OS::Window* window, Lum::Base::DataExchangeObject* object);

        bool SetClipboard(const std::wstring& content);
        std::wstring GetClipboard() const;
        void ClearClipboard();
        bool HasClipboard() const;

        void ReinitWindows();
        void Close();

        bool GetMousePos(int& x, int& y) const;

        void GetEvent();
      private:
        void StopContextHelp();
        void RestartContextHelp();
        void StartContextHelp();

        void AddWindow(Window* w);
        void RemoveWindow(Window* w);

        void CheckSleeps();
      };
    }
  }
}

#endif
