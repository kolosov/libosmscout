#ifndef LUM_WINDOW_H
#define LUM_WINDOW_H

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


#include <Lum/Model/Action.h>

#include <Lum/OS/Display.h>
#include <Lum/OS/Frame.h>
#include <Lum/OS/Window.h>

#include <Lum/Object.h>

namespace Lum {

  class Window;

  /**
    Class for dialog-like windows, like main dialogs,
    sub dialogs, popups and toolbox dialogs.
  */
  class LUMAPI Window : public Base::MsgObject
  {
  private:
    enum PosMode {
      posModeAny,
      posModeMenu
    };

    /**
      Store one shortcut entry.
    */
    class Shortcut
    {
    public:
      Lum::Object      *object;    //! The object that owns the shortcodes
      unsigned long    qualifier; //! Qualifier to be matched
      std::wstring     key;       //! Character to be matched
      Model::ActionRef action;    //! The model that should be triggered

    public:
      bool Match(OS::KeyEvent* event) const;
    };

    class LUMAPI KeyHandler
    {
    public:
      OS::Window           *window;   //! The corresponding window
      Lum::Object          *top;      //! The top of the object tree
      Lum::Object          *current;  //! Current object having focus
      std::list<Shortcut*> shortcuts; //! List of shortcuts
      Shortcut             *sCurrent; //! Currently potentially active shortcode

    public:
      KeyHandler(OS::Window* window);
      ~KeyHandler();

      void SetTop(Lum::Object* top);

      void SetFocus(Lum::Object* gadget);
      Lum::Object* GetCurrentFocusObject() const;
      void SetFocusFirst();
      void SetFocusNext();
      void SetFocusPrevious();
      void SetFocusNextInGroup();
      void SetFocusPreviousInGroup();
      void Activate();
      void Deactivate();
      bool HandleEvent(OS::KeyEvent* event);

      void CancelCurrent();
      void Reset();

      void AddShortcut(Lum::Object* object,
                       unsigned long qualifier,
                       const std::wstring& key,
                       Model::Action* action);
      void RemoveShortcut(Model::Action* action);
      Shortcut* GetShortcut(OS::KeyEvent* event) const;
    };

  private:
    Lum::Object      *main;            //! The top most object

    Lum::Object      *mouseGrab;       //! Object currently holding mouse event focus
    Lum::Object      *mouseActive;     //! The object current under the mouse pointer

    OS::Window       *window;          //! The underlying OS window
    KeyHandler       keyHandler;       //! Instance of key handler
    bool             mouseGrabHunting; //! Currently investigating mouse grab
    bool             deleted;
    Model::ActionRef menuAction;
    int              mouse1DownX;
    int              mouse1DownY;

  private:
    Lum::Object* GetPosObject(PosMode mode) const;
    void      EvaluateMouseActive();
    Lum::Object* HandleMouseEvent(const OS::MouseEvent& event);

  public:
    Window();
    virtual ~Window();

    void Resync(Base::Model* model,
                const Base::ResyncMsg& msg);

    virtual OS::Window* GetWindow() const;

    virtual void SetMain(Lum::Object* object);
    virtual Lum::Object* GetMain() const;

    virtual void CalcSize() = 0;

    virtual void PreInit();
    virtual bool HandleEvent(OS::Event* event);

    virtual void ReinitWindow();

    virtual void OnContextHelp();
    virtual bool OnContextMenu();

    // Focus
    virtual void FocusFirst();
    virtual void FocusNext();
    virtual void SetFocus(Lum::Object* gadget);
    virtual Lum::Object* GetFocus() const;

    virtual void ObjectHides(Lum::Object* object);

    // Shortcuts
    virtual void RegisterShortcut(Lum::Object* object,
                                  unsigned long qualifier,
                                  const std::wstring& key,
                                  Model::Action* action);
    virtual void RegisterCommitShortcut(Lum::Object* object,
                                        Model::Action* action);
    virtual void RegisterCancelShortcut(Lum::Object* object,
                                        Model::Action* action);
    virtual void RegisterDefaultShortcut(Lum::Object* object,
                                         Model::Action* action);
    virtual void UnregisterShortcut(Model::Action* action);

    // Convinience function to access window
    virtual bool IsOpen() const;
    virtual bool IsShown() const;
    virtual bool Show(bool activate=true);
    virtual bool Hide();

    virtual void SetParent(Window* parent);
    virtual void SetParent(OS::Window* parent);
    virtual void SetType(OS::Window::Type type);
    virtual OS::Window::Type GetType() const;
    virtual void SetPosition(int x, int y);
    virtual void SetPosition(OS::Window::Position horiz, OS::Window::Position vert);
    virtual void SetTitle(const std::wstring& name);

    virtual bool Open(bool activate=true);
    virtual void Close();

    virtual void Exit();
    virtual void EventLoop();

    virtual void SetExitAction(Model::Action* action);

    virtual Model::Action* GetPreOpenedAction() const;
    virtual Model::Action* GetOpenedAction() const;
    virtual Model::Action* GetClosedAction() const;
    virtual Model::Action* GetExitAction() const;
  };
}

#endif
