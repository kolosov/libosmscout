/*
  This source is part of the Illumination library
  Copyright (C) 2004  Tim Teulings

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

#include <Lum/Model/Number.h>

#include <cassert>
#include <cstdlib>
#include <cmath>
#include <iostream>
namespace Lum {
  namespace Model {

    class Number::SimpleValue
    {
    public:
      virtual ~SimpleValue()
      {
        // no code
      }
    };

    class Number::SignedSimpleValue : public SimpleValue
    {
    private:
      long value;

    public:
      SignedSimpleValue(long value)
      : value(value)
      {
        // no code
      }

      long GetValue() const
      {
        return value;
      }
    };

    class Number::UnsignedSimpleValue : public SimpleValue
    {
    private:
      unsigned long value;

    public:
      UnsignedSimpleValue(unsigned long value)
      : value(value)
      {
        // no code
      }

      unsigned long GetValue() const
      {
        return value;
      }
    };

    class Number::FloatSimpleValue : public SimpleValue
    {
    private:
      double value;

    public:
      FloatSimpleValue(double value)
      : value(value)
      {
        // no code
      }

      double GetValue() const
      {
        return value;
      }
    };

    class Number::Value
    {
    private:
      bool isNull;

    public:
      Value()
      : isNull(true)
      {
        // no code
      }

      Value(SimpleValue* value)
      {
        // no code
      }

      Value(Value* value)
      {
        // no code
      }

      Value(long value, SimpleValue* min, SimpleValue* max)
      {
        // no code
      }

      Value(unsigned long value, SimpleValue* min, SimpleValue* max)

      {
        // no code
      }

      Value(double value, SimpleValue* min, SimpleValue* max)
      {
        // no code
      }

      virtual ~Value()
      {
        // no code
      }

      void SetNull(bool isNull)
      {
        this->isNull=isNull;
      }

      bool IsNull() const
      {
        return isNull;
      }

      virtual bool Add(long value, SimpleValue* min, SimpleValue* max) = 0;
      virtual bool Add(unsigned long value, SimpleValue* min, SimpleValue* max) = 0;
      virtual bool Add(double value, SimpleValue* min, SimpleValue* max) = 0;

      virtual bool Sub(long value, SimpleValue* min, SimpleValue* max) = 0;
      virtual bool Sub(unsigned long value, SimpleValue* min, SimpleValue* max) = 0;
      virtual bool Sub(double value, SimpleValue* min, SimpleValue* max) = 0;

      virtual long GetLong() const = 0;
      virtual unsigned long GetUnsignedLong() const = 0;
      virtual double GetDouble() const = 0;

      virtual bool Equals(Value* other) const = 0;
    };

    class Number::SignedValue : public Number::Value
    {
    private:
      long value;

    public:
      SignedValue()
      {
        // no code
      }

      SignedValue(SimpleValue* value)
      {
        SetNull(false);
        this->value=dynamic_cast<SignedSimpleValue*>(value)->GetValue();
      }

      SignedValue(Value* value)
      {
        if (value->IsNull()) {
          SetNull(true);
        }
        else {
          SetNull(false);
          this->value=value->GetLong();
        }
      }

      SignedValue(long value, SimpleValue* min, SimpleValue* max)
      {
        if (value<dynamic_cast<SignedSimpleValue*>(min)->GetValue() ||
            value>dynamic_cast<SignedSimpleValue*>(max)->GetValue()) {
          SetNull(true);
          return;
        }

        this->value=value;
        SetNull(false);
      }

      SignedValue(unsigned long value, SimpleValue* min, SimpleValue* max)
      {
        if (value>(unsigned long)std::numeric_limits<long>::max()) {
          SetNull(true);
          return;
        }

        if (dynamic_cast<SignedSimpleValue*>(max)->GetValue()<0) {
          SetNull(true);
          return;
        }

        if (value<(unsigned long)std::max(0l,dynamic_cast<SignedSimpleValue*>(min)->GetValue()) ||
            value>(unsigned long)std::max(0l,dynamic_cast<SignedSimpleValue*>(max)->GetValue())) {
          SetNull(true);
          return;
        }

        this->value=(unsigned long)value;
        SetNull(false);
      }

      SignedValue(double value, SimpleValue* min, SimpleValue* max)
      {
        if (((double)value)<(double)dynamic_cast<SignedSimpleValue*>(min)->GetValue() ||
            ((double)value)>(double)dynamic_cast<SignedSimpleValue*>(max)->GetValue()) {
          SetNull(true);
          return;
        }

        this->value=(long)value;
        SetNull(false);
      }

      bool Add(long value, SimpleValue* min, SimpleValue* max)
      {
        if (value>=0) {
          if (dynamic_cast<SignedSimpleValue*>(max)->GetValue()-value<this->value) {
            SetNull(true);
            return false;
          }

          this->value+=value;
          SetNull(false);
        }
        else {
          if (dynamic_cast<SignedSimpleValue*>(min)->GetValue()-value>=this->value) {
            SetNull(true);
            return false;
          }

          this->value+=value;
          SetNull(false);
        }

        return true;
      }

      bool Add(unsigned long value, SimpleValue* min, SimpleValue* max)
      {
        if (value>(unsigned long)std::numeric_limits<long>::max()) {
          SetNull(true);
          return false;
        }

        if (dynamic_cast<SignedSimpleValue*>(max)->GetValue()-(long)value<this->value) {
          SetNull(true);
          return false;
        }

        this->value+=value;
        SetNull(false);

        return true;
      }
      bool Add(double value, SimpleValue* min, SimpleValue* max)
      {
        // value is bigger than our [min,max] interval
        if ((double)dynamic_cast<SignedSimpleValue*>(max)->GetValue()-
            (double)dynamic_cast<SignedSimpleValue*>(min)->GetValue()<std::abs(value)) {
          SetNull(true);
          return false;
        }

        if (value>=0) {
          if (dynamic_cast<SignedSimpleValue*>(max)->GetValue()-(long)value<this->value) {
            SetNull(true);
            return false;
          }

          this->value+=(long)value;
          SetNull(false);
        }
        else {
          if (dynamic_cast<SignedSimpleValue*>(min)->GetValue()-(long)value>this->value) {
            SetNull(true);
            return false;
          }

          this->value+=(long)value;
          SetNull(false);
        }

        return true;
      }

      bool Sub(long value, SimpleValue* min, SimpleValue* max)
      {
        if (value>=0) {
          if (dynamic_cast<SignedSimpleValue*>(min)->GetValue()+value>this->value) {
            SetNull(true);
            return false;
          }

          this->value-=value;
          SetNull(false);
        }
        else {
          if (dynamic_cast<SignedSimpleValue*>(max)->GetValue()+value<this->value) {
            SetNull(true);
            return false;
          }

          this->value-=value;
          SetNull(false);
        }

        return true;
      }

      bool Sub(unsigned long value, SimpleValue* min, SimpleValue* max)
      {
        if (value>(unsigned long)std::numeric_limits<long>::max()) {
          SetNull(true);
          return false;
        }

        // value is bigger than our [min,max] interval
        if (dynamic_cast<SignedSimpleValue*>(max)->GetValue()-
            dynamic_cast<SignedSimpleValue*>(min)->GetValue()<(long)value) {
          SetNull(true);
          return false;
        }

        if (dynamic_cast<SignedSimpleValue*>(min)->GetValue()+(long)value>this->value) {
          SetNull(true);
          return false;
        }

        this->value-=value;
        SetNull(false);

        return true;
      }

      bool Sub(double value, SimpleValue* min, SimpleValue* max)
      {
        // value is bigger than our [min,max] interval
        if ((double)dynamic_cast<SignedSimpleValue*>(max)->GetValue()-
            (double)dynamic_cast<SignedSimpleValue*>(min)->GetValue()<std::abs(value)) {
          SetNull(true);
          return false;
        }

        if (value>=0) {
          if (dynamic_cast<SignedSimpleValue*>(min)->GetValue()+(long)value>this->value) {
            SetNull(true);
            return false;
          }

          this->value-=(long)value;
          SetNull(false);
        }
        else {
          if (dynamic_cast<SignedSimpleValue*>(max)->GetValue()+(long)value<this->value) {
            SetNull(true);
            return false;
          }

          this->value+=(long)value;
          SetNull(false);
        }

        return true;
      }

      long GetLong() const
      {
        assert(!IsNull());

        return value;
      }

      unsigned long GetUnsignedLong() const
      {
        assert(!IsNull());
        assert(value>=0);

        return value;
      }

      double GetDouble() const
      {
        assert(!IsNull());

        return value;
      }

      bool Equals(Value* other) const
      {
        if (IsNull() && other->IsNull()) {
          return true;
        }
        else if (IsNull()!=other->IsNull()) {
          return false;
        }
        else {
          return value==other->GetLong();
        }
      }
    };

    class Number::UnsignedValue : public Number::Value
    {
    private:
      unsigned long value;

    public:
      UnsignedValue()
      {
        // no code
      }

      UnsignedValue(SimpleValue* value)
      {
        SetNull(false);
        this->value=dynamic_cast<UnsignedSimpleValue*>(value)->GetValue();
      }

      UnsignedValue(Value* value)
      {
        if (value->IsNull()) {
          SetNull(true);
        }
        else {
          SetNull(false);
          this->value=value->GetUnsignedLong();
        }
      }

      UnsignedValue(long value, SimpleValue* min, SimpleValue* max)
      {
        if (value<0) {
          SetNull(true);
          return;
        }

        if (((unsigned long)value)<dynamic_cast<UnsignedSimpleValue*>(min)->GetValue() ||
            ((unsigned long)value)>dynamic_cast<UnsignedSimpleValue*>(max)->GetValue()) {
          SetNull(true);
          return;
        }

        this->value=(unsigned long)value;
        SetNull(false);
      }

      UnsignedValue(unsigned long value, SimpleValue* min, SimpleValue* max)
      {
        if ((value<dynamic_cast<UnsignedSimpleValue*>(min)->GetValue() ||
             value>dynamic_cast<UnsignedSimpleValue*>(max)->GetValue())) {
          SetNull(true);
          return;
        }

        this->value=(unsigned long)value;
        SetNull(false);
      }

      UnsignedValue(double value, SimpleValue* min, SimpleValue* max)
      {
        if (((double)value)<(double)dynamic_cast<UnsignedSimpleValue*>(min)->GetValue() ||
            ((double)value)>(double)dynamic_cast<UnsignedSimpleValue*>(max)->GetValue()) {
          SetNull(true);
          return;
        }

        this->value=(unsigned long)value;
        SetNull(false);
      }

      bool Add(long value, SimpleValue* min, SimpleValue* max)
      {
        if (value>=0) {
          if (dynamic_cast<UnsignedSimpleValue*>(max)->GetValue()-value<this->value) {
            SetNull(true);
            return false;
          }

          this->value+=value;
          SetNull(false);
        }
        else {
          if (dynamic_cast<UnsignedSimpleValue*>(min)->GetValue()-value>=this->value) {
            SetNull(true);
            return false;
          }

          this->value+=value;
          SetNull(false);
        }

        return true;
      }

      bool Add(unsigned long value, SimpleValue* min, SimpleValue* max)
      {
        if (dynamic_cast<UnsignedSimpleValue*>(max)->GetValue()-value<this->value) {
          SetNull(true);
          return false;
        }

        this->value+=value;
        SetNull(false);

        return true;
      }
      bool Add(double value, SimpleValue* min, SimpleValue* max)
      {
        // value is bigger than our [min,max] interval
        if ((double)dynamic_cast<UnsignedSimpleValue*>(max)->GetValue()-
            (double)dynamic_cast<UnsignedSimpleValue*>(min)->GetValue()<std::abs(value)) {
          SetNull(true);
          return false;
        }

        if (value>=0) {
          if (dynamic_cast<UnsignedSimpleValue*>(max)->GetValue()-(unsigned long)value<this->value) {
            SetNull(true);
            return false;
          }

          this->value+=(unsigned long)value;
          SetNull(false);
        }
        else {
          if (dynamic_cast<UnsignedSimpleValue*>(min)->GetValue()-(unsigned long)value>this->value) {
            SetNull(true);
            return false;
          }

          this->value+=(unsigned long)value;
          SetNull(false);
        }

        return true;
      }

      bool Sub(long value, SimpleValue* min, SimpleValue* max)
      {
        if (value>=0) {
          if (dynamic_cast<UnsignedSimpleValue*>(min)->GetValue()+value>this->value) {
            SetNull(true);
            return false;
          }

          this->value-=value;
          SetNull(false);
        }
        else {
          if (dynamic_cast<UnsignedSimpleValue*>(max)->GetValue()+value<this->value) {
            SetNull(true);
            return false;
          }

          this->value-=value;
          SetNull(false);
        }

        return true;
      }

      bool Sub(unsigned long value, SimpleValue* min, SimpleValue* max)
      {
        if (dynamic_cast<UnsignedSimpleValue*>(min)->GetValue()+value>this->value) {
          SetNull(true);
          return false;
        }

        this->value-=value;
        SetNull(false);

        return true;
      }

      bool Sub(double value, SimpleValue* min, SimpleValue* max)
      {
        // value is bigger than our [min,max] interval
        if ((double)dynamic_cast<UnsignedSimpleValue*>(max)->GetValue()-
            (double)dynamic_cast<UnsignedSimpleValue*>(min)->GetValue()<std::abs(value)) {
          SetNull(true);
          return false;
        }

        if (value>=0) {
          if (dynamic_cast<UnsignedSimpleValue*>(min)->GetValue()+(unsigned long)value>this->value) {
            SetNull(true);
            return false;
          }

          this->value-=(unsigned long)value;
          SetNull(false);
        }
        else {
          if (dynamic_cast<UnsignedSimpleValue*>(max)->GetValue()+(unsigned long)value<this->value) {
            SetNull(true);
            return false;
          }

          this->value+=(unsigned long)value;
          SetNull(false);
        }

        return true;
      }

      long GetLong() const
      {
        assert(!IsNull());

        return (long)value;
      }

      unsigned long GetUnsignedLong() const
      {
        assert(!IsNull());

        return value;
      }

      double GetDouble() const
      {
        assert(!IsNull());

        return value;
      }

      bool Equals(Value* other) const
      {
        if (IsNull() && other->IsNull()) {
          return true;
        }
        else if (IsNull()!=other->IsNull()) {
          return false;
        }
        else {
          return value==other->GetUnsignedLong();
        }
      }
    };

    class Number::FloatValue : public Number::Value
    {
    private:
      double value;

    public:
      FloatValue()
      {
        // no code
      }

      FloatValue(SimpleValue* value)
      {
        SetNull(false);
        this->value=dynamic_cast<FloatSimpleValue*>(value)->GetValue();
      }

      FloatValue(Value* value)
      {
        if (value->IsNull()) {
          SetNull(true);
        }
        else {
          SetNull(false);
          this->value=value->GetDouble();
        }
      }

      FloatValue(long value, SimpleValue* min, SimpleValue* max)
      {
        if (value<dynamic_cast<FloatSimpleValue*>(min)->GetValue() ||
            value>dynamic_cast<FloatSimpleValue*>(max)->GetValue()) {
          SetNull(true);
          return;
        }

        this->value=(long)value;
        SetNull(false);
      }

      FloatValue(unsigned long value, SimpleValue* min, SimpleValue* max)
      {
        if (value<dynamic_cast<FloatSimpleValue*>(min)->GetValue() ||
            value>dynamic_cast<FloatSimpleValue*>(max)->GetValue()) {
          SetNull(true);
          return;
        }

        this->value=(unsigned long)value;
        SetNull(false);
      }

      FloatValue(double value, SimpleValue* min, SimpleValue* max)
      {
        if (value<dynamic_cast<FloatSimpleValue*>(min)->GetValue() ||
            value>dynamic_cast<FloatSimpleValue*>(max)->GetValue()) {
          SetNull(true);
          return;
        }

        this->value=value;
        SetNull(false);
      }

      bool Add(long value, SimpleValue* min, SimpleValue* max)
      {
        // value is bigger than our [min,max] interval
        if (dynamic_cast<FloatSimpleValue*>(max)->GetValue()-
            dynamic_cast<FloatSimpleValue*>(min)->GetValue()<std::abs(value)) {
          SetNull(true);
          return false;
        }

        if (value>=0) {
          if (dynamic_cast<FloatSimpleValue*>(max)->GetValue()-value<this->value) {
            SetNull(true);
            return false;
          }

          this->value+=value;
          SetNull(false);
        }
        else {
          if (dynamic_cast<FloatSimpleValue*>(min)->GetValue()-value>=this->value) {
            SetNull(true);
            return false;
          }

          this->value+=value;
          SetNull(false);
        }

        return true;
      }

      bool Add(unsigned long value, SimpleValue* min, SimpleValue* max)
      {
        // value is bigger than our [min,max] interval
        if (dynamic_cast<FloatSimpleValue*>(max)->GetValue()-
            dynamic_cast<FloatSimpleValue*>(min)->GetValue()<value) {
          SetNull(true);
          return false;
        }

        if (dynamic_cast<FloatSimpleValue*>(max)->GetValue()-value<this->value) {
          SetNull(true);
          return false;
        }

        this->value+=value;
        SetNull(false);

        return true;
      }
      bool Add(double value, SimpleValue* min, SimpleValue* max)
      {
        // value is bigger than our [min,max] interval
        if (dynamic_cast<FloatSimpleValue*>(max)->GetValue()-
            dynamic_cast<FloatSimpleValue*>(min)->GetValue()<std::abs(value)) {
          SetNull(true);
          return false;
        }

        if (value>=0) {
          if (dynamic_cast<FloatSimpleValue*>(max)->GetValue()-value<this->value) {
            SetNull(true);
            return false;
          }

          this->value+=value;
          SetNull(false);
        }
        else {
          if (dynamic_cast<FloatSimpleValue*>(min)->GetValue()-value>this->value) {
            SetNull(true);
            return false;
          }

          this->value+=value;
          SetNull(false);
        }

        return true;
      }

      bool Sub(long value, SimpleValue* min, SimpleValue* max)
      {
        // value is bigger than our [min,max] interval
        if (dynamic_cast<FloatSimpleValue*>(max)->GetValue()-
            dynamic_cast<FloatSimpleValue*>(min)->GetValue()<std::abs(value)) {
          SetNull(true);
          return false;
        }

        if (value>=0) {
          if (dynamic_cast<FloatSimpleValue*>(min)->GetValue()+value>this->value) {
            SetNull(true);
            return false;
          }

          this->value-=value;
          SetNull(false);
        }
        else {
          if (dynamic_cast<FloatSimpleValue*>(max)->GetValue()+value<this->value) {
            SetNull(true);
            return false;
          }

          this->value-=value;
          SetNull(false);
        }

        return true;
      }

      bool Sub(unsigned long value, SimpleValue* min, SimpleValue* max)
      {
        // value is bigger than our [min,max] interval
        if (dynamic_cast<FloatSimpleValue*>(max)->GetValue()-
            dynamic_cast<FloatSimpleValue*>(min)->GetValue()<value) {
          SetNull(true);
          return false;
        }

        if (dynamic_cast<FloatSimpleValue*>(min)->GetValue()+value>this->value) {
          SetNull(true);
          return false;
        }

        this->value-=value;
        SetNull(false);

        return true;
      }

      bool Sub(double value, SimpleValue* min, SimpleValue* max)
      {
        // value is bigger than our [min,max] interval
        if (dynamic_cast<FloatSimpleValue*>(max)->GetValue()-
            dynamic_cast<FloatSimpleValue*>(min)->GetValue()<std::abs(value)) {
          SetNull(true);
          return false;
        }

        if (value>=0) {
          if (dynamic_cast<FloatSimpleValue*>(min)->GetValue()+value>this->value) {
            SetNull(true);
            return false;
          }

          this->value-=value;
          SetNull(false);
        }
        else {
          if (dynamic_cast<FloatSimpleValue*>(max)->GetValue()+value<this->value) {
            SetNull(true);
            return false;
          }

          this->value+=value;
          SetNull(false);
        }

        return true;
      }

      long GetLong() const
      {
        assert(!IsNull());

        return (long)value;
      }

      unsigned long GetUnsignedLong() const
      {
        assert(!IsNull());
        assert(value>=0);

        return (unsigned long)value;
      }

      double GetDouble() const
      {
        assert(!IsNull());

        return value;
      }

      bool Equals(Value* other) const
      {
        if (IsNull() && other->IsNull()) {
          return true;
        }
        else if (IsNull()!=other->IsNull()) {
          return false;
        }
        else {
          return value==other->GetLong();
        }
      }
    };

    Number::Value* Number::GetNewValueInstance() const
    {
      switch (valueType) {
      case valueTypeSigned:
        return new SignedValue();
      case valueTypeUnsigned:
        return new UnsignedValue();
      case valueTypeFloat:
        return new FloatValue();
      default:
        assert(false);
      }
    }

    Number::Value* Number::GetNewValueInstance(SimpleValue* value) const
    {
      switch (valueType) {
      case valueTypeSigned:
        return new SignedValue(value);
      case valueTypeUnsigned:
        return new UnsignedValue(value);
      case valueTypeFloat:
        return new FloatValue(value);
      default:
        assert(false);
      }
    }

    Number::Value* Number::GetNewValueInstance(Value* value) const
    {
      switch (valueType) {
      case valueTypeSigned:
        return new SignedValue(value);
      case valueTypeUnsigned:
        return new UnsignedValue(value);
      case valueTypeFloat:
        return new FloatValue(value);
      default:
        assert(false);
      }
    }

    Number::Value* Number::GetNewValueInstance(long value,
                                                     SimpleValue* min,
                                                     SimpleValue* max) const
    {
      switch (valueType) {
      case valueTypeSigned:
        return new SignedValue(value,min,max);
      case valueTypeUnsigned:
        return new UnsignedValue(value,min,max);
      case valueTypeFloat:
        return new FloatValue(value,min,max);
      default:
        assert(false);
      }
    }

    Number::Value* Number::GetNewValueInstance(unsigned long value,
                                                     SimpleValue* min,
                                                     SimpleValue* max) const
    {
      switch (valueType) {
      case valueTypeSigned:
        return new SignedValue(value,min,max);
      case valueTypeUnsigned:
        return new UnsignedValue(value,min,max);
      case valueTypeFloat:
        return new FloatValue(value,min,max);
      default:
        assert(false);
      }
    }

    Number::Value* Number::GetNewValueInstance(double value,
                                                     SimpleValue* min,
                                                     SimpleValue* max) const
    {
      switch (valueType) {
      case valueTypeSigned:
        return new SignedValue(value,min,max);
      case valueTypeUnsigned:
        return new UnsignedValue(value,min,max);
      case valueTypeFloat:
        return new FloatValue(value,min,max);
      default:
        assert(false);
      }
    }

    Number::Number(Type type)
    {
      switch (type) {
      case typeShort:
      case typeInt:
      case typeLong:
        valueType=valueTypeSigned;
        value=new SignedValue();
        break;
      case typeUnsignedShort:
      case typeUnsignedInt:
      case typeUnsignedLong:
        valueType=valueTypeUnsigned;
        value=new UnsignedValue();
        break;
      case typeFloat:
      case typeDouble:
        valueType=valueTypeFloat;
        value=new FloatValue();
        break;
      default:
        assert(false);
      }

      switch (type) {
      case typeShort:
        minimum=new SignedSimpleValue((long)std::numeric_limits<short>::min());
        maximum=new SignedSimpleValue((long)std::numeric_limits<short>::max());
        break;
      case typeUnsignedShort:
        minimum=new UnsignedSimpleValue((unsigned long)std::numeric_limits<unsigned short>::min());
        maximum=new UnsignedSimpleValue((unsigned long)std::numeric_limits<unsigned short>::max());
        break;
      case typeInt:
        minimum=new SignedSimpleValue((long)std::numeric_limits<int>::min());
        maximum=new SignedSimpleValue((long)std::numeric_limits<int>::max());
        break;
      case typeUnsignedInt:
        minimum=new UnsignedSimpleValue((unsigned long)std::numeric_limits<unsigned int>::min());
        maximum=new UnsignedSimpleValue((unsigned long)std::numeric_limits<unsigned int>::max());
        break;
      case typeLong:
        minimum=new SignedSimpleValue((long)std::numeric_limits<long>::min());
        maximum=new SignedSimpleValue((long)std::numeric_limits<long>::max());
        break;
      case typeUnsignedLong:
        minimum=new UnsignedSimpleValue((unsigned long)std::numeric_limits<unsigned long>::min());
        maximum=new UnsignedSimpleValue((unsigned long)std::numeric_limits<unsigned long>::max());
        break;
      case typeFloat:
        minimum=new FloatSimpleValue((double)std::numeric_limits<float>::min());
        maximum=new FloatSimpleValue((double)std::numeric_limits<float>::max());
        break;
      case typeDouble:
        minimum=new FloatSimpleValue((double)std::numeric_limits<double>::min());
        maximum=new FloatSimpleValue((double)std::numeric_limits<double>::max());
        break;
      default:
        assert(false);
      }
    }

    Number::Number(int value)
    {
      valueType=valueTypeSigned;

      minimum=new SignedSimpleValue((long)std::numeric_limits<int>::min());
      maximum=new SignedSimpleValue((long)std::numeric_limits<int>::max());

      this->value=new SignedValue((long)value,minimum,maximum);
    }

    Number::Number(unsigned int value)
    {
      valueType=valueTypeUnsigned;

      minimum=new UnsignedSimpleValue((unsigned long)std::numeric_limits<unsigned int>::min());
      maximum=new UnsignedSimpleValue((unsigned long)std::numeric_limits<unsigned int>::max());

      this->value=new UnsignedValue((unsigned long)value,minimum,maximum);
    }

    Number::Number(long value)
    {
      valueType=valueTypeSigned;

      minimum=new SignedSimpleValue(std::numeric_limits<long>::min());
      maximum=new SignedSimpleValue(std::numeric_limits<long>::max());

      this->value=new SignedValue(value,minimum,maximum);
    }

    Number::Number(unsigned long value)
    {
      valueType=valueTypeUnsigned;

      minimum=new UnsignedSimpleValue(std::numeric_limits<unsigned long>::min());
      maximum=new UnsignedSimpleValue(std::numeric_limits<unsigned long>::max());

      this->value=new UnsignedValue(value,minimum,maximum);
    }

    Number::Number(double value)
    {
      valueType=valueTypeFloat;


      minimum=new FloatSimpleValue(std::numeric_limits<double>::min());
      maximum=new FloatSimpleValue(std::numeric_limits<double>::max());

      this->value=new FloatValue(value,minimum,maximum);
    }

    Number::Number(int min, int max)
    {
      assert(max>=min);

      valueType=valueTypeSigned;

      value=new SignedValue();

      minimum=new SignedSimpleValue((long)min);
      maximum=new SignedSimpleValue((long)max);
    }

    Number::Number(unsigned int min, unsigned int max)
    {
      assert(max>=min);

      valueType=valueTypeSigned;

      value=new SignedValue();

      minimum=new SignedSimpleValue((unsigned long)min);
      maximum=new SignedSimpleValue((unsigned long)max);
    }

    Number::Number(long min, long max)
    {
      assert(max>=min);

      valueType=valueTypeSigned;

      value=new SignedValue();

      minimum=new SignedSimpleValue(min);
      maximum=new SignedSimpleValue(max);
    }

    Number::Number(unsigned long min, unsigned long max)
    {
      assert(max>=min);

      valueType=valueTypeUnsigned;

      value=new UnsignedValue();

      minimum=new UnsignedSimpleValue(min);
      maximum=new UnsignedSimpleValue(max);
    }

    Number::Number(double min, double max)
    {
      assert(max>=min);

      valueType=valueTypeFloat;

      value=new FloatValue();

      minimum=new FloatSimpleValue(min);
      maximum=new FloatSimpleValue(max);
    }

    Number::Number(int value, int min, int max)
    {
      assert(max>=min);

      valueType=valueTypeSigned;

      minimum=new SignedSimpleValue((long)min);
      maximum=new SignedSimpleValue((long)max);

      this->value=GetNewValueInstance((long)value,minimum,maximum);
    }

    Number::Number(unsigned int value, unsigned int min, unsigned int max)
    {
      assert(max>=min);

      valueType=valueTypeSigned;

      minimum=new SignedSimpleValue((unsigned long)min);
      maximum=new SignedSimpleValue((unsigned long)max);

      this->value=GetNewValueInstance((unsigned long)value,minimum,maximum);
    }

    Number::Number(long value, long min, long max)
    {
      assert(max>=min);

      valueType=valueTypeSigned;

      minimum=new SignedSimpleValue(min);
      maximum=new SignedSimpleValue(max);

      this->value=GetNewValueInstance(value,minimum,maximum);
    }

    Number::Number(unsigned long value, unsigned long min, unsigned long max)
    {
      assert(max>=min);

      valueType=valueTypeUnsigned;

      minimum=new UnsignedSimpleValue(min);
      maximum=new UnsignedSimpleValue(max);

      this->value=GetNewValueInstance(value,minimum,maximum);
    }

    Number::Number(double value, double min, double max)
    {
      assert(max>=min);

      valueType=valueTypeFloat;

      minimum=new FloatSimpleValue(min);
      maximum=new FloatSimpleValue(max);

      this->value=GetNewValueInstance(value,minimum,maximum);
    }

    Number::~Number()
    {
      while (!stack.empty()) {
        Value* value=stack.top();

        stack.pop();
        delete value;
      }

      delete value;
      delete minimum;
      delete maximum;
    }

    bool Number::Changed() const
    {
      if (stack.empty()) {
        return false;
      }

      if (stack.top()->IsNull()!=value->IsNull()) {
        return true;
      }
      else if (stack.top()->IsNull() && value->IsNull()) {
        return false;
      }
      else {
        return !value->Equals(stack.top());
      }
    }

    void Number::Push()
    {
      Value *newValue=GetNewValueInstance(value);

      stack.push(value);
      value=newValue;

      Base::Model::Push();
    }

    void Number::Pop()
    {
      assert(stack.size()>0);

      bool changed=Changed();

      delete value;
      value=stack.top();

      stack.pop();

      Base::Model::Pop();

      if (changed) {
        Notify();
      }
    }

    void Number::Undo()
    {
      assert(stack.size()>0);

      bool changed=Changed();

      delete value;
      value=GetNewValueInstance(stack.top());

      if (changed) {
        Notify();
      }
    }

    void Number::Save()
    {
      assert(stack.size()>0);

      bool  changed=Changed();
      Value *oldValue=stack.top();

      Base::Model::Pop();

      stack.pop();
      delete oldValue;
      stack.push(GetNewValueInstance(value));

      if (changed) {
        Notify();
      }

      Base::Model::Push();
    }

    void Number::SetNull()
    {
      Value *oldValue=value;

      value=GetNewValueInstance();

      if (!oldValue->Equals(value)) {
        Notify();
      }

      delete oldValue;
    }

    bool Number::IsNull() const
    {
      return value->IsNull();
    }

    void Number::SetRange(long min, long max)
    {
      delete minimum;
      delete maximum;

      minimum=new SignedSimpleValue(min);
      maximum=new SignedSimpleValue(max);

      //TODO: value->RoundToRange(minimum,maximum);

      Notify();
    }

    void Number::SetRange(unsigned long min, unsigned long max)
    {
      delete minimum;
      delete maximum;

      minimum=new UnsignedSimpleValue(min);
      maximum=new UnsignedSimpleValue(max);

      //TODO: value->RoundToRange(minimum,maximum);

      Notify();
    }

    void Number::SetRange(double min, double max)
    {
      delete minimum;
      delete maximum;

      minimum=new FloatSimpleValue(min);
      maximum=new FloatSimpleValue(max);

      //TODO: value->RoundToRange(minimum,maximum);

      Notify();
    }

    bool Number::Set(int value)
    {
      return Set((long)value);
    }

    bool Number::Set(unsigned int value)
    {
      return Set((unsigned long)value);
    }

    bool Number::Set(long value)
    {
      Value *oldValue=this->value;

      this->value=GetNewValueInstance(value,minimum,maximum);

      if (!oldValue->Equals(this->value)) {
        Notify();
      }

      delete oldValue;

      return !this->value->IsNull();
    }

    bool Number::Set(unsigned long value)
    {
      Value *oldValue=this->value;

      this->value=GetNewValueInstance(value,minimum,maximum);

      if (!oldValue->Equals(this->value)) {
        Notify();
      }

      delete oldValue;

      return !this->value->IsNull();
    }

    bool Number::Set(double value)
    {
      Value *oldValue=this->value;

      this->value=GetNewValueInstance(value,minimum,maximum);

      if (!oldValue->Equals(this->value)) {
        Notify();
      }

      delete oldValue;

      return !this->value->IsNull();
    }

    void Number::SetToMin()
    {
      Value *oldValue=value;

      value=GetNewValueInstance(minimum);

      if (!oldValue->Equals(value)) {
        Notify();
      }

      delete oldValue;
    }

    void Number::SetToMax()
    {
      Value *oldValue=value;

      value=GetNewValueInstance(maximum);

      if (!oldValue->Equals(value)) {
        Notify();
      }

      delete oldValue;
    }

    bool Number::Inc()
    {
      return Add(1ul);
    }

    bool Number::Dec()
    {
      return Sub(1ul);
    }

    bool Number::Add(long increment)
    {
      bool result;

      result=value->Add(increment,minimum,maximum);

      // TODO: Check for change
      Notify();

      return result;
    }

    bool Number::Add(unsigned long increment)
    {
      bool result;

      result=value->Add(increment,minimum,maximum);

      // TODO: Check for change
      Notify();

      return result;
    }

    bool Number::Add(double increment)
    {
      bool result;

      result=value->Add(increment,minimum,maximum);

      // TODO: Check for change
      Notify();

      return result;
    }

    bool Number::Sub(long decrement)
    {
      bool result;

      result=value->Sub(decrement,minimum,maximum);

      // TODO: Check for change
      Notify();

      return result;
    }

    bool Number::Sub(unsigned long decrement)
    {
      bool result;

      result=value->Sub(decrement,minimum,maximum);

      // TODO: Check for change
      Notify();

      return result;
    }

    bool Number::Sub(double decrement)
    {
      bool result;

      result=value->Sub(decrement,minimum,maximum);

      // TODO: Check for change
      Notify();

      return result;
    }

    Number::ValueType Number::GetValueType() const
    {
      return valueType;
    }

    long Number::GetMinAsLong() const
    {
      switch (valueType) {
      case valueTypeSigned:
        return dynamic_cast<SignedSimpleValue*>(minimum)->GetValue();
        break;
      case valueTypeUnsigned:
        assert(dynamic_cast<UnsignedSimpleValue*>(minimum)->GetValue()<=(unsigned long)std::numeric_limits<long>::max());
        return (long)dynamic_cast<UnsignedSimpleValue*>(minimum)->GetValue();
        break;
      case valueTypeFloat:
        assert(dynamic_cast<FloatSimpleValue*>(minimum)->GetValue()>=std::numeric_limits<long>::min());
        assert(dynamic_cast<FloatSimpleValue*>(minimum)->GetValue()<=std::numeric_limits<long>::max());
        return dynamic_cast<FloatSimpleValue*>(minimum)->GetValue();
        break;
      default:
        assert(false);
      }
    }

    unsigned long Number::GetMinAsUnsignedLong() const
    {
      switch (valueType) {
      case valueTypeSigned:
        assert(dynamic_cast<SignedSimpleValue*>(minimum)->GetValue()>=0);
        return dynamic_cast<SignedSimpleValue*>(minimum)->GetValue();
        break;
      case valueTypeUnsigned:
        return dynamic_cast<UnsignedSimpleValue*>(minimum)->GetValue();
        break;
      case valueTypeFloat:
        assert(dynamic_cast<FloatSimpleValue*>(minimum)->GetValue()>=std::numeric_limits<unsigned long>::min());
        assert(dynamic_cast<FloatSimpleValue*>(minimum)->GetValue()<=std::numeric_limits<unsigned long>::max());
        return dynamic_cast<FloatSimpleValue*>(minimum)->GetValue();
        break;
      default:
        assert(false);
      }
    }

    double Number::GetMinAsDouble() const
    {
      switch (valueType) {
      case valueTypeSigned:
        return dynamic_cast<SignedSimpleValue*>(minimum)->GetValue();
        break;
      case valueTypeUnsigned:
        return dynamic_cast<UnsignedSimpleValue*>(minimum)->GetValue();
        break;
      case valueTypeFloat:
        return dynamic_cast<FloatSimpleValue*>(minimum)->GetValue();
        break;
      default:
        assert(false);
      }
    }

    long Number::GetMaxAsLong() const
    {
      switch (valueType) {
      case valueTypeSigned:
        return dynamic_cast<SignedSimpleValue*>(maximum)->GetValue();
        break;
      case valueTypeUnsigned:
        assert(dynamic_cast<UnsignedSimpleValue*>(maximum)->GetValue()<=(unsigned long)std::numeric_limits<long>::max());
        return (long)dynamic_cast<UnsignedSimpleValue*>(maximum)->GetValue();
        break;
      case valueTypeFloat:
        assert(dynamic_cast<FloatSimpleValue*>(maximum)->GetValue()>=std::numeric_limits<long>::min());
        assert(dynamic_cast<FloatSimpleValue*>(maximum)->GetValue()<=std::numeric_limits<long>::max());
        return dynamic_cast<FloatSimpleValue*>(maximum)->GetValue();
        break;
      default:
        assert(false);
      }
    }

    unsigned long Number::GetMaxAsUnsignedLong() const
    {
      switch (valueType) {
      case valueTypeSigned:
        assert(dynamic_cast<SignedSimpleValue*>(maximum)->GetValue()>=0);
        return dynamic_cast<SignedSimpleValue*>(maximum)->GetValue();
        break;
      case valueTypeUnsigned:
        return dynamic_cast<UnsignedSimpleValue*>(maximum)->GetValue();
        break;
      case valueTypeFloat:
        assert(dynamic_cast<FloatSimpleValue*>(maximum)->GetValue()>=std::numeric_limits<unsigned long>::min());
        assert(dynamic_cast<FloatSimpleValue*>(maximum)->GetValue()<=std::numeric_limits<unsigned long>::max());
        return dynamic_cast<FloatSimpleValue*>(maximum)->GetValue();
        break;
      default:
        assert(false);
      }
    }

    double Number::GetMaxAsDouble() const
    {
      switch (valueType) {
      case valueTypeSigned:
        return dynamic_cast<SignedSimpleValue*>(maximum)->GetValue();
        break;
      case valueTypeUnsigned:
        return dynamic_cast<UnsignedSimpleValue*>(maximum)->GetValue();
        break;
      case valueTypeFloat:
        return dynamic_cast<FloatSimpleValue*>(maximum)->GetValue();
        break;
      default:
        assert(false);
      }
    }

    long Number::GetLong() const
    {
      return value->GetLong();
    }

    unsigned long Number::GetUnsignedLong() const
    {
      return value->GetUnsignedLong();
    }

    double Number::GetDouble() const
    {
      return value->GetDouble();
    }

    Long::Long()
    : Number(typeLong)
    {
      // no code
    }

    Long::Long(long value)
    : Number(value)
    {
      // no code
    }

    Long::Long(long min, long max)
    : Number(min,max)
    {
      // no code
    }

    Long::Long(long value, long min, long max)
    : Number(value,min,max)
    {
      // no code
    }

    long Long::Get() const
    {
      return GetLong();
    }

    ULong::ULong()
    : Number(typeUnsignedLong)
    {
      // no code
    }

    ULong::ULong(unsigned long value)
    : Number(value)
    {
      // no code
    }

    ULong::ULong(unsigned long min, unsigned long max)
    : Number(min,max)
    {
      // no code
    }

    ULong::ULong(unsigned long value, unsigned long min, unsigned long max)
    : Number(value,min,max)
    {
      // no code
    }

    unsigned long ULong::Get() const
    {
      return GetUnsignedLong();
    }

    Double::Double()
    : Number(typeFloat)
    {
      // no code
    }
    Double::Double(double value)
    : Number(value)
    {
      // no code
    }

    Double::Double(double min, double max)
    : Number(min,max)
    {
      // no code
    }

    Double::Double(double value, double min, double max)
    : Number(value,min,max)
    {
      // no code
    }

    double Double::Get() const
    {
      return GetDouble();
    }
  }
}

