#include <Lum/DatePicker.h>

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

#include <Lum/Base/String.h>

#include <time.h>

namespace Lum {

  DatePicker::DatePicker()
  : prevYear(new Model::Action()),
    prevMonth(new Model::Action()),
    nextMonth(new Model::Action()),
    nextYear(new Model::Action()),
    select(new Model::Action())
  {
    SetBackground(OS::display->GetFill(OS::Display::tableBackgroundFillIndex));

    if (!OS::display->GetTheme()->FocusOnEditableControlsOnly()) {
      SetCanFocus(true);
      RequestFocus();
    }

    weekStart=1;

    Observe(prevYear);
    Observe(prevMonth);
    Observe(nextMonth);
    Observe(nextYear);

    prevYearButton=Button::Create(L"<<",prevYear,true,true);
    prevYearButton->SetPulse(true);
    prevMonthButton=Button::Create(L"<",prevMonth,true,true);
    prevMonthButton->SetPulse(true);
    nextMonthButton=Button::Create(L">",nextMonth,true,true);
    nextMonthButton->SetPulse(true);
    nextYearButton=Button::Create(L">>",nextYear,true,true);
    nextYearButton->SetPulse(true);

    current=new Text(L"",OS::Font::normal,Text::centered);
    current->SetFlex(true,true);
    current->SetParent(this);

    /* Initialize days texts */
    for (size_t x=0; x<31; x++) {
      texts[x]=new Text(Base::NumberToWString(x+1),
                             OS::Font::normal,Text::centered);
    }

    for (size_t x=0; x<7; x++) {
      texts[x+31]=new Text(Base::Calendar::GetWeekDayNameShort((x+1)%7),
                                OS::Font::normal,Text::centered);
    }

    for (size_t x=0; x<31+7; x++) {
      texts[x]->SetFlex(true,true);
      texts[x]->SetParent(this);
    }

    /* Initial first row with weekdays texts */
    for (size_t x=0; x<7; x++) {
      area[x][0]=x+31;
    }

    /* Initialize the rest to empty string */
    for (size_t y=1; y<7; y++) {
      for (size_t x=0; x<7; x++) {
        area[x][y]=(size_t)-1;
      }
    }

    HandleDateChange();
  }

  DatePicker::~DatePicker()
  {
    for (size_t x=0; x<31+7; x++) {
      delete texts[x];
    }

    delete current;

    delete prevYearButton;
    delete prevMonthButton;
    delete nextMonthButton;
    delete nextYearButton;
  }

  void DatePicker::HandleDateChange()
  {
    Base::Calendar current;
    size_t         day,week;

    current.SetDate(1,local.GetMonth(),local.GetYear());

    for (size_t y=1; y<7; y++) {
      for (size_t x=0; x<7; x++) {
        area[x][y]=(size_t)-1;
      }
    }

    week=1;
    cS=(size_t)-1;
    for (size_t x=1; x<=(size_t)current.GetDaysOfCurrentMonth(); x++) {
      current.SetDate(x,current.GetMonth(),current.GetYear());
      day=current.GetDayOfWeek();
      area[(day+7-weekStart) % 7][week]=x-1;

      if (model.Valid()) {
        if (current.GetYear()==model->Get().GetYear() &&
            current.GetMonth()==model->Get().GetMonth() &&
            current.GetDayOfMonth()==model->Get().GetDayOfMonth()) {
          cS=x-1;
        }
      }

      if (day==((weekStart-1) % 7)) {
        week++;
      }
    }

    this->current->SetText(Base::Calendar::GetMonthNameShort(current.GetMonth())+L" "+Base::NumberToWString(current.GetYear()),
                           OS::Font::normal,Text::centered);

    if (visible) {
      Redraw();
    }
  }

  bool DatePicker::VisitChildren(Visitor &visitor, bool /*onlyVisible*/)
  {
    if (!visitor.Visit(prevYearButton)) {
      return false;
    }

    if (!visitor.Visit(prevMonthButton)) {
      return false;
    }

    if (!visitor.Visit(nextMonthButton)) {
      return false;
    }

    if (!visitor.Visit(nextYearButton)) {
      return false;
    }

    if (!visitor.Visit(current)) {
      return false;
    }

    for (size_t x=0; x<31+7; x++) {
      if (!visitor.Visit(texts[x])) {
        return false;
      }
    }

    return true;
  }

  bool DatePicker::SetModel(Base::Model* model)
  {
    this->model=dynamic_cast<Model::Calendar*>(model);

    Control::SetModel(this->model);

    return this->model.Valid();
  }

  Model::Action* DatePicker::GetSelectAction() const
  {
    return select.Get();
  }

  void DatePicker::CalcSize()
  {
    cw=0;
    ch=0;

    prevYearButton->SetParent(this);
    prevMonthButton->SetParent(this);
    nextMonthButton->SetParent(this);
    nextYearButton->SetParent(this);

    prevYearButton->CalcSize();
    prevMonthButton->CalcSize();
    nextMonthButton->CalcSize();
    nextYearButton->CalcSize();

    for (size_t x=0; x<31+7; x++) {
      texts[x]->CalcSize();
      cw=std::max(cw,(size_t)texts[x]->GetOMinWidth());
      ch=std::max(ch,(size_t)texts[x]->GetOMinHeight());
    }

    current->CalcSize();

    topWidth=prevYearButton->GetOWidth()+prevMonthButton->GetOWidth()+
             OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject)+
             current->GetOWidth()+
             OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject)+
             nextYearButton->GetOWidth()+nextMonthButton->GetOWidth();

    topHeight=0;
    topHeight=std::max(topHeight,prevYearButton->GetOHeight());
    topHeight=std::max(topHeight,prevMonthButton->GetOHeight());
    topHeight=std::max(topHeight,current->GetOHeight());
    topHeight=std::max(topHeight,nextMonthButton->GetOHeight());
    topHeight=std::max(topHeight,nextYearButton->GetOHeight());

    minWidth=std::max(7*cw,topWidth);
    minHeight=topHeight+
              OS::display->GetSpaceVertical(OS::Display::spaceIntraObject)+
              7*ch;

    width=minWidth;
    height=minHeight;

    Control::CalcSize();
  }

  void DatePicker::Layout()
  {
    cw=width/7;
    ch=(height-OS::display->GetSpaceVertical(OS::Display::spaceIntraObject)-topHeight)/7;


    prevYearButton->Move(x,y);
    prevMonthButton->Move(prevYearButton->GetOX()+prevYearButton->GetOWidth(),y);

    nextYearButton->Move(x+width-1-nextYearButton->GetOWidth(),y);
    nextMonthButton->Move(nextYearButton->GetOX()-nextMonthButton->GetOWidth(),y);

    current->MoveResize(prevMonthButton->GetOX()+
                        prevMonthButton->GetOWidth()+
                        OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject),
                        y+(topHeight-current->GetOHeight())/2,
                        width-
                        2*OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject)-
                        prevMonthButton->GetOWidth()-
                        prevYearButton->GetOWidth()-
                        nextMonthButton->GetOWidth()-
                        nextYearButton->GetOWidth(),
                        current->GetOHeight());

    for (size_t Y=0; Y<7; Y++) {
      for (size_t X=0; X<7; X++) {
        size_t text=area[X][Y];

        if (text!=(size_t)-1) {
          texts[text]->MoveResize(x+X*cw,topHeight+OS::display->GetSpaceVertical(OS::Display::spaceIntraObject)+y+Y*ch,cw,ch);
        }
      }
    }
  }

  void DatePicker::Draw(OS::DrawInfo* draw,
                        int x, int y, size_t w, size_t h)
  {
    Control::Draw(draw,x,y,w,h);

    if (!Intersect(x,y,w,h)) {
      return;
    }

    /* --- */

    current->Draw(draw,x,y,w,h);

    prevYearButton->Draw(draw,x,y,w,h);
    prevMonthButton->Draw(draw,x,y,w,h);
    nextMonthButton->Draw(draw,x,y,w,h);
    nextYearButton->Draw(draw,x,y,w,h);

    for (size_t Y=0; Y<7; Y++) {
      for (size_t X=0; X<7; X++) {
        size_t text=area[X][Y];

        if (text!=(size_t)-1) {
          if (text==cS) {
            draw->PushForeground(OS::Display::fillColor);
            draw->FillRectangle(texts[text]->GetOX(),texts[text]->GetOY(),
                                texts[text]->GetOWidth(),texts[text]->GetOHeight());
            draw->selected=true;
          }
          texts[text]->Draw(draw,x,y,w,h);

          if (text==cS) {
            draw->selected=false;
            draw->PopForeground();
          }
        }
      }
    }
  }

  bool DatePicker::HandleMouseEvent(const OS::MouseEvent& event)
  {
    /* It makes no sense to get the focus if we are currently not visible */
    if (!visible || !model.Valid() || !model->IsEnabled()) {
      return false;
    }

    /*
      When the left mousebutton gets pressed without any qualifier
      in the bounds of our button...
    */

    if (event.type==OS::MouseEvent::down && PointIsIn(event) && event.button==OS::MouseEvent::button1) {
      for (size_t x=0; x<31+7; x++) {
        if (texts[x]->PointIsIn(event.x,event.y)) {
          if (x<=30) {
            local.SetDate(x+1,local.GetMonth(),local.GetYear());
            model->SetDate(local.GetDayOfMonth(),local.GetMonth(),local.GetYear());
            HandleDateChange();

            select->Trigger();

            return true;
          }
        }
      }

      if (current->PointIsIn(event.x,event.y)) {
        if (event.qualifier==0) {
          local.SetDate(model->Get().GetDayOfMonth(),
                        model->Get().GetMonth(),
                        model->Get().GetYear());
        }
        else {
          local.SetToCurrent();
          model->SetDate(local.GetDayOfMonth(),local.GetMonth(),local.GetYear());
        }
        HandleDateChange();
        return true;
      }
    }

    return false;
  }

  bool DatePicker::HandleKeyEvent(const OS::KeyEvent& event)
  {
    if (event.type==OS::KeyEvent::down) {

      if ((event.qualifier & OS::qualifierAlt) || (event.qualifier & OS::qualifierControl)) {
        if (event.key==OS::keyLeft || event.key==OS::keyUp) {
          Base::Calendar date=model->Get();

          date.AddYears(-1);
          model->Set(date);

          return true;
        }
        else if (event.key==OS::keyRight || event.key==OS::keyDown) {
          Base::Calendar date=model->Get();

          date.AddYears(1);
          model->Set(date);

          return true;
        }
      }
      else if (event.qualifier & OS::qualifierShift) {
        if (event.key==OS::keyLeft || event.key==OS::keyUp) {
          Base::Calendar date=model->Get();

          date.AddMonths(-1);
          model->Set(date);

          return true;
        }
        else if (event.key==OS::keyRight || event.key==OS::keyDown) {
          Base::Calendar date=model->Get();

          date.AddMonths(1);
          model->Set(date);

          return true;
        }
      }
      else if (event.qualifier==0) {
        if (event.key==OS::keyLeft) {
          Base::Calendar date=model->Get();

          date.AddDays(-1);
          model->Set(date);

          return true;
        }
        else if (event.key==OS::keyRight) {
          Base::Calendar date=model->Get();

          date.AddDays(1);
          model->Set(date);

          return true;
        }
        else if (event.key==OS::keyUp) {
          Base::Calendar date=model->Get();

          date.AddDays(-7);
          model->Set(date);

          return true;
        }
        else if (event.key==OS::keyDown) {
          Base::Calendar date=model->Get();

          date.AddDays(7);
          model->Set(date);

          return true;
        }
        else if (event.key==OS::keyReturn) {
          select->Trigger();

          return true;
        }
      }
    }

    return false;
  }

  void DatePicker::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==this->model) {
      local.SetDate(this->model->Get().GetDayOfMonth(),
                    this->model->Get().GetMonth(),
                    this->model->Get().GetYear());

      HandleDateChange();
    }
    else if (model==prevYear && prevYear->IsFinished()) {
      local.AddYears(-1);
      HandleDateChange();
    }
    else if (model==prevMonth && prevMonth->IsFinished()) {
      local.AddMonths(-1);
      HandleDateChange();
    }
    else if (model==nextMonth && nextMonth->IsFinished()) {
      local.AddMonths(1);
      HandleDateChange();
    }
    else if (model==nextYear && nextYear->IsFinished()) {
      local.AddYears(1);
      HandleDateChange();
    }

    Control::Resync(model,msg);
  }
}
