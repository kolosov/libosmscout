#ifndef LUM_OS_X11_DISPLAY_H
#define LUM_OS_X11_DISPLAY_H

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

// std
#include <list>
#include <string>

#include <Lum/Private/Config.h>

// X11
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#if defined(HAVE_LIBSM)
#include <X11/SM/SMlib.h>
#endif

#if defined(HAVE_LIB_OSSO)
  #include <libosso.h>
  #include <dbus/dbus.h>
#elif defined(HAVE_LIB_DBUS)
  #include <dbus/dbus.h>
#endif

#include <Lum/OS/Base/Display.h>

#include <Lum/OS/Event.h>
#include <Lum/OS/Thread.h>

#include <Lum/OS/X11/Event.h>

namespace Lum {
  namespace OS {
    namespace X11 {

      /*
      class PatternDesc; // module private
      */

      class Tray;
      class Window;
      class Display;

      class LUMAPI Display : public OS::Base::Display
      {
      private:
        friend class Window;
        friend class Tray;

      public:
        enum Atoms {
          atomAtom                   =  0,
          deleteProtAtom             =  1,
          wmStateAtom                =  2,
          XdndAwareAtom              =  3,
          XdndEnterAtom              =  4,
          XdndLeaveAtom              =  5,
          XdndPositionAtom           =  6,
          XdndStatusAtom             =  7,
          XdndFinishedAtom           =  8,
          XdndDropAtom               =  9,
          XdndActionCopyAtom         = 10,
          XdndActionMoveAtom         = 11,
          XdndActionLinkAtom         = 12,
          XdndActionAskAtom          = 13,
          XdndActionPrivateAtom      = 14,
          XdndActionListAtom         = 15,
          XdndSelectionAtom          = 16,
          XdndTypeListAtom           = 17,
          dropAtom                   = 18,
          selectionAtom              = 19,
          compoundTextAtom           = 20,
          clipboardAtom              = 21,
          clipBufferAtom             = 22,
          netWMNameAtom              = 23,
          netWMWindowTypeAtom        = 24,
          netWMWindowTypeDesktopAtom = 25,
          netWMWindowTypeToolbarAtom = 26,
          netWMWindowTypeMenuAtom    = 27,
          netWMWindowTypeMenuPopupAtom = 28,
          netWMWindowTypeMenuDropdownAtom = 29,
          netWMWindowTypeUtilityAtom = 30,
          netWMWindowTypeSplashAtom  = 31,
          netWMWindowTypeDialogAtom  = 32,
          netWMWindowTypeNormalAtom  = 33,
          netWMPingAtom              = 34,
          netWMPIDAtom               = 35,
          netWMStateAtom             = 36,
          netWMStateFullScreenAtom   = 37,
          netWMContextCustomAtom     = 38,
          netSystemTrayOpcodeAtom    = 39,
          netSystemTrayMsgDataAtom   = 40,
          netWorkAreaAtom            = 41,
          netFrameExtents            = 42,
          netWMWindowOpacity         = 43,
          utf8StringAtom             = 44,
          xEmbedAtom                 = 45,
          xEmbedInfoAtom             = 46,
          cardinalAtom               = 47,
          windowAtom                 = 48,
          targetsAtom                = 49,

          atomCount                  = 50
        };

      public:
        Atom atoms[atomCount];

      private:
        Cursor                    sleepCursor;
        Cursor                    popCursor;
        Cursor                    dndCursor;
        Cursor                    copyCursor;
        Cursor                    moveCursor;
        Cursor                    linkCursor;

        std::list<Tray*>          trayList;
        std::list<Window*>        winList;
        Window                    *currentWin;

        Lum::Base::DataExchangeObject *selectObject;
        Lum::Base::DataExchangeObject *querySelectObject;
        Lum::Base::DataExchangeObject *queryClipboardObject;
        bool                      selClearPend;

        //::Lum::OS::Display::Timer contextTimer;
        bool                      contextHelp;

        Window*                   dropWindow;

        std::wstring              clipboard;

        double                    dpi;

#if defined(HAVE_LIBSM)
        SmcConn                   smSession;
        IceConn                   iceSession;
        int                       iceFd;
#endif

        //::Lum::OS::Display::Object dragObject;
        //::Lum::Base::DragDrop::DnDDataInfo dragInfo;
        //int dndAction;
        //int dragX;
        //int dragY;
        //bool dragging;
        //bool dragStart;

      public:
        ::Window                  appWindow;
        int                       scrNum;
        ::Display                 *display;
        Visual                    *visual;
        Colormap                  colorMap;
        XIM                       xim;

#if defined(HAVE_LIB_OSSO)
        osso_context_t            *ossoContext;
#endif

#if defined(HAVE_LIB_DBUS) || defined(HAVE_LIB_OSSO)
        DBusConnection            *dbusSession;
        DBusConnection            *dbusSystem;
#endif

#if defined(HAVE_LIB_XRANDR)
        bool                      xrandrActive;
        int                       xrandrMinor,xrandrMajor;
        int                       xrandrEventBase,xrandrErrorBase;
#endif

      private:
        void GetWorkAreaDimension();

      public:
        Display();
        ~Display();

        double GetDPI() const;

        void Beep();

        FontRef GetFontInternal(FontType type, size_t size) const;
        //::Lum::OS::Display::FontList GetFontList();

        bool AllocateColor(double red, double green, double blue,
                           OS::Color& color);
        bool AllocateNamedColor(const std::string& name,
                                OS::Color& color);
        void FreeColor(OS::Color color);

        bool Open();
        void Close();

        bool RegisterSelection(Lum::Base::DataExchangeObject* object, OS::Window* window);
        void CancelSelection();
        Lum::Base::DataExchangeObject* GetSelectionOwner() const;
        bool QuerySelection(OS::Window* window, Lum::Base::DataExchangeObject* object);

        bool SetClipboard(const std::wstring& content);
        std::wstring GetClipboard() const;
        void ClearClipboard();
        bool HasClipboard() const;

        void ReinitWindows();

        bool GetMousePos(int& x, int& y) const;

#if defined(HAVE_LIBSM)
        //Session Management
        void CallbackDie(SmcConn smc_conn, SmPointer client_data);
        void CallbackSaveYourself(SmcConn smc_conn, SmPointer client_data,
                                  int save_style, Bool shutdown,
                                  int interact_style,
                                  Bool fast);
        void CallbackShutdownCancelled(SmcConn smc_conn, SmPointer client_data);
        void CallbackSaveComplete(SmcConn smc_conn, SmPointer client_data);
        void CallbackICEIOError(IceConn connection);
#endif

#if defined(HAVE_LIB_OSSO)
        osso_context_t* InitializeLibOsso();
#elif defined(HAVE_LIB_DBUS)
        DBusConnection* InitiateDBusConnection(DBusBusType busType);
#endif

        void SetAtomProperty(Window* window, Atom property, Atom value);
        void SetCardinalProperty(Window* window, Atom property, int value);
        void SetIntegerProperty(Window* window, Atom property, int value);
        void SetEmbedInfoProperty(::Window window);

      private:
        void EnlargeUCS2Buffer(int len);

        void SendNetSystemTrayRequestDock(::Window window);
        void SendNetSystemTrayBaloonMessage(Window* window);

        Window* GetWindowOnScreen(int rX, int rY, int cX, int cY);
        Window* GetWindow(::Window window);
        Tray* GetTray(::Window window);

        void StopContextHelp();
        void RestartContextHelp();
        void StartContextHelp();

        void AddWindow(Window* w);
        void RemoveWindow(Window* w);

        void AddTray(Tray* tray);
        void RemoveTray(Tray* tray);

        void HandleXSelectionNotify(const XSelectionEvent& event);
        void HandleXSelectionRequest(const XSelectionRequestEvent& event);

        void Flush();

      public:
        bool WaitTimedForX11Event(::Window window, int eventType, XEvent& event) const;
        bool WaitTimedForPropertyChange(::Window window, Atom property, XEvent& event) const;
        void GetEvent();
      };
    }
  }
}

#endif
