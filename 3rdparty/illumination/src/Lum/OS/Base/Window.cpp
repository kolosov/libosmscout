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

#include <Lum/OS/Base/Window.h>

#include <Lum/OS/Driver.h>
#include <Lum/OS/Theme.h>
#include <Lum/Manager/Display.h>

namespace Lum {
  namespace OS {
    namespace Base {

      Window::Window()
      : onOpened(false),
        master(NULL),
        closedAction(new Model::Action),
        mapedAction(new Model::Action),
        unmapedAction(new Model::Action),
        preOpenedAction(new Model::Action),
        openedAction(new Model::Action),
        hiddenAction(new Model::Action),
        focusInAction(new Model::Action),
        focusOutAction(new Model::Action),
        mouseInAction(new Model::Action),
        mouseOutAction(new Model::Action),
        drawAction(new Model::Action),
        resizeAction(new Model::Action),
        preInitAction(new Model::Action),
        eventAction(new Model::Action),
        grabCancelAction(new Model::Action()),
        menuAction(new Model::Action()),
        isInited(false),
        open(false),
        inEventLoop(false),
        shown(false),
        focus(false),
        activateOnOpen(false),
        hasMenuHint(false),
        screenOrientationHint(screenOrientationAgnostic),
        x(0),y(0),width(0),height(0),
        minWidth(0),minHeight(0),
        maxWidth(32000),maxHeight(32000),
        horizontalPos(positionOS),
        verticalPos(positionOS),
        opacity(1.0),
        modalCount(0),
        parent(NULL),
        draw(NULL),
        backgroundDraw(NULL),
        backgroundBitmap(NULL),
        type(typeAuto),
        grabMouse(false),
        grabKeyboard(false)
      {
        // no code
      }

      Window::~Window()
      {
        // no code
      }

      void Window::Resync(Lum::Base::Model* model,
                          const Lum::Base::ResyncMsg& /*msg*/)
      {
        if (model==exitAction && exitAction->IsFinished()) {
          Exit();
        }
      }

      void Window::SetMaster(Lum::Base::MsgObject* master)
      {
        this->master=master;
      }

      void Window::SetExitAction(Model::Action* action)
      {
        if (exitAction.Valid()) {
          Forget(exitAction);
        }

        exitAction=action;

        if (exitAction.Valid()) {
          Observe(exitAction);
        }
      }

      Lum::Base::MsgObject* Window::GetMaster() const
      {
        return master;
      }

      Model::Action* Window::GetExitAction() const
      {
        return exitAction.Get();
      }

      Model::Action* Window::GetClosedAction() const
      {
        return closedAction.Get();
      }

      Model::Action* Window::GetMapedAction() const
      {
        return mapedAction.Get();
      }

      Model::Action* Window::GetUnmapedAction() const
      {
        return unmapedAction.Get();
      }

      Model::Action* Window::GetPreOpenedAction() const
      {
        return preOpenedAction.Get();
      }

      Model::Action* Window::GetOpenedAction() const
      {
        return openedAction.Get();
      }

      Model::Action* Window::GetHiddenAction() const
      {
        return hiddenAction.Get();
      }

      Model::Action* Window::GetFocusInAction() const
      {
        return focusInAction.Get();
      }

      Model::Action* Window::GetFocusOutAction() const
      {
        return focusOutAction.Get();
      }

      Model::Action* Window::GetMouseInAction() const
      {
        return mouseInAction.Get();
      }

      Model::Action* Window::GetMouseOutAction() const
      {
        return mouseOutAction.Get();
      }

      Model::Action* Window::GetResizeAction() const
      {
        return resizeAction.Get();
      }

      Model::Action* Window::GetDrawAction() const
      {
        return drawAction.Get();
      }

      Model::Action* Window::GetPreInitAction() const
      {
        return preInitAction.Get();
      }

      Model::Action* Window::GetEventAction() const
      {
        return eventAction.Get();
      }

      Model::Action* Window::GetGrabCancelAction() const
      {
        return grabCancelAction.Get();
      }

      Model::Action* Window::GetMenuAction() const
      {
        return menuAction.Get();
      }

      void Window::SetParent(OS::Window* parent)
      {
        if (!IsOpen()) {
          this->parent=dynamic_cast<Window*>(parent);
        }
      }

      void Window::SetType(OS::Window::Type type)
      {
        this->type=type;
      }

      void Window::SetHasMenuHint(bool hasMenu)
      {
        hasMenuHint=hasMenu;

      }

      bool Window::GetHasMenuHint() const
      {
        return hasMenuHint;
      }

      void Window::SetScreenOrientationHint(ScreenOrientationHint hint)
      {
        screenOrientationHint=hint;
      }

      Window::ScreenOrientationHint Window::GetScreenOrientationHint() const
      {
        return screenOrientationHint;
      }

      void Window::SetTitle(const std::wstring& name)
      {
        if (!name.empty()) {
          title=name;
        }
        else {
          title=OS::display->GetAppName();
        }
      }

      void Window::SetSize(size_t width, size_t height)
      {
        if (!IsOpen()) {
          this->width=width;
          this->height=height;
        }
      }

      void Window::SetMinMaxSize(size_t minWidth,
                                 size_t minHeight,
                                 size_t maxWidth,
                                 size_t maxHeight)
      {
        this->minWidth=minWidth;
        this->minHeight=minHeight;
        this->maxWidth=maxWidth;
        this->maxHeight=maxHeight;
      }

      void Window::SetPos(int x, int y)
      {
        if (!IsOpen()) {
          this->x=x;
          this->y=y;
        }
      }

      void Window::SetPosition(OS::Window::Position horiz, OS::Window::Position vert)
      {

        horizontalPos=horiz;
        verticalPos=vert;
      }

      bool Window::SetOpacity(double opacity)
      {
        this->opacity=opacity;

        return false;
      }

      double Window::GetOpacity() const
      {
        return opacity;
      }

      int Window::GetX() const
      {
        return x;
      }

      int Window::GetY() const
      {
        return y;
      }

      size_t Window::GetWidth() const
      {
        return width;
      }

      size_t Window::GetHeight() const
      {
        return height;
      }

      size_t Window::GetMinWidth() const
      {
        return minWidth;
      }

      size_t Window::GetMinHeight() const
      {
        return minHeight;
      }

      size_t Window::GetMaxWidth() const
      {
        return maxWidth;
      }

      size_t Window::GetMaxHeight() const
      {
        return maxHeight;
      }

      bool Window::IsOpen() const
      {
        return open;
      }

      bool Window::IsShown() const
      {
        return shown;
      }

      bool Window::HasFocus() const
      {
        return focus;
      }

      OS::Window* Window::GetParent() const
      {
        return parent;
      }

      DrawInfo* Window::RequestDrawInfo(int x, int y, size_t width, size_t height)
      {
        assert(backgroundBitmap==NULL);

        backgroundBitmap=driver->CreateBitmap(GetWidth(),GetHeight());

        if (backgroundBitmap==NULL) {
          return draw;
        }

        backgroundDraw=backgroundBitmap->GetDrawInfo();

        if (backgroundDraw==NULL) {
          delete backgroundBitmap;
          backgroundBitmap=NULL;

          return draw;
        }

        draw->CopyToBitmap(x,y,width,height,x,y,backgroundBitmap);

        return backgroundDraw;
      }

      void Window::ReleaseDrawInfo(int x, int y, size_t width, size_t height)
      {
        if (backgroundBitmap!=NULL) {
          draw->CopyFromBitmap(backgroundBitmap,x,y,width,height,x,y);

          delete backgroundBitmap;
          backgroundBitmap=NULL;
          backgroundDraw=NULL;
        }
      }

      DrawInfo* Window::RequestRefreshDrawInfo(int x, int y, size_t width, size_t height)
      {
        assert(backgroundBitmap==NULL);

        backgroundBitmap=driver->CreateBitmap(GetWidth(),GetHeight());

        if (backgroundBitmap==NULL) {
          return draw;
        }

        backgroundDraw=backgroundBitmap->GetDrawInfo();

        if (backgroundDraw==NULL) {
          delete backgroundBitmap;
          backgroundBitmap=NULL;

          return draw;
        }

        draw->CopyToBitmap(x,y,width,height,x,y,backgroundBitmap);

        return backgroundDraw;
      }

      void Window::ReleaseRefreshDrawInfo(int x, int y, size_t width, size_t height)
      {
        if (backgroundBitmap!=NULL) {
          draw->CopyFromBitmap(backgroundBitmap,x,y,width,height,x,y);

          delete backgroundBitmap;
          backgroundBitmap=NULL;
          backgroundDraw=NULL;
        }
      }

      OS::Window::Type Window::GetType() const
      {
        return type;
      }

      OS::Window* Window::GetTopWindow()
      {
        Window *window;

        if (parent==NULL) {
          return NULL;
        }
        else {
          window=this;
          while (window->parent!=NULL) {
            window=window->parent;
          }
          return window;
        }
      }

      std::wstring Window::GetTitle() const
      {
        return title;
      }

      /**
        returns 'true', if the mouse cursor ins withinthe window, else
        'false'.
      */
      bool Window::CursorIsIn() const
      {
        int x;
        int y;

        if (GetMousePos(x,y)) {
          return x>=0 && x<(int)GetWidth() && y>=0 && y<(int)GetHeight();
        }
        else {
          return false;
        }
      }

      bool Window::IsInited() const
      {
        return isInited;
      }

      /**
        Opens the window.

        \note
        Derived classes must call the baseclass method.
      */
      bool Window::Open(bool activate)
      {
        assert(!IsOpen());

        if (type==OS::Window::typeAuto) {
          if (parent!=NULL) {
            type=OS::Window::typeDialog;
          }
          else {
            type=OS::Window::typeMain;
          }
        }

        if (!IsInited()) {
          preInitAction->Trigger();
          isInited=true;
        }

        switch (type) {
        case OS::Window::typePopup:
          assert(parent!=NULL);
          GrabMouse(true);
          if (activate) {
            GrabKeyboard(true);
          }
          break;
        default:
          break;
        }

        open=true;
        modalCount=0;

        return true;
      }

      /**
        Removes the window from the list of windows known by the
        Display.

        \note
        You must call this method before closing the window.
      */
      void Window::Close()
      {
        if (IsInEventLoop()) {
          Exit();
        }

        open=false;
        shown=false;
        onOpened=false;
        modalCount=0;
      }

      void Window::EventLoop()
      {
        assert(!inEventLoop);

        inEventLoop=true;

        if (type!=typeMenu &&
            type!=typeMenuPopup &&
            type!=typeMenuDropdown &&
            type!=typePopup) {
          DisableParents();
        }

        display->MainLoop();

        if (type!=typeMenu &&
            type!=typeMenuPopup &&
            type!=typeMenuDropdown &&
            type!=typePopup) {
          EnableParents();
        }

        inEventLoop=false;
      }

      bool Window::IsInEventLoop() const
      {
        return inEventLoop;
      }

      void Window::Exit()
      {
        display->Exit();
      }

      /**
        If you derive yourself from window and want to overload the
        defaulthandler, call the baseclass first in your handler and check the
        result, if its is TRUE the defaulthandler has allready handled it.
      */
      bool Window::HandleEvent(Event* event)
      {
        OS::Window::EventMsg msg;

        if (dynamic_cast<MouseEvent*>(event)!=NULL &&
          dynamic_cast<MouseEvent*>(event)->type==MouseEvent::down) {
          oldButton=lastButton;
          lastButton=thisButton;
          thisButton=*dynamic_cast<MouseEvent*>(event);

          oldPress=lastPress;
          lastPress=thisPress;
          thisPress.SetToNow();
        }

        msg.event=event;

        eventAction->Trigger(msg);

        return false;
      }

      /**
        Stupid helper because derived classes are no friends of Window
      */
      void Window::TriggerMaped()
      {
        assert(!shown);

        shown=true;
        mapedAction->Trigger();
      }

      /**
        Stupid helper because derived classes are no friends of Window
      */
      void Window::TriggerUnmaped()
      {
        assert(shown);

        shown=false;
        unmapedAction->Trigger();
      }

      /**
        Stupid helper because derived classes are no friends of Window
      */
      void Window::TriggerPreOpened()
      {
        preOpenedAction->Trigger();
      }

      /**
        Stupid helper because derived classes are no friends of Window
      */
      void Window::TriggerOpened()
      {
        onOpened=true;
        openedAction->Trigger();
      }

      /**
        Stupid helper because derived classes are no friends of Window
      */
      void Window::TriggerHidden()
      {
        hiddenAction->Trigger();
      }

      /**
        Stupid helper because derived classes are no friends of Window
      */
      void Window::TriggerClosed()
      {
        closedAction->Trigger();
      }

      /**
        Stupid helper because derived classes are no friends of Window
      */
      void Window::TriggerFocusIn()
      {
        focus=true;
        focusInAction->Trigger();
      }

      /**
        Stupid helper because derived classes are no friends of Window
      */
      void Window::TriggerFocusOut()
      {
        focus=false;
        focusOutAction->Trigger();
      }

      /**
        Stupid helper because derived classes are no friends of Window
      */
      void Window::TriggerMouseIn()
      {
        mouseInAction->Trigger();
      }

      /**
        Stupid helper because derived classes are no friends of Window
      */
      void Window::TriggerMouseOut()
      {
        mouseOutAction->Trigger();
      }

      /**
        See Window::Resize.
      */
      void Window::Resize(size_t width, size_t height)
      {

        this->width=width;
        this->height=height;
      }

      /**
        Opposite of Window.Disable.
      */
      void Window::Enable()
      {
        modalCount--;
      }

      /**
        Disables the window. Disallows input and make uses of some visual feedback
        (like a special mouse pointer).

        Disabling and enabling are additive, that means, if oyu call Disable
        twice you must call Enable twice, too.
      */
      void Window::Disable()
      {
        modalCount++;
      }

      bool Window::IsEnabled() const
      {
        return modalCount==0;
      }

      void Window::EnableParents()
      {
        Window* window;

        window=parent;
        while (window!=NULL) {
          window->Enable();
          window=window->parent;
        }
      }

      void Window::DisableParents()
      {
        Window* window;

        window=parent;
        while (window!=NULL) {
          window->Disable();
          window=window->parent;
        }
      }

      void Window::TriggerResize(size_t width, size_t height)
      {
        OS::Window::ResizeMsg msg;

        msg.width=width;
        msg.height=height;

        resizeAction->Trigger(msg);
      }

      void Window::TriggerGrabCancel()
      {
        grabCancelAction->Trigger();
      }

      void Window::TriggerMenu()
      {
        menuAction->Trigger();
      }

      bool Window::SendBalloonMessage(const std::wstring& /*text*/, unsigned long /*timeout*/)
      {
        return false;
      }

      /**
       * Evaluates horizontalPos and verticalPos and modifies x and y memeber variables.
       * If one of the coordinates has to be asigned with the OS-specific default position,
       * \p xManPos or \p yManPos will be set to true and the caller must not use x and/or y position
       * (but must initialize it to OS-assigned pixel values later).
       */
      void Window::CalculateWindowPos(bool& xManPos, bool& yManPos)
      {
        xManPos=false;
        yManPos=false;

        if (horizontalPos==positionOS && this->parent!=NULL) {
          horizontalPos=positionCenterOnParent;
        }
        else if (horizontalPos==positionCenterOnParent && this->parent==NULL) {
          horizontalPos=positionCenterOnScreen;
        }

        switch (horizontalPos) {
        case OS::Window::positionCenterOnParent:
          x=parent->GetX()+((int)parent->GetWidth()-(int)width) / 2;
          xManPos=true;
          break;
        case OS::Window::positionCenterOnScreen:
          x=((int)display->GetScreenWidth()-(int)width) / 2;
          xManPos=true;
          break;
        case OS::Window::positionOS:
          x=((int)display->GetScreenWidth()-(int)width) / 2;
          break;
        case OS::Window::positionManual:
          xManPos=true;
          break;
        default:
          break;
        }

        if (verticalPos==positionOS && this->parent!=NULL) {
          verticalPos=positionCenterOnParent;
        }
        else if (verticalPos==positionCenterOnParent && this->parent==NULL) {
          verticalPos=positionCenterOnScreen;
        }

        switch (verticalPos) {
        case OS::Window::positionCenterOnParent:
          y=parent->GetY()+((int)parent->GetHeight()-(int)height) / 2;
          yManPos=true;
          break;
        case OS::Window::positionCenterOnScreen:
          y=((int)display->GetScreenHeight()-(int)height) / 2;
          yManPos=true;
          break;
        case OS::Window::positionOS:
          y=((int)display->GetScreenHeight()-(int)height) / 2;
          break;
        case OS::Window::positionManual:
          yManPos=true;
          break;
        default:
          break;
        }
      }

      void Window::AcquireKeyboardInput()
      {
        // By default do nothing
      }

      void Window::ReleaseKeyboardInput()
      {
        // By default do nothing
      }

      void Window::GrabMouse(bool grab)
      {
        if (this->grabMouse==grab) {
          return;
        }
        this->grabMouse=grab;
        if (IsOpen()) {
          if (this->grabMouse) {
            //OS::display->StopContextHelp();
            MouseGrabOn();
          }
          else {
            MouseGrabOff();
            //OS::display->RestartContextHelp();
          }
        }
      }

      void Window::GrabKeyboard(bool grab)
      {
        if (this->grabKeyboard==grab) {
          return;
        }
        this->grabKeyboard=grab;
        if (IsOpen()) {
          if (this->grabKeyboard) {
            KeyboardGrabOn();
          }
          else {
            KeyboardGrabOff();
          }
        }
      }

      void Window::MouseGrabOn()
      {
        // no code
      }

      void Window::MouseGrabOff()
      {
        // no code
      }

      bool Window::IsDoubleClicked() const
      {
        Lum::Base::SystemTime n(thisPress);

        n.Sub(lastPress);

        return ((unsigned long)(n.GetTime()*1000+n.GetMicroseconds()/1000)<=display->GetMultiClickTime())
          && (lastButton.type==thisButton.type)
          && (lastButton.qualifier==thisButton.qualifier);
      }

      const OS::MouseEvent& Window::GetLastButtonClickEvent() const
      {
        return lastButton;
      }

      void Window::KeyboardGrabOn()
      {
        // no code
      }

      void Window::KeyboardGrabOff()
      {
        // no code
      }

      bool Window::SetFullScreen()
      {
        return false;
      }

      bool Window::LeaveFullScreen()
      {
        return false;
      }

      bool Window::IsFullScreen() const
      {
        return false;
      }
    }
  }
}

