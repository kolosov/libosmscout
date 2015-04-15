#ifndef LUM_DLG_PASSWORD_H
#define LUM_DLG_PASSWORD_H

/*
  This source is part of the Illumination library
  Copyright (C) 2007  Tim Teulings

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

#include <Lum/Dlg/ActionDialog.h>

#include <Lum/Model/Action.h>
#include <Lum/Model/Boolean.h>
#include <Lum/Model/String.h>


namespace Lum {
  namespace Dlg {
    class LUMAPI Password : public ActionDialog
    {
    private:
      bool              result;

      std::wstring      caption;
      std::wstring      text;
      std::wstring      comment;

      Model::StringRef  username;
      Model::StringRef  password;
      Model::BooleanRef savePassword;

      Model::ActionRef  okAction;

    public:
      Password(Model::String* username, Model::String* password);
      ~Password();

      void SetCaption(const std::wstring& caption);
      void SetText(const std::wstring& text);
      void SetComment(const std::wstring& comment);
      void SetSavePasswordModel(Model::Boolean* savePassword);

      Lum::Object* GetContent();
      void GetActions(std::vector<ActionInfo>& actions);

      void Resync(Lum::Base::Model* model, const Lum::Base::ResyncMsg& msg);

      bool Success() const;

      static bool GetPassword(OS::Window* parent,
                              const std::wstring& caption,
                              const std::wstring& text,
                              const std::wstring& comment,
                              Model::String* username,
                              Model::String* password,
                              Model::Boolean* savePassword);

      static bool GetPassword(OS::Window* parent,
                              const std::wstring& caption,
                              const std::wstring& text,
                              std::wstring& username,
                              std::wstring& password);

      static bool GetPassword(OS::Window* parent,
                              const std::wstring& caption,
                              const std::wstring& text,
                              const std::wstring& comment,
                              std::wstring& username,
                              std::wstring& password,
                              bool& savePassword);
    };
  }
}

#endif
