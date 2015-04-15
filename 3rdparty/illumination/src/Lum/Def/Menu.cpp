/*
  This source is part of the Illumination library
  Copyright (C) 2009  Tim Teulings

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

#include <Lum/Base/L10N.h>

#include <Lum/OS/Event.h>

namespace Lum {
  namespace Def {

    MenuItem::MenuItem()
    {
      // no code
    }

    MenuItem::~MenuItem()
    {
      // no code
    }

    MenuAction::MenuAction(const Action& action)
    : action(action)
    {
      // no code
    }

    const Action& MenuAction::GetAction() const
    {
      return action;
    }

    MenuBoolean::MenuBoolean(const Boolean& boolean)
    : boolean(boolean)
    {
      // no code
    }

    const Boolean& MenuBoolean::GetBoolean() const
    {
      return boolean;
    }

    MenuOneOfN::MenuOneOfN(const OneOfN& oneOfN)
    : oneOfN(oneOfN)
    {
      // no code
    }

    const OneOfN& MenuOneOfN::GetOneOfN() const
    {
      return oneOfN;
    }

    MenuSeparator::MenuSeparator()
    {
      // no code
    }

    Menu::Menu(Menu* parent, const Desc& description)
    : parent(parent),
    description(description)
    {
      // no code
    }

    Menu::Menu()
    : parent(NULL)
    {
      // no code
    }

    Menu::Menu(const Menu& other)
    {
      parent=NULL;
      description=other.description;

      for (std::list<MenuItem*>::const_iterator item=other.items.begin();
           item!=other.items.end();
           ++item) {
        if (dynamic_cast<MenuAction*>(*item)!=NULL) {
          items.push_back(new MenuAction(*dynamic_cast<MenuAction*>(*item)));
        }
        else if (dynamic_cast<MenuBoolean*>(*item)!=NULL) {
          items.push_back(new MenuBoolean(*dynamic_cast<MenuBoolean*>(*item)));
        }
        else if (dynamic_cast<MenuOneOfN*>(*item)!=NULL) {
          items.push_back(new MenuOneOfN(*dynamic_cast<MenuOneOfN*>(*item)));
        }
        else if (dynamic_cast<MenuSeparator*>(*item)!=NULL) {
          items.push_back(new MenuSeparator(*dynamic_cast<MenuSeparator*>(*item)));
        }
        else if (dynamic_cast<Menu*>(*item)!=NULL) {
          Menu *menu=new Menu(*dynamic_cast<Menu*>(*item));

          menu->parent=this;

          items.push_back(menu);
        }
      }
    }

    Menu::~Menu()
    {
      for (std::list<MenuItem*>::iterator item=items.begin();
           item!=items.end();
           ++item) {
        delete *item;
      }
    }


    Menu* Menu::Action(const Def::Action& action)
    {
      items.push_back(new MenuAction(action));

      return this;
    }

    Menu* Menu::Boolean(const Def::Boolean& boolean)
    {
      items.push_back(new MenuBoolean(boolean));

      return this;
    }

    Menu* Menu::OneOfN(const Def::OneOfN& oneOfN)
    {
      items.push_back(new MenuOneOfN(oneOfN));

      return this;
    }

    Menu* Menu::Separator()
    {
      items.push_back(new MenuSeparator());

      return this;
    }

    Menu* Menu::Group(const Desc& description)
    {
      Menu *menu=new Menu(this,description);

      items.push_back(menu);

      return menu;
    }

    Menu* Menu::Group(const std::wstring& name)
    {
      Menu *menu=new Menu(this,Desc(name));

      items.push_back(menu);

      return menu;
    }

    Menu* Menu::End()
    {
      return parent;
    }

    Menu* Menu::Append(Menu* menu)
    {
      assert(menu!=NULL);

      for (std::list<MenuItem*>::const_iterator item=menu->items.begin();
           item!=menu->items.end();
           ++item) {
        if (dynamic_cast<MenuAction*>(*item)!=NULL) {
          items.push_back(new MenuAction(*dynamic_cast<MenuAction*>(*item)));
        }
        else if (dynamic_cast<MenuBoolean*>(*item)!=NULL) {
          items.push_back(new MenuBoolean(*dynamic_cast<MenuBoolean*>(*item)));
        }
        else if (dynamic_cast<MenuOneOfN*>(*item)!=NULL) {
          items.push_back(new MenuOneOfN(*dynamic_cast<MenuOneOfN*>(*item)));
        }
        else if (dynamic_cast<MenuSeparator*>(*item)!=NULL) {
          items.push_back(new MenuSeparator(*dynamic_cast<MenuSeparator*>(*item)));
        }
        else if (dynamic_cast<Menu*>(*item)!=NULL) {
          Menu *menu=new Menu(*dynamic_cast<Menu*>(*item));

          menu->parent=this;

          items.push_back(menu);
        }
      }

      return this;
    }

    const Desc& Menu::GetDesc() const
    {
      return description;
    }

    Menu* Menu::GroupProject()
    {
      return Group(_ld(menuProject));
    }

    Menu* Menu::GroupEdit()
    {
      return Group(_ld(menuEdit));
    }

    Menu* Menu::GroupHelp()
    {
      return Group(_ld(menuHelp));
    }

    Menu* Menu::ActionQuit(Lum::Model::Action *action)
    {
      return Action(Lum::Def::Action(Lum::Def::Desc(_ld(menuProjectQuit))
                                     .SetShortcut(Lum::OS::qualifierControl,L"q"),
                                     action));
    }

    Menu* Menu::ActionSettings(Lum::Model::Action *action)
    {
      return Action(Lum::Def::Action(Lum::Def::Desc(_ld(menuEditSettings)),
                                     action).SetOpensDialog());
    }

    Menu* Menu::ActionHelp(Lum::Model::Action *action)
    {
      return Action(Lum::Def::Action(Lum::Def::Desc(_ld(menuHelpHelp)),action).SetOpensDialog());
    }

    Menu* Menu::ActionAbout(Lum::Model::Action *action)
    {
      return Action(Lum::Def::Action(Lum::Def::Desc(_ld(menuHelpAbout)),action).SetOpensDialog());
    }

    const std::list<MenuItem*>& Menu::GetItems() const
    {
      return items;
    }

    Menu* Menu::Create()
    {
      return new Menu();
    }
  }
}

