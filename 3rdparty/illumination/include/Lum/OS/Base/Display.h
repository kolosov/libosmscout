#ifndef LUM_OS_BASE_DISPLAY_H
#define LUM_OS_BASE_DISPLAY_H

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

#include <map>

#include <Lum/OS/Display.h>
#include <Lum/OS/Driver.h>
#include <Lum/OS/Theme.h>
#include <Lum/OS/Thread.h>

#include <Lum/Manager/Keyboard.h>

namespace Lum {
  namespace OS {
    namespace Base {

      class LUMAPI Display : public Lum::OS::Display
      {
      protected:
        Type                             type;

        ColorMode                        colorMode;
        size_t                           colorDepth;

        unsigned long                    screenWidth;
        unsigned long                    screenHeight;

        unsigned long                    workAreaWidth;
        unsigned long                    workAreaHeight;

        Model::ActionRef                 screenRotatedAction;

        unsigned long                    multiClickTime;

        std::wstring                     appName;
        std::wstring                     programName;
        std::vector<std::wstring>        args;

        std::string                      displayName;

        Size                             size;

        Color                            color[colorCount];
        Theme                            *theme;

        mutable std::map<size_t,FontRef> propFonts;
        size_t                           propFontSize;
        FontRef                          propFont;

        mutable std::map<size_t,FontRef> fixedFonts;
        size_t                           fixedFontSize;
        FontRef                          fixedFont;

        OS::EventLoop                    *eventLoop;

        ByteOrder                        systemByteOrder;

      private:
        OS::Font* GetFont(FontType type, size_t normal, size_t size,
                          std::map<size_t,FontRef>& cache, const FontRef &standard) const;

      protected:
        Display();

        void EvaluateDisplaySize();

        void TriggerScreenRotated();

        virtual FontRef GetFontInternal(FontType type, size_t size) const;

        bool GetFontFromCache(FontType type, size_t size, FontRef& font) const;

      public:
        void Close();

        // Getter
        Type GetType() const;
        Size GetSize() const;
        bool IsLandscape() const;
        bool IsPortrait() const;
        Model::Action* GetScreenRotatedAction() const;

        void SetAppName(const std::wstring& name);
        std::wstring GetAppName() const;

        void SetProgramName(const std::wstring& name);
        std::wstring GetProgramName() const;

        std::wstring GetThemePath() const;
        std::wstring GetThemeName() const;
        Theme* GetTheme() const;

        ColorMode GetColorMode() const;
        size_t GetColorDepth() const;

        unsigned long GetScreenWidth() const;
        unsigned long GetScreenHeight() const;

        unsigned long GetWorkAreaWidth() const;
        unsigned long GetWorkAreaHeight() const;

        unsigned long GetMultiClickTime() const;

        // Setter

        void SetColorDepth(size_t depth);

        void SetMultiClickTime(unsigned long time);

        // Font stuff
        OS::Font* GetFont(size_t scale) const;
        OS::Font* GetFont(FontType type, size_t scale) const;
        OS::Font* GetFontByPixel(FontType type, size_t size) const;
        OS::Font* GetFontByMaxPixel(FontType type, size_t size) const;

        // Color stuff
        Color GetColor(ColorIndex color) const;
        Color GetColorByName(const std::string& name);

        // Objecr stuff
        Fill* GetFill(FillIndex fill) const;
        Frame* GetFrame(FrameIndex frame) const;
        Image* GetImage(ImageIndex image) const;

        // Event Loop
        EventLoop* GetEventLoop() const;
        void MainLoop();
        void Exit();
        void QueueActionForAsyncNotification(Model::Action *action);
        void QueueActionForEventLoop(Model::Action *action);

        // Clipboard
        bool SetClipboard(const std::wstring& content);
        std::wstring GetClipboard() const;
        void ClearClipboard();

        // Timer
        void AddTimer(long seconds, long microseconds, Model::Action* action);
        void RemoveTimer(Model::Action* action);

        // Key-String-Mapping
        void KeyToKeyDescription(unsigned long qualifier,
                                 const std::wstring& key,
                                 std::wstring& description);
        void KeyToDisplayKeyDescription(unsigned long qualifier,
                                        const std::wstring& key,
                                        std::wstring& description);
        bool KeyDescriptionToKey(const std::wstring& description,
                                 unsigned long& qualifier,
                                 std::wstring& key);

        size_t GetSpaceHorizontal(Space space) const;
        size_t GetSpaceVertical(Space space) const;

        TextDirection GetDefaultTextDirection() const;

        Manager::Keyboard* GetKeyboardManager() const;

        ByteOrder GetSystemByteOrder() const;

      };

      class Driver : public OS::Driver
      {
      protected:
        Driver(const std::wstring name, int priority);
      };
    }
  }
}

#endif
