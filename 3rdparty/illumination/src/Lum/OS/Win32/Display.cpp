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

#include <Lum/OS/Win32/Display.h>

#include <stdio.h>
#include <iostream>

#include <Lum/Base/String.h>

#include <Lum/OS/Base/Behaviour.h>

#include <Lum/OS/Win32/DrawInfo.h>
#include <Lum/OS/Win32/Image.h>
#include <Lum/OS/Win32/Tray.h>
#include <Lum/OS/Win32/Win32Theme.h>
#include <Lum/OS/Win32/Window.h>

namespace Lum {
  namespace OS {
    namespace Win32 {

      WNDCLASS wClass;
      ATOM     wClassAtom;

      // In Window.cpp
      extern LRESULT CALLBACK MessageHandler(HWND wnd,UINT msg, WPARAM wParam, LPARAM lParam);

      void CALLBACK TimerProc(HWND /*hWnd*/, UINT /*uMsg*/, UINT /*idEvent*/, DWORD /*dwTime*/)
      {
      }

      class Win32MsgEventSource : public EventSource
      {
      private:
        Display *display;

      public:
        Win32MsgEventSource(Display *display)
        {
          this->display=display;
        }

        bool Matches(const Model::Action* action) const
        {
          return false;
        }

        bool GetTimeout(Lum::Base::SystemTime& time) const
        {
          return false;
        }

        bool GetFileIO(FileIO& fileIO) const
        {
          return false;
        }

        bool DataAvailable() const
        {
          return GetQueueStatus(QS_ALLINPUT)!=0;
        }

        bool DataAvailable(Lum::Base::SystemTime& time) const
        {
          return false;
        }

        bool DataAvailable(const FileIO& fileIO) const
        {
          return false;
        }

        void Trigger()
        {
          MSG msg;

          while (::PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
            if (msg.message==WM_QUIT) {
              break;
            }
            ::DispatchMessage(&msg);
          }
        }

        bool IsFinished() const
        {
          return false;
        }
      };

      Display::Display()
       : selectObject(NULL),
         instance(0),
         hdc(0),
         appWindow(0)
      {
      }

      double Display::GetDPI() const
      {
        return (::GetDeviceCaps(hdc,LOGPIXELSX)+::GetDeviceCaps(hdc,LOGPIXELSY))/2*1.5;
      }

      void Display::Beep()
      {
        ::MessageBeep(MB_OK);
      }

      bool Display::AllocateColor(double red, double green, double blue,
                                  OS::Color& color)
      {
        color=RGB((unsigned char)(red*255),(unsigned char)(green*255),(unsigned char)(blue*255));

        return true;
      }

      bool Display::AllocateNamedColor(const std::string& name, Color& color)
      {
        if (name=="black") {
          color=RGB(0,0,0);
        }
        else if (name=="red") {
          color=RGB(0xFF,0,0);
        }
        else if (name=="green") {
          color=RGB(0,0xFF,0);
        }
        else if (name=="dark green") {
          color=RGB(0x0,0x64,0x0);
        }
        else if (name=="yellow") {
          color=RGB(0xFF,0xFF,0);
        }
        else if (name=="blue") {
          color=RGB(0,0,0xFF);
        }
        else if (name=="magenta") {
          color=RGB(0xFF,0,0xFF);
        }
        else if (name=="dark magenta") {
          color=RGB(0x0b,0,0x0b);
        }
        else if (name=="cyan") {
          color=RGB(0,0xFF,0xff);
        }
        else if (name=="dark cyan") {
          color=RGB(0,0x8b,0x8b);
        }
        else if (name=="white") {
          color=RGB(0xff,0xff,0xff);
        }
        else if (name=="grey") {
          color=RGB(0xAA,0xAA,0xAA);
        }
        else if (name=="windows grey") {
          color=RGB(212,208,200);
        }
        else if (name=="dark slate grey")  {
          color=RGB(0x2f,0x4f,0x4f);
        }
        else if (name=="grey70") {
          color=RGB(0xB3,0xB3,0xB3);
        }
        else if (name=="grey95") {
          color=RGB(0xF2,0xF2,0xF2);
        }
        else if (name=="grey82") {
          color=RGB(0xD1,0xD1,0xD1);
        }
        else if (name=="grey45") {
          color=RGB(0x73,0x73,0x73);
        }
        else if (name=="grey20") {
          color=RGB(0x33,0x33,0x33);
        }
        else if (name=="grey60") {
          color=RGB(0x99,0x99,0x99);
        }
        else if (name=="grey92") {
          color=RGB(0xEB,0xEB,0xEB);
        }
        else if (name=="grey30") {
          color=RGB(0x4D,0x4D,0x4D);
        }
        else if (name=="royal blue") {
          color=RGB(0x41,0x69,0xE1);
        }
        else if (name=="light yellow") {
          color=RGB(0xff,0xff,0xE0);
        }
        else if (name=="orange") {
          color=RGB(0xff,0xA5,0x0);
        }
        else if (name=="dark orange") {
          color=RGB(0xff,0x8c,0);
        }
        else if (name=="dim grey") {
          color=RGB(0x69,0x69,0x69);
        }
        else if (name=="yellow4") {
          color=RGB(0x8B,0x8B,0x0);
        }
        else if (name=="dark slate grey") {
          color=RGB(0x2F,0x4F,0x4F);
        }
        else if (name=="steel blue") {
          color=RGB(0x46,0x8C,0xB8);
        }
        else if (name=="purple") {
          color=RGB(0xa0,0x20,0xF0);
        }
        else if (name=="dark violet") {
          color=RGB(0x94,0,0xd3);
        }
        else if (name=="aquamarine") {
          color=RGB(0x7F,0xff,0xC8);
        }
        else if (name=="pale green") {
          color=RGB(0x98,0xff,0x98);
        }
        else if (name=="brown") {
          color=RGB(0xa5,0x2a,0x2a);
        }
        else if (name=="windows background") {
          color=::GetSysColor(COLOR_BTNFACE);
        }
        else if (name=="windows table background") {
          color=::GetSysColor(COLOR_WINDOW);
        }
        else if (name=="windows table text") {
          color=::GetSysColor(COLOR_WINDOWTEXT);
        }
        else if (name=="windows button background") {
          color=::GetSysColor(COLOR_BTNFACE);
        }
        else if (name=="windows text") {
          color=::GetSysColor(COLOR_BTNTEXT);
        }
        else if (name=="windows hilight") {
          color=::GetSysColor(COLOR_3DHIGHLIGHT);
        }
        else if (name=="windows light") {
          color=::GetSysColor(COLOR_3DLIGHT);
        }
        else if (name=="windows shadow") {
          color=::GetSysColor(COLOR_3DSHADOW);
        }
        else if (name=="windows dark shadow") {
          color=::GetSysColor(COLOR_3DDKSHADOW);
        }
        else if (name=="windows fill") {
          color=::GetSysColor(COLOR_HIGHLIGHT);
        }
        else if (name=="windows text fill") {
          color=::GetSysColor(COLOR_HIGHLIGHTTEXT);
        }
        else if (name=="windows help background") {
          color=::GetSysColor(COLOR_INFOBK);
        }
        else {
          std::cout << "Cannot resolve color name '" << name << "'!" << std::endl;
          return false;
        }

        return true;
      }

      void Display::FreeColor(OS::Color /*color*/)
      {
        // TODO
      }

      FontRef Display::GetFontInternal(FontType type, size_t size) const
      {
        FontRef font;

        font=driver->CreateFont();
        font->features=Font::attrName|Font::attrHeight|Font::attrSpacing;

        switch (type) {
        case fontTypeProportional:
          font->name=propFont->name;
          font->spacing=Font::spacingProportional;
          break;
        case fontTypeFixed:
          font->name=fixedFont->name;
          font->spacing=Font::spacingMonospace;
          break;
        }

        font->pixelHeight=size;

        font=font->Load();

        if (font.Valid()) {
          return font.Get();
        }
        else {
          return NULL;
        }
      }

      bool Display::RegisterSelection(Lum::Base::DataExchangeObject* object, OS::Window* /*window*/)
      {
        if (selectObject!=object) {
          if (selectObject!=NULL) {
            selectObject->Deselect();
            selectObject=NULL;
          }

          selectObject=object;
        }

        return true;
      }
      void Display::CancelSelection()
      {
        assert(selectObject!=NULL);

        selectObject->Deselect();
        selectObject=NULL;
      }

      Lum::Base::DataExchangeObject* Display::GetSelectionOwner() const
      {
        return selectObject;
      }

      bool Display::QuerySelection(OS::Window* /*window*/, Lum::Base::DataExchangeObject* /*object*/)
      {
        return false;
        // TODO
      }

      /**
        Some helper template function to get text out of the Windows clipboard.
      */
      template <class V>
      static HGLOBAL GetDataHandle(const V& data)
      {
        size_t                 tLen;
        typename V::value_type *tDest;

        size_t  x,y;
        HGLOBAL handle;

        tLen=data.length();
        for (x=0; x<data.length(); x++) {
          if (data[x]=='\n') {
            tLen++;
          }
        }

        handle=::GlobalAlloc(GMEM_MOVEABLE + GMEM_DDESHARE,(tLen+1)*sizeof(typename V::value_type));
        if (handle!=0) {
          tDest=(typename V::value_type*)GlobalLock(handle);
          x=0;
          y=0;
          while (x<data.length()) {
            if (data[x]=='\n') {
              tDest[y]='\r';
              y++;
              tDest[y]='\n';
            }
            else {
              tDest[y]=data[x];
            }
            x++;
            y++;
          }
          tDest[y]='\0';

          /* ignore */ ::GlobalUnlock(handle);
        }

        return handle;
      }

      template <class V>
      static void GetDataFromHandle(HGLOBAL handle, V& res)
      {
        typename V::value_type *data;
        size_t                 length,x;

        res.erase();

        data=(typename V::value_type*)::GlobalLock(handle);
        if (data!=NULL) {
          length=0;
          x=0;
          while (data[x]!='\0') {
            // compute length after translation
            length++;
            if (data[x]=='\r' && data[x+1]=='\n') {
              x++;
            }
            x++;
          }

          // translate text
          res.reserve(length);
          x=0;
          while (data[x]!='\0') {
            if (data[x]=='\r' && data[x+1]=='\n') {
              res.append(1,'\n');
              x++;
            }
            else {
              res.append(1,data[x]);
            }
            x++;
          }
        }
        /* ignore */ ::GlobalUnlock(handle);
      }

      bool Display::SetClipboard(const std::wstring& content)
      {
        std::string  t8;
        std::wstring t16;
        HGLOBAL      handle;

       if (OpenClipboard(0)) {
          t8=Lum::Base::WStringToString(content);
          t16=content;

          handle=GetDataHandle(t8);
          if (handle!=0) {
            SetClipboardData(CF_TEXT,handle);
          }

          handle=GetDataHandle(t8);
          if (handle!=0) {
            SetClipboardData(CF_OEMTEXT,handle);
          }

          handle=GetDataHandle(t16);
          if (handle!=0) {
            SetClipboardData(CF_UNICODETEXT,handle);
          }

          /* ignore */ CloseClipboard();
        }

        return true;
      }
      std::wstring Display::GetClipboard() const
      {
        std::wstring res;

        if (OpenClipboard(0)) {
          HGLOBAL handle;

          handle=GetClipboardData(CF_UNICODETEXT);
          if (handle!=0) {
            GetDataFromHandle(handle,res);
            /* ignore */ CloseClipboard();
          }
          else {
            handle=GetClipboardData(CF_TEXT);
            if (handle!=0) {
              std::string tmp;

              GetDataFromHandle(handle,tmp);
              res=Lum::Base::StringToWString(tmp);
            }
            else {
              handle=GetClipboardData(CF_OEMTEXT);
              if (handle!=0) {
                std::string tmp;

                GetDataFromHandle(handle,tmp);
                res=Lum::Base::StringToWString(tmp);
              }
            }
          }

          /* ignore */ CloseClipboard();
        }

        return res;
      }
      void Display::ClearClipboard()
      {
        if (OpenClipboard(0)) {
          EmptyClipboard();
          CloseClipboard();
        }
      }

      bool Display::HasClipboard() const
      {
        return !GetClipboard().empty();
      }

      void Display::ReinitWindows()
      {
        // TODO
      }

      bool Display::Open()
      {
        HBRUSH brush;

        instance=::GetModuleHandleA(NULL);
        if (instance==0) {
          std::cerr << "Cannot get module handle" << std::endl;
          return false;
        }
        hdc=::CreateDC("DISPLAY",NULL,NULL,NULL);
        if (hdc==NULL) {
          std::cerr << "Cannot get display dc:" << GetLastError() << std::endl;
          return false;
        }
        ::SetGraphicsMode(hdc,GM_ADVANCED);

        ::SetTextAlign(hdc,TA_BASELINE|TA_LEFT);

        Images::Factory::Set(driver->CreateImageFactory(this));

        brush=::CreateSolidBrush(backgroundColor);

        wClass.hCursor=::LoadCursorA(0,IDC_ARROW);
        wClass.hIcon=::LoadIconA(instance,MAKEINTRESOURCE(101));
        wClass.lpszMenuName=NULL;
        wClass.lpszClassName="IlluminationWindowClass";
        wClass.hbrBackground=0;
        wClass.style=CS_VREDRAW|CS_HREDRAW;
        wClass.hInstance=instance;
        wClass.lpfnWndProc=MessageHandler;
        wClass.cbClsExtra=0;
        wClass.cbWndExtra=0;
        wClassAtom=RegisterClassA(&wClass);
        if (wClassAtom==0) {
          std::cerr << "Cannot register window class: " << GetLastError() << std::endl;
          return false;
        }

        appWindow=::CreateWindowExA(0,
                                   (LPCSTR)wClassAtom,
                                   NULL,
                                   0,
                                   0,0,10,10,
                                   NULL,0,instance,0);

        if (appWindow==0) {
          std::cerr << "Cannot create hidden application window: " << ::GetLastError() << std::endl;
          return false;
        }

        if (SetGraphicsMode(GetDC(appWindow),GM_ADVANCED)==0) {
          std::cerr << "Cannot activate advanved grafics mode: " << ::GetLastError() << std::endl;
        }

        screenWidth=::GetSystemMetrics(SM_CXSCREEN);
        screenHeight=::GetSystemMetrics(SM_CYSCREEN);

        workAreaWidth=screenWidth;
        workAreaHeight=screenHeight;

        type=typeGraphical;

        EvaluateDisplaySize();

        colorMode=colorModeColor;

        eventLoop=new Win32EventLoop();
        eventLoop->AddSource(new Win32MsgEventSource(this));

        //D.smallChess=d.CreateBitmapPattern(D.disablePattern,D.disableWidth,D.disableHeight);
        //D.bigChess=d.CreateBitmapPattern(D.bigChessPattern,D.bigChessWidth,D.bigChessHeight);

        if (driver->GetDriverName()==L"Win32") {
          theme=new Win32Theme(this);
        }
        else if (driver->GetDriverName()==L"CairoWin32") {
          theme=new Win32Theme(this);
        }
        else {
          theme=new OS::Base::DefaultTheme(this);
        }

        propFont=driver->CreateFont();
        propFont->features=Font::attrName|Font::attrHeight|Font::attrSpacing;
        propFont->name=Lum::Base::WStringToString(theme->GetProportionalFontName());
        propFont->spacing=Font::spacingProportional;
        propFont->pixelHeight=theme->GetProportionalFontSize();

        propFont=propFont->Load();

        if (!propFont.Valid()) {
          std::cerr << "Cannot load font '"<< propFont->name << ", " << propFont->pixelHeight << "'" << std::endl;
          return false;
        }

        fixedFont=driver->CreateFont();
        fixedFont->features=Font::attrName|Font::attrHeight|Font::attrSpacing;
        fixedFont->name=Lum::Base::WStringToString(theme->GetFixedFontName());
        fixedFont->spacing=Font::spacingMonospace;
        fixedFont->pixelHeight=theme->GetFixedFontSize();

        fixedFont=fixedFont->Load();

        if (!fixedFont.Valid()) {
          std::cerr << "Cannot load font '" << fixedFont->name << ", " << fixedFont->pixelHeight << "'" << std::endl;
          return false;
        }

        propFontSize=propFont->pixelHeight;
        fixedFontSize=fixedFont->pixelHeight;

        for (size_t i=0; i<colorCount; i++) {
          color[i]=theme->GetColor((ColorIndex)i);
        }

        multiClickTime=200;

        return true;
      }

      void Display::Close()
      {
        //std::list<Window*>::iterator win;

        OS::Base::Display::Close();

        /*
        if (selectObject!=NULL) {
          CancelSelection();
        }
        */
        ClearClipboard();
        //d.selector.Close;

        /*
        win=winList.begin();
        while (win!=winList.end()) {
          std::cerr << "Warning: window '" << ::Lum::Base::WStringToString((*win)->GetTitle()) << "' not explicitely closed" << std::endl;
          (*win)->interface->Close(); // removes itself from list

          win=winList.begin();
        }
        */

        DestroyWindow(appWindow);

        delete eventLoop;
      }

      bool Display::GetMousePos(int& x, int& y) const
      {
        POINT cursorPos;

        if (GetCursorPos(&cursorPos)) {
          x=cursorPos.x;
          y=cursorPos.y;

          return true;
        }
        else {
          return false;
        }
      }

      /**
        Adds window to the internal list of windows.
      */
      void Display::AddWindow(Window* w)
      {
        winList.push_back(w);
      }

      /**
        Removes window from the internal list of windows.
      */
      void Display::RemoveWindow(Window* w)
      {
        winList.remove(w);
      }

      /**
        Get the Illumination window matching the given X11 window.
      */
      Window* Display::GetWindow(::HWND window)
      {
        std::list<Window*>::const_iterator iter;

        iter=winList.begin();
        while (iter!=winList.end()) {
          if ((*iter)->window==window) {
            return *iter;
          }

          ++iter;
        }
        return NULL;
      }

      void Display::AddTray(Tray* tray)
      {
        trayList.push_back(tray);
      }

      /**
        Removes the tray from the internal list of trays.
      */
      void Display::RemoveTray(Tray* w)
      {
        trayList.remove(w);
      }

      /**
        Return the Illumination tray belonging to the given X11 window.
      */
      Tray* Display::GetTray(UINT id)
      {
        std::list<Tray*>::const_iterator iter;

        iter=trayList.begin();
        while (iter!=trayList.end()) {
          if (((UINT)*iter)==id) {
            return *iter;
          }

          ++iter;
        }
        return NULL;
      }
    }
  }
}

