#ifndef LUM_DIRSELECT_H
#define LUM_DIRSELECT_H

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

#include <vector>

#include <Lum/Model/Action.h>
#include <Lum/Model/Path.h>

#include <Lum/Object.h>

namespace Lum {
  class LUMAPI DirSelect : public Control
  {
  private:
    class Item
    {
    public:
      std::wstring label;
    };

  private:
    Model::PathRef                model;
    std::vector<Item>             items;
    std::vector<Object*>          objects;
    std::vector<Model::ActionRef> models;
    Object                        *templ;

  private:
    Object* CreateButton();
    void GenerateItemList();

  public:
    DirSelect();
    ~DirSelect();

    bool SetModel(Base::Model* model);

    bool VisitChildren(Visitor &visitor, bool onlyVisible);

    void CalcSize();
    bool HandleMouseEvent(const OS::MouseEvent& event);
    void Layout();
    void PreDrawChilds(OS::DrawInfo* draw);
    void PostDrawChilds(OS::DrawInfo* draw);

    void Resync(Base::Model* model, const Base::ResyncMsg& msg);
  };
}

#endif
