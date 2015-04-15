#ifndef LUM_MODEL_DIR_H
#define LUM_MODEL_DIR_H

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

#include <Lum/Base/DateTime.h>
#include <Lum/Base/Path.h>

#include <Lum/Model/Table.h>

namespace Lum {
  namespace Model {

    class LUMAPI Dir : public Table
    {
    public:
      enum Type
      {
        normalType,
        dirType
      };

      enum Right
      {
        rightRead    = 1 << 0,
        rightWrite   = 1 << 1,
        rightExecute = 1 << 2
      };

    public:
      class LUMAPI Filter
      {
      private:
        std::wstring name;

      public:
        Filter(const std::wstring& name);
        virtual ~Filter();

        std::wstring GetName() const;
        virtual bool Match(const std::wstring& directory, const Base::DirScanner::Entry& entry) const = 0;
      };

      class LUMAPI AllFilter : public Filter
      {
      public:
        AllFilter(const std::wstring& name);

        bool Match(const std::wstring& directory, const Base::DirScanner::Entry& entry) const;
      };

      class LUMAPI AllVisibleFilter : public Filter
      {
      public:
        AllVisibleFilter(const std::wstring& name);

        bool Match(const std::wstring& directory, const Base::DirScanner::Entry& entry) const;
      };


      class LUMAPI PatternFilter : public Filter
      {
        std::vector<std::wstring> patterns;

      public:
        PatternFilter(const std::wstring& name);

        bool Match(const std::wstring& directory, const Base::DirScanner::Entry& entry) const;

        void AddPattern(const std::wstring& pattern);
      };


    private:
      std::vector<Base::DirScanner::Entry> content;
      std::wstring                            directory;
      Filter                                  *filter;

    public:
      Dir();

      size_t GetRows() const;
      size_t GetColumns() const;

      void SetFilter(Filter* filter);
      void SetDirectory(const std::wstring& directory,
                        bool noHiddenFiles, bool onlyDirs);

      void GetEntry(size_t row, Base::DirScanner::Entry& entry) const;
    };

    typedef Base::Reference<Dir> DirRef;
  }
}

#endif
