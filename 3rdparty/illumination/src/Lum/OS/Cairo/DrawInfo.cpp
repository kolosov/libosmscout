/*
  This source is part of the Illumination library
  Copyright (C) 2007  Tim Teulings

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

#include <Lum/OS/Cairo/DrawInfo.h>

#include <math.h>

#if defined(CAIRO_HAS_WIN32_SURFACE)
  #include <Lum/OS/Win32/Display.h>
  #include <Lum/OS/Win32/Window.h>
#endif

#if defined(CAIRO_HAS_QUARTZ_SURFACE)
  #include <Lum/OS/Carbon/Display.h>
  #include <Lum/OS/Carbon/Window.h>
#endif

#if defined(CAIRO_HAS_XLIB_SURFACE)
  #include <Lum/OS/X11/Display.h>
  #include <Lum/OS/X11/Window.h>
#endif

#include <Lum/Base/String.h>

#include <Lum/OS/Cairo/Bitmap.h>
#include <Lum/OS/Cairo/Font.h>
#include <iostream>
namespace Lum {
  namespace OS {
    namespace Cairo {

      /**
        Initialize an instance of the  DrawInfo class.
      */
      DrawInfo::DrawInfo(OS::Window* window)
      : OS::Base::DrawInfo(window),
        clipPushCount(0),
        colorPushCount(0),
        dashPushCount(0)
      {
        // no code
      }

      DrawInfo::DrawInfo(OS::Bitmap* bitmap)
      : OS::Base::DrawInfo(bitmap),
        clipPushCount(0),
        colorPushCount(0),
        dashPushCount(0)
      {
        // no code
      }

      /**
        Deinitializes the drawInfo
      */
      DrawInfo::~DrawInfo()
      {
        assert(clipPushCount==0);
        assert(fontStack.size()==0);
        assert(colorPushCount==0);
        assert(dashPushCount==0);
      }

      void DrawInfo::SetDefaults()
      {
        cairo_set_line_width(cairo,1);
        cairo_set_line_cap(cairo,CAIRO_LINE_CAP_SQUARE);
        cairo_set_line_join(cairo,CAIRO_LINE_JOIN_MITER);
      }

      void DrawInfo::PushClip(size_t x, size_t y, size_t width, size_t height)
      {
        clipPushCount++;

        cairo_save(cairo);
        cairo_rectangle(cairo,x,y,width,height);
        cairo_clip(cairo);
      }

      void DrawInfo::PushClipBegin(size_t x, size_t y, size_t width, size_t height)
      {
        clipPushCount++;

        cairo_save(cairo);
        cairo_rectangle(cairo,x,y,width,height);
      }

      void DrawInfo::AddClipRegion(size_t x, size_t y, size_t width, size_t height)
      {
        cairo_rectangle(cairo,x,y,width,height);
      }

      void DrawInfo::SubClipRegion(size_t x, size_t y, size_t width, size_t height)
      {
        cairo_rectangle(cairo,x+width,y,-width,height);
      }

      void DrawInfo::PushClipEnd()
      {
        cairo_clip(cairo);
      }

      void DrawInfo::PopClip()
      {
        assert(clipPushCount>0);

        clipPushCount--;

        cairo_restore(cairo);
      }

      void DrawInfo::ReinstallClip()
      {
        // no code
      }

      void DrawInfo::PushFont(OS::Font *font, unsigned long style)
      {
        assert(font!=NULL);

        if (fontStack.size()>0 &&
            fontStack.top().font.Get()==font &&
            fontStack.top().style==style) {
          fontStack.top().count++;
        }
        else {
          FontEntry       entry;
          Font::FontStyle *s;

          entry.font=font;
          entry.style=style;
          entry.count=1;

          fontStack.push(entry);

          s=dynamic_cast<Font*>(font)->GetStyle(style);

          assert(s!=NULL);
          assert(s->fontScaled!=NULL);

          cairo_save(cairo);
          cairo_set_scaled_font(cairo,s->fontScaled);
        }
      }

      void DrawInfo::PopFont()
      {
        assert(fontStack.size()>0);

        if (fontStack.top().count>1) {
          fontStack.top().count--;
        }
        else {
          fontStack.pop();

          cairo_restore(cairo);
        }
      }

      void DrawInfo::DrawString(int x, int y,
                                const std::wstring& text,
                                size_t start,
                                size_t length)
      {
        assert(fontStack.size()>0);
        assert(colorPushCount>0);

        std::wstring tmp=text.substr(start,length);

        cairo_move_to(cairo,x,y);
        cairo_show_text(cairo,Lum::Base::WStringToUTF8(tmp).c_str());

        if (OS::Font::underlined & fontStack.top().style) {
          FontExtent extent;

          fontStack.top().font->StringExtent(tmp,extent,fontStack.top().style);

          DrawLine(x,y,x+extent.width-extent.left-extent.right,y);
          cairo_stroke(cairo);
        }
      }

      void DrawInfo::DrawFillString(int x, int y, const std::wstring& text,
                                    OS::Color /*background*/)
      {
        assert(fontStack.size()>0);
        assert(colorPushCount>0);

        std::string b;

        b=Lum::Base::WStringToUTF8(text);

        cairo_move_to(cairo,x,y);
        // TODO
        cairo_show_text(cairo,b.c_str());
      }

      void DrawInfo::PushForeground(OS::Color color)
      {
        double r,g,b;

        colorPushCount++;

        cairo_save(cairo);

#if defined(CAIRO_HAS_WIN32_SURFACE)
        r=GetRValue(color)/255.0;
        g=GetGValue(color)/255.0;
        b=GetBValue(color)/255.0;
#elif defined(CAIRO_HAS_QUARTZ_SURFACE)
        r=(color/(256*256) % 256);
        g=(color/(256    ) % 256);
        b=(color/(1      ) % 256);

        r=r/255;
        g=g/255;
        b=b/255;
#elif defined(CAIRO_HAS_XLIB_SURFACE)
        OS::X11::Display *display=dynamic_cast<OS::X11::Display*>(OS::display);

        if (display->visual->bits_per_rgb==8 &&
            display->visual->red_mask==0xFF0000 &&
            display->visual->green_mask==0xFF00 &&
            display->visual->blue_mask==0xFF) {
          r=((color & 0XFF0000) >> 16)/255.0;
          g=((color & 0X00FF00) >>  8)/255.0;
          b=((color & 0X0000FF) >>  0)/255.0;
        }
        else {
          XColor tmp;

          tmp.pixel=color;
          tmp.red=0;
          tmp.green=0;
          tmp.blue=0;
          tmp.flags=0;

          XQueryColor(display->display,
                      display->colorMap,
                      &tmp);

          r=((double)tmp.red)/((double)std::numeric_limits<unsigned short>::max());
          g=((double)tmp.green)/((double)std::numeric_limits<unsigned short>::max());
          b=((double)tmp.blue)/((double)std::numeric_limits<unsigned short>::max());
        }
#endif

        cairo_set_source_rgb(cairo,r,g,b);
      }

      void DrawInfo::PopForeground()
      {
        assert(colorPushCount>0);

        colorPushCount--;

        cairo_restore(cairo);
      }

      void DrawInfo::PushDrawMode(OS::DrawInfo::DrawMode /*mode*/)
      {
        // TODO
      }

      void DrawInfo::PopDrawMode()
      {
        // TODO
      }

      void DrawInfo::PushPen(int size, Pen pen)
      {
        cairo_save(cairo);

        switch (pen) {
        case penNormal:
          cairo_set_line_cap(cairo,CAIRO_LINE_CAP_SQUARE);
          break;
        case penRound:
          cairo_set_line_cap(cairo,CAIRO_LINE_CAP_ROUND);
          break;
        }

        cairo_set_line_width(cairo,size==0 ? 1 : size);
      }

      void DrawInfo::PopPen()
      {
        cairo_restore(cairo);
      }

      void DrawInfo::PushDash(const char* dashList, size_t len, DashMode /*mode*/)
      {
        assert(dashList!=NULL && len>0);

        dashPushCount++;

        cairo_save(cairo);

        double *dash;

        dash=new double[len];

        for (size_t x=0; x<len; x++) {
          dash[x]=dashList[x];
        }

        cairo_set_dash(cairo,dash,len,0);

        delete [] dash;
      }

      void DrawInfo::PopDash()
      {
        assert(dashPushCount>0);

        dashPushCount--;

        cairo_restore(cairo);
      }

      void DrawInfo::PushPattern(const char* /*pattern*/, int /*width*/, int /*height*/, int /*mode*/)
      {/*
        Pattern pat;


        pat=new Pattern;
        pat.pixMap=X11.XCreateBitmapFromData(D.display(Display).display,X11.XRootWindow(D.display(Display).display,D.display(Display).scrNum),pattern,width,height);
        if (pat.pixMap==0) {
          Err.String("Cannot create pimap!");
          Err.Ln;
        }
        pat.mode=mode;
        X11.XSetStipple(D.display(Display).display,d.cairo,pat.pixMap);
        CASEmodeOFD.fgPattern:X11.XSetFillStyle(D.display(Display).display,d.gc,X11.FillStippled);|D.fbPattern:X11.XSetFillStyle(D.display(Display).display,d.gc,X11.FillOpaqueStippled);
      }
      else {
        Err.String("Unsuported patternMode!");
        Err.Ln;*/
      }

      void DrawInfo::PopPattern()
      {
       /*
        X11.XFreePixmap(D.display(Display).display,d.patternStack.pixMap);
        d.patternStack=d.patternStack.next;
        if (d.patternStack!=NULL) {
          CASEd.patternStack.modeOFD.fgPattern:X11.XSetFillStyle(D.display(Display).display,d.gc,X11.FillStippled);|D.fbPattern:X11.XSetFillStyle(D.display(Display).display,d.gc,X11.FillOpaqueStippled);
        }
        else {
          Err.String("Unsuported patternMode!");
          Err.Ln;
        }
        X11.XSetStipple(D.display(Display).display,d.gc,d.patternStack.pixMap);
      }
      else {
        X11.XSetFillStyle(D.display(Display).display,d.gc,X11.FillSolid);*/
      }

      void DrawInfo::PushBitmap(OS::Bitmap* /*bitmap*/, BitmapMode /*mode*/)
      {
        // TODO
      }

      void DrawInfo::PopBitmap()
      {
        // TODO
      }

      void DrawInfo::DrawPoint(int x, int y)
      {
        cairo_save(cairo);
        cairo_set_line_width(cairo,1);
        cairo_arc(cairo,x+0.5,y+0.5,1.0,0,360*(M_PI/180.0));
        cairo_fill(cairo);
        cairo_restore(cairo);
      }

      void DrawInfo::DrawPointWithColor(int x, int y, OS::Color color)
      {
        PushForeground(color);
        PushPen(1,penNormal);
        cairo_arc(cairo,x+0.5,y+0.5,1.0,0,360*(M_PI/180.0));
        cairo_fill(cairo);
        PopPen();
        PopForeground();
      }

      void DrawInfo::DrawLine(int x1, int y1, int x2, int y2)
      {
        cairo_move_to(cairo,x1+0.5,y1+0.5);
        cairo_line_to(cairo,x2+0.5,y2+0.5);
        cairo_stroke(cairo);
      }

      void DrawInfo::DrawRectangle(int x, int y, int width, int height)
      {
        if (width==0 || height==0) {
          return;
        }

        double penSize=cairo_get_line_width(cairo);

        cairo_rectangle(cairo,x+penSize/2,y+penSize/2,width-penSize,height-penSize);
        cairo_stroke(cairo);
      }

      void DrawInfo::FillRectangle(int x, int y, int width, int height)
      {
        if (width==0 || height==0) {
          return;
        }

        cairo_rectangle(cairo,x,y,width,height);
        cairo_fill(cairo);
      }

      void DrawInfo::DrawArc(int x, int y, int width, int height, int angle1, int angle2)
      {
        cairo_save(cairo);
        cairo_translate(cairo,x+0.5+width/2.0,y+0.5+height/2.0);
        cairo_scale(cairo,1/2.0,width/(2.0*height));
        cairo_new_path(cairo);
        cairo_arc(cairo,0.0,0.0,width,
                  -(angle1+angle2)*(M_PI/180.0)/64,
                  -angle1*(M_PI/180.0)/64);
        cairo_stroke(cairo);
        cairo_restore(cairo);
      }

      void DrawInfo::FillArc(int x, int y, int width, int height, int angle1, int angle2)
      {
        cairo_save(cairo);
        cairo_translate(cairo,x+0.5+width/2.0,y+0.5+height/2.0);
        cairo_scale(cairo,1/2.0,width/(2.0*height));
        cairo_move_to(cairo,0.0,0.0);
        if (angle2<0) {
          cairo_arc_negative(cairo,0.0,0.0,width,
                             -(angle1+angle2)*(M_PI/180.0)/64,
                             -angle1*(M_PI/180.0)/64);
        }
        else {
          cairo_arc(cairo,0.0,0.0,width,
                    -(angle1+angle2)*(M_PI/180.0)/64,
                    -angle1*(M_PI/180.0)/64);
        }
        cairo_close_path(cairo);
        cairo_fill(cairo);
        cairo_restore(cairo);
      }

      void DrawInfo::FillPolygon(const Point points[], size_t count)
      {
        cairo_new_path(cairo);
        cairo_move_to(cairo,points[0].x,points[0].y);
        for (size_t x=1; x<count; x++) {
          cairo_line_to(cairo,points[x].x,points[x].y);
        }
        cairo_close_path(cairo);
        cairo_fill(cairo);
      }

      void DrawInfo::CopyArea(int sX, int sY, int width, int height, int dX, int dY)
      {
        cairo_save(cairo);
        cairo_set_source_surface(cairo,surface,dX-sX,dY-sY);
        cairo_rectangle(cairo,dX,dY,width,height);
        cairo_fill(cairo);
        cairo_restore(cairo);
      }

      /**
        Copy the rectangle sY,sY-width,height
        to the current DrawInfo starting at position dX,dY.
      */
      void DrawInfo::CopyFromBitmap(OS::Bitmap* bitmap,
                                    int sX, int sY,
                                    int width, int height,
                                    int dX, int dY)
      {
        assert(bitmap!=NULL);
	
        DrawInfo* draw;

        draw=dynamic_cast<DrawInfo*>(bitmap->GetDrawInfo());

        cairo_save(cairo);
        cairo_set_source_surface(cairo,draw->surface,dX-sX,dY-sY);
        cairo_rectangle(cairo,dX,dY,width,height);
        cairo_fill(cairo);
        cairo_restore(cairo);
      }

      void DrawInfo::CopyToBitmap(int sX, int sY,
                                  int width, int height,
                                  int dX, int dY,
                                  OS::Bitmap* bitmap)
      {
        assert(bitmap!=NULL);

        DrawInfo* draw;

        draw=dynamic_cast<DrawInfo*>(bitmap->GetDrawInfo());

        cairo_save(draw->cairo);
        cairo_set_source_surface(draw->cairo,surface,dX-sX,dY-sY);
        cairo_rectangle(draw->cairo,dX,dY,width,height);
        cairo_fill(draw->cairo);
        cairo_restore(draw->cairo);
      }

#if defined(CAIRO_HAS_WIN32_SURFACE)
      Win32DrawInfo::Win32DrawInfo(OS::Window* window)
      : DrawInfo(window)
      {
        dc=GetDC(dynamic_cast<OS::Win32::Window*>(window)->window);

        surface=cairo_win32_surface_create(dc);

        deleteSurface=true;
        cairo=cairo_create(surface);

        SetDefaults();
      }

      Win32DrawInfo::Win32DrawInfo(OS::Window* window, HDC dc)
      : DrawInfo(bitmap)
      {
        this->dc=dc;

        surface=cairo_win32_surface_create(dc);

        deleteSurface=true;
        cairo=cairo_create(surface);

        SetDefaults();
     }

      Win32DrawInfo::Win32DrawInfo(OS::Bitmap* bitmap)
      : DrawInfo(bitmap)
      {
        surface=dynamic_cast<Bitmap*>(bitmap)->surface;
        deleteSurface=false;

        dc=dynamic_cast<Bitmap*>(bitmap)->dc;

        cairo=cairo_create(surface);

        SetDefaults();
      }

      /**
        Deinitializes the drawInfo
      */
      Win32DrawInfo::~Win32DrawInfo()
      {
        cairo_destroy(cairo);
        if (deleteSurface) {
          cairo_surface_destroy(surface);
        }
      }

      void Win32DrawInfo::GetRGBValue(Color color, double &r, double &g, double &b)
      {
        assert(false);
      }
#endif

#if defined(CAIRO_HAS_XLIB_SURFACE)
      X11DrawInfo::X11DrawInfo(OS::Window* window)
      : DrawInfo(window)
      {
        display=dynamic_cast<OS::X11::Window*>(window)->GetDisplay();
        drawable=dynamic_cast<OS::X11::Window*>(window)->GetDrawable();

        surface=cairo_xlib_surface_create(display,
                                          drawable,
                                          dynamic_cast<OS::X11::Display*>(OS::display)->visual,
                                          window->GetWidth(),window->GetHeight());

        deleteSurface=true;
        cairo=cairo_create(surface);

        SetDefaults();
      }

      X11DrawInfo::X11DrawInfo(OS::Bitmap* bitmap)
      : DrawInfo(bitmap)
      {
        surface=dynamic_cast<Bitmap*>(bitmap)->surface;
        deleteSurface=false;

        display=dynamic_cast<Bitmap*>(bitmap)->display;
        drawable=dynamic_cast<Bitmap*>(bitmap)->drawable;

        cairo=cairo_create(surface);

        SetDefaults();
      }

      /**
        Deinitializes the drawInfo
      */
      X11DrawInfo::~X11DrawInfo()
      {
        cairo_destroy(cairo);
        if (deleteSurface) {
          cairo_surface_destroy(surface);
        }
      }

      void X11DrawInfo::SetNewSize(size_t width, size_t height)
      {
        cairo_xlib_surface_set_size(surface,width,height);
      }
#endif
    }
  }
}

