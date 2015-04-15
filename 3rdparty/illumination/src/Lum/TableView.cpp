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

#include <Lum/TableView.h>

#include <cmath>
#include <cstdlib>
#include <memory>

#include <Lum/Base/Size.h>

#include <Lum/Manager/Behaviour.h>

#include <Lum/OS/Driver.h>

#include <Lum/Table.h>

namespace Lum {

  static ControllerRef controller(new TableView::Controller());

  TableView::TableView()
  : autoFitColumns(false),
    autoHSize(false),
    autoVSize(false),
    tableWidth(0),
    rowHeight(0),
    refreshes(0),
    fullRefresh(true),
    top(0),
    painter(NULL),
    font(OS::display->GetFont())
  {
    SetClipFromParent(true);

    SetBackground(new OS::EmptyFill());

    SetCanFocus(true);
    RequestFocus();

    SetController(Lum::controller);

    SetDoubleClickAction(new Model::Action());
    SetMouseSelectionAction(new Model::Action());
    SetDefaultAction(new Model::Action());
  }

  TableView::~TableView()
  {
    // no code
  }

  bool TableView::RequiresKnobs() const
  {
    return false;
  }

  /**
    Return the cell under the given coordinates. Returns false if there is no
    cell at the given position.
  */
  bool TableView::GetCell(size_t mx, size_t my, size_t& x, size_t& y) const
  {
    assert(model.Valid() && PointIsIn(mx,my));

    if (model->GetRows()==0) {
      return false;
    }

    assert(rowHeight!=0);

    size_t start;

    y=((my-this->y)+vAdjustment->GetTop()-1+rowHeight-1)/rowHeight;

    if (y==0 || y>model->GetRows()) {
      return false;
    }

    mx-=this->x-hAdjustment->GetTop();

    start=0;
    for (x=0; x<header->GetColumns(); x++) {
      if ((mx>=start) && (mx<start+header->GetColumnWidth(x))) {
        return true;
      }
      start+=header->GetColumnWidth(x);
    }

    return false;
  }

  /**
    Redisplay text in that way, that the given point can be seen
  */
  void TableView::MakeVisible(size_t x, size_t y)
  {
    size_t pos;

    if (vAdjustment->IsValid()) {
      vAdjustment->MakeVisible(y*rowHeight-(rowHeight-1));
      vAdjustment->MakeVisible(y*rowHeight);
    }

    pos=0;
    for (size_t i=0; i<x-1; i++) {
      pos+=header->GetColumnWidth(i);
    }

    if (hAdjustment->IsValid()) {
      hAdjustment->MakeVisible(pos);
    }
  }

  /**
    If \p autoFitColumns is true and the table is wider than to sum of the size of all columns
    the table searches for flexible columns and will resize them to fit
    all remaining space.
  */
  void TableView::SetAutoFitColumns(bool autoFit)
  {
    autoFitColumns=autoFit;
  }

  /**
    If \p autoSize is 'true', the initial width of the
    table is be set that way, that all columns are as small as their content
    and all columns are visible if the maximum width of the table allows.

    Use Object.SetMaxWidth to restrict the maximum
    width of the table. Be aware that the table might exceed
    screen width, if you do not.
  */
  void TableView::SetAutoHSize(bool autoSize)
  {
    autoHSize=autoSize;
  }

  /**
    If \p autoSize is 'true', the initial height of the
    table is be set that way, that all entries are visible.

    Use Object.SetMaxHeight the restrict the maximum
    height of the table. Be aware that the table might exceed
    screen height, if do not.
  */
  void TableView::SetAutoVSize(bool autoSize)
  {
    autoVSize=autoSize;
  }

  /**
    Set the height of a single row in the table to a non-standard value.
    By default, the row height is choosen that way, that text using the
    standard font is completly visible. A non-standard height might be
    necessary if you choose a non-standard font are graphical elements.
  */
  void TableView::SetRowHeight(size_t height)
  {
    if (selection.Valid()) {
      rowHeight=std::max(OS::display->GetTheme()->GetMinimalTableRowHeight(),
                         height);
    }
    else {
      rowHeight=height;
    }
  }

  void TableView::RecalcTableWidth()
  {
    tableWidth=0;
    for (size_t column=0; column<header->GetColumns(); column++) {
      tableWidth+=header->GetColumnWidth(column);
    }
  }

  bool TableView::SetModel(Base::Model* model)
  {
    this->model=dynamic_cast<Model::Table*>(model);

    if (this->model.Valid())  {
      vAdjustment->Set(1,height,this->model->GetRows()*rowHeight);

      if (painter==NULL) {
        if (dynamic_cast<Model::StringTable*>(model)!=NULL) {
          SetPainter(new StringTablePainter());
        }
        else if (dynamic_cast<Model::StringRefTable*>(model)!=NULL) {
          SetPainter(new StringRefTablePainter());
        }
        else if (dynamic_cast<Model::StringMatrix*>(model)!=NULL) {
          SetPainter(new StringMatrixPainter());
        }
      }
    }

    Control::SetModel(this->model);

    return this->model.Valid();
  }

  void TableView::SetHeader(Model::Header *header)
  {
    if (this->header.Valid()) {
      Forget(this->header);
    }

    this->header=header;

    if (this->header.Valid()) {
      Observe(this->header);
    }
  }

  /**
    Set the Model::Selection object responsible for holding all information
    about the current selection. By default Model::NoneSelection ist set.

    You must assign a valid instance of a class derived from Model::Selection.
  */
  void TableView::SetSelection(Model::Selection* selection)
  {
    assert(selection!=NULL);

    if (this->selection.Valid()) {
      this->selection->Clear();
      Forget(this->selection);
    }

    this->selection=selection;

    if (this->selection.Valid()) {
      Observe(this->selection);
    }
  }

  void TableView::SetMouseSelectionAction(Model::Action* action)
  {
    mouseSelectionAction=action;
  }

  void TableView::SetDoubleClickAction(Model::Action* action)
  {
    if (doubleClickAction.Valid()) {
      Forget(doubleClickAction);
    }

    doubleClickAction=action;

    if (doubleClickAction.Valid()) {
      Observe(doubleClickAction);
    }
  }

  void TableView::SetDefaultAction(Model::Action* action)
  {
    if (defaultAction.Valid()) {
      Forget(defaultAction);
    }

    defaultAction=action;

    if (defaultAction.Valid()) {
      Observe(defaultAction);
    }
  }

  /**
    Assign a table painter to the table. A table painter is responsible for
    doing the real row and cell rendering.
    */
  void TableView::SetPainter(TableCellPainter* painter)
  {
    this->painter=painter;
    this->painter->SetParent(this);
    this->painter->SetModel(model);

    refreshes++;
    Redraw();
  }

  Model::Table* TableView::GetModel() const
  {
    return model.Get();
  }

  Model::Selection* TableView::GetSelection() const
  {
    return selection.Get();
  }

  Model::Action* TableView::GetDoubleClickAction() const
  {
    return doubleClickAction.Get();
  }

  Model::Action* TableView::GetMouseSelectionAction() const
  {
    return mouseSelectionAction.Get();
  }

  Model::Action* TableView::GetDefaultAction() const
  {
    return defaultAction.Get();
  }

  void TableView::FitColumn(size_t column)
  {
    size_t width;

    if (!model.Valid() || !header.Valid() || painter==NULL) {
      return;
    }

    width=2*OS::display->GetSpaceHorizontal(OS::Display::spaceObjectBorder);
    for (size_t i=1; i<model->GetRows(); i++) {
      painter->SetCell(column,i);
      width=std::max(width,painter->GetProposedWidth());
    }
    header->SetColumnWidth(column-1,width);
  }

  void TableView::ResyncWithModel()
  {
    CalculateRowHeight();

    if (this->model.Valid() &&
        !this->model->IsNull() &&
        this->model->GetRows()>0) {
      if (rowHeight!=0) {
        vAdjustment->SetStepSize(rowHeight);

        if (vAdjustment.Valid()) {
          vAdjustment->SetDimension(height,this->model->GetRows()*rowHeight);
        }
        else {
          vAdjustment->Set(1,height,this->model->GetRows()*rowHeight);
        }
      }
      else {
        vAdjustment->SetInvalid();
      }
    }
    else {
      vAdjustment->SetInvalid();
    }
  }

  void TableView::CalculateRowHeight()
  {
    if (rowHeight!=0 || !model.Valid() || model->GetRows()==0 || !header.Valid()) {
      return;
    }

    size_t rh=0;

    if (painter.Valid()) {
      rh=painter->GetProposedHeight();
    }
    else {
      for (size_t x=1; x<=this->header->GetColumns(); x++) {
        rh=std::max(rh,font->height);
      }
    }

    SetRowHeight(rh);
  }

  void TableView::FitColumns(size_t targetWidth)
  {
    if (header->GetColumns()==1) {
      header->SetColumnWidth(0,targetWidth);
    }
    else {
      size_t fitWidth=0;
      size_t flexs=0;

      for (size_t column=0; column<header->GetColumns(); column++) {
        fitWidth+=header->GetColumnWidth(column);
        if (header->CanColumnChangeSize(column)) {
          ++flexs;
        }
      }

      if (fitWidth!=targetWidth && flexs>0) {
        for (size_t column=0; column<header->GetColumns(); column++) {
          if (header->CanColumnChangeSize(column)) {
            if (fitWidth<targetWidth) {
              header->SetColumnWidth(column,
                                     header->GetColumnWidth(column)+
                                     (targetWidth-fitWidth)/flexs);
              fitWidth+=(targetWidth-fitWidth)/flexs;
            }
            else {
              header->SetColumnWidth(column,
                                     header->GetColumnWidth(column)-
                                     (fitWidth-targetWidth)/flexs);
              fitWidth+=(fitWidth-targetWidth)/flexs;
            }
          }
        }

        size_t newWidth=0;

        for (size_t column=0; column<header->GetColumns(); column++) {
          newWidth+=header->GetColumnWidth(column);
        }
      }
    }
  }

  /**
    Returns the vertical on-screen bounds for the given line. Returns false if the either
    the table or the line is not visible.
  */
  bool TableView::GetLineBounds(size_t line, int& ly, size_t& lheight) const
  {
    if (!visible) {
      return false;
    }

    size_t start,end;

    // In adjustment space, everything starts from 1
    start=(line-1)*rowHeight+1;
    end=start+rowHeight-1;

    // Is line visible at all?

    if (end<vAdjustment->GetTop()) {
      return false;
    }

    if (start>vAdjustment->GetTop()+height-1) {
      return false;
    }

    // From now on at least something must be visible

    if (start<vAdjustment->GetTop()) {
      start=vAdjustment->GetTop();
    }

    if (end>vAdjustment->GetTop()+height-1) {
      end=vAdjustment->GetTop()+height-1;
    }

    // Now back again to screen dimensions
    ly=start-vAdjustment->GetTop()+y;
    lheight=end-start+1;

    return true;
  }

  /* ---------- Printing --------------- */

  OS::Color TableView::GetTextColor(OS::DrawInfo *draw, const void* child) const
  {
    if (draw->selected) {
      return OS::display->GetColor(OS::Display::fillTextColor);
    }
    else {
      return OS::display->GetColor(OS::Display::tableTextColor);
    }
  }

  void TableView::FillWithBackground(OS::DrawInfo* draw,
                                     int xPos, int yPos, size_t width, size_t height)
  {
    if (!OIntersect(xPos,yPos,width,height)) {
      return;
    }

    OS::FillRef fill=OS::display->GetFill(OS::Display::tableBackgroundFillIndex);

    fill->Draw(draw,
               x,y,this->width,this->height,
               xPos,yPos,width,height);
  }

  void TableView::DrawCell(OS::DrawInfo* draw,
                           int xPos,
                           int yPos,
                           size_t width,
                           size_t height,
                           int x,
                           int y,
                           bool drawBackground)
  {
    if (painter==NULL) {
      return;
    }

    OS::FillRef back;
    bool        selected=selection.Valid() &&
                         (selection->IsLineSelected(y) || selection->IsCellSelected(x,y));
    if (selected) {
      draw->selected=true;
    }

    if (drawBackground) {
      if (selected) {
        back=OS::display->GetFill(OS::Display::tableBackgroundFillIndex);
      }
      else {
        painter->SetCell(x,y);
        back=painter->GetCellBackground(draw);

        if (!back.Valid()) {
          if (y%2!=0) {
            back=OS::display->GetFill(OS::Display::tableBackgroundFillIndex);
          }
          else {
            back=OS::display->GetFill(OS::Display::tableBackground2FillIndex);
          }
        }
      }

      back->Draw(draw,xPos,yPos,width,height,xPos,yPos,width,height);
    }

    painter->SetCell(x,y);
    painter->Draw(draw,xPos,yPos,width,height);

    if (selected) {
      draw->selected=false;
    }
  }

  /**
    Print the given line at the given display position.
  */
  void TableView::DrawLine(OS::DrawInfo* draw, int y, size_t line)
  {
    if (line>model->GetRows()) {
      FillWithBackground(draw,this->x,y,width,rowHeight);
      return;
    }

    int  x=this->x-hAdjustment->GetTop()+1;
    bool drawBackground=true;

    if (selection.Valid() && selection->IsLineSelected(line)) {
      OS::Fill* back;

      back=OS::display->GetFill(OS::Display::tableBackgroundFillIndex);

      drawBackground=false;

      draw->selected=true;
      back->Draw(draw,x,y,tableWidth,rowHeight,x,y,tableWidth,rowHeight);
      draw->selected=false;
    }

    for (size_t column=0; column<header->GetColumns(); column++) {
      size_t columnWidth;

      columnWidth=header->GetColumnWidth(column);

      if (!(x>=this->x+(int)this->width) && !(x+(int)columnWidth-1<this->x)) { /* Draw only if visible */
        DrawCell(draw,x,y,columnWidth,rowHeight,column+1,line,drawBackground);
      }
      x+=(int)columnWidth;
    }

    if (x<(int)(this->x+width)) { /* Fill space behind last cell in row */
      OS::Fill* back;

      if (line%2!=0) {
        back=OS::display->GetFill(OS::Display::tableBackgroundFillIndex);
      }
      else {
        back=OS::display->GetFill(OS::Display::tableBackground2FillIndex);
      }

      back->Draw(draw,
                 x,y,this->x+width-x+1,rowHeight,
                 x,y,this->x+width-x+1,rowHeight);
    }

    if (selection.Valid() &&
        selection->IsLineSelected(line) &&
        HasFocus()) {
      OS::FrameRef focus=OS::display->GetFrame(OS::Display::focusFrameIndex);

      /* Draw a frame arond whole line */
      if (header->GetColumns()==1) {
        focus->Draw(draw,this->x,y,this->width,rowHeight);
      }
      else {
        focus->Draw(draw,this->x-hAdjustment->GetTop()+1,y,tableWidth,rowHeight);
      }
    }
  }

  void TableView::DrawRect(OS::DrawInfo *draw, size_t start, size_t height)
  {
    size_t startLine;
    size_t endLine;
    int    startPos;
    int    endPos;

    draw->PushClip(x,y+start,width,height);

    draw->PushFont(font);
    draw->focused=HasFocus();

    startLine=(vAdjustment->GetTop()-1+start)/rowHeight;
    endLine=(vAdjustment->GetTop()-1+start+height-1)/rowHeight;

    startPos=startLine*rowHeight-(vAdjustment->GetTop()-1);
    endPos=startPos+(endLine-startLine+1)*rowHeight-1;

    startPos+=y;
    endPos+=y;

    for (size_t i=startLine; i<=endLine; i++) {
      DrawLine(draw,startPos,startLine+1);
      startPos+=rowHeight;
      startLine++;
    }

    draw->focused=false;
    draw->PopFont();
    draw->PopClip();
  }

  void TableView::DrawFull(OS::DrawInfo *draw)
  {
    DrawRect(draw,0,height);
  }

  void TableView::RedrawLine(size_t line)
  {
    int    ly=0;
    size_t lh=0;

    if (GetLineBounds(line,ly,lh)) {
      refreshes++;
      Redraw(this->x,ly,width,lh);
    }
  }

  void TableView::CalcSize()
  {
    minWidth=10; //TODO Make this font relative
    minHeight=std::max(font->height,OS::display->GetTheme()->GetMinimalTableRowHeight());

    width=minWidth;

    if (autoVSize && model.Valid()) {
      height=model->GetRows()*std::max(font->height,OS::display->GetTheme()->GetMinimalTableRowHeight());
      if (header->GetColumns()==1 && !autoHSize) {
      }
    }
    else {
      height=minHeight;
    }

    if (autoHSize &&
        model.Valid() &&
        header.Valid()) {
      size_t autoWidth=0;

      for (size_t column=0; column<header->GetColumns(); column++) {
        if (header->GetMinColumnWidth(column)==0) {
          FitColumn(column+1);
        }
        autoWidth+=header->GetColumnWidth(column);
      }

      width=autoWidth;
      minWidth=autoWidth;
      tableWidth=autoWidth;
    }
    else {
      RecalcTableWidth();
    }

    Scrollable::CalcSize();
  }

  void TableView::GetDimension(size_t& width, size_t& height)
  {
    CalculateRowHeight();

    if (model.Valid() &&
        !model->IsNull() &&
        (model->GetRows()>0 ||
         (header.Valid()))) {
      if (autoFitColumns) {
        FitColumns(width);
      }
      RecalcTableWidth();
      width=tableWidth;
    }

    if (model.Valid() &&
        !model->IsNull() &&
        model->GetRows()>0) {
      if (rowHeight!=0) {
        height=model->GetRows()*rowHeight;
      }
    }
  }

  void TableView::Layout()
  {
    ResyncWithModel();

    if (model.Valid() &&
        !model->IsNull() &&
        (model->GetRows()>0 ||
         (header.Valid()))) {
      if (autoFitColumns) {
        FitColumns(width);
      }

      RecalcTableWidth();

      hAdjustment->SetStepSize(font->StringWidth(L"m"));

      if (hAdjustment->IsValid()) {
        hAdjustment->SetDimension(width,tableWidth);
      }
      else {
        hAdjustment->Set(1,width,tableWidth);
      }
    }
    else {
      hAdjustment->SetInvalid();
    }

    Scrollable::Layout();
  }

  void TableView::Draw(OS::DrawInfo* draw,
                       int x, int y, size_t w, size_t h)
  {
    Scrollable::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* --- */

    if (refreshes>1) {
      fullRefresh=true;
    }

    if (OS::driver->GetDriverName()==L"CairoWin32") {
      fullRefresh=true;
    }
    refreshes=0;

    if (!model.Valid() || model->GetRows()==0) {
      std::wstring emptyText;

      if (model.Valid()) {
        emptyText=model->GetEmptyText();
      }

      FillWithBackground(draw,x,y,w,h);

      if (!emptyText.empty()) {
        OS::FontExtent extent;

        font->StringExtent(emptyText,extent);

        draw->PushFont(font);
        draw->PushForeground(OS::Display::tableTextColor);
        draw->DrawString(this->x+(width-extent.width) / 2,
                         this->y+font->ascent+((int)height-(int)font->height) / 2,
                         emptyText);
        draw->PopForeground();
        draw->PopFont();
      }
      return;
    }

    if (OS::display->GetType()==OS::Display::typeTextual) {
      DrawFull(draw);
      return;
    }

    if (!fullRefresh && top!=vAdjustment->GetTop()) {
      unsigned long diff=std::max(top,vAdjustment->GetTop())-std::min(top,vAdjustment->GetTop());

      if (diff>=height) {
        DrawFull(draw);
      }
      else if (vAdjustment->GetTop()>top) {
        draw->CopyArea(this->x,this->y+diff,width,height-diff,this->x,this->y);
        DrawRect(draw,height-diff,diff);
      }
      else if (top>vAdjustment->GetTop()) {
        draw->CopyArea(this->x,this->y,width,height-diff,this->x,this->y+diff);
        DrawRect(draw,0,diff);
      }
    }
    else {
      DrawRect(draw,y-this->y,h);
    }

    fullRefresh=false;
    top=vAdjustment->GetTop();
  }

  void TableView::Hide()
  {
    Scrollable::Hide();
  }

  void TableView::DrawFocus()
  {
    if (!model.Valid()) {
      return;
    }

    Model::SingleCellSelection *singleCell;
    Model::SingleLineSelection *singleLine;

    if ((singleCell=dynamic_cast<Model::SingleCellSelection*>(selection.Get()))!=NULL) {
      if (singleCell->HasSelection()) {
        RedrawLine(singleCell->GetRow());
      }
    }
    else if ((singleLine=dynamic_cast<Model::SingleLineSelection*>(selection.Get()))!=NULL) {
      if (singleLine->HasSelection()) {
        RedrawLine(singleLine->GetLine());
      }
    }
  }

  void TableView::HideFocus()
  {
    if (!model.Valid()) {
      return;
    }

    Model::SingleCellSelection *singleCell;
    Model::SingleLineSelection *singleLine;

    if ((singleCell=dynamic_cast<Model::SingleCellSelection*>(selection.Get()))!=NULL) {
      if (singleCell->HasSelection()) {
        RedrawLine(singleCell->GetRow());
      }
    }
    else if ((singleLine=dynamic_cast<Model::SingleLineSelection*>(selection.Get()))!=NULL) {
      if (singleLine->HasSelection()) {
        RedrawLine(singleLine->GetLine());
      }
    }
  }

  void TableView::OnMouseSelection()
  {
    if (mouseSelectionAction.Valid()) {
      mouseSelectionAction->Trigger();
    }
  }

  void TableView::ActionLeft()
  {
    Model::SingleCellSelection *singleCell;

    if ((singleCell=dynamic_cast<Model::SingleCellSelection*>(selection.Get()))!=NULL) {
      if (singleCell->HasSelection() && singleCell->GetColumn()>1) {
        singleCell->SelectCell(singleCell->GetColumn()-1,singleCell->GetRow());
        MakeVisible(singleCell->GetColumn(),singleCell->GetRow());
      }
    }
    else {
      hAdjustment->DecTop();
    }
  }

  void TableView::ActionRight()
  {
    Model::SingleCellSelection *singleCell;

    if ((singleCell=dynamic_cast<Model::SingleCellSelection*>(selection.Get()))!=NULL) {
      if (singleCell->HasSelection() && singleCell->GetColumn()<header->GetColumns()) {
        singleCell->SelectCell(singleCell->GetColumn()+1,singleCell->GetRow());
        MakeVisible(singleCell->GetColumn(),singleCell->GetRow());
      }
    }
    else {
      hAdjustment->IncTop();
    }
  }

  void TableView::ActionUp()
  {
    Model::SingleCellSelection *singleCell;
    Model::SingleLineSelection *singleLine;

    if ((singleCell=dynamic_cast<Model::SingleCellSelection*>(selection.Get()))!=NULL) {
      if (singleCell->GetRow()>1) {
        singleCell->SelectCell(singleCell->GetColumn(),singleCell->GetRow()-1);
        MakeVisible(singleCell->GetColumn(),singleCell->GetRow());
      }
    }
    else if ((singleLine=dynamic_cast<Model::SingleLineSelection*>(selection.Get()))!=NULL) {
      if (singleLine->HasSelection() && singleLine->GetLine()>1) {
        singleLine->SelectLine(singleLine->GetLine()-1);
        MakeVisible(1,singleLine->GetLine());
      }
    }
    else {
      vAdjustment->DecTop();
    }
  }

  void TableView::ActionDown()
  {
    Model::SingleCellSelection *singleCell;
    Model::SingleLineSelection *singleLine;

    if ((singleCell=dynamic_cast<Model::SingleCellSelection*>(selection.Get()))!=NULL) {
      if (singleCell->HasSelection()) {
        if (singleCell->GetRow()<model->GetRows()) {
          singleCell->SelectCell(singleCell->GetColumn(),singleCell->GetRow()+1);
        }
      }
      else {
        singleCell->SelectCell(1,1);
      }
      MakeVisible(singleCell->GetColumn(),singleCell->GetRow());
    }
    else if ((singleLine=dynamic_cast<Model::SingleLineSelection*>(selection.Get()))!=NULL) {
      if (singleLine->HasSelection()) {
        if (singleLine->GetLine()<model->GetRows()) {
          singleLine->SelectLine(singleLine->GetLine()+1);
          MakeVisible(1,singleLine->GetLine());
        }
      }
      else {
        singleLine->SelectLine(1);
        MakeVisible(1,singleLine->GetLine());
      }
    }
    else {
      vAdjustment->IncTop();
    }
  }

  void TableView::ActionStart()
  {
    Model::SingleCellSelection *singleCell;
    Model::SingleLineSelection *singleLine;

    if ((singleCell=dynamic_cast<Model::SingleCellSelection*>(selection.Get()))!=NULL) {
      singleCell->SelectCell(1,singleCell->GetRow());
      MakeVisible(singleCell->GetColumn(),singleCell->GetRow());
    }
    else if ((singleLine=dynamic_cast<Model::SingleLineSelection*>(selection.Get()))!=NULL) {
      singleLine->SelectLine(1);
      MakeVisible(1,singleLine->GetLine());
    }
    else {
      vAdjustment->SetTop(1);
    }
  }

  void TableView::ActionEnd()
  {
    Model::SingleCellSelection *singleCell;
    Model::SingleLineSelection *singleLine;

    if ((singleCell=dynamic_cast<Model::SingleCellSelection*>(selection.Get()))!=NULL) {
      singleCell->SelectCell(header->GetColumns(),singleCell->GetRow());
      MakeVisible(singleCell->GetColumn(),singleCell->GetRow());
    }
    else if ((singleLine=dynamic_cast<Model::SingleLineSelection*>(selection.Get()))!=NULL) {
      singleLine->SelectLine(model->GetRows());
      MakeVisible(1,singleLine->GetLine());
    }
    else {
      vAdjustment->SetTop(vAdjustment->GetTotal()-vAdjustment->GetVisible());
    }
  }

  void TableView::ActionPageUp()
  {
    Model::SingleCellSelection *singleCell;
    Model::SingleLineSelection *singleLine;

    if ((singleCell=dynamic_cast<Model::SingleCellSelection*>(selection.Get()))!=NULL) {
      if (singleCell->HasSelection()) {
        if (singleCell->GetRow()*rowHeight>vAdjustment->GetVisible()) {
          singleCell->SelectCell(singleCell->GetColumn(),
                                 std::max(1,(int)singleCell->GetRow()-(int)(height/rowHeight)-1));
        }
        else {
          singleCell->SelectCell(singleCell->GetColumn(),1);
        }
        MakeVisible(singleCell->GetColumn(),singleCell->GetRow());
      }
      else {
        vAdjustment->PageBack();
      }
    }
    else if ((singleLine=dynamic_cast<Model::SingleLineSelection*>(selection.Get()))!=NULL) {
      if (singleLine->HasSelection()) {
        if (singleLine->GetLine()*rowHeight>vAdjustment->GetVisible()) {
          singleLine->SelectLine(std::max(1,(int)singleLine->GetLine()-(int)(height/rowHeight)-1));
        }
        else {
          singleLine->SelectLine(1);
        }
        MakeVisible(1,singleLine->GetLine());
      }
      else {
        vAdjustment->PageBack();
      }
    }
    else {
      vAdjustment->PageBack();
    }
  }


  void TableView::ActionPageDown()
  {
    Model::SingleCellSelection *singleCell;
    Model::SingleLineSelection *singleLine;

    if ((singleCell=dynamic_cast<Model::SingleCellSelection*>(selection.Get()))!=NULL) {
      if (singleCell->HasSelection()) {
        singleCell->SelectCell(singleCell->GetColumn(),
                               std::min(model->GetRows(),
                                        singleCell->GetRow()+height/rowHeight+1));
        MakeVisible(singleCell->GetColumn(),singleCell->GetRow());
      }
      else {
        vAdjustment->PageForward();
      }
    }
    else if ((singleLine=dynamic_cast<Model::SingleLineSelection*>(selection.Get()))!=NULL) {
      if (singleLine->HasSelection()) {
        singleLine->SelectLine(std::min(model->GetRows(),
                                        singleLine->GetLine()+height/rowHeight+1));
        MakeVisible(1,singleLine->GetLine());
      }
      else {
        vAdjustment->PageForward();
      }
    }
    else {
      vAdjustment->PageForward();
    }
  }

  void TableView::ActionDoubleClick()
  {
    if (doubleClickAction.Valid()) {
      doubleClickAction->Trigger();
    }
  }

  bool TableView::HandleMouseEvent(const OS::MouseEvent& event)
  {
    if (!visible ||
        !model.Valid() ||
        !model->IsEnabled() ||
        !header.Valid()) {
      return false;
    }

    if (event.type==OS::MouseEvent::down &&
        PointIsIn(event)) {
      if (event.button==OS::MouseEvent::button1) {
        size_t xc,yc;

        if (GetCell(event.x,event.y,xc,yc) &&
            selection.Valid() &&
            xc>=0 &&
            xc<header->GetColumns() &&
            yc>=1 &&
            yc<=model->GetRows()) {

          if (selection->IsCellSelected(xc+1,yc)) {
            if (GetWindow()->IsDoubleClicked()) {
              ActionDoubleClick();
            }
          }
          else {
            selection->SelectCell(xc+1,yc);

            OnMouseSelection();
          }

          return true;
        }
      }
      else if (event.button==OS::MouseEvent::button4) {
        vAdjustment->DecTop();
      }
      else if (event.button==OS::MouseEvent::button5) {
        vAdjustment->IncTop();
      }
    }

    return false;
  }

  /* -------- model feetback ---------- */

  void TableView::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    const Model::Table::RefreshCell     *refreshCell;
    const Model::Table::RefreshRow      *refreshRow;
    const Model::Table::InsertRow       *insertRow;
    const Model::Table::DeleteRow       *deleteRow;
    const Model::Header::ResizedColumn  *resizedColumn;
    const Model::Header::FitColumn      *fitColumn;
    const Model::Selection::RefreshCell *selRefreshCell;
    const Model::Selection::RefreshRow  *selRefreshRow;
    const Model::Selection::Selected    *selected;

    if ((refreshCell=dynamic_cast<const Model::Table::RefreshCell*>(&msg))!=NULL) {
      RedrawLine(refreshCell->row);
    }
    else if ((refreshRow=dynamic_cast<const Model::Table::RefreshRow*>(&msg))!=NULL) {
      RedrawLine(refreshRow->row);
    }
    else if ((insertRow=dynamic_cast<const Model::Table::InsertRow*>(&msg))!=NULL) {
      CalculateRowHeight();
      vAdjustment->SetDimension(height,this->model->GetRows()*rowHeight);

      fullRefresh=true;
      refreshes++;
      Redraw(); // TODO: Better calculate to be redrawn region...
    }
    else if ((deleteRow=dynamic_cast<const Model::Table::DeleteRow*>(&msg))!=NULL) {
      if (selection.Valid() && selection->HasSelection()) {
        for (size_t r=deleteRow->row; r<deleteRow->row+deleteRow->count; r++) {
          selection->OnLineDeleted(r);
        }
      }
      CalculateRowHeight();
      vAdjustment->SetDimension(height,this->model->GetRows()*rowHeight);
      fullRefresh=true;
      refreshes++;
      Redraw(); // TODO: Better calculate to be redrawn region...
    }
    else if ((resizedColumn=dynamic_cast<const Model::Header::ResizedColumn*>(&msg))!=NULL) {
      refreshes++;
      Redraw(); // TODO: Calculate rectangle to get redrawn...
    }
    else if ((fitColumn=dynamic_cast<const Model::Header::FitColumn*>(&msg))!=NULL) {
      FitColumn(fitColumn->column+1);
    }
    else if (model==selection) {
      if ((selRefreshCell=dynamic_cast<const Model::Selection::RefreshCell*>(&msg))!=NULL) {
        RedrawLine(selRefreshCell->row);
        }
      else if ((selRefreshRow=dynamic_cast<const Model::Selection::RefreshRow*>(&msg))!=NULL) {
        RedrawLine(selRefreshRow->row);
      }
      else if ((selected=dynamic_cast<const Model::Selection::Selected*>(&msg))!=NULL &&
               defaultAction.Valid() &&
               defaultAction->IsEnabled() &&
               selection->HasSelection() &&
               Manager::Behaviour::Instance()->GetTableDefaultActionMode()==Manager::Behaviour::TableActionOnSelection) {
        defaultAction->Trigger();
      }
    }
    else if (model==doubleClickAction) {
      if (doubleClickAction->IsFinished() &&
          doubleClickAction->IsEnabled() &&
          defaultAction.Valid() &&
          defaultAction->IsEnabled() &&
          selection.Valid() &&
          selection->HasSelection() &&
          Manager::Behaviour::Instance()->GetTableDefaultActionMode()==Manager::Behaviour::TableActionOnDoubleclick) {
        defaultAction->Trigger();
      }
    }
    else { // unknown/empty message
      if (model==this->model) {
        if (selection.Valid() &&
            selection->HasSelection()) {
          selection->Clear();
        }

        ResyncWithModel();

        fullRefresh=true;
        refreshes++;
        Redraw();
      }
      else if (model==vAdjustment->GetTopModel()) {
        refreshes++;
        Redraw();
      }
      else if (model==hAdjustment->GetTopModel()) {
        fullRefresh=true;
        refreshes++;
        Redraw();
      }
    }

    Scrollable::Resync(model,msg);
  }

  TableView::Controller::Controller()
  {
    RegisterKeyAction(L"Left",actionLeft);
    RegisterKeyAction(L"Right",actionRight);
    RegisterKeyAction(L"Up",actionUp);
    RegisterKeyAction(L"Down",actionDown);
    RegisterKeyAction(L"Home",actionStart);
    RegisterKeyAction(L"End",actionEnd);
    RegisterKeyAction(L"Prior",actionPageUp);
    RegisterKeyAction(L"Next",actionPageDown);
    RegisterKeyAction(L"Space",actionDoubleClick);
  }

  bool TableView::Controller::DoAction(Lum::Object* object, Action action)
  {
    TableView *tableView=dynamic_cast<TableView*>(object);

    switch (action) {
    case actionLeft:
      tableView->ActionLeft();
      break;
    case actionRight:
      tableView->ActionRight();
      break;
    case actionUp:
      tableView->ActionUp();
      break;
    case actionDown:
      tableView->ActionDown();
      break;
    case actionStart:
      tableView->ActionStart();
      break;
    case actionEnd:
      tableView->ActionEnd();
      break;
    case actionPageUp:
      tableView->ActionPageUp();
      break;
    case actionPageDown:
      tableView->ActionPageDown();
      break;
    case actionDoubleClick:
      tableView->ActionDoubleClick();
      break;
    default:
      return false;
    }

    return true;
  }
}