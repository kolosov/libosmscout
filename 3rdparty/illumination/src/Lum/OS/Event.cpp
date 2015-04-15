/*
  This source is part of the Illumination library
  Copyright (C) 2004  Tim Teulings

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

#include <Lum/OS/Event.h>

namespace Lum {
  namespace OS {

    bool GetKeyName(Key key, std::wstring& buffer)
    {
      switch (key) {
      case keyBackspace:
        buffer=L"BackSpace";
        break;
      case keyDelete:
        buffer=L"Delete";
        break;
      case keyTab:
        buffer=L"Tab";
        break;
      case keyLeftTab:
        buffer=L"LeftTab";
        break;
      case keyReturn:
        buffer=L"Return";
        break;
      case keyEscape:
        buffer=L"Escape";
        break;
      case keySpace:
        buffer=L"Space";
        break;
      case keyHome:
        buffer=L"Home";
        break;
      case keyBegin:
        buffer=L"Begin";
        break;
      case keyEnd:
        buffer=L"End";
        break;
      case keyLeft:
        buffer=L"Left";
        break;
      case keyRight:
        buffer=L"Right";
        break;
      case keyUp:
        buffer=L"Up";
        break;
      case keyDown:
        buffer=L"Down";
        break;
      case keyPrior:
        buffer=L"Prior";
        break;
      case keyNext:
        buffer=L"Next";
        break;
      case keyPrint:
        buffer=L"Print";
        break;
      case keyInsert:
        buffer=L"Insert";
        break;
      case keyF1:
        buffer=L"F1";
        break;
      case keyF2:
        buffer=L"F2";
        break;
      case keyF3:
        buffer=L"F3";
        break;
      case keyF4:
        buffer=L"F4";
        break;
      case keyF5:
        buffer=L"F5";
        break;
      case keyF6:
        buffer=L"F6";
        break;
      case keyF7:
        buffer=L"F7";
        break;
      case keyF8:
        buffer=L"F8";
        break;
      case keyF9:
        buffer=L"F9";
        break;
      case keyF10:
        buffer=L"F10";
        break;
      case keyF11:
        buffer=L"F11";
        break;
      case keyF12:
        buffer=L"F12";
        break;
      default:
        buffer[0]='\0';
        return false;
      }

      return true;
    }
    
    Event::Event()
    {
      // no code
    }
    
    Event::~Event()
    {
      // no code
    }
    
    KeyEvent::KeyEvent()
    {
      // no code
    }
    
    MouseEvent::MouseEvent()
    {
      // no code
    }
  }
}

