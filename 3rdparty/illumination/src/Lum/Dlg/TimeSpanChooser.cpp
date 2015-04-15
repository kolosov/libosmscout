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

#include <Lum/Dlg/TimeSpanChooser.h>

#include <cassert>

#include <Lum/Panel.h>
#include <Lum/TimeSpan.h>

namespace Lum {
  namespace Dlg {

    TimeSpanChooser::TimeSpanChooser(Model::Number* seconds)
     : seconds(seconds),
       okAction(new Model::Action()),
       success(false)
    {
      assert(seconds!=NULL);

      seconds->Push();

      Observe(okAction);
      Observe(GetClosedAction());
    }

    TimeSpanChooser::~TimeSpanChooser()
    {
      seconds->Pop();
    }

    Object* TimeSpanChooser::GetContent()
    {
      HPanel *panel;

      panel=HPanel::Create(true,true);

      panel->Add(TimeSpan::Create(seconds));

      return panel;
    }

    void TimeSpanChooser::GetActions(std::vector<Dlg::ActionInfo>& actions)
    {
      ActionDialog::CreateActionInfosOkCancel(actions,okAction,GetClosedAction());
    }

    void TimeSpanChooser::Resync(Base::Model *model, const Base::ResyncMsg& msg)
    {
      if (model==GetClosedAction() &&
          GetClosedAction()->IsFinished()) {
        Exit();
      }
      else if (model==okAction &&
               okAction->IsFinished()) {
        success=true;
        seconds->Save();
        Exit();
      }

      ActionDialog::Resync(model,msg);
    }

    bool TimeSpanChooser::Success() const
    {
      return success;
    }
  }
}

