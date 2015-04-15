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

#include <Lum/Dlg/ColorChooser.h>

#include <Lum/Base/L10N.h>

#include <Lum/ButtonRow.h>
#include <Lum/ColorView.h>
#include <Lum/Panel.h>
#include <Lum/RGBControl.h>

namespace Lum {
  namespace Dlg {
    ColorChooser::ColorChooser(Model::Color* color)
    : color(color),
      okAction(new Lum::Model::Action)
    {
      Observe(okAction);
      Observe(GetClosedAction());
    }

    ColorChooser::~ColorChooser()
    {
      // no code
    }

    void ColorChooser::PreInit()
    {
      ColorView   *colorView;
      Panel       *hPanel;
      RGBControl  *rgb;

      hPanel=HPanel::Create(true,false);

      colorView=new ColorView();
      colorView->SetFlex(true,true);
      colorView->SetModel(color);
      hPanel->Add(colorView);

      hPanel->AddSpace();

      rgb=new RGBControl();
      rgb->SetModel(color);
      hPanel->Add(rgb);

      SetMain(VPanel::Create(true,false)
              ->Add(hPanel)
              ->AddSpace()
              ->Add(ButtonRow::CreateOkCancel(okAction,GetClosedAction(),true,false)));

      Dialog::PreInit();
    }

    void ColorChooser::Resync(Lum::Base::Model* model, const Lum::Base::ResyncMsg& msg)
    {
      if (model==okAction && okAction->IsFinished()) {
        color->Save();
        Exit();
      }
      else if (model==GetClosedAction() && GetClosedAction()->IsFinished()) {
        Exit();
      }
      else {
        Dialog::Resync(model,msg);
      }
    }

    void ColorChooser::GetColor(OS::Window* parent,
                                Model::Color* color)
    {
      assert(color!=NULL);

      ColorChooser *dlg;

      color->Push();

      dlg=new ColorChooser(color);
      dlg->SetParent(parent);
      dlg->SetTitle(L""/*caption*/);

      if (dlg->Open()) {
        dlg->EventLoop();
        dlg->Close();
      }

      delete dlg;

      color->Pop();
    }
  }
}
