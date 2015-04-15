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

#include <Lum/OS/Carbon/Event.h>

#include <Lum/Base/Util.h>
#include <iostream>
namespace Lum {
  namespace OS {
    namespace Carbon {

      static Event      *simpleEvent=NULL;
      static MouseEvent *mouseEvent=NULL;
      static KeyEvent   *keyEvent=NULL;

      unsigned long GetQualifiers(::EventRef event)
      {
        unsigned long system=0,qualifier=0;
        //button    : Carbon.EventMouseButton;

        if (::GetEventParameter(event,
                                ::kEventParamKeyModifiers,
                                ::typeUInt32,
                                NULL,
                                sizeof(unsigned long),
                                NULL,
                                &system)==::noErr) {
          if (::shiftKeyBit & system) {
            qualifier|=(qualifierShiftLeft|qualifierShiftRight);
          }
          if (::kEventKeyModifierNumLockBit & system) {
            qualifier|=qualifierCapsLock;
          }
          if (::controlKeyBit & system) {
            qualifier|=(qualifierControlLeft|qualifierControlRight);
          }
          if (::cmdKeyBit & system) {
            qualifier|=(qualifierAltLeft|qualifierAltRight);
          }
          if (::optionKeyBit & system) {
            qualifier|=(qualifierMetaLeft|qualifierMetaRight);
          }
        }
/*    IF Carbon.GetEventParameter(event,
                                Carbon.kEventParamMouseButton,
                                Carbon.typeMouseButton,
                                NIL,
                                SIZE(Carbon.EventMouseButton),
                                NIL,
                                SYSTEM.VAL(LONGINT,SYSTEM.ADR(button)))=0 THEN


      IF button MOD 2=1 THEN
        INCL(qualifier,E.button1);
      END;
    END;*/

        return qualifier;
      }

      bool KeyEvent::SetCarbonEvent(::EventRef event)
      {
        char          character[2];
        unsigned long keycode;

        this->event=event;

        switch (::GetEventKind(event)) {
        case ::kEventRawKeyDown:
        case ::kEventRawKeyRepeat:
          type=down;
          break;
        case kEventRawKeyUp:
          type=up;
          break;
        }

        character[0]=0;
        character[1]=0;
        if (GetEventParameter(event,
                              ::kEventParamKeyMacCharCodes,
                              ::typeChar,
                              NULL,
                              sizeof(char),
                              NULL,
                              character)!=::noErr) {
          character[0]=0;
        }

        text=::Lum::Base::StringToWString(character);

        qualifier=GetQualifiers(event);

        if (GetEventParameter(event,
                              ::kEventParamKeyCode,
                              ::typeUInt32,
                              NULL,
                              sizeof(unsigned long),
                              NULL,
                              &keycode)==::noErr) {
          switch (keycode) {
          case 0x24:
            key=keyReturn;
            break;
           case 0x30:
            if (qualifier & (qualifierShiftLeft|qualifierShiftRight)) {
              key=keyLeftTab;
            }
            else {
              key=keyTab;
            }
            break;
          case 0x31:
            key=keySpace;
            break;
          case 0x33:
            key=keyBackspace;
            break;
          case 0x35:
            key=keyEscape;
            break;
          case 0x60:
            key=keyF5;
          case 0x61:
            key=keyF6;
            break;
          case 0x62:
            key=keyF7;
            break;
          case 0x63:
            key=keyF3;
            break;
          case 0x64:
            key=keyF8;
            break;
          case 0x65:
            key=keyF9;
            break;
          case 0x67:
            key=keyF11;
            break;
          case 0x6D:
            key=keyF10;
            break;
          case 0x6F:
            key=keyF12;
            break;
          case 0x73:
            key=keyHome;
            break;
          case 0x74:
            key=keyPrior;
            break;
          case 0x76:
            key=keyF4;
            break;
          case 0x77:
            key=keyEnd;
            break;
          case 0x78:
            key=keyF2;
            break;
          case 0x79:
            key=keyNext;
            break;
          case 0x7A:
            key=keyF1;
            break;
          case 0x7B:
            key=keyLeft;
            break;
          case 0x7C:
            key=keyRight;
            break;
          case 0x7D:
            key=keyDown;
            break;
          case 0x7E:
            key=keyUp;
            break;
          default:
            key=keyUnknown;
            break;
          }
        }
        else {
          key=keyUnknown;
        }

        return true;
      }

      /**
        Returns a string representing the name of key(s) pressed.
      */
      void KeyEvent::GetName(std::wstring& buffer) const
      {
        /*
        const char* string;

        if (!GetKeyName(key,buffer)) {
          buffer=L"";
          string=XKeysymToString(xKey);
          if (string!=NULL) {
            buffer=::Lum::Base::StringToWString(string);
          }
        }*/
      }

      void MouseEvent::SetCarbonEvent(::EventRef event)
      {
        ::Point            point;
        ::EventMouseButton button;

        this->event=event;

        switch (::GetEventKind(event)) {
        case ::kEventMouseUp:
          type=up;
          break;
        case ::kEventMouseDown:
          type=down;
          break;
        case ::kEventMouseMoved:
        case ::kEventMouseDragged:
          type=move;
          break;
        }

        if (::GetEventParameter(event,
                                ::kEventParamMouseLocation,
                                ::typeQDPoint,
                                NULL,
                                sizeof(::Point),
                                NULL,
                                &point)==::noErr) {
          ::GlobalToLocal(&point);
          x=point.h;
          y=point.v;
        }
        else {
          x=-1;
          y=-1;
        }

        if (type==down || type==up) {
          if (::GetEventParameter(event,
                                  ::kEventParamMouseButton,
                                  ::typeMouseButton,
                                  NULL,
                                  sizeof(::EventMouseButton),
                                  NULL,
                                  &button)==::noErr) {
            if (button==2) {
              this->button=button3;
            }
            else if (button==3) {
              this->button=button2;
            }
            else {
              this->button=(Button)button;
            }
          }
        }
        else {
          this->button=none;
        }

        qualifier=GetQualifiers(event);

        if (type==up && this->button>=button1 && this->button<=button5) {
          qualifier|= (1 << (this->button-1)); // TODO: Remove hardcoded!!!
        }
      }

      /**
        Convert the given Carbon event to a Illumination event.
      */
      ::Lum::OS::EventPtr GetEvent(::EventRef event)
      {
        switch (GetEventClass(event)) {
        case kEventClassMouse:
          if (mouseEvent==NULL) {
            mouseEvent=new MouseEvent;
          }
          mouseEvent->SetCarbonEvent(event);
          return mouseEvent;
          break;
        case kEventClassKeyboard:
          if (keyEvent==NULL) {
            keyEvent=new KeyEvent;
          }
          if (keyEvent->SetCarbonEvent(event)) {
            return keyEvent;
          }
          else {
            return NULL;
          }
          break;
        default:
          if (simpleEvent==NULL) {
            simpleEvent=new Event;
          }
          simpleEvent->event=event;
          return simpleEvent;
          break;
        }
      }

      void GetCarbonEvent(::Lum::OS::EventPtr event, ::EventRef& carbonEvent)
      {
        MouseEvent *mouse;
        KeyEvent   *key;
        Event      *ev;

        if ((mouse=dynamic_cast<MouseEvent*>(event))!=NULL) {
          carbonEvent=mouse->event;
        }
        else if ((key=dynamic_cast<KeyEvent*>(event))!=NULL) {
          carbonEvent=key->event;
        }
        else if ((ev=dynamic_cast<Event*>(event))!=NULL) {
          carbonEvent=ev->event;
        }
        else {
          assert(false);
        }
      }
    }
  }
}
