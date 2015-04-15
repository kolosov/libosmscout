/*
  This source is part of the Illumination library
  Copyright (C) 2008  Tim Teulings

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

#include <Lum/Window.h>

#include <cstdlib>

#include <Lum/Base/String.h>
#include <Lum/Base/Util.h>

#include <Lum/Manager/Behaviour.h>
#include <Lum/Manager/Keyboard.h>

#include <Lum/OS/Driver.h>
#include <Lum/OS/Theme.h>

#include <Lum/Menu.h>
#include <Lum/Panel.h>

#define MENU_TIMEOUT 2

namespace Lum {
  /**
    Checks if the given shortcut entry matches the given key.
  */
  bool Window::Shortcut::Match(OS::KeyEvent* event) const
  {
    std::wstring  name;
    unsigned long reqQual;
    unsigned long evQual;

    event->GetName(name);
    Base::ToUpper(name);

    reqQual=this->qualifier;
    evQual=event->qualifier & ~OS::qualifierCapsLock;

    // If qualifierXXX is set, it matches both qualifierXXXLeft and qualifierXXXRight.
    // In this case just remove them on both sides of the match and check if the rest
    // matches, too. If we don't have a match in this case, the match belowe wil not succeed,
    // either.

    if ((reqQual & OS::qualifierShift) && (evQual & OS::qualifierShift)) {
      reqQual=reqQual & ~OS::qualifierShift;
      evQual=evQual & ~(OS::qualifierShiftLeft|OS::qualifierShiftRight|OS::qualifierShift);
    }

    if ((reqQual & OS::qualifierControl) && (evQual & OS::qualifierControl)) {
      reqQual=reqQual & ~OS::qualifierControl;
      evQual=evQual & ~(OS::qualifierControlLeft|OS::qualifierControlRight|OS::qualifierControl);
    }

    if ((reqQual & OS::qualifierAlt) && (evQual & OS::qualifierAlt)) {
      reqQual=reqQual & ~OS::qualifierAlt;
      evQual=evQual & ~(OS::qualifierAltLeft|OS::qualifierAltRight|OS::qualifierAlt);
    }

    if ((reqQual & OS::qualifierMeta) && (evQual & OS::qualifierMeta)) {
      reqQual=reqQual & ~OS::qualifierMeta;
      evQual=evQual & ~(OS::qualifierMetaLeft|OS::qualifierMetaRight|OS::qualifierMeta);
    }

    if ((reqQual & OS::qualifierSuper) && (evQual & OS::qualifierSuper)) {
      reqQual=reqQual & ~OS::qualifierSuper;
      evQual=evQual & ~(OS::qualifierSuperLeft|OS::qualifierSuperRight|OS::qualifierSuper);
    }

    if ((reqQual & OS::qualifierHyper) && (evQual & OS::qualifierHyper)) {
      reqQual=reqQual & ~OS::qualifierHyper;
      evQual=evQual & ~(OS::qualifierHyperLeft|OS::qualifierHyperRight|OS::qualifierHyper);
    }

    // TODO:
    // Check case insensitive

    return reqQual==evQual && key==name && (object->IsVisible() || object->ShortcutAlways());
  }

  Window::KeyHandler::KeyHandler(OS::Window* window)
  : window(window),
    top(NULL),current(NULL),sCurrent(NULL)
  {
    // no code
  }

  Window::KeyHandler::~KeyHandler()
  {
    for (std::list<Shortcut*>::iterator iter=shortcuts.begin();
         iter!=shortcuts.end();
         ++iter) {
      delete *iter;
    }
  }

  void Window::KeyHandler::SetTop(Lum::Object* top)
  {
    this->top=top;
  }

  void Window::KeyHandler::SetFocus(Lum::Object* object)
  {
    Lum::Object* oldCurrent=current;

    if (current==object) {
      return;
    }

    if (current!=NULL) {
      current->LostFocus();
    }

    current=object;

    if (current!=NULL) {
      current->CatchedFocus();
    }

    if (Manager::Keyboard::Instance()!=NULL) {
      Manager::Keyboard::Instance()->OnFocusChange(window,oldCurrent,object);
    }
  }

  Lum::Object* Window::KeyHandler::GetCurrentFocusObject() const
  {
    return current;
  }

  void Window::KeyHandler::SetFocusFirst()
  {
    SetFocus(top->GetFocusFirst());
  }

  void Window::KeyHandler::SetFocusNext()
  {
    Lum::Object *object;

    if (current!=NULL) {
      object=current->GetFocusNext(current);
      SetFocus(object);
      return;
    }

    object=top->GetFocusFirst();
    if (object!=current) {
      SetFocus(object);
    }
  }

  void Window::KeyHandler::SetFocusPrevious()
  {
    Lum::Object *object;

    if (current!=NULL) {
      object=current->GetFocusPrevious(current);
      if (object!=NULL) {
        SetFocus(object);
        return;
      }
    }

    object=top->GetFocusFirst();
    if (object!=NULL) {
      SetFocus(object);
    }
    else {
      SetFocus(NULL);
    }
  }

  void Window::KeyHandler::SetFocusNextInGroup()
  {
  }

  void Window::KeyHandler::SetFocusPreviousInGroup()
  {
  }

  void Window::KeyHandler::Activate()
  {
    if (current!=NULL ||
        Manager::Behaviour::Instance()->FocusObjectOnWindowFocusIn()) {
      if (current!=NULL &&
          current->HandlesKeyFocus()) {
        if (OS::display->GetKeyboardManager()!=NULL) {
          OS::display->GetKeyboardManager()->OnFocusChange(window,current,current);
        }
        current->RecatchedFocus();
      }
      else {
        SetFocusFirst();
      }
    }
  }

  void Window::KeyHandler::Deactivate()
  {
    if (current!=NULL) {
      current->LostFocus();
    }
  }

  /**
    Adds a shortcut with the given @oparam{qualifier} and character for
    @oparam{object}. An object must be visible to get notified, so shortcuts
    can be shared as long as only one object at the time is visible.

    If a shortcut gets triggered, a @otype{ShortcutMsgDesc} will be send to
    @oparam{target} or @oparam{object}, if @oparam{target} is @code{NIL}.
  */
  void Window::KeyHandler::AddShortcut(Lum::Object* object,
                                       unsigned long qualifier,
                                       const std::wstring& key,
                                       Model::Action* action)
  {
    Shortcut     *shortcut;
    std::wstring tmp;

    shortcut=new Shortcut();

    tmp=key;
    Base::ToUpper(tmp);

    shortcut->object=object;
    shortcut->qualifier=qualifier;
    shortcut->key=tmp;
    shortcut->action=action;

    shortcuts.push_back(shortcut);
  }

  void Window::KeyHandler::RemoveShortcut(Model::Action* action)
  {
    std::list<Shortcut*>::iterator iter;

    iter=shortcuts.begin();
    while (iter!=shortcuts.end()) {
      if ((*iter)->action.Get()==action) {
        iter=shortcuts.erase(iter);
      }
      else {
        ++iter;
      }
    }

    if (sCurrent!=NULL && sCurrent->action.Get()==action) {
      sCurrent->action->Cancel();
    }
    sCurrent=NULL;
  }

  /**
    Returns the shortcut matching or NIL.
  */
  Window::Shortcut* Window::KeyHandler::GetShortcut(OS::KeyEvent* event) const
  {
    std::list<Shortcut*>::const_iterator iter;

    iter=shortcuts.begin();
    while (iter!=shortcuts.end()) {
      if ((*iter)->Match(event)) {
          return (*iter);
      }
      ++iter;
    }

    return NULL;
  }

  void Window::KeyHandler::CancelCurrent()
  {
    if (sCurrent!=NULL) {
      sCurrent->action->Cancel();
    }
    sCurrent=NULL;
  }

  void Window::KeyHandler::Reset()
  {
    CancelCurrent();
    current=NULL;

    for (std::list<Shortcut*>::iterator iter=shortcuts.begin();
         iter!=shortcuts.end();
         ++iter) {
      delete *iter;
    }

    shortcuts.clear();
  }

  bool Window::KeyHandler::HandleEvent(OS::KeyEvent* event)
  {
    Shortcut *sc;

    if (current!=NULL &&
      current->HandleKeyEvent(*event)) {
      return true;
    }

    switch (event->type) {
    case OS::KeyEvent::down:
      sc=GetShortcut(event);
      if (sc!=NULL) {
        if (sc!=sCurrent) {
          CancelCurrent();
          sCurrent=sc;
          sc->action->Start();
        }
      }
      else {
        CancelCurrent();
      }
      break;
    case OS::KeyEvent::up:
      if (sCurrent!=NULL) {
        if (!sCurrent->Match(event)) {
          CancelCurrent();
        }
        else {
          Model::ActionRef localAction=sCurrent->action;

          localAction->Finish();
          sCurrent=NULL;
        }
      }
      break;
    }

    if (event->type==OS::KeyEvent::down) {
      switch (event->key) {
      case OS::keyTab:
        if (event->qualifier==0) {
          SetFocusNext();
          return true;
        }
        break;
      case OS::keyLeftTab:
        SetFocusPrevious();
        return true;
        break;
      case OS::keyLeft:
      case OS::keyUp:
        if (event->qualifier==0) {
          SetFocusPrevious();
          //SetFocusPreviousInGroup();
        }
        break;
      case OS::keyRight:
      case OS::keyDown:
        if (event->qualifier==0) {
          SetFocusNext();
          //SetFocusNextInGroup();
        }
        break;
      default:
        break;
      }
    }

    return false;
  }

  Window::Window()
  : main(NULL),
    mouseGrab(NULL),
    mouseActive(NULL),
    window(OS::driver->CreateWindow()),
    keyHandler(window),
    mouseGrabHunting(false),
    deleted(false),
    menuAction(new Model::Action()),
    mouse1DownX(0),
    mouse1DownY(0)
  {
    assert(window!=NULL);

    window->SetMaster(this);

    Observe(OS::display->GetScreenRotatedAction());

    Observe(window->GetUnmapedAction());
    Observe(window->GetFocusInAction());
    Observe(window->GetFocusOutAction());
    Observe(window->GetMouseInAction());
    Observe(window->GetMouseOutAction());
    Observe(window->GetDrawAction());
    Observe(window->GetResizeAction());
    Observe(window->GetPreInitAction());
    Observe(window->GetOpenedAction());
    Observe(window->GetEventAction());
    Observe(window->GetGrabCancelAction());
    Observe(menuAction);
  }

  Window::~Window()
  {
    deleted=true;

    if (window->IsOpen()) {
      window->Close();
    }

    delete main;
    main=NULL;
    delete window;
    window=NULL;
  }

  Lum::Object* Window::GetMain() const
  {
    return main;
  }


  OS::Window* Window::GetWindow() const
  {
    return window;
  }

  /**
    Returns the object under the cursor to supports the given mode.
  */
  Lum::Object* Window::GetPosObject(PosMode mode) const
  {
    class GetPosVisitor : public Visitor
    {
    public:
      int         x,y;
      PosMode     mode;
      Lum::Object *object;

    public:
      bool Visit(Lum::Object* object)
      {
        if (object->IsVisible() && object->PointIsIn(x,y)) {
          if (object->VisitChildren(*this,true)) {
            switch (mode) {
            case posModeMenu:
              if (this->object==NULL && object->GetMenu()!=NULL) {
                this->object=object;
                return false;
              }
              break;
            case posModeAny:
              if (this->object==NULL) {
                this->object=object;
                return false;
              }
              break;
            }
          }
          else {
            return false;
          }
        }

        return true;
      }
    };

    int           x,y;
    GetPosVisitor visitor;

    window->GetMousePos(x,y);

    visitor.x=x;
    visitor.y=y;
    visitor.mode=mode;
    visitor.object=NULL;

    /* ignore */ visitor.Visit(main);

    return visitor.object;
  }

  void Window::EvaluateMouseActive()
  {
    if (main==NULL ||
        !IsOpen() ||
        !IsShown()) {
      mouseActive=NULL;
      return;
    }

    Lum::Object *newActive;

    if (!window->HasFocus()) {
      newActive=NULL;
    }
    else if (mouseGrab!=NULL) {
      newActive=mouseGrab;
    }
    else {
      newActive=GetPosObject(posModeAny);
    }

    if (mouseActive!=newActive) {
      if (mouseActive!=NULL) {
        mouseActive->MouseInactive();
      }

      mouseActive=newActive;

      if (mouseActive!=NULL) {
        mouseActive->MouseActive();
      }
    }
  }

  /**
    Set the top object for this window.
  */
  void Window::SetMain(Lum::Object* object)
  {
    delete main;
    main=object;
    keyHandler.SetTop(main);
  }

  /**
     Creates a window on the display with top as top object
     and with title as title.
  */
  void Window::PreInit()
  {
    // no code
  }

  void Window::ReinitWindow()
  {
    mouseActive=NULL;
    keyHandler.Reset();

    delete main;
    main=NULL;

    PreInit();
    CalcSize();

    if (window->GetWidth()>=main->GetOMinWidth() &&
        window->GetWidth()<=main->GetOMaxWidth() &&
        window->GetHeight()>=main->GetOMinHeight() &&
        window->GetHeight()<=main->GetOMaxHeight()) {
      // Current window size does fit size of new main
      if (main->GetOWidth()!=window->GetWidth() ||
          main->GetOHeight()!=window->GetHeight()) {
        // we need to resize mai, to fit into the current window
        window->TriggerResize(window->GetWidth(),window->GetHeight());
        window->TriggerDraw(0,0,window->GetWidth(),window->GetHeight());
      }
      else if (IsShown() || IsOpen()) {
        // we just excahnged the window contents, no need to resize the window
        window->TriggerDraw(0,0,window->GetWidth(),window->GetHeight());
      }
    }
    else {
      // we need to resize the window to hold the new main, resize it to
      // the new main default size
      window->Resize(main->GetOWidth(),main->GetOHeight());
      window->TriggerDraw(0,0,window->GetWidth(),window->GetHeight());
    }

    window->SetMinMaxSize(main->GetOMinWidth(),
                          main->GetOMinHeight(),
                          main->GetOMaxWidth(),
                          main->GetOMaxHeight());
  }

  void Window::OnContextHelp()
  {
    // TODO
  }

  /**
    This method gets called, when the window things you should
    open a context sensitiv menu.

    RESULT
    Return TRUE if you have opened a context menu, else FALSE.

    NOTE
    If this method returns FALSE, the display will propagate
    the corresponding event that started contextsensitive help
    to the window.
  */
  bool Window::OnContextMenu()
  {
    Lum::Object *object;

    object=GetPosObject(posModeMenu);
    if (object!=NULL) {
      OS::Window* window;
      Menu*       menu=NULL;

      window=object->GetMenu();

      if (window!=NULL) {
        menu=dynamic_cast<Menu*>(window->GetMaster());
      }

      if (menu!=NULL) {
        object->PrepareMenu();
        /* Ignore */ menu->OpenInStickyMode();

        return true;
      }
      else if (window!=NULL) {
        object->PrepareMenu();
        /* ignore */ window->Open(true);

        return true;
      }
    }

    return false;
  }

  void Window::FocusFirst()
  {
    if (deleted) {
      // Do reassign focus if our destructor has ben called
      return;
    }

    keyHandler.SetFocusFirst();
  }

  void Window::FocusNext()
  {
    if (deleted) {
      // Do reassign focus if our destructor has ben called
      return;
    }

    keyHandler.SetFocusNext();
  }

  void Window::SetFocus(Lum::Object* gadget)
  {
    if (deleted) {
      // Do reassign focus if our destructor has ben called
      return;
    }

    keyHandler.SetFocus(gadget);
  }

  Lum::Object* Window::GetFocus() const
  {
    return keyHandler.current;
  }

  void Window::ObjectHides(Lum::Object* object)
  {
    if (main==NULL) {
      return;
    }

    if (object==mouseActive) {
      mouseActive=NULL;
    }

    if (object==keyHandler.current && window->IsShown()) {
      FocusNext();

      if (object==keyHandler.current) {
        keyHandler.current=NULL;
      }
    }
  }

  /**
    The defaulthandler ask all members of the layout object for the focus.
  */
  Lum::Object* Window::HandleMouseEvent(const OS::MouseEvent& event)
  {
    class MouseEventVisitor : public Visitor
    {
    public:
      const OS::MouseEvent& event;
      Lum::Object           *grab;

    public:
      MouseEventVisitor(const OS::MouseEvent& event)
      : event(event)
      {
        // no code
      }

      bool Visit(Lum::Object* object)
      {
        if (object->InterceptMouseEvents() &&
            object->HandleMouseEvent(event)) {
          grab=object;
        }

        object->VisitChildren(*this,true);

        if (grab!=NULL) {
          return false;
        }

        if (!object->InterceptMouseEvents() &&
            object->HandleMouseEvent(event)) {
          grab=object;
        }

        return grab==NULL;
      }
    };

    MouseEventVisitor visitor(event);

    visitor.grab=NULL;

    if (main!=NULL &&
        main->IsVisible()) {
      visitor.Visit(main);
    }

    return visitor.grab;
  }

  bool Window::HandleEvent(OS::Event* event)
  {
    OS::MouseEvent *mouseEvent;
    OS::KeyEvent   *keyEvent;

    /*
      now delegate the event to the current or if no current object
      exists, to the top most object
    */
    if ((mouseEvent=dynamic_cast<OS::MouseEvent*>(event))!=NULL) {
      /*
        Mouse grabbing and/or synchronous subdialogs may lead to mouse
        and key event occuring after window has been hidden!
      */
      if (!IsOpen() ||
          !IsShown()) {
        return false;
      }

      if (mouseEvent->type==OS::MouseEvent::down &&
          mouseEvent->button==Lum::OS::MouseEvent::button3) {
        if (OnContextMenu()) {
          // menu mouse button up might have been lost, we kill the current mouse grab
          // to be on the save side.
          mouseGrab=NULL;
        }
      }
      else if (mouseEvent->type==OS::MouseEvent::down &&
               mouseEvent->button==Lum::OS::MouseEvent::button1) {
        Lum::Object *object;

        object=GetPosObject(posModeMenu);
        if (object!=NULL && object->GetMenu()!=NULL) {
          mouse1DownX=mouseEvent->x;
          mouse1DownY=mouseEvent->y;
          OS::display->AddTimer(MENU_TIMEOUT,0,menuAction);
        }
      }
      else if (mouseEvent->type==OS::MouseEvent::up &&
               mouseEvent->button==Lum::OS::MouseEvent::button1 &&
               mouseEvent->qualifier==Lum::OS::MouseEvent::button1) {
        OS::display->RemoveTimer(menuAction);
      }
      else if (mouseEvent->type==OS::MouseEvent::move &&
               mouseEvent->qualifier & Lum::OS::MouseEvent::button1 &&
               (std::abs(mouseEvent->x-mouse1DownX)>(int)OS::display->GetTheme()->GetMouseClickHoldSensitivity() ||
                std::abs(mouseEvent->y-mouse1DownY)>(int)OS::display->GetTheme()->GetMouseClickHoldSensitivity())) {
        OS::display->RemoveTimer(menuAction);
      }

      if (mouseGrab!=NULL) {
        mouseEvent->SetGrabed(true);
        mouseGrab->HandleMouseEvent(*mouseEvent);

        if (mouseEvent->IsGrabEnd()) {
          if (mouseEvent->type==OS::MouseEvent::up &&
              mouseEvent->button==Lum::OS::MouseEvent::button1 &&
              mouseEvent->qualifier==Lum::OS::MouseEvent::button1 &&
              OS::display->GetKeyboardManager()!=NULL) {
            OS::display->GetKeyboardManager()->OnMouseClick(window,
                                                            mouseGrab,
                                                            mouseEvent);
          }

          mouseGrab=NULL;
        }
      }
      else if (mouseEvent->IsGrabStart()) {
        mouseGrabHunting=true;
        mouseEvent->SetGrabed(false);
        mouseGrab=HandleMouseEvent(*mouseEvent);

        if (mouseGrab!=NULL) {
          if (!mouseGrabHunting) {
            mouseGrab=NULL;
          }
        }

        mouseGrabHunting=false;

        if (mouseGrab!=NULL) {
          Lum::Object* focusObject=mouseGrab->GetMouseGrabFocusObject();

          if (focusObject!=NULL &&
              focusObject->HandlesClickFocus()) {
            SetFocus(focusObject);
          }
        }
      }
      else if (mouseEvent->type==OS::MouseEvent::move) {
        // We need that (currently) to get menu strip selection working
        // TODO: Try to activate this only if a OS mouse grab is active!?
        mouseEvent->SetGrabed(false);
        HandleMouseEvent(*mouseEvent);
      }

      EvaluateMouseActive();
    }
    else if ((keyEvent=dynamic_cast<OS::KeyEvent*>(event))!=NULL) {
      /*
        Mouse grabbing and/or synchronous subdialogs may lead to mouse
        and key event occuring after window has been hidden!
      */
      if (!IsShown()) {
        return false;
      }

      if (keyEvent->type==OS::KeyEvent::up &&
          keyEvent->qualifier & OS::qualifierControl &&
          keyEvent->key==38) {
        ReinitWindow();
      }

      keyHandler.HandleEvent(keyEvent);
    }

    // Current object that grabed the mouse is not visible anymore => kill the grab
    if (mouseGrab!=NULL &&
        !mouseGrab->IsVisible()) {
      mouseGrab=NULL;
    }

    return true;
  }

  void Window::RegisterShortcut(Lum::Object* object,
                                unsigned long qualifier,
                                const std::wstring& key,
                                Model::Action* action)
  {
    keyHandler.AddShortcut(object,qualifier,key,action);
  }

  void Window::RegisterCommitShortcut(Lum::Object* object,
                                      Model::Action* action)
  {
    keyHandler.AddShortcut(object,0,L"Return",action);
  }

  void Window::RegisterCancelShortcut(Lum::Object* object,
                                      Model::Action* action)
  {
    keyHandler.AddShortcut(object,0,L"Escape",action);
  }

  void Window::RegisterDefaultShortcut(Lum::Object* object,
                                       Model::Action* action)
  {
    keyHandler.AddShortcut(object,0,L"Return",action);
    keyHandler.AddShortcut(object,0,L"Escape",action);
  }

  void Window::UnregisterShortcut(Model::Action* action)
  {
    keyHandler.RemoveShortcut(action);
  }

  void Window::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==window->GetUnmapedAction() &&
        window->GetUnmapedAction()->IsFinished()) {
      OS::display->RemoveTimer(menuAction);

      if (mouseActive!=NULL) {
        mouseActive->MouseInactive();
        mouseActive=NULL;
      }

      mouseGrab=NULL;
      main->Hide();
    }
    else if (model==window->GetFocusInAction() &&
             window->GetFocusInAction()->IsFinished()) {
      mouseGrab=NULL;
      keyHandler.Activate();
      EvaluateMouseActive();
    }
    else if (model==window->GetFocusOutAction() &&
             window->GetFocusOutAction()->IsFinished()) {
      OS::display->RemoveTimer(menuAction);

      mouseGrab=NULL;
      mouseGrabHunting=false;
      keyHandler.Deactivate();
      EvaluateMouseActive();
    }
    else if (model==window->GetMouseInAction() &&
             window->GetMouseInAction()->IsFinished()) {
      EvaluateMouseActive();
    }
    else if (model==window->GetMouseOutAction() &&
             window->GetMouseOutAction()->IsFinished()) {
      OS::display->RemoveTimer(menuAction);

      if (mouseActive!=NULL) {
        mouseActive->MouseInactive();
        mouseActive=NULL;
      }
    }
    else if (model==window->GetResizeAction() &&
             window->GetResizeAction()->IsFinished()) {
      const OS::Window::ResizeMsg *resizeMsg=dynamic_cast<const OS::Window::ResizeMsg*>(&msg);

      main->Resize(resizeMsg->width,resizeMsg->height);
      EvaluateMouseActive();
    }
    else if (model==window->GetDrawAction() &&
             window->GetDrawAction()->IsFinished()) {
      const OS::Window::DrawMsg *drawMsg=dynamic_cast<const OS::Window::DrawMsg*>(&msg);
      assert(drawMsg->draw!=NULL);

      main->Move(0,0);
      main->Layout();
      main->DrawBackground(drawMsg->draw,
                           drawMsg->x,
                           drawMsg->y,
                           drawMsg->width,
                           drawMsg->height);
      main->Draw(drawMsg->draw,
                 drawMsg->x,
                 drawMsg->y,
                 drawMsg->width,
                 drawMsg->height);
    }
    else if (model==window->GetPreInitAction() &&
             window->GetPreInitAction()->IsFinished()) {
      PreInit();
      CalcSize();
    }
    else if (model==window->GetOpenedAction() &&
             window->GetOpenedAction()->IsFinished()) {
      EvaluateMouseActive();
    }
    else if (model==window->GetEventAction() &&
             window->GetEventAction()->IsFinished()) {
      const OS::Window::EventMsg *eventMsg=dynamic_cast<const OS::Window::EventMsg*>(&msg);
      HandleEvent(eventMsg->event);
    }
    else if (model==menuAction &&
             menuAction->IsFinished()) {
      if (OnContextMenu()) {
        // menu mouse button up might have been lost, we kill the current mouse grab
        // to be on the save side.
        mouseGrab=NULL;
      }
    }
    else if (model==window->GetGrabCancelAction() &&
             window->GetGrabCancelAction()->IsFinished()) {
      // A child popup opened while we were handling mouse button grab events.
      OS::display->RemoveTimer(menuAction);

      mouseGrab=NULL;
      mouseGrabHunting=false;
      if (mouseActive!=NULL) {
        mouseActive->MouseInactive();
        mouseActive=NULL;
      }
    }
    else if (model==OS::display->GetScreenRotatedAction() &&
             OS::display->GetScreenRotatedAction()->IsFinished()) {
      if (IsOpen() &&
          IsShown()) {
        if (GetWindow()->GetScreenOrientationHint()==OS::Window::screenOrientationBothSupported) {
          ReinitWindow();
        }
      }
    }
  }

  bool Window::IsOpen() const
  {
    return window->IsOpen();
  }

  bool Window::IsShown() const
  {
    return window->IsShown();
  }

  bool Window::Show(bool activate)
  {
    return window->Show(activate);
  }

  bool Window::Hide()
  {
    return window->Hide();
  }

  void Window::SetParent(Window* parent)
  {
    if (parent!=NULL) {
      window->SetParent(parent->GetWindow());
    }
    else {
      window->SetParent(NULL);
    }
  }

  void Window::SetParent(OS::Window* parent)
  {
    window->SetParent(parent);
  }

  void Window::SetType(OS::Window::Type type)
  {
    window->SetType(type);
  }

  OS::Window::Type Window::GetType() const
  {
    return window->GetType();
  }

  void Window::SetPosition(int x, int y)
  {
    window->SetPos(x,y);
    SetPosition(OS::Window::positionManual,OS::Window::positionManual);
  }

  void Window::SetPosition(OS::Window::Position horiz, OS::Window::Position vert)
  {
    window->SetPosition(horiz,vert);
  }

  void Window::SetTitle(const std::wstring& name)
  {
    window->SetTitle(name);
  }

  bool Window::Open(bool activate)
  {
    return window->Open(activate);
  }

  void Window::Close()
  {
    mouseGrab=NULL;
    keyHandler.current=NULL;

    window->Close();
  }

  void Window::Exit()
  {
    window->Exit();
  }

  void Window::EventLoop()
  {
    window->EventLoop();
  }

  void Window::SetExitAction(::Lum::Model::Action* action)
  {
    window->SetExitAction(action);
  }

  Model::Action* Window::GetPreOpenedAction() const
  {
    return window->GetPreOpenedAction();
  }

  Model::Action* Window::GetOpenedAction() const
  {
    return window->GetOpenedAction();
  }

  Model::Action* Window::GetClosedAction() const
  {
    return window->GetClosedAction();
  }

  Model::Action* Window::GetExitAction() const
  {
    return window->GetExitAction();
  }
}

