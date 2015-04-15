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

#include <Lum/OS/X11/Display.h>

#include <math.h>

#include <cassert>

#if defined(HAVE_UNISTD_H)
  #include <unistd.h>
#endif

#include <cstdlib>
#include <cstring>
#include <stdint.h>

#if defined(HAVE_PWD_H)
  #include <pwd.h>
#endif

// for select TODO: Write a configure test for it
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>

#include <iostream>
#include <limits>

#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h> // TODO: Add configure check

#if defined(HAVE_LIB_XRANDR)
  #include <X11/extensions/Xrandr.h>
#endif

#if defined(HAVE_LIB_GCONF)
  #include <gconf/gconf-client.h>
#endif

#if defined(HAVE_LIB_GTK)
  #include <gtk/gtk.h>
  #include <gdk/gdkx.h>
  #include <gdk-pixbuf-xlib/gdk-pixbuf-xlib.h>
#endif

#if defined(HAVE_LIB_DBUS) && !defined(HAVE_LIB_OSSO)
  #include <dbus/dbus-glib-lowlevel.h>
#endif

#if defined(HAVE_LIB_GSTREAMER)
  #include <gst/gst.h>
#endif

#if defined(LUM_HAVE_LIB_HILDON)
  #include <hildon/hildon-main.h>
#endif

#include <Lum/Base/String.h>

#include <Lum/Images/Image.h>

#include <Lum/OS/Base/Theme.h>

#include <Lum/OS/EventLoopUnix.h>
#if defined(HAVE_LIB_GTK)
  #include <Lum/OS/EventLoopGMainLoop.h>
#endif

#include <Lum/OS/X11/Tray.h>
#include <Lum/OS/X11/Window.h>

#if defined(HAVE_LIB_GTK)
  #include <Lum/OS/Gtk/GtkTheme.h>
#endif

#if defined(CAIRO_HAS_XLIB_SURFACE)
  #include <Lum/OS/Cairo/Driver.h>
#endif

namespace Lum {
  namespace OS {
    namespace X11 {

      static const char* atomNames[Display::atomCount] = {
        "ATOM",
        "WM_DELETE_WINDOW",
        "WM_STATE",
        "XdndAware",
        "XdndEnter",
        "XdndLeave",
        "XdndPosition",
        "XdndStatus",
        "XdndFinished",
        "XdndDrop",
        "XdndActionCopy",
        "XdndActionMove",
        "XdndActionLink",
        "XdndActionAsk",
        "XdndActionPrivate",
        "XdndActionList",
        "XdndSelection",
        "XdndTypeList",
        "_ILLUMINATION_DROP_DATA",
        "_ILLUMINATION_SELECTION_DATA",
        "COMPOUND_TEXT",
        "CLIPBOARD",
        "_ILLUMINATION_CLIPBOARD_DATA",
        "_NET_WM_NAME",
        "_NET_WM_WINDOW_TYPE",
        "_NET_WM_WINDOW_TYPE_DESKTOP",
        "_NET_WM_WINDOW_TYPE_TOOLBAR",
        "_NET_WM_WINDOW_TYPE_MENU",
        "_NET_WM_WINDOW_TYPE_POPUP_MENU",
        "_NET_WM_WINDOW_TYPE_DROPDOWN_MENU",
        "_NET_WM_WINDOW_TYPE_UTILITY",
        "_NET_WM_WINDOW_TYPE_SPLASH",
        "_NET_WM_WINDOW_TYPE_DIALOG",
        "_NET_WM_WINDOW_TYPE_NORMAL",
        "_NET_WM_PING",
        "_NET_WM_PID",
        "_NET_WM_STATE",
        "_NET_WM_STATE_FULLSCREEN",
        "_NET_WM_CONTEXT_CUSTOM",
        "_NET_SYSTEM_TRAY_OPCODE",
        "_NET_SYSTEM_TRAY_MESSAGE_DATA",
        "_NET_WORKAREA",
        "_NET_FRAME_EXTENTS",
        "_NET_WM_WINDOW_OPACITY",
        "UTF8_STRING",
        "_XEMBED",
        "_XEMBED_INFO",
        "CARDINAL",
        "WINDOW",
        "TARGETS"
      };

      static const int sleepCursor=150;
      static const int popCursor=94;
      static const int dndCursor=50;

#if defined(HAVE_LIB_DBUS) || defined(HAVE_LIB_OSSO)
      static const char* dbusDomain="org.teulings.illumination";
#endif

      const char* unicodeEncoding="iso10646-1";

      class LUMAPI X11EventSource : public EventSource
      {
      private:
        Display *display;

      public:
        X11EventSource(Display *display)
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
          fileIO.file=ConnectionNumber(display->display);
          fileIO.conditions=ioRead;

          return true;
        }

        bool DataAvailable() const
        {
          return XPending(display->display)>0;
        }

        bool DataAvailable(Lum::Base::SystemTime& time) const
        {
          return false;
        }

        bool DataAvailable(const FileIO& fileIO) const
        {
          return fileIO.file==ConnectionNumber(display->display) && (fileIO.conditions & ioRead);
        }

        void Trigger()
        {
          while (XPending(display->display)>0) {
            display->GetEvent();
          }
        }

        bool IsFinished() const
        {
          return false;
        }
      };

        // Find out dimensions of workSpace, this is size of desktop minus globals toolsbars etc...
      void Display::GetWorkAreaDimension()
      {
        unsigned char* atomProperties=NULL;
        Atom           atomType;
        int            atomFormat;
        unsigned long  atomItems;
        unsigned long  atomBytes;
        int            res;

        res=XGetWindowProperty(display,RootWindow(display,scrNum),
                               atoms[netWorkAreaAtom],2,2,false,atoms[cardinalAtom],
                               &atomType,&atomFormat,&atomItems,&atomBytes,&atomProperties);

        if (res==Success &&
            atomProperties!=NULL) {
          workAreaWidth=reinterpret_cast<uint32_t*>(atomProperties)[0];
          workAreaHeight=reinterpret_cast<uint32_t*>(atomProperties)[1];
        }
        else {
          workAreaWidth=0;
          workAreaHeight=0;
        }

        XFree(atomProperties);
      }

      double Display::GetDPI() const
      {
        return dpi;
      }

      void Display::SetAtomProperty(Window* window, Atom property, Atom value)
      {
        XChangeProperty(display,window->window,property,atoms[atomAtom],32,
                        PropModeReplace,(unsigned char*)&value,1);
      }

      void Display::SetCardinalProperty(Window* window, Atom property, int value)
      {
        XChangeProperty(display,window->window,property,atoms[cardinalAtom],32,
                        PropModeReplace,(unsigned char*)&value,1);
      }

      void Display::SetIntegerProperty(Window* window, Atom property, int value)
      {
        XChangeProperty(display,window->window,property,XA_INTEGER,32,
                        PropModeReplace,(unsigned char*)&value,1);
      }

      void Display::SetEmbedInfoProperty(::Window window)
      {
        uint32_t data[2];

        data[0]=0;
        data[1]=1;

        ::XChangeProperty(display,
                          window,
                          atoms[xEmbedInfoAtom],
                          atoms[xEmbedInfoAtom],32,
                          PropModeReplace,
                        (unsigned char*)data,2);
      }

      void Display::SendNetSystemTrayRequestDock(::Window window)
      {
        ::Window selWin;

        selWin=::XGetSelectionOwner(display,XInternAtom(display,"_NET_SYSTEM_TRAY_S0",False));

        if (selWin!=0) {
          ::XEvent trayEvt;

          memset(&trayEvt,0,sizeof(trayEvt));
          trayEvt.xclient.type=ClientMessage;
          trayEvt.xclient.window=selWin;
          trayEvt.xclient.message_type=atoms[netSystemTrayOpcodeAtom];
          trayEvt.xclient.format=32;
          trayEvt.xclient.data.l[0]=CurrentTime;
          trayEvt.xclient.data.l[1]=0;
          trayEvt.xclient.data.l[2]=window;
          trayEvt.xclient.data.l[3]=0;
          trayEvt.xclient.data.l[4]=0;
          if (XSendEvent(display,selWin,False,NoEventMask,&trayEvt)==0) {
            std::cerr << "Error while sending SystemtrayRequestDock!" << std::endl;
          }
        }
      }

#if 0

/**
  Returns the window currently under the mouse pointer. It also returns the
  screen relative and window relative mouse coordinates.
*/
              Window Display::GetWindowOnScreen(int rX, int rY, int cX, int cY)
              {
                int help;
                int rW;
                int cW;
                Window dragWin;
                unsigned long mask;
                Atom retType;
                int retFormat;
                int itemsReturn;
                int bytesLeft;
                ::C::string data;

                dragWin=0;
                data=NULL;
                itemsReturn=0;
                if (X11.XQueryPointer(d.display,X11.XRootWindow(d.display,0),rW,cW,rX,rY,cX,cY,mask)==X11.True) {
                  while ((rW!=0) && (dragWin==0) && (cW!=0) && (rW!=cW)) {
                    if ((X11.XGetWindowProperty(d.display,cW,d.atoms[wmStateAtom],0,MAX(LONGINT),X11.False,X11.AnyPropertyType,retType,retFormat,itemsReturn,bytesLeft,data)==X11.Success) && (retType!=X11.None)) {
                      dragWin=cW;
                    }
                    if (rW!=0) {
                      help=cW;
                      if (X11.XTranslateCoordinates(d.display,rW,cW,cX,cY,cX,cY,cW)==X11.True) {
                        rW=help;
                      }
                      else {
                        Err.String("Cannot convert coordinates!");
                        Err.Ln;
                        rW=0;
                      }
                    }
                  }
                }
                return dragWin;
              }

#endif

      /**
        Return the Illumination tray belonging to the given X11 window.
      */
      Tray* Display::GetTray(::Window window)
      {
        std::list<Tray*>::const_iterator iter;

        iter=trayList.begin();
        while (iter!=trayList.end()) {
          if ((*iter)->window==window) {
            return *iter;
          }

          ++iter;
        }
        return NULL;
      }

      FontRef Display::GetFontInternal(FontType type, size_t size) const
      {
        FontRef font;

        font=driver->CreateFont();
        font->features=Font::attrName|Font::attrHeight|Font::attrSpacing|Font::attrCharSet;

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
        font->charSet=unicodeEncoding;

        font=font->Load();

        if (font.Valid()) {
          return font.Get();
        }
        else {
          return NULL;
        }
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
      void Display::AddWindow(Window* w)
      {
        winList.push_back(w);
      }

      /**
        Removes the window from the internal list of windows.
      */
      void Display::RemoveWindow(Window* w)
      {
        winList.remove(w);
      }

      /**
        Get the Illumination window matching the given X11 window.
      */
      Window* Display::GetWindow(::Window window)
      {
        for (std::list<Window*>::const_iterator w=winList.begin();
             w!=winList.end();
             ++w) {
          if ((*w)->window==window) {
            return *w;
          }
        }

        return NULL;
      }

      /**
        Adds the tray to the internal list of trays.
      */
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

#if 0
              ::Lum::OS::FontList Display::GetFontList()
              {
                int count;
                int x;
                int y;
                int size;
                ::C::charPtr2d names;
                ::Lum::OS::FontName name;
                ::Lum::OS::FontName field;
                ::Lum::OS::FontName field2;
                ConvResults res;
                ::Lum::OS::FontList list;
                ::Lum::OS::FontFoundry foundry;
                ::Lum::OS::FontFamily family;
                ::Lum::OS::FontEncoding encoding;
                ::Lum::OS::FontSize fSize;
                FcConfigPtr config;
                FcPatternPtr pattern;
                FcPatternPtr pattern2;
                FcFontSetPtr set;
                FcFontSetPtr set2;
                FcObjectSetPtr oSet;
                FcObjectSetPtr oSet2;
                XftChar8Ptr buffer;
                XftChar8Ptr buffer2;
                int z;
                ::Lum::OS::FontName fName;
                ::Lum::OS::FontName fName;
                XftFontSetPtr set;
                XftFontSetPtr set2;
                int z;
                int t;
                ::C::string tmp;


                list=new FontList;
                list.families=NULL;
                names=X11.XListFonts(d.display,"-*-*-medium-r-*--*-*-*-*-*-*-*-*",MAX(INTEGER),count);
                if ((names!=NULL) && (count>0)) {
                  for (/*TODO*/x=0TOcount-1) {
                    y=0;
                    while (names[x][y]!=0x0) {
                      name[y]=names[x][y];
                      y++;
                    }
                    name[y]=0x0;
                    GetFontElement(name,2,field);
                    y=0;
                    while (field[y]!=0x0) {
                      if ((y==0) || (field[y-1]==" ")) {
                        field[y]=CAP(field[y]);
                      }
                      y++;
                    }
                    family=list.GetOrCreateFamily(field);
                    GetFontElement(name,1,field);
                    if (field=="") {
                      field="X11";
                    }
                    foundry=family.GetOrCreateFoundry(field);
                    GetFontElement(name,13,field);
                    GetFontElement(name,14,field2);
                    str.Append("-",field);
                    str.Append(field2,field);
                    encoding=foundry.GetOrCreateEncoding(field);
                    GetFontElement(name,7,field);
                    if (field!="*") {
                      co.StrToInt(field,size,res);
                      if (size!=0) {
                        if (~encoding.anySize) {
                          fSize=encoding.GetOrCreateSize(size);
                        }
                      }
                      else {
                        encoding.anySize=true;
                        encoding.sizes=NULL;
                      }
                    }
                  }
                }
                X11.XFreeFontNames(names); IF HAVE_LIB_FONTCONFIG=TRUE THEN config=fc.FcInitLoadConfigAndFonts();
                oSet=fc.FcObjectSetCreate();
                fc.FcObjectSetAdd(oSet,fc.FC_FOUNDRY);
                oSet2=fc.FcObjectSetCreate();
                fc.FcObjectSetAdd(oSet2,fc.FC_FAMILY);
                pattern=fc.FcPatternCreate();
                set=fc.FcFontList(config,pattern,oSet);
                for (/*TODO*/x=0TOset.nfont-1) {
                  if (fc.FcPatternGetString(set.fonts[x],fc.FC_FOUNDRY,0,buffer)==0) {
                    z=0;
                    while (buffer[z]!=0x0) {
                      fName[z]=buffer[z];
                      z++;
                    }
                    fName[z]=0x0;
                    pattern2=fc.FcPatternCreate();
                    if (fc.FcPatternAddString(pattern2,fc.FC_FOUNDRY,buffer^)) {
                    }
                    set2=fc.FcFontList(config,pattern2,oSet2);
                    fc.FcPatternDestroy(pattern2);
                  }
                  else {
                    fName="Xft";
                    set2=fc.FcFontList(config,pattern,oSet2);
                  }
                  for (/*TODO*/y=0TOset2.nfont-1) {
                    if (fc.FcPatternGetString(set2.fonts[y],fc.FC_FAMILY,0,buffer2)==0) {
                      z=0;
                      while (buffer2[z]!=0x0) {
                        if ((z==0) || (buffer2[z-1]==" ")) {
                          name[z]=CAP(buffer2[z]);
                        }
                        else {
                          name[z]=buffer2[z];
                        }
                        z++;
                      }
                      name[z]=0x0;
                      family=list.GetOrCreateFamily(name);
                      foundry=family.GetOrCreateFoundry(fName);
                      encoding=foundry.GetOrCreateEncoding(unicodeEncoding);
                    }
                  }
                  fc.FcFontSetDestroy(set2);
                }
                fc.FcFontSetDestroy(set); ELSIF HAVE_LIB_XFT=TRUE THEN set=Xft.XftListFonts(D.display(Display).display,D.display(Display).scrNum,NULL,Xft.XFT_FOUNDRY,NULL);
                if (set!=NULL) {
                  for (/*TODO*/x=0TOset.nfont-1) {
                    if (Xft.XftPatternGetString(set.fonts[x],Xft.XFT_FOUNDRY,0,tmp)==0) {
                      z=0;
                      while (tmp[z]!=0x0) {
                        fName[z]=tmp[z];
                        z++;
                      }
                      fName[z]=0x0;
                      set2=Xft.XftListFonts(D.display(Display).display,D.display(Display).scrNum,Xft.XFT_FOUNDRY,Xft.XftTypeString,s.ADR(fName[0]),NULL,Xft.XFT_FAMILY,Xft.XFT_ENCODING,NULL);
                    }
                    else {
                      fName="Xft";
                      set2=Xft.XftListFonts(D.display(Display).display,D.display(Display).scrNum,NULL,Xft.XFT_FAMILY,Xft.XFT_ENCODING,NULL);
                    }
                    for (/*TODO*/y=0TOset2.nfont-1) {
                      if (Xft.XftPatternGetString(set2.fonts[y],Xft.XFT_FAMILY,0,tmp)==0) {
                        z=0;
                        while (tmp[z]!=0x0) {
                          if ((z==0) || (name[z-1]==" ")) {
                            name[z]=CAP(tmp[z]);
                          }
                          else {
                            name[z]=tmp[z];
                          }
                          z++;
                        }
                        name[z]=0x0;
                        family=list.GetOrCreateFamily(name);
                        foundry=family.GetOrCreateFoundry(fName);
                        t=0;
                        while (Xft.XftPatternGetString(set2.fonts[y],Xft.XFT_ENCODING,t,tmp)==0) {
                          z=0;
                          while (tmp[z]!=0x0) {
                            name[z]=tmp[z];
                            z++;
                          }
                          name[z]=0x0;
                          encoding=foundry.GetOrCreateEncoding(name);
                          t++;
                        }
                      }
                    }
                    Xft.XftFontSetDestroy(set2);
                  }
                  Xft.XftFontSetDestroy(set);
                } END
                return list;
              }

#endif

      bool Display::AllocateColor(double red, double green, double blue,
                                  OS::Color& color)
      {
        XColor xcolor;

        xcolor.red=(unsigned short)(red*std::numeric_limits<unsigned short>::max());
        xcolor.green=(unsigned short)(green*std::numeric_limits<unsigned short>::max());
        xcolor.blue=(unsigned short)(blue*std::numeric_limits<unsigned short>::max());

        if (XAllocColor(display,colorMap,&xcolor)!=0) {
          color=xcolor.pixel;
          return true;
        }
        else {
          return false;
        }
      }

      bool Display::AllocateNamedColor(const std::string& name,
                                       OS::Color& color)
      {
        XColor exact;
        XColor xcolor;

        if (XLookupColor(display,colorMap,name.c_str(),&exact,&xcolor)!=0) {
          if (XAllocColor(display,colorMap,&xcolor)!=0) {
            color=xcolor.pixel;
            return true;
          }
        }

        return false;
      }

      void Display::FreeColor(OS::Color color)
      {
        XFreeColors(display,colorMap,&color,1,0);
      }

/*      ::Lum::OS::Bitmap Display::CreateBitmapPattern(char pattern[], int width, int height)
      {
        ::Lum::OS::Bitmap bitmap;

        bitmap=D.factory.CreateBitmap();
        bitmap(Bitmap).pixmap=X11.XCreateBitmapFromData(d.display,X11.XRootWindow(d.display,d.scrNum),pattern,width,height);
        if (bitmap(Bitmap).pixmap==0) {
          return NULL;
        }
        bitmap.draw=new DrawInfo();
        bitmap.width=width;
        bitmap.height=height;
        return bitmap;
      }*/

#if defined(HAVE_LIBSM)
  #if 0
      static void callback_die(SmcConn smc_conn, SmPointer client_data)
      {
        ((Display*)OS::display)->CallbackDie(smc_conn,client_data);
      }

      static void callback_save_yourself(SmcConn smc_conn, SmPointer client_data,
                                         int save_style, Bool shutdown,
                                         int interact_style,
                                         Bool fast)
      {
        ((Display*)OS::display)->CallbackSaveYourself(smc_conn,client_data,
                                                      save_style,shutdown,
                                                      interact_style,fast);
      }

      static void callback_shutdown_cancelled(SmcConn smc_conn, SmPointer client_data)
      {
        ((Display*)OS::display)->CallbackShutdownCancelled(smc_conn,client_data);
      }

      static void callback_save_complete(SmcConn smc_conn, SmPointer client_data)
      {
        ((Display*)OS::display)->CallbackSaveComplete(smc_conn,client_data);
      }

      static void ice_io_error_handler(IceConn connection)
      {
        ((Display*)OS::display)->CallbackICEIOError(connection);
      }
  #endif
#endif

#if defined(HAVE_LIB_OSSO)
      osso_context_t* Display::InitializeLibOsso()
      {
        osso_context_t* context;
        std::string     appName;
        std::string     serviceName;

        appName=Lum::Base::WStringToString(GetAppName());
        for (size_t i=0; i<appName.length(); ++i) {
          appName[i]=tolower(appName[i]);
        }

        serviceName=std::string(dbusDomain)+"."+appName;

        context=osso_initialize(serviceName.c_str(),"1.0",FALSE,
                                dynamic_cast<GMainLoopEventLoop*>(eventLoop)->context);

        if (context!=NULL) {
          dbusSession=(DBusConnection*)osso_get_dbus_connection(context);
          dbusSystem=(DBusConnection*)osso_get_sys_dbus_connection(context);
        }

        return context;
      }
#elif defined(HAVE_LIB_DBUS)
      /*
      static DBusHandlerResult DBusMsgHandler(DBusConnection *connection, DBusMessage *msg, void *data)
      {
        const char* interface;
        const char* type;
        bool        isMethod;

        if (dbus_message_get_type(msg)==DBUS_MESSAGE_TYPE_METHOD_CALL) {
          type="method";
          isMethod=true;
        }
        else if (dbus_message_get_type(msg) == DBUS_MESSAGE_TYPE_SIGNAL) {
          type="signal";
          isMethod=false;
        }
        else {
          return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
        }

        interface=dbus_message_get_interface(msg);
        if (interface==NULL) {
          return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
        }

        std::cout << "Got a " <<  type << " " <<  (isMethod? "to" : "from") << " interface '" << interface << "' '" << dbus_message_get_path(msg) << "' '" << dbus_message_get_member(msg) << "'" << std::endl;

        if (dbus_message_get_type(msg)==DBUS_MESSAGE_TYPE_METHOD_CALL) {
          DBusMessage *response;
          std::string appName;
          std::string error;

          appName=Lum::Base::WStringToString(display->GetAppName());
          Lum::Base::ToLower(appName);

          error=std::string(dbusDomain)+"."+appName+".notImplemented";

          response=dbus_message_new_error(msg,error.c_str(),"Message not implemented");
          dbus_connection_send(connection,response,NULL);
          dbus_message_unref(response);
          return DBUS_HANDLER_RESULT_HANDLED;
        }

        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
      }*/

      DBusConnection *Display::InitiateDBusConnection(DBusBusType busType)
      {
        DBusConnection *connection;
        DBusError      err;
        std::string    appName;
        std::string    serviceName;

        dbus_error_init(&err);

        connection=dbus_bus_get(busType,&err);
        if (connection==NULL) {
          std::cerr << "Cannot connect to DBUS session: " << err.message << std::endl;
          dbus_error_free(&err);
          return NULL;
        }

        dbus_connection_setup_with_g_main(connection,
                                          dynamic_cast<Lum::OS::GMainLoopEventLoop*>(eventLoop)->context);

        appName=Lum::Base::WStringToString(GetAppName());
        for (size_t i=0; i<appName.length(); ++i) {
          appName[i]=tolower(appName[i]);
        }

          serviceName=std::string(dbusDomain)+"."+appName;

          if (dbus_bus_request_name(connection,
                                    serviceName.c_str(),
                                    DBUS_NAME_FLAG_ALLOW_REPLACEMENT,&err)==-1) {
            //std::cerr << "Cannot request name '" << serviceName << "': "<< err.message << std::endl;
            dbus_error_free(&err);
          }

        //dbus_connection_add_filter(connection,DBusMsgHandler,NULL,NULL);

        dbus_connection_set_exit_on_disconnect(connection,false);

        return connection;
      }
#endif

      Display::Display()
      : currentWin(NULL),
        selectObject(NULL),
        querySelectObject(NULL),
        queryClipboardObject(NULL),
        selClearPend(false),
        contextHelp(true),
        dropWindow(NULL),
        appWindow(0),
        scrNum(0),
        display(NULL),
        visual(NULL),
        xim(0)
#if defined(HAVE_LIB_OSSO)
        ,ossoContext(NULL)
#endif

#if defined(HAVE_LIB_DBUS) || defined(HAVE_LIB_OSSO)
        ,dbusSession(NULL),
        dbusSystem(NULL)
#endif
      {
        // no code
      }

      Display::~Display()
      {
        // no code
      }

      static void XIMInstantiateCallback(::Display *display,
                                         XPointer clientData,
                                         XPointer callData)
      {
        std::cout << "XIM found!" << std::endl;

        ((Display*)clientData)->xim=XOpenIM(display,NULL,NULL,NULL);
      }

      bool Display::Open()
      {
#if !defined(HAVE_LIB_GTK)
        const char*    result;
#endif
        OS::FontRef    font;
        /*
        if (XInitThreads()==0) {
          std::cerr << "Warning: X11 does not support multiple threads!" << std::endl;
        }*/

#if defined(HAVE_LIB_GTK)
        if (!gtk_init_check(NULL,NULL)) {
          std::cerr << "Cannot initialize GTK!" << std::endl;
          return false;
        }

        if (!g_thread_supported ()) {
          g_thread_init (NULL);
        }

        gdk_threads_init();

#if defined(LUM_HAVE_LIB_HILDON)
        hildon_init();
#endif
	
        display=GDK_DISPLAY_XDISPLAY(gdk_display_get_default());
        scrNum=GDK_SCREEN_XNUMBER(gdk_screen_get_default());
        visual=GDK_VISUAL_XVISUAL(gdk_visual_get_system());

        colorDepth=gdk_visual_get_system()->depth;
        colorMap=GDK_COLORMAP_XCOLORMAP(gdk_colormap_get_system());

        gdk_pixbuf_xlib_init(display,scrNum);
#else
        result=XDisplayName(NULL);
        if (result!=NULL) {
          displayName=result;
        }
        display=XOpenDisplay(displayName.c_str());
        if (display==NULL) {
          std::cerr << "Cannot open X11 display!" << std::endl;
          return false;
        }
        scrNum=XDefaultScreen(display);
        visual=XDefaultVisual(display,scrNum);

        if (visual==NULL) {
          std::cerr << "Cannot get visual!" << std::endl;
          XCloseDisplay(display);
          return false;
        }

        colorDepth=XDefaultDepth(display,scrNum);
        colorMap=XDefaultColormap(display,scrNum);
#endif

#if defined(HAVE_LIB_XRANDR)
        xrandrActive=false;
        if (XRRQueryVersion(display,&xrandrMajor,&xrandrMinor)!=0) {
          if (XRRQueryExtension(display,&xrandrEventBase,&xrandrErrorBase)) {
            xrandrActive=true;
            XRRSelectInput(display,
                           XRootWindow(display,scrNum),
                           RRScreenChangeNotifyMask);		
          }
        }
#endif

#if defined(HAVE_LIB_GTK)
        eventLoop=new GMainLoopEventLoop();
  #if defined(HAVE_LIB_GSTREAMER)
        gst_init(NULL,NULL);
  #endif
#else
        eventLoop=new UnixEventLoop();
#endif
        eventLoop->AddSource(new X11EventSource(this));


        Images::Factory::Set(driver->CreateImageFactory(this));

        screenWidth=XDisplayWidth(display,scrNum);
        screenHeight=XDisplayHeight(display,scrNum);

        type=typeGraphical;

        EvaluateDisplaySize();

        dpi=round((XDisplayHeight(display,scrNum)*25.4)/XDisplayHeightMM(display,scrNum));
        /*
        int            eventBase,errorBase;

        // Checking for shape extension
        if (XShapeQueryExtension(display,&eventBase,&errorBase)) {
          // TODO
        }
        */

        if (XSupportsLocale()) {
          if (XSetLocaleModifiers("")==NULL) {
            std::cerr << "Cannot set locale modifiers!" << std::endl;
          }

          xim=XOpenIM(display,NULL,NULL,NULL);

          if (xim==0) {
            XRegisterIMInstantiateCallback(display,
                                           NULL,NULL,NULL,
                                           XIMInstantiateCallback,
                                           (XPointer)display);
          }
        }

        if ((visual->c_class==GrayScale) || (visual->c_class==StaticGray)) {
          if (colorDepth==1) {
            colorMode=colorModeMonochrome;
          }
          else {
            colorMode=colorModeGreyScale;
          }
        }
        else if ((visual->c_class==PseudoColor) ||
                 (visual->c_class==StaticColor) ||
                 (visual->c_class==DirectColor) ||
                 (visual->c_class==TrueColor)) {
          colorMode=colorModeColor;
        }
        else {
          std::cerr << "Unsupported visual class!" << std::endl;
          return false;
        }


        XSetWindowAttributes attr;

        attr.colormap=colorMap;
        attr.bit_gravity=NorthWestGravity;
        attr.event_mask=KeyPressMask | KeyReleaseMask |  ExposureMask |
                        StructureNotifyMask | ButtonPressMask |  ButtonReleaseMask |
                        ButtonMotionMask | PointerMotionMask | FocusChangeMask;

        attr.override_redirect=False;
        attr.save_under=False;
        attr.backing_store=False;
        appWindow=XCreateWindow(display,
                                XRootWindow(display,scrNum),
                                0,0,
                                10,10,
                                CopyFromParent,
                                CopyFromParent,
                                InputOutput,
                                visual,
                                CWBorderPixel
                                | CWBitGravity
                                | CWBackingStore
                                | CWOverrideRedirect
                                | CWSaveUnder
                                | CWEventMask
                                | CWColormap,
                                &attr);

        if (appWindow==0) {
          std::cerr << "Cannot create hidden application window!" << std::endl;
          return false;
        }

        XInternAtoms(display,const_cast<char**>(atomNames),atomCount,false,atoms);
        /*
        for (size_t i=0; i<atomCount; i++) {
          atoms[i]=XInternAtom(display,atomNames[i],False);
        }*/

                      /*
        D.smallChess:=d.CreateBitmapPattern(D.disablePattern,D.disableWidth,D.disableHeight);
        D.bigChess:=d.CreateBitmapPattern(D.bigChessPattern,D.bigChessWidth,D.bigChessHeight);

                      */
#if defined(HAVE_LIB_GTK)
        if (((driver->GetDriverName()==L"X11" ||
             driver->GetDriverName()==L"CairoX11")) &&
             (getenv("ILLUMINATION_THEME")==NULL ||
              strcmp(getenv("ILLUMINATION_THEME"),"X11"))!=0) {
          theme=new Gtk::GtkTheme(this);
        }
        else {
          theme=new OS::Base::DefaultTheme(this);
        }
#else
        theme=new OS::Base::DefaultTheme(this);
#endif

        font=driver->CreateFont();
        font->features=Font::attrName|Font::attrHeight|Font::attrSpacing|Font::attrCharSet;
        font->name=Lum::Base::WStringToString(theme->GetProportionalFontName());
        font->spacing=Font::spacingProportional;
        font->charSet=unicodeEncoding;
        font->pixelHeight=theme->GetProportionalFontSize();

        propFont=font->Load();

        if (!propFont.Valid()) {
          std::cerr << "Cannot load font '" << font->name << ", " << font->pixelHeight << "!" << std::endl;
#if !defined(HAVE_LIB_GTK)
          XCloseDisplay(display);
#endif
          return false;
        }

        font=driver->CreateFont();
        font->features=Font::attrName|Font::attrHeight|Font::attrSpacing|Font::attrCharSet;
        font->name=Lum::Base::WStringToString(theme->GetFixedFontName());
        font->spacing=Font::spacingMonospace;
        font->charSet=unicodeEncoding;
        font->pixelHeight=theme->GetFixedFontSize();

        fixedFont=font->Load();

        if (!fixedFont.Valid()) {
          std::cerr << "Cannot load font '" << font->name << ", " << font->pixelHeight << "!" << std::endl;
#if !defined(HAVE_LIB_GTK)
          XCloseDisplay(display);
#endif
          return false;
        }

        propFontSize=propFont->pixelHeight;
        fixedFontSize=fixedFont->pixelHeight;

        for (size_t i=0; i<colorCount; i++) {
          color[i]=theme->GetColor((ColorIndex)i);
        }

      /*
        d.sleepCursor:=X11.XCreateFontCursor(d.display,sleepCursor);
        IF d.sleepCursor=0 THEN
          Err.String("Cannot get sleep cursor!"); Err.Ln;
          X11.XCloseDisplay(d.display);
          RETURN FALSE;
        END;

        d.popCursor:=X11.XCreateFontCursor(d.display,popCursor);
        IF d.popCursor=0 THEN
          Err.String("Cannot get popup cursor!"); Err.Ln;
          X11.XFreeCursor(d.display,d.sleepCursor);
          X11.XCloseDisplay(d.display);
          RETURN FALSE;
        END;

        d.dndCursor:=X11.XCreateFontCursor(d.display,dndCursor);
        IF d.dndCursor=0 THEN
          Err.String("Cannot get drag & drop cursor!"); Err.Ln;
          X11.XFreeCursor(d.display,d.sleepCursor);
          X11.XFreeCursor(d.display,d.popCursor);
          X11.XCloseDisplay(d.display);
          RETURN FALSE;
        END;

        d.copyCursor:=CreatePixmapCursor(s.ADR(copyCursorData),s.ADR(copyMaskData),14,15);
        IF d.copyCursor=0 THEN
          Err.String("Cannot create copy cursor"); Err.Ln;
          X11.XFreeCursor(d.display,d.sleepCursor);
          X11.XFreeCursor(d.display,d.popCursor);
          X11.XFreeCursor(d.display,d.dndCursor);
          X11.XCloseDisplay(d.display);
          RETURN FALSE;
        END;

        d.moveCursor:=CreatePixmapCursor(s.ADR(moveCursorData),s.ADR(moveMaskData),11,13);
        IF d.moveCursor=0 THEN
          Err.String("Cannot create move cursor"); Err.Ln;
          X11.XFreeCursor(d.display,d.sleepCursor);
          X11.XFreeCursor(d.display,d.popCursor);
          X11.XFreeCursor(d.display,d.dndCursor);
          X11.XFreeCursor(d.display,d.copyCursor);
          X11.XCloseDisplay(d.display);
          RETURN FALSE;
        END;

        d.linkCursor:=CreatePixmapCursor(s.ADR(linkCursorData),s.ADR(linkMaskData),15,17);
        IF d.linkCursor=0 THEN
          Err.String("Cannot create move cursor"); Err.Ln;
          X11.XFreeCursor(d.display,d.sleepCursor);
          X11.XFreeCursor(d.display,d.popCursor);
          X11.XFreeCursor(d.display,d.dndCursor);
          X11.XFreeCursor(d.display,d.copyCursor);
          X11.XFreeCursor(d.display,d.moveCursor);
          X11.XCloseDisplay(d.display);
          RETURN FALSE;
        END;

      */

        GetWorkAreaDimension();
        /*
        d.contextTimer.SetSecs(D.prefs.contextTimer DIV 1000,D.prefs.contextTimer MOD 1000);
                      d.contextTimer.SetObject(d);
        */

                      /*
        d.sleepList:=NIL;
        d.channelList:=NIL;

        d.dragging:=FALSE;
        d.dragObject:=NIL;
       */

        multiClickTime=200;

#if 0
        if (getenv("SESSION_MANAGER")!=NULL) {
          SmcCallbacks callbacks;
          char*        clientId;
          char         errorString[4096];

          IceSetIOErrorHandler(ice_io_error_handler);

          callbacks.save_yourself.callback=callback_save_yourself;
          callbacks.die.callback=callback_die;
          callbacks.save_complete.callback=callback_save_complete;
          callbacks.shutdown_cancelled.callback=callback_shutdown_cancelled;

          callbacks.save_yourself.client_data=(SmPointer)NULL;
          callbacks.die.client_data=(SmPointer)NULL;
          callbacks.save_complete.client_data=(SmPointer)NULL;
          callbacks.shutdown_cancelled.client_data=(SmPointer)NULL;
          smSession=SmcOpenConnection(NULL,NULL,
                                      SmProtoMajor, SmProtoMinor,
                                      SmcSaveYourselfProcMask | SmcDieProcMask |
                                      SmcSaveCompleteProcMask |
                                      SmcShutdownCancelledProcMask,
                                      &callbacks,NULL,&clientId,
                                      4096,errorString);

          if (smSession) {
            iceSession=SmcGetIceConnection(smSession);
            iceFd=IceConnectionNumber(iceSession);
          }
          else {
            std::cerr << "Error while connecting to session manager: " << errorString << "!" << std::endl;
          }
        }
#endif

#if defined(HAVE_LIB_OSSO)
        ossoContext=InitializeLibOsso();
        if (ossoContext==NULL) {
          std::cerr << "Cannot create OSSO context!" << std::endl;
        }
#elif defined(HAVE_LIB_DBUS)
        dbus_g_thread_init ();

        dbusSession=InitiateDBusConnection(DBUS_BUS_SESSION);
        if (dbusSession==NULL) {
          std::cerr << "Cannot create session DBUS connection!" << std::endl;
        }
        dbusSystem=InitiateDBusConnection(DBUS_BUS_SYSTEM);
        if (dbusSystem==NULL) {
          std::cerr << "Cannot create system DBUS connection!" << std::endl;
        }
#endif
        return true;
      }

      void Display::Close()
      {
        std::list<Window*>::iterator win;

        OS::Base::Display::Close();

#if defined(HAVE_LIB_OSSO)
        if (ossoContext!=NULL) {
          osso_deinitialize(ossoContext);
        }
#elif defined(HAVE_LIB_DBUS)
        if (dbusSystem!=NULL) {
          dbus_connection_unref(dbusSystem);
        }
        if (dbusSession!=NULL) {
          dbus_connection_unref(dbusSession);
        }
#endif


#if 0
        if (smSession!=NULL) {
          SmcCloseConnection(smSession,0,NULL);
        }
#endif

        if (selectObject!=NULL) {
          CancelSelection();
        }
        ClearClipboard();

        win=winList.begin();
        while (win!=winList.end()) {
          std::cerr << "Warning: window " << *win << " not explicitely closed!" << std::endl;
          std::cerr << "  Window title: '" <<Lum::Base::WStringToString((*win)->GetTitle()) << "'" << std::endl;
          (*win)->Close(); // removes itself from list

          win=winList.begin();
        }

        /*
        if (d.sleepCursor!=0) {
          X11.XFreeCursor(d.display,d.sleepCursor);
        }
        */

        XDestroyWindow(display,appWindow);

        delete Images::Factory::factory;

        if (xim!=NULL) {
          if (XCloseIM(xim)!=0) {
          }
        }

#if defined(LUM_HAVE_LIB_CAIRO) && defined(CAIRO_HAS_XLIB_SURFACE)
        Cairo::FreeGlobalCairo();
        Cairo::FreeGlobalSurface();
#endif

        delete eventLoop;

#if !defined(HAVE_LIB_GTK)
        if (display!=NULL) {
          XCloseDisplay(display);
        }
#endif

#if defined(LUM_HAVE_LIB_CAIRO) && defined(CAIRO_HAS_XLIB_SURFACE)
        //cairo_debug_reset_static_data();
#endif
      }

       /**
        Flushes all pending events. makes only sense for asnsycronous
        windowing systems like X11.
       */
       void Display::Flush()
       {
         XSync(display,False);
       }

       void Display::Beep()
       {
         XBell(display,100);
       }


      bool Display::RegisterSelection(Lum::Base::DataExchangeObject* object,
                                      OS::Window* window)
      {
        if (selectObject!=object) {
          if (selectObject!=NULL) {
            selectObject->Deselect();
            selectObject=NULL;
          }
          XSetSelectionOwner(display,
                             XA_PRIMARY,
                             dynamic_cast<Window*>(window)->window,
                             CurrentTime);
          selectObject=object;
        }

        return true;
      }

      void Display::CancelSelection()
      {
        assert(selectObject!=NULL);
        selClearPend=true;
        XSetSelectionOwner(display,XA_PRIMARY,None,CurrentTime);
        selectObject->Deselect();
        selectObject=NULL;
      }

      Lum::Base::DataExchangeObject* Display::GetSelectionOwner() const
      {
        return selectObject;
      }

      bool Display::QuerySelection(OS::Window* window,
                                   Lum::Base::DataExchangeObject* object)
      {
        XConvertSelection(display,
                          XA_PRIMARY,
                          XA_STRING,
                          atoms[selectionAtom],
                          dynamic_cast<Window*>(window)->window,
                          CurrentTime);

        querySelectObject=object;

        return false;
      }

      bool Display::SetClipboard(const std::wstring& content)
      {
        if (!content.empty()) {
          clipboard=content;
          XSetSelectionOwner(display,atoms[clipboardAtom],appWindow,CurrentTime);
          return true;
        }
        else {
          return false;
        }
      }

      std::wstring Display::GetClipboard() const
      {
        if (!clipboard.empty()) {
          return clipboard;
        }

        if (!HasClipboard()) {
          return L"";
        }

        XEvent         event;
        Atom           clipboardType=XA_STRING;
        Atom           retType;
        int            retFormat;
        unsigned long  itemsReturn,bytesLeft;
        unsigned char  *data;

        XConvertSelection(display,
                          atoms[clipboardAtom],
                          atoms[targetsAtom],
                          atoms[clipBufferAtom],
                          appWindow,
                          CurrentTime);

        if (!WaitTimedForX11Event(appWindow,SelectionNotify,event) ||
            event.xselection.property==None) {
          // This is not necesarily an error! Not all applications support this! We fall back
          // to XA_STRING in this case.
        }
        else {
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
            std::cout << "Canot get list of support selection types!" << std::endl;
          }
          else if (retType!=atoms[atomAtom] || retFormat/8!=sizeof(Atom)) {
            std::cerr << "Unexspected type of clipboard format list: "<< retType << "!" << std::endl;
            ::XFree(data);
            return L"";
          }
          else {
            Atom *formats=(Atom*)data;

            for (size_t x=0; x<itemsReturn; x++) {
              if (formats[x]==atoms[utf8StringAtom]) {
                clipboardType=atoms[utf8StringAtom];
              }
            }

            ::XFree(data);
          }
        }

        XConvertSelection(display,
                          atoms[clipboardAtom],
                          clipboardType,
                          atoms[clipBufferAtom],
                          appWindow,
                          CurrentTime);

        if (!WaitTimedForX11Event(appWindow,SelectionNotify,event)) {
          std::cerr << "Could not get clipboard response!" << std::endl;
          return L"";
        }

        if (event.xselection.property==None) {
          std::cerr << "Clipboard is empty!" << std::endl;
          return L"";
        }

        if (event.xselection.property!=atoms[clipBufferAtom]) {
          std::cerr << "Could not get clipboard!" << std::endl;
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
          std::cerr << "Cannot get property data!" << std::endl;
          return L"";
        }

        std::wstring result;

        if (retType==None || itemsReturn==0) {
          result=L"";
        }
        else if (retType==XA_STRING && retFormat==8) {
          result=Lum::Base::StringToWString((char*)data);
        }
        else if (retType==atoms[utf8StringAtom] && retFormat==8) {
          result=Lum::Base::UTF8ToWString((char*)data);
        }
        else {
          std::cerr << "Unsupported clipboard content of type " << retType << " and format " << retFormat << "!"<< std::endl;
          result=L"";
        }

        ::XFree(data);

        return result;
      }

      void Display::ClearClipboard()
      {
        if (!clipboard.empty()) {
          selClearPend=true;
          ::XSetSelectionOwner(display,atoms[clipboardAtom],None,CurrentTime);
          clipboard=L"";
        }
      }

      bool Display::HasClipboard() const
      {
        return ::XGetSelectionOwner(display,atoms[clipboardAtom])!=None;
      }

      void Display::ReinitWindows()
      {
        std::list<Window*>::iterator win;

        win=winList.begin();
        while (win!=winList.end()) {
          //(*win)->ReinitWindow();

          ++win;
        }
      }

      /**
        Handle the XSelecitionNotify of X11. A SelectionNotify gets send
        as a result of our request for the selection value. The event states
        where one can get the selection value from.
      */
      void Display::HandleXSelectionNotify(const XSelectionEvent& event)
      {
        Atom               retType;
        int                retFormat;
        unsigned long      itemsReturn,bytesLeft;
        unsigned char      *data;
        Lum::Base::Data dndData;

        if (event.property==None) {
          return;
        }

        if (XGetWindowProperty(display,
                               event.requestor,
                               event.property,
                               0,
                               65000,
                               True,
                               AnyPropertyType,
                               &retType,&retFormat,
                               &itemsReturn,&bytesLeft,
                               &data)!=Success) {
          std::cerr << "Cannot get property data!" << std::endl;
          return;
        }

        if (retType==None) {
          std::cerr << "Illegal property data type!" << std::endl;
          ::XFree(data);
          return;
        }

        if (retType==XA_STRING && retFormat==8) {
          dndData.SetText(Lum::Base::StringToWString((char*)data));
        }
        else {
          std::cerr << "Unsupported drop datatype!" << std::endl;
        }

        ::XFree(data);

        if (event.property==atoms[selectionAtom]) {
          if (querySelectObject!=NULL) {
            if (!querySelectObject->ExecuteAction(Lum::Base::Datatype(Lum::Base::Datatype::text,
                                                                      Lum::Base::Datatype::plain),
                                                  dndData,
                                                  Lum::Base::DataExchangeObject::actionInsert)) {
              Beep();
            }
            querySelectObject=NULL;
          }
        }
        else if (event.property==atoms[clipBufferAtom]) {
          Lum::Base::DataExchangeObject *queryObject;

          queryObject=queryClipboardObject;
          if (queryObject!=NULL) {
            if (!queryObject->ExecuteAction(Lum::Base::Datatype(Lum::Base::Datatype::text,
                                                                Lum::Base::Datatype::plain),
                                            dndData,
                                            Lum::Base::DataExchangeObject::actionInsert)) {
              Beep();
            }
            queryObject=NULL;
          }
        }
        else {
          Beep();
        }
      }

      /**
        handle XSelectionrequest of X11. A SelectionRequest gets send when some application
        wants the selection value and your window has registered the selection. We
        ask the object which has registered the selection for the selection value
        and return a notify message to the requestor.
      */
      void Display::HandleXSelectionRequest(const XSelectionRequestEvent& event)
      {
        Lum::Base::Data data;
        XEvent          selNotify;
        std::string     text;

        selNotify.xselection.type=SelectionNotify;
        selNotify.xselection.display=event.display;
        selNotify.xselection.requestor=event.requestor;
        selNotify.xselection.selection=event.selection;
        selNotify.xselection.target=event.target;
        selNotify.xselection.property=event.property;
        selNotify.xselection.time=event.time;

        if (event.target==XA_STRING ||
            event.target==atoms[compoundTextAtom] ||
            event.target==atoms[utf8StringAtom] ||
            event.target==atoms[targetsAtom]) {
          if (event.selection==XA_PRIMARY) {
            if (selectObject!=NULL) {
              if (selectObject->ExecuteAction(Lum::Base::Datatype(Lum::Base::Datatype::text,
                                                                  Lum::Base::Datatype::plain),
                                              data,
                                              Lum::Base::DataExchangeObject::actionCopy)) {
                text=Lum::Base::WStringToString(data.GetText());
              }
            }
          }
          else if (event.selection==atoms[clipboardAtom]) {
            if (!clipboard.empty()) {
              text=Lum::Base::WStringToString(clipboard);
            }
          }

          if (!text.empty()) {
            if (event.target==XA_STRING || event.target==atoms[compoundTextAtom]) {
              XChangeProperty(event.display,event.requestor,event.property,
                              XA_STRING,8,
                              PropModeReplace,(const unsigned char*)text.c_str(),
                              text.length());
            }
            else if (event.target==atoms[targetsAtom]) {
              Atom formats[3];

              formats[0]=atoms[utf8StringAtom];
              formats[1]=atoms[compoundTextAtom];
              formats[2]=XA_STRING;

              XChangeProperty(event.display,event.requestor,event.property,
                              XA_ATOM,sizeof(Atom)*8,
                              PropModeReplace,(const unsigned char*)formats,
                              sizeof(formats)/sizeof(Atom));
            }
            else {
              std::string tmp=Lum::Base::WStringToUTF8(clipboard);
              XChangeProperty(event.display,event.requestor,event.property,
                              atoms[utf8StringAtom],8,
                              PropModeReplace,(const unsigned char*)tmp.c_str(),
                              tmp.length());
            }
          }
          else {
            std::cerr << "Clipboard is empty" << std::endl;
            selNotify.xselection.property=None;
          }
        }
        else {
          std::cerr << "Unsupported datatype " <<  event.target << " for clipboard request from other application" << std::endl;
          selNotify.xselection.property=None;
        }

        if (XSendEvent(selNotify.xselection.display,
                   selNotify.xselection.requestor,
                   True,
                   NoEventMask,
                   &selNotify)==0) {
          std::cerr << "Error sending selection notify!" << std::endl;
        }
      }

      void Display::GetEvent()
      {
        Tray*  currentTray;
        XEvent e;

        XNextEvent(display,&e);

        if (XFilterEvent(&e,0)!=0) {
          return;
        }

#if defined(HAVE_LIB_XRANDR)
        if (xrandrActive) {
          XRRUpdateConfiguration(&e);
        }
#endif

      /*
        IF d.contextTimer.active THEN
          IF (e.type=X11.ButtonPress) OR (e.type=X11.ButtonRelease) OR (e.type=X11.MotionNotify) OR
            (e.type=X11.KeyPress) OR (e.type=X11.KeyRelease) THEN
            d.RestartContextHelp;
          END;
        END;
                              */

#if defined(HAVE_LIB_XRANDR)
        if (xrandrActive) {
          if (e.type-xrandrEventBase==RRScreenChangeNotify) {
            XRRScreenChangeNotifyEvent *xrandrEvent=(XRRScreenChangeNotifyEvent*)&e;
            int                        width,height;

            if (xrandrEvent->rotation==1 ||
                xrandrEvent->rotation==4) {
              width=xrandrEvent->width;
              height=xrandrEvent->height;
            }
            else {
              width=xrandrEvent->height;
              height=xrandrEvent->width;
            }

            if (GetScreenWidth()!=(unsigned long)width ||
                GetScreenHeight()!=(unsigned long)height) {
              screenWidth=(unsigned long)width;
              screenHeight=(unsigned long)height;

              GetWorkAreaDimension();

              TriggerScreenRotated();
            }

            return;
          }
        }
#endif

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
          OS::EventRef event;

          event=X11::GetEvent(e,currentWin->xic);

          if (event.Valid()) {
            do {
              event->reUse=false;
              currentWin->HandleEvent(event.Get());
            }
            while (event->reUse);
          }
        }
        else if (currentTray!=NULL) {
          OS::EventRef event;

          event=X11::GetEvent(e,currentTray->xic);

          if (event.Valid()) {
            currentTray->HandleEvent(event.Get());
          }
        }
      }

      /**
        Waits for the given event to happen:
        *A X11 event occurs
        *Timer run out

        Returns TRUE, if the wait exceeds the given timeout, else FALSE, if Wait
        returned because an X11 event is available.

        File descriptors getting available will be handled internaly. In this
        case a notification will be send and the wait will be restarted.
      */
      bool Display::WaitTimedForX11Event(::Window window, int eventType, XEvent& event) const
      {
        Lum::Base::SystemTime timer,max;

        XFlush(display);

        timer.SetTime(3,0);

        max.Add(timer);

        while (true) {
          fd_set                set;
          Lum::Base::SystemTime now,tmp;
          timeval               timeout;
          int                   ret,x11Fd;

          x11Fd=ConnectionNumber(display);

          FD_ZERO(&set);
          FD_SET(x11Fd,&set);

          if (now>=max) {
            return false;
          }
          tmp=max;
          tmp.Sub(now);

          timeout.tv_sec=tmp.GetTime();
          timeout.tv_usec=tmp.GetMicroseconds();

          ret=select(x11Fd+1,&set,NULL,NULL,&timeout);

          if (ret>0) {
            if (XCheckTypedWindowEvent(display,window,eventType,&event)==True) {
              return true;
            }
          }
        }
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
      bool Display::WaitTimedForPropertyChange(::Window window, Atom property, XEvent& event) const
      {
        Lum::Base::SystemTime timer,max;

        XFlush(display);

        timer.SetTime(3,0);

        max.Add(timer);

        while (true) {
          fd_set                set;
          Lum::Base::SystemTime now,tmp;
          timeval               timeout;
          int                   ret,x11Fd;

          x11Fd=ConnectionNumber(display);

          FD_ZERO(&set);
          FD_SET(x11Fd,&set);

          if (now>=max) {
            return false;
          }
          tmp=max;
          tmp.Sub(now);

          timeout.tv_sec=tmp.GetTime();
          timeout.tv_usec=tmp.GetMicroseconds();

          ret=select(x11Fd+1,&set,NULL,NULL,&timeout);

          if (ret>0) {
            if (XCheckTypedWindowEvent(display,window,PropertyNotify,&event)==True) {
              if (event.type==PropertyNotify && event.xproperty.atom==property) {
                return true;
              }
            }
          }
        }
      }

      bool Display::GetMousePos(int& x, int& y) const
      {
        ::Window     root;
        ::Window     child;
        unsigned int bmask;
        int          wx,wy;

        return XQueryPointer(display,
                             XRootWindow(display,scrNum),&root,&child,
                             &x,&y,&wx,&wy,&bmask)!=False;
      }


#if defined(HAVE_LIBSM)
      void Display::CallbackDie(SmcConn smc_conn, SmPointer client_data)
      {
        std::cout << "CallbackDie" << std::endl;
      }

      void Display::CallbackSaveYourself(SmcConn smc_conn, SmPointer client_data,
                                         int save_style, Bool shutdown,
                                         int interact_style,
                                         Bool fast)
      {
        struct {
          SmPropValue program[1];
          SmPropValue user[1];
          SmPropValue hint[1];
          SmPropValue restart[1];
        } vals;

        SmProp prop[] = {
          { const_cast<char*>(SmProgram),          const_cast<char*>(SmLISTofARRAY8), 1, vals.program},
          { const_cast<char*>(SmUserID),           const_cast<char*>(SmLISTofARRAY8), 1, vals.user   },
          { const_cast<char*>(SmRestartStyleHint), const_cast<char*>(SmCARD8),        1, vals.hint   },
          { const_cast<char*>(SmCloneCommand),     const_cast<char*>(SmLISTofARRAY8), 1, vals.restart},
          { const_cast<char*>(SmRestartCommand),   const_cast<char*>(SmLISTofARRAY8), 1, vals.restart}
        };

        SmProp *props[] = {
          &prop[0],
          &prop[1],
          &prop[2],
          &prop[3],
          &prop[4],
        };

        std::cout << "CallbackSaveYourself" << std::endl;
        vals.program->value=(void*)const_cast<wchar_t*>(programName.c_str());
        vals.program->length=programName.length();

#if HAVE_PWD_H
        struct passwd *pw = getpwuid(getuid());

        vals.user->value=(void*)const_cast<char*>(pw ? pw->pw_name : "");
        vals.user->length=strlen((const char*)vals.user->value);
#endif

        char restartStyle=SmRestartIfRunning;

        vals.hint->value=&restartStyle;
        vals.hint->length=1;

        vals.restart->value=(void*)const_cast<wchar_t*>(programName.c_str());
        vals.restart->length=programName.length();

        SmcSetProperties(smc_conn,sizeof(props)/sizeof(SmProp*),props);

        SmcSaveYourselfDone(smc_conn,True);
      }

      void Display::CallbackShutdownCancelled(SmcConn smc_conn, SmPointer client_data)
      {
        std::cout << "ShutdownCanceled" << std::endl;
      }

      void Display::CallbackSaveComplete(SmcConn smc_conn, SmPointer client_data)
      {
        std::cout << "SaveComplete" << std::endl;
      }

      void Display::CallbackICEIOError(IceConn connection)
      {
      }
#endif
    }
  }
}
