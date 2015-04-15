/*
  This source is part of the Illumination library
  Copyright (C) 2005  Tim Teulings

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

#include <Lum/OS/Base/Display.h>

#include <Lum/Private/Config.h>
#include <wctype.h>

#include <Lum/Base/Path.h>
#include <Lum/Base/String.h>
#include <Lum/Base/Util.h>

#include <Lum/Manager/FileSystem.h>

namespace Lum {
  namespace OS {
    namespace Base {

      static bool IsLowerByteSet(unsigned char *bytes)
      {
        return bytes[0]!=0;
      }

      Display::Display()
      : screenWidth(0),
        screenHeight(0),
        workAreaWidth(0),
        workAreaHeight(0),
        screenRotatedAction(new Model::Action()),
        multiClickTime(200),
        appName(L""),
        theme(NULL),
        eventLoop(NULL)
      {
        int i=1;

        if (IsLowerByteSet((unsigned char*)&i)) {
          systemByteOrder=littleEndian;
        }
        else {
          systemByteOrder=bigEndian;
        }
      }

      void Display::EvaluateDisplaySize()
      {
        switch (type) {
        case typeGraphical:
          if (screenWidth<=640) {
            size=sizeTiny;
          }
          else if (screenWidth<1024) {
            size=sizeSmall;
          }
          else if (screenWidth<1280) {
            size=sizeNormal;
          }
          else {
            size=sizeHuge;
          }
          break;
        case typeTextual:
          size=sizeTiny;
          break;
        }
      }

      void Display::TriggerScreenRotated()
      {
        screenRotatedAction->Trigger();
      }

      void Display::Close()
      {
        if (GetKeyboardManager()!=NULL) {
          GetKeyboardManager()->Shutdown();
        }

        for (std::map<size_t,FontRef>::iterator iter=propFonts.begin(); iter!=propFonts.end(); ++iter) {
          iter->second=NULL;
        }

        for (std::map<size_t,FontRef>::iterator iter=fixedFonts.begin(); iter!=fixedFonts.end(); ++iter) {
          iter->second=NULL;
        }

        propFont=NULL;
        fixedFont=NULL;

        delete theme;
      }

      std::wstring Display::GetAppName() const
      {
        return appName;
      }

      void Display::SetAppName(const std::wstring& name)
      {
        appName=name;
      }

      std::wstring Display::GetProgramName() const
      {
        return programName;
      }

      void Display::SetProgramName(const std::wstring& name)
      {
        programName=name;
      }

      std::wstring Display::GetThemePath() const
      {
        std::wstring    dir;
        Lum::Base::Path path;

        Manager::FileSystem::Instance()->GetEntry(Manager::FileSystem::userDir,dir);

        path.SetNativeDir(dir);
        path.AppendDir(L".illumination");
        path.AppendDir(L"themes");

        return path.GetPath();
      }

      std::wstring Display::GetThemeName() const
      {
        Lum::Base::Path path;

        path.SetNativeDir(GetThemePath());
        path.SetBaseName(L"theme.xml");

        return path.GetPath();
      }

      Theme* Display::GetTheme() const
      {
        return theme;
      }

      Display::Type Display::GetType() const
      {
        return type;
      }

      Display::Size Display::GetSize() const
      {
        return size;
      }

      bool Display::IsLandscape() const
      {
        return GetScreenWidth()>=GetScreenHeight();
      }

      bool Display::IsPortrait() const
      {
        return GetScreenWidth()<GetScreenHeight();
      }

      Model::Action* Display::GetScreenRotatedAction() const
      {
        return screenRotatedAction;
      }

      Display::ColorMode Display::GetColorMode() const
      {
        return colorMode;
      }

      size_t Display::GetColorDepth() const
      {
        return colorDepth;
      }

      unsigned long Display::GetScreenWidth() const
      {
        return screenWidth;

      }
      unsigned long Display::GetScreenHeight() const
      {
        return screenHeight;
      }

      unsigned long Display::GetWorkAreaWidth() const
      {
        if (workAreaWidth==0) {
          return GetScreenWidth();
        }
        else {
          return workAreaWidth;
        }
      }
      unsigned long Display::GetWorkAreaHeight() const
      {
        if (workAreaHeight==0) {
          return GetScreenHeight();
        }
        else {
          return workAreaHeight;
        }
      }

      unsigned long Display::GetMultiClickTime() const
      {
        return multiClickTime;
      }

      void Display::SetColorDepth(size_t depth)
      {
        colorDepth=depth;
      }

      void Display::SetMultiClickTime(unsigned long time)
      {
        multiClickTime=time;
      }

      Color Display::GetColor(ColorIndex color) const
      {
        return this->color[color];
      }

      Fill* Display::GetFill(FillIndex fill) const
      {
        return theme->GetFill(fill);
      }

      Frame* Display::GetFrame(FrameIndex frame) const
      {
        return theme->GetFrame(frame);
      }

      Image* Display::GetImage(ImageIndex image) const
      {
        return theme->GetImage(image);
      }

      Color Display::GetColorByName(const std::string& name)
      {
        int index;

        index=GetColorIndexByName(name);

        assert(index>=0);

        return GetColor((ColorIndex)index);
      }

      EventLoop* Display::GetEventLoop() const
      {
        return eventLoop;
      }

      void Display::MainLoop()
      {
        eventLoop->Run();
      }

      void Display::Exit()
      {
        eventLoop->Exit();
      }

      void Display::QueueActionForAsyncNotification(Model::Action *action)
      {
        eventLoop->QueueActionForAsyncNotification(action);
      }

      void Display::QueueActionForEventLoop(Model::Action *action)
      {
        eventLoop->QueueActionForEventLoop(action);
      }

      void Display::AddTimer(long seconds, long microseconds,
                             Model::Action* action)
      {
        assert(action!=NULL);

        eventLoop->AddTimer(seconds,microseconds,action);
      }

      void Display::RemoveTimer(Model::Action* action)
      {
        eventLoop->RemoveSource(action);
      }

      bool Display::SetClipboard(const std::wstring& /*content*/)
      {
        return false;
      }

      std::wstring Display::GetClipboard() const
      {
        return L"";
      }

      void Display::ClearClipboard()
      {
        // no code
      }

     void Display::KeyToKeyDescription(unsigned long qualifier,
                                       const std::wstring& key,
                                       std::wstring& description)
     {
       description=L"";
       if (qualifier!=0) {
         if (qualifier & (qualifierShiftLeft|qualifierShiftRight)) {
           description+=L"S";
         }
         if (qualifier & (qualifierControlLeft|qualifierControlRight)) {
           description+=L"C";
         }
         if (qualifier & (qualifierAltLeft|qualifierAltRight)) {
           description+=L"A";
         }
         if (qualifier & (qualifierMetaLeft|qualifierMetaRight)) {
           description+=L"M";
         }
         if (qualifier & (qualifierSuperLeft|qualifierSuperRight)) {
           description+=L"X";
         }
         if (qualifier & (qualifierHyperLeft|qualifierHyperRight)) {
           description+=L"H";
         }
         if (qualifier & qualifierButton1) {
           description+=L"1";
         }
         if (qualifier & qualifierButton2) {
           description+=L"2";
         }
         if (qualifier & qualifierButton3) {
           description+=L"3";
         }
         if (qualifier & qualifierButton4) {
           description+=L"4";
         }
         if (qualifier & qualifierButton5) {
           description+=L"5";
         }
         description+=L"+";
       }
       description+=key;
     }

      void Display::KeyToDisplayKeyDescription(unsigned long qualifier,
                                               const std::wstring& key,
                                               std::wstring& description)
      {

        description=L"";
        if (qualifier!=0) {
          if (qualifier & qualifierShift) {
            if (description.length()>0) {
              description+=L"+";
            }
            description+=L"Shift";
          }
          if (qualifier & qualifierControl) {
            if (description.length()>0) {
              description+=L"+";
            }
            description+=L"Ctrl";
          }
          if (qualifier & qualifierAlt) {
           if (description.length()>0) {
             description+=L"+";
            }
            description+=L"Alt";
          }
          if (qualifier & qualifierMeta) {
            if (description.length()>0) {
              description+=L"+";
            }
            description+=L"Meta";
          }
          if (qualifier & qualifierSuper) {
            if (description.length()>0) {
             description+=L"+";
            }
            description+=L"Super";
          }
          if (qualifier & qualifierHyper) {
            if (description.length()>0) {
             description+=L"+";
            }
            description+=L"Hyper";
          }
         if (qualifier & qualifierButton1) {
           if (description.length()>0) {
             description+=L"+";
            }
           description+=L"MouseButton1";
         }
         if (qualifier & qualifierButton2) {
           if (description.length()>0) {
             description+=L"+";
            }
           description+=L"MouseButton2";
         }
         if (qualifier & qualifierButton3) {
           if (description.length()>0) {
             description+=L"+";
            }
           description+=L"MouseButton3";
         }
         if (qualifier & qualifierButton4) {
           if (description.length()>0) {
             description+=L"+";
            }
           description+=L"MouseButton4";
         }
         if (qualifier & qualifierButton5) {
           if (description.length()>0) {
             description+=L"+";
            }
           description+=L"MouseButton5";
         }
          description+=L"+";
        }

        if (key.length()==1) {
          // TODO: Better use local and std::toupper(char,locale) for this
          description.append(1,(wchar_t)towupper(key.at(0)));
        }
        else {
          description+=key;
        }
      }

      bool Display::KeyDescriptionToKey(const std::wstring& description,
                                        unsigned long& qualifier,
                                        std::wstring& key)
      {
        size_t pos;

        qualifier=0;
        key=L"";
        pos=0;
        while ((pos<description.length()) && (description[pos]!='+')) {
          pos++;
        }

        if (pos<description.length()) {
          for (size_t x=0; x<pos; x++) {
            switch (description[x]) {
            case 'S':
              qualifier|=qualifierShift;
              break;
            case 'C':
              qualifier|=qualifierControl;
              break;
            case 'A':
              qualifier|=qualifierAlt;
              break;
            case 'M':
              qualifier|=qualifierMeta;
              break;
            case 'X':
              qualifier|=qualifierSuper;
              break;
            case 'H':
              qualifier|=qualifierHyper;
              break;
            case '1':
              qualifier|=qualifierButton1;
              break;
            case '2':
              qualifier|=qualifierButton2;
              break;
            case '3':
              qualifier|=qualifierButton3;
              break;
            case '4':
              qualifier|=qualifierButton4;
              break;
            case '5':
              qualifier|=qualifierButton5;
              break;
            default:
              return false;
              break;
            }
          }

          ++pos;
        }
        else {
          pos=0;
        }

        if (pos>description.length()) {
          return false;
        }

        key=description.substr(pos);

        return true;
      }

      size_t Display::GetSpaceHorizontal(Space space) const
      {
        return theme->GetSpaceHorizontal(space);
      }

      size_t Display::GetSpaceVertical(Space space) const
      {
        return theme->GetSpaceVertical(space);
      }

      Display::TextDirection Display::GetDefaultTextDirection() const
      {
        return textDirectionLeftToRight;
      }

      FontRef Display::GetFontInternal(FontType /*type*/, size_t /*size*/) const
      {
        FontRef font;

        return font;
      }

      bool Display::GetFontFromCache(FontType type, size_t size, FontRef& font) const
      {
        if (type==fontTypeProportional) {
          std::map<size_t,FontRef>::const_iterator iter=propFonts.find(size);

          if (iter!=propFonts.end()) {
            font=iter->second;

            return true;
          }
        }
        else if (type==fontTypeFixed) {
          std::map<size_t,FontRef>::const_iterator iter=fixedFonts.find(size);

          if (iter!=fixedFonts.end()) {
            font=iter->second;

            return true;
          }
        }
        else {
          assert(false);
        }

        return false;
      }

      OS::Font* Display::GetFont(FontType type, size_t normal, size_t size,
                                 std::map<size_t,FontRef>& cache, const FontRef &standard) const
      {
        assert(standard.Valid());

        if (size>normal) {
          while (size>normal) {
            FontRef font;

            if (!GetFontFromCache(type,size,font)) {
              font=GetFontInternal(type,size);
              cache[size]=font;
            }

            if (font.Valid()) {
              return font.Get();
            }

            --size;
          }
        }
        else if (size<normal) {
          while (size<normal) {
            FontRef font;

            if (!GetFontFromCache(type,size,font)) {
              font=GetFontInternal(type,size);
              cache[size]=font;
            }

            if (font.Valid()) {
              return font.Get();
            }

            ++size;
          }
        }

        return standard.Get();
      }

      OS::Font* Display::GetFont(size_t scale) const
      {
        return GetFont(fontTypeProportional,scale);
      }

      OS::Font* Display::GetFont(FontType type, size_t scale) const
      {
        switch (type) {
        case fontTypeProportional:
          return GetFont(type,propFontSize,propFontSize*scale/100,propFonts,propFont);
        case fontTypeFixed:
          return GetFont(type,fixedFontSize,fixedFontSize*scale/100,fixedFonts,fixedFont);
        }

        return NULL;
      }

      OS::Font* Display::GetFontByPixel(FontType type, size_t size) const
      {
        FontRef font;

        if (type==fontTypeProportional) {
          if (!GetFontFromCache(type,size,font)) {
            font=GetFontInternal(type,size);
            propFonts[size]=font;
          }
        }
        else if (type==fontTypeFixed) {
          if (!GetFontFromCache(type,size,font)) {
            font=GetFontInternal(type,size);
            fixedFonts[size]=font;
          }
        }
        else {
          assert(false);
        }

        if (font.Valid()) {
          return font.Get();
        }
        else {
          return NULL;
        }
      }

      OS::Font* Display::GetFontByMaxPixel(FontType type, size_t size) const
      {
        switch (type) {
        case fontTypeProportional:
          assert(size>=propFontSize);
        case fontTypeFixed:
          assert(size>=fixedFontSize);
        }

        switch (type) {
        case fontTypeProportional:
          return GetFont(type,propFontSize,size,propFonts,propFont);
        case fontTypeFixed:
          return GetFont(type,fixedFontSize,size,fixedFonts,fixedFont);
        }

        return NULL;
      }

      Manager::Keyboard* Display::GetKeyboardManager() const
      {
        return Manager::Keyboard::Instance();
      }

      OS::Display::ByteOrder Display::GetSystemByteOrder() const
      {
        return systemByteOrder;
      }

      Driver::Driver(const std::wstring name, int priority)
      : OS::Driver(name,priority)
      {
        // no code
      }

    }
  }
}

