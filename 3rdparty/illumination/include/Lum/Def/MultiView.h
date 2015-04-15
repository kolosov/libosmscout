#ifndef LUM_DEF_VIEW_H
#define LUM_DEF_VIEW_H

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

#include <string>

#include <Lum/Private/ImportExport.h>

#include <Lum/Def/Desc.h>
#include <Lum/Def/Menu.h>

#include <Lum/Model/Number.h>

#include <Lum/Object.h>

namespace Lum {
  namespace Def {
    /**
      Definition of an abstract multi view. Multi view means that depending on
      the value of the given model one of n views (GUI-object) will be shown.

      MultiViews can be either used as top level element in a dialog.

      MultiView should be used when it is obvious that visualisation is somehow grouped,
      possibly or even likely not everything visible at the same time or object likely
      want as much screen space as possible - and no better representation exists.

      MultiView is static. The number of views must be fixed at the time of creation of
      the control. No views can be added, removed or changed after creation.

      MultiView assures that the user can freely choose from one fo the views
      and can freely switch between the views.
     */
    class LUMAPI MultiView
    {
    public:
      class LUMAPI View {
      private:
        size_t      value;
        Desc        description;
        Menu        *menu;
        Lum::Object *object;

      public:
        View(size_t value,
             const Desc& description,
             Lum::Object* object);
        View(const View& other);
        ~View();

        View& SetMenu(Menu* menu);

        const Desc& GetDesc() const;
        size_t GetValue() const;
        Menu* GetMenu() const;
        Lum::Object* GetObject() const;
      };

    private:
      Desc             description;
      Menu*            preMenu;
      Menu*            postMenu;
      Model::NumberRef value;
      std::list<View>  views;

    public:
      MultiView(const Desc& description);
      MultiView(const MultiView& other);
      virtual ~MultiView();

      MultiView& SetMenu(Menu* pre, Menu* post);
      MultiView& SetValue(Model::Number* value);
      MultiView& AddView(const View& view);

      Model::Number* GetValue() const;

      const Desc& GetDesc() const;
      Menu* GetPreMenu() const;
      Menu* GetPostMenu() const;
      const std::list<View>& GetViews() const;
    };
  }
}

#endif

