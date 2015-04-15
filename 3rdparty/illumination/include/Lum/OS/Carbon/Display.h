#ifndef LUM_OS_CARBON_DISPLAY_H
#define LUM_OS_CARBON_DISPLAY_H

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

// std
#include <list>
#include <string>

#include <Lum/Private/Config.h>

// Carbon
#include <Carbon/Carbon.h>

#include <Lum/OS/Base/Display.h>

#include <Lum/OS/Event.h>

//#include <Lum/OS/X11/Event.h>

namespace Lum {
  namespace OS {
    namespace Carbon {

      //class Tray;
      //typedef Tray *TrayPtr;

      class Window;
      typedef Window *WindowPtr;

      class Display;
      typedef Display *DisplayPtr;

      class LUMAPI Display : public ::Lum::OS::Base::Display
      {
        friend class Window;
        //friend class Tray;

      private:
        //std::list<TrayPtr>        trayList;
        std::list<WindowPtr>      winList;
        //WindowPtr                 currentWin;

        ::Lum::Base::DnDObjectPtr selectObject;
        ::Lum::Base::DnDObjectPtr querySelectObject;
        ::Lum::Base::DnDObjectPtr queryClipboardObject;
        bool                      selClearPend;

        //::Lum::OS::Display::Timer contextTimer;
        bool                      contextHelp;

        //Sleep sleepList;
        //Channel channelList;

        //WindowPtr                 dropWindow;

        //std::wstring              clipboard;

        //double                    dpi;

        bool                      exit;

      public:
        ::CGColorSpaceRef           colorSpace;
        ::GWorldPtr                 fontScratch;
        ::CGContextRef              globalContext;

      public:
        Display();

        double GetDPI() const;

        void Beep();

        //::Lum::OS::Display::FontList GetFontList();
        void AllocateColor(double red, double green, double blue,
                           ::Lum::OS::Color defaultColor,
                           ::Lum::OS::Color& color);
        void AllocateNamedColor(const char* name,
                                ::Lum::OS::Color defaultColor,
                                ::Lum::OS::Color& color);
        bool IsAllocatedColor(::Lum::OS::Color color);
        void FreeColor(::Lum::OS::Color color);
        //Sleep AddSleep(::Lum::OS::Object::MsgObject object);
        //void RemoveSleep(::Lum::Base::Display::Sleep sleep);
        //Channel AddChannel(Channel channel, unsigned long ops, ::Lum::Base::Object::MsgObject object);
        //void RemoveChannel(::Lum::OS::Display::Channel channel);

        bool Open();

        bool RegisterSelection(::Lum::Base::DnDObjectPtr object, ::Lum::OS::WindowPtr window);
        void CancelSelection();
        ::Lum::Base::DnDObjectPtr GetSelectionOwner() const;
        bool QuerySelection(::Lum::OS::WindowPtr window, ::Lum::Base::DnDObjectPtr object);

        bool SetClipboard(const std::wstring& content);
        std::wstring GetClipboard() const;
        void ClearClipboard();

        void PutBackEvent(::Lum::OS::EventPtr event,
                          ::Lum::OS::WindowPtr destWin);
        void Exit();
        void ReinitWindows();
        void Close();
        //void Receive(::Lum::Base::Object::Message message);
        void EventLoop();

        bool GetMousePos(int& x, int& y) const;

      private:
        void StopContextHelp();
        void RestartContextHelp();
        void StartContextHelp();

        void AddWindow(WindowPtr w);
        void RemoveWindow(WindowPtr w);

        /*void AddTray(TrayPtr tray);
        void RemoveTray(TrayPtr tray);

        void CheckSleeps();*/
        void GetEvent();
        bool Wait();

        WindowPtr GetWindow(::WindowPtr window) const;
      };
    }
  }
}

#endif

