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

#include <Lum/OS/Win32/Window.h>

#include <iostream>

#include <Lum/Private/Config.h>

#include <Lum/OS/Win32/Display.h>
#include <Lum/OS/Win32/DrawInfo.h>
#include <Lum/OS/Win32/Event.h>
#include <Lum/OS/Win32/Tray.h>

#if defined(LUM_HAVE_LIB_CAIRO)
  #include <Lum/OS/Cairo/Driver.h>
  #include <Lum/OS/Cairo/DrawInfo.h>
#endif

namespace Lum {
  namespace OS {
    namespace Win32 {

      static Window *grabWindow=NULL;

      LRESULT CALLBACK MessageHandler(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
      {
        Window *window;
/*
        std::cout << GetTickCount() << " " << wnd << " ";
        switch (msg) {
        case WM_CREATE:
          std::cout << "WM_CREATE" << std::endl;
          break;
        case WM_NCCREATE:
          std::cout << "WM_NCCREATE" << std::endl;
          break;
        case WM_DESTROY:
          std::cout << "WM_DESTROY" << std::endl;
          break;
        case WM_NCDESTROY:
          std::cout << "WM_NCDESTROY" << std::endl;
          break;
        case WM_NCCALCSIZE:
          std::cout << "WM_NCCALCSIZE" << std::endl;
          break;
        case WM_CLOSE:
          std::cout << "WM_CLOSE" << std::endl;
          break;
        case WM_PAINT:
          std::cout << "WM_PAINT" << std::endl;
          break;
        case WM_LBUTTONDOWN:
          std::cout << "WM_LBUTTONDOWN" << std::endl;
          break;
        case WM_LBUTTONUP:
          std::cout << "WM_LBUTTONUP" << std::endl;
          break;
        case WM_MBUTTONDOWN:
          std::cout << "WM_MBUTTONDOWN" << std::endl;
          break;
        case WM_MBUTTONUP:
          std::cout << "WM_MBUTTONUP" << std::endl;
          break;
        case WM_RBUTTONDOWN:
          std::cout << "WM_RBUTTONDOWN" << std::endl;
          break;
        case WM_RBUTTONUP:
          std::cout << "WM_RBUTTONUP" << std::endl;
          break;
        case WM_MOUSEMOVE:
          std::cout << "WM_MOUSEMOVE" << std::endl;
          break;
        case WM_KEYDOWN:
          std::cout << "WM_KEYDOWN" << std::endl;
          break;
        case WM_KEYUP:
          std::cout << "WM_KEYUP" << std::endl;
          break;
        case WM_SYSKEYDOWN:
          std::cout << "WM_SYSKEYDOWN" << std::endl;
          break;
        case WM_SYSKEYUP:
          std::cout << "WM_SYSKEYUP" << std::endl;
          break;
        case WM_MOUSEWHEEL:
          std::cout << "WM_MOUSEWHEEL" << std::endl;
          break;
        case WM_ACTIVATE:
          std::cout << "WM_ACTIVATE" << std::endl;
          break;
        case WM_ACTIVATEAPP:
          std::cout << "WM_ACTIVATEAPP" << std::endl;
          break;
        case WM_SHOWWINDOW:
          std::cout << "WM_SHOWWINDOW" << std::endl;
          break;
        case WM_GETMINMAXINFO:
          std::cout << "WM_GETMINMAXINFO" << std::endl;
          break;
        case WM_WINDOWPOSCHANGED:
          std::cout << "WM_WINDOWPOSCHANGED" << std::endl;
          break;
        case WM_WINDOWPOSCHANGING:
          std::cout << "WM_WINDOWPOSCHANGING" << std::endl;
          break;
        case WM_ENABLE:
          std::cout << "WM_ENABLE" << std::endl;
          break;
        case WM_NCACTIVATE:
          std::cout << "WM_NCACTIVATE" << std::endl;
          break;
        case WM_CANCELMODE:
          std::cout << "WM_CANCELMODE" << std::endl;
          break;
        case WM_SETTEXT:
          std::cout << "WM_SETTEXT" << std::endl;
          break;
        case WM_GETTEXT:
          std::cout << "WM_GETTEXT" << std::endl;
          break;
        case WM_SETFOCUS:
          std::cout << "WM_SETFOCUS" << std::endl;
          break;
        case WM_KILLFOCUS:
          std::cout << "WM_KILLFOCUS" << std::endl;
          break;
        case WM_NCPAINT:
          std::cout << "WM_NCPAINT" << std::endl;
          break;
        case WM_ERASEBKGND:
          std::cout << "WM_ERASEBKGND" << std::endl;
          break;
        case WM_SIZE:
          switch (wParam) {
          case SIZE_MAXHIDE:
            std::cout << "WM_SIZE MAXHIDE" << std::endl;
            break;
          case SIZE_MAXIMIZED:
            std::cout << "WM_SIZE MAXIMIZED" << std::endl;
            break;
          case SIZE_MAXSHOW:
            std::cout << "WM_SIZE MAXSHOW" << std::endl;
            break;
          case SIZE_MINIMIZED:
            std::cout << "WM_SIZE MINIMIZED" << std::endl;
            break;
          case SIZE_RESTORED:
            std::cout << "WM_SIZE RESTORED" << std::endl;
            break;
          default:
            std::cout << "WM_SIZE" << std::endl;
            break;
          }
          break;
        case WM_MOVE:
          std::cout << "WM_MOVE" << std::endl;
          break;
        case WM_SETCURSOR:
          std::cout << "WM_SETCURSOR" << std::endl;
          break;
        case WM_NCHITTEST:
          std::cout << "WM_NCHITTEST" << std::endl;
          break;
        case WM_NCMOUSEMOVE:
          std::cout << "WM_NCMOUSEMOVE" << std::endl;
          break;
        case WM_SYSCOMMAND:
          std::cout << "WM_SYSCOMMAND" << std::endl;
          break;
        case WM_NCLBUTTONDOWN:
          std::cout << "WM_NCLBUTTONDOWN" << std::endl;
          break;
        case WM_CAPTURECHANGED:
          std::cout << "WM_CAPTURECHANGED" << std::endl;
          break;
        case WM_MOVING:
          std::cout << "WM_MOVING" << std::endl;
          break;
        case WM_MOUSEACTIVATE:
          std::cout << "WM_MOUSEACTIVATE" << std::endl;
          break;
        case WM_ENTERSIZEMOVE:
          std::cout << "WM_ENTERSIZEMOVE" << std::endl;
          break;
        case WM_EXITSIZEMOVE:
          std::cout << "WM_EXITSIZEMOVE" << std::endl;
          break;
        case WM_CONTEXTMENU:
          std::cout << "WM_CONTEXTMENU" << std::endl;
          break;
        default:
          std::cout << msg << std::endl;
          break;
        }*/

        if (grabWindow!=NULL) {
          POINT pt;

          switch (msg) {
          case WM_MOUSEMOVE:
          case WM_LBUTTONDOWN:
          case WM_LBUTTONUP:
          case WM_LBUTTONDBLCLK:
          case WM_RBUTTONDOWN:
          case WM_RBUTTONUP:
          case WM_RBUTTONDBLCLK:
          case WM_MBUTTONDOWN:
          case WM_MBUTTONUP:
          case WM_MBUTTONDBLCLK:
            pt.x=LOWORD(lParam);
            pt.y=HIWORD(lParam);
            MapWindowPoints(wnd,grabWindow->window,&pt,1);
            return grabWindow->MessageHandler(grabWindow->window,msg,wParam,MAKELPARAM(pt.x,pt.y));

          case WM_NCMOUSEMOVE:
          case WM_NCLBUTTONDOWN:
          case WM_NCLBUTTONUP:
          case WM_NCLBUTTONDBLCLK:
          case WM_NCRBUTTONDOWN:
          case WM_NCRBUTTONUP:
          case WM_NCRBUTTONDBLCLK:
          case WM_NCMBUTTONDOWN:
          case WM_NCMBUTTONUP:
          case WM_NCMBUTTONDBLCLK:
          case WM_KEYDOWN:
          case WM_KEYUP:
          case WM_CHAR:
          case WM_DEADCHAR:
          case WM_SYSKEYDOWN:
          case WM_SYSKEYUP:
          case WM_SYSCHAR:
          case WM_SYSDEADCHAR:
            return grabWindow->MessageHandler(grabWindow->window,msg,wParam,lParam);
          }
        }

        window=dynamic_cast<Display*>(display)->GetWindow(wnd);

        if (window!=NULL) {
          return window->MessageHandler(wnd,msg,wParam,lParam);
        }
        else if (wnd==dynamic_cast<Display*>(display)->appWindow) {
          Tray *tray=dynamic_cast<Display*>(display)->GetTray(wParam);

          if (tray!=NULL) {
            ::Lum::OS::Event *event;
            MSG              winMsg;

            winMsg.lParam=lParam;
            winMsg.wParam=wParam;
            winMsg.message=msg;
            event=GetEvent(NULL,winMsg);

            tray->HandleEvent(event);
          }
        }

        return DefWindowProcA(wnd,msg,wParam,lParam);
      }

      Window::Window(OS::Display* display)
      : display(dynamic_cast<Display*>(display)),
        drawn(false),queueFocus(false),
        paintDraw(NULL),
        window(0)
      {
        // no code
      }

      Window::~Window()
      {
        // no code
      }

      /**
        Grabs the mouse cursor and keyboard.
      */
      void Window::MouseGrabOn()
      {
        assert(grabWindow==NULL);
        HWND grabHWND;

        if (parent!=NULL) {
          grabHWND=dynamic_cast<Window*>(parent)->window;

          //display->StopContextHelp();
        }
        else {
          grabHWND=dynamic_cast<Display*>(OS::display)->appWindow;
        }

        grabWindow=this;

        SetCapture(grabHWND);
        if (grabHWND!=GetCapture()) {
          std::cerr << "Cannot start mouse capture" << std::endl;
        }
      }

      /**
        Releases the grab of the mouse cursor and the keyboard.
      */
      void Window::MouseGrabOff()
      {
        assert(grabWindow!=NULL);

        if (ReleaseCapture()==0) {
          std::cerr << "Cannot stop mouse capture" << std::endl;
        }
        //display->RestartContextHelp();
        grabWindow=NULL;
      }

      void Window::SetTitle(const std::wstring& name)
      {
        OS::Base::Window::SetTitle(name);

        switch (type) {
        case typeToolbar:
        case typeUtility:
        case typeDialog:
        case typeMain:
          break;
        default:
          return;
        }

        if (window!=0) {
          std::wstring title=GetTitle();

          /* ignore */ ::SetWindowTextW(window,title.c_str());
        }
      }

      void Window::Resize(size_t /*width*/, size_t /*height*/)
      {
        /*
        ::Lum::OS::WindowImpl::Resize(width,height);
        XResizeWindow(display->display,window,width,height);
        */
      }

      bool Window::Open(bool activate)
      {
        RECT  rect;
        DWORD exFlags;
        DWORD flags;
        HWND  parent;
        bool  xManPos;
        bool  yManPos;

        if (!OS::Base::Window::Open(activate)) {
          return false;
        }

        CalculateWindowPos(xManPos,yManPos);

        switch (type) {
        case typeMenu:
        case typeMenuDropdown:
        case typeMenuPopup:
        case typePopup:
        case typeSplash:
          flags=WS_POPUP;
          break;
        case typeMain:
        default:
          if (minWidth!=maxWidth || minHeight!=maxHeight) {
            flags=WS_OVERLAPPEDWINDOW;
          }
          else {
            flags=WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
          }
        }

        switch (type) {
        case typeMain:
          exFlags=WS_EX_APPWINDOW;
          break;
        case typeToolbar:
          exFlags=WS_EX_TOOLWINDOW;
          break;
        case typeMenu:
        case typeMenuDropdown:
        case typeMenuPopup:
        case typePopup:
          exFlags=WS_EX_NOACTIVATE|WS_EX_TOPMOST;
          break;
        case typeSplash:
          exFlags=WS_EX_NOACTIVATE|WS_EX_TOPMOST; // TODO: TOPMOST OK?
          break;
        default:
          exFlags=0;
        }

        rect.left=x;
        rect.top=y;
        rect.right=x+width-1;
        rect.bottom=y+height-1;
        /* ignore */::AdjustWindowRectEx(&rect,flags,false,0);

        wOff=x-rect.left;
        hOff=y-rect.top;
        wDelta=(rect.right-rect.left+1)-width;
        hDelta=(rect.bottom-rect.top+1)-height;

        if (xManPos) {
          x=rect.left;
        }
        else {
          x=CW_USEDEFAULT;
        }

        if (yManPos) {
          y=rect.top;
        }
        else {
          y=CW_USEDEFAULT;
        }

        if (this->parent!=NULL) {
          parent=dynamic_cast<Window*>(this->parent)->window;
        }
        else {
          parent=0;
        }

        dynamic_cast<Display*>(display)->AddWindow(this);

        window=::CreateWindowExA(exFlags,
                                 (LPCSTR)wClassAtom,
                                 NULL,
                                 flags,
                                 x,y,
                                 rect.right-rect.left+1,
                                 rect.bottom-rect.top+1,
                                 parent,0,dynamic_cast<Display*>(display)->instance,0);

        if (window==0) {
          std::cerr << "Cannot create window: " << GetLastError() << std::endl;
          return false;
        }

        SetTitle(GetTitle());
        
        {
          HRGN region;

          region=CreateRectRgn(0,0,rect.right-rect.left+1,rect.bottom-rect.top+1);
          /* ignore */ SelectClipRgn(GetDC(window),region);
        }
        
        if ((type==typeMenu ||
             type==typeMenuDropdown ||
             type==typeMenuPopup ||
             type==typePopup) &&
            parent!=NULL) {
          /* ignore */ ::ShowWindow(window,SW_SHOWNOACTIVATE);
          if (this->parent!=NULL) {
            dynamic_cast<Window*>(this->parent)->TriggerGrabCancel();
          }
        }
        else if (type==typeSplash) {
          /* ignore */ ::ShowWindow(window,SW_SHOWNOACTIVATE);
          if (this->parent!=NULL) {
            dynamic_cast<Window*>(this->parent)->TriggerGrabCancel();
          }
        }
        else {
          /* ignore */ ::ShowWindow(window,activate ? SW_SHOWNORMAL : SW_SHOWNOACTIVATE);
        }

        return true;
      }

      void Window::Close()
      {

        if (IsOpen() && window!=0) {
          HWND hwnd;

          hwnd=window;

          if (grabMouse) {
            MouseGrabOff();
          }
          if (grabKeyboard) {
            KeyboardGrabOff();
          }

          /*
           DestroyWindow will send a deactivate message, which for
           pop-up menus in turn invokes Close. To avoid recursivness,
           we clear the window handle before closing the window.
          */

          window=0;

          TriggerUnmaped();
          /* ignore */ DestroyWindow(hwnd);

          /*
            Destroying the active window SEEMS to NOT always activate
            the parent. Perhaps there is a reason for this. For now,
            we explicitly activate the parent before closing.
          */
          if (parent!=NULL) {
            switch (type) {
            case typeMenu:
            case typeMenuDropdown:
            case typeMenuPopup:
            case typePopup:
              break;
            default:
              /*ignore */ ::SetActiveWindow(dynamic_cast<Window*>(parent)->window);
              break;
            }
          }
        }

        dynamic_cast<Display*>(display)->RemoveWindow(this);

        OS::Base::Window::Close();
      }

      bool Window::Show(bool activate)
      {
        ::ShowWindow(window,activate ? SW_SHOW : SW_SHOWNA);
        return true;
      }

      bool Window::Hide()
      {
        ::ShowWindow(window,SW_HIDE);
        return true;
      }

      bool Window::GetMousePos(int& x, int& y) const
      {
        POINT cursorPos;
        RECT  rect;

        if (::GetCursorPos(&cursorPos)) {
          if (::GetWindowRect(window,&rect)) {
            x=cursorPos.x-rect.left-wOff;
            y=cursorPos.y-rect.top-hOff;
            return true;
          }
        }

        return false;
      }

      void Window::Enable()
      {
        OS::Base::Window::Enable();


        if (modalCount==0) {
          ::EnableWindow(window,1);
        }
      }

      void Window::Disable()
      {
        OS::Base::Window::Disable();

        if (modalCount==1) {
          ::EnableWindow(window,0);
        }
      }

      void Window::TriggerDraw(int x, int y, size_t width, size_t height)
      {
        RECT rect;
        
        rect.left=x;
        rect.top=y;
        rect.right=x+width;
        rect.bottom=y+height;
        
        ::InvalidateRect(window,&rect,false);
      }
      
      bool Window::SendBalloonMessage(const std::wstring& /*text*/,
                                      unsigned long /*timeout*/)
      {
        return true;
      }

      void Window::OnPaint()
      {
        HDC          dc;
        RECT         updateRect;
        PAINTSTRUCT  ps;
        OS::Bitmap   *bitmap=NULL;
        OS::DrawInfo *bitmapDraw=NULL;
        DrawMsg      msg;

        if (IsShown() && !onOpened) {
          TriggerPreOpened();
        }
      
        if (::GetUpdateRect(window,&updateRect,false)==0) {
          return;
        }
      
        dc=::BeginPaint(window,&ps);
    
#if defined(CAIRO_HAS_WIN32_SURFACE)
        if (driver->GetDriverName()==L"CairoWin32") {
          paintDraw=new Cairo::Win32DrawInfo(this,dc);
        }
        else {
          paintDraw=new DrawInfo(this,dc);
        }
#else          
        paintDraw=new DrawInfo(this,dc);
#endif  

        msg.x=ps.rcPaint.left;
        msg.y=ps.rcPaint.top;
        msg.width=ps.rcPaint.right-ps.rcPaint.left;
        msg.height=ps.rcPaint.bottom-ps.rcPaint.top;

        bitmap=driver->CreateBitmap(GetWidth(),GetHeight());

        if (bitmap!=NULL) {
          bitmapDraw=bitmap->GetDrawInfo();

          if (bitmapDraw==NULL) {
            delete bitmap;
            bitmap=NULL;
          }
          else {
            paintDraw->CopyToBitmap(msg.x,msg.y,msg.width,msg.height,msg.x,msg.y,bitmap);
          }
        }

        if (bitmapDraw!=NULL) {
          msg.draw=bitmapDraw;
        }
        else {
          msg.draw=paintDraw;
        }

        paintDraw->PushClip(msg.x,msg.y,msg.width,msg.height);
        
        drawAction->Trigger(msg);
      
        paintDraw->PopClip();
      
        if (bitmap!=NULL) {
          paintDraw->CopyFromBitmap(bitmap,msg.x,msg.y,msg.width,msg.height,msg.x,msg.y);

          delete bitmap;
          bitmap=NULL;
          bitmapDraw=NULL;
        }
      
        delete paintDraw;
        paintDraw=NULL;
    
        /* ignore */ ::EndPaint(window,&ps);

        /*
          HACK: If we received focus before our window is
          drawn, send one focus message after first drawing
          the window
         */
        drawn=true;
        
        if (IsShown() && !onOpened) {
          TriggerOpened();
        }

        // See WM_SETFOCUS handling
        if (!HasFocus() &&
            (queueFocus || grabWindow==this)) {
          queueFocus=false;
          TriggerFocusIn();
        }
      }
      
      LRESULT Window::MessageHandler(HWND wnd,UINT msg, WPARAM wParam, LPARAM lParam)
      {
        OS::EventRef event;
        MSG          winMsg;
        WINDOWPOS    *pos;
        MINMAXINFO   *mmInfo;

        switch (msg) {
        case WM_ERASEBKGND:
          return 0;
        case WM_CLOSE:
          TriggerClosed();
          return 0;
        case WM_PAINT:
          OnPaint();
          //break;
          return 0;
        case WM_SIZE:
          if (wParam==SIZE_MINIMIZED) {
            drawn=false;
            if (IsShown()) {
              TriggerUnmaped();
            }
          }
          else if (wParam==SIZE_RESTORED) {
            if (!IsShown()) {
              TriggerMaped();
            }
          }
          break;
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MOUSEMOVE:
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_MOUSEWHEEL:
          winMsg.lParam=lParam;
          winMsg.wParam=wParam;
          winMsg.message=msg;
          event=GetEvent(this,winMsg);
          if (event.Valid()) {
            do {
              event->reUse=false;
              /* ignore */ HandleEvent(event);
            } while (event->reUse);
          }

          if (msg==WM_MOUSEMOVE && type==typeMenu && parent==NULL) {
            // Hack: If not done, we have a hourglass cursor for this special popup case!?
            ::SetCursor(LoadCursor(0,IDC_ARROW));
          }

          break;
        case WM_KILLFOCUS:
          TriggerFocusOut();
          break;
        case WM_SETFOCUS:
          /* Window might send SETFOCUS before the window is drawn. In this case we cache the
             event to get internal focus handling and drawing right
           */
          if (drawn) {
            TriggerFocusIn();
          }
          else {
            queueFocus=true;
          }
          break;
        case WM_SHOWWINDOW:
          if (wParam==0) {
            if (IsShown()) {
              TriggerUnmaped();
            }
          }
          else {
            if (!IsShown()) {
              TriggerMaped();
              if (grabMouse) {
                MouseGrabOn();
              }
            }
          }
          break;
        case WM_GETMINMAXINFO:
          /* TODO: rethink everything */
          mmInfo=(MINMAXINFO*)lParam;
          mmInfo->ptMinTrackSize.x=minWidth+wDelta;
          mmInfo->ptMinTrackSize.y=minHeight+hDelta;
          if (maxWidth>=display->GetScreenWidth()) {
            mmInfo->ptMaxTrackSize.x=GetSystemMetrics(SM_CXFULLSCREEN)+wDelta;
          }
          else {
            mmInfo->ptMaxTrackSize.x=maxWidth+wDelta;
          }
          if (maxHeight>=display->GetScreenHeight()) {
            mmInfo->ptMaxTrackSize.y=GetSystemMetrics(SM_CYFULLSCREEN)+hDelta;
          }
          else {
            mmInfo->ptMaxTrackSize.y=maxHeight+hDelta;
          }
          break;
        case WM_WINDOWPOSCHANGED:
          pos=(WINDOWPOS*)lParam;
          x=pos->x+wOff;
          y=pos->y+hOff;
          if (pos->cx-wDelta!=(int)width || pos->cy-hDelta!=(int)height) {
            width=pos->cx-wDelta; // TODO: check width/height/clip rect
            height=pos->cy-hDelta;
            TriggerResize(width,height);
          }
          break;
        case WM_MOUSEACTIVATE:
          //We should not get activated
          if ((type==typeMenu ||
               type==typeMenuDropdown ||
               type==typeMenuPopup ||
               type==typePopup) && parent!=NULL) {
            return MA_NOACTIVATE;
          }
          break;
        default:
          break;
        }

        return ::DefWindowProcA(wnd,msg,wParam,lParam);
      }
    }
  }
}
