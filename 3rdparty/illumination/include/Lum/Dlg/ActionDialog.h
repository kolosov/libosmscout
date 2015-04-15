#ifndef LUM_DLG_ACTIONDIALOG_H
#define LUM_DLG_ACTIONDIALOG_H

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

#include <string>
#include <vector>

#include <Lum/Model/Action.h>

#include <Lum/Dialog.h>

namespace Lum {
  namespace Dlg {
      class LUMAPI ActionInfo
      {
      public:
        enum ActionType {
          typeCommit,
          typeCancel,
          typeDefault,
          typeAdditional,
          typeOptional
        };

      private:
        ActionType       type;
        std::wstring     label;
        OS::ImageRef     image;
        Model::ActionRef action;
        bool             isDefault;

      public:
        ActionInfo(ActionType type,
                   const std::wstring& label,
                   Model::Action *action,
                   bool isDefault=false);
        ActionInfo(ActionType type,
                   const std::wstring& label,
                   OS::Image* image,
                   Model::Action *action,
                   bool isDefault=false);

        ActionType GetType() const;
        std::wstring GetLabel() const;
        OS::Image* GetImage() const;
        Model::Action* GetAction() const;
        bool IsDefault() const;
      };

    class LUMAPI ActionDialog : public Dialog
    {
    private:
      std::vector<ActionInfo> list;
      Lum::Object             *content;

    public:
      ActionDialog();
      ~ActionDialog();

      void PreInit();

      virtual Lum::Object* GetContent() = 0;
      virtual void GetActions(std::vector<ActionInfo>& actions) = 0;

      static void CreateActionInfosOkCancel(std::vector<ActionInfo>& actions,
                                            Model::Action* okAction,
                                            Model::Action* cancelAction);
      static void CreateActionInfosOk(std::vector<ActionInfo>& actions,
                                      Model::Action* okAction);
      static void CreateActionInfosCancel(std::vector<ActionInfo>& actions,
                                          Model::Action* cancelAction);
      static void CreateActionInfosClose(std::vector<ActionInfo>& actions,
                                          Model::Action* closeAction);
      static void CreateActionInfosQuit(std::vector<ActionInfo>& actions,
                                        Model::Action* quitAction);
    };
  }
}

#endif
