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

#include <Lum/DateSelect.h>

#include <Lum/Dlg/ValuePopup.h>

#include <Lum/DateView.h>

namespace Lum {

  DateSelect::DateSelect()
  : PopupControl(new DateView())
  {
    if (!OS::display->GetTheme()->FocusOnEditableControlsOnly()) {
      SetCanFocus(true);
      RequestFocus();
    }
  }

  DateSelect::~DateSelect()
  {
    // no code
  }

  void DateSelect::OpenPopup()
  {
    Dlg::DateValuePopup *popup;

    GetModel()->Push();

    popup=new Dlg::DateValuePopup();
    popup->SetParent(GetWindow());
    popup->SetReference(this);
    popup->SetModel(GetModel());

    if (popup->Open()) {
      popup->EventLoop();
      popup->Close();
    }

    delete popup;
    popup=NULL;

    GetModel()->Pop();
  }

  void DateSelect::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==popupAction && popupAction->IsFinished()) {
      OpenPopup();
    }

    PopupControl::Resync(model,msg);
  }

  DateSelect* DateSelect::Create(Base::Model* model, bool horizontalFlex, bool verticalFlex)
  {
    DateSelect* object;

    object=new DateSelect();
    object->SetFlex(horizontalFlex,verticalFlex);
    object->SetModel(model);

    return object;
  }
}

