#ifndef LUM_OBJECT_H
#define LUM_OBJECT_H

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

#include <list>
#include <map>
#include <set>

#include <Lum/Base/Object.h>
#include <Lum/Base/Size.h>

#include <Lum/Model/Adjustment.h>

#include <Lum/OS/DrawInfo.h>
#include <Lum/OS/Event.h>
#include <Lum/OS/Fill.h>
#include <Lum/OS/Window.h>

namespace Lum {

  class Object;

  typedef unsigned long Action;

  /**
    Visitor allows you to enumerate a list of objects.
    You call a special visit-method, handing over an instance of
    Visitor (or a derived class containing additional information).
    The visit method will be called a every object visited.

    This makes it easy to iterate over a collection of objects
    without knowing the internal structure of the collection.
    It follows the iterator design pattern.
   */
  class LUMAPI Visitor
  {
  public:
    virtual ~Visitor();
    virtual bool Visit(Lum::Object* object);
  };

  /**
    Controller, mapping external interaction like keyboard and
    mouse events onto actions supported by the corresponding
    object.

    Normally all objects of a certain type share the same derived
    controller instance.
  */
  class LUMAPI Controller : public Base::Referencable
  {
  public:
    static const Action actionNone = 0xffff; //! Constant value for the "NULL"-action.

  private:
    std::map<std::wstring,Action> keys;

  public:
    virtual ~Controller();

    void RegisterKeyAction(const std::wstring& key, Action action);
    Action GetActionForKey(const std::wstring& key);
    virtual bool DoAction(Lum::Object* object, Action action);
  };

#if defined(LUM_INSTANTIATE_TEMPLATES)
    LUM_EXPTEMPL template class LUMAPI Lum::Base::Reference<Controller>;
#endif

  typedef Lum::Base::Reference<Controller> ControllerRef;

  /**
    Abstract base class for handling input method protocol handling.

    Classes that return an IMHandler via GetIMHandler() can participate
    in IM handling.
   */
  class IMHandler
  {
  public:
    virtual ~IMHandler();
    virtual void GetData(std::wstring& data,
                         size_t& cursorOffset) = 0;
    virtual void SetData(const std::wstring& data) = 0;
    virtual void SetCursorOffset(size_t cursorOffset) = 0;
    virtual bool IsInEditingArea(OS::MouseEvent* event) const;
  };


  /**
   \brief The base class for all graphical objects.

   This class is the base class for all graphical objects. It support attributes and methods
   supporting
   - object position and minimal, maximum and current bounds
   - Resizing
   - Drawing and refreshing
   - Keyboard Focus handling
  */
  class LUMAPI Object : public Base::DataExchangeObject
  {
  public:
    static const wchar_t* inputTypeAlpha;
    static const wchar_t* inputTypeUnsignedBinaryNumber;
    static const wchar_t* inputTypeUnsignedOctalNumber;
    static const wchar_t* inputTypeSignedDecimalNumber;
    static const wchar_t* inputTypeUnsignedDecimalNumber;
    static const wchar_t* inputTypeUnsignedHexadecimalNumber;
    static const wchar_t* inputTypeSignedFloatNumber;
    static const wchar_t* inputTypeUnsignedFloatNumber;
    static const wchar_t* inputTypePhoneNumber;

    enum Constraint
    {
      constraintMinWidth,
      constraintMinHeight,
      constraintWidth,
      constraintHeight,
      constraintMaxWidth,
      constraintMaxHeight
    };

  private:
    class ConstraintSize
    {
    public:
      Constraint constraint;
      Base::Size size;

    public:
      ConstraintSize(Constraint constraint, const Base::Size& size)
      : constraint(constraint),
        size(size)
      {
        // no code
      }
    };

  private:
    Object              *parent;        //! The parent object, may be NULL for top level objects.
    mutable OS::Window* window;        //! The OS::Window this object belongs to.

  protected:

    /**
      @name Internal position and dimension.
      The objects internal (without outer frame) minimal, current and maximal dimension.
    */
    //@{
    int                 x;
    int                 y;
    size_t              width;
    size_t              height;
    size_t              minWidth;
    size_t              minHeight;
    size_t              maxWidth;
    size_t              maxHeight;
    //@}

    /**
      @name External position and dimension.
      The objects outer (including outer frame) minimal, current and maximal dimension.
    */
    //@{
    int                 oX;
    int                 oY;
    size_t              oWidth;
    size_t              oHeight;
    size_t              oMinWidth;
    size_t              oMinHeight;
    size_t              oMaxWidth;
    size_t              oMaxHeight;
    //@}

    /**
      @name Look.
      Common variables that define the look of the widget.
    */
    //@{
    OS::FillRef         background;    //! The object background. \see Object::SetBackground
    //@}

    bool                horizontalFlex; //! Object can change its size horizontically.
    bool                verticalFlex;   //! Object can change its size vertically.
    bool                visible;        //! The object is currently visible
    bool                firstDraw;      //! If true, this is the first drawing of the widget
    bool                layouted;       //! The object has calculated its layout (can be reseted by move/resize)

  private:
    bool                clipFromParent; //! Parent objects must not draw into the area of this object (drawing of parent must be cliped)
    bool                clipBackgroundToBorder; //! Draw only the border part of the background

    /**
      @name Input focus.
      A number of flags that handle input focus behaviour.
    */
    //@{
    bool                hasFocus;      //! The object currently has the focus
    bool                canFocus;      //! The object can handle keyboard focus
    bool                showFocus;     //! The object does not have the focus, but should draw it anyway
    bool                requestsFocus; //! The object request the keyboard focus
    bool                requestsLabelFocus; //! The control requests that the label should draw focus, too.
    bool                requestsKeyboard; //! The control requests keyboard input
    //@}

    bool                mouseActive;   //! Is currently activated by mouse
    bool                redrawOnMouseActive; //! If set (default) redraw object if it gets mouse active

    /**
      @name Initial dimensions.
      The initial dimension of the object.
    */
    //@{
    std::list<ConstraintSize> constraints;
    //@}

    std::wstring        inputTypeHint; //! Optional information about the type of input
                                       //! to evaluated for virtual keyboards

  public:
    bool                inited;        //! Has been initialized
    bool                shortcutAlways;

    OS::Window*         menu;          //! The window for a popup menu. \see Object::SetMenu
    OS::Window*         help;

    Object              *label;        //! The assigned label. May be NULL. \see Object::SetLabel.
    Object              *labeling;     //! The object we are label of. May be NULL. \see Object::SetLabel.

    ControllerRef       controller;    //! The assigned Controller. May be NULL. \see Object::SetController.

  private:
    Object(const Object& object);

  protected:
    Object();

    /**
      @name Internal position and size
      Methods returning the internal position and size. The internal dimension is defined by the
      size without the outer frame around the object.
    */
    //@{
    virtual int GetX() const;
    virtual int GetY() const;
    virtual size_t GetWidth() const;
    virtual size_t GetHeight() const;
    //@}

    virtual void RequestFocus();
    virtual void UnrequestFocus();

    virtual void SetRedrawOnMouseActive(bool redraw);
    virtual void SetClipFromParent(bool clip);
    virtual void SetClipBackgroundToBorder(bool clip);

    virtual void DrawChildren(OS::DrawInfo* draw, int x, int y, size_t w, size_t h);
    virtual void PreDrawChilds(OS::DrawInfo* draw);
    virtual void PostDrawChilds(OS::DrawInfo* draw);
    virtual void PreDrawChild(OS::DrawInfo* draw, Object* child);
    virtual void PostDrawChild(OS::DrawInfo* draw, Object* child);

  public:
    virtual ~Object();

    virtual void SetController(Controller* controller);

    /**
      @name position and size
      Methods returning the (outer) position and size, this is the dimension including the
      the frame surounding the object.
    */
    //@{
    virtual int GetOX() const;
    virtual int GetOY() const;
    virtual size_t GetOWidth() const;
    virtual size_t GetOHeight() const;
    virtual size_t GetOMinWidth() const;
    virtual size_t GetOMaxWidth() const;
    virtual size_t GetOMinHeight() const;
    virtual size_t GetOMaxHeight() const;
    //@}

    /**
      @name object border
      Methods returning the border size (frame) of the object.
    */
    //@{
    virtual size_t GetTopBorder() const;
    virtual size_t GetBottomBorder() const;
    virtual size_t GetLeftBorder() const;
    virtual size_t GetRightBorder() const;
    //@}

    /**
      @name Baseline support
      Method that return information about the vertical position
      of the textual baseline of the object.
    */
    //@{
    virtual bool HasBaseline() const;
    virtual size_t GetBaseline() const;
    //@}

    /**
      @name Size setter
      Methods setting the minimal, current and maximum sizeof the object.
    */
    //@{
    virtual Object* SetConstraint(Constraint constraint,
                                  Base::Size::Mode mode,
                                  size_t value,
                                  const OS::FontRef& font=::Lum::OS::FontRef());

    virtual Object* SetConstraint(Constraint constraint,
                                  const Base::Size& size);

    virtual Object* SetWidth(Base::Size::Mode mode,
                             size_t value,
                             const OS::FontRef& font=::Lum::OS::FontRef());
    virtual Object* SetHeight(Base::Size::Mode mode,
                              size_t value,
                              const OS::FontRef& font=::Lum::OS::FontRef());

    virtual Object* SetMinWidth(Base::Size::Mode mode,
                                size_t value,
                                const OS::FontRef& font=::Lum::OS::FontRef());
    virtual Object* SetMinHeight(Base::Size::Mode mode,
                                 size_t value,
                                 const OS::FontRef& font=::Lum::OS::FontRef());

    virtual Object* SetMaxWidth(Base::Size::Mode mode,
                                size_t value,
                                const OS::FontRef& font=::Lum::OS::FontRef());
    virtual Object* SetMaxHeight(Base::Size::Mode mode,
                                 size_t value,
                                 const OS::FontRef& font=::Lum::OS::FontRef());

    virtual Object* SetMinWidth(const Base::Size& size);
    virtual Object* SetMinHeight(const Base::Size& size);
    virtual Object* SetWidth(const Base::Size& size);
    virtual Object* SetHeight(const Base::Size& size);
    virtual Object* SetMaxWidth(const Base::Size& size);
    virtual Object* SetMaxHeight(const Base::Size& size);

    //@}

    /**
      @name Methods for manipulation object resize behaviour
    */
    //@{
    virtual Object* SetFlex(bool horizontal, bool vertical);
    virtual bool HorizontalFlex() const;
    virtual bool VerticalFlex() const;

    virtual bool CanGrowHorizontal() const;
    virtual bool CanGrowVertical() const;
    virtual bool CanShrinkHorizontal() const;
    virtual bool CanShrinkVertical() const;
    virtual bool CanResize(bool grow, bool horiz) const;
    //@}

    /**
      @name Methods for accessing OS structures
    */
    //@{
    virtual void SetWindow(OS::Window* window);
    virtual OS::Window* GetWindow() const;

    //@}

    /**
      @name Methods for accessing the object hierachie
    */
    //@{
    virtual void SetParent(Object* parent);
    //@}

    /**
      @name Methods for accessing assigned popup windows
    */
    //@{
    virtual void SetHelp(OS::Window* window);
    virtual OS::Window* GetHelp() const;

    virtual void SetMenu(OS::Window* window);
    virtual OS::Window* GetMenu() const;
    virtual void PrepareMenu();
    //@}

    /**
      @name Focus manipulation methods
    */
    //@{
    virtual bool HasFocus() const;
    virtual bool CanFocus() const;
    virtual bool ShowFocus() const;
    virtual bool HandlesClickFocus() const;
    virtual bool HandlesKeyFocus() const;
    virtual void SetRequestLabelFocus(bool request);
    virtual bool RequestsLabelFocus() const;

    virtual void SetCanFocus(bool can);
    virtual void SetShowFocus(bool show);

    virtual bool RequestedFocus() const;

    virtual Object* GetFocusFirst();
    virtual Object* GetFocusLast();
    virtual Object* GetFocusNext(Object* object);
    virtual Object* GetFocusPrevious(Object* object);

    virtual void DrawFocus();
    virtual void HideFocus();
    virtual void LeaveFocus();

    virtual void CatchedFocus();
    virtual void RecatchedFocus();
    virtual void LostFocus();

    virtual Object* GetMouseGrabFocusObject() const;
    //@}

    /**
      @name Labeling methods
    */
    //@{
    virtual void SetLabelObject(Object* label);

    /**
      If true is returned, this control has implicitly a label (like
      for example a button through its button label) and thus does not
      need an extra label like a Label would offer it.
      */
    virtual bool HasImplicitLabel() const;
    //@}

    /**
      @name Input handling methods
    */
    //@{
    virtual void SetRequestsKeyboard(bool request);
    virtual bool RequestsKeyboard() const;
    virtual Object* SetInputTypeHint(const std::wstring& inputTypeHint);
    virtual std::wstring GetInputTypeHint() const;
    virtual IMHandler* GetIMHandler();
    //@}

    virtual bool VisitChildren(Visitor &visitor, bool onlyVisible);


    virtual void CalcSize();

    virtual bool IsInited() const;
    virtual bool IsVisible() const;
    virtual bool ShortcutAlways() const;

    /**
      @name Object resizing and placement methods
    */
    //@{
    virtual void Move(int x, int y);
    virtual void MoveResize(int x, int y, size_t width, size_t height);
    virtual void Resize(size_t width, size_t height);
    virtual void ResizeWidth(size_t width);
    virtual void ResizeHeight(size_t height);
    //@}

    /**
      @name Layout related methods
    */
    //@{
    virtual void SetRelayout();
    virtual bool IsLayouted() const;
    virtual void Layout();
    //@}

    /**
      @name Object point inclusion methods
    */
    //@{
    virtual bool OIntersect(int &x, int &y, size_t &width, size_t &height) const;
    virtual bool Intersect(int &x, int &y, size_t &width, size_t &height) const;
    virtual bool PointIsIn(int x, int y) const;
    virtual bool PointIsIn(const OS::MouseEvent& event) const;
    //@}

    /**
      @name Control background manipulation and drawing methods
    */
    //@{
    void SetBackground(OS::Fill* background);
    OS::Fill* GetBackground() const;
    bool    HasBackground() const;

    virtual void PrepareForBackground(OS::DrawInfo* draw);
    void DrawBackground(OS::DrawInfo* draw,
                        int x, int y, size_t w, size_t h);
    //@}

    /**
      @name Drawing style for object and its children
    */
    virtual OS::Color GetTextColor(OS::DrawInfo* draw,
                                   const void* child) const;
    virtual OS::Font* GetFont(const void* child) const;
    virtual unsigned long GetFontStyle(const void* child) const;
    virtual bool GetFontStyleSmart(const void* child) const;
    //@}

    /**
      @name Object drawing methods
    */
    //@{
    virtual void Draw(OS::DrawInfo* draw,
                      int x, int y, size_t w, size_t h);
    virtual void Redraw();
    virtual void Redraw(int x, int y, size_t w, size_t h);
    virtual void Hide();
    //@}

    /**
      @name MouseActive methods
    */
    //@{
    virtual void MouseActive();
    virtual void MouseInactive();
    virtual bool IsMouseActive() const;
    //@}

    /**
      @name Mouse and keyboard input callbacks
    */
    //@{
    virtual bool InterceptMouseEvents() const;
    virtual bool HandleMouseEvent(const OS::MouseEvent& event);
    virtual bool HandleKeyEvent(const OS::KeyEvent& event);
    //@}

    /**
      @name Mouse and keyboard input callbacks
    */
    //@{
    virtual void GetModels(std::set<Base::Model*>& models);
    //@}
  };

  /**
    New baseclasse that distinguishes between objects which
    control data and those which are used to control
    layout.
  */
  class LUMAPI Control : public Object
  {
  private:
    Base::ModelRef model;

  public:
    ~Control();

    bool HandlesClickFocus() const;
    bool HandlesKeyFocus() const;

    // Setter
    virtual bool SetModel(Base::Model* model);

    // Getter
    Base::Model* GetModel() const;

    bool HasModel() const;

    void Resync(Base::Model* model, const Base::ResyncMsg& msg);
  };

  /**
    Objects that can scoll their content.

    Scrollables must assure that they set the adjustments to their
    current and correct values in the Layout() methods. The parent
    control on the other hand  must call Layout() with the dimensions
    of the Scrollable correctly set before. This way the parent
    control should be able to decide if scrolling bars should be shown
    or not.
  */
  class LUMAPI Scrollable : public Control
  {
  protected:
    Model::AdjustmentRef hAdjustment;
    Model::AdjustmentRef vAdjustment;
    Object*              scrollView;

  protected:
    virtual void GetDimension(size_t& width, size_t& height) = 0;

  public:
    Scrollable();
    ~Scrollable();

    virtual void GetODimension(size_t& width, size_t& height);
    virtual bool RequiresKnobs() const;

    void SetHAdjustment(Model::Adjustment* adjustment);
    void SetVAdjustment(Model::Adjustment* adjustment);

    Model::Adjustment* GetHAdjustment() const;
    Model::Adjustment* GetVAdjustment() const;

    void SetScrollView(Object *scrollView);

    void Redraw();
    void Redraw(int x, int y, size_t w, size_t h);
  };

  /**
    Objects that have child controls (only for tagging purposes, does not implement any additional
    features).
  */
  class LUMAPI Group : public Object
  {
  };

  /**
    A implementation of Group using a simple list for holding all its children.
  */
  class LUMAPI List : public Group
  {
  protected:
    std::list<Object*> list; /** A linked list of all children */

  public:
    ~List();

    bool VisitChildren(Visitor &visitor, bool onlyVisible);

    List* Add(Object* object);
  };

  /**
    A Component is an object that behaves as a normal object but just
    wraps a Group as a container for a complex object structure.
  */
  class LUMAPI Component : public Object
  {
  protected:
    Object *container;

  public:
    Component();
    ~Component();

    bool VisitChildren(Visitor &visitor, bool onlyVisible);

    bool HasBaseline() const;
    size_t GetBaseline() const;

    void CalcSize();

    void Layout();
  };

  /**
    A ControlComponent is an object that behaves as a normal control but just
    wraps a Group as a container for a complex object structure.
  */
  class LUMAPI ControlComponent : public Control
  {
  protected:
    Object *container;

  public:
    ControlComponent();
    ~ControlComponent();

    bool VisitChildren(Visitor &visitor, bool onlyVisible);

    bool HasBaseline() const;
    size_t GetBaseline() const;

    void CalcSize();

    void Layout();
  };
}

#endif
