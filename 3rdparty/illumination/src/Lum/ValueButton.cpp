
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

#include <Lum/ValueButton.h>

#include <Lum/Base/DateTime.h>
#include <Lum/Base/L10N.h>

#include <Lum/Model/String.h>

#include <Lum/OS/Theme.h>

#include <Lum/Dialog.h>
#include <Lum/Text.h>

namespace Lum {

  ValueButton::ValueButton()
  : label(NULL),
    shortCut('\0'),scAssigned(false),
    action(new Model::Action()),
    triggering(false),
    popup(NULL),
    paintingValue(false)
  {
    SetBackground(OS::display->GetFill(OS::Display::buttonBackgroundFillIndex));

    if (!OS::display->GetTheme()->FocusOnEditableControlsOnly()) {
      SetCanFocus(true);
    }

    SetRedrawOnMouseActive(true);

    Observe(action);
  }

  ValueButton::~ValueButton()
  {
    delete popup;
    delete label;
  }

  void ValueButton::SetPainter(ValuePainter* painter)
  {
    this->painter=painter;
  }

  void ValueButton::PreparePainter(ValuePainter* painter)
  {
    painter->SetParent(this);
    painter->SetModel(GetModel());
  }

  bool ValueButton::HasBaseline() const
  {
    return label!=NULL && label->HasBaseline();
  }

  size_t ValueButton::GetBaseline() const
  {
    assert(inited && label!=NULL && label->HasBaseline());

    return GetTopBorder()+label->GetBaseline();
  }

  OS::Font* ValueButton::GetFont(const void* child) const
  {
    if (child!=NULL && child==painter && paintingValue) {
      return OS::display->GetFont(70);
    }
    else {
      return Control::GetFont(child);
    }
  }

  bool ValueButton::GetFontStyleSmart(const void* child) const
  {
    if (child!=NULL && child==painter && paintingValue) {
      return true;
    }
    else {
      return Control::GetFontStyleSmart(child);
    }
  }

  bool ValueButton::HasImplicitLabel() const
  {
    return true;
  }

  /**
    Use this method if you do not want text displayed in the button but
    want to use some other Object as label
  */
  ValueButton* ValueButton::SetLabel(Object* label)
  {
    delete this->label;

    this->label=label;
    this->label->SetParent(this);

    return this;
  }

  /**
  */
  ValueButton* ValueButton::SetLabel(const std::wstring& string)
  {
    Text *text;

    text=new Text();
    text->SetParent(this);
    text->SetFlex(true,false);
    text->SetText(string);

    delete label;

    label=text;

    return this;
  }

  ValueButton* ValueButton::SetPopup(Dlg::ValuePopup* popup)
  {
    delete this->popup;

    this->popup=popup;

    return this;
  }

  void ValueButton::CalcSize()
  {
    assert(label!=NULL);
    assert(painter.Valid());

    SetBackground(OS::display->GetFill(OS::Display::buttonBackgroundFillIndex));
    if (!OS::display->GetTheme()->FocusOnEditableControlsOnly()) {
      RequestFocus();
    }

    if (OS::display->GetTheme()->RequestFingerFriendlyControls()) {
      SetMinWidth(OS::display->GetTheme()->GetFingerFriendlyMinWidth());
      SetMinHeight(OS::display->GetTheme()->GetFingerFriendlyMinHeight());
    }

    width=0;
    height=0;
    minWidth=0;
    minHeight=0;

    /*
      Now we let the image calculate its bounds and simply add its size
      to the size of the button.
    */
    label->CalcSize();
    minWidth=std::max(minWidth,label->GetOMinWidth());
    width=std::max(width,label->GetOWidth());
    minHeight+=label->GetOMinHeight();
    height+=label->GetOHeight();

    PreparePainter(painter);
    paintingValue=true;
    minWidth=std::max(minWidth,painter->GetProposedWidth());
    width=std::max(width,painter->GetProposedWidth());
    minHeight+=painter->GetProposedHeight();
    height+=painter->GetProposedHeight();
    paintingValue=false;

    maxWidth=30000;
    maxHeight=30000;

    Control::CalcSize();
  }

  bool ValueButton::HandleMouseEvent(const OS::MouseEvent& event)
  {
    /* It makes no sense to get the focus if we are currently not visible */
    if (!visible || GetModel()==NULL || !GetModel()->IsEnabled()) {
      return false;
    }

    /*
      When the left mousebutton gets pressed without any qualifier
      in the bounds of our button...
    */

    if (event.type==OS::MouseEvent::down &&
        PointIsIn(event) &&
        event.button==OS::MouseEvent::button1) {
      if (!action->IsStarted()) {
        /* We change our state to pressed and redisplay ourself */
        triggering=true;
        action->Start();

        /*
          Since we want the focus for waiting for buttonup we return
          a pointer to ourself.
        */
        return true;
      }
    }
    else if (event.IsGrabEnd()) {
      if (action->IsStarted()) {
        /*
          If the users released the left mousebutton over our bounds we really
          got selected.
        */
        if (PointIsIn(event)) {
          action->Finish();
          triggering=false;
        }
        else {
          action->Cancel();
          triggering=false;
        }
      }
    }
    else if (event.type==OS::MouseEvent::move && event.IsGrabed()) {
      if (PointIsIn(event)) {
        if (!action->IsStarted()) {
          triggering=true;
          action->Start();
        }
      }
      else {
        if (action->IsStarted()) {
          action->Cancel();
          triggering=false;
        }
      }

      return true;
    }

    return false;
  }

  bool ValueButton::HandleKeyEvent(const OS::KeyEvent& event)
  {
    if (event.type==OS::KeyEvent::down) {
      if (event.key==OS::keySpace && action->IsInactive()) {
        triggering=true;
        action->Start();

        return true;
      }
    }
    else if (event.type==OS::KeyEvent::up) {
      if (event.key==OS::keySpace) {
        if (event.qualifier==0 && action->IsStarted()) {
          action->Finish();
          triggering=false;
        }
        else if (event.qualifier!=0 && action->IsStarted()) {
          action->Cancel();
          triggering=false;
        }
      }
    }

    return false;
  }

  /*
     We tell the image to resize themself to
     our current bounds. Our bounds could have changed
     because Resize may have been called by some layout-objects
     between Button.CalcSize and Button.Draw.
   */
  void ValueButton::Layout()
  {
    if (label!=NULL) {
      label->Resize(width,label->GetOHeight());
      label->Move(x+(width-label->GetOWidth()) / 2,
                  y);
      label->Layout();
    }

    Control::Layout();
  }

  void ValueButton::PrepareForBackground(OS::DrawInfo* draw)
  {
    if (action.Valid() && action->IsEnabled()) {
      if (IsMouseActive() && !action->IsStarted()) {
        draw->activated=true;
      }
      if (triggering && action->IsStarted()) {
        draw->selected=true;
      }
    }
    else {
      draw->disabled=true;
    }

    draw->focused=HasFocus();
  }

  void ValueButton::Draw(OS::DrawInfo* draw,
                         int x, int y, size_t w, size_t h)
  {
    Control::Draw(draw,x,y,w,h); /* We must call Draw of our superclass */

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /*
      Set the correct draw mode before calling the baseclass,
      since the baseclass draw the object frame.
    */
    if (action.Valid() && action->IsEnabled()) {
      if (IsMouseActive() && !action->IsStarted()) {
        draw->activated=true;
      }
      if (triggering && action->IsStarted()) {
        draw->selected=true;
      }
    }
    else {
      draw->disabled=true;
    }

    draw->focused=HasFocus();

    label->Draw(draw,x,y,w,h);

    if (!GetModel()->IsNull()) {
      PreparePainter(painter);
      paintingValue=true;
      painter->Draw(draw,
                    this->x,this->y+label->GetOHeight(),
                    this->width,this->height-label->GetOHeight());
      paintingValue=false;
    }

    draw->activated=false;
    draw->selected=false;
    draw->disabled=false;
    draw->focused=false;

    if (!scAssigned) {
      Dialog* window;

      window=dynamic_cast<Dialog*>(GetWindow()->GetMaster());
      if (window!=NULL) {
        if (shortCut!='\0') {
          window->RegisterShortcut(this,0,std::wstring(1,shortCut),action);
          window->RegisterShortcut(this,OS::qualifierAlt,std::wstring(1,shortCut),action);
        }
      }
      scAssigned=true;
    }
 }

  void ValueButton::Hide()
  {
    if (visible) {
      label->Hide();

      Control::Hide();
    }
  }

  void ValueButton::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==this->action && visible) {
      if (this->action->IsInactive()) {
        Redraw();
      }
      else if (this->action->IsStarted()) {
        Redraw();
      }

      if (action->IsFinished() && GetModel()!=NULL) {
        GetModel()->Push();

        popup->SetParent(GetWindow());
        popup->SetReference(this);
        popup->SetModel(GetModel());

        if (popup->Open()) {
          popup->EventLoop();
          popup->Close();
        }

        GetModel()->Pop();
      }
    }
    else if (model==GetModel()) {
      Redraw();
    }

    Control::Resync(model,msg);
  }

  DateValueButton::DateValueButton()
  {
    SetPainter(new DatePainter());
    SetPopup(new Dlg::DateValuePopup());
  }

  TimeValueButton::TimeValueButton(Base::TimeFormat format)
   : format(format)
  {
    SetPainter(new TimePainter(format));
    SetPopup(new Dlg::TimeValuePopup(format));
  }

  TableStringValueButton::TableStringValueButton(Model::Table* table,
                                                 Model::Header* header)
  : tableModel(table),
    headerModel(header)
  {
    Observe(table);

    SetPainter(new StringPainter());
    SetPopup(new Dlg::TableStringValuePopup(table,header));
  }

  TableIndexValueButton::TableIndexValueButton(Model::Table* table,
                                               TableCellPainter* painter,
                                               Model::Header* header)
  : tableModel(table),
    headerModel(header)
  {
    Observe(table);

    if (painter==NULL) {
      painter=TableCellPainter::GetDefaultTableCellPainter(table);
    }

    SetPainter(painter);
    SetPopup(new Dlg::TableIndexValuePopup(table,painter,header));
  }

  void TableIndexValueButton::PreparePainter(ValuePainter* painter)
  {
    ValueButton::PreparePainter(painter);

    Model::Number    *model=dynamic_cast<Model::Number*>(GetModel());
    TableCellPainter *tablePainter=dynamic_cast<TableCellPainter*>(painter);

    tablePainter->SetModel(tableModel);

    if (model!=NULL && !model->IsNull()) {
      tablePainter->SetCell(1,model->GetUnsignedLong());
    }
  }

  TableIndexValueButton* TableIndexValueButton::Create(const std::wstring& label,
                                                       Base::Model* model,
                                                       Model::Table* table,
                                                       TableCellPainter* painter,
                                                       bool horizontalFlex,
                                                       bool verticalFlex)
  {
    TableIndexValueButton *control;

    control=new TableIndexValueButton(table,painter);

    control->SetLabel(label);
    control->SetModel(model);
    control->SetFlex(horizontalFlex,verticalFlex);

    return control;
  }
}
