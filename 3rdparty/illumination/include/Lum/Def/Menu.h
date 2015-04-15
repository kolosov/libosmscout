#ifndef LUM_DEF_MENU_H
#define LUM_DEF_MENU_H

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

#include <Lum/Def/Action.h>
#include <Lum/Def/Boolean.h>
#include <Lum/Def/Desc.h>
#include <Lum/Def/OneOfN.h>

namespace Lum {
  namespace Def {

    class LUMAPI MenuItem
    {
    protected:
      MenuItem();

    public:
      virtual ~MenuItem();
    };

    class LUMAPI MenuAction : public MenuItem
    {
    private:
      Action action;

    public:
      MenuAction(const Action& action);

      const Action& GetAction() const;
    };

    class LUMAPI MenuBoolean : public MenuItem
    {
    private:
      Boolean boolean;

    public:
      MenuBoolean(const Boolean& boolean);

      const Boolean& GetBoolean() const;
    };

    class LUMAPI MenuOneOfN : public MenuItem
    {
    private:
      OneOfN oneOfN;

    public:
      MenuOneOfN(const OneOfN& oneOfN);

      const OneOfN& GetOneOfN() const;
    };

    class LUMAPI MenuSeparator : public MenuItem
    {
    public:
      MenuSeparator();
    };

    class LUMAPI Menu : public MenuItem
    {
    private:
      Menu                 *parent;
      Desc                 description;
      std::list<MenuItem*> items;

    private:
      Menu(Menu* parent,const Desc& description);

    public:
      Menu();
      Menu(const Menu& other);
      ~Menu();

      // Base

      Menu* Action(const Def::Action& action);
      Menu* Boolean(const Def::Boolean& boolean);
      Menu* OneOfN(const Def::OneOfN& oneOfN);
      Menu* Separator();
      Menu* Group(const Desc& description);
      Menu* Group(const std::wstring& name);
      Menu* End();
      Menu* Append(Menu* menu);

      // Helper
      Menu* GroupProject();
      Menu* GroupEdit();
      Menu* GroupHelp();

      Menu* ActionQuit(Lum::Model::Action *action);
      Menu* ActionSettings(Lum::Model::Action *action);
      Menu* ActionHelp(Lum::Model::Action *action);
      Menu* ActionAbout(Lum::Model::Action *action);

      const std::list<MenuItem*>& GetItems() const;

      const Desc& GetDesc() const;

      static Menu* Create();
    };
  }
}

#endif
