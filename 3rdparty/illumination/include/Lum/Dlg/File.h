#ifndef LUM_DLG_FILE_H
#define LUM_DLG_FILE_H

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

#include <Lum/Dlg/ActionDialog.h>

#include <Lum/Model/Action.h>
#include <Lum/Model/Boolean.h>
#include <Lum/Model/Dir.h>
#include <Lum/Model/Number.h>
#include <Lum/Model/Path.h>
#include <Lum/Model/String.h>
#include <Lum/Model/Table.h>

#include <Lum/String.h>
#include <Lum/Table.h>

namespace Lum {
  namespace Dlg {

    class LUMAPI File : public ActionDialog
    {
    public:
      class LUMAPI Options : public Base::Referencable
      {
      public:
        enum Mode
        {
          modeFile,
          modeDirectory
        };

      public:
        bool                             existingOnly;
        Mode                             mode;
        std::vector<Model::Dir::Filter*> filters;
        size_t                           defaultFilter;

      public:
        Options();
        virtual ~Options();

        void SetMode(Mode mode);
        void SetExistingOnly(bool exists);
        void AddFilter(Model::Dir::Filter* filter);
        void SetDefaultFilter(size_t pos);
        void SetDefaultFilter(const std::wstring& name);
      };

      typedef Base::Reference<Options> OptionsRef;

    private:
      Model::DirRef                 dirList;
      Model::PathRef                path;
      Model::StringRef              full;
      Model::ULongRef               filter;
      Model::BooleanRef             showHidden;
      Model::ActionRef              okAction;
      Model::ActionRef              reloadAction;
      Model::ActionRef              parentAction;
      Model::ActionRef              fullAction;
      Model::ActionRef              doubleClickAction;
      Model::ActionRef              locationsAction;
      Model::SingleLineSelectionRef selection;
      String                        *fullStr;
      Button                        *locations;
      Table                         *table;
      bool                          result;
      OptionsRef                    options;

    private:
      void SetPath(const std::wstring& path);
      void HandleChange();
      void GotoParent();
      void GotoChildDirectory(const std::wstring& dir);

      bool GetSelection(Base::DirScanner::Entry& entry) const;

      void OnSelectionChange();
      void OnDoubleClick();

      void OnOk();

      void Load();

    public:
      File();
      virtual ~File();

      Lum::Object* GetContent();
      void GetActions(std::vector<ActionInfo>& actions);

      void Resync(Base::Model* model, const Base::ResyncMsg& msg);

      void SetOptions(Options* options);
      bool GetResult() const;
      std::wstring GetPath() const;

      static bool GetFile(OS::Window* parent,
                          const std::wstring& title,
                          std::wstring& file,
                          Options* options=NULL);
      static bool SaveFile(OS::Window* parent,
                           const std::wstring& title,
                           std::wstring& file,
                           Options* options=NULL);

      static bool GetDir(OS::Window* parent,
                         const std::wstring& title,
                         std::wstring& dir,
                         Options* options=NULL);
    };
  }
}

#endif
