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

#include <Lum/Dlg/Value.h>

#include <Lum/String.h>

namespace Lum {
  namespace Dlg {

    TextInput::TextInput(Model::String* value, InputValidator *validator)
    {
      assert(value!=NULL);

      Value  v;

      v.label=L"";
      v.control=String::Create(true,false);
      v.model=value;
      v.validator=validator;

      AddValue(v);
    }

    bool TextInput::GetText(OS::Window* parent,
                           const std::wstring& caption,
                           const std::wstring& text,
                           Model::String* value,
                           InputValidator* validator)
    {
      assert(value!=NULL);

      TextInput *dlg;
      bool      res;

      dlg=new TextInput(value,validator);
      dlg->SetParent(parent);
      dlg->SetTitle(L""/*caption*/);
      dlg->SetCaption(caption);
      dlg->SetText(text);

      if (dlg->Open()) {
        dlg->EventLoop();
        dlg->Close();
      }

      res=dlg->HasResult();

      delete dlg;

      return res;
    }
  }
}
