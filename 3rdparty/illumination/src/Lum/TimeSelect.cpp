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

#include <Lum/TimeSelect.h>

#include <Lum/Base/L10N.h>

#include <Lum/Dlg/ValuePopup.h>

#include <Lum/TimeView.h>

namespace Lum {

  TimeSelect::TimeSelect()
  : PopupControl(new TimeView()),
    format(Base::timeFormatHM)
  {
    SetCanFocus(true);
  }

  TimeSelect::~TimeSelect()
  {
    // no code
  }

  void TimeSelect::SetFormat(Base::TimeFormat format)
  {
    assert(!inited);

    this->format=format;
    dynamic_cast<TimeView*>(control)->SetFormat(format);
  }

  void TimeSelect::OpenPopup()
  {
    Dlg::TimeValuePopup *popup;

    GetModel()->Push();

    popup=new Dlg::TimeValuePopup(format);
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

  void TimeSelect::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==popupAction &&
        popupAction->IsFinished()) {
      OpenPopup();
    }

    PopupControl::Resync(model,msg);
  }

  TimeSelect* TimeSelect::Create(Base::Model* model, bool horizontalFlex, bool verticalFlex)
  {
    TimeSelect* object;

    object=new TimeSelect();
    object->SetFlex(horizontalFlex,verticalFlex);
    object->SetModel(model);

    return object;
  }
}

