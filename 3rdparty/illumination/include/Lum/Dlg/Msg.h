#ifndef LUM_DLG_MSG_H
#define LUM_DLG_MSG_H

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

#include <Lum/Dlg/ActionDialog.h>

#include <Lum/Model/Action.h>

#include <Lum/Button.h>

namespace Lum {
  namespace Dlg {
    class LUMAPI Msg : public ActionDialog
    {
    private:
      class ButtonInfo
      {
      public:
        std::wstring     label;
        Button::Type     type;
        Model::ActionRef action;
      };

    private:
      std::vector<ButtonInfo> list;
      std::wstring            caption;
      std::wstring            text;
      Model::ActionRef        resultAction;

    public:
      Msg();
      ~Msg();

      void SetCaption(const std::wstring& caption);
      void SetText(const std::wstring& text);
      void AddButton(const std::wstring& label,
                     Button::Type type,
                     Model::Action* action);
      void AddButtons(const std::wstring& buttons);
      size_t GetResultIndex() const;

      Lum::Object* GetContent();
      void GetActions(std::vector<ActionInfo>& actions);

      void Resync(Base::Model* model, const Base::ResyncMsg& msg);

      static void ShowOk(Dialog* parent,
                         const std::wstring& caption,
                         const std::wstring& text);
      static void ShowOk(OS::Window* parent,
                         const std::wstring& caption,
                         const std::wstring& text);
      static size_t Ask(Dialog* parent,
                        const std::wstring& caption,
                        const std::wstring& text,
                        const std::wstring& buttons);
      static size_t Ask(OS::Window* parent,
                        const std::wstring& caption,
                        const std::wstring& text,
                        const std::wstring& buttons);
      static size_t Ask(Dialog* parent,
                        const std::wstring& caption,
                        const std::wstring& text,
                        const std::wstring& buttons,
                        int x, int y);
      static size_t Ask(OS::Window* parent,
                        const std::wstring& caption,
                        const std::wstring& text,
                        const std::wstring& buttons,
                        int x, int y);
    };
  }
}

#endif
