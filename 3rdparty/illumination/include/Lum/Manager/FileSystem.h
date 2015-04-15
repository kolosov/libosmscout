#ifndef LUM_MANAGER_FILESYSTEM_H
#define LUM_MANAGER_FILESYSTEM_H

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

#include <Lum/Base/Model.h>

#include <Lum/Model/Action.h>

namespace Lum {
  namespace Manager {
    /**
      Manager for handling of advanced display aspects.
    */
    class LUMAPI FileSystem : public Lum::Base::MsgObject
    {
    protected:
      FileSystem();

    public:
      static const wchar_t* userDir;
      static const wchar_t* userConfigDir;
      static const wchar_t* userDesktopDir;
      static const wchar_t* userDocumentDir;
      static const wchar_t* userDownloadDir;
      static const wchar_t* userMusicDir;
      static const wchar_t* userPictureDir;
      static const wchar_t* userPublicShareDir;
      static const wchar_t* userTemplateDir;
      static const wchar_t* userTmpDir;
      static const wchar_t* userVideoDir;

      static const wchar_t* libDataDir;

      static const wchar_t* appDataDir;

      static const wchar_t* appConfigFile;

    public:
      struct Place
      {
        std::wstring description;
        std::wstring path;
      };

    public:
      virtual ~FileSystem();

      /**
        Return the global FileSystemManager instance.

        An instance is optional.
        */
      static FileSystem* Instance();

      std::wstring GetDirFromEnvironment(const std::string& environment) const;

      void SetApplicationDataDir(const std::wstring& directory);

      /**
        Returns the path of the given logical directory in the path variable. If there is a
        valid physical directory for the given logical directory true is returned, else false.
       */
      virtual bool GetEntry(const std::wstring& directory,
                            std::wstring& path) const;

      /**
        Return a list of predefined places.

        Places are desktop specific shortcuts for certain important target directories.
       */
      virtual bool GetPlaces(std::list<Place>& places) const;
    };
  }
}

#endif
