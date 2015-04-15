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

#include <Lum/OS/Curses/DrawInfo.h>

#include <assert.h>

#include <Lum/Base/String.h>

#include <Lum/Images/Image.h>

//#include <Lum/OS/X11/Bitmap.h>
#include <Lum/OS/Curses/Font.h>
//#include <Lum/OS/X11/Image.h>
#include <Lum/OS/Curses/Window.h>
#include <iostream>
namespace Lum {
  namespace OS {
    namespace Curses {

      void DrawInfo::SetNewSize(size_t /*width*/, size_t /*height*/)
      {
        // no code
        // TODO: Resize clip mask!
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

        RecalcClipRegion();
      }

      /**
        Recalces the current clipping regions by analysing the
        current clipping stack.
       */
      void DrawInfo::RecalcClipRegion()
      {
        clip.resize(width*height);

        if (clipStack.size()==0){
          for (size_t x=0; x<clip.size(); x++) {
            clip[x]=true;
          }
        }
        else {
          for (size_t x=0; x<clip.size(); x++) {
            clip[x]=clipStack[0].region[x];
          }

          for (size_t i=1; i<clipStack.size(); i++) {
            for (size_t x=0; x<clip.size(); x++) {
              if (!clipStack[i].region[x]) {
                clip[x]=false;
              }
            }
          }
        }
      }

      /**
      Initializes a clip entry object.
      */
      void DrawInfo::ClipEntry::Init(DrawInfo *draw)
      {
        region.resize(draw->width*draw->height);

        for (size_t x=0; x<region.size(); x++) {
          region[x]=false;
        }

        this->draw=draw;
      }

      /**
      Frees the given clip entry.
      */
      void DrawInfo::ClipEntry::Free()
      {
        // no code
      }

      /**
      adds the given rectangle to the current clip entry.
      */
      void DrawInfo::ClipEntry::Add(int x, int y, int width, int height)
      {
        for (int i=x; i<x+width; i++) {
          for (int j=y; j<y+height; j++) {
            region[j*draw->width+i]=true;
          }
        }
      }

      /**
        adds the given rectangle to the current clip entry.
      */
      void DrawInfo::ClipEntry::Sub(int x, int y, int width, int height)
      {
        for (int i=x; i<x+width; i++) {
          for (int j=y; j<y+height; j++) {
            region[j*draw->width+i]=false;
          }
        }
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
      DrawInfo::DrawInfo(Lum::OS::Window* window)
      : OS::Base::DrawInfo(window)
      {
        this->window=dynamic_cast<Window*>(window)->GetWINDOW();
        width=window->GetWidth();
        height=window->GetHeight();

        clip.resize(width*height);
        for (size_t x=0; x<clip.size(); x++) {
          clip[x]=true;
        }
      }

      DrawInfo::DrawInfo(Lum::OS::Bitmap* bitmap)
      : OS::Base::DrawInfo(bitmap)
      {
        //window=dynamic_cast<Bitmap*>(bitmap)->GetDrawable();
      }

      /**
        Deinitializes the drawInfo
      */
      DrawInfo::~DrawInfo()
      {
        // no code
      }

      int DrawInfo::GetBackground(int x, int y) const
      {
        return PAIR_NUMBER(mvwinch(window,y,x))/(display->GetColorDepth()*2);
      }

      void DrawInfo::DrawChar(int x, int y, char character)
      {
        if (clip[y*width+x]) {
          int bg=GetBackground(x,y);
          int style;

          switch (fontStack.top().style & (Font::bold|Font::italic|Font::underlined|Font::slanted)) {
          case Font::normal:
          case Font::italic:
          case Font::slanted:
            style=A_NORMAL;
            break;
          case Font::bold:
            style=A_BOLD;
            break;
          case Font::underlined:
            style=A_UNDERLINE;
            break;
          default:
            style=A_NORMAL;
            break;
          }

          wattrset(window,style+COLOR_PAIR(display->GetColorDepth()*2*bg+fPenStack.top().color));
          mvwaddch(window,y,x,character);
        }
      }

      void DrawInfo::DrawDirect(int x, int y, int character)
      {
        if (clip[y*width+x]) {
          int bg=GetBackground(x,y);

          wattrset(window,COLOR_PAIR(display->GetColorDepth()*2*bg+fPenStack.top().color));
          mvwaddch(window,y,x,character);
        }
      }

      void DrawInfo::Fill(int x, int y)
      {
        if (clip[y*width+x]) {
          wattrset(window,A_NORMAL+COLOR_PAIR(display->GetColorDepth()*2*fPenStack.top().color));
          mvwaddch(window,y,x,' ');
        }
      }

      void DrawInfo::PushFont(OS::Font *font, unsigned long style)
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
        std::string tmp=Lum::Base::WStringToString(text);

        for (size_t i=start; i<start+length; i++) {
          DrawChar(x,y,tmp[i]);
          x++;
        }
      }

      void DrawInfo::DrawFillString(int x, int y,
                                    const std::wstring& text,
                                    OS::Color background)
      {
        if (text.length()==1) {
          switch (text[0]) {
          case L'\x2500':
            PushForeground(background);
            Fill(x,y);
            PopForeground();
            DrawDirect(x,y,ACS_HLINE);
            return;
          case L'\x2502':
            PushForeground(background);
            Fill(x,y);
            PopForeground();
            DrawDirect(x,y,ACS_VLINE);
            return;
          case L'\x250c':
            PushForeground(background);
            Fill(x,y);
            PopForeground();
            DrawDirect(x,y,ACS_ULCORNER);
            return;
          case L'\x2510':
            PushForeground(background);
            Fill(x,y);
            PopForeground();
            DrawDirect(x,y,ACS_URCORNER);
            return;
          case L'\x2514':
            PushForeground(background);
            Fill(x,y);
            PopForeground();
            DrawDirect(x,y,ACS_LLCORNER);
            return;
          case L'\x2518':
            PushForeground(background);
            Fill(x,y);
            PopForeground();
            DrawDirect(x,y,ACS_LRCORNER);
            return;
          case L'\x2190':
            PushForeground(background);
            Fill(x,y);
            PopForeground();
            DrawDirect(x,y,ACS_LARROW);
            return;
          case L'\x2191':
            PushForeground(background);
            Fill(x,y);
            PopForeground();
            DrawDirect(x,y,ACS_UARROW);
            return;
          case L'\x2192':
            PushForeground(background);
            Fill(x,y);
            PopForeground();
            DrawDirect(x,y,ACS_RARROW);
            return;
          case L'\x2193':
            PushForeground(background);
            Fill(x,y);
            PopForeground();
            DrawDirect(x,y,ACS_DARROW);
            return;
          default:
            break;
          }
        }

        std::string tmp=Lum::Base::WStringToString(text);

        PushForeground(background);
        for (size_t i=0; i<tmp.length(); i++) {
          Fill(x+i,y);
        }
        PopForeground();

        for (size_t i=0; i<tmp.length(); i++) {
          DrawChar(x+i,y,tmp[i]);
        }
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
      }

      void DrawInfo::PopForeground()
      {
        if (fPenStack.top().count>1) {
          fPenStack.top().count--;
        }
        else {
          fPenStack.pop();
        }
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
        if (penStack.size()>0 && penStack.top().pen==pen && penStack.top().size==size) {
         penStack.top().count++;
        }
        else {
          PenStyle entry;

          entry.size=size;
          entry.pen=pen;
          entry.count=1;

          penStack.push(entry);

          /*
          if (d.dashStack!=NULL) {
            lMode=d.dashStack.mode;
          }
          else {
            lMode=X11.LineSolid;
          }*/

          switch (pen) {
          case penNormal:
            //XSetLineAttributes(display,gc,size,LineSolid,CapButt,JoinBevel);
            break;
          case penRound:
            //XSetLineAttributes(display,gc,size,LineSolid,CapRound,JoinRound);
            break;
          }
        }
      }

      void DrawInfo::PopPen()
      {
        if (penStack.top().count>1) {
          penStack.top().count--;
        }
        else {
          penStack.pop();

          /*
          if (d.dashStack!=NULL) {
            mode=d.dashStack.mode;
          }
          else {
            mode=X11.LineSolid;
          }*/

          if (penStack.size()>0) {
            switch (penStack.top().pen) {
            case penNormal:
              //XSetLineAttributes(display,gc,penStack.top().size,LineSolid,CapButt,JoinBevel);
              break;
            case penRound:
              //XSetLineAttributes(display,gc,penStack.top().size,LineSolid,CapRound,JoinRound);
              break;
            }
          }
          else {
            //XSetLineAttributes(display,gc,0,LineSolid,CapButt,JoinBevel);
          }
        }
      }

      void DrawInfo::PushDash(const char* dashList, size_t len, DashMode /*mode*/)
      {
	assert(dashList!=NULL && len>0);
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
        }  */
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
        Fill(x,y);
      }

      void DrawInfo::DrawPointWithColor(int x, int y, OS::Color color)
      {
        PushForeground(color);
        Fill(x,y);
        PopForeground();
      }

      void DrawInfo::DrawLine(int x1, int y1, int x2, int y2)
      {
        if (y1==y2) {
          for (int i=x1; i<=x2; i++) {
            DrawPoint(i,y1);
          }
        }
        else if (x1==x2) {
          for (int i=y1; i<=y2; i++) {
            DrawPoint(x1,i);
          }
        }
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
        for (int i=x; i<x+width; i++) {
          for (int j=y; j<y+height; j++) {
            Fill(i,j);
          }
        }
      }

      void DrawInfo::DrawArc(int /*x*/, int /*y*/, int /*width*/, int /*height*/, int /*angle1*/, int /*angle2*/)
      {
        //XDrawArc(display,window,gc,x,y,width-1,height-1,angle1,angle2);
      }

      void DrawInfo::FillArc(int /*x*/, int /*y*/, int /*width*/, int /*height*/, int /*angle1*/, int /*angle2*/)
      {
        //XFillArc(display,window,gc,x,y,width,height,angle1,angle2);
      }

      void DrawInfo::FillPolygon(const Point /*points*/[], size_t /*count*/)
      {
        /*
        for (size_t x=0; x< count; x++) {
          pointArray[x].x=points[x].x;
          pointArray[x].y=points[x].y;
        }
        XFillPolygon(display,window,gc,pointArray,count,Complex,CoordModeOrigin);
        */
      }

      void DrawInfo::CopyArea(int sX, int sY, int width, int height, int dX, int dY)
      {
        std::vector<int> buffer;
        size_t           s;

        buffer.resize(width*height);

        s=0;
        for (int j=sY; j<sY+height; j++) {
          for (int i=sX; i<sX+width; i++) {
            buffer[s]=mvwinch(window,j,i);

            ++s;
          }
        }

        s=0;
        for (int j=dY; j<dY+height; j++) {
          for (int i=dX; i<dX+width; i++) {
            if (clip[j*this->width+i]) {
              mvwaddch(window,j,i,buffer[s]);
            }

            ++s;
          }
        }
      }

      /**
        Copy the rectangle sY,sY-width,height
        to the current DrawInfo starting at position dX,dY.
      */
      void DrawInfo::CopyFromBitmap(OS::Bitmap* /*bitmap*/, int /*sX*/, int /*sY*/, int /*width*/, int /*height*/, int /*dX*/, int /*dY*/)
      {
        //XCopyArea(display,dynamic_cast<Bitmap*>(bitmap)->pixmap,window,gc,sX,sY,width,height,dX,dY);
      }

      void DrawInfo::CopyToBitmap(int /*sX*/, int /*sY*/, int /*width*/, int /*height*/, int /*dX*/, int /*dY*/, OS::Bitmap* /*bitmap*/)
      {
        /*
        XCopyArea(display,window,
                  dynamic_cast<Bitmap*>(bitmap)->pixmap,
                  dynamic_cast<DrawInfo*>(bitmap->GetDrawInfo())->gc,
                  sX,sY,width,height,dX,dY);
        */
      }
    }
  }
}
