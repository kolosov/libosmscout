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

#include <Lum/OS/X11/SubWindow.h>

#include <Lum/OS/Cairo/DrawInfo.h>

#include <Lum/OS/X11/Display.h>
#include <Lum/OS/X11/Window.h>

namespace Lum {
  namespace OS {
    namespace X11 {

      SubWindow::SubWindow()
      : done(false),raised(false),destroyOnCleanup(true),
        window(0)
      {
        // no code
      }

      SubWindow::~SubWindow()
      {
        if (drawInfo!=NULL) {
          delete drawInfo;
        }

        if (window!=0) {
          if (destroyOnCleanup) {
            ::XDestroyWindow(dynamic_cast<Display*>(display)->display,window);
          }
          window=0;
        }
      }

      void SubWindow::SetDestroyOnCleanup(bool destroyOnCleanup)
      {
        this->destroyOnCleanup=destroyOnCleanup;
      }

      void SubWindow::Layout()
      {
        if (window==0 && !done) {
          window=::XCreateSimpleWindow(dynamic_cast<Display*>(display)->display,
                                       dynamic_cast<Window*>(GetWindow())->GetDrawable(),
                                       0,0,
                                       width,height,
                                       0,0,0);
          //drawInfo=new OS::Cairo::DrawInfo(dynamic_cast<Display*>(display)->display,
          //                                 window);
          done=true;
        }

        if (window!=0) {
          ::XMoveResizeWindow(dynamic_cast<Display*>(display)->display,
                              window,
                              x,y,width,height);
        }

        Lum::Object::Layout();
      }

      void SubWindow::Draw(OS::DrawInfo* draw,
                           int x, int y, size_t w, size_t h)
      {
        Lum::Object::Draw(drawInfo,x,y,w,h);

        if (!raised && window!=0) {
          ::XMapWindow(dynamic_cast<Display*>(display)->display,window);
          raised=true;
        }
      }

      void SubWindow::Hide()
      {
        if (raised && window!=0) {
          ::XUnmapWindow(dynamic_cast<Display*>(display)->display,window);
          raised=false;
        }
      }
    }
  }
}
