#ifndef LUM_OS_BASE_BEHAVIOUR_H
#define LUM_OS_BASE_BEHAVIOUR_H

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

#include <Lum/Manager/Behaviour.h>

namespace Lum {
  namespace OS {
    namespace Base {

      class DefaultBehaviour : public Lum::Manager::Behaviour
      {
      public:
        DefaultBehaviour();
        ~DefaultBehaviour();

        bool FocusObjectOnWindowFocusIn() const;
        bool ScrollKnobActive() const;
        TableActionMode GetTableDefaultActionMode() const;

        CustomMenuHandler* GetCustomMenuHandler(OS::Window* window) const;
        void ApplyMultiViewDlg(Dialog* dialog,
                               const Def::MultiView& multiView) const;

        Lum::Object* GetActionControl(const Def::Action& def) const;
        Lum::Object* GetActionControl(const Def::Action& def,
                                      bool horizFlex,
                                      bool vertFlex) const;

        Lum::Object* GetOneOfNControl(const Def::OneOfN& def) const;
        Lum::Object* GetOneOfNControl(const Def::OneOfN& def,
                                      bool horizFlex,
                                      bool vertFlex) const;

        Lum::Object* GetOneOfManyControl(const Def::OneOfMany& def) const;
        Lum::Object* GetOneOfManyControl(const Def::OneOfMany& def,
                                         bool horizFlex,
                                         bool vertFlex) const;

        Lum::Object* GetBooleanControl(const Def::Boolean& def) const;
        Lum::Object* GetBooleanControl(const Def::Boolean& def,
                                       bool horizFlex,
                                       bool vertFlex) const;
      };
    }
  }
}

#endif
