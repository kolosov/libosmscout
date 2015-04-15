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

#include <Lum/Model/Dir.h>

#include <Lum/Private/Config.h>

#if defined(__WIN32__) || defined(WIN32)
  #include <io.h>
  #include <direct.h>
  #include <Lum/OS/Win32/OSAPI.h>

#else
  #include <dirent.h>
  #include <unistd.h>
#endif

#include <algorithm>

#include <Lum/Base/L10N.h>
#include <Lum/Base/String.h>

namespace Lum {
  namespace Model {

    bool lessEntry(const Base::DirScanner::Entry& e1, const Base::DirScanner::Entry& e2)
    {
      if (e1.isDir!=e2.isDir) {
        return e1.isDir;
      }
      else {
        return e1.name<e2.name;
      }
    }

    Dir::Filter::Filter(const std::wstring& name)
    : name(name)
    {
      // no code
    }

    Dir::Filter::~Filter()
    {
      // no code
    }

    std::wstring Dir::Filter::GetName() const
    {
      return name;
    }

    Dir::AllFilter::AllFilter(const std::wstring& name)
    : Filter(name)
    {
      // no code
    }

    bool Dir::AllFilter::Match(const std::wstring& directory, const Base::DirScanner::Entry& entry) const
    {
      return true;
    }

    Dir::AllVisibleFilter::AllVisibleFilter(const std::wstring& name)
    : Filter(name)
    {
      // no code
    }

    bool Dir::AllVisibleFilter::Match(const std::wstring& directory, const Base::DirScanner::Entry& entry) const
    {
      if (entry.name.length()>0) {
        return entry.name[0]!=L'.';
      }
      else {
        return true;
      }
    }

    Dir::PatternFilter::PatternFilter(const std::wstring& name)
    : Filter(name)
    {
      // no code
    }

    bool Dir::PatternFilter::Match(const std::wstring& directory, const Base::DirScanner::Entry& entry) const
    {
      if (entry.isDir) {
        return true;
      }

      for (size_t x=0; x<patterns.size(); ++x) {
        if (Base::MatchWildcard(patterns[x],entry.name,false)) {
          return true;
        }
      }
      return false;
    }

    void Dir::PatternFilter::AddPattern(const std::wstring& pattern)
    {
      patterns.push_back(pattern);
    }

    Dir::Dir()
    : filter(NULL)
    {
      // no code
    }

    size_t Dir::GetRows() const
    {
      return content.size();
    }

    size_t Dir::GetColumns() const
    {
      return 3;
    }

    void Dir::SetFilter(Filter* filter)
    {
      this->filter=filter;
    }

    void Dir::SetDirectory(const std::wstring& directory,
                           bool noHiddenFiles, bool onlyDirs)
    {
      std::string name;
      char        buffer[4096];

      name=Base::WStringToString(directory);

      content.clear();

      if (getcwd(buffer,sizeof(buffer))==NULL) {
        Notify();
        return;
      }

      if (chdir(name.c_str())!=0) {
        chdir(buffer);
        Notify();
        return;
      }

      Base::DirScannerRef     scanner=Base::DirScanner::Create(directory);
      Base::DirScanner::Entry entry;
      Base::Status            status;

      while (scanner->GetNext(entry,status)) {
        if ((!noHiddenFiles || (noHiddenFiles && !(entry.isHidden))) &&
            (!onlyDirs || (onlyDirs && entry.isDir)) &&
            entry.name!=L"." && entry.name!=L"..") {
          if (entry.isDir || filter==NULL || filter->Match(directory,entry)) {
            content.push_back(entry);
          }
        }
      }

      chdir(buffer);

      std::sort(content.begin(),content.end(),lessEntry);

      Notify();
    }

    void Dir::GetEntry(size_t row, Base::DirScanner::Entry& entry) const
    {
      entry=content[row-1];
    }
  }
}
