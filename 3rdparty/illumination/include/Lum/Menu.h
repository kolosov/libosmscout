#ifndef LUM_MENU_H
#define LUM_MENU_H

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

#include <Lum/Def/Menu.h>

#include <list>
#include <set>

#include <Lum/Model/Action.h>
#include <Lum/Model/Boolean.h>
#include <Lum/Model/Number.h>

#include <Lum/OS/Image.h>

#include <Lum/Object.h>

namespace Lum {

  class Menu;
  class PullDownMenu;

  /**
    Always visible menu bar containing a number of
    pulldown menues which in turn point to menu
    that contain any number of menu entries.
  */
  class LUMAPI MenuStrip : public Group
  {
  private:
    std::list<PullDownMenu*> list; /** A linked list of all children */
    Menu*                    selection;
    int                      x,y;
    PullDownMenu             *pullDownMenu;
  public:
    bool                     stickyMode;

  private:
    std::list<Lum::PullDownMenu*>::iterator FindLabel(Menu* menu);
    void PreDrawChild(OS::DrawInfo* draw,
                      Lum::Object* child);
    void PostDrawChild(OS::DrawInfo* draw,
                       Lum::Object* child);

  public:
    MenuStrip();
    ~MenuStrip();

    bool VisitChildren(Visitor &visitor, bool onlyVisible);

    void CalcSize();
    void Layout();

    bool HandleMouseEvent(const OS::MouseEvent& event);

    void SetSelection(Menu* newMenu);
    Menu* GetSelection() const;
    void AddPullDownMenu(Lum::Object* label, wchar_t sc, Menu* subMenu);
    void AddPullDownMenu(const std::wstring& label, Menu* subMenu);
    Lum::Object* GetMouseSelected(const OS::MouseEvent& event) const;

    void SelectPrevious();
    void SelectNext();

    void EvaluateMouseUp(const OS::MouseEvent& event);
  };

  /**
    Entry for a menu strip.
  */
  class LUMAPI PullDownMenu : public Object
  {
  private:
    Lum::Object*     label;          //! Label to display
    wchar_t          sc;             //! Shortcode for the menu
    Model::ActionRef shortcutAction; //! Action for shortcode keyboard event
    bool             scAssigned;

  public:
    Menu*            subMenu; //! Menu that openes if selecting the pulldown menu
    MenuStrip*       strip;   //! Points back to the strip

  public:
    PullDownMenu();
    ~PullDownMenu();

    void SetLabel(Lum::Object* object);
    void SetHotkey(wchar_t sc);
    void CalcSize();
    void Layout();
    void PrepareForBackground(OS::DrawInfo* draw);
    void Draw(OS::DrawInfo* draw,
              int x, int y, size_t w, size_t h);
    void Hide();
    void Resync(Base::Model* model, const Base::ResyncMsg& msg);

    void Activate();
  };


  /**
    Common base class for all menu entries.

    A Menu entry consists of a number of areas
    * (1) The icon area, containing entry specific icons or the bool or checkbox icons
    * (2) The label area containing the label of the entry
    * (3) The short code area, containing the short code for this menu entry
    * (4) The sub menu icon area, containing an icon for signaling the existance of an
      sub menu.

    The icon, short code and sub menu icon area can be empty (have no value) but
    will still reserve space.
  */
  class LUMAPI MenuEntry : public Control
  {
  protected:
    bool             selectable;
    unsigned long    qualifier;
    std::wstring     key;
    Model::ActionRef shortcutAction;

    OS::ImageRef     image;
    Lum::Object*     label;
    Lum::Object*     shortcut;
    OS::ImageRef     subImage;

    int              x1;
    int              x2;
    int              x3;
    int              x4;

  public:
    wchar_t          sc;
    size_t           w1;
    size_t           w2;
    size_t           w3;
    size_t           w4;
    Menu             *menuWindow;
    OS::Window       *parentWindow;

  protected:
    MenuEntry();
    ~MenuEntry();
    void Disable(bool disable);
    void Hide();
    void SetHotkey(wchar_t key);

    void DrawImage(OS::DrawInfo* draw);
    void DrawLabel(OS::DrawInfo* draw);
    void DrawShortcut(OS::DrawInfo* draw);
    void DrawSubImage(OS::DrawInfo* draw);

  public:
    void SetOffsets(int x1, int x2, int x3, int x4);

    virtual void RegisterToParentWindow(OS::Window* parent);

    virtual bool IsSelectable() const;
    virtual void OnSelection();

    virtual Menu* GetSubMenu() const;

    void SetLabel(Lum::Object* label);
    void SetShortcut(unsigned long qualifier, const std::wstring& key);
    void SetTextLabel(const std::wstring& label);

    void CalcSize();

    void Resync(Base::Model* model, const Base::ResyncMsg& msg);
  };

  /**
    A selectable menu item for a menu.
  */
  class LUMAPI ActionItem : public MenuEntry
  {
  private:
    Model::ActionRef action;

  public:
    ActionItem();

    bool SetModel(Base::Model* model);
    virtual void OnSelection();

    void PrepareForBackground(OS::DrawInfo* draw);
    void Draw(OS::DrawInfo* draw,
              int x, int y, size_t w, size_t h);
  };

  /**
    A (non-selectable) menu title object as part of a
    menu.
  */
  class TitleItem : public MenuEntry
  {
  };

  /**
    A separator for a menu.
  */
  class LUMAPI SeparatorItem : public MenuEntry
  {
  public:
    bool SetModel(Base::Model* model);

    void CalcSize();
    void Draw(OS::DrawInfo* draw,
              int x, int y, size_t w, size_t h);
  };

  /**
    A special menu entry that opens a new sub menu.
  */
  class LUMAPI SubMenu : public MenuEntry
  {
  private:
    Menu *subMenu;

  public:
    SubMenu();
    ~SubMenu();

    bool SetModel(Base::Model* model);
    void SetSubMenu(Menu* menu);

    bool IsSelectable() const;

    void RegisterToParentWindow(OS::Window* parent);
    void CalcSize();
    void PrepareForBackground(OS::DrawInfo* draw);
    void Draw(OS::DrawInfo* draw,
              int x, int y, size_t w, size_t h);

    Menu* GetSubMenu() const;
  };

  /**
    A special menu entry that has boolean state.
  */
  class LUMAPI BoolItem : public MenuEntry
  {
  private:
    Model::BooleanRef model;

  public:
    BoolItem();

    bool SetModel(Base::Model* model);
    void SetValue(size_t value);

    void OnSelection();

    void PrepareForBackground(OS::DrawInfo* draw);
    void Draw(OS::DrawInfo* draw,
              int x, int y, size_t w, size_t h);
  };

  /**
    A special menu entry that has radio state.
  */
  class RadioItem : public MenuEntry
  {
  private:
    Model::NumberRef model;
    size_t           value;

  public:
    RadioItem();

    bool SetModel(Base::Model* model);
    void SetValue(size_t value);

    void OnSelection();

    void PrepareForBackground(OS::DrawInfo* draw);
    void Draw(OS::DrawInfo* draw,
              int x, int y, size_t w, size_t h);
  };

  /* Anonymous type, please ignore! */
  class MenuBox;

  class LUMAPI Menu : public Base::MsgObject
  {
  private:
    MenuBox     *top;        //! Layout container object

    OS::Window  *window;     //! The underlying OS window
    // Pulldown stuff
    OS::Window *parent;      //! Parent window
    bool        popup;       //! Top menu of a popup menu
    bool        registered;
    Lum::Object *reference;  //! Optional control the window should be opened relative to
    bool        manualPos;   //! If true, xPos and yPos hold the position of the menu
    int         xPos;
    int         yPos;
    bool        stickyMode;

  public:
    SubMenu     *parentMenu; //! Parent menu
    Menu        *child;      //! Currently open child menu
    MenuEntry   *selected;   //! The currently selected entry
    MenuStrip   *strip;      //! If !=NULL this is a pulldown menu and points to the menu strip

  private:
    bool VisitChildren(Visitor &visitor, bool onlyVisible);
    void Add(MenuEntry* entry);
    Lum::Object* GetObjectByName(const std::wstring& name);
    MenuEntry* GetSelected();
    MenuStrip* GetStrip();
    Menu* GetTop();
    Menu* GetMenuWithSelection();
    Menu* GetMenuWithMouseSelection();
    void CloseAll();
    void EndAction();
    void Select(MenuEntry* entry);
    void SelectUp();
    void SelectDown();
    void SelectLeft();
    void SelectRight();
    void ActivateSelection();
    void SelectKey(wchar_t key);

  public:
    Menu();
    ~Menu();

    bool IsOpen() const;
    bool CursorIsIn() const;

    void SetParent(OS::Window* parent);
    void SetReference(Lum::Object* reference);
    void SetPos(int x, int y);

    void CalcSize();
    void PreInit();
    bool Open();
    bool OpenInStickyMode();
    void Close();
    bool HandleEvent(OS::Event* event);

    OS::Window* GetWindow() const;

    void AddActionItem(const std::wstring& label,
                       Model::Action* action=NULL,
                       unsigned long qualifier=0,
                       const std::wstring& key=L"");
    void AddBoolItem(const std::wstring& label,
                     Model::Boolean* model,
                     unsigned long qualifier=0,
                     const std::wstring& key=L"");
    void AddRadioItem(const std::wstring& label,
                     Model::Number* model,
                     size_t index,
                     unsigned long qualifier=0,
                     const std::wstring& key=L"");
    void AddSeparator();
    void AddSubMenu(Lum::Object* label, Menu* subMenu);
    void AddSubMenu(const std::wstring& label, Menu* subMenu);

    void Resync(Lum::Base::Model* model,
                const Lum::Base::ResyncMsg& msg);
  };

  extern LUMAPI Menu* ConvertToMenu(Def::Menu* def);
  extern LUMAPI Menu* ConvertToMenu(Def::Menu* def,
                                    const std::set<Base::Model*>& excludes);
  extern LUMAPI MenuStrip* ConvertToMenuStrip(Def::Menu* def);
  extern LUMAPI MenuStrip* ConvertToMenuStrip(Def::Menu* def,
                                              const std::set<Base::Model*>& excludes);
}

#endif
