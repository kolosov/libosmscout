/*
  This source is part of the Illumination library
  Copyright (C) 2006  Tim Teulings

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

#include <Lum/PopupControl.h>

#include <Lum/Panel.h>

namespace Lum {

  PopupControl::PopupControl(Control* control)
  : control(control),
    button(new Button()),
    popupAction(new Model::Action())
  {
    button->SetModel(popupAction);
    //button->SetType(Button::typeSmall);
    button->SetText(L"...");

    popupAction->Disable();

    Observe(popupAction);
  }

  PopupControl::~PopupControl()
  {
    // no code
  }

  bool PopupControl::HasBaseline() const
  {
    return control->HasBaseline();
  }

  size_t PopupControl::GetBaseline() const
  {
    if (control->HasBaseline()) {
      return GetTopBorder()+(height-control->GetOHeight())/2+control->GetBaseline();
    }
    else {
      return 0;
    }
  }

  bool PopupControl::SetModel(Base::Model* model)
  {
    bool res;

    if (control->GetModel()!=NULL) {
      Forget(control->GetModel());
    }

    res=control->SetModel(model);

    if (control->GetModel()) {
      Observe(control->GetModel());
    }

    return res;
  }

  Base::Model* PopupControl::GetModel() const
  {
    return control->GetModel();
  }

  void PopupControl::CalcSize()
  {
    Lum::Panel *panel;

    panel=new Lum::HPanel();

    control->SetFlex(true,false);
    panel->Add(control);
    panel->Add(button);

    container=panel;

    ControlComponent::CalcSize();
  }

  void PopupControl::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (control!=NULL && popupAction.Valid() && control->GetModel()==model) {
      if (model->IsEnabled()) {
        popupAction->Enable();
      }
      else {
        popupAction->Disable();
      }
    }

    Control::Resync(model,msg);
  }
}
