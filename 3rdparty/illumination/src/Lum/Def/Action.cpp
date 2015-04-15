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

#include <Lum/Def/Action.h>

namespace Lum {
  namespace Def {
    Action::Action(const Desc& description, Model::Action *action)
     : description(description),
       action(action),
       type(normal),
       opensDialog(false)
    {
      assert(action!=NULL);
    }

    Action& Action::SetImage(const std::wstring& imageFilename)
    {
      this->imageFilename=imageFilename;
      
      return *this;
    }

    std::wstring Action::GetImageFilename() const
    {
      return imageFilename;
    }

    Action& Action::SetType(Type type)
    {
      this->type=type;

      return *this;
    }

    Action::Type Action::GetType() const
    {
      return type;
    }

    Model::Action* Action::GetAction() const
    {
      return action;
    }

    const Desc& Action::GetDesc() const
    {
      return description;
    }

    Action& Action::SetOpensDialog()
    {
      opensDialog=true;

      return *this;
    }

    bool Action::GetOpensDialog() const
    {
      return opensDialog;
    }
  }
}

