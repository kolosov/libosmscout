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

#include <Lum/FileNameSelect.h>

#include <Lum/Dlg/File.h>

#include <Lum/String.h>

namespace Lum {

  FileNameSelect::FileNameSelect()
  : PopupControl(new String())
  {
    // no code
  }

  FileNameSelect::~FileNameSelect()
  {
   // no code
  }

  void FileNameSelect::SetDialogTitle(const std::wstring& title)
  {
    this->title=title;
  }

  std::wstring FileNameSelect::GetDialogTitle() const
  {
    return title;
  }

  void FileNameSelect::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==popupAction && popupAction->IsFinished()) {
      Model::StringRef stringModel=dynamic_cast<Model::String*>(control->GetModel());

      std::wstring value;

      if (stringModel.Valid() && !stringModel->IsNull()) {
        value=stringModel->Get();
      }

      if (Dlg::File::GetFile(GetWindow(),title,value)) {
        stringModel->Set(value);
      }
    }

    PopupControl::Resync(model,msg);
  }
}
