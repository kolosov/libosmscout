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

#include <Lum/OS/Win32/Tray.h>

#include <Lum/OS/Win32/Bitmap.h>
#include <Lum/OS/Win32/Display.h>
#include <Lum/OS/Win32/Event.h>
#include <iostream>
namespace Lum {
  namespace OS {
    namespace Win32 {

      static HICON BitmapToIcon(HDC hdc, HBITMAP hBitmap, size_t width, size_t height)
      {
        ICONINFO ii;
        HICON    hIcon;
        HDC      hdc1,hdc2;

        hdc1=::CreateCompatibleDC(hdc); // Drawing context for mask
        hdc2=::CreateCompatibleDC(hdc); // Drawing context for image data

        // Fill icon structure
        ii.fIcon   =TRUE;
        ii.xHotspot=0;
        ii.yHotspot=0;
        ii.hbmMask =(HBITMAP)::SelectObject(hdc1,::CreateBitmap(width,height,1,1,NULL));
        ii.hbmColor=(HBITMAP)::SelectObject(hdc2,::CopyImage(hBitmap,IMAGE_BITMAP,width,height,0));

        // Make the mask fill to zero, aka not bits currently masked
        HBRUSH brush;
        RECT   rect;

        brush=::CreateSolidBrush(RGB(0,0,0));

        rect.left=0;
        rect.top=0;
        rect.right=width;
        rect.bottom=height;
        /* ignore */ FillRect(hdc1,&rect,brush);

        ii.hbmColor=(HBITMAP)::SelectObject(hdc2,ii.hbmColor);
        ii.hbmMask =(HBITMAP)::SelectObject(hdc1,ii.hbmMask);

        // Clean up
        ::DeleteDC(hdc2);
        ::DeleteDC(hdc1);

        // Create icon
        hIcon=::CreateIconIndirect(&ii);

        // More cleanup
        ::DeleteObject((HGDIOBJ)ii.hbmMask);
        ::DeleteObject((HGDIOBJ)ii.hbmColor);

        return hIcon;
      }

      Tray::Tray()
      : status (statusHidden),
        eventAction(new ::Lum::Model::Action),
        openedAction(new ::Lum::Model::Action),
        relWidth(100),relHeight(100),
        width(0),height(0),
        bitmap(NULL),
        menu(NULL)
      {
        ::ZeroMemory(&nid,sizeof(NOTIFYICONDATA));
        nid.cbSize=sizeof(NOTIFYICONDATA);
        nid.hWnd=dynamic_cast<Display*>(OS::display)->appWindow;
        nid.uID=(UINT)this;
        nid.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP;
        nid.uCallbackMessage=WM_USER+101;
        nid.hIcon=LoadIcon(NULL,IDI_INFORMATION);
        strcpy(nid.szTip,"Illumination system tray tool tip");
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

      OS::Bitmap* Tray::GetBitmap() const
      {
        return bitmap;
      }

      void Tray::SetRelativeSize(size_t width, size_t height)
      {
        assert(width>0 && height>0);

        relWidth=width;
        relHeight=height;
      }

      void Tray::SetMenu(OS::Window* window)
      {
        menu=window;
      }

      bool Tray::Show()
      {
        assert(status==statusHidden);

        width=::GetSystemMetrics(SM_CXSMICON);
        height=::GetSystemMetrics(SM_CYSMICON);

        // TODO: Check return value
        ::Shell_NotifyIcon(NIM_ADD,&nid);

        status=statusShown;

        bitmap=OS::driver->CreateBitmap(width,height);
        openedAction->Trigger();

        dynamic_cast<OS::Win32::Display*>(display)->AddTray(this);

        return true;
      }

      bool Tray::Hide()
      {
        assert(status==statusOpening || status==statusShown);

        status=statusHidden;

        delete bitmap;
        ::Shell_NotifyIcon(NIM_DELETE,&nid);

        dynamic_cast<OS::Win32::Display*>(display)->RemoveTray(this);

        return true;
      }

      void Tray::Redraw()
      {
        assert(status==statusShown);

        nid.uFlags=NIF_ICON;
        nid.hIcon=BitmapToIcon(dynamic_cast<Display*>(display)->hdc,dynamic_cast<Bitmap*>(bitmap)->bitmap,width,height);
        ::Shell_NotifyIcon(NIM_MODIFY,&nid);
      }

      void Tray::SetTitle(const std::wstring& title)
      {
        this->title=title;

        // TODO
      }

      ::Lum::Model::Action* Tray::GetEventAction() const
      {
        return eventAction.Get();
      }

      ::Lum::Model::Action* Tray::GetOpenedAction() const
      {
        return openedAction.Get();
      }

      void Tray::HandleEvent(::Lum::OS::Event* event)
      {
        Event *wEvent;

        wEvent=dynamic_cast<Event*>(event);
        switch (wEvent->event.lParam) {
        case WM_CONTEXTMENU:
        case WM_RBUTTONDOWN:
          if (menu!=NULL) {
            int x,y;

            if (display->GetMousePos(x,y)) {
              SetForegroundWindow(dynamic_cast<Display*>(::Lum::OS::display)->appWindow);
              menu->SetPos(x,y);
              /* ignore */ menu->Open();
            }
          }
          break;
        default:
          break;
        }
      }
    }
  }
}
