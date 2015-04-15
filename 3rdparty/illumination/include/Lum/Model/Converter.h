#ifndef LUM_MODEL_CONVERTER_H
#define LUM_MODEL_CONVERTER_H

/*
  This source is part of the Illumination library
  Copyright (C) 2010  Tim Teulings

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
#include <Lum/Base/Reference.h>
#include <Lum/Base/String.h>

#include <Lum/Model/Number.h>

namespace Lum {
  namespace Model {

    /**
     * Abstract base class for converting model values from and to string values.
     *
     * This is useful for using Lum::String* and similar input controls together
     * with non-text models like ones derived from Lum::Model::Number.
     *
     */
    class LUMAPI StringConverter : public Lum::Base::Referencable
    {
    public:
      virtual ~StringConverter();

      virtual bool ConvertModelToString(Lum::Base::Model* model, std::wstring& value) = 0;
      virtual bool ConvertStringToModel(const std::wstring& value, Lum::Base::Model* model) = 0;
    };

    typedef Base::Reference<StringConverter> StringConverterRef;

    class LUMAPI StringStringConverter : public StringConverter
    {
    public:
      bool ConvertModelToString(Lum::Base::Model* model, std::wstring& value);
      bool ConvertStringToModel(const std::wstring& value, Lum::Base::Model* model);
    };

    typedef Base::Reference<StringStringConverter> StringStringConverterRef;

    class LUMAPI NumberStringConverter : public StringConverter
    {
    public:
      enum FloatFormat {
        floatAutomatic,
        floatScientific,
        floatFixed
      };

    private:
      FloatFormat   floatFormat;
      unsigned long precision;

    public:
      NumberStringConverter();
      NumberStringConverter(FloatFormat floatFormat, unsigned long precision);

      bool ConvertModelToString(Lum::Base::Model* model, std::wstring& value);
      bool ConvertStringToModel(const std::wstring& value, Lum::Base::Model* model);
    };

    typedef Base::Reference<NumberStringConverter> NumberStringConverterRef;
  }
}

#endif
