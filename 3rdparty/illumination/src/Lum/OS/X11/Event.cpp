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

#include <Lum/OS/X11/Event.h>

#include <X11/keysym.h>

#include <Lum/Base/String.h>

namespace Lum {
  namespace OS {
    namespace X11 {

      static EventRef      simpleEvent;
      static MouseEventRef mouseEvent;
      static KeyEventRef   keyEvent;

      static XComposeStatus compose;

      /**
        Convert the given X11 qualifier description to a VisualOberon
        qualifier description.
      */

      static unsigned long EvaluateQualifier(unsigned int state)
      {
        unsigned long qualifier=0;

        if (state & ShiftMask) {
          qualifier|=qualifierShiftLeft|qualifierShiftRight|qualifierShift;
        }
        if (state & LockMask) {
          qualifier|=qualifierCapsLock;
        }
        if (state & ControlMask) {
          qualifier|=qualifierControlLeft|qualifierControlRight|qualifierControl;
        }
#if defined(__APPLE__)
        if ((state & Mod2Mask) || (state & Mod1Mask)) {
          qualifier|=qualifierAltLeft|qualifierAltRight|qualifierAlt;
        }
#else
        if (state & Mod1Mask) {
          qualifier|=qualifierAltLeft|qualifierAltRight|qualifierAlt;
        }
#endif

/*        if (state & Mod2Mask) {
          qualifier|=qualifierMetaLeft|qualifierMetaRight;
        }*/
        if (state & Mod3Mask) {
          qualifier|=qualifierSuperLeft|qualifierSuperRight|qualifierSuper;
        }
        if (state & Mod4Mask) {
          qualifier|=qualifierHyperLeft|qualifierHyperRight|qualifierHyper;
        }

        if (state & Button1Mask) {
          qualifier|=qualifierButton1;
        }
        if (state & Button2Mask) {
          qualifier|=qualifierButton2;
        }
        if (state & Button3Mask) {
          qualifier|=qualifierButton3;
        }
        if (state & Button4Mask) {
          qualifier|=qualifierButton4;
        }
        if (state & Button5Mask) {
          qualifier|=qualifierButton5;
        }

        return qualifier;
      }

      Event::Event(const XEvent& event)
      : event(event)
      {
        // no code
      }

      KeyEvent::KeyEvent()
       : valid(true)
      {
        // no code
      }

      /**
        NOTE
        This as a VODisplay-only access-method for initializing the MouseEvent.
        Do not call this method in your code, it is VODisplay only and does only
       exists on X11 implementations of VisualOberon.
      */
      KeyEvent::KeyEvent(XKeyEvent& event, XIC xic)
       : valid(true)
      {
        this->event.xkey=event;
        if (event.type==KeyPress) {
          type=down;
        }
        else {
          type=up;
        }

        qualifier=EvaluateQualifier(event.state);
        if (type==down) {
          if (xic!=0) {
            Status  status;
            int     strLen;


#if defined(X_HAVE_UTF8_STRING)
            char buffer[1024];

            buffer[0]='\0';

            strLen=Xutf8LookupString(xic,(XKeyPressedEvent*)&event,buffer,1023,&xKey,&status);
#else
            wchar_t buffer[1024];

            buffer[0]=L'\0';

            strLen=XwcLookupString(xic,(XKeyPressedEvent*)&event,buffer,1023,&xKey,&status);
#endif
            if (status==XLookupNone) {
              valid=false;
              return;
            }
            else if (status==XBufferOverflow) {
              valid=false;
              return;
            }
            else {
#if defined(X_HAVE_UTF8_STRING)
              buffer[strLen]='\0';

              text=Lum::Base::UTF8ToWString(buffer);
#else
              buffer[strLen]=L'\0';

              text=buffer;
#endif
            }
          }
          else {
            char   buffer[1024];
            int    strLen;

            buffer[0]='\0';

            strLen=XLookupString(&this->event.xkey,buffer,1023,&xKey,&compose);
            buffer[strLen]='\0';
            text=::Lum::Base::StringToWString(buffer);
          }
        }
        else {
          char   buffer[1024];
          int    strLen;

          buffer[0]='\0';

          strLen=XLookupString(&this->event.xkey,buffer,1023,&xKey,&compose);
          buffer[strLen]='\0';
          text=::Lum::Base::StringToWString(buffer);
        }

        switch (xKey) {
        case XK_Return:
        case XK_KP_Enter:
          key=keyReturn;
          break;
        case XK_ISO_Left_Tab:
          key=keyLeftTab;
          break;
        case XK_Tab:
          key=keyTab;
          break;
        case XK_space:
          key=keySpace;
          break;
        case XK_BackSpace:
          key=keyBackspace;
          break;
        case XK_Escape:
          key=keyEscape;
          break;
        case XK_F5:
          key=keyF5;
          break;
        case XK_F6:
          key=keyF6;
          break;
        case XK_F7:
          key=keyF7;
          break;
        case XK_F3:
          key=keyF3;
          break;
        case XK_F8:
          key=keyF8;
          break;
        case XK_F9:
          key=keyF9;
          break;
        case XK_F11:
          key=keyF11;
          break;
        case XK_F10:
          key=keyF10;
          break;
        case XK_F12:
          key=keyF12;
          break;
        case XK_Home:
          key=keyHome;
          break;
        case XK_Prior:
          key=keyPrior;
          break;
        case XK_F4:
          key=keyF4;
          break;
        case XK_End:
          key=keyEnd;
          break;
        case XK_F2:
          key=keyF2;
          break;
        case XK_Next:
          key=keyNext;
          break;
        case XK_F1:
          key=keyF1;
          break;
        case XK_Left:
          key=keyLeft;
          break;
        case XK_Right:
          key=keyRight;
          break;
        case XK_Down:
          key=keyDown;
          break;
        case XK_Up:
          key=keyUp;
          break;
        case XK_Delete:
          key=keyDelete;
          break;
        case XK_Print:
          key=keyPrint;
          break;
        case XK_Insert:
          key=keyInsert;
          break;
        case XK_Shift_L:
          key=keyShiftLeft;
          break;
        case XK_Shift_R:
          key=keyShiftRight;
          break;
        case XK_Shift_Lock:
          key=keyShiftLock;
          break;
        case XK_Caps_Lock:
          key=keyCapsLock;
          break;
        case XK_Scroll_Lock:
          key=keyScrollLock;
          break;
        case XK_Num_Lock:
          key=keyNumLock;
          break;
        case XK_Control_L:
          key=keyControlLeft;
          break;
        case XK_Control_R:
          key=keyControlRight;
          break;
        case XK_Alt_L:
          key=keyAltLeft;
          break;
        case XK_Alt_R:
          key=keyAltRight;
          break;
        case XK_Meta_L:
          key=keyMetaLeft;
          break;
        case XK_Meta_R:
          key=keyMetaRight;
          break;
        case XK_Super_L:
          key=keySuperLeft;
          break;
        case XK_Super_R:
          key=keySuperRight;
          break;
        case XK_Hyper_L:
          key=keyHyperLeft;
          break;
        case XK_Hyper_R:
          key=keyHyperRight;
          break;
        default:
          key=keyUnknown;
          break;
        }
      }

      bool KeyEvent::IsValid() const
      {
        return valid;
      }

      /**
        Returns a string representing the name of key(s) pressed.
      */
      void KeyEvent::GetName(std::wstring& buffer) const
      {
        const char* string;

        if (!GetKeyName(key,buffer)) {
          buffer=L"";
          string=XKeysymToString(xKey);
          if (string!=NULL) {
            buffer=::Lum::Base::StringToWString(string);
          }
        }
      }

/**
  NOTE
    THis as a VODisplay-only access-method for initializing the MouseEvent.
    Do not call this method in your code, it is VODisplay only and does only
    exists on X11 implementations of VisualOberon.
*/
      MouseEvent::MouseEvent(XButtonEvent& event)
      {

        this->event.xbutton=event;
        if (event.type==ButtonPress) {
          type=down;
        }
        else {
          type=up;
        }
        x=event.x;
        y=event.y;
        qualifier=EvaluateQualifier(event.state);

        switch (event.button)
        {
        case Button1:
          button=button1;
          break;
        case Button2:
          button=button2;
          break;
        case Button3:
          button=button3;
          break;
        case Button4:
          button=button4;
          break;
        case Button5:
          button=button5;
          break;
        default:
          button=none;
          break;
        }
      }

/**
  NOTE
    THis as a VODisplay-only access-method for initializing the MouseEvent.
    Do not call this method in your code, it is VODisplay only and does only
    exists on X11 implementations of VisualOberon.
*/
      MouseEvent::MouseEvent(XMotionEvent& event)
      {
        type=move;

        this->event.xmotion=event;
        button=none;

        if (event.is_hint!='\0') {
          if (XQueryPointer(event.display,
                            event.window,
                            &event.root,
                            &event.subwindow,
                            &event.x_root,
                            &event.y_root,
                            &event.x,
                            &event.y,
                            &event.state)==0) {
          }
        }
        x=event.x;
        y=event.y;
        qualifier=EvaluateQualifier(event.state);
      }

/**
  Convert the given X11 event to a Illumination event.
*/
      ::Lum::OS::Event* GetEvent(XEvent event, XIC xic)
      {
        switch (event.type) {
        case ButtonPress:
        case ButtonRelease:
          mouseEvent=new MouseEvent(event.xbutton);
          return mouseEvent;
        case MotionNotify:
          mouseEvent=new MouseEvent(event.xmotion);
          return mouseEvent;
        case KeyPress:
        case KeyRelease:
          keyEvent=new KeyEvent(event.xkey,xic);
          if (dynamic_cast<KeyEvent*>(keyEvent.Get())->IsValid()) {
            return keyEvent;
          }
          else {
            return NULL;
          }
        default:
          simpleEvent=new Event(event);
          return simpleEvent;
        }
      }

      void GetX11Event(OS::Event* event, XEvent& x11Event)
      {
        MouseEvent *mouse;
        KeyEvent   *key;
        Event      *ev;

        if ((mouse=dynamic_cast<MouseEvent*>(event))!=NULL) {
          x11Event=mouse->event;
        }
        else if ((key=dynamic_cast<KeyEvent*>(event))!=NULL) {
          x11Event=key->event;
        }
        else if ((ev=dynamic_cast<Event*>(event))!=NULL) {
          x11Event=ev->event;
        }
      }
    }
  }
}
