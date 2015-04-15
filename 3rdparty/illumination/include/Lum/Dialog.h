#ifndef LUM_DIALOG_H
#define LUM_DIALOG_H

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

#include <Lum/Def/Menu.h>

#include <Lum/Manager/Behaviour.h>

#include <Lum/OS/Theme.h>

#include <Lum/Window.h>
#include <Lum/WindowGroup.h>

namespace Lum {

  class Dialog;

  /**
    Class for dialog-like windows, like main dialogs,
    sub dialogs, popups and toolbox dialogs.
  */
  class LUMAPI Dialog : public Window
  {
  private:
    Def::Menu                        *menuDef;
    Manager::Behaviour::CustomMenuHandler *customMenuHandler;
    Lum::Object                      *toolbar;
    Lum::Object                      *main;
    Lum::Object                      *statusLine;
    WindowGroup                      *group;
    bool                             seamless;

  private:
    void HandleMenuChange();

  public:
    Dialog();
    ~Dialog();

    void SetMenu(Def::Menu* menuDef);
    void SetToolbar(Lum::Object* toolbar);
    void SetMain(Lum::Object* main, bool seamless=false);
    void SetStatusLine(Lum::Object* statusLine);

    void CalcSize();

    void ReinitWindow();

    void Resync(Base::Model* model,
                const Base::ResyncMsg& msg);
  };
}

#endif
