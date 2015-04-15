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

#include <Lum/Manager/FileSystem.h>

#include <Lum/Private/Config.h>

#include <cstdlib>

#if defined(__WIN32__) || defined(WIN32)
  #include <windows.h>
  #include <winnt.h>
  #include <direct.h>
  #include <shlobj.h>
  #include <Lum/OS/Win32/OSAPI.h>
#endif

#if defined(HAVE_LIB_GLIB)
  #include <glib.h>
#endif

#include <Lum/Base/L10N.h>
#include <Lum/Base/Path.h>
#include <Lum/Base/String.h>

#include <Lum/OS/Display.h>

namespace Lum {
  namespace Manager {

    static FileSystem* instance=NULL;

    static std::wstring applicationDataDir;

    const wchar_t* FileSystem::userDir            = L"user";
    const wchar_t* FileSystem::userConfigDir      = L"user.config";
    const wchar_t* FileSystem::userDesktopDir     = L"user.desktop";
    const wchar_t* FileSystem::userDocumentDir    = L"user.document";
    const wchar_t* FileSystem::userDownloadDir    = L"user.download";
    const wchar_t* FileSystem::userPublicShareDir = L"user.publicshare";
    const wchar_t* FileSystem::userPictureDir     = L"user.picture";
    const wchar_t* FileSystem::userMusicDir       = L"user.music";
    const wchar_t* FileSystem::userTemplateDir    = L"user.template";
    const wchar_t* FileSystem::userTmpDir         = L"user.tmp";
    const wchar_t* FileSystem::userVideoDir       = L"user.video";
    const wchar_t* FileSystem::libDataDir         =L"lib.data";
    const wchar_t* FileSystem::appDataDir         =L"app.data";
    const wchar_t* FileSystem::appConfigFile      =L"app.data.file";

    std::wstring FileSystem::GetDirFromEnvironment(const std::string& environment) const
    {
      const char* tmp;

      tmp=getenv(environment.c_str());
      if (tmp!=NULL) {
        Lum::Base::Path path;

        path.SetNativeDir(Lum::Base::StringToWString(tmp));

        return path.GetPath();
      }
      else {
        return L"";
      }
    }

    void FileSystem::SetApplicationDataDir(const std::wstring& directory)
    {
      applicationDataDir=directory;
    }

    FileSystem::FileSystem()
    {
      // no code
    }

    FileSystem::~FileSystem()
    {
      // no code
    }

    FileSystem* FileSystem::Instance()
    {
      if (instance==NULL) {
        instance=new FileSystem();
      }

      return instance;
    }

    bool FileSystem::GetEntry(const std::wstring& directory,
                              std::wstring& path) const
    {
      if (directory==userDir) {
        std::wstring home;

#if defined(__WIN32__) || defined(WIN32)
        path=GetDirFromEnvironment("USERPROFILE");
#else
        path=GetDirFromEnvironment("HOME");
#endif

        return !path.empty();
      }
      else if (directory==userConfigDir) {
#if defined(__WIN32__) || defined(WIN32)
        wchar_t p[MAX_PATH];

        if (SUCCEEDED(SHGetFolderPathW(NULL,
                                       CSIDL_LOCAL_APPDATA|CSIDL_FLAG_CREATE,
                                       NULL,
                                       0,
                                       p))) {
          path=p;
          return !path.empty();
        }
#elif defined(HAVE_LIB_GLIB)
        const gchar *dir=g_get_user_config_dir();
        if (dir==NULL) {
          dir="";
        }
        path=Lum::Base::StringToWString(dir);

        return !path.empty();
#else
        return GetDirectory(userDir,path);
#endif
      }
      else if (directory==userDesktopDir) {
#if defined(HAVE_LIB_GLIB)
        const gchar *dir=g_get_user_special_dir(G_USER_DIRECTORY_DESKTOP);
        if (dir==NULL) {
          dir="";
        }
        path=Lum::Base::StringToWString(dir);

        return !path.empty();
#endif
      }
      else if (directory==userDocumentDir) {
#if defined(HAVE_LIB_GLIB)
        const gchar *dir=g_get_user_special_dir(G_USER_DIRECTORY_DOCUMENTS);
        if (dir==NULL) {
          dir="";
        }
        path=Lum::Base::StringToWString(dir);

        return !path.empty();
#endif
      }
      else if (directory==userDownloadDir) {
#if defined(HAVE_LIB_GLIB)
        const gchar *dir=g_get_user_special_dir(G_USER_DIRECTORY_DOWNLOAD);
        if (dir==NULL) {
          dir="";
        }
        path=Lum::Base::StringToWString(dir);

        return !path.empty();
#endif
      }
      else if (directory==userMusicDir) {
#if defined(HAVE_LIB_GLIB)
        const gchar *dir=g_get_user_special_dir(G_USER_DIRECTORY_MUSIC);
        if (dir==NULL) {
          dir="";
        }
        path=Lum::Base::StringToWString(dir);

        return !path.empty();
#endif
      }
      else if (directory==userPictureDir) {
#if defined(HAVE_LIB_GLIB)
        const gchar *dir=g_get_user_special_dir(G_USER_DIRECTORY_PICTURES);
        if (dir==NULL) {
          dir="";
        }
        path=Lum::Base::StringToWString(dir);

        return !path.empty();
#endif
      }
      else if (directory==userPublicShareDir) {
#if defined(HAVE_LIB_GLIB)
        const gchar *dir=g_get_user_special_dir(G_USER_DIRECTORY_PUBLIC_SHARE);
        if (dir==NULL) {
          dir="";
        }
        path=Lum::Base::StringToWString(dir);

        return !path.empty();
#endif
      }
      else if (directory==userTemplateDir) {
#if defined(HAVE_LIB_GLIB)
        const gchar *dir=g_get_user_special_dir(G_USER_DIRECTORY_TEMPLATES);
        if (dir==NULL) {
          dir="";
        }
        path=Lum::Base::StringToWString(dir);

        return !path.empty();
#endif
      }
      else if (directory==userTmpDir) {
#if defined(HAVE_LIB_GLIB)
        const gchar *dir=g_get_tmp_dir();
        if (dir==NULL) {
          dir="";
        }
        path=Lum::Base::StringToWString(dir);

        return !path.empty();
#elif defined(__WIN32__) || defined(WIN32)
        path=GetDirFromEnvironment("TEMP");

        return !path.empty();
#else
        path=GetDirFromEnvironment("TMPDIR");
        if (path.empty()) {
          path=GetDirFromEnvironment("TMP");
        }
        if (path.empty()) {
          path=L"/tmp";
        }

        return path.empty();
#endif
      }
      else if (directory==userVideoDir) {
#if defined(HAVE_LIB_GLIB)
        const gchar *dir=g_get_user_special_dir(G_USER_DIRECTORY_VIDEOS);
        if (dir==NULL) {
          dir="";
        }
        path=Lum::Base::StringToWString(dir);

        return !path.empty();
#endif
      }
      else if (directory==libDataDir) {
#if defined(LIB_DATADIR)
        path=Base::StringToWString(LIB_DATADIR);
        return !path.empty();
#else
        return false;
#endif
      }
      else if (directory==appDataDir) {
        path=applicationDataDir;
        return !path.empty();
      }
      else if (directory==appConfigFile) {
        std::wstring    configDir;
        std::wstring    homeDir;
        std::wstring    filename;
        Lum::Base::Path p;

        if (!GetEntry(userConfigDir,configDir)) {
          return false;
        }

        if (!GetEntry(userDir,homeDir)) {
          return false;
        }

        filename=OS::display->GetAppName();

        filename.append(L".xml");

        if (configDir==homeDir) {
          Lum::Base::ToLower(filename);

          filename.insert(0,L".");
        }

        p.SetNativeDirAndFile(configDir,filename);

        path=p.GetPath();

        return true;
      }

      return false;
    }

    bool FileSystem::GetPlaces(std::list<Place>& places) const
    {
      std::wstring path;

      places.clear();

#if defined(__WIN32__) || defined(WIN32)
    size_t       size,pos;
    wchar_t      *buffer;
    std::wstring name;

    size=GetLogicalDriveStringsW(0,NULL);

    buffer=new wchar_t[size];
    if (GetLogicalDriveStringsW(size,buffer)<=size) {
      pos=0;
      while (buffer[pos]!=L'\0') {
        while (buffer[pos]!=L'\0') {
          name.append(1,buffer[pos]);
          pos++;
        }

        Place place;

        place.description=name;
        place.path=name;

        places.push_back(place);
        name.erase();

        pos++;
      }
    }

    delete [] buffer;
#else
      {
        Place place;

        place.description=_l(L"PLACE_ROOT",L"Rroot");
        place.path=L"/";
        places.push_back(place);
      }

#endif

      if (GetEntry(userDir,path)) {
        Place place;

        place.description=_l(L"PLACE_HOME",L"Home");
        place.path=path;
        places.push_back(place);
      }

      if (GetEntry(userDesktopDir,path)) {
        Place place;

        place.description=_l(L"PLACE_DESKTOP",L"Desktop");
        place.path=path;
        places.push_back(place);
      }

      if (GetEntry(userDocumentDir,path)) {
        Place place;

        place.description=_l(L"PLACE_DOCUMENT",L"Documents");
        place.path=path;
        places.push_back(place);
      }

      if (GetEntry(userDownloadDir,path)) {
        Place place;

        place.description=_l(L"PLACE_DOWNLOAD",L"Downloads");
        place.path=path;
        places.push_back(place);
      }

      if (GetEntry(userMusicDir,path)) {
        Place place;

        place.description=_l(L"PLACE_MUSIC",L"Music");
        place.path=path;
        places.push_back(place);
      }

      if (GetEntry(userPictureDir,path)) {
        Place place;

        place.description=_l(L"PLACE_PICTURE",L"Pictures");
        place.path=path;
        places.push_back(place);
      }

      if (GetEntry(userPublicShareDir,path)) {
        Place place;

        place.description=_l(L"PLACE_PUBLICSHARE",L"Public share");
        place.path=path;
        places.push_back(place);
      }

      if (GetEntry(userTemplateDir,path)) {
        Place place;

        place.description=_l(L"PLACE_TEMPLATE",L"Templates");
        place.path=path;
        places.push_back(place);
      }

      if (GetEntry(userTmpDir,path)) {
        Place place;

        place.description=_l(L"PLACE_TMP",L"Temporary");
        place.path=path;
        places.push_back(place);
      }

      if (GetEntry(userVideoDir,path)) {
        Place place;

        place.description=_l(L"PLACE_VIDEO",L"Videos");
        place.path=path;
        places.push_back(place);
      }

      return false;
    }
  }
}
