#ifndef LUM_POPUP_H
#define LUM_POPUP_H

/*
  This source is part of the Illumination library
  Copyright (C) 2008  Tim Teulings

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

#include <Lum/Window.h>

#include <Lum/PopupGroup.h>

namespace Lum {

  class Popup;

  /**
    Class for dialog-like windows, like main dialogs,
    sub dialogs, popups and toolbox dialogs.
  */
  class LUMAPI Popup : public Window
  {
  private:
    PopupGroup  *container;
    Lum::Object *main;
    Lum::Object *reference;
    OS::FillRef background;
    bool        seamless;
    bool        border;

  public:
    Popup();
    ~Popup();

    void SetMain(Lum::Object* main, bool seamless=true, bool border=true);
    void SetBackground(OS::Fill* background);
    void SetReference(Lum::Object* object);
    Lum::Object* GetReference() const;

    void CalcSize();

    void ReinitWindow();

    bool Open(bool activate=true);

    void Resync(Base::Model* model,
                const Base::ResyncMsg& msg);
  };
}

#endif
