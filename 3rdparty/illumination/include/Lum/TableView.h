#ifndef LUM_TABLEVIEW_H
#define LUM_TABLEVIEW_H

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

#include <Lum/Base/DateTime.h>
#include <Lum/Base/Reference.h>

#include <Lum/Model/Action.h>
#include <Lum/Model/Header.h>
#include <Lum/Model/Number.h>
#include <Lum/Model/Selection.h>
#include <Lum/Model/Table.h>

#include <Lum/Object.h>
#include <Lum/ValuePainter.h>

namespace Lum {

  class LUMAPI TableView : public Scrollable
  {
  private:
    enum Actions {
      actionUp,
      actionDown,
      actionLeft,
      actionRight,
      actionStart,
      actionEnd,
      actionPageUp,
      actionPageDown,
      actionDoubleClick
    };

    public:
    enum Alignment
    {
      left,
      center,
      right
    };

  public:
    class Controller : public Lum::Controller
    {
    public:
      Controller();

      bool DoAction(Lum::Object* object,
                    Action action);
    };

  private:
    Model::TableRef       model;                //! The table model
    Model::HeaderRef      header;               //! The header model
    Model::SelectionRef   selection;            //! The selection mode
    Model::ActionRef      doubleClickAction;    //! Action to be triggered on double click
    Model::ActionRef      mouseSelectionAction;
    Model::ActionRef      defaultAction;        //! A non-destructive default action
                                                //! triggered by selection or doubleclick
                                                //! depending on the environment

    bool                  autoFitColumns;
    bool                  autoHSize;
    bool                  autoVSize;

    size_t                tableWidth;
    size_t                rowHeight;

    size_t                refreshes;
    bool                  fullRefresh;
    unsigned long         top;

    TableCellPainterRef   painter;
    OS::FontRef           font;

  private:
    bool GetCell(size_t mx, size_t my, size_t& x, size_t& y) const;
    void ResyncWithModel();
    void RecalcTableWidth();
    void FitColumn(size_t column);
    void CalculateRowHeight();
    void FitColumns(size_t targetWidth);
    bool GetLineBounds(size_t line, int& y, size_t& height) const;

    void FillWithBackground(OS::DrawInfo* draw, int xPos, int yPos, size_t width, size_t height);
    void DrawCell(OS::DrawInfo* draw,
                  int xPos, int yPos, size_t width, size_t height,
                  int x, int y,
                  bool drawBackground);
    void DrawLine(OS::DrawInfo* draw, int y, size_t line);
    void DrawRect(OS::DrawInfo *draw, size_t start, size_t height);
    void DrawFull(OS::DrawInfo *draw);

    void RedrawLine(size_t line);

  public:
    TableView();
    ~TableView();

    bool RequiresKnobs() const;

    // Setter
    void SetAutoFitColumns(bool autoFit);
    void SetAutoHSize(bool autoSize);
    void SetAutoVSize(bool autoSize);
    void SetRowHeight(size_t height);

    bool SetModel(Base::Model* model);
    void SetHeader(Model::Header *header);
    void SetSelection(Model::Selection* selection);

    void SetMouseSelectionAction(Model::Action* action);
    void SetDoubleClickAction(Model::Action* action);
    void SetDefaultAction(Model::Action* action);

    void SetPainter(TableCellPainter* painter);
    TableCellPainter* GetPainter() const;

    // Getter
    Model::Table* GetModel() const;
    Model::Selection* GetSelection() const;
    Model::Action* GetDoubleClickAction() const;
    Model::Action* GetMouseSelectionAction() const;
    Model::Action* GetDefaultAction() const;

    void MakeVisible(size_t x, size_t y);

    void CalcSize();
    void GetDimension(size_t& width, size_t& height);
    void Layout();

    OS::Color GetTextColor(OS::DrawInfo *draw,
                           const void* child) const;

    void Draw(OS::DrawInfo* draw,
              int x, int y, size_t w, size_t h);
    void Hide();
    void DrawFocus();
    void HideFocus();

    void ActionLeft();
    void ActionRight();
    void ActionUp();
    void ActionDown();
    void ActionStart();
    void ActionEnd();
    void ActionPageUp();
    void ActionPageDown();
    void ActionDoubleClick();

    virtual void OnMouseSelection();

    bool HandleMouseEvent(const OS::MouseEvent& event);

    void Resync(Base::Model* model, const Base::ResyncMsg& msg);
  };
}

#endif
