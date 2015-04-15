#ifndef LUM_MODEL_SIMPLE_H
#define LUM_MODEL_SIMPLE_H

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

#include <stack>

#include <Lum/Base/Model.h>

namespace Lum {
  namespace Model {

    template <class V>
    class LUMAPI Simple : public Base::Model
    {
    private:
      class Value
      {
      public:
        V    value;
        bool isNull;
      };

    private:
      std::stack<Value> stack;

    protected:
      Value             value;

    public:
      Simple();
      Simple(const V& value);

      bool Changed() const;

      void Push();
      void Pop();
      void Undo();
      void Save();

      void SetNull();
      bool IsNull() const;

      void Set(const V& value);
      const V& Get() const;

      const V& operator=(const V& value);
      operator V() const;
    };

    template <class V>
    Simple<V>::Simple()
    {
      value.isNull=true;
    }

    template <class V>
    Simple<V>::Simple(const V& value)
    {
      this->value.value=value;
      this->value.isNull=false;
    }

    template <class V>
    bool Simple<V>::Changed() const
    {
      if (stack.empty()) {
        return false;
      }

      if (stack.top().isNull!=value.isNull) {
        return true;
      }
      else if (!value.isNull && !stack.top().isNull && value.value!=stack.top().value) {
        return true;
      }

      return false;
    }

    template <class V>
    void Simple<V>::Push()
    {
      stack.push(value);

      Base::Model::Push();
    }

    template <class V>
    void Simple<V>::Pop()
    {
      bool changed;

      assert(stack.size()>0);

      changed=Changed();

      value=stack.top();

      stack.pop();
      Base::Model::Pop();

      if (changed) {
        Notify();
      }
    }

    template <class V>
    void Simple<V>::Undo()
    {
      assert(stack.size()>0);

      bool changed;

      changed=Changed();

      value=stack.top();

      if (changed) {
        Notify();
      }
    }

    template <class V>
    void Simple<V>::Save()
    {
      assert(stack.size()>0);

      bool changed;

      changed=Changed();

      stack.top()=value;

      Base::Model::Pop();

      if (changed) {
        Notify();
      }

      Base::Model::Push();
    }

    template <class V>
    void Simple<V>::SetNull()
    {
      if (!value.isNull) {
        value.isNull=true;
        Notify();
      }
    }

    template <class V>
    bool Simple<V>::IsNull() const
    {
      return value.isNull;
    }

    template <class V>
    void Simple<V>::Set(const V& value)
    {
      if (this->value.isNull || (!this->value.isNull && this->value.value!=value)) {
        this->value.value=value;
        this->value.isNull=false;
        Notify();
      }
    }

    template <class V>
    const V& Simple<V>::Get() const
    {
      assert(!value.isNull);

      return value.value;
    }

    template <class V>
    const V& Simple<V>::operator=(const V& value)
    {
      Set(value);

      return value;
    }

    template <class V>
    Simple<V>::operator V() const
    {
      assert(!value.isNull);

      return value.value;
    }
  }
}

#endif
