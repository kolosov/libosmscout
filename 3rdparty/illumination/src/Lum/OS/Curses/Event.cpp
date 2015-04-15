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

#include <Lum/OS/Curses/Event.h>

#include <ctype.h>

#include <Lum/Base/String.h>

namespace Lum {
  namespace OS {
    namespace Curses {

      static EventRef      simpleEvent;
      static MouseEventRef mouseEvent;
      static KeyEventRef   keyEvent;

      /**
       This a Display class only access-method for initializing the KeyEvent
       from OS-specific information.

       @note
       Do not call this method in your code.
      */
      bool KeyEvent::SetCursesEvent(Window* window, int event, bool meta)
      {
        char        str[3];
        std::string name;

        this->event=event;

        str[0]=(char)event;
        str[1]='\0';

        text=Lum::Base::StringToWString(str);

        name=keyname(event);

        type=down;
        qualifier=0;

        if (meta) {
          qualifier|=qualifierAltLeft|qualifierAltRight|qualifierAlt;
        }

        if (name=="^M") {
          key=keyReturn;
          text=L"";
        }
        else if (name=="KEY_PPAGE") {
          key=keyPrior;
          text=L"";
        }
        else if (name=="KEY_NPAGE") {
          key=keyNext;
          text=L"";
        }
        else if (name=="KEY_HOME") {
          key=keyHome;
          text=L"";
        }
        else if (name=="KEY_END") {
          key=keyEnd;
          text=L"";
        }
        else if (name=="KEY_DC") {
          key=keyDelete;
          text=L"";
        }
        else if (name=="KEY_IC") {
          key=keyInsert;
          text=L"";
        }
        else if (name=="KEY_UP") {
          key=keyUp;
          text=L"";
        }
        else if (name=="KEY_DOWN") {
          key=keyDown;
          text=L"";
        }
        else if (name=="KEY_LEFT") {
          key=keyLeft;
          text=L"";
        }
        else if (name=="KEY_RIGHT") {
          key=keyRight;
          text=L"";
        }
        else if (name=="KEY_BACKSPACE") {
          key=keyBackspace;
          text[0]=8; // Backspace
        }
        else if (name=="KEY_BTAB") {
          key=keyLeftTab;
          text=L"";
        }
        else if (name=="KEY_F(1)") {
          key=keyF1;
          text=L"";
        }
        else if (name=="KEY_F(2)") {
          key=keyF2;
          text=L"";
        }
        else if (name=="KEY_F(3)") {
          key=keyF3;
          text=L"";
        }
        else if (name=="KEY_F(4)") {
          key=keyF4;
          text=L"";
        }
        else if (name=="KEY_F(5)") {
          key=keyF5;
          text=L"";
        }
        else if (name=="KEY_F(6)") {
          key=keyF6;
          text=L"";
        }
        else if (name=="KEY_F(7)") {
          key=keyF7;
          text=L"";
        }
        else if (name=="KEY_F(8)") {
          key=keyF8;
          text=L"";
        }
        else if (name=="KEY_F(9)") {
          key=keyF9;
          text=L"";
        }
        else if (name=="KEY_F(10)") {
          key=keyF10;
          text=L"";
        }
        else if (name=="KEY_F(11)") {
          key=keyF11;
          text=L"";
        }
        else if (name=="KEY_F(12)") {
          key=keyF12;
          text=L"";
        }
        else if (name=="^I") {
          key=keyTab;
        }
        else if (name=="^[") {
          key=keyEscape;
        }
        else if (name==" ") {
          key=keySpace;
        }
        else if (name[0]=='M' && name[1]=='-') {
          qualifier|=qualifierAltLeft|qualifierAltRight|qualifierAlt;
        }
        else if (name[0]=='^') {
          key=keyUnknown;

          str[0]=name[0];
          str[1]=tolower(name[1]);
          str[2]='\0';
          text=Lum::Base::StringToWString(str);
          text.erase(0,1);
          qualifier|=qualifierControlLeft|qualifierControlRight|qualifierControl;
        }
        else {
          key=keyUnknown;

          if (isalpha(str[0]) && isupper(str[0])) {
            qualifier|=qualifierShiftLeft|qualifierShiftRight|qualifierShift;
          }
        }

        //std::cerr << "Meta: " << meta << " key: " << key << " '" << str << "' '" << name << "' '"<< ::Lum::Base::WStringToString(text) << "'" << std::endl;

        return true;
      }

      /**
        Returns a string representing the name of key(s) pressed.
      */
      void KeyEvent::GetName(std::wstring& buffer) const
      {
        if (!GetKeyName(key,buffer)) {
          buffer=text;
        }
      }

      /**
        NOTE
        THis as a Display-only access-method for initializing the MouseEvent.
        Do not call this method in your code, it is Display only and does only
        exists on curses implementations of Illumination.
      */
      bool MouseEvent::SetCursesEvent(Window* window, int event)
      {
        MEVENT tmp;

        this->event=event;

        getmouse(&tmp);

        x=tmp.x-window->GetX();
        y=tmp.y-window->GetY();

        qualifier=0;

        switch (tmp.bstate) {
        case BUTTON1_PRESSED:
        case BUTTON1_RELEASED:
          button=button1;
          break;
        case BUTTON2_PRESSED:
        case BUTTON2_RELEASED:
          button=button2;
          break;
        case BUTTON3_PRESSED:
        case BUTTON3_RELEASED:
          button=button3;
          break;
        case BUTTON4_PRESSED:
        case BUTTON4_RELEASED:
          button=button4;
          break;
        default:
          button=none;
          break;
        }

        switch (tmp.bstate) {
        case BUTTON1_PRESSED:
        case BUTTON2_PRESSED:
        case BUTTON3_PRESSED:
        case BUTTON4_PRESSED:
          type=down;
          break;
        case BUTTON1_RELEASED:
          qualifier|=qualifierButton1;
          type=up;
          break;
        case BUTTON2_RELEASED:
          qualifier|=qualifierButton2;
          type=up;
          break;
        case BUTTON3_RELEASED:
          qualifier|=qualifierButton3;
          type=up;
          break;
        case BUTTON4_RELEASED:
          qualifier|=qualifierButton4;
          type=up;
          break;
          break;
        default:
          button=none;
          type=down;
          break;
        }

        return true;
      }

      /**
        Convert the given curses event to a illumination event.
      */
      OS::Event* GetEvent(Window* window, int event, bool meta)
      {
        if (event==KEY_MOUSE) {
          mouseEvent=new MouseEvent();

          if (dynamic_cast<MouseEvent*>(mouseEvent.Get())->SetCursesEvent(window,event)) {
            return mouseEvent;
          }
          else {
            return NULL;
          }
        }
        else if (event!=KEY_MOUSE && event!=KEY_RESIZE) {
          keyEvent=new KeyEvent();

          if (dynamic_cast<KeyEvent*>(keyEvent.Get())->SetCursesEvent(window,event,meta)) {
            return keyEvent;
          }
          else {
            return NULL;
          }
        }

        simpleEvent=new Event();

        dynamic_cast<Event*>(simpleEvent.Get())->event=event;
        return simpleEvent;
      }
    }
  }
}
