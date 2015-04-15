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

#include <Lum/Table.h>

#include <Lum/Manager/Behaviour.h>

#include <Lum/Panel.h>
#include <Lum/PosMarker.h>
#include <Lum/Scroller.h>

namespace Lum {

  Table::Table()
  : object(NULL),
    hScroller(NULL),vScroller(NULL),
    kineticScroller(new KineticScroller()),
    hVisible(false),vVisible(false),
    table(new TableView()),
    header(new Header()),headerModel(new Model::HeaderImpl()),showHeader(false)
  {
    SetBackground(OS::display->GetFill(OS::Display::scrolledBackgroundFillIndex));

    headerModel->AddColumn(L"",Base::Size::pixel,32000,true);

    Observe(table->GetHAdjustment()->GetTopModel());
    Observe(table->GetVAdjustment()->GetTopModel());
    Observe(table->GetHAdjustment()->GetTotalModel());
    Observe(table->GetVAdjustment()->GetTotalModel());
  }

  Table::~Table()
  {
    delete kineticScroller;
    delete object;
    delete hScroller;
    delete vScroller;
  }

  Lum::Object* Table::GetMouseGrabFocusObject() const
  {
    return table;
  }
  
  bool Table::VisitChildren(Visitor &visitor, bool onlyVisible)
  {
    if (hScroller!=NULL && (!onlyVisible || hVisible)) {
      if (!visitor.Visit(hScroller)) {
        return false;
      }
    }

    if (vScroller!=NULL && (!onlyVisible || vVisible)) {
      if (!visitor.Visit(vScroller)) {
        return false;
      }
    }

    if (object!=NULL) {
      if (!visitor.Visit(object)) {
        return false;
      }
    }

    return true;
  }

  TableView* Table::GetTableView() const
  {
    return table;
  }

  /**
    Assign a custom instance for the internal table object.

    You must call SetModel() after you have called SetTableView() otherwise
    model assignment will not work.
  */
  void Table::SetTableView(TableView* tableView)
  {
    assert(tableView!=NULL);

    if (table!=NULL) {
      delete table;
    }

    table=tableView;
  }

  Model::Header* Table::GetHeaderModel() const
  {
    return headerModel.Get();
  }

  void Table::SetHeaderModel(Model::Header* headerModel)
  {
    assert(!IsVisible());

    this->headerModel=headerModel;
  }

  void Table::SetShowHeader(bool show)
  {
    showHeader=show;
  }

  void Table::SetAutoFitColumns(bool autoFit)
  {
    table->SetAutoFitColumns(autoFit);
  }

  void Table::SetAutoHSize(bool autoSize)
  {
    table->SetAutoHSize(autoSize);
  }

  void Table::SetAutoVSize(bool autoSize)
  {
    table->SetAutoVSize(autoSize);
  }

  void Table::SetRowHeight(size_t height)
  {
    table->SetRowHeight(height);
  }

  bool Table::SetModel(Base::Model* model)
  {
    return table->SetModel(model);
  }

  void Table::SetSelection(Model::Selection* selection)
  {
    table->SetSelection(selection);
  }

  void Table::SetDoubleClickAction(Model::Action* action)
  {
    table->SetDoubleClickAction(action);
  }

  void Table::SetMouseSelectionAction(Model::Action* action)
  {
    table->SetMouseSelectionAction(action);
  }

  void Table::SetDefaultAction(Model::Action* action)
  {
    table->SetDefaultAction(action);
  }

  void Table::SetPainter(TableCellPainter* painter)
  {
    table->SetPainter(painter);
  }

  void Table::CalcSize()
  {
    if (!inited) {
      table->SetFlex(true,true);
      table->SetHeader(headerModel);
      table->SetScrollView(this);
      kineticScroller->SetScrollable(table);
    }

    if (!inited) {
      header->SetFlex(true,false);
      header->SetModel(headerModel);
      header->SetAdjustment(table->GetHAdjustment());
    }

    if (object==NULL) {
      object=new VPanel();
      object->SetParent(this);
      object->SetFlex(true,true);
      object->SetBackground(OS::display->GetFill(OS::Display::listboxBackgroundFillIndex));

      if (showHeader) {
        dynamic_cast<VPanel*>(object)->Add(header);
      }

      dynamic_cast<VPanel*>(object)->Add(table);
    }


    if (hScroller==NULL) {
      if (!Manager::Behaviour::Instance()->ScrollKnobActive() &&
          !table->RequiresKnobs()) {
        hScroller=new PosMarker(false);
      }
      else {
        hScroller=new Scroller(false);
      }

      hScroller->SetParent(this);
      hScroller->SetFlex(true,false);
      hScroller->SetModel(table->GetHAdjustment());
    }

    if (vScroller==NULL) {
      if (!Manager::Behaviour::Instance()->ScrollKnobActive() &&
          !table->RequiresKnobs()) {
        vScroller=new PosMarker(true);
      }
      else {
        vScroller=new Scroller(true);
      }

      vScroller->SetParent(this);
      vScroller->SetFlex(false,true);
      vScroller->SetModel(table->GetVAdjustment());
    }

    hScroller->CalcSize();
    vScroller->CalcSize();
    object->CalcSize();

    // Initial dimension equal to table dimension
    minWidth=object->GetOMinWidth();
    width=object->GetOWidth();
    minHeight=object->GetOMinHeight();
    height=object->GetOHeight();

    // Add Horizontal scroller
    minWidth=std::max(hScroller->GetOWidth(),minWidth);
    width=std::max(hScroller->GetOWidth(),width);
    minHeight+=hScroller->GetOHeight();


    // Add Vertical scroller
    minWidth+=vScroller->GetOMinWidth();
    width+=vScroller->GetOWidth();
    minHeight=std::max(vScroller->GetOHeight(),minHeight);
    height=std::max(vScroller->GetOHeight(),height);

    maxWidth=std::min(maxWidth,object->GetOMaxWidth()+vScroller->GetOMaxWidth());
    maxHeight=std::min(maxHeight,object->GetOMaxHeight()+hScroller->GetOMaxHeight());

    Group::CalcSize();
  }

  void Table::Layout()
  {
    size_t tWidth,tHeight;

    hScroller->ResizeWidth(GetOWidth());
    vScroller->ResizeHeight(GetOHeight());

    tWidth=width;
    tHeight=height;
    if (object->GetBackground()!=NULL) {
      tWidth-=object->GetBackground()->leftBorder+object->GetBackground()->rightBorder;
      tHeight-=object->GetBackground()->topBorder+object->GetBackground()->bottomBorder;
    }
    table->GetODimension(tWidth,tHeight);
    if (object->GetBackground()!=NULL) {
      tWidth+=object->GetBackground()->leftBorder+object->GetBackground()->rightBorder;
      tHeight+=object->GetBackground()->topBorder+object->GetBackground()->bottomBorder;
    }

    hVisible=width<tWidth;
    vVisible=height<tHeight;

    // Showing one scrollbar reduces the visible table bar, so the other scrollbar
    // might be shown, too

    if (hVisible && !vVisible) {
      tHeight=height-hScroller->GetOHeight();
      if (object->GetBackground()!=NULL) {
        tWidth-=object->GetBackground()->leftBorder+object->GetBackground()->rightBorder;
        tHeight-=object->GetBackground()->topBorder+object->GetBackground()->bottomBorder;
      }
      table->GetODimension(tWidth,tHeight);
      if (object->GetBackground()!=NULL) {
        tWidth+=object->GetBackground()->leftBorder+object->GetBackground()->rightBorder;
        tHeight+=object->GetBackground()->topBorder+object->GetBackground()->bottomBorder;
      }
      vVisible=height-hScroller->GetOHeight()<tHeight;
    }
    else if (!hVisible && vVisible) {
      tWidth=width-vScroller->GetOWidth();
      if (object->GetBackground()!=NULL) {
        tWidth-=object->GetBackground()->leftBorder+object->GetBackground()->rightBorder;
        tHeight-=object->GetBackground()->topBorder+object->GetBackground()->bottomBorder;
      }
      table->GetODimension(tWidth,tHeight);
      if (object->GetBackground()!=NULL) {
        tWidth+=object->GetBackground()->leftBorder+object->GetBackground()->rightBorder;
        tHeight+=object->GetBackground()->topBorder+object->GetBackground()->bottomBorder;
      }
      hVisible=width-vScroller->GetOWidth()<tWidth;
    }

    tWidth=width;
    tHeight=height;

    // Calculate object dimensions

    if (hVisible) {
      tHeight-=hScroller->GetOHeight();
    }

    if (vVisible) {
      tWidth-=vScroller->GetOWidth();
    }

    if (hVisible) {
      hScroller->MoveResize(x,y+tHeight,tWidth,hScroller->GetOHeight());
    }
    else {
      hScroller->Hide();
    }

    if (vVisible) {
      vScroller->MoveResize(x+tWidth,y,vScroller->GetOWidth(),tHeight);
    }
    else {
      vScroller->Hide();
    }

    object->MoveResize(x,y,tWidth,tHeight);

    Group::Layout();
  }

  size_t Table::GetHorizontalScrollerHeight() const
  {
    return hScroller->GetOHeight();
  }

  size_t Table::GetVerticalScrollerWidth() const
  {
    return vScroller->GetOWidth();
  }

  bool Table::InterceptMouseEvents() const
  {
    return true;
  }

  bool Table::HandleMouseEvent(const OS::MouseEvent& event)
  {
    if (table==NULL) {
      return false;
    }

    if (!table->RequiresKnobs() &&
        kineticScroller->HandleMouseEvent(event)) {
      return true;
    }

    return Group::HandleMouseEvent(event);
  }

  void Table::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (object==NULL) {
      return;
    }

    if (model==table->GetHAdjustment()->GetTotalModel() ||
        model==table->GetVAdjustment()->GetTotalModel()) {
      SetRelayout();
    }

    if (!table->RequiresKnobs()) {
      kineticScroller->Resync(model,msg);
    }

    Group::Resync(model,msg);
  }
}
