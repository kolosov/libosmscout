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

#include <Lum/OS/Win32/DrawInfo.h>

#if defined(_MSC_VER)
#define _USE_MATH_DEFINES
#endif
#include <math.h>

#include <Lum/OS/Win32/Bitmap.h>
#include <Lum/OS/Win32/Font.h>
#include <Lum/OS/Win32/Window.h>
//#include <iostream>
namespace Lum {
  namespace OS {
    namespace Win32 {

      DrawInfo::DrawInfo(OS::Window* window)
      : OS::Base::DrawInfo(window)
      {
        hwnd=dynamic_cast<Window*>(window)->window;
        dc=GetDC(dynamic_cast<Window*>(window)->window);

        /* ignore */ SetGraphicsMode(dc,GM_ADVANCED);
        /* ignore */ SetROP2(dc,R2_COPYPEN);
        /* ignore */ SetTextAlign(dc,TA_BASELINE+TA_LEFT);
        /* ignore */ SetMapMode(dc,MM_TEXT);
        /* ignore */ SetBkMode(dc,TRANSPARENT);
        /* ignore */ SetBkColor(dc,RGB(0,0,0));

        brush=0;
        pen=0;
      }

      DrawInfo::DrawInfo(OS::Window* window, HDC dc)
      : OS::Base::DrawInfo(window)
      {
        hwnd=dynamic_cast<Window*>(window)->window;
        this->dc=dc;

        /* ignore */ SetGraphicsMode(dc,GM_ADVANCED);
        /* ignore */ SetROP2(dc,R2_COPYPEN);
        /* ignore */ SetTextAlign(dc,TA_BASELINE+TA_LEFT);
        /* ignore */ SetMapMode(dc,MM_TEXT);
        /* ignore */ SetBkMode(dc,TRANSPARENT);
        /* ignore */ SetBkColor(dc,RGB(0,0,0));

        brush=0;
        pen=0;
      }
      
      DrawInfo::DrawInfo(OS::Bitmap* window)
      : OS::Base::DrawInfo(window)
      {
        hwnd=0;
        dc=dynamic_cast<Bitmap*>(window)->dc;

        /* ignore */ SetGraphicsMode(dc,GM_ADVANCED);
        /* ignore */ SetROP2(dc,R2_COPYPEN);
        /* ignore */ SetTextAlign(dc,TA_BASELINE+TA_LEFT);
        /* ignore */ SetMapMode(dc,MM_TEXT);
        /* ignore */ SetBkMode(dc,TRANSPARENT);
        /* ignore */ SetBkColor(dc,RGB(0,0,0));

        brush=0;
        pen=0;
      }

      /**
        Deinitializes the drawInfo
      */
      DrawInfo::~DrawInfo()
      {
        assert(fPenStack.size()==0);
        assert(penStack.size()==0);
        assert(fontStack.size()==0);
        assert(clipStack.size()==0);
        assert(dashStack.size()==0);

        if (pen!=0) {
          /* ignore */ DeleteObject(pen);
        }

        if (brush!=0) {
          /* ignore */ DeleteObject(brush);
        }
      }

      void DrawInfo::PushClip(size_t x, size_t y, size_t width, size_t height)
      {
        ClipEntry entry;

        entry.Init(this);

        clipStack.push_back(entry);

        clipStack.back().Add(x,y,width,height);
        RecalcClipRegion();
      }

      void DrawInfo::PushClipBegin(size_t x, size_t y, size_t width, size_t height)
      {
        ClipEntry entry;

        entry.Init(this);

        clipStack.push_back(entry);

        clipStack.back().Add(x,y,width,height);
      }

      void DrawInfo::AddClipRegion(size_t x, size_t y, size_t width, size_t height)
      {
        clipStack.back().Add(x,y,width,height);
      }

      void DrawInfo::SubClipRegion(size_t x, size_t y, size_t width, size_t height)
      {
        clipStack.back().Sub(x,y,width,height);
      }

      void DrawInfo::PushClipEnd()
      {
        RecalcClipRegion();
      }

      void DrawInfo::PopClip()
      {
        clipStack[clipStack.size()-1].Free();

        clipStack.pop_back();

        if (clipStack.size()>0) {
          RecalcClipRegion();
        }
      }

      /**
        Recalces the current clipping regions by analysing the
        current clipping stack.
       */

      void DrawInfo::RecalcClipRegion()
      {
        int  pos;
        HRGN region;

        if (clipStack.size()==0) {
          return;
        }

        region=CreateRectRgn(0,0,0,0);
        /* ignore */ CombineRgn(region,clipStack.back().region,0,RGN_COPY);

        if (clipStack.size()>1) {
          pos=clipStack.size()-2;
          while (pos>=0) {
            /* ignore */ CombineRgn(region,region,clipStack[pos].region,RGN_AND);

            --pos;
          }
        }

        /* ignore */ SelectClipRgn(dc,region);
        /* ignore */ DeleteObject(region);
      }

      /**
      Initializes a clip entry object.
      */
      void DrawInfo::ClipEntry::Init(DrawInfo *draw)
      {
        region=0;
        this->draw=draw;
      }

      /**
      Frees the given clip entry.
      */
      void DrawInfo::ClipEntry::Free()
      {
        if (region!=0) {
          /* ignore */ DeleteObject(region);
        }
      }

      /**
      adds the given rectangle to the current clip entry.
      */
      void DrawInfo::ClipEntry::Add(int x, int y, int width, int height)
      {
        HRGN tmpRegion;

        if (region==0) {
          region=CreateRectRgn(x,y,x+width,y+height);
        }
        else {
          tmpRegion=CreateRectRgn(x,y,x+width,y+height);
          /* ignore */ CombineRgn(region,region,tmpRegion,RGN_OR);
          /* ignore */ DeleteObject(tmpRegion);
        }
      }

      /**
        adds the given rectangle to the current clip entry.
      */
      void DrawInfo::ClipEntry::Sub(int x, int y, int width, int height)
      {
        HRGN tmpRegion;

        assert(region!=0);

        tmpRegion=CreateRectRgn(x,y,x+width,y+height);
        /* ignore */ CombineRgn(region,region,tmpRegion,RGN_DIFF);
        /* ignore */ DeleteObject(tmpRegion);
      }

      void DrawInfo::ReinstallClip()
      {
        if (clipStack.size()>0) {
          RecalcClipRegion();
        }
      }

      void DrawInfo::PushFont(OS::Font *font, unsigned long style)
      {
        if (fontStack.size()>0 && (fontStack.top().font.Get()==font) && fontStack.top().style==style) {
          fontStack.top().count++;
        }
        else {
          FontEntry       entry;
          Font            *f;
          Font::FontStyle *s;

          entry.font=font;
          entry.style=style;
          entry.count=1;

          f=dynamic_cast<Font*>(font);

          s=f->GetStyle(style);

          fontStack.push(entry);

          if (s!=NULL) {
            SelectObject(dc,s->hFont);
          }
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

          if (fontStack.size()>0) {
            Font            *font=dynamic_cast<Font*>(fontStack.top().font.Get());
            Font::FontStyle *s;

            s=font->GetStyle(fontStack.top().style);

            if (s!=NULL) {
              SelectObject(dc, s->hFont);
            }
          }
        }
      }

      void DrawInfo::DrawString(int x, int y, const std::wstring& text, size_t start, size_t length)
      {
        int old;

        old=SetBkMode(dc,TRANSPARENT);
        /* ignore */ TextOutW(dc,x,y,text.data()+start,length);
        /* ignore */ SetBkMode(dc,old);
      }

      void DrawInfo::DrawFillString(int x, int y,
                                    const std::wstring& text,
                                    OS::Color /*background*/)
      {
        int old;

        old=SetBkMode(dc,OPAQUE);
        /* ignore */ TextOutW(dc,x,y,text.c_str(),text.length());
        /* ignore */ SetBkMode(dc,old);
      }

      void DrawInfo::PushForeground(OS::Color color)
      {
        if (fPenStack.size()>0 && fPenStack.top().color==color) {
          fPenStack.top().count++;
        }
        else {
          PenColor entry;

          entry.color=color;
          entry.count=1;
          fPenStack.push(entry);
        }
        CalculatePenAndBrush();
      }

      void DrawInfo::PopForeground()
      {
        if (fPenStack.top().count>1) {
          fPenStack.top().count--;
        }
        else {
          fPenStack.pop();
        }
        CalculatePenAndBrush();
      }

      void DrawInfo::PushDrawMode(OS::DrawInfo::DrawMode /*mode*/)
      {
        /*
        DrawMode entry;
        int      xMode=GXcopy;

        entry.mode=mode;
        modeStack.push(entry);

        switch (mode) {
        case drawModeCopy:
          xMode=GXcopy;
          break;
        case drawModeInvert:
          xMode=GXinvert;
          break;
        }

        XSetFunction(display,gc,xMode);
        */
      }

      void DrawInfo::PopDrawMode()
      {
        /*
        int xMode=GXcopy;

        modeStack.pop();

        if (modeStack.size()>0) {
          switch (modeStack.top().mode) {
          case drawModeCopy:
            xMode=GXcopy;
            break;
          case drawModeInvert:
            xMode=GXinvert;
            break;
          }

          XSetFunction(display,gc,xMode);
        }
        else {
          XSetFunction(display,gc,GXcopy);
        }
        */
      }

      void DrawInfo::PushPen(int size, Pen pen)
      {
        if (penStack.size()>0 && penStack.top().pen==pen && penStack.top().size==size) {
         penStack.top().count++;
        }
        else {
          PenStyle entry;

          entry.size=size;
          entry.pen=pen;
          entry.count=1;

          penStack.push(entry);
        }

        CalculatePenAndBrush();
      }

      void DrawInfo::PopPen()
      {
        if (penStack.top().count>1) {
          penStack.top().count--;
        }
        else {
          penStack.pop();
        }
        CalculatePenAndBrush();
      }

      void DrawInfo::PushDash(const char* dashList, size_t len, DashMode mode)
      {
    assert(dashList!=NULL && len>0);

        DashEntry entry;
        //int       size,cap=CapButt,join=JoinBevel,line=0;

        entry.dash=new DWORD[len];

        for (size_t x=0; x<len; x++) {
          entry.dash[x]=dashList[x];
        }
        entry.len=len;
        entry.mode=mode;

        /*
        switch (mode) {
        case fDash:
          line=LineOnOffDash;
          break;
        case fbDash:
          line=LineDoubleDash;
          break;
        }*/

        dashStack.push(entry);

        CalculatePenAndBrush();
      }

      void DrawInfo::PopDash()
      {
        delete dashStack.top().dash;
        dashStack.pop();

        CalculatePenAndBrush();
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
        X11.XSetStipple(D.display(Display).display,d.gc,pat.pixMap);
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
        /*
        PatternEntry pattern;

        pattern.pixmap=dynamic_cast<Bitmap*>(bitmap)->pixmap;
        pattern.mode=mode;

        XSetStipple(display,gc,pattern.pixmap);

        switch (mode) {
        case bitmapModeFg:
          XSetFillStyle(display,gc,FillStippled);
          break;
        case bitmapModeFgBg:
          XSetFillStyle(display,gc,FillOpaqueStippled);
          break;
        }

        patternStack.push(pattern);
        */
      }

      void DrawInfo::PopBitmap()
      {
        /*
        patternStack.pop();

        if (patternStack.size()>0) {
          XSetStipple(display,gc,patternStack.top().pixmap);

          switch (patternStack.top().mode) {
          case bitmapModeFg:
            XSetFillStyle(display,gc,FillStippled);
            break;
          case bitmapModeFgBg:
            XSetFillStyle(display,gc,FillOpaqueStippled);
            break;
          }
        }
        else {
          XSetFillStyle(display,gc,FillSolid);
        }
        */
      }

      void DrawInfo::DrawPoint(int x, int y)
      {
        SetPixel(dc,x,y,fPenStack.top().color);
      }

      void DrawInfo::DrawPointWithColor(int x, int y, OS::Color color)
      {
        SetPixel(dc,x,y,color);
      }

      void DrawInfo::DrawLine(int x1, int y1, int x2, int y2)
      {
        MoveToEx(dc,x1,y1,NULL);
        LineTo(dc,x2,y2);
        DrawPoint(x2,y2);
      }

      void DrawInfo::DrawRectangle(int x, int y, int width, int height)
      {
        POINT points[5];

        points[0].x=x;
        points[0].y=y;

        points[1].x=x+width-1;
        points[1].y=y;

        points[2].x=x+width-1;
        points[2].y=y+height-1;

        points[3].x=x;
        points[3].y=y+height-1;

        points[4].x=x;
        points[4].y=y;

        Polyline(dc,points,5);
      }

      void DrawInfo::FillRectangle(int x, int y, int width, int height)
      {
        RECT rect;

        rect.left=x;
        rect.top=y;
        rect.right=x+width;
        rect.bottom=y+height;

        /*
        IF ((d.patternStack#NIL) & (d.patternStack.mode=D.fgPattern)) THEN
          IF W.PatBlt(d.dc,x,y,width,height,0FA0089H)#0 THEN END;
        ELSE*/
          /* ignore */ FillRect(dc,&rect,brush);
          /*
        END;*/
      }

      void DrawInfo::DrawArc(int x, int y, int width, int height, int angle1, int angle2)
      {
        double a,b;
        int    sx,sy,ex,ey;

        if (angle1==0 && angle2==360*64) {
          /*ignore */ Arc(dc,x,y,x+width,y+height,0,0,0,0);
          return;
        }

        /* Swap angles, if necessary */
        if (angle1>angle2) {
          SetArcDirection(dc,AD_CLOCKWISE);
          angle2=angle1+angle2;
        }
        else {
          angle1=angle1-angle2;
          SetArcDirection(dc,AD_COUNTERCLOCKWISE);
        }

         /* Transformation to windows: absolute angle */

        /* Calculate radials */
        a=((angle1 / 64)*M_PI)/180.0;
        b=((angle2 / 64)*M_PI)/180.0;

        sx=x+width  / 2 +(int)(100*cos(a));
        sy=y+height / 2 -(int)(100*sin(a));
        ex=x+width  / 2 +(int)(100*cos(b));
        ey=y+height / 2 -(int)(100*sin(b));

        /*ignore */ Arc(dc,x,y,x+width,y+height,sx,sy,ex,ey);
      }

      void DrawInfo::FillArc(int x, int y, int width, int height, int angle1, int angle2)
      {
        double a,b;
        int    sx,sy,ex,ey;

        //std::cout << x << "," << y << " " << width << "x" << height << " " << angle1 << " " << angle2 << std::endl;

        if (angle1==0 && angle2==360*64) {
          //std::cout << "Draw ellipse" << std::endl;
          /*ignore */ Ellipse(dc,x,y,x+width,y+height);
          return;
        }

        /* Swap angles, if necessary */
        if (angle1>angle2) {
          //std::cout << "=> Swapping angles" << std::endl;
          SetArcDirection(dc,AD_CLOCKWISE);
          angle2=angle1+angle2;
        }
        else {
          angle1=angle1-angle2;
          SetArcDirection(dc,AD_COUNTERCLOCKWISE);
        }

        /* Calculate radials */
        a=((angle1 / 64)*M_PI)/180.0;
        b=((angle2 / 64)*M_PI)/180.0;

        sx=x+width  / 2 +(int)(100*cos(a));
        sy=y+height / 2 -(int)(100*sin(a));
        ex=x+width  / 2 +(int)(100*cos(b));
        ey=y+height / 2 -(int)(100*sin(b));

        //std::cout << "=> " << sx << "x" << sy << " " << ex << "x" << ey << std::endl;

        /*ignore */ Pie(dc,x,y,x+width,y+height,sx,sy,ex,ey);
      }

      void DrawInfo::FillPolygon(const Point points[], size_t count)
      {
        /* ignore */ Polygon(dc,(const POINT*)points,count);
      }

      void DrawInfo::CopyArea(int sX, int sY, int width, int height, int dX, int dY)
      {
        /* ignore */ BitBlt(dc,dX,dY,width,height,dc,sX,sY,SRCCOPY);
      }

      /**
        Copy the rectangle sY,sY-width,height
        to the current DrawInfo starting at position dX,dY.
      */
      void DrawInfo::CopyFromBitmap(OS::Bitmap* bitmap, int sX, int sY, int width, int height, int dX, int dY)
      {
        Bitmap* b=dynamic_cast<Bitmap*>(bitmap);

        /* ignore */ BitBlt(dc,dX,dY,width,height,b->dc,sX,sY,SRCCOPY);
      }

      void DrawInfo::CopyToBitmap(int sX, int sY, int width, int height, int dX, int dY, OS::Bitmap* bitmap)
      {
        Bitmap* b=dynamic_cast<Bitmap*>(bitmap);

        /* ignore */ BitBlt(b->dc,dX,dY,width,height,dc,sX,sY,SRCCOPY);
      }

      void DrawInfo::CalculatePenAndBrush()
      {
        Color    color;
        size_t   size;
        LOGBRUSH info;
        HBRUSH   newBrush,oldBrush;
        HPEN     newPen,oldPen;

        if (fPenStack.size()>0) {
          color=fPenStack.top().color;
        }
        else {
          color=RGB(0,0,0);
        }

        if (penStack.size()>0) {
          size=penStack.top().size;
        }
        else {
          size=0;
        }

        newBrush=CreateSolidBrush(color);

        info.lbStyle=BS_SOLID;
        info.lbColor=color;
        info.lbHatch=0;

        if (dashStack.size()>0) {
          newPen=ExtCreatePen(PS_GEOMETRIC|PS_USERSTYLE|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,size,&info,dashStack.top().len,dashStack.top().dash);
        }
        else {
          newPen=ExtCreatePen(PS_GEOMETRIC|PS_SOLID|PS_ENDCAP_FLAT|PS_JOIN_BEVEL,size,&info,0,NULL);
        }

        /* ignore */ SetTextColor(dc,color);
        /* ignore */ SetBkColor(dc,color);

        oldBrush=(HBRUSH)SelectObject(dc,newBrush);
        oldPen=(HPEN)SelectObject(dc,newPen);

        if (oldBrush==brush) {
          DeleteObject(brush);
        }
        else if (brush!=0) {
          assert(false);
        }
        brush=newBrush;

        if (oldPen==pen) {
          DeleteObject(pen);
        }
        else if (pen!=0) {
          assert(false);
        }
        pen=newPen;
      }
    }
  }
}

