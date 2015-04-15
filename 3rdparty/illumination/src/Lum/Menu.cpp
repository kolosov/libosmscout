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

#include <Lum/Menu.h>

#include <cctype>
#include <cstdlib>

#include <Lum/Base/Util.h>

#include <Lum/OS/Driver.h>
#include <Lum/OS/Theme.h>

#include <Lum/Dialog.h>
#include <Lum/Text.h>

namespace Lum {

  size_t GetValidItemsCount(const std::list<Def::MenuItem*>& items,
                            const std::set<Base::Model*>& excludes);

  bool WillFoldSubItems(const std::list<Def::MenuItem*>& items,
                        const std::set<Base::Model*>& excludes)
  {
    size_t currentItemsCount=0;
    size_t subItemsCount=0;

    for (std::list<Def::MenuItem*>::const_iterator iter=items.begin();
         iter!=items.end();
         ++iter) {
      assert((*iter)!=NULL);

      if (dynamic_cast<Def::Menu*>(*iter)!=NULL) {
        Def::Menu *sub=dynamic_cast<Def::Menu*>(*iter);

        if (sub->GetItems().size()==1 &&
            Lum::OS::display->GetTheme()->OptimizeMenus()) {
          currentItemsCount++;
        }
        else {
          subItemsCount+=GetValidItemsCount(sub->GetItems(),excludes);
        }
      }
      else if (dynamic_cast<Def::MenuAction*>(*iter)!=NULL) {
        Def::MenuAction *action=dynamic_cast<Def::MenuAction*>(*iter);

        if (Lum::OS::display->GetTheme()->OptimizeMenus()) {
          if (excludes.find(action->GetAction().GetAction())!=excludes.end()) {
            continue;
          }
        }

        currentItemsCount++;
      }
      else if (dynamic_cast<Def::MenuBoolean*>(*iter)!=NULL) {
        Def::MenuBoolean *boolean=dynamic_cast<Def::MenuBoolean*>(*iter);

        if (Lum::OS::display->GetTheme()->OptimizeMenus()) {
          if (excludes.find(boolean->GetBoolean().GetBoolean())!=excludes.end()) {
            continue;
          }
        }

        currentItemsCount++;
      }
      else if (dynamic_cast<Def::MenuOneOfN*>(*iter)!=NULL) {
        Def::MenuOneOfN *oneOfN=dynamic_cast<Def::MenuOneOfN*>(*iter);

        if (Lum::OS::display->GetTheme()->OptimizeMenus()) {
          if (excludes.find(oneOfN->GetOneOfN().GetValue())!=excludes.end()) {
            continue;
          }
        }

        currentItemsCount+=oneOfN->GetOneOfN().GetChoices().size();
      }
      else if (dynamic_cast<Def::MenuSeparator*>(*iter)!=NULL) {
        if (!Lum::OS::display->GetTheme()->OptimizeMenus()) {
          currentItemsCount++;
        }
      }
      else {
        assert(false);
      }
    }

    return Lum::OS::display->GetTheme()->OptimizeMenus() &&
           currentItemsCount+subItemsCount<=Lum::OS::display->GetTheme()->MaxPopupMenuEntries();
  }

  size_t GetValidItemsCount(const std::list<Def::MenuItem*>& items,
                            const std::set<Base::Model*>& excludes)
  {
    size_t result=0;
    bool   foldSubItems=WillFoldSubItems(items,excludes);

    for (std::list<Def::MenuItem*>::const_iterator iter=items.begin();
         iter!=items.end();
         ++iter) {
      assert((*iter)!=NULL);
      if (dynamic_cast<Def::Menu*>(*iter)!=NULL) {
        Def::Menu *sub=dynamic_cast<Def::Menu*>(*iter);

        if (sub->GetItems().size()>0) {
          if (sub->GetItems().size()==1 &&
              Lum::OS::display->GetTheme()->OptimizeMenus()) {
            result++;
          }
          else if (foldSubItems) {
            result+=GetValidItemsCount(sub->GetItems(),excludes);
          }
          else {
            result+=1+GetValidItemsCount(sub->GetItems(),excludes);
          }
        }
      }
      else if (dynamic_cast<Def::MenuAction*>(*iter)!=NULL) {
        Def::MenuAction *action=dynamic_cast<Def::MenuAction*>(*iter);

        if (Lum::OS::display->GetTheme()->OptimizeMenus()) {
          if (excludes.find(action->GetAction().GetAction())!=excludes.end()) {
            continue;
          }
        }

        result++;
      }
      else if (dynamic_cast<Def::MenuBoolean*>(*iter)!=NULL) {
        Def::MenuBoolean *boolean=dynamic_cast<Def::MenuBoolean*>(*iter);

        if (Lum::OS::display->GetTheme()->OptimizeMenus()) {
          if (excludes.find(boolean->GetBoolean().GetBoolean())!=excludes.end()) {
            continue;
          }
        }

        result++;
      }
      else if (dynamic_cast<Def::MenuOneOfN*>(*iter)!=NULL) {
        Def::MenuOneOfN *oneOfN=dynamic_cast<Def::MenuOneOfN*>(*iter);

        if (Lum::OS::display->GetTheme()->OptimizeMenus()) {
          if (excludes.find(oneOfN->GetOneOfN().GetValue())!=excludes.end()) {
            continue;
          }
        }

        result+=oneOfN->GetOneOfN().GetChoices().size();
      }
      else if (dynamic_cast<Def::MenuSeparator*>(*iter)!=NULL) {
        if (!Lum::OS::display->GetTheme()->OptimizeMenus()) {
          result++;
        }
      }
      else {
        assert(false);
      }
    }

    return result;
  }

  void ConvertItem(Menu* menu, Def::MenuItem* item,
                   const std::set<Base::Model*>& excludes, bool foldSubItems)
  {
    if (dynamic_cast<Def::Menu*>(item)!=NULL) {
      Def::Menu *sub=dynamic_cast<Def::Menu*>(item);
      size_t subCount=GetValidItemsCount(sub->GetItems(),excludes);

      if (subCount>0) {
        if (subCount==1 &&
            Lum::OS::display->GetTheme()->OptimizeMenus()) {
          ConvertItem(menu,sub->GetItems().front(),excludes,true);
        }
        else if (foldSubItems) {
          for (std::list<Def::MenuItem*>::const_iterator s=sub->GetItems().begin();
               s!=sub->GetItems().end();
               ++s) {
            ConvertItem(menu,*s,excludes,true);
          }
        }
        else {
          menu->AddSubMenu(sub->GetDesc().GetLabel(),
                           ConvertToMenu(sub,excludes));
        }
      }
    }
    else if (dynamic_cast<Def::MenuAction*>(item)!=NULL) {
      Def::MenuAction *action=dynamic_cast<Def::MenuAction*>(item);

      if (Lum::OS::display->GetTheme()->OptimizeMenus()) {
        if (excludes.find(action->GetAction().GetAction())!=excludes.end()) {
          return;
        }
      }

      std::wstring label=action->GetAction().GetDesc().GetLabel();

      if (action->GetAction().GetOpensDialog()) {
        label=label+L"...";
      }

      if (action->GetAction().GetDesc().HasShortcut()) {
      menu->AddActionItem(label,
                          /*action->GetImage(),*/
                          action->GetAction().GetAction(),
                          action->GetAction().GetDesc().GetShortcutQualifier(),
                          action->GetAction().GetDesc().GetShortcutKey());
      }
      else {
        menu->AddActionItem(label,
                            /*action->GetImage(),*/
                            action->GetAction().GetAction());
      }
    }
    else if (dynamic_cast<Def::MenuBoolean*>(item)!=NULL) {
      Def::MenuBoolean *boolean=dynamic_cast<Def::MenuBoolean*>(item);

      if (Lum::OS::display->GetTheme()->OptimizeMenus()) {
        if (excludes.find(boolean->GetBoolean().GetBoolean())!=excludes.end()) {
          return;
        }
      }

      if (boolean->GetBoolean().GetDesc().HasShortcut()) {
        menu->AddBoolItem(boolean->GetBoolean().GetDesc().GetLabel(),
                          /*action->GetImage(),*/
                          boolean->GetBoolean().GetBoolean(),
                            boolean->GetBoolean().GetDesc().GetShortcutQualifier(),
                            boolean->GetBoolean().GetDesc().GetShortcutKey());
      }
      else {
        menu->AddBoolItem(boolean->GetBoolean().GetDesc().GetLabel(),
                          /*action->GetImage(),*/
                          boolean->GetBoolean().GetBoolean());
      }
    }
    else if (dynamic_cast<Def::MenuOneOfN*>(item)!=NULL) {
      Def::MenuOneOfN *oneOfN=dynamic_cast<Def::MenuOneOfN*>(item);

      if (Lum::OS::display->GetTheme()->OptimizeMenus()) {
        if (excludes.find(oneOfN->GetOneOfN().GetValue())!=excludes.end()) {
          return;
        }
      }

      size_t index=1;
      for (std::list<Def::OneOfN::Choice>::const_iterator choice=oneOfN->GetOneOfN().GetChoices().begin();
           choice!=oneOfN->GetOneOfN().GetChoices().end();
           ++choice) {
        if (choice->GetDesc().HasShortcut()) {
          menu->AddRadioItem(choice->GetDesc().GetLabel(),
                             /*radio->GetImage(),*/
                             oneOfN->GetOneOfN().GetValue(),
                             index,
                             choice->GetDesc().GetShortcutQualifier(),
                             choice->GetDesc().GetShortcutKey());
        }
        else {
          menu->AddRadioItem(choice->GetDesc().GetLabel(),
                             /*radio->GetImage(),*/
                             oneOfN->GetOneOfN().GetValue(),
                             index);
        }

        index++;
      }
    }
    else if (dynamic_cast<Def::MenuSeparator*>(item)!=NULL) {
      if (Lum::OS::display->GetTheme()->OptimizeMenus()) {
        return;
      }

      menu->AddSeparator();
    }
    else {
      assert(false);
    }
  }

  Menu* ConvertToMenu(Def::Menu* def,
                      const std::set<Base::Model*>& excludes)
  {
    Menu *menu=new Menu();
    bool foldSubItems=WillFoldSubItems(def->GetItems(),excludes);

    for (std::list<Def::MenuItem*>::const_iterator iter=def->GetItems().begin();
         iter!=def->GetItems().end();
         ++iter) {
      assert((*iter)!=NULL);

      ConvertItem(menu,*iter,excludes,foldSubItems);
    }

    return menu;
  }

  Menu* ConvertToMenu(Def::Menu* def)
  {
    std::set<Base::Model*> excludes;

    return ConvertToMenu(def,excludes);
  }

  MenuStrip* ConvertToMenuStrip(Def::Menu* def,
                                const std::set<Base::Model*>& excludes)
  {
    assert(def!=NULL);

    MenuStrip *strip=new MenuStrip();
    size_t    pullDownCount=0;

    strip->SetFlex(true,false);

    for (std::list<Def::MenuItem*>::const_iterator iter=def->GetItems().begin();
         iter!=def->GetItems().end();
         ++iter) {
      assert((*iter)!=NULL);

      if (dynamic_cast<Def::Menu*>(*iter)!=NULL) {
        Def::Menu *sub=dynamic_cast<Def::Menu*>(*iter);

        if (GetValidItemsCount(sub->GetItems(),excludes)>0) {
          strip->AddPullDownMenu(sub->GetDesc().GetLabel(),
                                 ConvertToMenu(sub));
          pullDownCount++;
        }
      }
      else {
        assert(false);
      }
    }

    if (pullDownCount==0) {
      delete strip;
      strip=NULL;
    }

    return strip;
  }

  MenuStrip* ConvertToMenuStrip(Def::Menu* def)
  {
    std::set<Base::Model*> excludes;

    return ConvertToMenuStrip(def,excludes);
  }


  /* ---------- Menustrip stuff ------------------*/

  MenuStrip::MenuStrip()
  : selection(NULL),
    x(0),
    y(0),
    pullDownMenu(NULL),
    stickyMode(false)
  {
    SetBackground(OS::display->GetFill(OS::Display::menuStripBackgroundFillIndex));

    SetFlex(true,false);
  }

  MenuStrip::~MenuStrip()
  {
    for (std::list<PullDownMenu*>::const_iterator object=list.begin();
         object!=list.end();
         ++object) {
      delete *object;
    }
  }

  /**
    Finds the menu entry (@otype{PullDownMenu}) that opens the given (sub)menu).
  */
  std::list<PullDownMenu*>::iterator MenuStrip::FindLabel(Menu* menu)
  {
    std::list<PullDownMenu*>::iterator iter;

    iter=list.begin();
    while (iter!=list.end()) {
      if ((*iter)->subMenu==menu) {
        return iter;
      }

      ++iter;
    }

    return list.end();
  }

  /**
    Select the given (direct) sub menu.
  */
  void MenuStrip::SetSelection(Menu* newMenu)
  {
    if (selection!=NULL && selection->IsOpen()) {
      selection->Close();
    }
    selection=newMenu;
    if (selection!=NULL) {
      selection->Open();
    }

    Redraw();
  }

  Menu* MenuStrip::GetSelection() const
  {
    return selection;
  }

  /**
    Select the previous entry in the menu strip.
  */
  void MenuStrip::SelectPrevious()
  {
    std::list<PullDownMenu*>::iterator object;

    if (selection==NULL) {
      return;
    }

    object=FindLabel(selection);

    if (object==list.begin()) {
      SetSelection((*list.rbegin())->subMenu);
    }
    else {
      object--;
      SetSelection((*object)->subMenu);
    }
  }

  /**
    Select the next entry in the menu strip.
  */
  void MenuStrip::SelectNext()
  {
    std::list<PullDownMenu*>::iterator object;

    if (selection==NULL) {
      return;
    }

    object=FindLabel(selection);

    object++;

    if (object!=list.end()) {
      SetSelection((*object)->subMenu);
    }
    else if (list.begin()!=list.end()) {
      SetSelection((*list.begin())->subMenu);
    }
  }

  /**
    Return the (PullDownMenu) object currently under the mouse or
    NULL, if there is no child object under the mouse.
  */
  Object* MenuStrip::GetMouseSelected(const OS::MouseEvent& event) const
  {
    for (std::list<PullDownMenu*>::const_iterator object=list.begin();
         object!=list.end();
         ++object) {
      if ((*object)->PointIsIn(event)) {
        return *object;
      }
    }

    return NULL;
  }

  void MenuStrip::AddPullDownMenu(Object* label, wchar_t sc, Menu* subMenu)
  {
    PullDownMenu *menu;

    menu=new PullDownMenu();
    menu->SetParent(this);
    menu->SetFlex(false,true);
    menu->SetLabel(label);
    menu->SetHotkey(sc);
    menu->subMenu=subMenu;
    menu->strip=this;

    if (subMenu!=NULL) {
      subMenu->strip=this;
      subMenu->SetReference(menu);
    }

    list.push_back(menu);
  }

  void MenuStrip::AddPullDownMenu(const std::wstring& label, Menu* subMenu)
  {
    Text    *text;
    wchar_t shortcut;
    size_t  pos;

    text=new Text();
    text->SetAlignment(Text::centered);
    text->SetFlex(true,true);

    pos=label.find('_');
    if (pos!=std::wstring::npos && pos+1<label.length()) {
      shortcut=label[pos+1];
      text->AddText(label.substr(0,pos));
      if (OS::display->GetTheme()->ShowKeyShortcuts()) {
        text->AddText(label.substr(pos+1,1),OS::Font::underlined);
      }
      else {
        text->AddText(label.substr(pos+1,1));
      }
      text->AddText(label.substr(pos+2));
    }
    else {
      shortcut='\0';
      text->AddText(label);
    }

    AddPullDownMenu(text,shortcut,subMenu);
  }

  bool MenuStrip::VisitChildren(Visitor &visitor, bool onlyVisible)
  {
    for (std::list<PullDownMenu*>::iterator iter=list.begin(); iter!=list.end(); ++iter) {
      if (!visitor.Visit(*iter)) {
        return false;
      }
    }

    return true;
  }

  void MenuStrip::CalcSize()
  {
    std::list<PullDownMenu*>::const_iterator object;

    width=0;
    height=0;

    object=list.begin();
    while (object!=list.end()) {
      (*object)->CalcSize();
      height=std::max(height,(*object)->GetOHeight());
      width+=(*object)->GetOWidth();

      ++object;
    }

    Group::CalcSize();
  }

  void MenuStrip::Layout()
  {
    std::list<PullDownMenu*>::const_iterator object;
    int                                  pos;

    pos=x;
    object=list.begin();
    while (object!=list.end()) {
      (*object)->ResizeHeight(height);
      (*object)->Move(pos,y + (height-(*object)->GetOHeight()) / 2);
      pos+=(*object)->GetOWidth();

      ++object;
    }

    Group::Layout();
  }

  /**
    Before drawing a child, find out, if it is currently selected and make it
    drawn selected, if it is.
  */
  void MenuStrip::PreDrawChild(OS::DrawInfo* draw,
                               Object* child)
  {
    std::list<PullDownMenu*>::iterator label=FindLabel(selection);
    if (label!=list.end() && child==*label) {
      draw->selected=true;
    }
  }

  /**
    Reverts @oproc{MenuStrip.PreDrawChild}.
  */
  void MenuStrip::PostDrawChild(OS::DrawInfo* draw,
                                Object* /*child*/)
  {
    draw->selected=false;
  }

  bool MenuStrip::HandleMouseEvent(const OS::MouseEvent& event)
  {
    if (!visible) {
      return false;
    }

    if (event.type==OS::MouseEvent::down &&
        PointIsIn(event) &&
        (event.button==OS::MouseEvent::button1 || event.button==OS::MouseEvent::button3)) {

      stickyMode=false;
      pullDownMenu=NULL;

      for (std::list<PullDownMenu*>::iterator p=list.begin(); p!=list.end(); ++p) {
        if ((*p)->PointIsIn(event)) {
          (*p)->Activate();
          x=event.x;
          y=event.y;
          pullDownMenu=*p;
          break;
        }
      }

      return true;
    }
    else if (event.type==OS::MouseEvent::move &&
             PointIsIn(event) &&
             (event.qualifier==OS::qualifierButton1 || event.qualifier==OS::qualifierButton3 || stickyMode)) {
      for (std::list<PullDownMenu*>::iterator p=list.begin(); p!=list.end(); ++p) {
        if ((*p)->PointIsIn(event)) {
          (*p)->Activate();
        }
      }
      return true;
    }
    else if (event.type==OS::MouseEvent::up) {
      EvaluateMouseUp(event);
    }

    return false;
  }

  void MenuStrip::EvaluateMouseUp(const OS::MouseEvent& event)
  {
    if (event.type==OS::MouseEvent::up &&
        PointIsIn(event) &&
        (event.button==OS::MouseEvent::button1 || event.button==OS::MouseEvent::button3)) {
      if ((size_t)std::abs(event.x-x)<=OS::display->GetTheme()->GetMouseClickHoldSensitivity() &&
          (size_t)std::abs(event.y-y)<=OS::display->GetTheme()->GetMouseClickHoldSensitivity()) {
        stickyMode=true;
      }
      else {
        for (std::list<PullDownMenu*>::iterator p=list.begin(); p!=list.end(); ++p) {
          if ((*p)->PointIsIn(event) && pullDownMenu!=*p) {
            stickyMode=true;
            break;
          }
        }
      }
    }
  }

  /* ---------- PullDownMenu stuff ------------------*/

  PullDownMenu::PullDownMenu()
  : label(NULL),
    sc('\0'),
    shortcutAction(new Model::Action),
    scAssigned(false),
    subMenu(NULL)
  {
    SetBackground(OS::display->GetFill(OS::Display::menuPulldownBackgroundFillIndex));

    SetFlex(true,true);

    Observe(shortcutAction);
  }

  PullDownMenu::~PullDownMenu()
  {
    if (sc!=L'\0' && dynamic_cast<Dialog*>(GetWindow()->GetMaster())!=NULL) {
      dynamic_cast<Dialog*>(GetWindow()->GetMaster())->UnregisterShortcut(shortcutAction);
    }

    delete label;
    delete subMenu;
  }


  void PullDownMenu::SetLabel(Object* object)
  {
    label=object;
    label->SetParent(this);
  }

  void PullDownMenu::SetHotkey(wchar_t sc)
  {
    this->sc=sc;
  }

  void PullDownMenu::CalcSize()
  {
    label->CalcSize();

    width=label->GetOWidth()+2*OS::display->GetSpaceHorizontal(OS::Display::spaceObjectBorder);
    height=label->GetOHeight()+2*OS::display->GetSpaceVertical(OS::Display::spaceObjectBorder);
    minWidth=width;
    minHeight=height;

    Object::CalcSize();
  }

  void PullDownMenu::Layout()
  {
    label->Resize(width-2*OS::display->GetSpaceHorizontal(OS::Display::spaceObjectBorder),
                  height-2*OS::display->GetSpaceVertical(OS::Display::spaceObjectBorder));
    label->Move(x+OS::display->GetSpaceHorizontal(OS::Display::spaceObjectBorder),
                y+OS::display->GetSpaceVertical(OS::Display::spaceObjectBorder));

    Object::Layout();
  }

  void PullDownMenu::PrepareForBackground(OS::DrawInfo* draw)
  {
    draw->activated=IsMouseActive();
    draw->selected=strip!=NULL && subMenu!=NULL && strip->GetSelection()==subMenu;
  }

  void PullDownMenu::Draw(OS::DrawInfo* draw,
                          int x, int y, size_t w, size_t h)
  {
    Object::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* ---- */

    if (!scAssigned) {
      if (sc!=L'\0' && dynamic_cast<Dialog*>(GetWindow()->GetMaster())!=NULL) {
        dynamic_cast<Dialog*>(GetWindow()->GetMaster())->RegisterShortcut(this,
                                                                          OS::qualifierAlt,std::wstring(1,sc),
                                                                          shortcutAction);
      }

      if (subMenu!=NULL) {
        subMenu->SetParent(GetWindow());
      }

      scAssigned=true;
    }

    draw->activated=IsMouseActive();
    label->Draw(draw,x,y,w,h);
    draw->activated=false;
  }

  void PullDownMenu::Hide()
  {
    if (visible) {
      label->Hide();
    }

    Object::Hide();
  }

  void PullDownMenu::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    // TODO: Move this code to the strip
    if (model==this->shortcutAction && this->shortcutAction->IsFinished()) {
      strip->stickyMode=true;
      strip->SetSelection(subMenu);
    }

    Object::Resync(model,msg);
  }

  void PullDownMenu::Activate()
  {
    if (subMenu!=NULL && !subMenu->IsOpen()) {
      strip->SetSelection(subMenu);
    }
  }

  /* ---------- Menu entry stuff ------------------*/

  MenuEntry::MenuEntry()
  : selectable(false),
    shortcutAction(new Model::Action),
    image(NULL),label(NULL),shortcut(NULL),subImage(NULL),
    x1(0),x2(0),x3(0),x4(0),
    sc(L'\0'),
    w1(0),w2(0),w3(0),w4(0),
    menuWindow(NULL),
    parentWindow(NULL)
  {
    SetBackground(OS::display->GetFill(OS::Display::menuEntryBackgroundFillIndex));

    shortcutAlways=true;

    Observe(shortcutAction);
  }

  MenuEntry::~MenuEntry()
  {
    if (!key.empty() &&
        parentWindow!=NULL &&
        parentWindow->GetMaster()!=NULL &&
        dynamic_cast<Dialog*>(parentWindow->GetMaster())!=NULL) {
      dynamic_cast<Dialog*>(parentWindow->GetMaster())->UnregisterShortcut(shortcutAction);
    }

    delete label;
    delete shortcut;
  }

  void MenuEntry::SetOffsets(int x1, int x2, int x3, int x4)
  {
    this->x1=x1;
    this->x2=x2;
    this->x3=x3;
    this->x4=x4;
  }

  void MenuEntry::Hide()
  {
    if (label!=NULL) {
      label->Hide();
    }
    if (shortcut!=NULL) {
      shortcut->Hide();
    }

    Control::Hide();
  }

  bool MenuEntry::IsSelectable() const
  {
    return selectable && GetModel()!=NULL && GetModel()->IsEnabled();
  }

  void MenuEntry::SetHotkey(wchar_t key)
  {
    sc=key;
  }

  void MenuEntry::RegisterToParentWindow(OS::Window* parent)
  {
    parentWindow=parent;

    if (!key.empty()) {
      assert(parent!=NULL);
      if (dynamic_cast<Dialog*>(parent->GetMaster())!=NULL) {
        dynamic_cast<Dialog*>(parent->GetMaster())->RegisterShortcut(this,qualifier,key,shortcutAction);
      }
    }
  }

  void MenuEntry::CalcSize()
  {
    if (image.Valid()) {
      w1=image->GetWidth();
    }
    else {
      w1=0;
    }

    if (label!=NULL) {
      if (!label->IsInited()) {
        label->SetParent(this);
        label->SetFlex(true,true);
        label->CalcSize();
      }
      w2=label->GetOWidth();
      height=std::max(height,label->GetOHeight()+2*OS::display->GetSpaceVertical(OS::Display::spaceObjectBorder));
    }

    if (shortcut!=NULL &&
        OS::display->GetTheme()->ShowKeyShortcuts()) {
      if (!shortcut->IsInited()) {
        shortcut->SetParent(this);
        shortcut->SetFlex(true,true);
        shortcut->CalcSize();
      }
      w3=shortcut->GetOWidth();
      height=std::max(height,shortcut->GetOHeight()+2*OS::display->GetSpaceVertical(OS::Display::spaceObjectBorder));
    }
    else {
      w3=0;
    }

    if (subImage.Valid()) {
      w4=subImage->GetWidth();
    }
    else {
      w4=0;
    }

    width=w1+w2+w3+w4;

    Control::CalcSize();
  }

  void MenuEntry::DrawImage(OS::DrawInfo* draw)
  {
    if (image.Valid()) {
      // Left align image
      image->Draw(draw,
                   x1,y+(height-image->GetHeight()) / 2);
    }
  }

  void MenuEntry::DrawLabel(OS::DrawInfo* draw)
  {
    if (label!=NULL) {
      // Left align label
      label->Move(x2,y+(height-label->GetOHeight()) / 2);
      label->Draw(draw,x,y,width,height);
    }
  }

  void MenuEntry::DrawShortcut(OS::DrawInfo* draw)
  {
    if (shortcut!=NULL &&
        OS::display->GetTheme()->ShowKeyShortcuts()) {
      // Right align shortcut
      shortcut->Move(x3+w3-shortcut->GetOWidth(),
                     y+(height-shortcut->GetOHeight()) / 2);
      shortcut->Draw(draw,x,y,width,height);
    }
  }

  void MenuEntry::DrawSubImage(OS::DrawInfo* draw)
  {
    if (subImage.Valid()) {
      // Left align sub image
      subImage->Draw(draw,
                     x4,y+(height-subImage->GetHeight()) / 2);
    }
  }

  Menu* MenuEntry::GetSubMenu() const
  {
    return NULL;
  }

  void MenuEntry::SetLabel(Object* label)
  {
    this->label=label;
  }

  void MenuEntry::SetTextLabel(const std::wstring& label)
  {
    Text    *text;
    size_t  pos;
    wchar_t sc;

    text=new Text();
    text->SetAlignment(Text::centered);
    text->SetFlex(true,true);

    pos=label.find('_');
    if (pos!=std::wstring::npos && pos+1<label.length()) {
      sc=label[pos+1];
      text->AddText(label.substr(0,pos));
      if (OS::display->GetTheme()->ShowKeyShortcuts()) {
        text->AddText(label.substr(pos+1,1),OS::Font::underlined);
      }
      else {
        text->AddText(label.substr(pos+1,1));
      }
      text->AddText(label.substr(pos+2));

      SetHotkey(sc);
    }
    else {
      sc='\0';
      text->AddText(label);
    }

    SetLabel(text);
  }

  void MenuEntry::SetShortcut(unsigned long qualifier, const std::wstring& key)
  {
    std::wstring shortcut;

    OS::display->KeyToDisplayKeyDescription(qualifier,key,shortcut);
    this->shortcut=new Text(shortcut);
    this->qualifier=qualifier;
    this->key=key;
  }

  void MenuEntry::OnSelection()
  {
    // no code
  }

  void MenuEntry::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==this->shortcutAction && this->shortcutAction->IsFinished()) {
      OnSelection();
    }

    Control::Resync(model,msg);
  }

  /* ---------- Menu item stuff ------------------*/

  ActionItem::ActionItem()
  : action(NULL)
  {
    selectable=true;
  }

  bool ActionItem::SetModel(Base::Model* model)
  {
    action=dynamic_cast<Model::Action*>(model);

    Control::SetModel(action);

    return action.Valid();
  }

  void ActionItem::OnSelection()
  {
    // We make a copy of the action reference, because the triggering of the action
    // might result in deletion of the menu and the containing action references, making us
    // work on an already deleted model instance.
    Model::ActionRef localAction=action;

    if (localAction.Valid() && localAction->IsEnabled()) {
      localAction->Trigger();
    }
  }

  void ActionItem::PrepareForBackground(OS::DrawInfo* draw)
  {
    draw->disabled=(!action.Valid() || !action->IsEnabled());
    draw->selected=menuWindow->selected==this;
  }

  void ActionItem::Draw(OS::DrawInfo* draw,
                        int x, int y, size_t w, size_t h)
  {
    MenuEntry::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* --- */

    draw->disabled=(!action.Valid() || !action->IsEnabled());
    draw->selected=menuWindow->selected==this;

    DrawImage(draw);
    DrawLabel(draw);
    DrawShortcut(draw);

    draw->disabled=false;
    draw->selected=false;
  }

  /* ---------- Separator menu stuff ------------------*/

  bool SeparatorItem::SetModel(Base::Model* /*model*/)
  {
    return false;
  }

  void SeparatorItem::CalcSize()
  {
    height=OS::display->GetImage(OS::Display::menuDividerImageIndex)->GetHeight();
    height+=2*OS::display->GetSpaceVertical(OS::Display::spaceObjectBorder);

    MenuEntry::CalcSize();
  }

  void SeparatorItem::Draw(OS::DrawInfo* draw,
                           int x, int y, size_t w, size_t h)
  {
    MenuEntry::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* --- */

    OS::ImageRef divider=OS::display->GetImage(OS::Display::menuDividerImageIndex);

    divider->DrawStretched(draw,
                           this->x,this->y+(height-divider->GetHeight())/2,
                           this->width,divider->GetHeight());
  }

  /* ---------- Sub menu stuff ------------------*/

  SubMenu::SubMenu()
  : subMenu(NULL)
  {
    selectable=true;
    subImage=OS::display->GetImage(OS::Display::menuSubImageIndex);
  }

  SubMenu::~SubMenu()
  {
    delete subMenu;
  }

  bool SubMenu::SetModel(Base::Model* /*model*/)
  {
    return false;
  }

  void SubMenu::SetSubMenu(Menu* menu)
  {
    subMenu=menu;
    subMenu->parentMenu=this;
  }

  bool SubMenu::IsSelectable() const
  {
    return true;
  }

  void SubMenu::RegisterToParentWindow(OS::Window* parent)
  {
    MenuEntry::RegisterToParentWindow(parent);

    subMenu->SetParent(parent);
  }

  void SubMenu::CalcSize()
  {
    height=std::max(height,subImage->GetHeight()+2*OS::display->GetSpaceVertical(OS::Display::spaceObjectBorder));

    MenuEntry::CalcSize();
  }

  void SubMenu::PrepareForBackground(OS::DrawInfo* draw)
  {
    draw->selected=menuWindow->selected==this;
  }

  void SubMenu::Draw(OS::DrawInfo* draw,
                     int x, int y, size_t w, size_t h)
  {
    MenuEntry::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* --- */

    if (menuWindow->selected==this) {
      draw->selected=true;
    }

    if (menuWindow->selected==this) {
      if (!subMenu->IsOpen() && subMenu->Open()) {
        menuWindow->child=subMenu;
      }
    }
    else {
      if (subMenu->IsOpen()) {
        subMenu->Close();
        menuWindow->child=NULL;
      }
    }

    DrawLabel(draw);
    DrawSubImage(draw);

    draw->selected=false;
  }

  Menu* SubMenu::GetSubMenu() const
  {
    return subMenu;
  }

  /* ---------- Bool item stuff ------------------*/

  BoolItem::BoolItem()
  : model(NULL)
  {
    selectable=true;

    image=OS::display->GetImage(OS::Display::menuCheckImageIndex);
  }

  bool BoolItem::SetModel(Base::Model* model)
  {
    this->model=dynamic_cast<Model::Boolean*>(model);

    Control::SetModel(this->model);

    return this->model.Valid();
  }

  void BoolItem::OnSelection()
  {
    if (model.Valid() && model->IsEnabled()) {
      model->Toggle();
    }
  }

  void BoolItem::PrepareForBackground(OS::DrawInfo* draw)
  {
    draw->disabled=(!model.Valid() || !model->IsEnabled());
    draw->selected=menuWindow->selected==this;
  }

  void BoolItem::Draw(OS::DrawInfo* draw,
                      int x, int y, size_t w, size_t h)
  {
    MenuEntry::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* --- */

    draw->disabled=(!model.Valid() || !model->IsEnabled());

    /* Image */

    draw->selected=(model.Valid() && model->Get());
    DrawImage(draw);
    draw->selected=false;

    /* Rest */

    draw->selected=(menuWindow->selected==this);

    DrawLabel(draw);
    DrawShortcut(draw);

    draw->disabled=false;
    draw->selected=false;
  }

  /* ---------- Radio item stuff ------------------*/

  RadioItem::RadioItem()
  : model(NULL),
    value(0)
  {
    selectable=true;

    image=OS::display->GetImage(OS::Display::menuRadioImageIndex);
  }

  bool RadioItem::SetModel(Base::Model* model)
  {
    this->model=dynamic_cast<Model::Number*>(model);

    Control::SetModel(this->model);

    return this->model.Valid();
  }

  void RadioItem::SetValue(size_t value)
  {
    this->value=value;
  }

  void RadioItem::OnSelection()
  {
    if (model.Valid() && model->IsEnabled()) {
      model->Set(value);
    }
  }

  void RadioItem::PrepareForBackground(OS::DrawInfo* draw)
  {
    draw->disabled=(!model.Valid() || !model->IsEnabled());
    draw->selected=menuWindow->selected==this;
  }

  void RadioItem::Draw(OS::DrawInfo* draw,
                       int x, int y, size_t w, size_t h)
  {
    MenuEntry::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* --- */

    if (!model.Valid() || !model->IsEnabled()) {
      draw->disabled=true;
    }

    /* Image */

    draw->selected=(model.Valid() && model->GetUnsignedLong()==value);
    DrawImage(draw);
    draw->selected=false;

    /* Rest */

    draw->selected=(menuWindow->selected==this);

    DrawLabel(draw);
    DrawShortcut(draw);

    draw->disabled=false;
    draw->selected=false;
  }

  /* ---------- Menu box stuff ------------------*/

  /**
    Container for a list of menu entries.
  */
  class MenuBox : public List
  {
  public:
    MenuBox();

    void RegisterToParentWindow(OS::Window* parent);
    void CalcSize();
    void Layout();

    MenuEntry* GetSelected();
    MenuEntry* GetFirst() const;
    size_t     GetEntryCount() const;

    MenuEntry* GetPreviousSelectable(MenuEntry* entry);
    MenuEntry* GetNextSelectable(MenuEntry* entry);
    MenuEntry* GetShortcut(wchar_t sc);
  };

  MenuBox::MenuBox()
  {
    SetBackground(OS::display->GetFill(OS::Display::menuWindowBackgroundFillIndex));
  }

  void MenuBox::RegisterToParentWindow(OS::Window* parent)
  {
    std::list<Object*>::iterator iter;

    iter=list.begin();
    while (iter!=list.end()) {
      dynamic_cast<MenuEntry*>(*iter)->RegisterToParentWindow(parent);
      ++iter;
    }
  }

  void MenuBox::CalcSize()
  {
    size_t w1=0,w2=0,w3=0,w4=0;

    width=0;
    height=0;

    for (std::list<Object*>::iterator iter=list.begin(); iter!=list.end(); ++iter) {
      MenuEntry* entry=dynamic_cast<MenuEntry*>(*iter);

      entry->CalcSize();
      w1=std::max(w1,entry->w1);
      w2=std::max(w2,entry->w2);
      w3=std::max(w3,entry->w3);
      w4=std::max(w4,entry->w4);
      height+=entry->GetOHeight();
    }

    width=2*OS::display->GetSpaceHorizontal(OS::Display::spaceObjectBorder);

    // Image
    width+=w1;

    //Label
    if (w1!=0 && w2!=0) {
      width+=OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject);
    }
    width+=w2;

    // Shortcut
    if (w2!=0 && w3!=0) {
      width+=OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject);
    }
    width+=w3;

    // Sub Image
    if (w3!=0 && w4!=0) {
      width+=OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject);
    }
    width+=w4;

    for (std::list<Object*>::iterator iter=list.begin(); iter!=list.end(); ++iter) {
      MenuEntry* entry=dynamic_cast<MenuEntry*>(*iter);

      entry->w1=w1;
      entry->w2=w2;
      entry->w3=w3;
      entry->w4=w4;
    }

    minWidth=width;
    minHeight=height;

    List::CalcSize();
  }

  void MenuBox::Layout()
  {
    std::list<Object*>::iterator iter;
    int                          yPos;
    int                          x1,x2,x3,x4;
    size_t                       w1=0,w2=0,w3=0;

    if (list.size()>0) {
      MenuEntry* entry=dynamic_cast<MenuEntry*>(*list.begin());

      w1=entry->w1;
      w2=entry->w2;
      w3=entry->w3;
    }

    // Image
    x1=x+OS::display->GetSpaceHorizontal(OS::Display::spaceObjectBorder);

    //Label
    x2=x1+w1;
    if (w1!=0) {
      x2+=OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject);
    }

    // Shortcut
    x3=x2+w2;
    if (w2!=0) {
      x3+=OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject);
    }

    // SubImage
    x4=x3+w3;
    if (w3!=0) {
      x4+=OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject);
    }

    for (std::list<Object*>::iterator iter=list.begin(); iter!=list.end(); ++iter) {
      MenuEntry* entry=dynamic_cast<MenuEntry*>(*iter);

      entry->SetOffsets(x1,x2,x3,x4);
    }

    yPos=y;

    iter=list.begin();
    while (iter!=list.end()) {
      (*iter)->Move(x,yPos);
      (*iter)->Resize(width,(*iter)->GetOHeight()); // Does not work?

      yPos+=(*iter)->GetOHeight();

      ++iter;
    }

    List::Layout();
  }

  /**
    Get menu entry the mouse currently is over or NULL.
  */
  MenuEntry* MenuBox::GetSelected()
  {
    std::list<Object*>::iterator entry;
    int                            x,y;

    GetWindow()->GetMousePos(x,y);

    if (x>=0 && x<(int)GetWindow()->GetWidth()) {
      entry=list.begin();
      while (entry!=list.end()) {
        if (y>=(*entry)->GetOY() && y<=(*entry)->GetOY()+(int)(*entry)->GetOHeight()-1) {
          return dynamic_cast<MenuEntry*>(*entry);
        }

        ++entry;
      }
    }

    return NULL;
  }

  MenuEntry* MenuBox::GetFirst() const
  {
    if (list.begin()==list.end()) {
      return NULL;
    }
    else {
      return dynamic_cast<MenuEntry*>(*list.begin());
    }
  }

  size_t MenuBox::GetEntryCount() const
  {
    return list.size();
  }

  MenuEntry* MenuBox::GetPreviousSelectable(MenuEntry* entry)
  {
    std::list<Object*>::reverse_iterator end,current;

    if (list.size()==0) {
      return NULL;
    }

    if (entry==NULL) {
      end=list.rend();
      current=list.rbegin();

    }
    else {
      end=list.rbegin();
      while (*end!=entry) {
        ++end;
      }
      current=end;
      ++current;
    }

    while (current!=end) {
      if (current!=list.rend()) {
        if (dynamic_cast<MenuEntry*>(*current)->IsSelectable()) {
          return dynamic_cast<MenuEntry*>(*current);
        }

        ++current;
      }
      else {
        current=list.rbegin();
      }
    }

    return entry;
  }

  MenuEntry* MenuBox::GetNextSelectable(MenuEntry* entry)
  {
    std::list<Object*>::iterator end,current;

    if (list.size()==0) {
      return NULL;
    }

    if (entry==NULL) {
      end=list.end();
      current=list.begin();

    }
    else {
      end=list.begin();
      while (*end!=entry) {
        ++end;
      }
      current=end;
      ++current;
    }

    while (current!=end) {
      if (current!=list.end()) {
        if (dynamic_cast<MenuEntry*>(*current)->IsSelectable()) {
          return dynamic_cast<MenuEntry*>(*current);
        }

        ++current;
      }
      else {
        current=list.begin();
      }
    }

    return entry;
  }

  MenuEntry* MenuBox::GetShortcut(wchar_t sc)
  {
    std::list<Object*>::iterator iter;

    iter=list.begin();
    while (iter!=list.end())  {
      if (toupper(dynamic_cast<MenuEntry*>(*iter)->sc)==toupper(sc)) {
        return dynamic_cast<MenuEntry*>(*iter);
      }

      ++iter;
    }

    return NULL;
  }

  /* ---------- Menu stuff ------------------*/

  Menu::Menu()
  : window(OS::driver->CreateWindow()),
    parent(NULL),
    popup(true),
    registered(false),
    reference(NULL),
    manualPos(false),
    xPos(0),
    yPos(0),
    parentMenu(NULL),
    child(NULL),
    selected(NULL),
    strip(NULL)
  {
    window->SetMaster(this);

    top=new MenuBox();
    top->SetWindow(window);

    Observe(window->GetUnmapedAction());
    Observe(window->GetHiddenAction());
    Observe(window->GetDrawAction());
    Observe(window->GetMouseOutAction());
    Observe(window->GetPreInitAction());
    Observe(window->GetEventAction());
  }

  Menu::~Menu()
  {
    delete top;
    delete window;
  }

  bool Menu::IsOpen() const
  {
    return window->IsOpen();
  }

  bool Menu::CursorIsIn() const
  {
    return window->CursorIsIn();
  }

  /**
    Private. This method is overloaded to get the parent window once.
    We need the parent window to register our shortcuts.
  */
  void Menu::SetParent(OS::Window* parent)
  {
    if (!registered) {
      top->RegisterToParentWindow(parent);
      registered=true;
      this->parent=parent;
      window->SetParent(parent);
    }
  }

  void Menu::SetReference(Lum::Object* reference)
  {
    this->reference=reference;
  }

  void Menu::SetPos(int x, int y)
  {
    manualPos=true;
    xPos=x;
    yPos=y;
  }

  bool Menu::VisitChildren(Visitor &visitor, bool onlyVisible)
  {
    return top->VisitChildren(visitor,onlyVisible);
  }

  /**
    Initializes an empty menu.
  */
  void Menu::Add(MenuEntry* entry)
  {
    entry->SetFlex(true,false);
    entry->SetWindow(window);
    entry->menuWindow=this;

    top->Add(entry);
  }

  void Menu::CalcSize()
  {
    top->CalcSize();

    window->SetSize(top->GetOWidth(),top->GetOHeight());
  }

  Lum::Object* Menu::GetObjectByName(const std::wstring& /*name*/)
  {
    return NULL; //return top->GetObjectByName(name);
  }

  /**
    Get menu entry the mouse currently is over or NULL.
  */
  MenuEntry* Menu::GetSelected()
  {
    return top->GetSelected();
  }

  /**
    Return menu strip or @code{NULL}, if there is no menu strip (popup menu).
  */
  MenuStrip* Menu::GetStrip()
  {
    return GetTop()->strip;
  }

  /**
    Return top most menu.
  */
  Menu* Menu::GetTop()
  {
    Menu *menu=this;

    while (menu->parentMenu!=NULL) {
      menu=menu->parentMenu->menuWindow;
    }

    return menu;
  }

  Menu* Menu::GetMenuWithSelection()
  {
    Menu *menu=GetTop();

    while (menu!=NULL &&
           menu->IsOpen() &&
           menu->selected!=NULL &&
           menu->selected->GetSubMenu()!=NULL &&
           menu->selected->GetSubMenu()->selected!=NULL) {
      menu=menu->selected->GetSubMenu();
    }

    return menu;
  }

  Menu* Menu::GetMenuWithMouseSelection()
  {
    Menu *menu=GetTop();

    while (menu!=NULL &&
           menu->IsOpen() &&
           menu->selected!=NULL &&
           !menu->CursorIsIn()) {
      menu=menu->selected->GetSubMenu();
    }

    return menu;
  }

  void Menu::PreInit()
  {
    OS::Window *help;
    int        rx,ry,x,y;

    CalcSize();

    if (strip!=NULL) {
      window->SetType(OS::Window::typeMenuDropdown);
    }
    else {
      window->SetType(OS::Window::typeMenuPopup);
    }

    selected=NULL;
    if (manualPos) {
      x=xPos;
      y=yPos;
    }
    else if (strip!=NULL && reference!=NULL) {
      help=reference->GetWindow();
      x=help->GetX();
      y=help->GetY();
      x+=reference->GetOX();
      y+=reference->GetOY()+reference->GetOHeight();
    }
    else if (parentMenu!=NULL) { /* submenu */
      x=parentMenu->GetWindow()->GetX()+parentMenu->GetWindow()->GetWidth()+
        OS::display->GetTheme()->GetSubMenuHorizontalOffset();
      y=parentMenu->GetWindow()->GetY()+parentMenu->GetOY()-top->GetTopBorder();
    }
    else if (parent!=NULL) {
      OS::display->GetMousePos(rx,ry);
      if (popup) {
        x=rx;//+OS::display->GetSpaceHorizontal(OS::Display::spaceLabelObject);
        y=ry;//+OS::display->GetSpaceVertical(OS::Display::spaceLabelObject);
      }
      else {
        x=rx-selected->GetOX()-selected->GetOWidth() / 2;
        y=ry-selected->GetOY()-selected->GetOHeight() / 2;
      }
    }
    else {
      x=window->GetX();
      y=window->GetY();
    }

    x=Base::RoundRange(x,0,(int)(OS::display->GetScreenWidth()-1-window->GetWidth()));
    y=Base::RoundRange(y,0,(int)(OS::display->GetScreenHeight()-1-window->GetHeight()));

    window->SetPos(x,y);
    window->SetPosition(OS::Window::positionManual,OS::Window::positionManual);
  }

  bool Menu::Open()
  {
    if ((strip!=NULL && reference!=NULL) ||
        (strip==NULL && parentMenu==NULL)) {
      window->GrabMouse(true);
      window->GrabKeyboard(true);
    }

    return window->Open(true);
  }

  bool Menu::OpenInStickyMode()
  {
    bool result;

    if ((strip!=NULL && reference!=NULL) ||
        (strip==NULL && parentMenu==NULL)) {
      window->GrabMouse(true);
      window->GrabKeyboard(true);
    }

    if (window->IsOpen()) {
      result=window->Show(true);
    }
    else {
      result=window->Open(true);
    }

    if (result) {
      stickyMode=true;
    }

    return result;
  }

  /**
    Close menu.
  */
  void Menu::Close()
  {
    if (child!=NULL) {
      child->Close();
    }
    if (parentMenu!=NULL) {
      parentMenu->menuWindow->child=NULL;
    }

    window->Close();
  }

  /**
    Close current and all parent menues.
  */
  void Menu::CloseAll()
  {
    Menu      *current;
    MenuStrip *strip=GetStrip();

    current=this;
    while (current!=NULL) {
      current->Close();
      if (current->parentMenu!=NULL) {
        current=current->parentMenu->menuWindow;
      }
      else {
        current=NULL;
      }
    }

    if (strip!=NULL) {
      strip->SetSelection(NULL);
    }
  }

  /**
    Close down menu and finish menu selection.
  */
  void Menu::EndAction()
  {
    MenuStrip *strip;

    strip=GetStrip();

    if (strip!=NULL) {
      strip->stickyMode=false;
    }

    CloseAll();
  }

  void Menu::Select(MenuEntry* entry)
  {
    MenuEntry* old;

    old=selected;
    if (entry!=old) {
      if (entry!=NULL && !entry->IsSelectable()) {
        selected=NULL;
      }
      else {
        selected=entry;
      }

      if (old!=NULL) {
        old->Redraw();
      }

      if (selected!=NULL) {
        selected->Redraw();
      }
    }
  }

  /**
    Select next menu item in menu.
  */
  void Menu::SelectDown()
  {
    Menu *menu;

    menu=GetMenuWithSelection();

    if (menu->top->GetEntryCount()==0 || (menu->top->GetEntryCount()==1 && menu->selected==menu->top->GetFirst())) {
      /* nothing to do */
      return;
    }

    menu->Select(menu->top->GetNextSelectable(menu->selected));
  }

  /**
    Select previous menu item in menu.
  */
  void Menu::SelectUp()
  {
    Menu *menu;

    menu=GetMenuWithSelection();

    if (menu->top->GetEntryCount()==0 || (menu->top->GetEntryCount()==1 && menu->selected==menu->top->GetFirst())) {
      /* nothing to do */
      return;
    }

    menu->Select(menu->top->GetPreviousSelectable(menu->selected));
  }

  /**
   Select parent menu if available or previous entry in menu strip.
  */
  void Menu::SelectLeft()
  {
    Menu *menu;

    menu=GetMenuWithSelection();

    if (menu->parentMenu!=NULL) {
      menu->Select(NULL);
    }
    else {
      MenuStrip* strip;

      strip=GetStrip();
      if (strip!=NULL) {
        strip->SelectPrevious();
      }
    }
  }

  /**
    Select sub menu if available or next entry in menu strip.
  */
  void Menu::SelectRight()
  {
    Menu *menu;

    menu=GetMenuWithSelection();

    if (menu->selected!=NULL && menu->selected->GetSubMenu()!=NULL) {
      menu=menu->selected->GetSubMenu();
      menu->Select(menu->top->GetNextSelectable(NULL));
      //menu->selected->GetSubMenu()->SelectDown();
    }
    else {
      MenuStrip* strip;

      strip=GetStrip();
      if (strip!=NULL) {
        strip->SelectNext();
      }
    }
  }

  /**
    Select menu item with the given hotkey.
  */
  void Menu::SelectKey(wchar_t key)
  {
    Menu *menu;

    menu=GetMenuWithSelection();

    if (menu!=NULL) {
      MenuEntry* entry;

      entry=menu->top->GetShortcut(key);

      if (entry!=NULL) {
        if (entry->IsSelectable()) {
          menu->EndAction();
          entry->OnSelection();
        }
        else {
          menu->Select(entry);
        }
      }
    }
  }

  /**
    Call OnSelection onthe current selection.
  */
  void Menu::ActivateSelection()
  {
    Menu      *menu;
    MenuEntry *entry;

    menu=GetMenuWithSelection();
    entry=menu->selected;

    if (entry->IsSelectable()) {
      menu->EndAction();
      entry->OnSelection();
    }
  }


  /* ---------------------------------- */

  bool Menu::HandleEvent(OS::Event* event)
  {
    Menu           *menu;
    Object         *object;
    MenuStrip      *strip;
    OS::MouseEvent *mouse;
    OS::KeyEvent   *key;

    if ((mouse=dynamic_cast<OS::MouseEvent*>(event))!=NULL) {
      if (mouse->type==OS::MouseEvent::up &&
          (mouse->button==OS::MouseEvent::button3 ||
           mouse->button==OS::MouseEvent::button1)) {
        menu=GetMenuWithMouseSelection();

        if (menu!=NULL && menu->CursorIsIn()) {
          menu->Select(menu->GetSelected());

          if (menu->selected!=NULL) {
            EndAction();
            menu->selected->OnSelection();
          }
        }
        else {
          /* No menu item selected */
          strip=GetStrip();
          if (strip!=NULL) {
            // Transform menu coordinates to strip coordinates (in another window)
            mouse->x=mouse->x+GetWindow()->GetX()-strip->GetWindow()->GetX();
            mouse->y=mouse->y+GetWindow()->GetY()-strip->GetWindow()->GetY();
            strip->EvaluateMouseUp(*mouse);

            if (!strip->stickyMode) {
              EndAction();
            }
          }
          else if (!stickyMode) {
            EndAction();
          }
        }
      }
      else if (mouse->type==OS::MouseEvent::down &&
               (mouse->button==OS::MouseEvent::button3 ||
                mouse->button==OS::MouseEvent::button1)) {
        menu=GetMenuWithMouseSelection();

        if (menu==NULL || !menu->CursorIsIn()) {
          EndAction();
        }
      }
      else if (mouse->type==OS::MouseEvent::move) {
        /*
          We got a motion event. We first go to the top of the menu tree
          and then walk down the menu selection tree until we
          find a menu the mouse is currently over.
        */
        menu=GetMenuWithMouseSelection();

        if (menu!=NULL) { /* We found a submenu */
          if (menu->CursorIsIn()) {
            /*
              Select the select entry
            */
            menu->Select(menu->GetSelected());
          }
          else if (this->strip!=NULL) {
            // Transform to screen coordinates
            mouse->x+=window->GetX();
            mouse->y+=window->GetY();

            // Transform to strip window coordinates
            mouse->x-=this->strip->GetWindow()->GetX();
            mouse->y-=this->strip->GetWindow()->GetY();

            /*
              If the mouse is over the strip, check if a new pulldown
              menu has been selected. If this is the case, close the old one.
            */
            if (this->strip->PointIsIn(*mouse)) {
              object=this->strip->GetMouseSelected(*mouse);
              if (object!=NULL && reference!=object) {
                CloseAll();
              }
            }
          }
        }
        else {
          /*
            The mouse is currently over no menu. Now, find the leaf of the current
            menu selection tree.
          */
          menu=GetMenuWithSelection();

          if (menu!=NULL &&
              menu->selected!=NULL &&
              menu->selected->GetSubMenu()==NULL) {
            /*
              Deselect the last selected entry, because it is not
              selected by the mouse anymore.

              This code sequence is only necessary for driver that do not handle
              OnMouseLeft!
            */
            menu->Select(NULL);
          }
        }
      }
    }
    else if ((key=dynamic_cast<OS::KeyEvent*>(event))!=NULL &&
             key->type==OS::KeyEvent::down) {
      switch (key->key) {
      case OS::keyEscape:
        EndAction();
        break;
      case OS::keyReturn:
        ActivateSelection();
        break;
      case OS::keyUp:
        SelectUp();
        break;
      case OS::keyDown:
        SelectDown();
        break;
      case OS::keyLeft:
        SelectLeft();
        break;
      case OS::keyRight:
        SelectRight();
        break;
      default:
        if (key->text[0]!='\0') {
          SelectKey(key->text[0]);
        }
        break;
      }
    }

    return true;
  }

  void Menu::Resync(Lum::Base::Model* model,
                    const Lum::Base::ResyncMsg& msg)
  {
    if (model==window->GetUnmapedAction() &&
        window->GetUnmapedAction()->IsFinished()) {
      top->Hide();
    }
    else if (model==window->GetHiddenAction() &&
             window->GetHiddenAction()->IsFinished()) {
      CloseAll();
    }
    else if (model==window->GetMouseOutAction() &&
             window->GetMouseOutAction()->IsFinished()) {
      if (selected!=NULL && selected->GetSubMenu()==NULL) {
        MenuEntry *oldSelected;

        oldSelected=selected;
        selected=NULL;
        oldSelected->Redraw();
      }
    }
    else if (model==window->GetDrawAction() &&
             window->GetDrawAction()->IsFinished()) {
      const OS::Window::DrawMsg *drawMsg=dynamic_cast<const OS::Window::DrawMsg*>(&msg);

      assert(drawMsg->draw!=NULL);

      top->Move(0,0);
      top->Layout();
      top->DrawBackground(drawMsg->draw,
                          drawMsg->x,
                          drawMsg->y,
                          drawMsg->width,
                          drawMsg->height);
      top->Draw(drawMsg->draw,
                drawMsg->x,
                drawMsg->y,
                drawMsg->width,
                drawMsg->height);
    }
    else if (model==window->GetPreInitAction() &&
             window->GetPreInitAction()->IsFinished()) {
      stickyMode=false;
      PreInit();
    }
    else if (model==window->GetEventAction() &&
             window->GetEventAction()->IsFinished()) {
      const OS::Window::EventMsg *eventMsg=dynamic_cast<const OS::Window::EventMsg*>(&msg);

      HandleEvent(eventMsg->event);
    }
  }

  OS::Window* Menu::GetWindow() const
  {
    return window;
  }

  void Menu::AddActionItem(const std::wstring& label,
                           Model::Action* action,
                           unsigned long qualifier,
                           const std::wstring& key)
  {
    ActionItem *item;

    item=new ActionItem();
    item->SetTextLabel(label);

    if (!key.empty()) {
      item->SetShortcut(qualifier,key);
    }
    item->SetModel(action);

    Add(item);
  }

  void Menu::AddBoolItem(const std::wstring& label,
                         Model::Boolean* model,
                         unsigned long qualifier,
                         const std::wstring& key)
  {
    BoolItem *item;

    item=new BoolItem();
    item->SetTextLabel(label);

    if (!key.empty()) {
      item->SetShortcut(qualifier,key);
    }
    item->SetModel(model);

    Add(item);
  }

  void Menu::AddRadioItem(const std::wstring& label,
                         Model::Number* model,
                         size_t value,
                         unsigned long qualifier,
                         const std::wstring& key)
  {
    RadioItem *item;

    item=new RadioItem();
    item->SetTextLabel(label);

    if (!key.empty()) {
      item->SetShortcut(qualifier,key);
    }
    item->SetModel(model);
    item->SetValue(value);

    Add(item);
  }

  void Menu::AddSeparator()
  {
    Add(new SeparatorItem());
  }

  void Menu::AddSubMenu(Lum::Object* label, Menu* subMenu)
  {
    SubMenu* sub;

    sub=new SubMenu();
    sub->SetLabel(label);
    sub->SetSubMenu(subMenu);

    Add(sub);
  }

  void Menu::AddSubMenu(const std::wstring& label, Menu* subMenu)
  {
    SubMenu* sub;

    sub=new SubMenu();
    sub->SetTextLabel(label);
    sub->SetSubMenu(subMenu);

    Add(sub);
  }
}

