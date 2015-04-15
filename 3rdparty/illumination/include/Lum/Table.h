#ifndef LUM_TABLE_H
#define LUM_TABLE_H

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

#include <Lum/Model/Action.h>
#include <Lum/Model/Header.h>
#include <Lum/Model/Selection.h>

#include <Lum/Object.h>
#include <Lum/Header.h>
#include <Lum/KineticScroller.h>
#include <Lum/TableView.h>

namespace Lum {

  class LUMAPI Table : public Group
  {
  private:
    Object           *object;
    Control          *hScroller;
    Control          *vScroller;
    KineticScroller  *kineticScroller;

    bool             hVisible;
    bool             vVisible;

    TableView        *table;
    Header           *header;
    Model::HeaderRef headerModel;
    bool             showHeader;

  public:
    Table();
    ~Table();

    TableView* GetTableView() const;
    void SetTableView(TableView* tableView);

    Lum::Object* GetMouseGrabFocusObject() const;
    
    bool VisitChildren(Visitor &visitor, bool onlyVisible);

    Model::Header* GetHeaderModel() const;
    void SetHeaderModel(Model::Header* headerModel);

    void SetShowHeader(bool show);
    void SetAutoFitColumns(bool autoFit);
    void SetAutoHSize(bool autoSize);
    void SetAutoVSize(bool autoSize);
    void SetRowHeight(size_t height);

    bool SetModel(Base::Model* model);
    void SetSelection(Model::Selection* selection);
    void SetDoubleClickAction(Model::Action* action);
    void SetMouseSelectionAction(Model::Action* action);
    void SetDefaultAction(Model::Action* action);
    void SetPainter(TableCellPainter* painter);

    void CalcSize();
    void Layout();

    size_t GetHorizontalScrollerHeight() const;
    size_t GetVerticalScrollerWidth() const;

    bool InterceptMouseEvents() const;
    bool HandleMouseEvent(const OS::MouseEvent& event);
    void Resync(Base::Model* model, const Base::ResyncMsg& msg);
  };
}

#endif
