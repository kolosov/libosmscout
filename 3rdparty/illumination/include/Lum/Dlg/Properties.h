#ifndef LUM_DLG_PROPERTIES_H
#define LUM_DLG_PROPERTIES_H

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

#include <Lum/Private/ImportExport.h>

#include <Lum/Def/Props.h>

#include <Lum/Dlg/ActionDialog.h>

#include <Lum/Model/Action.h>
#include <Lum/Model/String.h>

#include <Lum/Label.h>

namespace Lum {
  namespace Dlg {
    class LUMAPI Properties : public ActionDialog
    {
    private:
      struct Visitor
      {
        virtual ~Visitor();
        virtual void Visit(Lum::Base::Model* model) = 0;
      };

    private:
      Def::PropGroup   *props;
      Model::ActionRef okAction;

    private:
      void VisitModels(Def::PropGroup *props, Visitor& visitor);
      void VisitModels(Visitor& visitor);

      void ObserveModels();
      void PushModels();
      void ValidateModels();
      void SaveModels();
      void PopModels();

    protected:
      void AddControlForProperty(Label* label,
                                 Def::Property* prop);

    public:
      Properties(Def::PropGroup *props);
      ~Properties();

      Lum::Object* GetContent();
      void GetActions(std::vector<Lum::Dlg::ActionInfo>& actions);

      void Resync(Lum::Base::Model* model, const Lum::Base::ResyncMsg& msg);

      static void Show(Dialog* parent, Def::PropGroup* props);
      static void Show(OS::Window* parent, Def::PropGroup* props);
    };
  }
}

#endif
