#ifndef LUM_MANAGER_BEHAVIOUR_H
#define LUM_MANAGER_BEHAVIOUR_H

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

#include <set>

#include <Lum/Private/ImportExport.h>

#include <Lum/Base/Model.h>

#include <Lum/Def/Boolean.h>
#include <Lum/Def/Menu.h>
#include <Lum/Def/MultiView.h>
#include <Lum/Def/OneOfMany.h>

#include <Lum/OS/Window.h>

namespace Lum {

  // Forward declaration of dialog because of recursive import of CustomMenuHandler
  class Dialog;

  namespace Manager {

    /**
      A abstract base class for holding all information for influencing the behaviour
      of an user interface.

      Behaviour should try to operate on objects in the namespace Lum::Def and return
      approviate controls for implementing the required behaviour hinted by the
      abstract definitions.

      Clearly visual aspects should be implemented in Lum::OS::Theme.
    */
    class LUMAPI Behaviour : public Lum::Base::MsgObject
    {
    public:
      enum TableActionMode {
        TableActionOnSelection,
        TableActionOnDoubleclick
      };

    public:
      class CustomMenuHandler
      {
      protected:
        OS::Window *window;

      public:
        CustomMenuHandler(OS::Window* window);
        virtual ~CustomMenuHandler();

        virtual bool SetMenu(Def::Menu* def,
                             const std::set<Lum::Base::Model*>& excludes) = 0;
        virtual bool Open() = 0;
      };

    protected:
      Behaviour();

    public:
      virtual ~Behaviour();

      /**
        Return the global BehaviourManager instance.

        An instance is available as soon as the display is initialized.
        */
      static Behaviour* Instance();

      /**
        If true, the first focusable object in a window will be automatically
        focused.
        */
      virtual bool FocusObjectOnWindowFocusIn() const = 0;

      /**
        If true, scroll knob can be grabed to change the position in a scrolled
        view. If false the scroll knob does not be draggable and thus can have a minimized
        read-only visualisation.
        */
      virtual bool ScrollKnobActive() const = 0;

      /**
        A table can have a special action to trigger a non-destructive default action
        on a table. Depending on the environment this action can be triggered by
        just selection or by double click. This flags sepcifies which of them should
        be used.
        */
      virtual TableActionMode GetTableDefaultActionMode() const = 0;

      /**
        @name Menu
        Using system specific menu implementation
      */
      //@{
      virtual CustomMenuHandler* GetCustomMenuHandler(OS::Window* window) const = 0;
      //@}

      /**
        @name Dlg
        Choosing between different dialog representations
      */
      //@{
      virtual void ApplyMultiViewDlg(Dialog* dialog,
                                     const Def::MultiView& multiView) const = 0;
      //@}

      /**
        @name Controls
        Choosing between different control implementation
      */
      //@{
      virtual Lum::Object* GetActionControl(const Def::Action& def) const = 0;
      virtual Lum::Object* GetActionControl(const Def::Action& def,
                                            bool horizFlex,
                                            bool vertFlex) const = 0;

      virtual Lum::Object* GetOneOfNControl(const Def::OneOfN& def) const = 0;
      virtual Lum::Object* GetOneOfNControl(const Def::OneOfN& def,
                                            bool horizFlex,
                                            bool vertFlex) const = 0;


      virtual Lum::Object* GetOneOfManyControl(const Def::OneOfMany& def) const = 0;
      virtual Lum::Object* GetOneOfManyControl(const Def::OneOfMany& def,
                                               bool horizFlex,
                                               bool vertFlex) const = 0;

      virtual Lum::Object* GetBooleanControl(const Def::Boolean& def) const = 0;
      virtual Lum::Object* GetBooleanControl(const Def::Boolean& def,
                                             bool horizFlex,
                                             bool vertFlex) const = 0;
      //@}
    };
  }
}

#endif
