#ifndef LUM_DEF_ACTION_H
#define LUM_DEF_ACTION_H

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

#include <string>

#include <Lum/Private/ImportExport.h>

#include <Lum/Def/Desc.h>

#include <Lum/Images/StatefulImage.h>

#include <Lum/Model/Action.h>

namespace Lum {

  namespace Def {
    /**
     * Definition of an action and together with all necessary information to
       visualize this action in various contextes like dialog global actions,
       buttons, menu items and similar.
     */
    class LUMAPI Action
    {
    public:
      enum Type {
        normal,
        positive,
        negative
      };

    private:
      Desc             description;
      Model::ActionRef action;
      std::wstring     imageFilename;
      Type             type;
      bool             opensDialog;

    public:
      Action(const Desc& description, Model::Action *action);

      Model::Action* GetAction() const;
      const Desc& GetDesc() const;
      
      Action& SetImage(const std::wstring& filename);
      std::wstring GetImageFilename() const;

      Action& SetType(Type type);
      Type GetType() const;

      Action& SetOpensDialog();
      bool GetOpensDialog() const;
    };
  }
}

#endif

