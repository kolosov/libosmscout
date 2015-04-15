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

#include <Lum/Dlg/ValuePopup.h>

#include <Lum/Model/String.h>

#include <Lum/ButtonRow.h>
#include <Lum/Panel.h>
#include <Lum/Time.h>

namespace Lum {
  namespace Dlg {

    ValuePopup* ValuePopup::SetModel(Base::Model* model)
    {
      this->model=model;

      return this;
    }

    Base::Model* ValuePopup::GetModel() const
    {
      return model;
    }

    DateValuePopup::DateValuePopup()
    : picker(new DatePicker())
    {
      Observe(picker->GetSelectAction());
    }

    void DateValuePopup::PreInit()
    {
      picker->SetFlex(true,true);
      picker->SetModel(GetModel());

      SetMain(picker,true,true);

      Popup::PreInit();
    }

    void DateValuePopup::Resync(Base::Model* model, const Base::ResyncMsg& msg)
    {
      if (model==picker->GetSelectAction() &&
          picker->GetSelectAction()->IsFinished() &&
          GetModel()!=NULL) {
        GetModel()->Save();
        Exit();
      }

      Popup::Resync(model,msg);
    }

    TimeValuePopup::TimeValuePopup(Base::TimeFormat format)
     : format(format),
       commit(new Model::Action())
    {
      Observe(commit);
    }

    void TimeValuePopup::PreInit()
    {
      Time *picker;

      picker=new Time();
      picker->SetFormat(format);
      picker->SetFlex(true,true);
      picker->SetModel(GetModel());

      SetMain(VPanel::Create(true,true)
              ->Add(picker)
              ->AddSpace()
              ->Add(ButtonRow::CreateOk(commit,true,false)),
              false,true);

      Popup::PreInit();
    }

    void TimeValuePopup::Resync(Base::Model* model, const Base::ResyncMsg& msg)
    {
      if (model==commit && commit->IsFinished()) {
        GetModel()->Save();
        Exit();
      }

      Popup::Resync(model,msg);
    }

    TableStringValuePopup::TableStringValuePopup(Lum::Model::Table* table,
                                                 Lum::Model::Header* header)
    : table(NULL),
      tableModel(table),
      headerModel(header),
      selectionModel(new Model::SingleLineSelection)
    {
      Observe(GetPreOpenedAction());
      Observe(selectionModel);

      if (!headerModel.Valid()) {
        headerModel=new Model::HeaderImpl();
        headerModel->AddColumn(L"",Lum::Base::Size::pixel,0);
      }
    }

    void TableStringValuePopup::PositionSelection()
    {
      Model::StringRef model=dynamic_cast<Model::String*>(GetModel());

      if (!model.Valid() ||
          model->IsNull() ||
          !tableModel.Valid()) {
        selectionModel->Clear();
        return;
      }

      if (dynamic_cast<Model::StringTable*>(tableModel.Get())!=NULL) {
        Model::StringTable* tm=dynamic_cast<Model::StringTable*>(tableModel.Get());

        for (size_t i=1; i<tm->GetRows(); i++) {
          if (model->Get()==tm->GetEntry(i)) {
            selectionModel->SelectLine(i);
            table->GetTableView()->MakeVisible(1,i);
            return;
          }
        }
      }

      selectionModel->Clear();
    }

    void TableStringValuePopup::CopySelection()
    {
      Model::StringRef model=dynamic_cast<Model::String*>(GetModel());

      if (!model.Valid() ||
          model->IsNull() ||
          !tableModel.Valid() ||
          !selectionModel->HasSelection()) {
        model->SetNull();
        model->Save();

        return;
      }

      if (dynamic_cast<Model::StringTable*>(tableModel.Get())!=NULL) {
        Model::StringTable* tm=dynamic_cast<Model::StringTable*>(tableModel.Get());

        model->Set(tm->GetEntry(selectionModel->GetLine()));
      }

      model->Save();
    }

    void TableStringValuePopup::PreInit()
    {
      table=new Lum::Table();
      table->SetFlex(true,true);

      if (OS::display->GetTheme()->PopupsAreDialogs()) {
        table->SetMaxWidth(Base::Size::workHRel,100);
        table->SetMaxHeight(Base::Size::workVRel,100);
      }
      else {
        table->SetMaxWidth(Base::Size::workHRel,80);
        table->SetMaxHeight(Base::Size::workVRel,40);
      }

      if (headerModel.Valid()){
        table->GetTableView()->SetAutoFitColumns(true);
        table->SetHeaderModel(headerModel);
        table->GetTableView()->SetAutoHSize(true);
      }

      table->SetModel(tableModel);
      table->SetSelection(selectionModel);
      table->GetTableView()->SetAutoVSize(true);
      RegisterCommitShortcut(table->GetTableView(),
                             table->GetTableView()->GetMouseSelectionAction());

      Observe(table->GetTableView()->GetMouseSelectionAction());

      SetMain(table,true,true);

      Popup::PreInit();
    }

    void TableStringValuePopup::Resync(Base::Model* model, const Base::ResyncMsg& msg)
    {
      if (table!=NULL && IsOpen() &&
          model==table->GetTableView()->GetMouseSelectionAction() &&
          table->GetTableView()->GetMouseSelectionAction()->IsFinished()) {
        CopySelection();
        Exit();
      }
      else if (model==GetPreOpenedAction() &&
               GetPreOpenedAction()->IsFinished()) {
        PositionSelection();
      }

      Popup::Resync(model,msg);
    }

    TableIndexValuePopup::TableIndexValuePopup(Lum::Model::Table* table,
                                               TableCellPainter* painter,
                                               Lum::Model::Header* header)
    : table(NULL),
      tableModel(table),
      painter(painter),
      headerModel(header),
      selectionModel(new Model::SingleLineSelection)
    {
      Observe(GetPreOpenedAction());
      Observe(selectionModel);

      if (!headerModel.Valid()) {
        headerModel=new Model::HeaderImpl();
        headerModel->AddColumn(L"",Lum::Base::Size::pixel,0);
      }
    }

    void TableIndexValuePopup::PositionSelection()
    {
      Model::NumberRef model=dynamic_cast<Model::Number*>(GetModel());

      if (!model.Valid() ||
          model->IsNull() ||
          !tableModel.Valid()) {
        selectionModel->Clear();
        return;
      }

      selectionModel->SelectLine(model->GetUnsignedLong());
      table->GetTableView()->MakeVisible(1,model->GetUnsignedLong());
    }

    void TableIndexValuePopup::CopySelection()
    {
      Model::NumberRef model=dynamic_cast<Model::Number*>(GetModel());

      if (!model.Valid() ||
          model->IsNull() ||
          !tableModel.Valid() ||
          !selectionModel->HasSelection()) {
        model->SetNull();
        model->Save();
        return;
      }

      model->Set(selectionModel->GetLine());
      model->Save();
    }

    void TableIndexValuePopup::PreInit()
    {
      table=new Lum::Table();
      table->SetFlex(true,true);

      if (OS::display->GetTheme()->PopupsAreDialogs()) {
        table->SetMaxWidth(Base::Size::workHRel,100);
        table->SetMaxHeight(Base::Size::workVRel,100);
      }
      else {
        table->SetMaxWidth(Base::Size::workHRel,80);
        table->SetMaxHeight(Base::Size::workVRel,40);
      }

      if (headerModel.Valid()){
        table->SetAutoFitColumns(true);
        table->SetHeaderModel(headerModel);
        table->SetAutoHSize(true);
      }

      table->SetModel(tableModel);
      table->SetSelection(selectionModel);
      table->SetPainter(painter);
      table->SetAutoVSize(true);
      RegisterCommitShortcut(table->GetTableView(),
                             table->GetTableView()->GetMouseSelectionAction());

      Observe(table->GetTableView()->GetMouseSelectionAction());

      SetMain(table,true,true);

      Popup::PreInit();
    }

    void TableIndexValuePopup::Resync(Base::Model* model, const Base::ResyncMsg& msg)
    {
      if (table!=NULL && IsOpen() &&
          model==table->GetTableView()->GetMouseSelectionAction() &&
          table->GetTableView()->GetMouseSelectionAction()->IsFinished()) {
        CopySelection();
        Exit();
      }
      else if (model==GetPreOpenedAction() &&
               GetPreOpenedAction()->IsFinished()) {
        PositionSelection();
      }

      Popup::Resync(model,msg);
    }
  }
}

