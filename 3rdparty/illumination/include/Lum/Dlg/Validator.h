#ifndef LUM_DLG_VALIDATOR_H
#define LUM_DLG_VALIDATOR_H

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

#include <string>

#include <Lum/Base/Model.h>
#include <Lum/Base/String.h>

#include <Lum/Model/String.h>

namespace Lum {
  namespace Dlg {

    class LUMAPI InputValidator
    {
    public:
      virtual ~InputValidator();

      virtual bool IsValid(Base::Model* model) const = 0;
    };

    class LUMAPI InputValidatorStringNotEmpty : public InputValidator
    {
    public:
      bool IsValid(Base::Model* model) const;
    };

    template<class N>
    class LUMAPI InputValidatorIsInteger : public InputValidator
    {
    public:
      bool IsValid(Base::Model* model) const
      {
        if (model==NULL) {
          return false;
        }

        if (model->IsNull()) {
          return false;
        }

        Model::String *string;

        string=dynamic_cast<Model::String*>(model);

        if (string==NULL) {
          return false;
        }

        if (string->Empty()) {
          return false;
        }

        N value;

        return Base::WStringToNumber(string->Get(),value);
      }
    };

#if defined(LUM_INSTANTIATE_TEMPLATES)
    LUM_EXPTEMPL template class LUMAPI InputValidatorIsInteger<int>;
    LUM_EXPTEMPL template class LUMAPI InputValidatorIsInteger<unsigned int>;
    LUM_EXPTEMPL template class LUMAPI InputValidatorIsInteger<long>;
    LUM_EXPTEMPL template class LUMAPI InputValidatorIsInteger<unsigned long>;
    LUM_EXPTEMPL template class LUMAPI InputValidatorIsInteger<size_t>;
#endif

    typedef InputValidatorIsInteger<int> InputValidatorIsInt;
    typedef InputValidatorIsInteger<unsigned int> InputValidatorIsUInt;
    typedef InputValidatorIsInteger<long> InputValidatorIsLong;
    typedef InputValidatorIsInteger<unsigned long> InputValidatorIsULong;
    typedef InputValidatorIsInteger<size_t> InputValidatorIsSizeT;
  }
}

#endif
