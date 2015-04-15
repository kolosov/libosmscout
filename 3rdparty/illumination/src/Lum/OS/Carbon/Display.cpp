/*
  This source is part of the Illumination library
  Copyright (C) 2006  Tim Teulings

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

#include <Lum/OS/Carbon/Display.h>

#include <assert.h>

// for select TODO: Write a configure test for it
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>

#include <iostream>

#include <Lum/Images/Image.h>

#include <Lum/Base/Fill.h>
#include <Lum/Base/Util.h>

#include <Lum/OS/Base/Theme.h>

//#include <Lum/OS/X11/Font.h>
//#include <Lum/OS/X11/Image.h>
//#include <Lum/OS/X11/Tray.h>
#include <Lum/OS/Carbon/Window.h>

#include <Lum/Prefs/Base.h>

namespace Lum {
  namespace OS {
    namespace Carbon {

      class Prefs : public ::Lum::OS::Display::Prefs
      {
      private:
        DisplayPtr display;

      public:
        Prefs(DisplayPtr display);
        ~Prefs();

        void EvaluateColor();
        void Initialize();

        void ReadConfig(::Lum::Config::Node *top);
        void ReadTheme(::Lum::Config::Node *top);
      };

      /**
        Initializes an instance.
      */
      Prefs::Prefs(DisplayPtr display)
      : display(display)
      {
        // no code
      }

      Prefs::~Prefs()
      {
        for (size_t x=0; x<Display::fontCount; x++) {
          fonts[x]=NULL;
        }
      }

      void Prefs::EvaluateColor()
      {
        if (display->GetColorMode()==Display::colorModeMonochrome) {
          color[Display::backgroundColor]="white";
          color[Display::tableTextColor]="black";
          color[Display::halfShineColor]="black";
          color[Display::halfShadowColor]="black";
          color[Display::textColor]="black";
          color[Display::textSelectColor]="black";
          color[Display::shineColor]="black";
          color[Display::shadowColor]="black";
          color[Display::fillColor]="black";
          color[Display::fillTextColor]="white";
          color[Display::warnColor]="black";
          color[Display::disabledColor]="black";
          color[Display::focusColor]="black";
          color[Display::blackColor]="black";
          color[Display::whiteColor]="white";
        }
        else if (display->GetColorMode()==Display::colorModeGreyScale) {
          color[Display::backgroundColor]="grey70";
          color[Display::tableTextColor]="black";
          color[Display::textColor]="black";
          color[Display::textSelectColor]="black";
          color[Display::shineColor]="grey95";
          color[Display::halfShineColor]="grey82";
          color[Display::halfShadowColor]="grey45";
          color[Display::shadowColor]="grey20";
          color[Display::fillColor]="grey60";
          color[Display::fillTextColor]="black";
          color[Display::warnColor]="grey92";
          color[Display::disabledColor]="grey20";
          color[Display::focusColor]="grey30";
          color[Display::blackColor]="black";
          color[Display::whiteColor]="white";
        }
        else {
          color[Display::backgroundColor]="grey70";
          color[Display::tableTextColor]="black";
          color[Display::textColor]="black";
          color[Display::textSelectColor]="black";
          color[Display::shineColor]="grey95";
          color[Display::halfShineColor]="grey82";
          color[Display::halfShadowColor]="grey45";
          color[Display::shadowColor]="grey20";
          color[Display::fillColor]="royal blue";
          color[Display::fillTextColor]="white";
          color[Display::warnColor]="red";
          color[Display::disabledColor]="grey20";
          color[Display::focusColor]="grey30";
          color[Display::blackColor]="black";
          color[Display::whiteColor]="white";
        }
      }

      void Prefs::Initialize()
      {
        EvaluateColor();

        propFont="Helvetica";
        propFontSize=12;

        fixedFont="Monaco";
        fixedFontSize=13;

        //p.contextTimer=2500;
      }

      void Prefs::ReadConfig(::Lum::Config::Node *top)
      {
        ::Lum::Config::Node *node,*sub;

        node=top->GetNode(L"theme");
        if (node!=NULL) {
          theme=node->GetString();
        }

        sub=top->GetNode(L"Fonts");
        if (sub!=NULL) {
          node=sub->GetNode(L"proportional");
          if (node!=NULL) {
            propFont=::Lum::Base::WStringToString(node->GetString());
          }
          node=sub->GetNode(L"proportionalSize");
          if (node!=NULL) {
            propFontSize=node->GetInt();
          }
          node=sub->GetNode(L"fixed");
          if (node!=NULL) {
            fixedFont=::Lum::Base::WStringToString(node->GetString());
          }
          node=sub->GetNode(L"fixedSize");
          if (node!=NULL) {
            fixedFontSize=node->GetInt();
          }
        }
      }

      void Prefs::ReadTheme(::Lum::Config::Node *top)
      {
        ::Lum::Config::Node *sub,*node;
        std::string         proportional,fixed;

        sub=top->GetNode(L"Colors");
        if (sub!=NULL) {
          for (size_t x=0; x<Display::colorCount; x++) {
            node=sub->GetSubNodeWithAttrValue(L"Color",L"name",
                                               ::Lum::Base::StringToWString(::Lum::OS::colorNames[x]));
            if (node!=NULL) {
              node=node->GetNode(L"value");
              if (node!=NULL) {
                color[x]=::Lum::Base::WStringToString(node->GetString());
              }
            }
          }
        }
      }

      double Display::GetDPI() const
      {
        // TODO
        return 92;
      }

      /**
        Call this method if you want the Display to stop generating
        QuickHelp calls to windows. This is necessesarry, if you are
        opening a QuickHelp and don't want to have a second after the
        second timeout.
      */
      void Display::StopContextHelp()
      {
        if (contextHelp) {
          contextHelp=false;
          /*if (d.contextTimer.active) {
            d.RemoveTimer(d.contextTimer);
          }*/
        }
      }

      /**
        Restart the generation of QuickHelp calls to windows stoped
        with Display.StopContextHelp.
      */
      void Display::RestartContextHelp()
      {
        /*if (d.contextTimer.active) {
          d.RemoveTimer(d.contextTimer);
        }
        d.AddTimer(d.contextTimer);*/
        contextHelp=true;
      }

      /**
        Restart the generation of QuickHelp calls to windows stoped
        with Display.StopContextHelp.
      */
      void Display::StartContextHelp()
      {
        if (!contextHelp) {
          //d.AddTimer(d.contextTimer);
          contextHelp=true;
        }
      }

      /**
        Adds the window to the internal list of windows.
      */
      void Display::AddWindow(WindowPtr w)
      {
        winList.push_back(w);
      }

      /**
        Removes the window from the internal list of windows.
      */
      void Display::RemoveWindow(WindowPtr w)
      {
        winList.remove(w);
      }

      /**
        Adds the tray to the internal list of trays.
      */
      /*void Display::AddTray(TrayPtr tray)
      {
        trayList.push_back(tray);
      }*/

      /**
        Removes the tray from the internal list of trays.
      */
      /*void Display::RemoveTray(TrayPtr w)
      {
        trayList.remove(w);
      }*/

      ::Lum::OS::Color RGB(unsigned char r, unsigned char g, unsigned char b)
      {
        return (r << 16)|(g << 8)|b;
      }

      void Display::AllocateColor(double red, double green, double blue,
                                  ::Lum::OS::Color defaultColor,
                                  ::Lum::OS::Color& color)
      {
        color=RGB((unsigned char)red*255,(unsigned char)green*255,(unsigned char)blue*255);
      }

      void Display::AllocateNamedColor(const char* name,
                                         ::Lum::OS::Color defaultColor,
                                         ::Lum::OS::Color& color)
      {
        if (strcmp(name,"black")==0) {
          color=RGB(0,0,0);
        }
        else if (strcmp(name,"red")==0) {
          color=RGB(0xFF,0,0);
        }
        else if (strcmp(name,"green")==0) {
          color=RGB(0,0xFF,0);
        }
        else if (strcmp(name,"dark green")==0) {
          color=RGB(0x0,0x64,0x0);
        }
        else if (strcmp(name,"yellow")==0) {
          color=RGB(0xFF,0xFF,0);
        }
        else if (strcmp(name,"blue")==0) {
          color=RGB(0,0,0xFF);
        }
        else if (strcmp(name,"magenta")==0) {
          color=RGB(0xFF,0,0xFF);
        }
        else if (strcmp(name,"dark magenta")==0) {
          color=RGB(0x0b,0,0x0b);
        }
        else if (strcmp(name,"cyan")==0) {
          color=RGB(0,0xFF,0xff);
        }
        else if (strcmp(name,"dark cyan")==0) {
          color=RGB(0,0x8b,0x8b);
        }
        else if (strcmp(name,"white")==0) {
          color=RGB(0xff,0xff,0xff);
        }
        else if (strcmp(name,"grey")==0) {
          color=RGB(0xAA,0xAA,0xAA);
        }
        else if (strcmp(name,"windows grey")==0) {
          color=RGB(212,208,200);
        }
        else if (strcmp(name,"dark slate grey")==0)  {
          color=RGB(0x2f,0x4f,0x4f);
        }
        else if (strcmp(name,"grey70")==0) {
          color=RGB(0xB3,0xB3,0xB3);
        }
        else if (strcmp(name,"grey95")==0) {
          color=RGB(0xF2,0xF2,0xF2);
        }
        else if (strcmp(name,"grey82")==0) {
          color=RGB(0xD1,0xD1,0xD1);
        }
        else if (strcmp(name,"grey45")==0) {
          color=RGB(0x73,0x73,0x73);
        }
        else if (strcmp(name,"grey20")==0) {
          color=RGB(0x33,0x33,0x33);
        }
        else if (strcmp(name,"grey60")==0) {
          color=RGB(0x99,0x99,0x99);
        }
        else if (strcmp(name,"grey92")==0) {
          color=RGB(0xEB,0xEB,0xEB);
        }
        else if (strcmp(name,"grey30")==0) {
          color=RGB(0x4D,0x4D,0x4D);
        }
        else if (strcmp(name,"royal blue")==0) {
          color=RGB(0x41,0x69,0xE1);
        }
        else if (strcmp(name,"light yellow")==0) {
          color=RGB(0xff,0xff,0xE0);
        }
        else if (strcmp(name,"orange")==0) {
          color=RGB(0xff,0xA5,0x0);
        }
        else if (strcmp(name,"dark orange")==0) {
          color=RGB(0xff,0x8c,0);
        }
        else if (strcmp(name,"dim grey")==0) {
          color=RGB(0x69,0x69,0x69);
        }
        else if (strcmp(name,"yellow4")==0) {
          color=RGB(0x8B,0x8B,0x0);
        }
        else if (strcmp(name,"dark slate grey")==0) {
          color=RGB(0x2F,0x4F,0x4F);
        }
        else if (strcmp(name,"steel blue")==0) {
          color=RGB(0x46,0x8C,0xB8);
        }
        else if (strcmp(name,"purple")==0) {
          color=RGB(0xa0,0x20,0xF0);
        }
        else if (strcmp(name,"dark violet")==0) {
          color=RGB(0x94,0,0xd3);
        }
        else if (strcmp(name,"aquamarine")==0) {
          color=RGB(0x7F,0xff,0xC8);
        }
        else if (strcmp(name,"pale green")==0) {
          color=RGB(0x98,0xff,0x98);
        }
        else if (strcmp(name,"brown")==0) {
          color=RGB(0xa5,0x2a,0x2a);
        }
        else {
          color=defaultColor;
        }
      }

      bool Display::IsAllocatedColor(::Lum::OS::Color /*color*/)
      {
        return true;
      }

      void Display::FreeColor(::Lum::OS::Color color)
      {
//        XFreeColors(display,colorMap,&color,1,0);
      }

      Display::Display()
      : /*currentWin(NULL),*/selectObject(NULL),querySelectObject(NULL),
      queryClipboardObject(NULL),selClearPend(false),contextHelp(true),/*dropWindow(NULL),*/
      exit(true)
      {
        // no code
      }

      bool Display::Open()
      {
        RgnHandle region;
        Rect      rect;
        Color     colors[colorCount];

        InitCursor();

        SetAntiAliasedTextEnabled(true,11);

        colorSpace=CGColorSpaceCreateDeviceRGB();
        colorDepth=24;

        ::Lum::Images::Factory::Set(::Lum::OS::Factory::factory->CreateImageFactory(this));

        if (DMGetDeskRegion(&region)!=noErr) {
          std::cerr << "Cannot get desktop region" << std::endl;
          return false;
        }

        if (GetRegionBounds(region,&rect)==NULL) {
          std::cerr << "Cannot get screen rect" << std::endl;
          return false;
        }

        scrWidth=rect.right-rect.left;
        scrHeight=rect.bottom-rect.top;

        type=typeGraphical;

        EvaluateDisplaySize();

        colorMode=colorModeColor;

        prefs=new ::Lum::OS::Carbon::Prefs(this);
        prefs->Initialize();

        ::Lum::Prefs::LoadConfig(GetPrefsName());
        ::Lum::Prefs::ReadDisplayConfig();
        if (!prefs->theme.empty()) {
          ::Lum::Prefs::LoadTheme(GetThemeName());
          ::Lum::Prefs::ReadDisplayTheme();
        }

        for (size_t i=0; i<colorCount; i++) {
          AllocateNamedColor(prefs->color[i].c_str(),colors[0],colors[i]);
        }

        for (size_t i=0; i<fillCount; i++) {
          AllocateNamedColor(prefs->fill[i].c_str(),fills[0],fills[i]);
        }

        theme=new ::Lum::OS::Base::DefaultTheme(this);

        rect.left=0;
        rect.top=0;
        rect.right=10;
        rect.bottom=10;

        if (NewGWorld(&fontScratch,0,&rect,NULL,NULL,0)!=0) {
          std::cerr << "Cannot create font scratch!" << std::endl;
          return false;
        }

        if (CreateCGContextForPort(fontScratch,&globalContext)!=::noErr) {
          std::cerr << "Cannot create context for font scratch!" << std::endl;
          return false;
        }

        propFont=::Lum::OS::Factory::factory->CreateFont();
        propFont->features=Font::attrName|Font::attrHeight|Font::attrSpacing|Font::attrCharSet;
        propFont->name=prefs->propFont;
        propFont->spacing=Font::spacingProportional;
        propFont->charSet=unicodeEncoding;
        propFont->pixelHeight=prefs->propFontSize;

        propFont=propFont->Load();

        if (!propFont.Valid()) {
          std::cerr << "Cannot load font '" << dynamic_cast<Font*>(propFont.Get())->fullName << "/"<< prefs->propFont << ", " << prefs->propFontSize << "'" << std::endl;
          // TODO: Free colors
          XCloseDisplay(display);
          return false;
        }

        fixedFont=::Lum::OS::Factory::factory->CreateFont();
        fixedFont->features=Font::attrName|Font::attrHeight|Font::attrSpacing|Font::attrCharSet;
        fixedFont->name=prefs->fixedFont;
        fixedFont->spacing=Font::spacingMonospace;
        fixedFont->charSet=unicodeEncoding;
        fixedFont->pixelHeight=prefs->fixedFontSize;

        fixedFont=fixedFont->Load();

        if (!fixedFont.Valid()) {
          std::cerr << "Cannot load font '" << dynamic_cast<Font*>(fixedFont.Get())->fullName << "/"<< prefs->fixedFont << ", " << prefs->fixedFontSize << "'" << std::endl;
          // TODO: Free colors
          XCloseDisplay(display);
          return false;
        }

        propFontSize=propFont->pixelHeight;
        fixedFontSize=fixedFont->pixelHeight;

        workAreaWidth=scrWidth;
        workAreaHeight=scrHeight;

        ::Lum::Prefs::InitializePrefs(prefs);
        ::Lum::Prefs::ReadOthersConfig();
        if (!prefs->theme.empty()) {
          ::Lum::Prefs::ReadOthersTheme();
        }
        ::Lum::Prefs::FreeData();

        return true;
      }

      void Display::Beep()
      {

//        XBell(display,100);
      }

      bool Display::RegisterSelection(::Lum::Base::DnDObjectPtr object,
                                      ::Lum::OS::WindowPtr window)
      {
/*
        if (selectObject!=object) {
          if (selectObject!=NULL) {
            selectObject->Deselect();
            selectObject=NULL;
          }
          XSetSelectionOwner(display,
                             XA_PRIMARY,
                             dynamic_cast<WindowPtr>(window)->window,
                             CurrentTime);
          selectObject=object;
        }
*/
        return true;
      }

      void Display::CancelSelection()
      {
/*
        assert(selectObject!=NULL);
        selClearPend=true;
        XSetSelectionOwner(display,XA_PRIMARY,None,CurrentTime);
        selectObject->Deselect();
        selectObject=NULL;
*/
      }

      ::Lum::Base::DnDObjectPtr Display::GetSelectionOwner() const
      {
//        return selectObject;
        return NULL;
      }

      bool Display::QuerySelection(::Lum::OS::WindowPtr window,
                                   ::Lum::Base::DnDObjectPtr object)
      {
/*
        XConvertSelection(display,
                          XA_PRIMARY,
                          XA_STRING,
                          atoms[selectionAtom],
                          dynamic_cast<WindowPtr>(window)->window,
                          CurrentTime);

        querySelectObject=object;
*/
        return false;
      }

      bool Display::SetClipboard(const std::wstring& content)
      {
        return false;
/*
        if (!content.empty()) {
          clipboard=content;
          XSetSelectionOwner(display,atoms[clipboardAtom],appWindow,CurrentTime);
          return true;
        }
        else {
          return false;
        }
*/
      }

      std::wstring Display::GetClipboard() const
      {
        return false;
/*
        if (!clipboard.empty()) {
          return clipboard;
        }

        XConvertSelection(display,
                          atoms[clipboardAtom],
                          XA_STRING,
                          atoms[clipBufferAtom],
                          appWindow,
                          CurrentTime);

        XFlush(display);

        XEvent                  event;
        ::Lum::Base::SystemTime timer,max;

        timer.SetTime(3,0);

        max.Add(timer);

        while (true) {
          fd_set                  set;
          ::Lum::Base::SystemTime now,tmp;
          timeval                 timeout;
          int                     ret,x11Fd;

          x11Fd=ConnectionNumber(display);

          FD_ZERO(&set);
          FD_SET(x11Fd,&set);

          if (now>=max) {
            return L"";
          }
          tmp=max;
          tmp.Sub(now);

          tmp.GetTimeval(timeout);

          ret=select(x11Fd+1,&set,NULL,NULL,&timeout);

          if (ret>0) {
            if (XCheckTypedWindowEvent(display,appWindow,SelectionNotify,&event)==True) {
              break;
            }
          }
        }

        Atom                 retType;
        int                  retFormat;
        unsigned long        itemsReturn,bytesLeft;
        unsigned char        *data;

        if (event.xselection.property==None) {
          return L"";
        }

        if (event.xselection.property!=atoms[clipBufferAtom]) {
          return L"";
        }

        if (XGetWindowProperty(display,
                               event.xselection.requestor,
                               event.xselection.property,
                               0,
                               65000,
                               True,
                               AnyPropertyType,
                               &retType,&retFormat,
                               &itemsReturn,&bytesLeft,
                               &data)!=Success) {
          std::cerr << "Cannot get property data" << std::endl;
          return L"";
        }

        if (retType==None) {
          std::cerr << "Illegal property data type" << std::endl;
          return L"";
        }

        if (retType!=XA_STRING || retFormat!=8) {
          std::cerr << "Unsupported selection datatype" << std::endl;
          return L"";
        }

        return ::Lum::Base::StringToWString((char*)data);
*/
      }

      void Display::ClearClipboard()
      {
/*
        if (!clipboard.empty()) {
          selClearPend=true;
          XSetSelectionOwner(display,atoms[clipboardAtom],None,CurrentTime);
          clipboard=L"";
        }
*/
      }

      void Display::PutBackEvent(::Lum::OS::EventPtr event,
                                   ::Lum::OS::WindowPtr destWin)
      {
/*
        XEvent x11Event;

        ::Lum::OS::X11::GetX11Event(event,x11Event);
        x11Event.xany.window=dynamic_cast<WindowPtr>(destWin)->window;
        if (XSendEvent(display,dynamic_cast<WindowPtr>(destWin)->window,
                       True,NoEventMask,&x11Event)==0) {
          std::cerr << "Cannot resend event!" << std::endl;
        }
*/
      }

      void Display::Exit()
      {
        assert(!exit);

        exit=true;
      }

      void Display::ReinitWindows()
      {
        std::list<WindowPtr>::iterator win;

        win=winList.begin();
        while (win!=winList.end()) {
          //(*win)->ReinitWindow();

          ++win;
        }
      }

      void Display::Close()
      {
        std::list<WindowPtr>::iterator win;

        ::Lum::OS::Base::Display::Close();

        Lum::Prefs::FreePrefs();

        if (selectObject!=NULL) {
          CancelSelection();
        }
        ClearClipboard();
        //d.selector.Close;

        win=winList.begin();
        while (win!=winList.end()) {
          std::cerr << "Warning: window '" << ::Lum::Base::WStringToString((*win)->GetTitle()) << "' not explicitely closed" << std::endl;
          (*win)->Close(); // removes itself from list

          win=winList.begin();
        }

        DisposeGWorld(fontScratch);

        delete ::Lum::Images::Factory::factory;

        CGColorSpaceRelease(colorSpace);
      }

      void Display::GetEvent()
      {
#if 0
        TrayPtr currentTray;

        XEvent e;

        XNextEvent(display,&e);

        if (XFilterEvent(&e,0)!=0) {
          return;
        }
      /*
        IF d.contextTimer.active THEN
          IF (e.type=X11.ButtonPress) OR (e.type=X11.ButtonRelease) OR (e.type=X11.MotionNotify) OR
            (e.type=X11.KeyPress) OR (e.type=X11.KeyRelease) THEN
            d.RestartContextHelp;
          END;
        END;
                              */

        switch (e.type) {
        case SelectionClear:
          if (!selClearPend) {
            if (e.xselectionclear.selection==XA_PRIMARY) {
              if (selectObject!=NULL) {
                selectObject->Deselect();
                selectObject=NULL;
              }
            }
            else if (e.xselectionclear.selection==atoms[clipboardAtom]) {
              clipboard=L"";
            }
          }
          else {
            selClearPend=false;
          }
          return;
        case SelectionNotify:
          HandleXSelectionNotify(e.xselection);
          return;
        case SelectionRequest:
          HandleXSelectionRequest(e.xselectionrequest);
          return;
        default:
          break;
        }

        currentWin=GetWindow(e.xany.window);
        currentTray=GetTray(e.xany.window);

        if (currentWin!=NULL) {
          ::Lum::OS::EventPtr event;

          event=::Lum::OS::X11::GetEvent(e,currentWin->xic);

          if (event!=NULL) {
            do {
              event->reUse=false;
              currentWin->HandleEvent(event);
            }
            while (event->reUse);
          }
        }
        else if (currentTray!=NULL) {
          ::Lum::OS::EventPtr event;

          event=::Lum::OS::X11::GetEvent(e,currentTray->xic);

          if (event!=NULL) {
            currentTray->HandleEvent(event);
          }
        }
#endif
      }

      /**
        Waits for certain events to happen:
        *A X11 event occurs
        *Timer run out

        Returns TRUE, if the wait exceeds the given timeout, else FALSE, if Wait
        returned because an X11 event is available.

        File descriptors getting available will be handled internaly. In this
        case a notification will be send and the wait will be restarted.
      */
      bool Display::Wait()
      {
#if 0
        while (true) {
          fd_set                  set;
          int                     ret;
          ::Lum::Base::SystemTime tmp;
          timeval                 timeout;
          int                     max;
          int                     x11Fd;

          x11Fd=ConnectionNumber(display);

          FD_ZERO(&set);
          FD_SET(x11Fd,&set);
          max=x11Fd;
          GetNextTimer(tmp);
          tmp.GetTimeval(timeout);

          ret=select(max+1,&set,NULL,NULL,&timeout);

          if (ret==-1) {
            std::cerr << "Error while selecting..." << std::endl;
          }
          else if (ret>=0) {
            return true;
          }
        }

#endif
        return true;
      }

      void Display::EventLoop()
      {
        assert(exit);

        exit=false;
#if 0
        while (true) {
            /*IF d.sleepList#NIL THEN
              IF X11.XEventsQueued(d.display,X11.QueuedAlready)=0 THEN
                d.CheckSleeps;
                d.CheckTimers;
              ELSE
                d.GetEvent;
              END;
            ELSE*/
          if (XEventsQueued(display,QueuedAlready)==0) {
            if (XEventsQueued(display,QueuedAfterFlush)==0) {
              CheckTimers();
              if (Wait()) {
                CheckTimers();
              }
            }
            else {
              CheckTimers();
            }
          }
          else {
            CheckTimers();
          }
          /*END;*/

          if (XEventsQueued(display,QueuedAfterReading)>0) {
            GetEvent();
            CheckTimers();
          }

          if (exit) {
            break;
          }
        }
#endif
      }

      bool Display::GetMousePos(int& x, int& y) const
      {
        return 0;
#if 0
        ::Window     root;
        ::Window     child;
        unsigned int bmask;
        int          wx,wy;

        return XQueryPointer(display,
                             XRootWindow(display,scrNum),&root,&child,
                             &x,&y,&wx,&wy,&bmask)!=False;
#endif
      }

      WindowPtr Display::GetWindow(::WindowPtr window) const
      {
        std::list<WindowPtr>::const_iterator iter;

        iter=winList.begin();
        while (iter!=winList.end()) {
          if ((*iter)->window==window) {
            return *iter;
          }

          ++iter;
        }
        return NULL;
      }

    }
  }
}
