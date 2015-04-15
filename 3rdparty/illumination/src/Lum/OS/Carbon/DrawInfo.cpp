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

#include <Lum/OS/Carbon/DrawInfo.h>

//#include <assert.h>

//#include <unistd.h>

#include <Lum/Base/Util.h>

#include <Lum/Images/Image.h>

//#include <Lum/OS/Carbon/Bitmap.h>
#include <Lum/OS/Carbon/Font.h>
//#include <Lum/OS/Carbon/Image.h>
#include <Lum/OS/Carbon/Window.h>
#include <iostream>
namespace Lum {
  namespace OS {
    namespace Carbon {

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

        RecalcClipRegion();
      }

      /**
        Recalces the current clipping regions by analysing the
        current clipping stack.
       */
      void DrawInfo::RecalcClipRegion()
      {
        RgnHandle region;

        region=::NewRgn();

        if (clipStack.size()>0) {
          UnionRgn(clipStack.back().region,region,region);

          if (clipStack.size()>1) {
            int pos=clipStack.size()-2;

            while (pos>=0) {
              ::SectRgn(clipStack[pos].region,region,region);
              --pos;
            }
          }
        }
        else {
          ::SetRectRgn(region,
                       0,0,
                       32000,32000);
        }

        ::SetGWorld(port,NULL);
        ::SetClip(region);

        ::DisposeRgn(region);
      }

      /**
      Initializes a clip entry object.
      */
      void DrawInfo::ClipEntry::Init(DrawInfo *draw)
      {
        region=::NewRgn();
        this->draw=draw;
      }

      /**
      Frees the given clip entry.
      */
      void DrawInfo::ClipEntry::Free()
      {
        ::DisposeRgn(region);
      }

      /**
      adds the given rectangle to the current clip entry.
      */
      void DrawInfo::ClipEntry::Add(int x, int y, int width, int height)
      {
        ::RgnHandle region;

        region=::NewRgn();
        ::SetRectRgn(region,
                     x,y,
                     x+width,y+height);
        UnionRgn(this->region,region,this->region);
        DisposeRgn(region);
      }

      /**
        adds the given rectangle to the current clip entry.
      */
      void DrawInfo::ClipEntry::Sub(int x, int y, int width, int height)
      {
        ::RgnHandle region;

        region=::NewRgn();
        ::SetRectRgn(region,
                     x,y,
                     x+width,y+height);
        DiffRgn(this->region,region,this->region);
        DisposeRgn(region);
      }

      void DrawInfo::ReinstallClip()
      {
        if (clipStack.size()>0) {
          RecalcClipRegion();
        }
      }

      /**
        Initialize an instance of the  DrawInfo class.
      */
      DrawInfo::DrawInfo(Lum::OS::WindowPtr window)
      : ::Lum::OS::Base::DrawInfo(window)
      {
        port=GetWindowPort(dynamic_cast<Window*>(window)->window);

        ::SetGWorld(port,NULL);

        ::PenNormal();
        ::PenSize(1,1);
      }

      DrawInfo::DrawInfo(Lum::OS::BitmapPtr bitmap)
      : ::Lum::OS::Base::DrawInfo(bitmap)
      {
        // TODO
      }

      /**
        Deinitializes the drawInfo
      */
      DrawInfo::~DrawInfo()
      {
        // TODO: Add assertions
      }

      void DrawInfo::PushFont(const ::Lum::OS::FontPtr font, unsigned long style)
      {
        if (fontStack.size()>0 && fontStack.top().font.Get()==font && fontStack.top().style==style) {
          fontStack.top().count++;
        }
        else {
          FontEntry entry;

          entry.font=font;
          entry.style=style;
          entry.count=1;

          fontStack.push(entry);
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
        }
      }

      void DrawInfo::DrawString(int x, int y, const std::wstring& text, size_t start, size_t length)
      {
        char          *tmp=::Lum::Base::WStringToChar16BE(text,length);
        unsigned long style=(fontStack.top().style&(::Lum::OS::Font::maxStyleNum-1));
        Font          *font=dynamic_cast<Font*>(fontStack.top().font.Get());

        ::SetGWorld(port,NULL);

        if (::ATSUSetTextPointerLocation(font->layout,
                                        (const UniChar*)tmp,
                                        0,length,length)!=::noErr) {
          std::cerr << "Cannot set text pointer" << std::endl;
          delete [] tmp;
          return;
        }

        if (::ATSUSetRunStyle(font->layout,
                              font->styles[style],
                              0,length)!=::noErr) {
          std::cerr << "Cannot set run style" << std::endl;
          delete [] tmp;
          return;
        }

        if (::ATSUDrawText(font->layout,
                           0,length,
                           ::Long2Fix(x),
                           ::Long2Fix(y))!=::noErr) {
          std::cerr << "Cannot draw layout" << std::endl;
        }

        delete [] tmp;
      }

      void DrawInfo::DrawFillString(int x, int y,
                                    const std::wstring& text,
                                    ::Lum::OS::Color background)
      {
        /*
        bool handled=false;

        assert(fontStack.size()>0);

        if (!handled) {
          char   *buffer;
          size_t length;

          buffer=::Lum::Base::WStringToChar16BE(text,length);

          if (buffer!=NULL) {
            XDrawImageString16(display,window,gc,x,y,(XChar2b*)buffer,length);
            delete [] buffer;
          }
        }*/
      }

      void DrawInfo::SetForeground(::Lum::OS::Color color)
      {
        ::RGBColor rgb;

        rgb.red  =((color >> 16) % 256)*256;
        rgb.green=((color >>  8) % 256)*256;
        rgb.blue =((color >>  0) % 256)*256;

        ::SetGWorld(port,NULL);
        ::RGBForeColor(&rgb);
        ::RGBBackColor(&rgb);
      }

      void DrawInfo::PushForeground(::Lum::OS::Color color)
      {
        if (fPenStack.size()>0 && fPenStack.top().color==color) {
          fPenStack.top().count++;
        }
        else {
          PenColor entry;

          entry.color=color;
          entry.count=1;

          fPenStack.push(entry);

          SetForeground(color);
        }
      }

      void DrawInfo::PopForeground()
      {
        if (fPenStack.top().count>1) {
          fPenStack.top().count--;
        }
        else {
          fPenStack.pop();
          if (fPenStack.size()>0) {
            SetForeground(fPenStack.top().color);
          }
        }
      }

      void DrawInfo::PushDrawMode(::Lum::OS::DrawInfo::DrawMode mode)
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

        XSetFunction(display,gc,xMode);*/
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
        }*/
      }

      void DrawInfo::PushPen(int size, Pen pen)
      {
        /*
        if (penStack.size()>0 && penStack.top().pen==pen && penStack.top().size==size) {
         penStack.top().count++;
        }
        else {
          PenStyle entry;

          entry.size=size;
          entry.pen=pen;
          entry.count=1;

          penStack.push(entry);

          switch (pen) {
          case penNormal:
            XSetLineAttributes(display,gc,size,LineSolid,CapButt,JoinBevel);
            break;
          case penRound:
            XSetLineAttributes(display,gc,size,LineSolid,CapRound,JoinRound);
            break;
          }
        }*/
      }

      void DrawInfo::PopPen()
      {
        /*
        if (penStack.top().count>1) {
          penStack.top().count--;
        }
        else {
          penStack.pop();

          if (penStack.size()>0) {
            switch (penStack.top().pen) {
            case penNormal:
              XSetLineAttributes(display,gc,penStack.top().size,LineSolid,CapButt,JoinBevel);
              break;
            case penRound:
              XSetLineAttributes(display,gc,penStack.top().size,LineSolid,CapRound,JoinRound);
              break;
            }
          }
          else {
            XSetLineAttributes(display,gc,0,LineSolid,CapButt,JoinBevel);
          }
        }*/
      }

      void DrawInfo::PushDash(const char* dashList, size_t len, DashMode mode)
      {
        /*
        DashEntry entry;
        int       size,cap=CapButt,join=JoinBevel,line=0;

        entry.dash=dashList;
        entry.len=len;
        entry.mode=mode;

        switch (mode) {
        case fDash:
          line=LineOnOffDash;
          break;
        case fbDash:
          line=LineDoubleDash;
          break;
        }

        if (penStack.size()>0) {
          size=penStack.top().size;

          switch (penStack.top().pen) {
          case penNormal:
            cap=CapButt;
            join=JoinBevel;
            break;
            case penRound:
            cap=CapRound;
            join=JoinRound;
            break;
          }
        }
        else {
          size=0;
          cap=CapButt;
          join=JoinBevel;
        }

        dashStack.push(entry);

        XSetLineAttributes(display,gc,size,line,cap,join);
        XSetDashes(display,gc,0,dashList,len);*/
      }

      void DrawInfo::PopDash()
      {
        /*
        int size,cap=CapButt,join=JoinBevel;

        dashStack.pop();

        if (penStack.size()>0) {
          size=penStack.top().size;

          switch (penStack.top().pen) {
          case penNormal:
            cap=CapButt;
            join=JoinBevel;
            break;
            case penRound:
            cap=CapRound;
            join=JoinRound;
            break;
          }
        }
        else {
          size=0;
          cap=CapButt;
          join=JoinBevel;
        }

        if (dashStack.size()>0) {
          int line=0;

          switch (dashStack.top().mode) {
          case fDash:
            line=LineOnOffDash;
            break;
          case fbDash:
            line=LineDoubleDash;
            break;
          }

          XSetLineAttributes(display,gc,size,line,cap,join);
          XSetDashes(display,gc,0,dashStack.top().dash,
                     dashStack.top().len);
        }
        else {
          XSetLineAttributes(display,gc,size,LineSolid,cap,join);
        }*/
      }

      void DrawInfo::PushPattern(const char * /*pattern*/, int /*width*/, int /*height*/, int /*mode*/)
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

      void DrawInfo::PushBitmap(::Lum::OS::BitmapPtr bitmap, BitmapMode mode)
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

        patternStack.push(pattern);*/
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
        }*/
      }

      void DrawInfo::DrawPoint(int x, int y)
      {
        //XDrawPoint(display,window,gc,x,y);
      }

      void DrawInfo::DrawPointWithColor(int x, int y, ::Lum::OS::Color color)
      {

        //XSetForeground(display,gc,color);
        //XDrawPoint(display,window,gc,x,y);
      }

      void DrawInfo::DrawLine(int x1, int y1, int x2, int y2)
      {
        ::SetGWorld(port,NULL);

        /*
        IF d.dashStack#NIL THEN
          Carbon.PenPat(SYSTEM.VAL(Carbon.ConstPatternParam,
                                   SYSTEM.ADR(d.dashStack.pattern)));
          IF d.dashStack.mode=D.fMode THEN
            Carbon.PenMode(Carbon.srcOr);
            Carbon.MoveTo(SHORT(x1),SHORT(y1));
            Carbon.LineTo(SHORT(x2),SHORT(y2));
            Carbon.PenMode(Carbon.srcCopy);
          ELSE
            Carbon.MoveTo(SHORT(x1),SHORT(y1));
            Carbon.LineTo(SHORT(x2),SHORT(y2));
          END;

          IF Carbon.GetQDGlobalsBlack(SYSTEM.VAL(Carbon.ConstPatternParam,
                                                 SYSTEM.ADR(pattern)))#NIL THEN
          END;
          Carbon.PenPat(SYSTEM.VAL(Carbon.ConstPatternParam,SYSTEM.ADR(pattern)));
        ELSE*/
        ::MoveTo(x1,y1);
        ::LineTo(x2,y2);
      }

      void DrawInfo::DrawRectangle(int x, int y, int width, int height)
      {
        DrawLine(x,y+height-1,x,y+1);
        DrawLine(x,y,x+width-1,y);
        DrawLine(x+width-1,y+1,x+width-1,y+height-1);
        DrawLine(x+width-1,y+height-1,x,y+height-1);
      }

      void DrawInfo::FillRectangle(int x, int y, int width, int height)
      {
        ::Rect rect;
        //pattern : Carbon.Pattern;

        rect.top   =y;
        rect.left  =x;
        rect.bottom=rect.top+height;
        rect.right =rect.left+width;

        ::SetGWorld(port,NULL);

        /*
        IF d.patternStack#NIL THEN
            Carbon.PenPat(SYSTEM.VAL(Carbon.ConstPatternParam,
                                     SYSTEM.ADR(d.patternStack.pattern)));

          IF d.patternStack.mode=D.fgPattern THEN
            Carbon.PenMode(Carbon.srcOr);
            Carbon.PaintRect(SYSTEM.VAL(Carbon.RectPtr,SYSTEM.ADR(rect)));
            Carbon.PenMode(Carbon.srcCopy);
          ELSE
            Carbon.PaintRect(SYSTEM.VAL(Carbon.RectPtr,SYSTEM.ADR(rect)));
          END;

          IF Carbon.GetQDGlobalsBlack(SYSTEM.VAL(Carbon.ConstPatternParam,
                                                 SYSTEM.ADR(pattern)))#NIL THEN
          END;
          Carbon.PenPat(SYSTEM.VAL(Carbon.ConstPatternParam,SYSTEM.ADR(pattern)));
        ELSE*/
          ::PaintRect(&rect);
        //END;
      }

      void DrawInfo::DrawArc(int x, int y, int width, int height, int angle1, int angle2)
      {
        ::Rect rect;
        int    a,b;

        rect.top   =y;
        rect.left  =x;
        rect.bottom=rect.top+height;
        rect.right =rect.left+width;

        a=(angle1+angle2) / 64;
        b=angle2 / 64;

        a=(450-a) % 360;

        ::SetGWorld(port,NULL);
        ::FrameArc(&rect,a,b);
      }

      void DrawInfo::FillArc(int x, int y, int width, int height, int angle1, int angle2)
      {
        ::Rect rect;
        int    a,b;

        rect.top   =y;
        rect.left  =x;
        rect.bottom=rect.top+height;
        rect.right =rect.left+width;

        a=(angle1+angle2) / 64;
        b=angle2 / 64;

        a=(450-a) % 360;

        ::SetGWorld(port,NULL);
        ::PaintArc(&rect,a,b);
      }

      void DrawInfo::FillPolygon(const Point points[], size_t count)
      {
        ::PolyHandle handle;

        ::SetGWorld(port,NULL);
        handle=::OpenPoly();

        for (size_t i=0; i<count; ++i) {
          ::LineTo(points[i].x,points[i].y);
        }
        LineTo(points[0].x,points[0].y);
        ClosePoly();
        PaintPoly(handle);
      }

      void DrawInfo::CopyArea(int sX, int sY, int width, int height, int dX, int dY)
      {
        //XCopyArea(display,window,window,gc,sX,sY,width,height,dX,dY);
      }

      /**
        Copy the rectangle sY,sY-width,height
        to the current DrawInfo starting at position dX,dY.
      */
      void DrawInfo::CopyFromBitmap(::Lum::OS::BitmapPtr bitmap, int sX, int sY, int width, int height, int dX, int dY)
      {
        //XCopyArea(display,dynamic_cast<Bitmap*>(bitmap)->pixmap,window,gc,sX,sY,width,height,dX,dY);
      }

      void DrawInfo::CopyToBitmap(int sX, int sY, int width, int height, int dX, int dY, ::Lum::OS::BitmapPtr bitmap)
      {
        /*
        XCopyArea(display,window,
                  dynamic_cast<Bitmap*>(bitmap)->pixmap,
                  dynamic_cast<DrawInfo*>(bitmap->GetDrawInfo())->gc,
                  sX,sY,width,height,dX,dY);*/
      }
    }
  }
}
