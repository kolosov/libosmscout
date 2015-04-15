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

#include <Lum/OS/X11/Window.h>

#include <cassert>
#include <cstdlib>
#include <cstring>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <iostream>

#include <X11/X.h>
#include <X11/extensions/shape.h>
#include <X11/Xatom.h>

#if defined(LUM_HAVE_LIB_HILDON)
  #include <hildon/hildon-version.h>
#endif

#include <Lum/Base/String.h>

#include <Lum/Manager/Display.h>

#include <Lum/OS/X11/Display.h>

namespace Lum {
  namespace OS {
    namespace X11 {

      static const int XDND_VERSION=3;

#if 0

      /**
        Called when some drag action should be started.
      */
      bool WindowDesc::HandleDrag(OS::MotionEvent event)
      {
        XEvent x11Event;
        Display display;

        display=D.display(Display);
        display.dragStart=false;
        if (w.modalCount==0) {
          display.dragObject=w.interface.GetDnDObject(display.dragX,display.dragY,true);
          if (display.dragObject!=NULL) {

            display.dragInfo=new DnDDataInfo;
            display.dragInfo.Init;
            display.dragObject.GetDragInfo(display.dragInfo);
            display.StopContextHelp;Calculation action *)

            if (event.qualifier*E.keyMask==E.shiftMask) {
              display.dndAction=DD.move;
              X11.XDefineCursor(display.display,w.window,display.moveCursor);
            }
            else if (event.qualifier*E.keyMask==E.controlMask) {
              display.dndAction=DD.copy;
              X11.XDefineCursor(display.display,w.window,display.copyCursor);
            }
            else if (event.qualifier*E.keyMask==E.shiftMask+E.controlMask) {
              display.dndAction=DD.link;
              X11.XDefineCursor(display.display,w.window,display.linkCursor);
            }
            else {
              display.dndAction=DD.default;
              X11.XDefineCursor(display.display,w.window,display.dndCursor);
            }
            EO.GetX11Event(event,x11Event);
            display.dropWindow=x11Event.xmotion.window;
            return true;
          }
        }
        return false;
      }

      /**
        Called when a drop action should be handled.
      */
      bool WindowDesc::HandleDragDrop(OS::Event event)
      {
        Window dragWin;
        int rX;
        int rY;
        int cX;
        int cY;
        bool return;
        OS::Window window;
        OS::Object dropObject;
        Base::DragDrop::DnDData dragData;
        int group;
        int type;

        return=false;
        D.display(Display).dragStart=false;
        if ((D.display(Display).dropWindow!=0)) {
          D.display(Display).RestartContextHelp;
          dragWin=D.display(Display).GetWindowOnScreen(rX,rY,cX,cY);
          if (dragWin!=0) {
            window=D.display(Display).GetWindow(dragWin);
            if (window!=NULL) {
              dropObject=window.interface.GetDnDObject(cX,cY,false);
              if ((window(Window).modalCount==0) && (dropObject!=NULL) && (dropObject!=D.display(Display).dragObject)) {
                if (dropObject.GetDropDataType(D.display(Display).dragInfo,group,type,D.display(Display).dndAction)) {
                  dragData=D.display(Display).dragObject.GetDragData(group,type,D.display(Display).dndAction);
                  if (dragData!=NULL) {
                    if (dropObject.HandleDrop(dragData,D.display(Display).dndAction)) {nothing to do yet*)

                    }
                    else {
                      D.display.Beep;
                    }
                  }
                }
                return=true;
              }
            }
            else {
              Err.String("End external drag -> drop");
              Err.Ln;.HandleMotifDragDrop(dragWin,rX,rY,event.event.xbutton);*)

            }
          }
          X11.XUndefineCursor(D.display(Display).display,w.window);
          D.display(Display).dropWindow=0;
          D.display(Display).dragObject=NULL;
        }
        return return;
      }

#endif

      Window::Window(OS::Display* display)
      : display(dynamic_cast<Display*>(display)),
        balloonId(0),
        window(0),fullScreen(false),
        customWindowType(0),
        checkRefreshAction(new Model::Action()),
        xic(0)
      {
        Observe(checkRefreshAction);
      }

      Window::~Window()
      {
        // no code
      }

      void Window::Resync(Lum::Base::Model* model,
                          const Lum::Base::ResyncMsg& msg)
      {
        if (model==checkRefreshAction &&
            checkRefreshAction->IsFinished()) {
          CheckRefresh();
        }

        OS::Base::Window::Resync(model,msg);
      }

      void Window::SetTitle(const std::wstring& name)
      {
        OS::Base::Window::SetTitle(name);

        if (window!=0) {
          std::string buffer;

          XStoreName(display->display,window,Lum::Base::WStringToString(title).c_str());

          buffer=Lum::Base::WStringToUTF8(title);
          XChangeProperty(display->display,window,
                          display->atoms[Display::netWMNameAtom],
                          display->atoms[Display::utf8StringAtom],8,
                          PropModeReplace,
                          (const unsigned char*)buffer.c_str(),
                          buffer.length());
        }
      }

      ::Drawable Window::GetDrawable() const
      {
        return window;
      }

      ::Display* Window::GetDisplay() const
      {
        return display->display;
      }

      void Window::RecalcWindowPosition()
      {
        int      x;
        int      y;
        ::Window tmp;

        if (XTranslateCoordinates(display->display,window,
                                  XRootWindow(display->display,display->scrNum),
                                  0,0,&x,&y,&tmp)!=0) {
          this->x=x;
          this->y=y;
        }
      }

      void Window::OnFocusIn()
      {
        if (xic!=0) {
          XSetICFocus(xic);
        }

        if (!grabMouse) {
          display->StartContextHelp();
        }

        TriggerFocusIn();
      }

      void Window::OnFocusOut()
      {
        if (!grabMouse) {
          display->StopContextHelp();
        }

        TriggerFocusOut();

        if (xic!=0) {
          XUnsetICFocus(xic);
        }
      }


      bool Window::HandleEvent(OS::Event* event)
      {
        if (display->GetKeyboardManager()!=NULL &&
            display->GetKeyboardManager()->HandleEvent(this,event)) {
          return true;
        }

/*
  name        : C.string;
  text        : U.Text;
  msg         : X11.XClientMessageEvent;

  retType     : X11.Atom;
  retFormat,
  itemsReturn,
  bytesLeft   : LONGINT;
  xData       : C.string;
data        : POINTER TO ARRAY OF LONGINT;*/
        XEvent x11Event;

        GetX11Event(event,x11Event);

        switch (x11Event.type) {
        case ClientMessage:
          if (((Atom)x11Event.xclient.data.l[0])==display->atoms[Display::deleteProtAtom]
              && modalCount==0) {
            TriggerClosed();
            return true;
          }
          else if (((Atom)x11Event.xclient.data.l[0])==display->atoms[Display::netWMPingAtom]) {
            x11Event.xclient.window=XRootWindow(display->display,display->scrNum);
            XSendEvent(display->display,x11Event.xclient.window,False,
                       SubstructureNotifyMask|SubstructureRedirectMask,&x11Event);
          }
          /*
          ELSIF (x11Event.xclient.message_type=D.display(Display).atoms[XdndEnterAtom]) THEN
                        Out.String("XDND Enter"); Out.Ln;
                        Out.String("XID: "); Out.Hex(x11Event.xclient.data.l[0],0); Out.Ln;
                        Out.String("Protokoll: "); Out.LongInt(s.VAL(SHORTINT,x11Event.xclient.data.b[7]),0); Out.Ln;

        FOR x:=2 TO 4 DO
          name:=X11.XGetAtomName(D.display(Display).display,x11Event.xclient.data.l[x]);
          IF name#NIL THEN
            Out.String("Datatype: ");
            text:=U.CStringToText(name);
            Out.String(text^); Out.Ln;
          END;
        END;

        IF x11Event.xclient.data.b[4]#0X THEN (* TODO: fix *)
          IF X11.XGetWindowProperty(D.display(Display).display,
                                    x11Event.xclient.data.l[0],
                                    D.display(Display).atoms[XdndTypeListAtom],
                                    0,
                                    MAX(LONGINT),
                                    X11.False,
                                    D.display(Display).atoms[atomAtom],
                                    retType,
                                    retFormat,
                                    itemsReturn,
                                    bytesLeft,
                                    xData)#X11.Success THEN
            Err.String("Cannot get property"); Err.Ln;
          ELSE
            NEW(data,itemsReturn);
            s.MOVE(s.VAL(LONGINT,xData),s.VAL(LONGINT,data),SIZE(LONGINT)*itemsReturn);
            XFree(xData);
            FOR x:=0 TO itemsReturn-1 DO
              name:=X11.XGetAtomName(D.display(Display).display,data[x]);
              IF name#NIL THEN
                Out.String("Datatype: ");
                text:=U.CStringToText(name);
                Out.String(text^); Out.Ln;
              END;
            END;
          END;
        END;

        Out.Ln;

      ELSIF (x11Event.xclient.message_type=D.display(Display).atoms[XdndLeaveAtom]) THEN
        Out.String("XDND Leave"); Out.Ln;
        Out.String("XID: "); Out.Hex(x11Event.xclient.data.l[0],0); Out.Ln;
        Out.Ln;

      ELSIF (x11Event.xclient.message_type=D.display(Display).atoms[XdndPositionAtom]) THEN
        Out.String("XDND Position"); Out.Ln;
        Out.String("XID: "); Out.Hex(x11Event.xclient.data.l[0],0); Out.Ln;
        Out.String("X: "); Out.LongInt(x11Event.xclient.data.s[5],0); Out.Ln;
        Out.String("Y: "); Out.LongInt(x11Event.xclient.data.s[4],0); Out.Ln;

        IF x11Event.xclient.data.l[4]=D.display(Display).atoms[XdndActionMoveAtom] THEN
          Out.String("action move"); Out.Ln;
        ELSIF x11Event.xclient.data.l[4]=D.display(Display).atoms[XdndActionCopyAtom] THEN
          Out.String("action copy"); Out.Ln;
        ELSIF x11Event.xclient.data.l[4]=D.display(Display).atoms[XdndActionLinkAtom] THEN
          Out.String("action link"); Out.Ln;
        END;
        Out.Ln;

        msg.type:=X11.ClientMessage;
        msg.format:=32;
        msg.display:=D.display(Display).display;
        msg.window:=x11Event.xclient.data.l[0];
        msg.message_type:=D.display(Display).atoms[XdndStatusAtom];
        msg.data.l[0]:=w.window;
        msg.data.l[1]:=3;
        msg.data.s[4]:=0; (* y *)
        msg.data.s[5]:=0; (* x *)
        msg.data.s[6]:=0; (* h *)
        msg.data.s[7]:=0; (* w *)
        msg.data.l[4]:=D.display(Display).atoms[XdndActionCopyAtom];

        IF X11.XSendEvent(D.display(Display).display,
                          x11Event.xclient.data.l[0],
                          X11.False,X11.NoEventMask,
                          s.VAL(X11.XEventPtr,s.ADR(msg)))=0 THEN
          Out.String("Could not send XDNDStatus message"); Out.Ln;
        END;


      ELSIF (x11Event.xclient.message_type=D.display(Display).atoms[XdndStatusAtom]) THEN
        Out.String("XDND Status"); Out.Ln;

      ELSIF (x11Event.xclient.message_type=D.display(Display).atoms[XdndDropAtom]) THEN
        Out.String("XDND Drop"); Out.Ln;

        msg.type:=X11.ClientMessage;
        msg.format:=32;
        msg.display:=D.display(Display).display;
        msg.window:=w.window;
        msg.message_type:=D.display(Display).atoms[XdndFinishedAtom];
        msg.data.l[0]:=w.window;
        msg.data.l[1]:=0;

        X11.XConvertSelection(D.display(Display).display,
                              D.display(Display).atoms[XdndSelectionAtom],
                              297, (* text/plain *)
                              D.display(Display).atoms[dropAtom],
                              w.window,
                              x11Event.xclient.data.l[2]);

(*          dropObject:=window.GetDnDObject(cX,cY,FALSE);
              IF dropObject.HandleDrop(dragData,D.display.dndAction) THEN*)

(*          IF X11.XSendEvent(D.display.display,
                          x11Event.xclient.data.l[0],
                          X11.False,X11.NoEventMask,
                          s.VAL(X11.XEventPtr,s.ADR(msg)))=0 THEN
          Out.String("Could not send XDNDStatus message"); Out.Ln;
        END;*)

      ELSIF (x11Event.xclient.message_type=D.display(Display).atoms[XdndFinishedAtom]) THEN
        Out.String("XDND Finished"); Out.Ln;
        Out.String("XID: "); Out.Hex(x11Event.xclient.data.l[0],0); Out.Ln;
              Out.Ln;
              */
          else if (x11Event.xclient.message_type==display->atoms[Display::xEmbedAtom]) {
            switch (x11Event.xclient.data.l[1]) {
            case 0:
              //std::cout << "NOTIFY" << std::endl;
              break;
            case 1:
              //std::cout << "ACTIVATE" << std::endl;
              OnFocusIn();
              break;
            case 2:
              //std::cout << "DEACTIVATE" << std::endl;
              OnFocusOut();
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
              Disable();
              break;
            case 11:
              //std::cout << "MODALITY_OFF" << std::endl;
              Enable();
              break;
            default:
              //std::cout << x11Event.xclient.data.l[1] << std::endl;
              break;
            }
            return true;
          }
          else if ((Atom)x11Event.xclient.message_type==XInternAtom(display->display,"_MB_GRAB_TRANSFER",true)) {
#ifdef LUM_HAVE_LIB_HILDON
            TriggerMenu();
#endif
          }
          break;
        case ConfigureNotify:
          while (XCheckTypedWindowEvent(display->display,this->window,ConfigureNotify,&x11Event)==True) {
            // no code
          }

          RecalcWindowPosition();

          fullScreen=IsFullScreenInternal();

          if ((size_t)x11Event.xconfigure.width!=width ||
              (size_t)x11Event.xconfigure.height!=height) {
            width=x11Event.xconfigure.width;
            height=x11Event.xconfigure.height;

            draw->SetNewSize(width,height);

            TriggerResize(x11Event.xconfigure.width-x11Event.xconfigure.border_width,
                          x11Event.xconfigure.height-x11Event.xconfigure.border_width);
            TriggerDraw(0,0,GetWidth(),GetHeight());
            CheckRefresh();
          }
          return true;
        case Expose:
          {
            if (IsShown() && !onOpened) {
              TriggerPreOpened();
            }

            TriggerDraw(x11Event.xexpose.x,
                        x11Event.xexpose.y,
                        x11Event.xexpose.width,
                        x11Event.xexpose.height);

            while (XCheckTypedWindowEvent(display->display,window,Expose,&x11Event)==True) {
              TriggerDraw(x11Event.xexpose.x,
                          x11Event.xexpose.y,
                          x11Event.xexpose.width,
                          x11Event.xexpose.height);
            }

            CheckRefresh();

            if (IsShown() && !onOpened) {
              TriggerOpened();
            }
          }
          return true;
        case GraphicsExpose:
          {
            TriggerDraw(x11Event.xgraphicsexpose.x,
                        x11Event.xgraphicsexpose.y,
                        x11Event.xgraphicsexpose.width,
                        x11Event.xgraphicsexpose.height);

            while (XCheckTypedWindowEvent(display->display,window,GraphicsExpose,&x11Event)==True) {
              TriggerDraw(x11Event.xgraphicsexpose.x,
                          x11Event.xgraphicsexpose.y,
                          x11Event.xgraphicsexpose.width,
                          x11Event.xgraphicsexpose.height);
            }

            CheckRefresh();
          }
          return true;
        case CreateNotify:
          // no code
          break;
        case ReparentNotify:
          // no code
          break;
        case MappingNotify:
          XRefreshKeyboardMapping(&x11Event.xmapping);
          return true;
        case UnmapNotify:
          TriggerUnmaped();
          draw->OnUnmap();
          return true;
        case MapNotify:
          {
            XWMHints *wHints;

            wHints=XGetWMHints(display->display,window);
            if (wHints!=NULL) {
              if (wHints->flags & InputHint && wHints->input==False) {
                wHints->input=True;
                wHints->flags=InputHint;
                XSetWMHints(display->display,window,wHints);
              }
              XFree(wHints);
            }
          }

          RecalcWindowPosition();
          draw->OnMap();
          TriggerMaped();

          if (grabMouse) {
            MouseGrabOn();
          }
          if (grabKeyboard) {
            KeyboardGrabOn();
          }
          return true;
        case FocusIn:
          if (display->GetKeyboardManager()!=NULL) {
            display->GetKeyboardManager()->OnWindowFocusIn(this);
          }
          OnFocusIn();
          return true;
        case FocusOut:
          OnFocusOut();
          if (display->GetKeyboardManager()!=NULL) {
            display->GetKeyboardManager()->OnWindowFocusOut(this);
          }
          return true;
        case EnterNotify:
          TriggerMouseIn();
          return true;
        case LeaveNotify:
          TriggerMouseOut();
          return true;
        case VisibilityNotify:
          if (x11Event.xvisibility.state>0) {
            TriggerHidden();
          }
          return true;
        case ButtonPress:
          if (modalCount!=0) {
            return true;
          }
          break;
        case ButtonRelease:
          if (modalCount!=0) {
            return true;
          }
          break;
        case MotionNotify:
          // skip all pending motion events and evaluate only the last available
          while (XCheckMaskEvent(display->display,ButtonMotionMask,&x11Event)) {
          }
          GetX11Event(event,x11Event);
          if (modalCount!=0) {
            return true;
          }
          break;
#if defined(LUM_HAVE_LIB_HILDON)
        case KeyPress:
          if (dynamic_cast<KeyEvent*>(event)->key==keyF4) {
            TriggerMenu();
          }
          else if (dynamic_cast<KeyEvent*>(event)->key==keyF6) {
            if (IsFullScreen()) {
              LeaveFullScreen();
            }
            else {
              SetFullScreen();
            }
          }
#endif
        case PropertyNotify:
          //std::cout << "Property " << x11Event.xproperty.atom << " has changed!" << std::endl;
          break;
        default:
          break;
        }
        /*
        CASE x11Event.type OF
        | X11.ButtonPress:
        IF (x11Event.xbutton.button=E.dragDropButton) THEN
        D.display(Display).dragStart:=TRUE;
        D.display(Display).dragX:=x11Event.xbutton.x;
        D.display(Display).dragY:=x11Event.xbutton.y;
      END;

  | X11.ButtonRelease:
      IF (x11Event.xbutton.type=X11.ButtonRelease)
      & (x11Event.xbutton.button=E.dragDropButton) THEN
        IF w.HandleDragDrop(event) THEN
          RETURN TRUE;
        END;
      END;
  | X11.MotionNotify:
      IF (event(E.MotionEvent).qualifier*E.buttonMask={E.dragDropButton})
       & (D.display(Display).dragStart) THEN
        IF   (x11Event.xmotion.x>D.display(Display).dragX+D.display.spaceWidth DIV 2)
          OR (x11Event.xmotion.x<D.display(Display).dragX-D.display.spaceWidth DIV 2)
          OR (x11Event.xmotion.y>D.display(Display).dragY+D.display.spaceHeight DIV 2)
          OR (x11Event.xmotion.y<D.display(Display).dragY-D.display.spaceHeight DIV 2) THEN
          IF w.HandleDrag(event(E.MotionEvent)) THEN
            RETURN TRUE;
          END;
        END;
      END;*/
        return Base::Window::HandleEvent(event);
      }

      void Window::TriggerDraw(int x, int y, size_t width, size_t height)
      {
        /*
          Try to optimize redrawRects list
        */
        std::vector<Rectangle>::iterator r=redrawRects.begin();
        while (r!=redrawRects.end()) {
          /*
            If the new rectangle is completly contained by an existing rectangle,
            skip insertion.
            */
          if (x>=r->x &&
              y>=r->y &&
              x+width<=r->x+r->width &&
              y+height<=r->y+r->height) {
            return;
          }
          /*
            If the new rectange completely contains an existing rectangle,
            exchange data of the existing rectangle with the data of the new
            rectangle.
          */
          else if (x<=r->x &&
                   y<=r->y &&
              x+width>=r->x+r->width &&
              y+height>=r->y+r->height) {
            r->x=x;
            r->y=y;
            r->width=width;
            r->height=height;
            return;
          }

          ++r;
        }

        Rectangle rect;

        rect.x=x;
        rect.y=y;
        rect.width=width;
        rect.height=height;

        redrawRects.push_back(rect);

        if (redrawRects.size()==1) {
          display->QueueActionForEventLoop(checkRefreshAction);
        }
      }

      bool Window::CheckRefresh()
      {
        if (redrawRects.size()==0) {
          return false;
        }

        if (!IsShown()) {
          redrawRects.clear();
          return false;
        }

        // We do not redraw anything if the display is off...
        if (Manager::Display::Instance()!=NULL &&
            Manager::Display::Instance()->GetDisplayState()==Manager::Display::DisplayStateOff) {
          redrawRects.clear();
          return false;
        }

        // We make a copy of the original redra wrectangles and
        // clear the redrawRects list. This may we do not get disturbed
        // if somebody adds a new rectangle while we are redrawing.
        std::vector<Rectangle> rects(redrawRects);

        redrawRects.clear();

        int x1=rects.front().x;
        int y1=rects.front().y;
        int x2=x1+rects.front().width-1;
        int y2=y1+rects.front().height-1;

        for (std::vector<Rectangle>::const_iterator rect=rects.begin();
             rect!=rects.end();
             ++rect) {
          x1=std::min(x1,rect->x);
          y1=std::min(y1,rect->y);
          x2=std::max(x2,(int)(rect->x+rect->width-1));
          y2=std::max(y2,(int)(rect->y+rect->height-1));
        }

        DrawInfo* draw=RequestRefreshDrawInfo(x1,y1,x2-x1+1,y2-y1+1);

        for (std::vector<Rectangle>::const_iterator rect=rects.begin();
             rect!=rects.end();
             ++rect) {
          // There seems to be a bug in the cairo version with hildon...
#if !defined(LUM_HAVE_LIB_HILDON)
          draw->PushClip(rect->x,rect->y,rect->width,rect->height);
#endif
          OS::Window::DrawMsg msg;

          msg.draw=draw;
          msg.x=rect->x;
          msg.y=rect->y;
          msg.width=rect->width;
          msg.height=rect->height;

          drawAction->Trigger(msg);

#if !defined(LUM_HAVE_LIB_HILDON)
          draw->PopClip();
#endif
        }

        ReleaseRefreshDrawInfo(x1,y1,x2-x1+1,y2-y1+1);

        return true;
      }

      void Window::Resize(size_t width, size_t height)
      {
        Base::Window::Resize(width,height);

        if (window!=0) {
          XResizeWindow(display->display,window,width,height);
          draw->SetNewSize(width,height);
        }
      }

      void Window::SetSize(size_t width,
                           size_t height)
      {
        Base::Window::SetSize(width,height);

        if (IsOpen()) {
          Resize(width,height);
          draw->SetNewSize(width,height);
        }
      }

      void Window::SetMinMaxSize(size_t minWidth,
                                 size_t minHeight,
                                 size_t maxWidth,
                                 size_t maxHeight)
      {
        Base::Window::SetMinMaxSize(minWidth,minHeight,
                                    maxWidth,maxHeight);

        if (IsOpen()) {
          XSizeHints *sHints;

          sHints=XAllocSizeHints();

          if (sHints!=NULL) {
            sHints->min_width=GetMinWidth();
            sHints->min_height=GetMinHeight();
            sHints->width=GetWidth();
            sHints->height=GetHeight();
            sHints->max_width=GetMaxWidth();
            sHints->max_height=GetMaxHeight();
            sHints->flags=PMinSize | PSize | PMaxSize;

            XSetWMProperties(display->display,
                             window,
                             NULL,
                             NULL,
                             NULL,
                             0,
                             sHints,
                             NULL,
                             NULL);

            XFree(sHints);
          }
        }
      }


      void Window::MouseGrabOn()
      {
        if (XGrabPointer(display->display,window,
                         False,
                         StructureNotifyMask|ButtonPressMask|ButtonReleaseMask|ButtonMotionMask|PointerMotionMask|PointerMotionHintMask|LeaveWindowMask|EnterWindowMask|VisibilityChangeMask|FocusChangeMask,
                         GrabModeAsync,GrabModeAsync,
                         None,
                         None,
                         CurrentTime)!=GrabSuccess) {
          std::cerr << "Cannot grab pointer" << std::endl;
        }
      }

      void Window::MouseGrabOff()
      {
        XUngrabPointer(display->display,CurrentTime);
      }

      void Window::KeyboardGrabOn()
      {
        if (XGrabKeyboard(display->display,window,
                          False,
                          GrabModeAsync,GrabModeAsync,
                          CurrentTime)!=GrabSuccess) {
          std::cerr << "Cannot grab keyboard" << std::endl;
        }
      }

      void Window::KeyboardGrabOff()
      {
        XUngrabKeyboard(display->display,CurrentTime);
      }

/*
      static Bool property_notify_predicate (::Display *xdisplay __attribute__((unused)),
                                             XEvent  *event,
                                             XPointer window_id)
      {
        if (event->xany.type == PropertyNotify &&
            //event->xproperty.window==*(&::Window)window_id &&
            event->xproperty.atom == 233)
          return True;
        else
          return False;
      }*/

      bool Window::Open(bool activate)
      {
        XSizeHints           *sHints;
        XWMHints             *wHints;
        XClassHint           *cHints;
        XSetWindowAttributes attr;
        ::Window             parent;
        bool                 xManPos;
        bool                 yManPos;
        std::string          className;
        Atom                 protocols[3];

        if (!Base::Window::Open(activate)) {
          return false;
        }

        sHints=XAllocSizeHints();
        wHints=XAllocWMHints();
        cHints=XAllocClassHint();

        if (sHints==NULL || wHints==NULL || cHints==NULL) {
          XFree(sHints);
          XFree(wHints);
          XFree(cHints);
          return false;
        }

        attr.colormap=display->colorMap;
        attr.bit_gravity=NorthWestGravity;
        attr.event_mask=KeyPressMask | KeyReleaseMask |  ExposureMask |
                        StructureNotifyMask | ButtonPressMask |  ButtonReleaseMask |
                        ButtonMotionMask | PointerMotionMask | FocusChangeMask |
                        EnterWindowMask | LeaveWindowMask | PropertyChangeMask;

        switch (type) {
        case typeTooltip:
        case typeMenu:
        case typeMenuDropdown:
        case typeMenuPopup:
        case typePopup:
        case typeSplash:
          attr.override_redirect=True;
          attr.save_under=True;
          attr.backing_store=True;
          break;
        default:
          attr.override_redirect=False;
          attr.save_under=False;
          attr.backing_store=False;
        }

        CalculateWindowPos(xManPos,yManPos);

        parent=XRootWindow(display->display,display->scrNum);

        window=XCreateWindow(display->display,
                             parent,
                             x,y,
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
          return false;
        }

        if (display->xim!=0) {
          xic=XCreateIC(display->xim,
                        XNInputStyle,XIMPreeditNothing | XIMStatusNothing,
                        XNClientWindow,window,
                        XNFocusWindow,window,
                        NULL);
        }
/*
        if (xic==NULL) {
          XFree(sHints);
          XFree(wHints);
          XFree(cHints);
          XDestroyWindow(display->display,window);
          return false;
        }*/

        /*
        Pixmap        pixmap;
        GC            gc;
        unsigned long mask;
        XGCValues     values;

        pixmap=XCreatePixmap(display->display,this->window,width,height,1);
        mask=0;
        gc=XCreateGC(display->display,pixmap,mask,&values);
        XSetForeground(display->display,gc,BlackPixel(display->display,display->scrNum));
        XFillRectangle(display->display,pixmap,gc,0,0,width,height);
        XSetForeground(display->display,gc,WhitePixel(display->display,display->scrNum));
        XFillArc(display->display,pixmap,gc,5,5,10,10,90*64,90*64); // top/left
        XFillArc(display->display,pixmap,gc,5,height-15,10,10,180*64,90*64); // bottom/left
        XFillArc(display->display,pixmap,gc,width-15,5,10,10,0*64,90*64); // top/right
        XFillArc(display->display,pixmap,gc,width-15,height-15,10,10,270*64,90*64); // bottom/right
        XFillRectangle(display->display,pixmap,gc,10,5,width-20,5); // top
        XFillRectangle(display->display,pixmap,gc,10,height-10,width-20,5); // bottom
        XFillRectangle(display->display,pixmap,gc,5,10,5,height-20); // left
        XFillRectangle(display->display,pixmap,gc,width-10,10,5,height-20); // right
        XFillRectangle(display->display,pixmap,gc,10,10,width-20,height-20); // bottom

        XShapeCombineMask(display->display,this->window,ShapeBounding,0,0,pixmap,ShapeSet);*/

        SetTitle(GetTitle());

        switch (type) {
        case typeToolbar:
          display->SetAtomProperty(this,
                                   display->atoms[Display::netWMWindowTypeAtom],
                                   display->atoms[Display::netWMWindowTypeToolbarAtom]);
          break;
        case typeMenu:
          display->SetAtomProperty(this,
                                   display->atoms[Display::netWMWindowTypeAtom],
                                   display->atoms[Display::netWMWindowTypeMenuAtom]);
          break;
        case typeMenuPopup:
          display->SetAtomProperty(this,
                                   display->atoms[Display::netWMWindowTypeAtom],
                                   display->atoms[Display::netWMWindowTypeMenuPopupAtom]);
          break;
        case typeMenuDropdown:
          display->SetAtomProperty(this,
                                   display->atoms[Display::netWMWindowTypeAtom],
                                   display->atoms[Display::netWMWindowTypeMenuDropdownAtom]);
          break;
        case typeTooltip:
        case typePopup:
          display->SetAtomProperty(this,
                                   display->atoms[Display::netWMWindowTypeAtom],
                                   display->atoms[Display::netWMWindowTypeMenuPopupAtom]);
          break;
        case typeUtility:
          display->SetAtomProperty(this,
                                   display->atoms[Display::netWMWindowTypeAtom],
                                   display->atoms[Display::netWMWindowTypeUtilityAtom]);
          break;
        case typeSplash:
          display->SetAtomProperty(this,
                                   display->atoms[Display::netWMWindowTypeAtom],
                                   display->atoms[Display::netWMWindowTypeSplashAtom]);
          break;
        case typeDialog:
          display->SetAtomProperty(this,
                                   display->atoms[Display::netWMWindowTypeAtom],
                                   display->atoms[Display::netWMWindowTypeDialogAtom]);
          break;
        case typeMain:
          display->SetAtomProperty(this,
                                   display->atoms[Display::netWMWindowTypeAtom],
                                   display->atoms[Display::netWMWindowTypeNormalAtom]);
          break;
        case typeCustom:
            if (customWindowType!=0) {
              display->SetAtomProperty(this,
                                       display->atoms[Display::netWMWindowTypeAtom],
                                       customWindowType);
            }
          break;
        case typeAuto:
          assert(false);
          break;
        }

#if defined(LUM_HAVE_LIB_HILDON)
   #if HILDON_CHECK_VERSION(2,1,0)
        if (type==typeMain) {
          size_t      stackCount=0;
          OS::Window *p=GetParent();

          while (p!=NULL) {
            stackCount++;
            p=p->GetParent();
          }

          display->SetIntegerProperty(this,
                                      XInternAtom(display->display,
                                                  "_HILDON_STACKABLE_WINDOW",
                                                  false),
                                                  stackCount);
        }

        if (GetHasMenuHint()) {
          display->SetIntegerProperty(this,
                                      XInternAtom(display->display,
                                                  "_HILDON_WM_WINDOW_MENU_INDICATOR",
                                                  false),
                                                  1);
        }

        if (GetScreenOrientationHint()==Window::screenOrientationPortraitSupported ||
            GetScreenOrientationHint()==Window::screenOrientationBothSupported) {
          display->SetCardinalProperty(this,
                                       XInternAtom(display->display,
                                                   "_HILDON_PORTRAIT_MODE_SUPPORT",
                                                   false),
                                                   1);
        }
        else {
          display->SetCardinalProperty(this,
                                       XInternAtom(display->display,
                                                   "_HILDON_PORTRAIT_MODE_SUPPORT",
                                                   false),
                                                   0);
        }

        if (type==typeMain) {
          if (GetScreenOrientationHint()==Window::screenOrientationPortraitSupported) {
            display->SetCardinalProperty(this,
                                         XInternAtom(display->display,
                                                     "_HILDON_PORTRAIT_MODE_REQUEST",
                                                     false),
                                                     1);
          }
        }
   #endif
#endif
        /*
        if (type==typeMain && XInternAtom(display->display,"_NET_REQUEST_FRAME_EXTENTS",true)!=None) {
          Atom           type;
          int            format;
          unsigned long  items;
          unsigned long  bytes;
          unsigned char* data;

          ::XEvent event;

          memset(&event,0,sizeof(event));
          event.xclient.type=ClientMessage;
          event.xclient.message_type=XInternAtom(display->display,"_NET_REQUEST_FRAME_EXTENTS",false);
          event.xclient.display=display->display;
          event.xclient.window=window;
          event.xclient.format=32;
          if (XSendEvent(display->display,XRootWindow(display->display,display->scrNum),
                         false,(SubstructureRedirectMask | SubstructureNotifyMask),&event)!=0) {
            XIfEvent(display->display,&event,property_notify_predicate,0);
            if (XGetWindowProperty(display->display,window,
                                   display->atoms[Display::netFrameExtents],
                                   0,sizeof(unsigned long)*4,
                                   false,
                                   XA_CARDINAL,
                                   &type,&format,&items,&bytes,&data)==Success) {
              if (type==XA_CARDINAL && format==32 && items==4 && data!=NULL) {
                unsigned long* frame=(unsigned long*)data;

                std::cout << "Frame: " << frame[0]<< " " << frame[1] << " " << frame[2] << " " << frame[3] << std::endl;
              }
            }
          }
        }*/

        sHints->min_width=minWidth;
        sHints->min_height=minHeight;
        sHints->width=width;
        sHints->height=height;
        sHints->max_width=maxWidth;
        sHints->max_height=maxHeight;
        sHints->flags=PMinSize | PSize| PMaxSize;

        if (xManPos || yManPos) {
          sHints->flags|=PPosition;
          sHints->x=x;
          sHints->y=y;
        }

        wHints->initial_state=NormalState;
        wHints->flags=StateHint;

        className=Lum::Base::WStringToString(display->GetAppName());
        cHints->res_name=const_cast<char*>(className.c_str());
        cHints->res_class=const_cast<char*>(className.c_str());

        if (type!=typeMenu &&
            type!=typeMenuPopup &&
            type!=typeMenuDropdown &&
            type!=typePopup) {
          OS::Window* window;

          wHints->flags|=WindowGroupHint;

          window=GetTopWindow();
          if (window!=NULL) {
            wHints->window_group=dynamic_cast<Window*>(window)->window;
            XSetTransientForHint(display->display,
                                       this->window,
                                       dynamic_cast<Window*>(window)->window);
          }
          else {
            wHints->window_group=this->window;
          }
        }

        XSetWMProperties(display->display,
                         this->window,
                         NULL,
                         NULL,
                         NULL,0,
                         sHints,
                         wHints,
                         cHints);

        XFree(sHints);
        XFree(wHints);
        XFree(cHints);

        protocols[0]=display->atoms[Display::deleteProtAtom];
        protocols[1]=display->atoms[Display::netWMPingAtom];
        protocols[2]=display->atoms[Display::netWMContextCustomAtom];

        if (XSetWMProtocols(display->display,this->window,&protocols[0],3)==0) {
          std::cerr << "Warning: Cannot set supported window manager protocols" << std::endl;
        }

        /* We mark the windows as Xdnd aware */

        display->SetAtomProperty(this,display->atoms[Display::XdndAwareAtom],
                                 XDND_VERSION);

#ifdef HAVE_GETPID
        display->SetCardinalProperty(this,display->atoms[Display::netWMPIDAtom],
                                     getpid());
#endif

        SetOpacity(GetOpacity());

        draw=driver->CreateDrawInfo(this);

        display->AddWindow(this);

        Show(activate);

        return true;
      }

      void Window::Close()
      {
        display->RemoveWindow(this);

        if (IsOpen() && (window!=0)) {
          if (IsShown()) {
            if (grabMouse) {
              GrabMouse(false);
            }
            if (grabKeyboard) {
              GrabKeyboard(false);
            }

            XUnmapWindow(display->display,window);

            XEvent e;

            do {
              XWindowEvent(display->display,window,StructureNotifyMask,&e);
            } while (e.type!=UnmapNotify);

            TriggerUnmaped();
            draw->OnUnmap();

            if (display->GetKeyboardManager()!=NULL) {
              display->GetKeyboardManager()->OnWindowFocusOut(this);
            }
          }
        }

        if (draw!=NULL) {
          delete draw;
          draw=NULL;
        }

        if (xic!=0) {
          XDestroyIC(xic);
          xic=0;
        }

        if (window!=0) {
          XDestroyWindow(display->display,window);
          window=0;
        }

        OS::Base::Window::Close();
      }

      bool Window::Show(bool activate)
      {
        assert(IsOpen());
        assert(!IsShown());

        XWMHints *wHints;

        wHints=XAllocWMHints();
        wHints->input=activate ? True : False;
        wHints->flags=InputHint;

        XSetWMHints(display->display,window,wHints);
        XFree(wHints);

        return XMapWindow(display->display,window)==Success;
      }

      bool Window::Hide()
      {
        bool result;

        assert(IsOpen());
        assert(IsShown());

        if (grabMouse) {
          GrabMouse(false);
        }
        if (grabKeyboard) {
          GrabKeyboard(false);
        }

        result=XUnmapWindow(display->display,window)==Success;

        if (display->GetKeyboardManager()!=NULL) {
          display->GetKeyboardManager()->OnWindowFocusOut(this);
        }

        return result;
      }

      bool Window::GetMousePos(int& x, int& y) const
      {
        ::Window     root;
        ::Window     child;
        unsigned int bmask;
        int          rx,ry;

        return XQueryPointer(display->display,
                             window,&root,&child,
                             &rx,&ry,&x,&y,&bmask)!=False;
      }

      void Window::Enable()
      {
        Base::Window::Enable();

        if (modalCount==0) {
          //XUndefineCursor(display->display,window);
        }
      }

      void Window::Disable()
      {
        Base::Window::Disable();

        if (modalCount==1) {
          //XDefineCursor(D.display(Display).display,w.window,D.display(Display).sleepCursor);
        }
      }

      bool Window::SendBalloonMessage(const std::wstring& text,
                                      unsigned long timeout)

      {
        std::string string=Lum::Base::WStringToUTF8(text);
        ::Window    selWin;

        selWin=::XGetSelectionOwner(display->display,XInternAtom(display->display,"_NET_SYSTEM_TRAY_S0",False));

        if (selWin==0) {
          std::cerr << "No system tray found!" << std::endl;
          return false;
        }

        ::XEvent trayEvt;

        memset(&trayEvt,0,sizeof(trayEvt));
        trayEvt.xclient.type=ClientMessage;
        trayEvt.xclient.window=window;
        trayEvt.xclient.message_type=display->atoms[Display::netSystemTrayOpcodeAtom];
        trayEvt.xclient.format=32;
        trayEvt.xclient.data.l[0]=CurrentTime;
        trayEvt.xclient.data.l[1]=1;
        trayEvt.xclient.data.l[2]=timeout;
        trayEvt.xclient.data.l[3]=string.length();
        trayEvt.xclient.data.l[4]=balloonId;

        if (XSendEvent(display->display,selWin,False,NoEventMask,&trayEvt)==0) {
          std::cerr << "Error while sending SystemtrayBeginMessage!" << std::endl;
          return false;
        }

        XSync(display->display,False);

        size_t i,j;

        memset(&trayEvt,0,sizeof(trayEvt));
        trayEvt.xclient.type=ClientMessage;
        trayEvt.xclient.window=window;
        trayEvt.xclient.message_type=display->atoms[Display::netSystemTrayMsgDataAtom];
        trayEvt.xclient.format=8;

        // Package message of 20 bytes
        j=0;
        for (i=0; i<string.length(); i++)  {
          trayEvt.xclient.data.b[j]=string[i];
          j++;

          if (j==20) {
            if (XSendEvent(display->display,selWin,False,NoEventMask,&trayEvt)==0) {
              std::cerr << "Error while sending SystemtrayMessageData!" << std::endl;
              return false;
            }
            XSync(display->display,False);

            j=0;
          }
        }

        // Send the potential rest
        if (j>0) {
          if (XSendEvent(display->display,selWin,False,NoEventMask,&trayEvt)==0) {
            std::cerr << "Error while sending SystemtrayMessageData!" << std::endl;
            return false;
          }
          XSync(display->display,False);
        }

        balloonId++;

        return true;
      }

      bool Window::SetFullScreen()
      {
        assert(window!=0);

        ::XEvent event;

        memset(&event,0,sizeof(event));
        event.xclient.type=ClientMessage;
        event.xclient.send_event=true;
        event.xclient.window=window;
        event.xclient.message_type=display->atoms[Display::netWMStateAtom];
        event.xclient.format=32;
        event.xclient.data.l[0]=1; // _NET_WM_STATE_ADD
        event.xclient.data.l[1]=display->atoms[Display::netWMStateFullScreenAtom];
        event.xclient.data.l[2]=0;
        event.xclient.data.l[3]=1;// Normal application window
        event.xclient.data.l[4]=0;
        if (XSendEvent(display->display,RootWindow(display->display,display->scrNum),False,SubstructureRedirectMask | SubstructureNotifyMask,&event)==0) {
          std::cerr << "Error while entering full screen mode" << std::endl;
          return false;
        }

        return true;
      }

      bool Window::LeaveFullScreen()
      {
        assert(window!=0);

        ::XEvent event;

        memset(&event,0,sizeof(event));
        event.xclient.type=ClientMessage;
        event.xclient.send_event=true;
        event.xclient.window=window;
        event.xclient.message_type=display->atoms[Display::netWMStateAtom];
        event.xclient.format=32;
        event.xclient.data.l[0]=0; // _NET_WM_STATE_REMOVE
        event.xclient.data.l[1]=display->atoms[Display::netWMStateFullScreenAtom];
        event.xclient.data.l[2]=0;
        event.xclient.data.l[3]=1;// Normal application window
        event.xclient.data.l[4]=0;
        if (XSendEvent(display->display,RootWindow(display->display,display->scrNum),False,SubstructureRedirectMask | SubstructureNotifyMask,&event)==0) {
          std::cerr << "Error while leaving full screen mode" << std::endl;
          return false;
        }

        return true;
      }

      bool Window::IsFullScreenInternal() const
      {
        assert(window!=0);

        Atom           atomType;
        int            atomFormat;
        int            res;
        unsigned long  atomItems;
        unsigned long  atomBytes;
        unsigned char* data=NULL;
        bool           found=false;

        res=XGetWindowProperty(display->display,window,
                               display->atoms[Display::netWMStateAtom],
                               0,1024,
                               false,
                               display->atoms[Display::atomAtom],
                               &atomType,&atomFormat,&atomItems,&atomBytes,&data);
        if (res==Success &&
            atomType==display->atoms[Display::atomAtom]) {
          Atom *value=(Atom*)data;

          for (size_t i=0; i<atomItems; i++) {
            if (value[i]==display->atoms[Display::netWMStateFullScreenAtom]) {
              found=true;
              break;
            }
          }
        }

        XFree(data);

        return found;
      }

      bool Window::IsFullScreen() const
      {
        return fullScreen;
      }

      bool Window::SetOpacity(double opacity)
      {
        if (opacity<0) {
          opacity=0;
        }
        else if (opacity>1) {
          opacity=1;
        }

        Lum::OS::Base::Window::SetOpacity(opacity);

        if (window!=0) {
          unsigned long value=(unsigned long)(opacity*0xffffffff);

          if (value==0xffffffff) {
            XDeleteProperty(display->display,window,display->atoms[Display::netWMWindowOpacity]);
          }
          else {
            XChangeProperty(display->display,window,
                            display->atoms[Display::netWMWindowOpacity],
                            XA_CARDINAL, 32,
                            PropModeReplace,
                            (const unsigned char*)&value,1);
          }
        }

        return true;
      }

      void Window::SetCustomWindowType(Atom type)
      {
        customWindowType=type;
      }

    }
  }
}
