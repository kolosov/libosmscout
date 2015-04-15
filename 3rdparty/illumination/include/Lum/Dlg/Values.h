#ifndef LUM_DLG_VALUES_H
#define LUM_DLG_VALUES_H

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

#include <list>
#include <string>

#include <Lum/Dlg/ActionDialog.h>
#include <Lum/Dlg/Validator.h>

#include <Lum/Model/Action.h>

namespace Lum {
  namespace Dlg {
    class LUMAPI ValuesInput : public ActionDialog
    {
    protected:
      class LUMAPI Value
      {
      public:
        std::wstring        label;
        Control             *control;
        Lum::Base::ModelRef model;
        InputValidator      *validator;

      public:
        Value();
      };

    private:
      bool             result;
      std::wstring     caption;
      std::wstring     text;
      std::wstring     note;
      Model::ActionRef okAction;
      std::list<Value> values;

    protected:
      ValuesInput();
      void AddValue(const Value& value);

      void Validate();

    public:
      ~ValuesInput();

      void SetCaption(const std::wstring& caption);
      void SetText(const std::wstring& text);
      void SetNote(const std::wstring& note);

      Lum::Object* GetContent();
      void GetActions(std::vector<ActionInfo>& actions);

      void Resync(Lum::Base::Model* model, const Lum::Base::ResyncMsg& msg);

      bool HasResult() const;
    };
  }
}

#endif
