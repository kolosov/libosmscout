#ifndef LUM_DATEPICKER_H
#define LUM_DATEPICKER_H

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

#include <Lum/Base/DateTime.h>

#include <Lum/Model/Action.h>
#include <Lum/Model/Calendar.h>

#include <Lum/Button.h>
#include <Lum/Object.h>
#include <Lum/Text.h>

namespace Lum {
  /**
    DatePicker implements a calender like view to select a day. It uses
    a monthly overview together with some navigation buttons so that
    you can select any day you like.

    Value will be read from and written to an assigned model
    that should be an instance of Model::Calendar.

    Future versions might even support some callback method to allow
    special coloring for different days.
  */
  class LUMAPI DatePicker : public Control
  {
  private:
    Base::Calendar     local;
    Model::CalendarRef model;
    size_t             weekStart;
    size_t             topWidth;
    size_t             topHeight;
    size_t             area[7][7];
    Text               *texts[31+7];
    Text               *current;
    size_t             cw,ch;
    size_t             cS; //< index within texts of the current selected day
    Button             *prevYearButton;
    Button             *prevMonthButton;
    Button             *nextMonthButton;
    Button             *nextYearButton;
    Model::ActionRef   prevYear;
    Model::ActionRef   prevMonth;
    Model::ActionRef   nextMonth;
    Model::ActionRef   nextYear;
    Model::ActionRef   select;

  private:
    void HandleDateChange();

  public:
    DatePicker();
    ~DatePicker();

    bool VisitChildren(Visitor &visitor, bool onlyVisible);

    bool SetModel(Base::Model* model);

    Model::Action* GetSelectAction() const;

    void CalcSize();
    void Layout();
    void Draw(OS::DrawInfo* draw,
              int x, int, size_t w, size_t h);

    bool HandleMouseEvent(const OS::MouseEvent& event);
    bool HandleKeyEvent(const OS::KeyEvent& event);

    void Resync(Base::Model* model, const Base::ResyncMsg& msg);
  };
}

#endif
