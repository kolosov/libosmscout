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

#include <Lum/Combo.h>

#include <Lum/Text.h>

namespace Lum {

  Combo::Combo(Object* value, bool editable)
  : imageRight(true),
    editable(editable),
    table(NULL),
    popup(NULL),
    prevAction(new Lum::Model::Action()),
    nextAction(new Lum::Model::Action()),
    tableModel(NULL),
    selection(new Model::SingleLineSelection()),
    value(value)
  {
    if (editable) {
      assert(value!=NULL);
    }
    else {
      assert(value==NULL);
    }

    if (editable) {
      SetBackground(OS::display->GetFill(OS::Display::editComboBackgroundFillIndex));
      value->SetParent(this);
    }
    else {
      SetBackground(OS::display->GetFill(OS::Display::comboBackgroundFillIndex));

      if (!OS::display->GetTheme()->FocusOnEditableControlsOnly()) {
        SetCanFocus(true);
        RequestFocus();
      }
    }

    SetRedrawOnMouseActive(true);

    headerModel=new Model::HeaderImpl();
    headerModel->AddColumn(L"",Lum::Base::Size::pixel,0);

    Observe(selection);

    Observe(prevAction);
    Observe(nextAction);
  }

  Combo::~Combo()
  {
    delete value;
  }

  bool Combo::HasBaseline() const
  {
    return value!=NULL && value->HasBaseline();
  }

  Object* Combo::GetValueObject() const
  {
    return value;
  }

  size_t Combo::GetBaseline() const
  {
    assert(value!=NULL);

    if (value->HasBaseline()) {
      // We also assume that is vertically flexible
      return GetTopBorder()+value->GetBaseline();
    }
    else {
      return 0;
    }
  }

  bool Combo::VisitChildren(Visitor &visitor, bool /*onlyVisible*/)
  {
    if (value!=NULL &&
        !visitor.Visit(value)) {
      return false;
    }

    return true;
  }

  void Combo::PreparePainter(ValuePainter* painter)
  {
    painter->SetParent(this);
    painter->SetModel(tableModel);
  }

  /**
    Call this method to assign the table model that will be used to display
    all possible values in the pop window.
  */
  void Combo::SetHeaderModel(Model::Header* model)
  {
    headerModel=model;
  }

  /**
    Call this method to assign the table model that will be used to display
    all possible values in the pop window.
  */
  void Combo::SetTableModel(Model::Table* model)
  {
    if (tableModel.Valid()) {
      Forget(tableModel);
    }

    tableModel=model;

    if (tableModel.Valid()) {
      if (!tablePainter.Valid()) {
        tablePainter=TableCellPainter::GetDefaultTableCellPainter(model);
      }

      Observe(tableModel);
    }
  }

  void Combo::SetTableCellPainter(TableCellPainter *tablePainter)
  {
    this->tablePainter=tablePainter;
  }

  void Combo::CalcSize()
  {
    assert(tablePainter.Valid());

    if (editable) {
      OS::ImageRef image=OS::display->GetImage(OS::Display::comboEditButtonImageIndex);

      value->SetFlex(true,true);
      value->CalcSize();

      minWidth=image->GetWidth()+value->GetOMinWidth();
      width=image->GetWidth()+value->GetOWidth();

      minHeight=std::max(value->GetOMinHeight(),
                         image->GetHeight());
      height=std::max(value->GetOHeight(),
                      image->GetHeight());
    }
    else {
      OS::ImageRef image=OS::display->GetImage(OS::Display::comboImageIndex);
      OS::ImageRef divider=OS::display->GetImage(OS::Display::comboDividerImageIndex);

      minWidth=OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject)+
               divider->GetWidth()+
               OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject)+
               image->GetWidth();

      width=minWidth;

      PreparePainter(tablePainter);
      minWidth+=tablePainter->GetProposedWidth();
      width+=tablePainter->GetProposedWidth();

      minHeight=std::max(tablePainter->GetProposedHeight(),image->GetHeight());
      height=std::max(tablePainter->GetProposedHeight(),image->GetHeight());
    }

    Control::CalcSize();
  }

  Combo::ComboPopup::ComboPopup(Combo *combo)
  : combo(combo),
    success(false)
  {
    // no code
  }

  Combo::ComboPopup::~ComboPopup()
  {
    Forget(combo->table->GetTableView()->GetMouseSelectionAction());

    combo->table=NULL;
  }

  void Combo::ComboPopup::PreInit()
  {
    combo->table=new Lum::Table();
    combo->table->SetFlex(true,true);
    combo->table->SetMinHeight(Base::Size::stdCharHeight,1);

    if (OS::display->GetTheme()->PopupsAreDialogs()) {
      combo->table->SetMaxHeight(Base::Size::workVRel,100);
    }
    else if (OS::display->GetSize()<=OS::Display::sizeSmall) {
      combo->table->SetMaxHeight(Base::Size::workVRel,80);
    }
    else {
      combo->table->SetMaxHeight(Base::Size::workVRel,40);
    }

    combo->table->SetHeaderModel(combo->headerModel);
    combo->table->SetModel(combo->tableModel);
    combo->table->SetPainter(combo->tablePainter);
    combo->table->SetSelection(combo->selection);
    combo->table->SetAutoFitColumns(true);
    combo->table->SetAutoHSize(true);
    combo->table->SetAutoVSize(true);

    RegisterCommitShortcut(combo->table->GetTableView(),
                           combo->table->GetTableView()->GetMouseSelectionAction());
    Observe(combo->table->GetTableView()->GetMouseSelectionAction());
    Observe(GetPreOpenedAction());

    SetMain(combo->table,true,true);

    Popup::PreInit();
  }

  void Combo::ComboPopup::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (combo->table!=NULL &&
        combo->table->GetTableView()!=NULL &&
        model==GetPreOpenedAction() && GetPreOpenedAction()->IsFinished()) {
      if (combo->tableModel.Valid() && combo->selection->GetLine()>0) {
        combo->table->GetTableView()->MakeVisible(1,combo->selection->GetLine());
      }
    }
    else if (combo->table!=NULL &&
             combo->table->GetTableView()!=NULL &&
             model==combo->table->GetTableView()->GetMouseSelectionAction() &&
             combo->table->GetTableView()->GetMouseSelectionAction()->IsFinished()) {
      success=true;
      Exit();
    }

    Popup::Resync(model,msg);
  }

  void Combo::OpenPopup()
  {
    popup=new ComboPopup(this);
    popup->SetParent(GetWindow());
    popup->SetReference(this);

    if (popup->Open()) {
      popup->EventLoop();
      popup->Close();

      if (popup->success &&
          HasModel() &&
          tableModel.Valid() &&
          selection->HasSelection()) {
        SetModelFromSelection();
      }
    }

    delete popup;
    popup=NULL;
  }

  void Combo::SetTableRow(size_t row)
  {
    if (tableModel.Valid() && tableModel->GetRows()>0) {
      if (row>0) {
        selection->SelectCell(1,row);
      }
      else {
        selection->Clear();
      }
    }


    if (HasModel() && tableModel.Valid()) {
      SetModelFromSelection();
    }
  }

  bool Combo::HandleMouseEvent(const OS::MouseEvent& event)
  {
    if (!visible || !HasModel() || !GetModel()->IsEnabled()) {
      return false;
    }

    if (event.type==OS::MouseEvent::down &&
        PointIsIn(event) &&
        event.button==OS::MouseEvent::button1) {
      OpenPopup();
      return true;
    }

    return false;
  }

  bool Combo::HandleKeyEvent(const OS::KeyEvent& event)
  {
    if (value!=NULL &&
        dynamic_cast<Control*>(value)!=NULL &&
        dynamic_cast<Control*>(value)->HandleKeyEvent(event)) {
      return true;
    }

    if (event.type==OS::KeyEvent::down) {
      switch (event.key) {
      case OS::keyUp:
        prevAction->Trigger();
        return true;
        break;

      case OS::keyDown:
        nextAction->Trigger();
        return true;
        break;

      default:
        break;
      }
    }

    return false;
  }

  void Combo::Layout()
  {
    if (editable) {
      OS::ImageRef image=OS::display->GetImage(OS::Display::comboEditButtonImageIndex);

      value->Resize(width-image->GetWidth(),height);

      if (imageRight) {
        value->Move(x,y+(height-value->GetOHeight()) / 2);
      }
      else {
        value->Move(x+image->GetWidth(),
                    y+(height-value->GetOHeight()) / 2);
      }
    }

    Control::Layout();
  }

  void Combo::PrepareForBackground(OS::DrawInfo* draw)
  {
    draw->selected=false;
    draw->activated=IsMouseActive();
    draw->focused=HasFocus();
    draw->disabled=!(GetModel()!=NULL) || GetModel()->IsNull() || !GetModel()->IsEnabled();
  }

  void Combo::Draw(OS::DrawInfo* draw,
                   int x, int y, size_t w, size_t h)
  {
    Control::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* --- */

    draw->selected=false;
    draw->activated=IsMouseActive();
    draw->focused=HasFocus();
    draw->disabled=!(GetModel()!=NULL) || GetModel()->IsNull() || !GetModel()->IsEnabled();

    if (editable) {
      OS::ImageRef image=OS::display->GetImage(OS::Display::comboEditButtonImageIndex);

      if (imageRight) {
        image->Draw(draw,
                    this->x+value->GetOWidth(),this->y+(height-image->GetHeight()) / 2);
      }
      else {
        image->Draw(draw,
                    this->x,
                    this->y+(height-image->GetHeight()) / 2);
      }
    }
    else {
      OS::ImageRef image=OS::display->GetImage(OS::Display::comboImageIndex);
      OS::ImageRef divider=OS::display->GetImage(OS::Display::comboDividerImageIndex);

      size_t valueWidth=width-
                        image->GetWidth()-
                        2*OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject)-
                        divider->GetWidth();
      size_t valueHeight=height;


      if (imageRight) {
        divider->DrawStretched(draw,
                               this->x+width-
                               image->GetWidth()-
                               OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject)-
                               divider->GetWidth(),
                               this->y,
                               image->GetWidth(),
                               height);

        image->Draw(draw,
                    this->x+width-
                    image->GetWidth(),
                    this->y+(height-image->GetHeight()) / 2);

        if (selection->HasSelection()) {
          PreparePainter(tablePainter);
          tablePainter->SetCell(1,selection->GetLine());
          tablePainter->Draw(draw,
                             this->x,
                             this->y+(height-valueHeight) / 2,
                             valueWidth,
                             valueHeight);
        }

      }
      else {
        image->Draw(draw,
                    this->x,
                    this->y+(height-image->GetHeight()) / 2);

        divider->DrawStretched(draw,
                               this->x+image->GetWidth()+
                               OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject),
                               this->y,
                               image->GetWidth(),
                               height);

        if (selection->HasSelection()) {
          PreparePainter(tablePainter);
          tablePainter->SetCell(1,selection->GetLine());
          tablePainter->Draw(draw,
                             this->x+width-valueWidth,
                             this->y+(height-valueHeight) / 2,
                             valueWidth,
                             valueHeight);
        }
      }
    }

    draw->activated=false;
    draw->focused=false;
    draw->disabled=false;
  }

  void Combo::Hide()
  {
    if (visible) {
      if (value!=NULL) {
        value->Hide();
      }

      Control::Hide();
    }
  }

  void Combo::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (tableModel.Valid()) {
      if (model==prevAction && prevAction->IsFinished()) {
        if (selection->HasSelection()) {
          if (selection->GetLine()>1) {
            SetTableRow(selection->GetLine()-1);
          }
        }
        else if (tableModel->GetRows()>0) {
          SetTableRow(1);
        }
      }
      else if (model==nextAction && nextAction->IsFinished()) {
        if (selection->HasSelection()) {
          if (selection->GetLine()<tableModel->GetRows()) {
            SetTableRow(selection->GetLine()+1);
          }
        }
        else if (tableModel->GetRows()>0) {
          SetTableRow(1);
        }
      }
      else if (model==tableModel) {
        SetSelectionFromModel();
        Redraw();
      }
      else if (model==GetModel()) {
        SetSelectionFromModel();
        Redraw();
      }
    }

    Control::Resync(model,msg);
  }

  TextCombo::TextCombo()
  : Combo(NULL,false),
    model(NULL)
  {
    // no code
  }

  TextCombo::~TextCombo()
  {
    // no code
  }

  bool TextCombo::SetModel(Base::Model* model)
  {
    this->model=dynamic_cast<Model::String*>(model);

    Combo::SetModel(this->model);

    return this->model.Valid();
  }

  /**
    This method will be called if a new value was selected from the popup
    window. The baseclass will try its best to assign a sensefull value to
    the model assigned to Combo. If the model is numemric it will
    assign the index of the current selceted (starting with 0), if its of type
    text, if will assign the text of the current selected (if the table model
    has more than one coloum if will iterate from left to right until some
    valid text will be returned).

    If you want some other (or more) behaviour, overwrite this method
    and call baseclass if necessary.
  */
  void TextCombo::SetModelFromSelection()
  {
    if (selection->HasSelection()) {
      if (dynamic_cast<const StringCellPainter*>(tablePainter.Get())!=NULL) {
        StringCellPainter* p=dynamic_cast<StringCellPainter*>(tablePainter.Get());

        PreparePainter(p);

        p->SetCell(1,selection->GetLine());
        model->Set(p->GetCellData());
      }
    }
    else {
      model->SetNull();
    }
  }

  void TextCombo::SetSelectionFromModel()
  {
    if (!selection.Valid()) {
      return;
    }

    if (!model.Valid()) {
      selection->Clear();
      return;
    }

    if (!tableModel.Valid()) {
      selection->Clear();
      return;
    }

    if (dynamic_cast<StringCellPainter*>(tablePainter.Get())==NULL) {
      selection->Clear();
      return;
    }

    StringCellPainter *p=dynamic_cast<StringCellPainter*>(tablePainter.Get());
    Model::String     *m=dynamic_cast<Model::String*>(model.Get());

    if (m==NULL) {
      selection->Clear();
      return;
    }

    if (m->IsNull()) {
      selection->Clear();
    }
    else {
      PreparePainter(p);

      for (size_t r=1; r<=tableModel->GetRows(); r++) {
        p->SetCell(1,r);

        if (m->Get()==p->GetCellData()) {
          selection->SelectLine(r);
          return;
        }
      }

      selection->Clear();
    }
  }

  IndexCombo::IndexCombo()
  : Combo(NULL,false),
    model(NULL)
  {
    // no code
  }

  IndexCombo::~IndexCombo()
  {
    // no code
  }

  bool IndexCombo::SetModel(Base::Model* model)
  {
    this->model=dynamic_cast<Model::Number*>(model);

    Combo::SetModel(this->model);

    return this->model.Valid();
  }

  void IndexCombo::SetModelFromSelection()
  {
    if (selection->HasSelection()) {
      model->Set(selection->GetLine());
    }
    else {
      this->model->SetNull();
    }
  }

  void IndexCombo::SetSelectionFromModel()
  {
    if (!selection.Valid()) {
      return;
    }

    if (!model.Valid()) {
      selection->Clear();
      return;
    }

    if (!tableModel.Valid()) {
      selection->Clear();
      return;
    }

    Model::Number *m=dynamic_cast<Model::Number*>(model.Get());

    if (m==NULL) {
      selection->Clear();
      return;
    }

    if (m->IsNull() || m->GetUnsignedLong()==0 || m->GetUnsignedLong()>tableModel->GetRows()) {
      selection->Clear();
    }
    else {
      selection->SelectLine(m->GetUnsignedLong());
    }
  }

  IndexCombo* IndexCombo::Create(Lum::Model::Number* index,
                                 Lum::Model::Table* table,
                                 bool horizontalFlex, bool verticalFlex)
  {
    IndexCombo *c=new IndexCombo();

    c->SetFlex(horizontalFlex,verticalFlex);
    c->SetModel(index);
    c->SetTableModel(table);

    return c;
  }

  TextEditCombo::TextEditCombo()
  : Combo(new String(),true),
    model(NULL)
  {
    string=dynamic_cast<String*>(value);
    string->SetFlex(true,true);
    string->SetCursorUpAction(prevAction);
    string->SetCursorDownAction(nextAction);
  }

  TextEditCombo::~TextEditCombo()
  {
    // no code
  }

  bool TextEditCombo::SetModel(Base::Model* model)
  {
    if (string->SetModel(model)) {
      this->model=dynamic_cast<Model::String*>(model);

      Combo::SetModel(this->model);

      return this->model.Valid();
    }
    else {
      return false;
    }
  }

  String* TextEditCombo::GetString() const
  {
    return string;
  }

  void TextEditCombo::SetModelFromSelection()
  {
    if (selection->HasSelection()) {
      if (dynamic_cast<const StringCellPainter*>(tablePainter.Get())!=NULL) {
        StringCellPainter* p=dynamic_cast<StringCellPainter*>(tablePainter.Get());

        PreparePainter(p);

        p->SetCell(1,selection->GetLine());
        model->Set(p->GetCellData());
      }
    }
    else {
      model->SetNull();
    }
  }

  void TextEditCombo::SetSelectionFromModel()
  {
    if (!selection.Valid()) {
      return;
    }

    if (!model.Valid()) {
      selection->Clear();
      return;
    }

    if (!tableModel.Valid()) {
      selection->Clear();
      return;
    }

    if (dynamic_cast<StringCellPainter*>(tablePainter.Get())==NULL) {
      selection->Clear();
      return;
    }

    StringCellPainter *p=dynamic_cast<StringCellPainter*>(tablePainter.Get());
    Model::String     *m=dynamic_cast<Model::String*>(model.Get());

    if (m==NULL) {
      selection->Clear();
      return;
    }

    if (m->IsNull()) {
      selection->Clear();
    }
    else {
      PreparePainter(p);

      for (size_t r=1; r<=tableModel->GetRows(); r++) {
        p->SetCell(1,r);

        if (m->Get()==p->GetCellData()) {
          selection->SelectLine(r);
          return;
        }
      }

      selection->Clear();
    }
  }
}

