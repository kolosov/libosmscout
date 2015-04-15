#ifndef LUM_OS_EVENT_H
#define LUM_OS_EVENT_H

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

#include <string>

#include <Lum/Private/ImportExport.h>

#include <Lum/Base/Reference.h>

namespace Lum {
  namespace OS {

    enum Qualifier {
      qualifierButton1      = (1 <<  0),
      qualifierButton2      = (1 <<  1),
      qualifierButton3      = (1 <<  2),
      qualifierButton4      = (1 <<  3),
      qualifierButton5      = (1 <<  4),

      qualifierCapsLock     = (1 <<  5),

      qualifierShiftLeft    = (1 <<  6),
      qualifierShiftRight   = (1 <<  7),
      qualifierControlLeft  = (1 <<  8),
      qualifierControlRight = (1 <<  9),
      qualifierAltLeft      = (1 << 10),
      qualifierAltRight     = (1 << 12),
      qualifierMetaLeft     = (1 << 13),
      qualifierMetaRight    = (1 << 14),
      qualifierSuperLeft    = (1 << 15),
      qualifierSuperRight   = (1 << 16),
      qualifierHyperLeft    = (1 << 17),
      qualifierHyperRight   = (1 << 18),

      qualifierShift        = (1 << 19),
      qualifierControl      = (1 << 20),
      qualifierAlt          = (1 << 21),
      qualifierMeta         = (1 << 22),
      qualifierSuper        = (1 << 23),
      qualifierHyper        = (1 << 24)
    };

    enum Key {
      keyUnknown,

      // Qualifiers
      keyShiftLeft,
      keyShiftRight,
      keyShiftLock,
      keyCapsLock,
      keyScrollLock,
      keyNumLock,
      keyControlLeft,
      keyControlRight,
      keyAltLeft,
      keyAltRight,
      keyMetaLeft,
      keyMetaRight,
      keySuperLeft,
      keySuperRight,
      keyHyperLeft,
      keyHyperRight,

      // Editing
      keyBackspace,
      keyDelete,

      // Advanced
      keyReturn,

      // Movement
      keyHome,
      keyBegin,
      keyEnd,
      keyLeft,
      keyRight,
      keyUp,
      keyDown,
      keyPrior,
      keyNext,

      // Special keys
      keyTab,
      keyLeftTab,
      keySpace,
      keyEscape,
      keyPrint,
      keyInsert,

      // Function keys

      keyF1,
      keyF2,
      keyF3,
      keyF4,
      keyF5,
      keyF6,
      keyF7,
      keyF8,
      keyF9,
      keyF10,
      keyF11,
      keyF12
    };

    class LUMAPI Event : public Lum::Base::Referencable
    {
    protected:
      Event();

    public:
      virtual ~Event();

    public:
      bool reUse;
    };

    typedef Lum::Base::Reference<Event> EventRef;

    class LUMAPI KeyEvent : public Event
    {
    public:
      enum Type {
        up,
        down
      };

    protected:
      KeyEvent();

    public:
      virtual void GetName(std::wstring& buffer) const = 0;

    public:
      Key           key;
      unsigned long qualifier;
      std::wstring  text;
      Type          type;
    };

    typedef Lum::Base::Reference<KeyEvent> KeyEventRef;

    class LUMAPI MouseEvent : public Event
    {
    public:
      enum Type {
        up,
        down,
        move
      };

      enum Button {
        none    = 0,
        button1 = 1,
        button2 = 2,
        button3 = 3,
        button4 = 4,
        button5 = 5
      };

    protected:
      MouseEvent();

    public:
      Type          type;
      unsigned long qualifier;
      Button        button;
      int           x,y;

    public:
      virtual bool IsGrabStart() const = 0;
      virtual bool IsGrabEnd() const = 0;
      virtual bool IsGrabed() const = 0;

      virtual void SetGrabed(bool grabed) = 0;
    };

    typedef Lum::Base::Reference<MouseEvent> MouseEventRef;

    bool GetKeyName(Key key, std::wstring& buffer);
  }
}

#endif
