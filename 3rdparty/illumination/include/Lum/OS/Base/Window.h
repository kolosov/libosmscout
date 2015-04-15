#ifndef LUM_OS_BASE_WINDOW_H
#define LUM_OS_BASE_WINDOW_H

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

#include <Lum/OS/Window.h>

#include <Lum/OS/DrawInfo.h>

#include <Lum/OS/Base/Event.h>

namespace Lum {
  namespace OS {
    namespace Base {

      class LUMAPI Window : public OS::Window
      {
      protected:
        bool                 onOpened;
        Lum::Base::MsgObject *master;          //! Potential master object (normally instance of Lum::Dialog)
        Model::ActionRef     closedAction;     //! Triggered, when window gets closed by user interaction
        Model::ActionRef     exitAction;       //! Action, the window listen on to be exited
        Model::ActionRef     mapedAction;      //! Triggered when the window gets maped
        Model::ActionRef     unmapedAction;    //! Triggered when the window gets unmaped
        Model::ActionRef     preOpenedAction;  //! Triggered when the window has been opened and first drawn the first time
        Model::ActionRef     openedAction;     //! Triggered when the window has been opened and drawn
        Model::ActionRef     hiddenAction;     //! Triggered when the window gets hidden
        Model::ActionRef     focusInAction;    //! Triggered when the window gets the focus
        Model::ActionRef     focusOutAction;   //! Triggered when the window has lost the focus
        Model::ActionRef     mouseInAction;    //! Triggered when the window gets the focus
        Model::ActionRef     mouseOutAction;   //! Triggered when the window has lost the focus
        Model::ActionRef     drawAction;       //! Triggered when the application wants to redraw a given area
        Model::ActionRef     resizeAction;     //! Triggered when the window has been resized
        Model::ActionRef     preInitAction;    //! Triggered before the window initially opens
        Model::ActionRef     eventAction;      //! Triggered for every low-level event
        Model::ActionRef     grabCancelAction; //! Triggered if a sub window opens and cancels our current mouse grab
        Model::ActionRef     menuAction;       //! Triggered if the windo should show its window (embedded app context)

        std::wstring         title;            //! Title of window

        bool                 isInited;         //! true, if the PreInit action has been triggered
        bool                 open;             //! Window is open
        bool                 inEventLoop;      //! True, if window is in event loop
        bool                 shown;            //! Window has been redrawn after open
        bool                 focus;            //! Windows has focus

        bool                 activateOnOpen;   //! Activate windows after it opens
        bool                 hasMenuHint;      //! Optional hint that the window has a menu
        ScreenOrientationHint screenOrientationHint;

        int                  x,y;
        size_t               width,height;
        size_t               minWidth,minHeight;
        size_t               maxWidth,maxHeight;
        OS::Window::Position horizontalPos;
        OS::Window::Position verticalPos;
        double               opacity;
        int                  modalCount;
        Window               *parent;          //! Parent window
        OS::DrawInfo         *draw;
        OS::DrawInfo         *backgroundDraw;
        OS::Bitmap           *backgroundBitmap;
        OS::Window::Type     type;
        bool                 grabMouse;
        bool                 grabKeyboard;
        MouseEvent           oldButton;
        MouseEvent           lastButton;
        MouseEvent           thisButton;
        Lum::Base::SystemTime oldPress;
        Lum::Base::SystemTime lastPress;
        Lum::Base::SystemTime thisPress;

      public:
        Window();
        ~Window();

        void Resync(Lum::Base::Model* model,
                    const Lum::Base::ResyncMsg& msg);

        void SetMaster(Lum::Base::MsgObject* master);
        Lum::Base::MsgObject* GetMaster() const;

        void SetParent(OS::Window* parent);
        OS::Window* GetParent() const;

        void SetType(OS::Window::Type type);
        OS::Window::Type GetType() const;

        void SetHasMenuHint(bool hasMenu);
        bool GetHasMenuHint() const;

        void SetScreenOrientationHint(ScreenOrientationHint hint);
        ScreenOrientationHint GetScreenOrientationHint() const;

        void SetTitle(const std::wstring& name);
        std::wstring GetTitle() const;

        void SetSize(size_t width, size_t height);
        void SetMinMaxSize(size_t minWidth,
                           size_t minHeight,
                           size_t maxWidth,
                           size_t maxHeight);

        void SetPos(int x, int y);
        void SetPosition(OS::Window::Position horiz, OS::Window::Position vert);

        bool SetOpacity(double opacity);
        double GetOpacity() const;

        int GetX() const;
        int GetY() const;
        size_t GetWidth() const;
        size_t GetHeight() const;
        size_t GetMinWidth() const;
        size_t GetMinHeight() const;
        size_t GetMaxWidth() const;
        size_t GetMaxHeight() const;

        void SetExitAction(Model::Action* action);

        Model::Action* GetExitAction() const;
        Model::Action* GetClosedAction() const;
        Model::Action* GetMapedAction() const;
        Model::Action* GetUnmapedAction() const;
        Model::Action* GetPreOpenedAction() const;
        Model::Action* GetOpenedAction() const;
        Model::Action* GetHiddenAction() const;
        Model::Action* GetFocusInAction() const;
        Model::Action* GetFocusOutAction() const;
        Model::Action* GetMouseInAction() const;
        Model::Action* GetMouseOutAction() const;
        Model::Action* GetResizeAction() const;
        Model::Action* GetDrawAction() const;
        Model::Action* GetPreInitAction() const;
        Model::Action* GetEventAction() const;
        Model::Action* GetGrabCancelAction() const;
        Model::Action* GetMenuAction() const;

        OS::Window* GetTopWindow();

        bool CursorIsIn() const;
        bool HasFocus() const;

        bool IsInited() const;
        bool Open(bool activate);
        bool IsOpen() const;
        void Close();
        bool IsShown() const;

        void EventLoop();
        bool IsInEventLoop() const;
        void Exit();
        bool HandleEvent(Event* event);
        void Enable();
        void Disable();
        bool IsEnabled() const;
        void EnableParents();
        void DisableParents();

        void Resize(size_t width, size_t height);

        // action
        void GrabMouse(bool grab);
        void GrabKeyboard(bool grab);

        bool IsDoubleClicked() const;
        const OS::MouseEvent& GetLastButtonClickEvent() const;

        void AcquireKeyboardInput();
        void ReleaseKeyboardInput();

        bool SetFullScreen();
        bool LeaveFullScreen();
        bool IsFullScreen() const;

        DrawInfo* RequestDrawInfo(int x, int y, size_t width, size_t height);
        void ReleaseDrawInfo(int x, int y, size_t width, size_t height);
        DrawInfo* RequestRefreshDrawInfo(int x, int y, size_t width, size_t height);
        void ReleaseRefreshDrawInfo(int x, int y, size_t width, size_t height);

        void TriggerMaped();
        void TriggerUnmaped();
        void TriggerPreOpened();
        void TriggerOpened();
        void TriggerClosed();
        void TriggerHidden();
        void TriggerFocusIn();
        void TriggerFocusOut();
        void TriggerMouseIn();
        void TriggerMouseOut();
        void TriggerResize(size_t width, size_t height);
        void TriggerGrabCancel();
        void TriggerMenu();

        bool SendBalloonMessage(const std::wstring& text,
                                unsigned long timeout);

      protected:
        virtual void MouseGrabOn();
        virtual void MouseGrabOff();
        virtual void KeyboardGrabOn();
        virtual void KeyboardGrabOff();

        void CalculateWindowPos(bool& xManPos, bool& yManPos);
      };
    }
  }
}

#endif


