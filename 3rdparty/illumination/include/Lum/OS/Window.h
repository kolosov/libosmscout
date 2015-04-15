#ifndef LUM_OS_WINDOW_H
#define LUM_OS_WINDOW_H

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

#include <Lum/Private/ImportExport.h>

#include <Lum/Base/Object.h>

#include <Lum/Model/Action.h>

#include <Lum/OS/DrawInfo.h>
#include <Lum/OS/Event.h>

namespace Lum {
  namespace OS {

    /**
      Representation of on OS desktop window.
    */
    class LUMAPI Window : public Lum::Base::MsgObject
    {
    public:
      enum Type {
        typeToolbar,
        typeTooltip,
        typeMenu,
        typeMenuDropdown,
        typeMenuPopup,
        typePopup,
        typeUtility,
        typeSplash,
        typeDialog,
        typeMain,
        typeAuto,
        typeCustom    //! Framework will leave the window allown and set any type hints
      };

      enum Position {
        positionManual,
        positionCenterOnParent,
        positionCenterOnScreen,
        positionOS
      };

      enum ScreenOrientationHint {
        screenOrientationAgnostic,
        screenOrientationLandscapeSupported,
        screenOrientationPortraitSupported,
        screenOrientationBothSupported
      };

      class LUMAPI DrawMsg : public ::Lum::Base::ResyncMsg
      {
      public:
        DrawInfo* draw;
        int       x;
        int       y;
        size_t    width;
        size_t    height;
      };

      class LUMAPI ResizeMsg : public ::Lum::Base::ResyncMsg
      {
      public:
        size_t width;
        size_t height;
      };

      class LUMAPI EventMsg : public ::Lum::Base::ResyncMsg
      {
      public:
        Event *event;
      };

    public:
      virtual void SetMaster(Lum::Base::MsgObject* master) = 0;
      virtual Lum::Base::MsgObject* GetMaster() const = 0;

      /**
        @name Window type
      */
      //@{
      virtual void SetType(Window::Type type) = 0;
      virtual Window::Type GetType() const = 0;
      //@}

      /**
        @name Window hints.
       */
      //@{
      /**
        If set to true, this signals that the window contains a main menu.
        This might have influence on the window visualization depending on the platform.
       */
      virtual void SetHasMenuHint(bool hasMenu) = 0;
      /**
        Returns true, if a menu hint has been set, else false.
       */
      virtual bool GetHasMenuHint() const = 0;

      /**
        If screenOrientationLandscapeSupported, the window layout is feasible under landscape
        displays (and not under portrait), if screenOrrientationPortraitSupported i set, the window
        layout is feasible under portrait (and not under landscape). If screenOrientationAgnostic
        is set, this means that the default orientation of the device is supported (somewhat) but the
        application is agnostic of screen rotation. It would snot change its layout if screen
        orientation is changed. ScreenOrientationBothSupported means that both orientations
        are supported and layout changes to fit to the new orientation.

        If screen rotation is detected and the new orientation is supported, Illumination
        might trigger a window reinit to initiate a layout reconstruction, expecting the window
        to supply a layout that fits the new orientation.
       */
      virtual void SetScreenOrientationHint(ScreenOrientationHint hint) = 0;

      /**
        Returns true, if the landscape orientation is supported, else false.
       */
      virtual ScreenOrientationHint GetScreenOrientationHint() const = 0;
      //@}

      /**
        @name Parent window
       */
      //@{
      /**
        Set the parent window of this window. A window can have a parent window. If a parent windows
        has been set you cannot reset the parent window while the window is open. Settings
        a parent windows doe snot influence the modality of the window, but else the visualization
        or behaviour may be influenced.
       */
      virtual void SetParent(Window* parent) = 0;
      /**
        Return the parent window or NULL.
       */
      virtual Window* GetParent() const = 0;
      //@}

      /**
        @name Window title
       */
      //@{
      /**
        Set the window title.
       */
      virtual void SetTitle(const std::wstring& name) = 0;
      /**
        Return the current window title.
       */
      virtual std::wstring GetTitle() const = 0;
      //@}

      /**
        @name Window opacity
       */
      //@{
      virtual bool SetOpacity(double opacity) = 0;
      virtual double GetOpacity() const = 0;
      //@}

      /**
        @name Position and size
        Method to set get and position and size of the window.
      */
      //@{
      virtual void SetSize(size_t width, size_t height) = 0;
      virtual void SetMinMaxSize(size_t minWidth,
                                 size_t minHeight,
                                 size_t maxWidth,
                                 size_t maxHeight) = 0;
      virtual void SetPos(int x, int y) = 0;
      virtual void SetPosition(Window::Position horiz, Window::Position vert) = 0;

      virtual int GetX() const = 0;
      virtual int GetY() const = 0;
      virtual size_t GetWidth() const = 0;
      virtual size_t GetHeight() const = 0;
      virtual size_t GetMinWidth() const = 0;
      virtual size_t GetMinHeight() const = 0;
      virtual size_t GetMaxWidth() const = 0;
      virtual size_t GetMaxHeight() const = 0;
      //@}

      /**
        @name Actions
        The Window offers a number of action that you can listen to get notified about
        special events
      */
      //@{
      virtual void SetExitAction(::Lum::Model::Action* action) = 0;

      virtual Model::Action* GetExitAction() const = 0;
      virtual Model::Action* GetClosedAction() const = 0;
      virtual Model::Action* GetMapedAction() const = 0;
      virtual Model::Action* GetUnmapedAction() const = 0;
      virtual Model::Action* GetPreOpenedAction() const = 0;
      virtual Model::Action* GetOpenedAction() const = 0;
      virtual Model::Action* GetHiddenAction() const = 0;
      virtual Model::Action* GetFocusInAction() const = 0;
      virtual Model::Action* GetFocusOutAction() const = 0;
      virtual Model::Action* GetMouseInAction() const = 0;
      virtual Model::Action* GetMouseOutAction() const = 0;
      virtual Model::Action* GetResizeAction() const = 0;
      virtual Model::Action* GetDrawAction() const = 0;
      virtual Model::Action* GetPreInitAction() const = 0;
      virtual Model::Action* GetEventAction() const = 0;
      virtual Model::Action* GetGrabCancelAction() const =0;
      virtual Model::Action* GetMenuAction() const =0;
      //@}

      /**
        @name Mouse
        Methods reading mouse position and chekcing for double and tripple clicks.
      */
      //@{
      virtual bool GetMousePos(int& x, int& y) const = 0;
      virtual bool CursorIsIn() const = 0;
      virtual bool HasFocus() const = 0;
      virtual bool IsDoubleClicked() const = 0;
      virtual const MouseEvent& GetLastButtonClickEvent() const = 0;
      //@}

      /**
        @name Window visibility
      */
      //@{
      virtual bool IsInited() const = 0;
      virtual bool Open(bool activate=true) = 0;
      virtual void Close() = 0;
      virtual bool IsOpen() const = 0;

      virtual bool Show(bool activate=true) = 0;
      virtual bool Hide() = 0;
      virtual bool IsShown() const = 0;
      //@}

      /**
        @name Event loop and event handling
      */
      //@{
      virtual bool HandleEvent(Event* event) = 0;
      virtual void Exit() = 0;
      virtual void EventLoop() = 0;
      virtual bool IsInEventLoop() const = 0;
      virtual void Enable() = 0;
      virtual void Disable() = 0;
      virtual bool IsEnabled() const = 0;
      virtual void EnableParents() = 0;
      virtual void DisableParents() = 0;
      //@}

      /**
        @name Full screen methods
      */
      //@{
      virtual bool SetFullScreen() = 0;
      virtual bool LeaveFullScreen() = 0;
      virtual bool IsFullScreen() const = 0;
      //@}

      /**
        @name Double buffering methods
      */


      virtual Window* GetTopWindow() = 0;

      virtual void Resize(size_t width, size_t height) = 0;

      virtual void GrabMouse(bool grab) = 0;
      virtual void GrabKeyboard(bool grab) = 0;

      virtual bool SendBalloonMessage(const std::wstring& text,
                                      unsigned long timeout) = 0;

      virtual void TriggerResize(size_t width, size_t height) = 0;
      virtual void TriggerDraw(int x, int y, size_t width, size_t height) = 0;
    };
  }
}

#endif
