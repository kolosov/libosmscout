/*
  This source is part of the Illumination library
  Copyright (C) 200  Tim Teulings

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

#include <Lum/Def/OneOfN.h>

namespace Lum {
  namespace Def {
    OneOfN::Choice::Choice(const Desc& description)
     : description(description)
    {
      // no code
    }

    const Desc& OneOfN::Choice::GetDesc() const
    {
      return description;
    }

    OneOfN::OneOfN(const Desc& description, Model::Number* value)
     : Property(description),
       value(value)
    {
      assert(value!=NULL);
    }

    Property* OneOfN::Copy() const
    {
      OneOfN *result=new OneOfN(GetDesc(),
                                value);

      result->choices=choices;

      return result;
    }

    OneOfN& OneOfN::AddChoice(const Choice& choice)
    {
      choices.push_back(choice);

      return *this;
    }

    Model::Number* OneOfN::GetValue() const
    {
      return value;
    }

    const std::list<OneOfN::Choice>& OneOfN::GetChoices() const
    {
      return choices;
    }
  }
}

