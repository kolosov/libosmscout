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

#include <Lum/Base/L10N.h>

#include <iostream>
#include <memory>
#include <vector>

#include <Lum/Base/Path.h>
#include <Lum/Base/String.h>

#include <Lum/Config/Config.h>

#include <Lum/Manager/FileSystem.h>

#include <Lum/OS/Display.h>

#include <Lum/Private/Config.h>

namespace Lum {
  namespace Base {

    struct StringMapping
    {
      const wchar_t *source;
      const wchar_t *replacement;
    };

    //
    // class Catalog
    //

    class Catalog
    {
    public:
      enum Type {
        typeLibrary,
        typeApplication
      };

    private:
      std::map<std::wstring,std::wstring> entries;

    private:
      void AddText(const std::wstring& key, const std::wstring& text);
    public:
      std::wstring GetText(const std::wstring& key, const std::wstring& defaultValue, bool warn=true) const;

      static Catalog* LoadCatalog(const std::wstring& locale, const std::wstring& path, Type type);
    };

    static std::auto_ptr<Catalog> libCatalog;
    static bool                   libCatalogSearched=false;
    static std::auto_ptr<Catalog> appCatalog;
    static bool                   appCatalogSearched=false;
    static bool                   appCatalogFallback=false;

#if defined(__WIN32__) || defined(WIN32)
    static StringMapping languageMapping[]={
                                             {L"English",L"en"},
                                             {L"American",L"us"},
                                             {L"German",L"de"},
                                             {L"Italian",L"it"},
                                             {NULL,NULL}
                                           };
#endif

    void Catalog::AddText(const std::wstring& key, const std::wstring& text)
    {
      entries.insert(std::make_pair(key,text));
    }

    std::wstring Catalog::GetText(const std::wstring& key, const std::wstring& defaultValue, bool warn) const
    {
      std::map<std::wstring,std::wstring>::const_iterator iter;

      iter=entries.find(key);
      if (iter!=entries.end()) {
        return iter->second;
      }
      else {
        if (warn) {
          std::cerr << "Warning: No entry for key '" << Base::WStringToString(key) << "' in locale catalog!" << std::endl;
        }
        return defaultValue;
      }
    }

    //
    // class L10N
    //

    std::wstring L10N::locale;
    std::wstring L10N::fallbackLocale;

    std::wstring StripSubData(const std::wstring& data)
    {
      size_t dividerPos;

      dividerPos=data.find(L'_');

      if (dividerPos!=std::wstring::npos) {
        return data.substr(0,dividerPos);
      }
      else {
        return data;
      }
    }

    void L10N::SetLocale(const std::wstring& locale)
    {
      Lum::Base::L10N::locale=locale;

#if defined(__WIN32__) || defined(WIN32)
      size_t dividerPos;

      dividerPos=Lum::Base::L10N::locale.find(L'_');

      if (dividerPos!=std::wstring::npos) {
        std::wstring language=Lum::Base::L10N::locale.substr(0,dividerPos);

        for (size_t i=0; languageMapping[i].source!=NULL; i++) {
          if (language==languageMapping[i].source) {
            Lum::Base::L10N::locale.replace(0,dividerPos,languageMapping[i].replacement);
            break;
          }
        }
      }
#endif
    }

    void L10N::SetFallbackLocale(const std::wstring& locale)
    {
      Lum::Base::L10N::fallbackLocale=locale;

#if defined(__WIN32__) || defined(WIN32)
      size_t dividerPos;

      dividerPos=Lum::Base::L10N::fallbackLocale.find(L'_');

      if (dividerPos!=std::wstring::npos) {
        std::wstring language=Lum::Base::L10N::fallbackLocale.substr(0,dividerPos);

        for (size_t i=0; languageMapping[i].source!=NULL; i++) {
          if (language==languageMapping[i].source) {
            Lum::Base::L10N::fallbackLocale.replace(0,dividerPos,languageMapping[i].replacement);
            break;
          }
        }
      }
#endif
    }

    std::wstring L10N::GetLocalizedText(const std::wstring& key,
                                        const std::wstring& defaultValue)
    {
      if (!appCatalogSearched) {
        std::wstring appDataDir;

        if (Manager::FileSystem::Instance()->GetEntry(Manager::FileSystem::appDataDir,
                                                      appDataDir)) {
          appCatalog=std::auto_ptr<Catalog>(Catalog::LoadCatalog(locale,
                                                                 appDataDir,
                                                                 Catalog::typeApplication));
          if (appCatalog.get()!=NULL) {
            appCatalogFallback=(StripSubData(locale)==StripSubData(fallbackLocale));
          }
          else {
            appCatalogFallback=true;
            appCatalog=std::auto_ptr<Catalog>(Catalog::LoadCatalog(fallbackLocale,
                                                                   appDataDir,
                                                                   Catalog::typeApplication));
          }
        }
        else {
          std::cerr << "Cannot retrieve application data directory!" << std::endl;
        }

        appCatalogSearched=true;
      }

      if (appCatalog.get()!=NULL) {
        return appCatalog->GetText(key,defaultValue,!appCatalogFallback);
      }
      else {
        return defaultValue;
      }
    }

    std::wstring L10N::GetLocalizedLibText(const std::wstring& key,
                                           const std::wstring& defaultValue)
    {
      if (!libCatalogSearched) {
        std::wstring libDataDir;

        if (Manager::FileSystem::Instance()->GetEntry(Manager::FileSystem::libDataDir,
                                                      libDataDir)) {
          libCatalog=std::auto_ptr<Catalog>(Catalog::LoadCatalog(locale,
                                                                 libDataDir,
                                                                 Catalog::typeLibrary));
        }

        libCatalogSearched=true;
      }

      if (libCatalog.get()!=NULL) {
        return libCatalog->GetText(key,defaultValue);
      }
      else {
        return defaultValue;
      }
    }

    std::wstring L10N::GetLocalizedDefaultText(Default id)
    {
      switch (id) {
      case menuProject:
        return GetLocalizedLibText(L"MENU_PROJECT",L"_Project");
      case menuProjectQuit:
        return GetLocalizedLibText(L"MENU_PROJECT_QUIT",L"_Quit");

      case menuEdit:
        return GetLocalizedLibText(L"MENU_EDIT",L"_Edit");
      case menuEditSettings:
        return GetLocalizedLibText(L"MENU_EDIT_SETTINGS",L"_Settings");

      case menuHelp:
        return GetLocalizedLibText(L"MENU_HELP",L"_Help");
      case menuHelpHelp:
        return GetLocalizedLibText(L"MENU_HELP_HELP",L"Help");
      case menuHelpAbout:
        return GetLocalizedLibText(L"MENU_HELP_ABOUT",L"About");

      case dlgButtonAbout:
        return GetLocalizedLibText(L"DLG_BUTTON_ABOUT",L"_About...");
      case dlgButtonCancel:
        return GetLocalizedLibText(L"DLG_BUTTON_CANCEL",L"_Cancel");
      case dlgButtonHelp:
        return GetLocalizedLibText(L"DLG_BUTTON_HELP",L"_Help");
      case dlgButtonOk:
        return GetLocalizedLibText(L"DLG_BUTTON_OK",L"_OK");
      case dlgButtonClose:
        return GetLocalizedLibText(L"DLG_BUTTON_CLOSE",L"_Close");
      case dlgButtonQuit:
        return GetLocalizedLibText(L"DLG_BUTTON_QUIT",L"_Quit");
      }

      assert(false); // We should never reach this point!!!
    }

    void L10N::Substitute(std::wstring& text,
                          const std::wstring& pattern,
                          const std::wstring& value)
    {
      std::string::size_type pos;

      pos=text.find(pattern);
      if (pos!=std::string::npos) {
        text.replace(pos,pattern.length(),value);
      }
    }

    Catalog* Catalog::LoadCatalog(const std::wstring& locale,
                                  const std::wstring& path,
                                  Type type)
    {
      std::vector<Path>         paths;
      std::vector<std::wstring> localeNames;

      Path                      catalogPath;
      std::wstring              localeDir;
      std::wstring              tmp;

      Config::Node              *xml=NULL;
      Config::ErrorList         errorList;

      Catalog                   *catalog=NULL;

      if (type==typeApplication) {
        if (!path.empty()) {
          catalogPath.SetNativeDir(path);
          catalogPath.AppendDir(L"catalogs");
          paths.push_back(catalogPath);
        }

        catalogPath.SetNativeDir(Path::GetCWD());
        catalogPath.AppendDir(L"catalogs");
        paths.push_back(catalogPath);
      }
      else if (type==typeLibrary) {
        tmp=Manager::FileSystem::Instance()->GetDirFromEnvironment("ILLUMINATION_ROOT");
        if (!tmp.empty()) {
          catalogPath.SetNativeDir(tmp);
          catalogPath.AppendDir(L"catalogs");
          paths.push_back(catalogPath);
        }

        if (!path.empty()) {
          catalogPath.SetNativeDir(path);
          catalogPath.AppendDir(L"catalogs");
          catalogPath.AppendDir(StringToWString(PACKAGE_NAME));
          paths.push_back(catalogPath);
        }

        if (!path.empty()) {
          catalogPath.SetNativeDir(path);
          catalogPath.AppendDir(L"catalogs");
          paths.push_back(catalogPath);
        }
      }

      localeDir=locale;

      if (type==typeLibrary) {
        localeNames.push_back(std::wstring(L"Illumination")+L"_"+localeDir);
      }
      else if (type==typeApplication && OS::display!=NULL) {
        localeNames.push_back(OS::display->GetAppName()+L"_"+localeDir);
      }
      localeNames.push_back(localeDir);

      if (localeDir.find(L'.')!=std::wstring::npos) {
        localeDir=localeDir.substr(0,localeDir.find(L'.'));

        if (type==typeLibrary && OS::display!=NULL) {
          localeNames.push_back(std::wstring(L"Illumination")+L"_"+localeDir);
        }
        else if (type==typeApplication && OS::display!=NULL) {
          localeNames.push_back(OS::display->GetAppName()+L"_"+localeDir);
        }

        localeNames.push_back(localeDir);
      }

      if (localeDir.find(L'_')!=std::wstring::npos) {
        localeDir=localeDir.substr(0,localeDir.find(L'_'));

        if (type==typeLibrary && OS::display!=NULL) {
          localeNames.push_back(std::wstring(L"Illumination")+L"_"+localeDir);
        }
        else if (type==typeApplication && OS::display!=NULL) {
          localeNames.push_back(OS::display->GetAppName()+L"_"+localeDir);
        }

        localeNames.push_back(localeDir);
      }

      for (size_t p=0; p<paths.size(); p++) {
        for (size_t l=0; l<localeNames.size(); l++) {
          std::wstring name=localeNames[l];

          name.append(L".xml");

          catalogPath=paths[p];
          catalogPath.SetBaseName(name);

          // For debugging
          /*
          if (type==typeApplication) {
            std::cout << "Application catalog: ";
          }
          else {
            std::cout << "Library catalog: ";
          }
          std::cout << Lum::Base::WStringToString(catalogPath.GetPath()) << std::endl;
          */

          if (catalogPath.Exists()) {
            xml=Config::LoadConfigFromXMLFile(catalogPath.GetPath(),errorList);

            if (xml!=NULL) {
              //std::cout << "SUCCESS!" << std::endl;
              break;
            }
          }
        }
        if (xml!=NULL) {
          break;
        }
      }

      if (xml!=NULL) {
        if (xml->GetName()==L"illumination-l10n-catalog") {
          Config::Node::NodeList nodes;

          catalog=new Catalog();
          nodes=xml->GetChildren();

          for (Config::Node::NodeList::const_iterator iter=nodes.begin(); iter!=nodes.end(); iter++) {
            Config::Node *node=*iter;

            if (node->GetName()==L"entry") {
              std::wstring key;

              if (node->GetAttribute(L"key",key) && node->HasValue()) {
                catalog->AddText(key,node->GetValue());
              }
            }
          }
        }
        else {
          std::cerr << "Catalog file '" << Lum::Base::WStringToString(catalogPath.GetPath()) << "' is not valid!" << std::endl;
        }

        delete xml;
      }

      return catalog;
    }
  }
}

