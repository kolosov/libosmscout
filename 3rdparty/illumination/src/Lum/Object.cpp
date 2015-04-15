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

#include <Lum/Object.h>

#include <algorithm>

#include <Lum/Base/Util.h>

#include <Lum/Dialog.h>

namespace Lum {

  Visitor::~Visitor()
  {
    // no code
  }

  /**
    If you visit a list of objects, this method will be called for
    every object visited.

    Returning true means "go on", returning false means,
    "stop processing".
  */
  bool Visitor::Visit(Object* /*object*/)
  {
    return true;
  }

  Controller::~Controller()
  {
    // no code
  }

  /**
    Register an Action for a key-shortcut.
  */
  void Controller::RegisterKeyAction(const std::wstring& key, Action action)
  {
    keys[key]=action;
  }

  /**
    Return the action for a certain key combination or Controller::actionNone
    if no action for this key combination can be found.
  */
  Action Controller::GetActionForKey(const std::wstring& key)
  {
    std::map<std::wstring,Action>::iterator iter;

    iter=keys.find(key);

    if (iter!=keys.end()) {
      return iter->second;
    }
    else {
      return actionNone;
    }
  }

  /**
    Execute action on object.
  */
  bool Controller::DoAction(Lum::Object* /*object*/, Action /*action*/)
  {
    return false;
  }

  IMHandler::~IMHandler()
  {
    // no code
  }

  bool IMHandler::IsInEditingArea(OS::MouseEvent* event) const
  {
    return true;
  }


  const wchar_t* Object::inputTypeAlpha                     = L"Alpha";
  const wchar_t* Object::inputTypeUnsignedBinaryNumber      = L"UnsignedBinaryNumber";
  const wchar_t* Object::inputTypeUnsignedOctalNumber       = L"UnsignedOctalNumber";
  const wchar_t* Object::inputTypeUnsignedHexadecimalNumber = L"UnsignedHexadecimalNumber";
  const wchar_t* Object::inputTypeSignedDecimalNumber       = L"SignedDecimalNumber";
  const wchar_t* Object::inputTypeUnsignedDecimalNumber     = L"UnsignedDecimalNumber";
  const wchar_t* Object::inputTypeSignedFloatNumber         = L"SsignedFloatNumber";
  const wchar_t* Object::inputTypeUnsignedFloatNumber       = L"UnsignedFloatNumber";
  const wchar_t* Object::inputTypePhoneNumber               = L"PhoneNumber";

  Object::Object()
  : parent(NULL),
    window(NULL),
    x(0),y(0),width(0),height(0),
    minWidth(0),minHeight(0),
    maxWidth(32000),maxHeight(32000),
    oX(0),oY(0),oWidth(0),oHeight(0),
    oMinWidth(0),oMinHeight(0),
    oMaxWidth(0),oMaxHeight(0),
    horizontalFlex(false),verticalFlex(false),
    visible(false),
    firstDraw(true),
    layouted(false),
    clipFromParent(false),
    clipBackgroundToBorder(false),
    hasFocus(false),
    canFocus(false),
    showFocus(false),
    requestsFocus(false),
    requestsLabelFocus(false),
    requestsKeyboard(false),
    mouseActive(false),
    redrawOnMouseActive(false),
    inited(false),
    shortcutAlways(false),
    menu(NULL),
    help(NULL),
    label(NULL),
    labeling(NULL),
    controller(NULL)
  {
    // no code
  }

  /**
    You cannot copy objects.
  */
  Object::Object(const Object& object)
  {
    assert(false);
  }

  Object::~Object()
  {
    // Tell the window, that this object hides
    // Dialog may delgate focus or other things
    if (visible &&
        GetWindow()!=NULL &&
        GetWindow()->GetMaster()!=NULL &&
        dynamic_cast<Window*>(GetWindow()->GetMaster())!=NULL) {
      dynamic_cast<Window*>(GetWindow()->GetMaster())->ObjectHides(this);
    }

    visible=false;

    // TODO: Move it to Display, DnDObject
    if (OS::display->GetSelectionOwner()==this) {
      OS::display->CancelSelection();
    }

    // Clearing label linkage
    if (label!=NULL) {
      label->labeling=NULL;
    }

    if (labeling!=NULL) {
      labeling->label=NULL;
    }

    label=NULL;
    labeling=NULL;

    if (menu!=NULL) {
      if (menu->GetMaster()!=NULL) {
        delete menu->GetMaster();
      }
      else {
        delete menu;
      }
    }
    menu=NULL;
  }

  /**
    Assign a Controller to the object.
  */
  void Object::SetController(Controller* controller)
  {
    this->controller=controller;
  }

  /**
    Return (inner) x position of the object excluding the bounds of the outer frame.
  */
  int Object::GetX() const
  {
    return x;
  }

  /**
    Return (inner) y position of the object excluding the bounds of the outer frame.
  */
  int Object::GetY() const
  {
    return y;
  }

  /**
    Return (inner) width of the object excluding the bounds of the outer frame.
  */
  size_t Object::GetWidth() const
  {
    return width;
  }

  /**
    Return (inner) height of the object excluding the bounds of the outer frame.
  */
  size_t Object::GetHeight() const
  {
    return height;
  }

  /**
    Return (outer) x position of the object including the bounds of the outer frame.
  */
  int Object::GetOX() const
  {
    return oX;
  }

  /**
    Return (outer) y position of the object including the bounds of the outer frame.
  */
  int Object::GetOY() const
  {
    return oY;
  }

  /**
    Return (outer) width of the object including the bounds of the outer frame.
  */
  size_t Object::GetOWidth() const
  {
    return oWidth;
  }

  /**
    Return (outer) height of the object including the bounds of the outer frame.
  */
  size_t Object::GetOHeight() const
  {
    return oHeight;
  }

  size_t Object::GetOMinWidth() const
  {
    return oMinWidth;
  }

  size_t Object::GetOMinHeight() const
  {
    return oMinHeight;
  }

  size_t Object::GetOMaxWidth() const
  {
    return oMaxWidth;
  }

  size_t Object::GetOMaxHeight() const
  {
    return oMaxHeight;
  }

  size_t Object::GetTopBorder() const
  {
    if (background.Valid()) {
      return background->topBorder;
    }
    else {
      return 0;
    }
  }

  size_t Object::GetBottomBorder() const
  {
    if (background.Valid()) {
      return background->bottomBorder;
    }
    else {
      return 0;
    }
  }

  size_t Object::GetLeftBorder() const
  {
    if (background.Valid()) {
      return background->leftBorder;
    }
    else {
      return 0;
    }
  }

  size_t Object::GetRightBorder() const
  {
    if (background.Valid()) {
      return background->rightBorder;
    }
    else {
      return 0;
    }
  }

  /**
    Returns true if the object has a baseline, else false.
    Objects normally have a baseline if they have textual
    content. The baseline information is interesting if
    you want to align objects horizontally based on the
    position of their internal textual baseline. If you do so,
    you support the human eye in reading textual content
    in a row.

    Returns false in the default implementation.
  */
  bool Object::HasBaseline() const
  {
    return false;
  }

  /**
    Return the position of the textual baseline relative to the
    upper position of the control. This value is always positive
    because the baseline must always be below the upper edge of the
    control.

    Since this method is calledbefore calcSize() is called,
    you cannot rely on stuff like y-oY. You must calculate
    to position of text relative to the outer bounds of the control
    yourself, taking into account a background with border
    manually.

    By default returns 0.
  */
  size_t Object::GetBaseline() const
  {
    assert(inited);

    return 0;
  }

  /**
    Set a size constraint for the object.
   */
  Object* Object::SetConstraint(Constraint constraint,
                                Base::Size::Mode mode,
                                size_t value,
                                const OS::FontRef& font)
  {
    constraints.push_back(ConstraintSize(constraint,Base::Size(mode,value,font)));

    return this;
  }

  Object* Object::SetConstraint(Constraint constraint,
                                const Base::Size& size)
  {
    constraints.push_back(ConstraintSize(constraint,size));

    return this;
  }

  /**
    Set the width of the object.
    "Mode" defines the interpretation of the given value.
  */
  Object* Object::SetWidth(Base::Size::Mode mode,
                           size_t value,
                           const OS::FontRef& font)
  {
    SetConstraint(constraintWidth,mode,value,font);

    return this;
  }

  /**
    Set the height of the object.
    "Mode" defines the interpretation of the given value.
  */
  Object* Object::SetHeight(Base::Size::Mode mode,
                            size_t value,
                            const OS::FontRef& font)
  {
    SetConstraint(constraintHeight,mode,value,font);

    return this;
  }

  /**
    Set the minimal width of the object.
    "Mode" defines the interpretation of the given value.
  */
  Object* Object::SetMinWidth(Base::Size::Mode mode,
                              size_t value,
                              const OS::FontRef& font)
  {
    SetConstraint(constraintMinWidth,mode,value,font);

    return this;
  }

  /**
    Set the minimal height of the object.
    "Mode" defines the interpretation of the given value.
  */
  Object* Object::SetMinHeight(Base::Size::Mode mode,
                               size_t value,
                               const OS::FontRef& font)
  {
    SetConstraint(constraintMinHeight,mode,value,font);

    return this;
  }

  /**
    Set the maximum width of the object.
    "Mode" defines the interpretation of the given value.
  */
  Object* Object::SetMaxWidth(Base::Size::Mode mode,
                              size_t value,
                              const OS::FontRef& font)
  {
    SetConstraint(constraintMaxWidth,mode,value,font);

    return this;
  }

  /**
    Set the maximum height of the object.
    "Mode" defines the interpretation of the given value.
  */
  Object* Object::SetMaxHeight(Base::Size::Mode mode,
                               size_t value,
                               const OS::FontRef& font)
  {
    SetConstraint(constraintMaxHeight,mode,value,font);

    return this;
  }

  Object* Object::SetMinWidth(const Base::Size& size)
  {
    return SetConstraint(constraintMinWidth,size);
  }

  Object* Object::SetMinHeight(const Base::Size& size)
  {
    return SetConstraint(constraintMinHeight,size);
  }

  Object* Object::SetWidth(const Base::Size& size)
  {
    return SetConstraint(constraintWidth,size);
  }

  Object* Object::SetHeight(const Base::Size& size)
  {
    return SetConstraint(constraintHeight,size);
  }

  Object* Object::SetMaxWidth(const Base::Size& size)
  {
    return SetConstraint(constraintMaxWidth,size);
  }

  Object* Object::SetMaxHeight(const Base::Size& size)
  {
    return SetConstraint(constraintMaxHeight,size);
  }


  Object* Object::SetFlex(bool horizontal, bool vertical)
  {
    horizontalFlex=horizontal;
    verticalFlex=vertical;

    return this;
  }

  bool Object::HorizontalFlex() const
  {
    return horizontalFlex;
  }

  bool Object::VerticalFlex() const
  {
    return verticalFlex;
  }

  /**
    Assign a label to the object. Some object may support some
    tricky operations with their label. A checkbox f.e. may delegate its
    focus frame to its label.
  */
  void Object::SetLabelObject(Object* label)
  {
    if (this->label!=NULL) {
      this->label->labeling=NULL;
    }

    this->label=label;

    if (this->label!=NULL) {
      this->label->labeling=this;
    }
  }

  bool Object::HasImplicitLabel() const
  {
    return false;
  }

  void Object::SetWindow(OS::Window* window)
  {
    class SetWindowVisitor : public Visitor
    {
    public:
      OS::Window *window;

      bool Visit(Object* object)
      {
        object->window=window;
        if (!object->VisitChildren(*this,false)) {
          // That should always work, nobody is allowed to deny this visitor!
          assert(false);
        }

        return true;
      }
    };

    SetWindowVisitor visitor;

    visitor.window=window;

    visitor.Visit(this);
  }

  /**
    \return Returns the OS::Window this Object belongs to. A valid window is available
    and will be returned after the call to CalcSize(), else NULL is returned.
  */
  OS::Window* Object::GetWindow() const
  {
    if (window!=NULL) {
      return window;
    }
    else if (parent!=NULL) {
      const Object* o=this;

      while (o->parent!=NULL) {
        o=o->parent;
      }

      return o->GetWindow();
    }
    else {
      return NULL;
    }
  }

  /**
    Sets the parent object of this object.

    \note
    Normaly the objects tries to get its window asking its parent. So be
    carefull to set a parent object for each object you instantiate.
    Most time this is automatically handled by the container object.

    Since top level object cannot have a parent, you must explicitely set
    the window calling SetWindow().
  */
  void Object::SetParent(Object* parent)
  {
    assert(this->parent==NULL || this->parent==parent);

    this->parent=parent;
  }

  /**
    Return the text color to be used for text drawing in this control and in children
    */
  OS::Color Object::GetTextColor(OS::DrawInfo* draw,
                                 const void* child) const
  {
    if (parent!=NULL) {
      return parent->GetTextColor(draw, this);
    }
    else if (draw->disabled) {
      return OS::display->GetColor(OS::Display::textDisabledColor);
    }
    else if (draw->selected) {
      return OS::display->GetColor(OS::Display::textSelectColor);
    }
    else {
      return OS::display->GetColor(OS::Display::textColor);
    }
  }

  /**
    Return the font to be used for text drawing in this control and in children
    */
  OS::Font* Object::GetFont(const void* child) const
  {
    if (parent!=NULL) {
      return parent->GetFont(this);
    }
    else {
      return OS::display->GetFont();
    }
  }

  /**
    Return the font style to be used for text drawing in this control and in children
    */
  unsigned long Object::GetFontStyle(const void* child) const
  {
    if (parent!=NULL) {
      return parent->GetFontStyle(this);
    }
    else {
      return OS::Font::normal;
    }
  }

  /**
    If true, text in this control and for children should be drawn "smart",
    that means in pseudo 3D look.
    */
  bool Object::GetFontStyleSmart(const void* child) const
  {
    if (parent!=NULL) {
      return parent->GetFontStyleSmart(this);
    }
    else {
      return false;
    }
  }

  /**
    Assign a OS::Window that will displayed as tooltip. The base class will
    store the object within a private variable and will return it when
    Object::GetHelp gets called.
   */
  void Object::SetHelp(OS::Window* help)
  {
    delete this->help;
    this->help=help;
  }

  /**
    \return Returns the help window for this object.
   */
  OS::Window* Object::GetHelp() const
  {
    return help;
  }

  /**
    Assign a OS::Window that will displayed as menu. The base class will
    store  the object within a private variable and will return it when
    Object::GetMenu gets called.
  */
  void Object::SetMenu(OS::Window* menu)
  {
    if (this->menu!=NULL) {
      if (this->menu->GetMaster()!=NULL) {
        delete this->menu->GetMaster();
      }
      else {
        delete this->menu;
      }
    }

    this->menu=menu;
  }

  /**
    \return Returns the menu window for this object.
  */
  OS::Window* Object::GetMenu() const
  {
    return menu;
  }

  /**
    By implementing this method in the derived class you can
    arrange some preparation like correctly settings models
    referenced in a popup menu, before the menu gets opened.
   */
  void Object::PrepareMenu()
  {
    // No code - the default implementation does nothing
  }

  /**
    Returns true, if the object does not have the focus but still should show the focus
    */
  bool Object::ShowFocus() const
  {
    return showFocus;
  }

  /**
    \return Returns true, when the object has the focus.
  */
  bool Object::HasFocus() const
  {
    return hasFocus;
  }

  /**
    \return Returns true, when the object can handle keyboard focus events.
  */
  bool Object::CanFocus() const
  {
    return canFocus;
  }

  /**
    \return Returns true, if the object supports focus handling and is visible.
  */
  bool Object::HandlesClickFocus() const
  {
    return CanFocus() && IsVisible();
  }

  /**
    \return Returns true, when the object requests focus handling as part of a
    focus chain and is visible.
  */
  bool Object::HandlesKeyFocus() const
  {
    return RequestedFocus() && IsVisible();
  }

  void Object::SetCanFocus(bool can)
  {
    canFocus=can;
  }

  void Object::SetShowFocus(bool show)
  {
    if (showFocus!=show) {
      showFocus=show;
      Redraw();
    }
  }

  /**
    Marks the object as supporting and requesting the keyboard focus.
  This object will be included in the focus chain.
  */
  void Object::RequestFocus()
  {
    assert(canFocus);

    requestsFocus=true;
  }

  /**
    Negates Object::RequestFocus.
  */
  void Object::UnrequestFocus()
  {
    requestsFocus=false;
  }

  /**
    \return Returns true, if the object actually has requested to handle focus.
  */
  bool Object::RequestedFocus() const
  {
    return requestsFocus;
  }

  void Object::SetRequestLabelFocus(bool request)
  {
    requestsLabelFocus=request;
  }

  bool Object::RequestsLabelFocus() const
  {
    return requestsLabelFocus;
  }

  /**
    Return the object that should be focused, if the current object
    requests a mouse event grab. The default implementation returns
    the object itself.
   */
  Lum::Object* Object::GetMouseGrabFocusObject() const
  {
    // Hmm, I do not like this, but making the method non-const is also strange...
    return const_cast<Lum::Object*>(this);
  }

  /**
    Controls should call this method within their constructor, if they
    request a keyboard for input.
    Application can later on unset this flag if they don't want the control
    to request a keyboard - for example, if they offer ways for text input
    (for example an explicit keyboard like in calculators).
  */
  void Object::SetRequestsKeyboard(bool request)
  {
    requestsKeyboard=request;
  }

  /**
    \return Returns true, when the object requests keyboard input when it
    gets the focus.

    The purpose of the flag is to trigger appearance of an on-screen keyboard
    (if no natural keyboard if available) if certain controls get the
    keyboard focus.

    As such, controls should only set this flag, if they require keyboard
    input for their work. If keyboard input is optional and can be worked
    around by mouse input, this flag should not be set.
  */
  bool Object::RequestsKeyboard() const
  {
    return requestsKeyboard;
  }

  Object* Object::SetInputTypeHint(const std::wstring& inputTypeHint)
  {
    this->inputTypeHint=inputTypeHint;

    return this;
  }

  std::wstring Object::GetInputTypeHint() const
  {
    return inputTypeHint;
  }

  IMHandler* Object::GetIMHandler()
  {
    return NULL;
  }

  class FocusFirstVisitor : public Visitor
  {
  public:
    Object *object;

  public:
    virtual bool Visit(Object* object)
    {
      if (object->HandlesKeyFocus()) {
        this->object=object;
        return false;
      }
      else {
        return object->VisitChildren(*this,true);
      }
    }
  };

  class FocusLastVisitor : public Visitor
  {
  public:
    Object *object;

  public:
    virtual bool Visit(Object* object)
    {
      Object *tmp;

      tmp=this->object;
      this->object=NULL;

      object->VisitChildren(*this,true);

      if (this->object==NULL) {
        if (object->HandlesKeyFocus()) {
          this->object=object;
        }
      }

      if (this->object==NULL) {
        this->object=tmp;
      }

      return true;
    }
  };

  class FocusAfterVisitor : public Visitor
  {
  public:
    bool   start;
    Object *base;
    Object *object;

  public:
    virtual bool Visit(Object* object)
    {
      if (start) {
        this->object=object->GetFocusFirst();
        return this->object==NULL;
      }
      else {
        if (object==base) {
          start=true;
        }

        return true;
      }
    }
  };

  class FocusBeforeVisitor : public Visitor
  {
  public:
    bool   start;
    Object *base;
    Object *object;

  public:
    virtual bool Visit(Object* object)
    {
      Object *tmp;

      if (object==base) {
        start=false;
        return false;
      }

      tmp=object->GetFocusFirst();
      if (tmp!=NULL) {
        this->object=tmp;
        return false;
      }

      return true;
    }
  };

  class FocusAfterRVisitor : public Visitor
  {
  public:
    bool   start;
    Object *base;
    Object *object;

  public:
    virtual bool Visit(Object* object)
    {
      Object *tmp;

      if (start) {
        tmp=object->GetFocusLast();
        if (tmp!=NULL) {
          this->object=tmp;
        }
      }
      else {
        if (object==base) {
          start=true;
        }
      }

      return true;
    }
  };

  class FocusBeforeRVisitor : public Visitor
  {
  public:
    bool   start;
    Object *base;
    Object *object;

  public:
    virtual bool Visit(Object* object)
    {
      Object *tmp;

      if (object==base) {
        return false;
      }

      tmp=object->GetFocusLast();
      if (tmp!=NULL) {
        this->object=tmp;
      }

      return true;
    }
  };

  class IsChildVisitor : public Visitor
  {
  public:
    bool   res;
    Object *object;

  public:
    virtual bool Visit(Object* object)
    {
      if (object==this->object) {
        res=true;
      }

      return true;
    }
  };

  /**
    \return Returns the first child object (or the object itself) that requests
    keyboard focusing.
  */
  Object* Object::GetFocusFirst()
  {
    FocusFirstVisitor visitor;

    if (HandlesKeyFocus()) {
      return this;
    }

    visitor.object=NULL;

    VisitChildren(visitor,true);

    return visitor.object;
  }

  /**
    \return Returns the last child object (or the object itself) that requests
    keyboard focusing.
  */
  Object* Object::GetFocusLast()
  {
    FocusLastVisitor visitor;

    visitor.object=NULL;

    VisitChildren(visitor,true);

    if (visitor.object!=NULL) {
      return visitor.object;
    }

    if (HandlesKeyFocus()) {
      return this;
    }
    else {
      return NULL;
    }
  }

  /**
    Returns the next object (or the object itself) that requests
    keyboard focusing.
  */
  Object* Object::GetFocusNext(Object* object)
  {
    FocusFirstVisitor  first;
    FocusAfterVisitor  after;
    FocusBeforeVisitor before;
    IsChildVisitor     test;
    Object             *res;

    if (object==this) {
      // First we search for the first focusable object in the list of children
      first.object=NULL;
      VisitChildren(first,true);

      if (first.object!=NULL) {
        return first.object;
      }

      // No focusable object in the list of children, delegate it to our parent
      if (parent!=NULL) {
        return parent->GetFocusNext(this);
      }
      else {
        return NULL;
      }
    }
    else {
      // Search for the next object after the given object
      // in our list of children
      after.start=false;
      after.base=object;
      after.object=NULL;
      VisitChildren(after,true);

      if (after.object!=NULL) {
        return after.object;
      }

      /*
        If we are called by a child and connot find a follower within our
        list of children we delegate it to the parent to investigate all
        other subtrees.
      */

      test.object=object;
      test.res=false;
      VisitChildren(test,true);

      if (test.res && parent!=NULL) {
        res=parent->GetFocusNext(this);
        if (res!=NULL) {
          return res;
        }
      }

      // Now, if that fails: can we handle it ourself?
      if (HandlesKeyFocus()) {
        return this;
      }

      // Search for the previous object in our list of children
      before.start=true;
      before.base=object;
      before.object=NULL;
      VisitChildren(before,true);

      if (before.object!=NULL) {
        return before.object;
      }
    }

    return NULL;
  }

  /**
    \return Returns the previous object (or the object itself) that requests
    keyboard focusing.
  */
  Object* Object::GetFocusPrevious(Object* object)
  {
    FocusLastVisitor    last;
    FocusAfterRVisitor  after;
    FocusBeforeRVisitor before;
    Object              *res;

    if (object==this) {
      // Delegate it to our parent
      if (parent!=NULL) {
        res=parent->GetFocusPrevious(this);
        if (res!=NULL) {
          return res;
        }
      }

      // After that we search for the last object in the list of children

      last.object=NULL;
      VisitChildren(last,true);

      return last.object;
    }
    else {
      // Search for the previous object in our list of children
      before.base=object;
      before.object=NULL;
      VisitChildren(before,true);

      if (before.object!=NULL) {
        return before.object;
      }

      // Now, if that fails: can we handle it ourself?
      if (HandlesKeyFocus()) {
        return this;
      }

      /*
        If we a called by a child and connot find a follower within our
        list of children we delegate it to the parent to investigate all
        other subtrees.
      */
      if (parent!=NULL) {
        res=parent->GetFocusPrevious(this);
        if (res!=NULL) {
          return res;
        }
      }

      // Search for the next object in our list of children
      after.start=false;
      after.base=object;
      after.object=NULL;
      VisitChildren(after,true);

      if (after.object!=NULL) {
        return after.object;
      }
    }

    return NULL;
  }

  /**
    The object base class does not know about children. This method will
    do nothing.

    Returning 'true' means "go on", returning 'false' means,
    stop and return.
  */
  bool Object::VisitChildren(Visitor &/*visitor*/, bool /*onlyVisible*/)
  {
    return true;
  }

  /**
    \return Returns true, if the object can grow in the stated direction

    An object is resizable, if
    * It hase the xxxFlex-flag set for that direction
    * And if its current size is smaller than it maximal size
  */
  bool Object::CanGrowHorizontal() const
  {
    if (horizontalFlex) {
      return width<maxWidth;
    }
    else {
      return false;
    }
  }

  /**
    \return Returns true, if the object can grow in the stated direction

    An object is resizable, if
    * It hase the xxxFlex-flag set for that direction
    * And if its current size is smaller than it maximal size
  */
  bool Object::CanGrowVertical() const
  {
    if (verticalFlex) {
      return height<maxHeight;
    }
    else {
      return false;
    }
  }

  /**
    \return Returns true, if the object can shrink in the stated direction

    An object is resizable, if
    * It hase the xxxFlex-flag set for that direction
    * And if its current size is smaller than it maximal size
  */
  bool Object::CanShrinkHorizontal() const
  {
    if (horizontalFlex) {
      return width>minWidth;
    }
    else {
      return false;
    }
  }

  /**
    \return Returns true, if the object can shrink in the stated direction

    An object is resizable, if
    * It hase the xxxFlex-flag set for that direction
    * And if its current size is smaller than it maximal size
  */
  bool Object::CanShrinkVertical() const
  {
    if (verticalFlex) {
      return height>minHeight;
    }
    else {
      return false;
    }
  }

  /**
    This calls Object::CanGrow or Object::CanShrink depending on its options
  */
  bool Object::CanResize(bool grow, bool horiz) const
  {
    if (grow) {
      if (horiz) {
        return CanGrowHorizontal();
      }
      else {
        return CanGrowVertical();
      }
    }
    else {
      if (horiz) {
        return CanShrinkHorizontal();
      }
      else {
        return CanShrinkVertical();
      }
    }
  }

  /**
    Tell the object it should calculate its size depending on its current settings

    This method normally gets calleed once before the first call of ObjectDraw.
    The window calls this method for its top object before it opens itself
    and draws its content. The top object call this method automatically for all
    its children.

    If this method get called for you object you normally have to simly
    propagate it to all your children and set the values for minWidth,
    minHeight, width and height. The setting of the max-values is optional,
    the defaultvalue for them is MAX(LONGINT).

    After this your \em must call this method of your base class.
  */
  void Object::CalcSize()
  {
    oWidth=width;
    oHeight=height;

    oMinWidth=minWidth;
    oMinHeight=minHeight;

    oMaxWidth=maxWidth;
    oMaxHeight=maxHeight;

    if (background.Valid()) {
      oWidth+=background->leftBorder+background->rightBorder;
      oHeight+=background->topBorder+background->bottomBorder;

      oMinWidth+=background->leftBorder+background->rightBorder;
      oMinHeight+=background->topBorder+background->bottomBorder;

      oMaxWidth+=background->leftBorder+background->rightBorder;
      oMaxHeight+=background->topBorder+background->bottomBorder;
    }

    for(std::list<ConstraintSize>::const_iterator iter=constraints.begin();
        iter!=constraints.end();
        ++iter) {
      switch (iter->constraint) {
      case constraintMinWidth:
        oMinWidth=std::max(oMinWidth,
                           Base::RoundRange(iter->size.GetSize(),oMinWidth,oMaxWidth));
        break;
      case constraintMinHeight:
        oMinHeight=std::max(oMinHeight,
                            Base::RoundRange(iter->size.GetSize(),oMinHeight,oMaxHeight));
        break;
      default:
        break;
      }
    }

    for(std::list<ConstraintSize>::const_iterator iter=constraints.begin();
        iter!=constraints.end();
        ++iter) {
      switch (iter->constraint) {
      case constraintMaxWidth:
        oMaxWidth=std::min(oMaxWidth,
                           Base::RoundRange(iter->size.GetSize(),oMinWidth,oMaxWidth));
        break;
      case constraintMaxHeight:
        oMaxHeight=std::min(oMaxHeight,
                            Base::RoundRange(iter->size.GetSize(),oMinHeight,oMaxHeight));
        break;
      default:
        break;
      }
    }

    oWidth=Base::RoundRange(oWidth,oMinWidth,oMaxWidth);
    oHeight=Base::RoundRange(oHeight,oMinHeight,oMaxHeight);

    for(std::list<ConstraintSize>::const_iterator iter=constraints.begin();
        iter!=constraints.end();
        ++iter) {
      switch (iter->constraint) {
        case constraintWidth:
          oWidth=Base::RoundRange(iter->size.GetSize(),oMinWidth,oMaxWidth);
          break;
        case constraintHeight:
          oHeight=Base::RoundRange(iter->size.GetSize(),oMinHeight,oMaxHeight);
          break;
        default:
          break;
      }
    }

    if (background.Valid()) {
      width=oWidth-background->leftBorder-background->rightBorder;
      height=oHeight-background->topBorder-background->bottomBorder;

      minWidth=oMinWidth-background->leftBorder-background->rightBorder;
      minHeight=oMinHeight-background->topBorder-background->bottomBorder;

      maxWidth=oMaxWidth-background->leftBorder-background->rightBorder;
      maxHeight=oMaxHeight-background->topBorder-background->bottomBorder;
    }
    else {
      width=oWidth;
      height=oHeight;
      minWidth=oMinWidth;
      minHeight=oMinHeight;
      maxWidth=oMaxWidth;
      maxHeight=oMaxHeight;
    }

    if (!horizontalFlex) {
      oMinWidth=oWidth;
      minWidth=width;
      oMaxWidth=oWidth;
      maxWidth=width;
    }

    if (!verticalFlex) {
      oMinHeight=oHeight;
      minHeight=height;
      oMaxHeight=oHeight;
      maxHeight=height;
    }

    inited=true;

    if (menu!=NULL) {
      menu->SetParent(GetWindow());
    }

    layouted=false;
  }

  bool Object::IsInited() const
  {
    return inited;
  }

  void Object::Move(int x, int y)
  {
    int oldOX,oldOY;

    oldOX=oX;
    oldOY=oY;

    oX=x;
    oY=y;

    this->x=x;
    this->y=y;

    if (background.Valid()) {
      this->x+=background->leftBorder;
      this->y+=background->topBorder;
    }

    if (oldOX!=oX || oldOY!=oY) {
      layouted=false;
    }
  }

  void Object::MoveResize(int x, int y, size_t width, size_t height)
  {
    Move(x,y);
    Resize(width,height);
  }

  /**
    This function tries to resize the object to the given measurement

    Note, that the object may not have the giving size after calling since
    it have the follow the settings for o.flags and it maximal size

    Normally the features of this implementation are enought.
    But you can overload it (At the moment none of the classes does this).
  */
  void Object::Resize(size_t width, size_t height)
  {
    ResizeWidth(width);
    ResizeHeight(height);
  }

  void Object::ResizeWidth(size_t width)
  {
    size_t oldOWidth;

    oldOWidth=oWidth;

    if (horizontalFlex) {
      oWidth=std::min(std::max(width,oMinWidth),oMaxWidth);
      this->width=oWidth;
      if (background.Valid()) {
        this->width-=background->leftBorder+background->rightBorder;
      }
    }

    if (oldOWidth!=oWidth) {
      layouted=false;
    }
  }

  void Object::ResizeHeight(size_t height)
  {
    size_t oldOHeight;

    oldOHeight=oHeight;

    if (verticalFlex) {
      oHeight=std::min(std::max(height,oMinHeight),oMaxHeight);
      this->height=oHeight;
      if (background.Valid()) {
        this->height-=background->topBorder+background->bottomBorder;
      }
    }

    if (oldOHeight!=oHeight) {
      layouted=false;
    }
  }

  void Object::SetRelayout()
  {
    layouted=false;
  }

  bool Object::IsLayouted() const
  {
    return layouted;
  }

  void Object::Layout()
  {
    struct LayoutVisitor : public Visitor
    {
      bool Visit(Lum::Object* object)
      {
        if (!object->IsLayouted()) {
          object->Layout();
        }

        object->VisitChildren(*this,true);

        return true;
      }
    };

    LayoutVisitor layoutVisitor;

    VisitChildren(layoutVisitor,true);
    layouted=true;
  }

  /**
    \return Returns true if the given area intersect with the outer area (including the outer frame),
    of the object, else return false.
  */
  bool Object::OIntersect(int &x, int &y, size_t &width, size_t &height) const
  {
    bool intersect=!((y+(int)height-1<oY) ||  /* above */
                     (y>oY+(int)oHeight-1) || /* under */
                     (x+(int)width-1<oX) ||   /* left */
                     (x>oX+(int)oWidth-1));   /* right */

    if (intersect) {
      if (x<this->oX) {
        width-=this->oX-x;
        x=this->oX;
      }

      if (y<this->oY) {
        height-=this->oY-y;
        y=this->oY;
      }

      if (x+width>this->oX+this->oWidth) {
        width-=x+width-(this->oX+this->oWidth);
      }

      if (y+height>this->oY+this->oHeight) {
        height-=y+height-(this->oY+this->oHeight);
      }
    }

    return intersect;
  }


  /**
    \return Returns true if the given area intersect with the inner area (without the outer frame),
    of the object, else return false.
  */
  bool Object::Intersect(int &x, int &y, size_t &width, size_t &height) const
  {
    bool intersect=!((y+(int)height-1<this->y) ||  /* above */
                     (y>this->y+(int)this->height-1) || /* under */
                     (x+(int)width-1<this->x) ||   /* left */
                     (x>this->x+(int)this->width-1));   /* right */

    if (intersect) {
      if (x<this->x) {
        width-=this->x-x;
        x=this->x;
      }

      if (y<this->y) {
        height-=this->y-y;
        y=this->y;
      }

      if (x+width>this->x+this->width) {
        width-=x+width-(this->x+this->width);
      }

      if (y+height>this->y+this->height) {
        height-=y+height-(this->y+this->height);
      }
    }

    return intersect;
  }

  /**
    Ask the object, if the given point it in its bounds.
  */
  bool Object::PointIsIn(int x, int y) const
  {
    if (!visible) {
      return false;
    }

    return (x>=oX) && (y>=oY) && (x<oX+(int)oWidth) && (y<oY+(int)oHeight);
  }

  bool Object::PointIsIn(const OS::MouseEvent& event) const
  {
    return PointIsIn(event.x,event.y);
  }

  /**
    This method gets called when the window thinks, that the
    object should show somekind of keyboardfocus. This will
    happen, when the object gets the focus, or when the
    focus has to be refreshed because of a window refresh.

    The base class just sets OS::DrawInfo.focused and
    redraws the object frame.
  */
  void Object::DrawFocus()
  {
    Redraw();
  }

  /**
    This method gets called when the window thinks, that the
    object should hide the keyboardfocus. This happens when
    the object loses the focus.

    The base class just redraws the object frame.
  */
  void Object::HideFocus()
  {
    Redraw();
  }

  /**
    Ask the window to change the focus to the next entry.
    This is usefull f.e. for stringgadgets. If you enter return, the
    gadget should get deactivated and the focus should change to the
    next element.

    \note
    The object must have the focus, otherwise nothing will happen.
  */
  void Object::LeaveFocus()
  {
    if (HasFocus()) {
      if (GetWindow()!=NULL && dynamic_cast<Window*>(GetWindow()->GetMaster())!=NULL) {
        dynamic_cast<Window*>(GetWindow()->GetMaster())->FocusNext();
      }
    }
  }

  /**
    Set the background object for the object. Behaviour for complex/group
    objects is undefined.
  */
  void Object::SetBackground(OS::Fill* background)
  {
    this->background=background;
  }

  /**
    Returns the fill that is responsible for drawing the background.
   */
  OS::Fill* Object::GetBackground() const
  {
    return background.Get();
  }

  /**
    Returns true if this objects has a Fill for drawing backgrounds assigned.
   */
  bool Object::HasBackground() const
  {
    return background.Valid();
  }


  void Object::PrepareForBackground(OS::DrawInfo* draw)
  {
    // no code
  }

  void Object::DrawBackground(OS::DrawInfo* draw,
                              int x, int y, size_t w, size_t h)
  {
    struct ClipVisitor : public Visitor
    {
      OS::DrawInfo *draw;
      bool         cliped;
      int          x,y;
      size_t       w,h;

      bool Visit(Object* object)
      {
        if (object->clipFromParent) {
          if (!cliped) {
            draw->PushClipBegin(x,y,w,h);
            cliped=true;
          }

          if (!object->HasBackground() ||
              !object->GetBackground()->transparentBorder) {
            draw->SubClipRegion(object->GetOX(),object->GetOY(),
                                object->GetOWidth(),object->GetOHeight());
          }
          else {
            OS::FillRef background=object->GetBackground();

            draw->SubClipRegion(object->GetOX()+background->leftBorder,
                                object->GetOY()+background->topBorder,
                                object->GetOWidth()-background->leftBorder-background->rightBorder,
                                object->GetOHeight()-background->topBorder-background->bottomBorder);
          }

        }

        object->VisitChildren(*this,true);

        return true;
      }
    };

    struct DrawVisitor : public Visitor
    {
      OS::DrawInfo *draw;
      int          x,y;
      size_t       w,h;

      bool Visit(Object* object)
      {
        int    cx,cy;
        size_t cw,ch;

        cx=x;
        cy=y;
        cw=w;
        ch=h;

        // Normally everything should be layouted in the layouting phase, however
        // not all object implement VisitChildren or call Layout manually for their children.
        if (!object->IsLayouted()) {
          object->Layout();
        }

        if (object->OIntersect(cx,cy,cw,ch)) {
          // TODO: Now, even if the object has a background it might not be necessary to draw it,
          // because the "to be drawn area" is complete covered by one or more children (recursivly)
          // that all have their own background and no transparent border.
          if (object->HasBackground()) {
            ClipVisitor visitor;

            visitor.draw=draw;
            visitor.cliped=false;
            visitor.x=cx;
            visitor.y=cy;
            visitor.w=cw;
            visitor.h=ch;

            object->VisitChildren(visitor,true);
            if (visitor.cliped) {
              draw->PushClipEnd();
            }

            object->PrepareForBackground(draw);

            if (object->clipBackgroundToBorder) {
              draw->PushClipBegin(object->GetOX(),object->GetOY(),
                                  object->GetOWidth(),object->GetOHeight());
              draw->SubClipRegion(object->GetX(),object->GetY(),
                                  object->GetWidth(),object->GetHeight());
              draw->PushClipEnd();

            }

            object->background->Draw(draw,
                                     object->GetOX(),object->GetOY(),
                                     object->GetOWidth(),object->GetOHeight(),
                                     cx,cy,cw,ch);

            if (object->clipBackgroundToBorder) {
              draw->PopClip();
            }

            draw->activated=false;
            draw->selected=false;
            draw->disabled=false;
            draw->focused=false;

            if (visitor.cliped) {
              draw->PopClip();
            }
          }

          object->PreDrawChilds(draw);
          object->VisitChildren(*this,true);
          object->PostDrawChilds(draw);
        }

        return true;
      }
    };

    assert(background.Valid());

    DrawVisitor visitor;

    visitor.draw=draw;
    visitor.x=x;
    visitor.y=y;
    visitor.w=w;
    visitor.h=h;

    visitor.Visit(this);
  }

  void Object::PreDrawChilds(OS::DrawInfo* draw)
  {
    // no code
  }

  void Object::PostDrawChilds(OS::DrawInfo* draw)
  {
    // no code
  }

  void Object::PreDrawChild(OS::DrawInfo* draw, Object* /*child*/)
  {
    // no code
  }

  void Object::PostDrawChild(OS::DrawInfo* draw, Object* /*child*/)
  {
    // no code
  }

  /**
    Tells the object to draw itself at the given position and with the mode
    set in OS::DrawInfo.mode.

    You can ignore modes you do not support and simply assume mode=normal.

    <ul>
      <li>You must call the Draw-method of your superclass first!</li>

      <li>You must not make model updates that may trigger redraw of controls, since
      a clipping reagion might be active. Use OS::display->QueueActionForEventLoop() for this</li>
    </ul>
  */
  void Object::Draw(OS::DrawInfo* draw,
                    int x, int y, size_t w, size_t h)
  {
    visible=true;
    firstDraw=false;

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* --- */

    // Normally everything should be layouted in the layouting phase, however
    // not all objects implement VisitChildren() or call Layout() manually for
    // their children.
    if (!IsLayouted()) {
      Layout();
    }

    DrawChildren(draw,x,y,w,h);
  }

  void Object::DrawChildren(OS::DrawInfo* draw, int x, int y, size_t w, size_t h)
  {
    /*
    class AreaVisitor : public Visitor
    {
    public:
      size_t area;
      int    x,y;
      size_t width,height;

      bool Visit(Object* object)
      {
        int    cx,cy;
        size_t cw,ch;

        cx=x;
        cy=y;
        cw=width;
        ch=height;

        if (object->OIntersect(cx,cy,cw,ch)) {
          area+=cw*ch;
        }

        return true;
      }
    };

    class SubRegionVisitor : public Visitor
    {
    public:
      OS::DrawInfo *draw;

      bool Visit(Object* object)
      {
        OS::Fill* background=object->GetBackground();
        if (background!=NULL) {
          if (!background->transparentBorder) {
            draw->SubClipRegion(object->GetOX(),object->GetOY(),
                                object->GetOWidth(),object->GetOHeight());
          }
          else {
            draw->SubClipRegion(object->GetOX()+background->leftBorder,
                                object->GetOY()+background->topBorder,
                                object->GetOWidth()-background->leftBorder-background->rightBorder,
                                object->GetOHeight()-background->topBorder-background->bottomBorder);
          }
        }
        return true;
      }
    };*/

    class DrawVisitor : public Visitor
    {
    public:
      OS::DrawInfo *draw;
      Object       *group;
      int          x,y;
      size_t       width,height;

      bool Visit(Object* object)
      {
        if (object->GetOX()>=x+(int)width) {
          // Object is right of area
          return true;
        }
        if (object->GetOX()+(int)object->GetOWidth()<=x) {
          // object is left of area
          return true;
        }
        if (object->GetOY()>=y+(int)height) {
          // object is below area
          return true;
        }
        if (object->GetOY()+(int)object->GetOHeight()<=y) {
          // object is above of area
          return true;
        }

        group->PreDrawChild(draw,object);
        object->Draw(draw,x,y,width,height);
        group->PostDrawChild(draw,object);

        return true;
      }
    };

    DrawVisitor drawVisitor;

    drawVisitor.draw=draw;
    drawVisitor.group=this;
    drawVisitor.x=x;
    drawVisitor.y=y;
    drawVisitor.width=w;
    drawVisitor.height=h;

    PreDrawChilds(draw);
    VisitChildren(drawVisitor,true);
    PostDrawChilds(draw);
  }

  /**
    Tells the object to redraw itself
    You normally do not need to overload this method.
  */
  void Object::Redraw()
  {
    if (visible) {
      assert(GetWindow()!=NULL);

      GetWindow()->TriggerDraw(oX,oY,oWidth,oHeight);
    }
  }

  void Object::Redraw(int x, int y, size_t w, size_t h)
  {
    if (visible) {
      assert(GetWindow()!=NULL);

      GetWindow()->TriggerDraw(x,y,w,h);
    }
  }

  /**
    Tell the object to hide itself.
    The space of the object should have the backgroundcolor after hiding.

    The method of the base class simply sets Object::visible to false. If you
    implement your own object, you must overload this methods and hide all
    possibly existing child objects. After that call
    Object::Hide of your base class. Since Object::Hide may have other purposes
    in the future, too.

    \note
    Hide is not expected to clean the area it covers when Object::Hide is
    called (It doesn't hurt though). It is expected that the parent object
    (or window if the top most object is hidden) handles hiding of an object.
    It must show anothe robject in place or must clean the area itself.

    While the parent should only call Object::Hide if the object is not
    already hidden, you should check for Object::visible before doing
    anything.
  */
  void Object::Hide()
  {
    class HideVisitor : public Visitor
    {

      bool Visit(Object* object)
      {
        object->Hide();
        return true;
      }
    };

    if (!visible) {
      return;
    }

    HideVisitor visitor;

    VisitChildren(visitor,true);

    visible=false;

    // Tell the window, that this object hides
    // Dialog may delegate focus or other things
    if (GetWindow()!=NULL && dynamic_cast<Window*>(GetWindow()->GetMaster())!=NULL) {
      dynamic_cast<Window*>(GetWindow()->GetMaster())->ObjectHides(this);
    }
  }

  bool Object::IsVisible() const
  {
    return visible;
  }

  bool Object::ShortcutAlways() const
  {
    return shortcutAlways;
  }

  /**
    If this method returns true, HandleMouseEvent() will be called before
    HandleMouseEvent() for its child objects wil be called.

    The default implementation returns false.

    */
  bool Object::InterceptMouseEvents() const
  {
    return false;
  }

  /**
    Handles Mouse events.

    \return Returns true, if the object has handled the event.

    If the objects want to grab mouse events (directly receive mouseevents),
    it must store itself in grab. If it wants to loose the grab, it must
    set grab to NIL.
  */
  bool Object::HandleMouseEvent(const OS::MouseEvent& /*event*/)
  {
    return false;
  }

  void Object::SetRedrawOnMouseActive(bool redraw)
  {
    redrawOnMouseActive=redraw;
  }

  void Object::SetClipFromParent(bool clip)
  {
    clipFromParent=clip;
  }

  void Object::SetClipBackgroundToBorder(bool clip)
  {
    clipBackgroundToBorder=clip;
  }

  /**
    Called when the mouse enters the object
  */
  void Object::MouseActive()
  {
    mouseActive=true;

    if (redrawOnMouseActive) {
      Redraw();
    }
  }

  /**
    Called when the mouse leaves the object
  */
  void Object::MouseInactive()
  {
    mouseActive=false;

    if (redrawOnMouseActive) {
      Redraw();
    }
  }

  /**
    Return true, if the mouse is currently over the object
  */
  bool Object::IsMouseActive() const
  {
    return mouseActive;
  }

  /**
    Called, when you got the keyboard focus.
  */
  void Object::CatchedFocus()
  {
    hasFocus=true;
    DrawFocus();
    if (label!=NULL && RequestsLabelFocus()) {
      label->SetShowFocus(true);
    }
  }

  /**
    Called, when you got the keyboard focus.
  */
  void Object::RecatchedFocus()
  {
    hasFocus=true;
    DrawFocus();

    if (label!=NULL && RequestsLabelFocus()) {
      label->SetShowFocus(true);
    }
  }

  /**
    Call, when the keyboard focus has been taken away from you.
  */
  void Object::LostFocus()
  {
    hasFocus=false;
    HideFocus();

    if (label!=NULL && RequestsLabelFocus()) {
      label->SetShowFocus(false);
    }
  }

  /**
    This gets called, when you have the current keyboard focus and
    the users presses keys. You can expect to get only keyboard events.

    \return
    Return true if you have handled the event, else false.

    \note
    If you activtly have grabed the focus using Object::GetFocus and
    Object::HandleEvent you will get the keyboardevents there. This
    function gets only called when you don't grabed the focus.
  */
  bool Object::HandleKeyEvent(const OS::KeyEvent& event)
  {
    std::wstring name,desc;
    Action       action;

    if (event.type==OS::KeyEvent::down &&
        controller.Valid()) {
      event.GetName(name);
      OS::display->KeyToKeyDescription(event.qualifier,name,desc);
      action=controller->GetActionForKey(desc);

      if (action!=Controller::actionNone &&
          controller->DoAction(this,action)) {
        return true;
      }
    }

    return false;
  }

  void Object::GetModels(std::set<Base::Model*>& models)
  {
    class ModelVisitor : public Visitor
    {
    private:
     std::set<Base::Model*>& models;

    public:
      ModelVisitor(std::set<Base::Model*>& models)
       : models(models)
      {
        // no code
      }

      bool Visit(Object* object)
      {
        Control* control=dynamic_cast<Control*>(object);

        if (control!=NULL) {
          Base::Model *model=control->GetModel();

          if (model!=NULL) {
            models.insert(model);
          }
        }

        return object->VisitChildren(*this,false);
      }
    };

    ModelVisitor visitor(models);

    visitor.Visit(this);
  }

  Control::~Control()
  {
    // no code
  }

  bool Control::HandlesClickFocus() const
  {
    return CanFocus() &&
           IsVisible() &&
           model.Valid();
  }

  bool Control::HandlesKeyFocus() const
  {
    return HandlesClickFocus() &&
           RequestedFocus() &&
           model.Valid() &&
           model->IsEnabled();
  }

  /**
    This method connects a model to the control. You must inherit this method,
    doing your own evaluation, if the assign of the model succeds. Depending
    on the model beeing valid or invalid you must call the base class with the
    resulting values as parameter.
   */
  bool Control::SetModel(Base::Model* model)
  {
    if (this->model.Valid()) {
      Forget(this->model);
    }

    this->model=model;

    if (this->model.Valid()) {
      Observe(this->model);
    }

    return true;
  }

  Base::Model* Control::GetModel() const
  {
    return model.Get();
  }

  bool Control::HasModel() const
  {
    return model.Valid();
  }

  void Control::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    // Delegate the focus, if we get disabled
    if (model==this->model &&
        this->model.Valid() &&
        !model->IsEnabled() &&
        HasFocus() &&
        HandlesKeyFocus()) {
      Window* dialog;

      if (GetWindow()!=NULL &&
          (dialog=dynamic_cast<Window*>(GetWindow()->GetMaster()))!=NULL) {
        dialog->FocusNext();
      }
    }

    Object::Resync(model,msg);
  }

  Scrollable::Scrollable()
  : hAdjustment(new Model::Adjustment()),
    vAdjustment(new Model::Adjustment()),
    scrollView(NULL)
  {
    Observe(hAdjustment->GetTopModel());
    Observe(vAdjustment->GetTopModel());
  }

  Scrollable::~Scrollable()
  {
    // no code
  }

  void Scrollable::GetODimension(size_t& width, size_t& height)
  {
    if (background.Valid()) {
      width-=background->leftBorder+background->rightBorder;
      height-=background->topBorder+background->bottomBorder;
    }

    GetDimension(width,height);

    if (background.Valid()) {
      width+=background->leftBorder+background->rightBorder;
      height+=background->topBorder+background->bottomBorder;
    }
  }

  /**
    If true returned, this object requires scroll knobs to change the
    adjustment. If false it returned, it is assumed that the widget itself has ways to change
    the adjustment (for exmaple by using dragging) and no scroll knobs will be used
    but possibly a simpler presentation.
    */
  bool Scrollable::RequiresKnobs() const
  {
    return true;
  }

  void Scrollable::SetHAdjustment(Model::Adjustment* adjustment)
  {
    assert(adjustment!=NULL && adjustment->GetTopModel()!=NULL);

    Forget(hAdjustment->GetTopModel());

    this->hAdjustment=adjustment;

    Observe(hAdjustment->GetTopModel());
  }

  void Scrollable::SetVAdjustment(Model::Adjustment* adjustment)
  {
    assert(adjustment!=NULL && adjustment->GetTopModel()!=NULL);

    Forget(vAdjustment->GetTopModel());

    vAdjustment=adjustment;

    Observe(vAdjustment->GetTopModel());
  }

  Model::Adjustment* Scrollable::GetHAdjustment() const
  {
    return hAdjustment;
  }

  Model::Adjustment* Scrollable::GetVAdjustment() const
  {
    return vAdjustment;
  }

  void Scrollable::SetScrollView(Object *scrollView)
  {
     this->scrollView=scrollView;
  }

  void Scrollable::Redraw()
  {
    if (scrollView!=NULL) {
      scrollView->Redraw();
    }
    else {
      Object::Redraw();
    }
  }

  void Scrollable::Redraw(int x, int y, size_t w, size_t h)
  {
    if (scrollView!=NULL) {
      scrollView->Redraw(x,y,w,h);
    }
    else {
      Object::Redraw(x,y,w,h);
    }
  }

  List::~List()
  {
    std::list<Object*>::iterator iter;

    iter=list.begin();
    while (iter!=list.end()) {
      delete *iter;

      ++iter;
    }
  }

  bool List::VisitChildren(Visitor &visitor, bool /*onlyVisible*/)
  {
    for (std::list<Object*>::iterator iter=list.begin();
         iter!=list.end();
         ++iter) {
      if (!visitor.Visit(*iter)) {
        return false;
      }
    }

    return true;
  }

  /**
    Add a new Object to the group
    Removing objects is currently not supported

    Note that some group-objects have a more special functions for adding
    members. However Add should always be supported
  */
  List* List::Add(Object* object)
  {
    object->SetParent(this);
    list.push_back(object);

    return this;
  }

  Component::Component()
  : container(NULL)
  {
    // no code
  }

  Component::~Component()
  {
    delete container;
  }

  bool Component::VisitChildren(Visitor &visitor, bool onlyVisible)
  {
    if (container!=NULL &&
        !visitor.Visit(container)) {
      return false;
    }

    return true;
  }

  bool Component::HasBaseline() const
  {
    if (container!=NULL) {
      return container->HasBaseline();
    }
    else {
      return false;
    }
  }

  size_t Component::GetBaseline() const
  {
    assert(inited && container!=NULL);

    return container->GetBaseline();
  }

  void Component::CalcSize()
  {
    assert(container!=NULL);

    container->SetParent(this);
    container->SetFlex(true,true);
    container->CalcSize();

    minWidth=container->GetOMinWidth();
    minHeight=container->GetOMinHeight();

    width=container->GetOWidth();
    height=container->GetOHeight();

    Object::CalcSize();
  }

  void Component::Layout()
  {
    assert(container!=NULL);

    container->MoveResize(x,y,width,height);

    Object::Layout();
  }

  ControlComponent::ControlComponent()
  : container(NULL)
  {
    // no code
  }

  ControlComponent::~ControlComponent()
  {
    delete container;
  }

  bool ControlComponent::VisitChildren(Visitor &visitor, bool onlyVisible)
  {
    if (container!=NULL &&
        !visitor.Visit(container)) {
      return false;
    }

    return true;
  }

  bool ControlComponent::HasBaseline() const
  {
    if (container!=NULL) {
      return container->HasBaseline();
    }
    else {
      return false;
    }
  }

  size_t ControlComponent::GetBaseline() const
  {
    assert(inited && container!=NULL);

    return container->GetBaseline();
  }

  void ControlComponent::CalcSize()
  {
    assert(container!=NULL);

    container->SetParent(this);
    container->SetFlex(true,true);
    container->CalcSize();

    minWidth=container->GetOMinWidth();
    minHeight=container->GetOMinHeight();

    width=container->GetOWidth();
    height=container->GetOHeight();

    Control::CalcSize();
  }

  void ControlComponent::Layout()
  {
    container->MoveResize(x,y,width,height);

    Control::Layout();
  }
}

