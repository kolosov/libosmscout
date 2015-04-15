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

#include <Lum/OS/Win32/Event.h>

#include <Lum/OS/Win32/Window.h>

namespace Lum {
  namespace OS {
    namespace Win32 {

      static EventRef      simpleEvent;
      static MouseEventRef mouseEvent;
      static KeyEventRef   keyEvent;

      Event::Event(const MSG& msg)
      : event(msg)
      {
        // no code
      }

      
      unsigned long EvaluateKeyQualifier(BYTE *state)
      {
        unsigned long qualifier=0;

        if (state[VK_LSHIFT] & 0x80) {
          qualifier|=qualifierShiftLeft;
        }

        if (state[VK_RSHIFT] & 0x80) {
          qualifier|=qualifierShiftRight;
        }

        if (state[VK_LCONTROL] & 0x80) {
          qualifier|=qualifierControlLeft;
        }

        if (state[VK_RCONTROL] & 0x80) {
          qualifier|=qualifierControlRight;
        }

        if (state[VK_LMENU] & 0x80) {
          qualifier|=qualifierAltLeft;
        }

        if (state[VK_RMENU] & 0x80) {
          qualifier|=qualifierAltRight;
        }

        if (state[VK_LBUTTON] & 0x80) {
          qualifier|=qualifierButton1;
        }

        if (state[VK_MBUTTON] & 0x80) {
          qualifier|=qualifierButton2;
        }

        if (state[VK_RBUTTON] & 0x80) {
          qualifier|=qualifierButton3;
        }

        if (qualifier & (qualifierShiftLeft|qualifierShiftRight)) {
          qualifier|=qualifierShift;
        }
        if (qualifier & (qualifierControlLeft|qualifierControlRight)) {
          qualifier|=qualifierControl;
        }
        if (qualifier & (qualifierAltLeft|qualifierAltRight)) {
          qualifier|=qualifierAlt;
        }

        return qualifier;
      }

      unsigned long EvaluateMouseQualifier(WPARAM state)
      {
        unsigned long qualifier=0;

        if (state & MK_SHIFT) {
          qualifier=(qualifier|qualifierShiftLeft|qualifierShiftRight);
        }

        if (state & MK_CONTROL) {
          qualifier=(qualifier|qualifierControlLeft|qualifierControlRight);
        }

        if (state & MK_LBUTTON) {
          qualifier=(qualifier|qualifierButton1);
        }

        if (state & MK_MBUTTON) {
          qualifier=(qualifier|qualifierButton2);
        }

        if (state & MK_RBUTTON) {
          qualifier=(qualifier|qualifierButton3);
        }

        return qualifier;
      }

      Key TranslateKey(int key, unsigned long qualifier)
      {
        switch (key) {
        /* Editing */
        case VK_BACK:
          return keyBackspace;
        case VK_DELETE:
          return keyDelete;

        /* Advanced editing */
        case VK_RETURN:
          return keyReturn;

        /* Movement */
        case VK_HOME:
          return keyHome;
        case VK_END:
          return keyEnd;
        case VK_LEFT:
          return keyLeft;
        case VK_RIGHT:
           return keyRight;
        case VK_UP:
           return keyUp;
        case VK_DOWN:
          return keyDown;
        case VK_PRIOR:
          return keyPrior;
        case VK_NEXT:
          return keyNext;

        /* Special keys */
        case VK_TAB:
          return ((qualifier & (qualifierShiftLeft|qualifierShiftRight)) ? keyLeftTab : keyTab);
        case VK_SPACE:
          return keySpace;
        case VK_ESCAPE:
          return keyEscape;
        case VK_PRINT:
          return keyPrint;
        case VK_INSERT:
          return keyInsert;

        /* Function keys */
        case VK_F1:
          return keyF1;
        case VK_F2:
          return keyF2;
        case VK_F3:
          return keyF3;
        case VK_F4:
          return keyF4;
        case VK_F5:
          return keyF5;
        case VK_F6:
          return keyF6;
        case VK_F7:
          return keyF7;
        case VK_F8:
          return keyF8;
        case VK_F9:
          return keyF9;
        case VK_F10:
          return keyF10;
        case VK_F11:
          return keyF11;
        case VK_F12:
          return keyF12;
        default:
          return keyUnknown;
        }
      }

      void KeyEvent::GetName(std::wstring& buffer) const
      {
        if (!GetKeyName(key,buffer)) {
          wchar_t tmp[1024];

          if (::GetKeyNameTextW(event.lParam,tmp,sizeof(tmp)/sizeof(wchar_t))!=0) {
            /* convert single letters to lower case */
            if (tmp[0]>=L'A' && tmp[0]<= L'Z' && tmp[1]==L'\0') {
              tmp[0]=tmp[0]+L'a'-L'A';
            }
            buffer=tmp;
          }
          else {
            buffer.erase();
          }
        }
      }

      KeyEvent::KeyEvent(const MSG& msg)
      {
        int     res;
        BYTE    keys[256];
        wchar_t buffer[256];

        event=msg;
        
        if (msg.message==WM_KEYDOWN || msg.message==WM_SYSKEYDOWN) {
          type=down;
        }
        else {
          type=up;
        }

        /* ignore */ GetKeyboardState(keys);
        res=::ToUnicode(msg.wParam,(msg.lParam/65536) % 2048,keys,buffer,sizeof(buffer)/sizeof(wchar_t),0);
        if (res>0) {
          text=std::wstring(buffer,res);
        }
        else {
          text=L"";
        }

        qualifier=EvaluateKeyQualifier(keys);
        key=TranslateKey(msg.wParam,qualifier);

        if (text.empty() && (qualifier & (qualifierControlLeft|qualifierControlRight))) {
          if (msg.wParam>=65 && msg.wParam<=90) {
            if (qualifier & (qualifierShiftLeft|qualifierShiftRight)) {
              text.assign(1,(wchar_t)msg.wParam);
            }
            else {
              text.assign(1,(wchar_t)msg.wParam+L'a'-L'A');
            }
          }
          else if (msg.wParam!=VK_CONTROL) {
            text.assign(1,(wchar_t)msg.wParam);
          }
        }
      }

      MouseEvent::MouseEvent(Window* window, const MSG& msg)
      {
        event=msg;
      
        x=GET_X_LPARAM(msg.lParam);
        y=GET_Y_LPARAM(msg.lParam);
        qualifier=EvaluateMouseQualifier(msg.wParam);

        switch (msg.message) {
        case WM_LBUTTONDOWN:
          type=down;
          button=button1;
          qualifier=(qualifier & (~qualifierButton1));
          break;
        case WM_LBUTTONUP:
          type=up;
          button=button1;
          qualifier=(qualifier | qualifierButton1);
          break;
        case WM_MBUTTONDOWN:
          type=down;
          button=button2;
          qualifier=(qualifier & (~qualifierButton2));
          break;
        case WM_MBUTTONUP:
          type=up;
          button=button2;
          qualifier=(qualifier | qualifierButton2);
          break;
        case WM_RBUTTONDOWN:
          type=down;
          button=button3;
          qualifier=(qualifier & (~qualifierButton3));
          break;
        case WM_RBUTTONUP:
          type=up;
          button=button3;
          qualifier=(qualifier | qualifierButton3);
          break;
        case WM_MOUSEMOVE:
          type=move;
          button=none;
          break;
        case WM_MOUSEWHEEL:
          /* For this event, mouse coords are relative to screen. Must tranform it to window coords */
          x=x-window->GetX();
          y=y-window->GetY();
          type=down;
          if (GET_WHEEL_DELTA_WPARAM(msg.wParam)>0) {
            button=button4;
          }
          else {
            button=button5;
          }
          break;
        default:
          type=down;
          button=none;
          break;
        }
      }

      ::Lum::OS::Event* GetEvent(Window* window, const MSG& msg)
      {
        switch (msg.message) {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MOUSEMOVE:
        case WM_MOUSEWHEEL:
          mouseEvent=new MouseEvent(window,msg);
          return mouseEvent;

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
          keyEvent=new KeyEvent(msg);
          return keyEvent;
        default:
          simpleEvent=new Event(msg);
          return simpleEvent;
        }
      }
    }
  }
}
