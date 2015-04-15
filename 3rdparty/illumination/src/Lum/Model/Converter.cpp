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

#include <Lum/Model/Converter.h>

#include <iomanip>
#include <sstream>

#include <Lum/Model/String.h>

namespace Lum {
  namespace Model {

    StringConverter::~StringConverter()
    {
      // no code
    }

    bool StringStringConverter::ConvertModelToString(Lum::Base::Model* model, std::wstring& value)
    {
      Lum::Model::String* sourceModel=dynamic_cast<Lum::Model::String*>(model);

      if (sourceModel==NULL) {
        return false;
      }

      if (sourceModel->IsNull()) {
        value=L"";
        return true;
      }

      value=sourceModel->Get();

      return true;
    }

    bool StringStringConverter::ConvertStringToModel(const std::wstring& value, Lum::Base::Model* model)
    {
      Lum::Model::String* targetModel=dynamic_cast<Lum::Model::String*>(model);

      if (targetModel==NULL) {
        return false;
      }

      targetModel->Set(value);

      return true;
    }

    NumberStringConverter::NumberStringConverter()
    : floatFormat(floatAutomatic),
      precision(6)
    {
      // no code
    }

    NumberStringConverter::NumberStringConverter(FloatFormat floatFormat, unsigned long precision)
    : floatFormat(floatFormat),
      precision(precision)
    {
      // no code
    }

    bool NumberStringConverter::ConvertModelToString(Lum::Base::Model* model,
                                                     std::wstring& value)
    {
      Lum::Model::Number* sourceModel=dynamic_cast<Lum::Model::Number*>(model);

      if (sourceModel==NULL) {
        return false;
      }

      if (sourceModel->IsNull()) {
          value=L"";
        return true;
      }

      if (sourceModel->GetValueType()==Number::valueTypeSigned) {
        value=Base::NumberToWString(sourceModel->GetLong());
      }
      else if (sourceModel->GetValueType()==Number::valueTypeUnsigned) {
        value=Base::NumberToWString(sourceModel->GetUnsignedLong());
      }
      else if (sourceModel->GetValueType()==Number::valueTypeFloat) {
        std::ostringstream buffer;

        buffer.imbue(std::locale(""));

        if (floatFormat==floatScientific) {
          buffer << std::scientific;
        }
        else if (floatFormat==floatFixed) {
          buffer << std::fixed;
        }

        buffer << std::setprecision(precision) << sourceModel->GetDouble();

        value=Lum::Base::StringToWString(buffer.str());
      }
      else {
        assert(false);
      }

      return true;
    }

    bool NumberStringConverter::ConvertStringToModel(const std::wstring& value,
                                                     Lum::Base::Model* model)
    {
      Lum::Model::Number* targetModel=dynamic_cast<Lum::Model::Number*>(model);

      if (targetModel==NULL) {
        return false;
      }

      if (targetModel->GetValueType()==Number::valueTypeSigned) {
        long number;

        if (Base::WStringToNumber(value,number)) {
          targetModel->Set(number);
        }
        else {
          targetModel->SetNull();
        }
      }
      else if (targetModel->GetValueType()==Number::valueTypeUnsigned) {
        unsigned long number;

        if (Base::WStringToNumber(value,number)) {
          targetModel->Set(number);
        }
        else {
          targetModel->SetNull();
        }
      }
      else if (targetModel->GetValueType()==Number::valueTypeFloat) {
        double number;

        std::istringstream buffer(Lum::Base::WStringToString(value));

        buffer.imbue(std::locale(""));

        buffer >> number;

        if (buffer.eof()) {
          targetModel->Set(number);
        }
        else {
          targetModel->SetNull();
        }
      }
      else {
        assert(false);
      }

      return true;
    }
  }
}
