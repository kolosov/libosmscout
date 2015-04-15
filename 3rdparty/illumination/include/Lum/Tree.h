#ifndef LUM_TREE_H
#define LUM_TREE_H

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

#include <Lum/Base/Size.h>

#include <Lum/Model/Action.h>
#include <Lum/Model/Tree.h>

#include <Lum/OS/Image.h>
#include <Lum/OS/Font.h>

#include <Lum/Object.h>

namespace Lum {

  /**
  */
  class LUMAPI Tree : public Scrollable
  {
  private:
    OS::FontRef       font;
    Model::TreeRef    model;
    Model::Tree::Item *selected;
    size_t            colHeight;
    Model::ActionRef  selectionChangedAction;
    Model::ActionRef  doubleClickAction;

  private:
    bool GetClickedEntryPos(int y, size_t& pos);
    void DrawItem(OS::DrawInfo* draw, Model::Tree::Item* item, size_t y, size_t offset);

  public:
    Tree();
    ~Tree();

    bool SetModel(Base::Model* model);

    void SetSelectionChangedAction(Lum::Model::Action* action);
    Model::Action* GetSelectionChangedAction() const;

    void SetDoubleClickAction(Model::Action* action);
    Model::Action* GetDoubleClickAction() const;

    void SetSelection(Model::Tree::Item* item);
    Model::Tree::Item* GetSelection() const;

    void CalcSize();
    void GetDimension(size_t& width, size_t& height);
    void Layout();
    void Draw(OS::DrawInfo* draw,
              int x, int y, size_t w, size_t h);

    bool HandleMouseEvent(const OS::MouseEvent& event);
    bool HandleKeyEvent(const OS::KeyEvent& event);

    void Resync(Base::Model* model, const Base::ResyncMsg& msg);
  };
}

#endif

