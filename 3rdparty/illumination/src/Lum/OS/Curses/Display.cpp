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

#include <Lum/OS/Curses/Display.h>

#include <cassert>
#include <cstring>
#include <iostream>
#include <limits>

#include <Lum/Base/String.h>

#include <Lum/Images/Image.h>

#include <Lum/OS/EventLoopUnix.h>

#include <Lum/OS/Curses/Font.h>
#include <Lum/OS/Curses/Theme.h>
#include <Lum/OS/Curses/Tray.h>
#include <Lum/OS/Curses/Window.h>

namespace Lum {
  namespace OS {
    namespace Curses {

      class LUMAPI CursesEventSource : public EventSource
      {
      private:
        Display *display;

      public:
        CursesEventSource(Display* display)
        {
          this->display=display;
        }

        bool Matches(const Model::Action* action) const
        {
          return false;
        }

        bool GetTimeout(Lum::Base::SystemTime& time) const
        {
          return false;
        }

        bool GetFileIO(FileIO& fileIO) const
        {
          fileIO.file=0;
          fileIO.conditions=ioRead;

          return true;
        }

        bool DataAvailable() const
        {
          return false;
        }

        bool DataAvailable(Lum::Base::SystemTime& time) const
        {
          return false;
        }

        bool DataAvailable(const FileIO& fileIO) const
        {
          return true;
        }

        void Trigger()
        {
          display->GetEvent();
        }

        bool IsFinished() const
        {
          return false;
        }
      };

      double Display::GetDPI() const
      {
        return 1;
      }

      /**
        Call this method if you want the Display to stop generating
        QuickHelp calls to windows. This is necessesarry, if you are
        opening a QuickHelp and don't want to have a second after the
        second timeout.
      */
      void Display::StopContextHelp()
      {
        if (contextHelp) {
          contextHelp=false;
          /*if (d.contextTimer.active) {
            d.RemoveTimer(d.contextTimer);
          }*/
        }
      }

      /**
        Restart the generation of QuickHelp calls to windows stoped
        with Display.StopContextHelp.
      */
      void Display::RestartContextHelp()
      {
        /*if (d.contextTimer.active) {
          d.RemoveTimer(d.contextTimer);
        }
        d.AddTimer(d.contextTimer);*/
        contextHelp=true;
      }

      /**
        Restart the generation of QuickHelp calls to windows stoped
        with Display.StopContextHelp.
      */
      void Display::StartContextHelp()
      {
        if (!contextHelp) {
          //d.AddTimer(d.contextTimer);
          contextHelp=true;
        }
      }

      /**
        Adds the window to the internal list of windows.
      */
      void Display::AddWindow(Window* w)
      {
        winList.push_back(w);
        currentWin.push(w);

        update_panels();
        doupdate();
      }

      /**
        Removes the window from the internal list of windows.
      */
      void Display::RemoveWindow(Window* w)
      {
        assert(w==currentWin.top());

        currentWin.pop();
        winList.remove(w);

        update_panels();
        doupdate();

        if (currentWin.size()>0) {
          currentWin.top()->TriggerFocusIn();
        }
      }

      bool Display::AllocateColor(double /*red*/, double /*green*/, double /*blue*/,
                                  OS::Color& color)
      {
        return false;
      }

      bool Display::AllocateNamedColor(const std::string& name,
                                       OS::Color& color)
      {
        if (colorMode==colorModeMonochrome) {
          if (name=="black") {
            color=COLOR_BLACK;
          }
          else if (name=="white") {
            color=COLOR_WHITE;
          }
          else {
            return false;
          }
        }
        else {
          if (name=="black") {
            color=COLOR_BLACK;
          }
          else if (name=="red") {
            color=COLOR_RED;
          }
          else if (name=="green") {
            color=COLOR_GREEN;
          }
          else if (name=="yellow") {
            color=COLOR_YELLOW;
          }
          else if (name=="blue") {
            color=COLOR_BLUE;
          }
          else if (name=="magenta") {
            color=COLOR_MAGENTA;
          }
          else if (name=="cyan") {
            color=COLOR_CYAN;
          }
          else if (name=="white") {
            color=COLOR_WHITE;
          }
          else {
            return false;
          }
        }

        return true;
      }

      void Display::FreeColor(OS::Color /*color*/)
      {
        // no code
      }

      Display::Display()
      : selectObject(NULL),
        contextHelp(true),
        exit(true),screen(NULL)
      {
        // no code
      }

      bool Display::Open()
      {
        // Open curses
        screen=initscr();

        if (screen==NULL) {
          std::cerr << "Cannot initialize curses" << std::endl;
          return false;
        }


#ifdef NCURSES_MOUSE_VERSION
        mousemask(BUTTON1_PRESSED|
                  BUTTON1_RELEASED/*|
                  BUTTON1_CLICKED|
                  BUTTON1_DOUBLE_CLICKED|
                  BUTTON1_TRIPLE_CLICKED*/,NULL);
#endif

        // Check availibility of colors
        colorDepth=1;
        colorMode=colorModeMonochrome;

        if (has_colors() && start_color()!=ERR && COLORS>=8) {
          colorMode=colorModeColor;
          colorDepth=4;
        }

        // Basic curses initialisation
        curs_set(0);
        noecho();
        cbreak();
        nonl();
        raw();
        //Images::Factory::Set(OS::Factory::factory->CreateImageFactory(this));

        screenWidth=COLS;
        screenHeight=LINES;

        workAreaWidth=screenWidth;
        workAreaHeight=screenHeight;

        type=typeTextual;

        EvaluateDisplaySize();

        for (int c=1; c<COLOR_PAIRS; c++) {
          if (init_pair(c,c%COLORS,c/COLORS)==ERR) {
            std::cerr << c << " " << c%COLORS << " " << c/COLORS << std::endl;
            std::cerr << "Cannot initialize terminal color pairs" << std::endl;
            colorDepth=1;
            colorMode=colorModeMonochrome;
            break;
          }
        }

        propFont=driver->CreateFont();
        propFont=propFont->Load();
        propFontSize=1;

        fixedFont=driver->CreateFont();
        fixedFont=fixedFont->Load();
        fixedFontSize=1;

        theme=new Theme(this);

        for (size_t i=0; i<colorCount; i++) {
          color[i]=theme->GetColor((ColorIndex)i);
        }

        multiClickTime=200;

        eventLoop=new UnixEventLoop();
        eventLoop->AddSource(new CursesEventSource(this));

        return true;
      }

      void Display::Close()
      {
        std::list<Window*>::iterator win;

        OS::Base::Display::Close();

        if (selectObject!=NULL) {
          CancelSelection();
        }
        ClearClipboard();
        //d.selector.Close;

        win=winList.begin();
        while (win!=winList.end()) {
          std::cerr << "Warning: window '" << Lum::Base::WStringToString((*win)->GetTitle()) << "' not explicitely closed" << std::endl;
          (*win)->Close(); // removes itself from list

          win=winList.begin();
        }

        delwin(screen);
        endwin();

        delete eventLoop;
      }

      void Display::Beep()
      {
        //XBell(display,100);
      }


      bool Display::RegisterSelection(Lum::Base::DataExchangeObject* object,
                                      OS::Window* /*window*/)
      {
        if (selectObject!=object) {
          if (selectObject!=NULL) {
            selectObject->Deselect();
            selectObject=NULL;
          }

          /*XSetSelectionOwner(display,
                             XA_PRIMARY,
                             dynamic_cast<Window*>(window)->window,
                             CurrentTime);*/
          selectObject=object;
        }

        return true;
      }

      void Display::CancelSelection()
      {
        assert(selectObject!=NULL);
        /*selClearPend=true;
        XSetSelectionOwner(display,XA_PRIMARY,None,CurrentTime);*/
        selectObject->Deselect();
        selectObject=NULL;
      }

      Lum::Base::DataExchangeObject* Display::GetSelectionOwner() const
      {
        return selectObject;
      }

      bool Display::QuerySelection(OS::Window* /*window*/,
                                   Lum::Base::DataExchangeObject* /*object*/)
      {
        /*
        XConvertSelection(display,
                          XA_PRIMARY,
                          XA_STRING,
                          atoms[selectionAtom],
                          dynamic_cast<Window*>(window)->window,
                          CurrentTime);

        querySelectObject=object;*/

        return false;
      }

      bool Display::SetClipboard(const std::wstring& content)
      {
        if (!content.empty()) {
          clipboard=content;
          return true;
        }
        else {
          return false;
        }
      }

      std::wstring Display::GetClipboard() const
      {
        if (!clipboard.empty()) {
          return clipboard;
        }

        return L"";
      }

      void Display::ClearClipboard()
      {
        if (!clipboard.empty()) {
          clipboard=L"";
        }
      }

      bool Display::HasClipboard() const
      {
        return !clipboard.empty();
      }

      void Display::ReinitWindows()
      {
        std::list<Window*>::iterator win;

        win=winList.begin();
        while (win!=winList.end()) {
          //(*win)->ReinitWindow();

          ++win;
        }
      }

      void Display::GetEvent()
      {
        bool              meta;
        int               e;
        Lum::OS::EventRef event;

        assert(currentWin.size()>0);

        meta=false;

        if ((e=wgetch(currentWin.top()->window))!=ERR) {
          if (e!=KEY_MOUSE && e!=KEY_RESIZE) {
            // Escape signals a potential meta sequence starter
            if (e==27) {
              int e2;

              if ((e2=wgetch(currentWin.top()->window))!=ERR) {
                meta=true;
                e=e2;
              }
            }
            // 8th bit set? xterm does not have alt maped as meta but
            // sends the alt qualified key codes directly. We recalculate
            // them to their qualified 7th bit value
            if (e>=128 && e<256) {
              e=e-128;
              meta=true;
            }
          }


          event=Curses::GetEvent(currentWin.top(),e,meta);

          if (event.Valid()) {
            do {
              event->reUse=false;
              currentWin.top()->HandleEvent(event);
            }
            while (event->reUse);

            // Generate key up event, since curses does not
            // offer any information about key state (All key events
            // are implicitely key down events).
            if (dynamic_cast<OS::KeyEvent*>(event.Get())!=NULL) {
              dynamic_cast<OS::KeyEvent*>(event.Get())->type=OS::KeyEvent::up;

              do {
                event->reUse=false;
                currentWin.top()->HandleEvent(event);
              }
              while (event->reUse);
            }
          }
        }

        update_panels();
        doupdate();
      }

      bool Display::GetMousePos(int& /*x*/, int& /*y*/) const
      {
        return false;
      }
    }
  }
}
