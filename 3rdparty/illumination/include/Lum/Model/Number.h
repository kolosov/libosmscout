#ifndef LUM_MODEL_NUMBER_H
#define LUM_MODEL_NUMBER_H

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

#include <assert.h>

#include <limits>

#include <Lum/Base/String.h>

#include <Lum/Model/Simple.h>

namespace Lum {
  namespace Model {

    class LUMAPI Number : public Base::Model
    {
    public:
      enum Type {
        typeShort,
        typeUnsignedShort,
        typeInt,
        typeUnsignedInt,
        typeLong,
        typeUnsignedLong,
        typeFloat,
        typeDouble
      };

    public:
      enum ValueType {
        valueTypeSigned,
        valueTypeUnsigned,
        valueTypeFloat
      };

    private:
      class SimpleValue;
      class SignedSimpleValue;
      class UnsignedSimpleValue;
      class FloatSimpleValue;

      class Value;
      class SignedValue;
      class UnsignedValue;
      class FloatValue;

    private:
      ValueType          valueType;
      SimpleValue        *minimum;
      SimpleValue        *maximum;
      Value              *value;
      std::stack<Value*> stack;

    private:
      Value* GetNewValueInstance() const;
      Value* GetNewValueInstance(SimpleValue* value) const;
      Value* GetNewValueInstance(Value* value) const;
      Value* GetNewValueInstance(long value, SimpleValue* min, SimpleValue* max) const;
      Value* GetNewValueInstance(unsigned long value, SimpleValue* min, SimpleValue* max) const;
      Value* GetNewValueInstance(double value, SimpleValue* min, SimpleValue* max) const;

    public:
      Number(Type type);

      Number(int value);
      Number(unsigned int value);
      Number(long value);
      Number(unsigned long value);
      Number(double value);

      Number(int min, int max);
      Number(unsigned int min, unsigned int max);
      Number(long min, long max);
      Number(unsigned long min, unsigned long max);
      Number(double min, double max);

      Number(int value, int min, int max);
      Number(unsigned int value, unsigned int min, unsigned int max);
      Number(long value, long min, long max);
      Number(unsigned long value, unsigned long min, unsigned long max);
      Number(double value, double min, double max);

      ~Number();

      bool Changed() const;

      void Push();
      void Pop();
      void Undo();
      void Save();

      void SetNull();
      bool IsNull() const;

      void SetRange(long min, long max);
      void SetRange(unsigned long min, unsigned long max);
      void SetRange(double min, double max);

      bool Set(int value);
      bool Set(unsigned int value);
      bool Set(long value);
      bool Set(unsigned long value);
      bool Set(double value);

      void SetToMin();
      void SetToMax();

      bool Inc();
      bool Dec();

      bool Add(long increment);
      bool Add(unsigned long increment);
      bool Add(double increment);

      bool Sub(long decrement);
      bool Sub(unsigned long decrement);
      bool Sub(double decrement);

      ValueType GetValueType() const;

      long GetMinAsLong() const;
      unsigned long GetMinAsUnsignedLong() const;
      double GetMinAsDouble() const;

      long GetMaxAsLong() const;
      unsigned long GetMaxAsUnsignedLong() const;
      double GetMaxAsDouble() const;

      long GetLong() const;
      unsigned long GetUnsignedLong() const;
      double GetDouble() const;
    };

    typedef Base::Reference<Number> NumberRef;

    class LUMAPI Long : public Number
    {
    public:
      Long();
      Long(long value);
      Long(long min, long max);
      Long(long value, long min, long max);

      long Get() const;
    };

    typedef Base::Reference<Long> LongRef;

    class LUMAPI ULong : public Number
    {
    public:
      ULong();
      ULong(unsigned long value);
      ULong(unsigned long min, unsigned long max);
      ULong(unsigned long value, unsigned long min, unsigned long max);

      unsigned long Get() const;
    };

    typedef Base::Reference<ULong> ULongRef;

    class LUMAPI Double : public Number
    {
    public:
      Double();
      Double(double value);
      Double(double min, double max);
      Double(double value, double min, double max);

      double Get() const;
    };

    typedef Base::Reference<Double> DoubleRef;
  }
}

#endif
