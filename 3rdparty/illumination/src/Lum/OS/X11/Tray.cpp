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

#include <Lum/OS/X11/Tray.h>

#include <cstring>
#include <iostream>

#include <Lum/Base/String.h>

namespace Lum {
  namespace OS {
    namespace X11 {

      Tray::Tray()
      : status (statusHidden),
        eventAction(new ::Lum::Model::Action),
        openedAction(new ::Lum::Model::Action),
        relWidth(100),relHeight(100),
        x(0),y(0),
        width(0),height(0),
        display(dynamic_cast<Display*>(::Lum::OS::display)),
        bitmap(NULL),
        menu(NULL),
        window(0),
        xic(NULL),
        gc(0)
      {
        assert(display!=NULL);
      }

      Tray::~Tray()
      {
        if (status==statusOpening || status==statusShown) {
          Hide();
        }
      }

      Tray::TrayStatus Tray::GetStatus() const
      {
        return status;
      }

      ::Lum::OS::Bitmap* Tray::GetBitmap() const
      {
        return bitmap;
      }

      void Tray::SetRelativeSize(size_t width, size_t height)
      {
        assert(width>0 && height>0);

        relWidth=width;
        relHeight=height;
      }

      void Tray::SetMenu(::Lum::OS::Window* window)
      {
        menu=window;
      }

      ::Window Tray::FindWindow(::Window parent, const char* name)
      {
        ::Window     rootWindow;
        ::Window     parentWindow;
        ::Window*    children;
        unsigned int childCount;
        ::Window     res=0;

        if (XQueryTree(display->display,parent,&rootWindow,&parentWindow,&children,&childCount)) {
          for (size_t x=0; res==0 && x<childCount; x++) {
            XClassHint cHint;

            if (XGetClassHint(display->display,children[x],&cHint)) {
              if (strcmp(cHint.res_name,name)==0) {
                res=children[x];
              }
              XFree(cHint.res_name);
              XFree(cHint.res_class);
            }

            if (res==0) {
              res=FindWindow(children[x],name);
            }

          }
          XFree(children);
        }

        return res;
      }

      bool Tray::Show()
      {
        assert(status==statusHidden);

        XSizeHints           *sHints;
        XWMHints             *wHints;
        XClassHint           *cHints;
        XSetWindowAttributes attr;
        unsigned long        mask;
        XGCValues            values;
        std::string          className;

        ::Window             rootWindow;
        ::Window             parentWindow;
        ::Window*            children;
        ::Window             panel=0;
        ::Window             notifArea=0;
        unsigned int         childCount;

        // Try to find the gnome-panel and widthin the gnome panel the notification area
        // if the NotificationArea has been found take its bound to precalculate
        // the size of the notification window.
        if (XQueryTree(display->display,XRootWindow(display->display,display->scrNum),&rootWindow,&parentWindow,&children,&childCount)) {
          for (size_t x=0; notifArea==0 && x<childCount; x++) {
            XClassHint cHint;

            if (XGetClassHint(display->display,children[x],&cHint)) {
              if (strcmp(cHint.res_name,"gnome-panel")==0) {
                //printf("Found gnome-panel %lx\n",children[x]);
                panel=children[x];
                notifArea=FindWindow(children[x],"NotificationArea");
              }
              XFree(cHint.res_name);
              XFree(cHint.res_class);
            }
          }
          XFree(children);
        }

        if (notifArea!=0) {
          //printf("Found NotificationArea %lx\n",notifArea);
          ::XWindowAttributes attributes;

          if (XGetWindowAttributes(display->display,panel,&attributes)) {
            //printf("%d %d\n",attributes.width,attributes.height);
            if (attributes.width>attributes.height) {
              height=attributes.height-1;
            }
            else {
              width=attributes.width-1;
            }
          }
        }

        if (height!=0) {
          width=(height*relWidth)/relHeight;
        }
        else if (width!=0){
          height=(width*relHeight)/relWidth;
        }
        else {
          width=16;
          height=16;
        }

        sHints=XAllocSizeHints();
        wHints=XAllocWMHints();
        cHints=XAllocClassHint();

        if (sHints==NULL || wHints==NULL || cHints==NULL) {
          XFree(sHints);
          XFree(wHints);
          XFree(cHints);
          std::cerr << "Cannot allocate window hints" << std::endl;
          return false;
        }

        attr.colormap=display->colorMap;
        attr.bit_gravity=NorthWestGravity;
        attr.event_mask=KeyPressMask | KeyReleaseMask |  ExposureMask |
                        StructureNotifyMask | ButtonPressMask |  ButtonReleaseMask |
                        ButtonMotionMask | PointerMotionMask | FocusChangeMask;

        attr.override_redirect=False;
        attr.save_under=False;
        attr.backing_store=False;

        window=XCreateWindow(display->display,
                             XRootWindow(display->display,
                                         display->scrNum),
                             0,0,
                             width,height,
                             CopyFromParent,
                             CopyFromParent,
                             InputOutput,
                             display->visual,
                             CWBorderPixel
                             | CWBitGravity
                             | CWBackingStore
                             | CWOverrideRedirect
                             | CWSaveUnder
                             | CWEventMask
                             | CWColormap,
                             &attr);

        if (window==0) {
          XFree(sHints);
          XFree(wHints);
          XFree(cHints);
          std::cerr << "Cannot open window" << std::endl;
          return false;
        }


        xic=XCreateIC(display->xim,
                      XNInputStyle,XIMPreeditNothing | XIMStatusNothing,
                      XNClientWindow,window,
                      XNFocusWindow,window,
                      NULL);

        if (xic==NULL) {
          XFree(sHints);
          XFree(wHints);
          XFree(cHints);
          XDestroyWindow(display->display,window);
          return false;
        }

        sHints->min_width=width;
        sHints->min_height=height;

        sHints->min_aspect.x=relWidth;
        sHints->min_aspect.y=relHeight;
        sHints->max_aspect.x=relWidth;
        sHints->max_aspect.y=relHeight;

        sHints->flags=PAspect|PMinSize;

        wHints->initial_state=NormalState;
        wHints->input=True;
        wHints->flags=StateHint | InputHint;

        className=::Lum::Base::WStringToString(display->GetAppName());
        cHints->res_name=const_cast<char*>(className.c_str());
        cHints->res_class=const_cast<char*>(className.c_str());

        XSetWMProperties(display->display,
                         window,
                         NULL,
                         NULL,
                         NULL,0,
                         sHints,
                         wHints,
                         cHints);

        XFree(sHints);
        XFree(wHints);
        XFree(cHints);

        /*
        if (XSetWMProtocols(display->display,this->window,&display->atoms[Display::deleteProtAtom],1)==0) {
          std::cerr << "Warning: Cannot activate close button protocol" << std::endl;
        }*/

        /* We mark the windows as Xdnd aware */

        /*
        display->SetAtomProperty(this,display->atoms[Display::XdndAwareAtom],
                                 XDND_VERSION);
        */

        display->SetEmbedInfoProperty(window);
        display->SendNetSystemTrayRequestDock(window);

        mask=0;
        gc=XCreateGC(display->display,window,mask,&values);

        status=statusOpening;

        display->AddTray(this);

        SetTitle(title);

        return true;
      }

      bool Tray::Hide()
      {
        assert(status==statusOpening || status==statusShown);

        status=statusHidden;

        if (xic!=NULL) {
          XDestroyIC(xic);
          xic=NULL;
        }

        if (window!=0) {
          XDestroyWindow(display->display,window);
          window=0;
        }

        if (gc!=0) {
          XFreeGC(display->display,gc);
          gc=0;
        }

        display->RemoveTray(this);

        delete bitmap;
        bitmap=NULL;

        return true;
      }


      void Tray::Redraw()
      {
        assert(status==statusShown);

        //XCopyArea(display->display,dynamic_cast<Bitmap*>(bitmap)->pixmap,window,gc,0,0,width,height,0,0);
      }

      void Tray::SetTitle(const std::wstring& title)
      {
        this->title=title;

        if (window!=0) {
          std::string buffer;

          XStoreName(display->display,window,::Lum::Base::WStringToString(title).c_str());

          buffer=::Lum::Base::WStringToUTF8(title);
          XChangeProperty(display->display,window,
                          display->atoms[Display::netWMNameAtom],
                          display->atoms[Display::utf8StringAtom],8,
                          PropModeReplace,
                          (const unsigned char*)buffer.c_str(),
                          buffer.length());
        }
      }

      Model::Action* Tray::GetEventAction() const
      {
        return eventAction.Get();
      }

      Model::Action* Tray::GetOpenedAction() const
      {
        return openedAction.Get();
      }

      void Tray::HandleEvent(::Lum::OS::Event* event)
      {
        XEvent x11Event;

        OS::X11::GetX11Event(event,x11Event);

        switch (x11Event.type) {
        case ClientMessage:
          if (x11Event.xclient.message_type==display->atoms[Display::xEmbedAtom]) {
            switch (x11Event.xclient.data.l[1]) {
            case 0:
              //std::cout << "NOTIFY" << std::endl;
              break;
            case 1:
              //std::cout << "ACTIVATE" << std::endl;
              //OnFocusIn();
              break;
            case 2:
              //std::cout << "DEACTIVATE" << std::endl;
              //OnFocusOut();
              break;
            case 3:
              //std::cout << "REQUEST_FOCUS" << std::endl;
              break;
            case 4:
              //std::cout << "FOCUS_IN" << std::endl;
              break;
            case 5:
              //std::cout << "FOCUS_OUT" << std::endl;
              break;
            case 6:
              //std::cout << "FOCUS_NEXT" << std::endl;
              break;
            case 7:
              //std::cout << "FOCUS_PREV" << std::endl;
              break;
            case 10:
              //std::cout << "MODALITY_ON" << std::endl;
              //Disable();
              break;
            case 11:
              //std::cout << "MODALITY_OFF" << std::endl;
              //Enable();
              break;
            default:
              std::cout << x11Event.xclient.data.l[1] << std::endl;
              break;
            }
          }
          break;
        case ConfigureNotify: {
          while (XCheckTypedWindowEvent(display->display,window,ConfigureNotify,&x11Event)==True) {
            // no code
          }

          int      x;
          int      y;
          ::Window tmp;

          if (XTranslateCoordinates(display->display,window,
                                    XRootWindow(display->display,display->scrNum),
                                    0,0,&x,&y,&tmp)!=0) {
            this->x=x;
            this->y=y;
          }

          if ((size_t)x11Event.xconfigure.width!=width || (size_t)x11Event.xconfigure.height!=height) {
            width=x11Event.xconfigure.width;
            height=x11Event.xconfigure.height;
            /*
              TODO: Potentially trigger redraw!?
            */
          }
          }
          break;
        case Expose:
          {
            Redraw();
            /*
            int x1,y1,x2,y2;

            draw->PushClipBegin(x11Event.xexpose.x,x11Event.xexpose.y,
                                x11Event.xexpose.width,x11Event.xexpose.height);

            x1=x11Event.xexpose.x;
            y1=x11Event.xexpose.y;
            x2=x11Event.xexpose.x+x11Event.xexpose.width-1;
            y2=x11Event.xexpose.y+x11Event.xexpose.height-1;

            while (XCheckTypedWindowEvent(display->display,window,Expose,&x11Event)==True) {
              if (x11Event.xexpose.x<x1) {
                x1=x11Event.xexpose.x;
              }
              if (x11Event.xexpose.y<y1) {
                y1=x11Event.xexpose.y;
              }
              if (x11Event.xexpose.x+x11Event.xexpose.width-1>x2) {
                x2=x11Event.xexpose.x+x11Event.xexpose.width-1;
              }
              if (x11Event.xexpose.y+x11Event.xexpose.height-1>y2) {
                y2=x11Event.xexpose.y+x11Event.xexpose.height-1;
              }

              draw->AddClipRegion(x11Event.xexpose.x,x11Event.xexpose.y,
                                  x11Event.xexpose.width,x11Event.xexpose.height);
            }
            draw->PushClipEnd();
            TriggerRedraw(x1,y1,x2-x1+1,y2-y1+1);
            draw->PopClip();*/
          }
          break;
        case GraphicsExpose:
          {
            Redraw();
            /*
            int x1,y1,x2,y2;

            draw->PushClipBegin(x11Event.xgraphicsexpose.x,x11Event.xgraphicsexpose.y,
                                x11Event.xgraphicsexpose.width,x11Event.xgraphicsexpose.height);

            x1=x11Event.xexpose.x;
            y1=x11Event.xexpose.y;
            x2=x11Event.xexpose.x+x11Event.xexpose.width-1;
            y2=x11Event.xexpose.y+x11Event.xexpose.height-1;

            while (XCheckTypedWindowEvent(display->display,window,GraphicsExpose,&x11Event)==True) {
              if (x11Event.xexpose.x<x1) {
                x1=x11Event.xexpose.x;
              }
              if (x11Event.xexpose.y<y1) {
                y1=x11Event.xexpose.y;
              }
              if (x11Event.xexpose.x+x11Event.xexpose.width-1>x2) {
                x2=x11Event.xexpose.x+x11Event.xexpose.width-1;
              }
              if (x11Event.xexpose.y+x11Event.xexpose.height-1>y2) {
                y2=x11Event.xexpose.y+x11Event.xexpose.height-1;
              }

              draw->AddClipRegion(x11Event.xgraphicsexpose.x,x11Event.xgraphicsexpose.y,
                                  x11Event.xgraphicsexpose.width,x11Event.xgraphicsexpose.height);
            }
            draw->PushClipEnd();
            TriggerRedraw(x1,y1,x2-x1+1,y2-y1+1);
            draw->PopClip();*/
          }
          break;
        case MappingNotify:
          XRefreshKeyboardMapping(&x11Event.xmapping);
          break;
        case UnmapNotify:
          if (status==statusOpening || status==statusShown) {
            Hide();
          }
          break;
        case MapNotify:
          status=statusShown;
          if (bitmap==NULL) {
            bitmap=driver->CreateBitmap(width,height);
          }
          openedAction->Trigger();
          break;
        case FocusIn:
          //OnFocusIn();
          break;
        case FocusOut:
          //OnFocusOut();
          break;
        case EnterNotify:
          //TriggerMouseIn();
          break;
        case LeaveNotify:
          //TriggerMouseOut();
          break;
        case VisibilityNotify:
          /*
          if (x11Event.xvisibility.state>0) {
            TriggerHidden();
          }*/
          break;
        case ButtonPress:
          /*
          oldButton=lastButton;
          oldPress=lastPress;
          lastButton=thisButton;
          lastPress=thisPress;
          thisButton=*dynamic_cast<MouseEvent*>(event);
          thisPress=x11Event.xbutton.time;

          if (modalCount!=0) {
            return true;
          }*/
          break;
        case ButtonRelease:
          /*
          if (modalCount!=0) {
            return true;
          }*/
          break;
        case MotionNotify:
          /*
          // skip all pending motion events and evaluate only the last available
          while (XCheckMaskEvent(display->display,ButtonMotionMask,&x11Event)) {
          }
          ::Lum::OS::X11::GetX11Event(event,x11Event);
          if (modalCount!=0) {
            return true;
          }*/
          break;
        default:
          break;
        }

        OS::Window::EventMsg msg;

        msg.event=event;

        eventAction->Trigger(msg);

        OS::MouseEvent *mouseEvent;

        if ((mouseEvent=dynamic_cast<OS::MouseEvent*>(event))!=NULL) {
          if (mouseEvent->type==OS::MouseEvent::down && mouseEvent->button==Lum::OS::MouseEvent::button3) {
            if (menu!=NULL) {
              menu->SetPos(x,y);
              /* ignore */ menu->Open(true);
            }
          }
        }
      }
    }
  }
}

