#ifndef LUM_DEF_BOOLEAN_H
#define LUM_DEF_BOOLEAN_H

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

#include <Lum/Private/ImportExport.h>

#include <Lum/Def/Property.h>

#include <Lum/Model/Boolean.h>

namespace Lum {

  namespace Def {
    /**
     * Definition of an boolean value and together with all necessary information to visualize this
      value in various contextes like dialogs, menu items and similar.
     */
    class LUMAPI Boolean : public Property
    {
    private:
      Model::BooleanRef value;

    public:
      Boolean(const Desc& description, Model::Boolean *value);

      Property* Copy() const;

      Model::Boolean* GetBoolean() const;
    };
  }
}

#endif

