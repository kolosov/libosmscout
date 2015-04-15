#ifndef LUM_DLG_ABOUT_H
#define LUM_DLG_ABOUT_H

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

#include <Lum/Private/ImportExport.h>

#include <Lum/Def/AppInfo.h>

#include <Lum/Dlg/ActionDialog.h>

#include <Lum/Model/Action.h>

namespace Lum {
  namespace Dlg {
    class LUMAPI About : public ActionDialog
    {
    private:
      Def::AppInfo info;

    public:
      About();
      ~About();

      Lum::Object* GetContent();
      void GetActions(std::vector<Lum::Dlg::ActionInfo>& actions);

      void SetAppInfo(const Def::AppInfo& info);

      static void Show(Dialog* parent, const Def::AppInfo& info);
      static void Show(OS::Window* parent, const Def::AppInfo& info);
    };
  }
}

#endif
