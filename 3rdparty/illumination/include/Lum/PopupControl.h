#ifndef LUM_POPUPCONTROL_H
#define LUM_POPUPCONTROL_H

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

#include <Lum/Button.h>
#include <Lum/Object.h>

#include <Lum/Model/Action.h>

namespace Lum {

  class LUMAPI PopupControl : public ControlComponent
  {
  protected:
    Control          *control;
    Button           *button;
    Model::ActionRef popupAction;

  public:
    PopupControl(Control* control);
    ~PopupControl();

    bool HasBaseline() const;
    size_t GetBaseline() const;

    bool SetModel(Base::Model* model);
    Base::Model* GetModel() const;

    void CalcSize();

    void Resync(Base::Model* model, const Base::ResyncMsg& msg);
  };
}

#endif
