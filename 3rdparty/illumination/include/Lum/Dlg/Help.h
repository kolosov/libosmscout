#ifndef LUM_DLG_HELP_H
#define LUM_DLG_HELP_H

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

#include <Lum/Dlg/ActionDialog.h>

#include <Lum/Edit/Text.h>

namespace Lum {
  namespace Dlg {

    class LUMAPI Help : public Dlg::ActionDialog
    {
    private:
      Edit::TextRef editContent;

    public:
      Help(const std::wstring& helpText);
      ~Help();

      Lum::Object* GetContent();
      void GetActions(std::vector<ActionInfo>& actions);

      void Resync(Base::Model* model, const Base::ResyncMsg& msg);

      static void Show(Dialog* parent, const std::wstring& helpText);
    };
  }
}
#endif
