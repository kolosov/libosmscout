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

#include <Lum/OS/Win32/Win32Theme.h>

#include <algorithm>
#include <iostream>

#include <uxtheme.h>
#include <tmschema.h>

#include <Lum/Private/Config.h>

#include <Lum/OS/Win32/OSAPI.h>

#include <Lum/Base/Size.h>

#if defined(LUM_HAVE_LIB_CAIRO)
  #include <Lum/OS/Cairo/DrawInfo.h>
#endif

#include <Lum/OS/Win32/Display.h>
#include <Lum/OS/Win32/DrawInfo.h>

#define MENU_BARBACKGROUND         7
#define MENU_BARITEM               8
#define MENU_POPUPCHECK           11
#define MENU_POPUPCHECKBACKGROUND 12
#define MENU_POPUPITEM            14
#define MENU_SEPARATOR            15
#define MENU_POPUPSUBMENU         16
#define MENU_SYSTEMCLOSE          17
#define MENU_SYSTEMMAXIMISE       18
#define MENU_SYSTEMMINIMIZE       19
#define MENU_SYSTEMRESTOREE       20

#define LBCP_BORDER_NOSCROLL       0

namespace Lum {
  namespace OS {
    namespace Win32 {

      static HMODULE themeModule=0;
      static HTHEME  buttonTheme;
      static HTHEME  comboboxTheme;
      static HTHEME  editTheme;
      static HTHEME  headerTheme;
      static HTHEME  listboxTheme;
      static HTHEME  listviewTheme;
      static HTHEME  menuTheme;
      static HTHEME  progressTheme;
      static HTHEME  scrollbarTheme;
      static HTHEME  tabTheme;
      static HTHEME  toolbarTheme;
      static HTHEME  trackbarTheme;
      static HTHEME  windowTheme;
      static bool    themeChecked=false;
      static bool    themeAvailable=false;

      typedef BOOL (WINAPI *IS_APP_THEMED)();
      typedef HANDLE (WINAPI *OPEN_THEME_DATA)(HWND hwnd, LPCWSTR classList);
      typedef HRESULT (WINAPI *DRAW_THEME_BACKGROUND)(HTHEME theme, HDC dc, int part, int state, const RECT* rect, const RECT* clip);
      typedef HRESULT (WINAPI *DRAW_THEME_BACKGROUND_EX)(HTHEME theme, HDC dc, int part, int state, const RECT* rect, const DTBGOPTS *options);
      typedef HRESULT (WINAPI *GET_THEME_BACKGROUND_CONTENT_RECT)(HTHEME theme, HDC dc, int part, int state, const RECT* extent, RECT* content);
      typedef HRESULT (WINAPI *GET_THEME_BACKGROUND_EXTENT)(HTHEME theme, HDC dc, int part, int state, const RECT* content, RECT* extent);
      typedef BOOL (WINAPI *IS_THEME_BACKGROUND_PARTIALLY_TRANSPARENT)(HTHEME theme, int partId, int stateId);
      typedef HRESULT (WINAPI *DRAW_THEME_PARENT_BACKGROUND)(HTHEME theme, HDC dc, const RECT* rect);
      typedef HRESULT (WINAPI *GET_THEME_PART_SIZE)(HTHEME theme, HDC hdc, int partId, int stateId, LPCRECT prc, THEMESIZE size, SIZE *psz);
      typedef HRESULT (WINAPI *GET_THEME_INT)(HTHEME theme,int partId,int stateId,int propId,int *val);
      typedef HRESULT (WINAPI *GET_THEME_RECT)(HTHEME theme, int partId, int stateId, int propId, LPRECT rect);

      BOOL MyIsAppThemed()
      {
        IS_APP_THEMED function=(IS_APP_THEMED)GetProcAddress(themeModule,"IsAppThemed");

        return (*function)();
      }

      HTHEME MyOpenThemeData(HWND hwnd, LPCWSTR classList)
      {
        OPEN_THEME_DATA function=(OPEN_THEME_DATA)GetProcAddress(themeModule,"OpenThemeData");

        return (*function)(hwnd,classList);
      }

      HRESULT MyDrawThemeBackground(HTHEME theme, HDC dc, int part, int state, const RECT* rect, const RECT* clip)
      {
        DRAW_THEME_BACKGROUND function=(DRAW_THEME_BACKGROUND)GetProcAddress(themeModule,"DrawThemeBackground");

        return (*function)(theme,dc,part,state,rect,clip);
      }

      HRESULT MyDrawThemeBackgroundEx(HTHEME theme, HDC dc, int part, int state, const RECT* rect, DTBGOPTS *options)
      {
        DRAW_THEME_BACKGROUND_EX function=(DRAW_THEME_BACKGROUND_EX)GetProcAddress(themeModule,"DrawThemeBackgroundEx");

        return (*function)(theme,dc,part,state,rect,options);
      }

      HRESULT MyGetThemeBackgroundContentRect(HTHEME theme, HDC dc, int part, int state, const RECT* extent, RECT* content)
      {
        GET_THEME_BACKGROUND_CONTENT_RECT function=(GET_THEME_BACKGROUND_CONTENT_RECT)GetProcAddress(themeModule,"GetThemeBackgroundContentRect");

        return (*function)(theme,dc,part,state,extent,content);
      }

       HRESULT MyGetThemeBackgroundExtent(HTHEME theme, HDC dc, int part, int state, const RECT* content, RECT* extent)
      {
        GET_THEME_BACKGROUND_EXTENT function=(GET_THEME_BACKGROUND_EXTENT)GetProcAddress(themeModule,"GetThemeBackgroundExtent");

        return (*function)(theme,dc,part,state,content,extent);
      }

      BOOL MyIsThemeBackgroundPartiallyTransparent(HTHEME theme, int partId, int stateId)
      {
        IS_THEME_BACKGROUND_PARTIALLY_TRANSPARENT function=(IS_THEME_BACKGROUND_PARTIALLY_TRANSPARENT)GetProcAddress(themeModule,"IsThemeBackgroundPartiallyTransparent");

        return (*function)(theme,partId,stateId);
      }

      HRESULT MyDrawThemeParentBackground(HTHEME theme, HDC dc, const RECT* rect)
      {
        DRAW_THEME_PARENT_BACKGROUND function=(DRAW_THEME_PARENT_BACKGROUND)GetProcAddress(themeModule,"DrawThemeParentBackground");

        return (*function)(theme,dc,rect);
      }


      HRESULT MyGetThemePartSize(HTHEME theme, HDC hdc, int partId, int stateId, LPCRECT prc, THEMESIZE size, SIZE *psz)
      {
        GET_THEME_PART_SIZE function=(GET_THEME_PART_SIZE)GetProcAddress(themeModule,"GetThemePartSize");

        return (*function)(theme,hdc,partId,stateId,prc,size,psz);
      }

      HRESULT MyGetThemeInt(HTHEME theme,int partId,int stateId,int propId,int *val)
      {
        GET_THEME_INT function=(GET_THEME_INT)GetProcAddress(themeModule,"GetThemeInt");

        return (*function)(theme,partId,stateId,propId,val);
      }

      HRESULT MyGetThemeRect(HTHEME theme, int partId, int stateId, int propId, LPRECT rect)
      {
        GET_THEME_RECT function=(GET_THEME_RECT)GetProcAddress(themeModule,"GetThemeRect");

        return (*function)(theme,partId,stateId,propId,rect);
      }

      bool HasTheming()
      {
        if (!themeChecked) {
          themeModule=LoadLibrary("UxTheme.dll");
          themeChecked=true;
          if (themeModule!=0) {
            themeAvailable=MyIsAppThemed()!=0;

            if (themeAvailable) {
              buttonTheme=MyOpenThemeData(dynamic_cast<Display*>(display)->appWindow,L"Button");
              comboboxTheme=MyOpenThemeData(dynamic_cast<Display*>(display)->appWindow,L"Combobox");
              editTheme=MyOpenThemeData(dynamic_cast<Display*>(display)->appWindow,L"Edit");
              headerTheme=MyOpenThemeData(dynamic_cast<Display*>(display)->appWindow,L"Header");
              listboxTheme=MyOpenThemeData(dynamic_cast<Display*>(display)->appWindow,L"Listbox");
              listviewTheme=MyOpenThemeData(dynamic_cast<Display*>(display)->appWindow,L"Listview");
              menuTheme=MyOpenThemeData(dynamic_cast<Display*>(display)->appWindow,L"Menu");
              progressTheme=MyOpenThemeData(dynamic_cast<Display*>(display)->appWindow,L"Progress");
              scrollbarTheme=MyOpenThemeData(dynamic_cast<Display*>(display)->appWindow,L"Scrollbar");
              tabTheme=MyOpenThemeData(dynamic_cast<Display*>(display)->appWindow,L"Tab");
              toolbarTheme=MyOpenThemeData(dynamic_cast<Display*>(display)->appWindow,L"Toolbar");
              trackbarTheme=MyOpenThemeData(dynamic_cast<Display*>(display)->appWindow,L"Trackbar");
              windowTheme=MyOpenThemeData(dynamic_cast<Display*>(display)->appWindow,L"Window");

              assert(buttonTheme!=0);
              assert(comboboxTheme!=0);
              assert(editTheme!=0);
              assert(headerTheme!=0);
              assert(listviewTheme!=0);
              // menuTheme is only available starting with Windows Vista
              assert(progressTheme!=0);
              assert(scrollbarTheme!=0);
              assert(tabTheme!=0);
              assert(toolbarTheme!=0);
              assert(trackbarTheme!=0);
              assert(windowTheme!=0);
            }
          }
        }

        return themeAvailable;
      }

      void DrawRectangle(OS::DrawInfo* draw, int x, int y, size_t w, size_t h, bool in)
      {
        if (in) {
          draw->PushForeground(GetSysColor(COLOR_3DSHADOW));
        }
        else {
          draw->PushForeground(GetSysColor(COLOR_3DHIGHLIGHT));
        }

        draw->DrawLine(x,y+h-1,x,y);
        draw->DrawLine(x+1,y,x+w-1,y);
        draw->PopForeground();

        if (in) {
          draw->PushForeground(GetSysColor(COLOR_3DHIGHLIGHT));
        }
        else {
          draw->PushForeground(GetSysColor(COLOR_3DSHADOW));
        }

        draw->DrawLine(x+w-1,y+1,x+w-1,y+h-1);
        draw->DrawLine(x+w-2,y+h-1,x+1,y+h-1);
        draw->PopForeground();
      }

      ThemeImage::ThemeImage(Display::ImageIndex image)
        : image(image)
      {
        switch (image) {
        case Display::hknobImageIndex:
          width=GetSystemMetrics(SM_CXHTHUMB);
          height=GetSystemMetrics(SM_CYHSCROLL);
          break;
        case Display::vknobImageIndex:
          width=GetSystemMetrics(SM_CXVSCROLL);
          height=GetSystemMetrics(SM_CYVTHUMB);
          break;
        case Display::arrowLeftImageIndex:
        case Display::arrowRightImageIndex:
        case Display::arrowUpImageIndex:
        case Display::arrowDownImageIndex:
          width=GetSystemMetrics(SM_CXVSCROLL);
          height=GetSystemMetrics(SM_CYHSCROLL);
          break;
        case Display::comboImageIndex:
          width=GetSystemMetrics(SM_CXVSCROLL);
          height=GetSystemMetrics(SM_CYHSCROLL);
          break;
        case Display::comboEditButtonImageIndex:
          width=GetSystemMetrics(SM_CXVSCROLL);
          height=GetSystemMetrics(SM_CYHSCROLL);
          break;
        case Display::comboDividerImageIndex:
          width=0;
          height=0;
          break;
        case Display::treeExpanderImageIndex:
          width=GetSystemMetrics(SM_CXMENUCHECK);
          height=GetSystemMetrics(SM_CYMENUCHECK);
          break;
        case Display::hscaleKnobImageIndex:
          if (HasTheming()) {
            SIZE size;

            if (MyGetThemePartSize(trackbarTheme,dynamic_cast<Display*>(display)->hdc,TKP_THUMBTOP,TUS_NORMAL,NULL,TS_TRUE,&size)==S_OK) {
              width=size.cx;
              height=size.cy;
            }

          }
          else {
            width=GetSystemMetrics(SM_CXHTHUMB);
            height=GetSystemMetrics(SM_CYHSCROLL);
          }
          break;
        case Display::vscaleKnobImageIndex:
          if (HasTheming()) {
            SIZE size;

            if (MyGetThemePartSize(trackbarTheme,dynamic_cast<Display*>(display)->hdc,TKP_THUMBLEFT,TUS_NORMAL,NULL,TS_TRUE,&size)==S_OK) {
              width=size.cx;
              height=size.cy;
            }
          }
          else {
            width=GetSystemMetrics(SM_CXVSCROLL);
            height=GetSystemMetrics(SM_CYVTHUMB);
          }
          break;
        case Display::leftSliderImageIndex:
        case Display::rightSliderImageIndex:
        case Display::topSliderImageIndex:
        case Display::bottomSliderImageIndex:
          width=0;
          height=0;
          break;
        case Display::menuDividerImageIndex:
          if (HasTheming() && menuTheme!=NULL) {
            SIZE size;

            if (MyGetThemePartSize(menuTheme,dynamic_cast<Display*>(display)->hdc,MENU_SEPARATOR,1,NULL,TS_TRUE,&size)==S_OK) {
              width=size.cx;
              height=size.cy;
            }
          }
          else {
            width=4;
            height=2;
          }
          break;
        case Display::menuSubImageIndex:
          if (HasTheming() && menuTheme!=NULL) {
            SIZE size;

            if (MyGetThemePartSize(menuTheme,dynamic_cast<Display*>(display)->hdc,MENU_POPUPSUBMENU,1,NULL,TS_TRUE,&size)==S_OK) {
              width=size.cx;
              height=size.cy;
            }
          }
          else {
            width=GetSystemMetrics(SM_CXMENUCHECK);
            height=GetSystemMetrics(SM_CYMENUCHECK);
          }
          break;
        case Display::menuCheckImageIndex:
          if (HasTheming() && menuTheme!=NULL) {
            SIZE size;

            if (MyGetThemePartSize(menuTheme,dynamic_cast<Display*>(display)->hdc,MENU_POPUPCHECK,1,NULL,TS_TRUE,&size)==S_OK) {
              width=size.cx;
              height=size.cy;
            }
          }
          else {
            width=GetSystemMetrics(SM_CXMENUCHECK);
            height=GetSystemMetrics(SM_CYMENUCHECK);
          }
          break;
        case Display::menuRadioImageIndex:
          if (HasTheming() && menuTheme!=NULL) {
            SIZE size;

            if (MyGetThemePartSize(menuTheme,dynamic_cast<Display*>(display)->hdc,MENU_POPUPCHECK,1,NULL,TS_TRUE,&size)==S_OK) {
              width=size.cx;
              height=size.cy;
            }
          }
          else {
            width=GetSystemMetrics(SM_CXMENUCHECK);
            height=GetSystemMetrics(SM_CYMENUCHECK);
          }
          break;
          /*
        case Display::closeImageIndex:
          if (HasTheming() && menuTheme!=NULL) {
            SIZE size;

            if (MyGetThemePartSize(menuTheme,dynamic_cast<Display*>(display)->hdc,MENU_SYSTEMCLOSE,1,NULL,TS_TRUE,&size)==S_OK) {
              width=size.cx;
              height=size.cy;
            }
          }
          else {
            width=GetSystemMetrics(SM_CXMENUCHECK)-4;
            height=GetSystemMetrics(SM_CYMENUCHECK)-4;
          }
          break;*/
        case Display::radioImageIndex:
          width=GetSystemMetrics(SM_CXMENUCHECK);
          height=GetSystemMetrics(SM_CYMENUCHECK);
          break;
        case Display::checkImageIndex:
          width=GetSystemMetrics(SM_CXMENUCHECK);
          height=GetSystemMetrics(SM_CYMENUCHECK);
          break;
        case Display::ledImageIndex:
          width=GetSystemMetrics(SM_CXMENUCHECK);
          height=GetSystemMetrics(SM_CYMENUCHECK);
          break;
        }
      }

      size_t ThemeImage::GetWidth() const
      {
        return width;
      }

      size_t ThemeImage::GetHeight() const
      {
        return height;
      }

      bool ThemeImage::GetAlpha() const
      {
        return true;
      }

      unsigned long ThemeImage::GetDrawCap() const
      {
        return drawsDisabled|drawsFocused|drawsActivated;
      }

      void ThemeImage::Draw(OS::DrawInfo *draw, int x, int y)
      {
        DrawStretched(draw,x,y,GetWidth(),GetHeight());
      }

      void ThemeImage::DrawScaled(OS::DrawInfo *draw, int x, int y, size_t w, size_t h)
      {
        // We cannot scale images
        std::cerr << "Image of type " << image << " cannot be scaled!" << std::endl;
        assert(false);
      }

      void ThemeImage::DrawStretched(Lum::OS::DrawInfo *draw, int x, int y, size_t w, size_t h)
      {
        HDC  dc;
        RECT rect;
        int  style;

        if (dynamic_cast<DrawInfo*>(draw)!=NULL) {
          dc=dynamic_cast<DrawInfo*>(draw)->dc;
        }
#if defined(LUM_HAVE_LIB_CAIRO)
        else if (dynamic_cast<Cairo::DrawInfo*>(draw)!=NULL) {
          cairo_surface_flush(dynamic_cast<Cairo::DrawInfo*>(draw)->surface);
          dc=cairo_win32_surface_get_dc(dynamic_cast<Cairo::DrawInfo*>(draw)->surface);
        }
#endif
        else {
          assert(false);
        }

        rect.left=x;
        rect.top=y;
        rect.right=x+w;
        rect.bottom=y+h;

        SetTextAlign(dc,TA_TOP+TA_LEFT);

        switch (image) {
        case Display::hknobImageIndex:
          if (HasTheming()) {
            if (draw->selected) {
              style=SCRBS_PRESSED;
            }
            else if (draw->activated) {
              style=SCRBS_HOT;
            }
            else if (draw->disabled) {
              style=SCRBS_DISABLED;
            }
            else {
              style=SCRBS_NORMAL;
            }
            /* ignore */ MyDrawThemeBackground(scrollbarTheme,dc,SBP_THUMBBTNHORZ,style,&rect,NULL);
            /* ignore */ MyDrawThemeBackground(scrollbarTheme,dc,SBP_GRIPPERHORZ,style,&rect,NULL);


          }
          else {
            draw->PushForeground(GetSysColor(COLOR_BTNFACE));
            draw->FillRectangle(x+2,y+2,w-4,h-4);
            draw->PopForeground();

            /* ignore */ DrawEdge(dc,&rect,EDGE_RAISED,BF_RECT);
          }
          break;
        case Display::vknobImageIndex:
          if (HasTheming()) {
            if (draw->selected) {
              style=SCRBS_PRESSED;
            }
            else if (draw->activated) {
              style=SCRBS_HOT;
            }
            else if (draw->disabled) {
              style=SCRBS_DISABLED;
            }
            else {
              style=SCRBS_NORMAL;
            }
            /* ignore */ MyDrawThemeBackground(scrollbarTheme,dc,SBP_THUMBBTNVERT,style,&rect,NULL);
            /* ignore */ MyDrawThemeBackground(scrollbarTheme,dc,SBP_GRIPPERVERT,style,&rect,NULL);
          }
          else {
            draw->PushForeground(GetSysColor(COLOR_BTNFACE));
            draw->FillRectangle(x+2,y+2,w-4,h-4);
            draw->PopForeground();

            /* ignore */ DrawEdge(dc,&rect,EDGE_RAISED,BF_RECT);
          }
          break;
        case Display::arrowLeftImageIndex:
          if (HasTheming()) {
            if (draw->selected) {
              style=ABS_LEFTPRESSED;
            }
            else if (draw->activated) {
              style=ABS_LEFTHOT;
            }
            else if (draw->disabled) {
              style=ABS_LEFTDISABLED;
            }
            else {
              style=ABS_LEFTNORMAL;
            }
            /* ignore */ MyDrawThemeBackground(scrollbarTheme,dc,SBP_ARROWBTN,style,&rect,NULL);
          }
          else {
            /* ignore */ ::DrawFrameControl(dc,&rect,DFC_SCROLL,DFCS_SCROLLLEFT | (draw->selected ? DFCS_FLAT : 0));
          }
          break;
        case Display::arrowRightImageIndex:
          if (HasTheming()) {
            if (draw->selected) {
              style=ABS_RIGHTPRESSED;
            }
            else if (draw->activated) {
              style=ABS_RIGHTHOT;
            }
            else if (draw->disabled) {
              style=ABS_RIGHTDISABLED;
            }
            else {
              style=ABS_RIGHTNORMAL;
            }

            /* ignore */ MyDrawThemeBackground(scrollbarTheme,dc,SBP_ARROWBTN,style,&rect,NULL);
          }
          else {
            /* ignore */ ::DrawFrameControl(dc,&rect,DFC_SCROLL,DFCS_SCROLLRIGHT | (draw->selected ? DFCS_FLAT : 0));
          }
          break;
        case Display::arrowUpImageIndex:
          if (HasTheming()) {
            if (draw->selected) {
              style=ABS_UPPRESSED;
            }
            else if (draw->activated) {
              style=ABS_UPHOT;
            }
            else if (draw->disabled) {
              style=ABS_UPDISABLED;
            }
            else {
              style=ABS_UPNORMAL;
            }

            /* ignore */ MyDrawThemeBackground(scrollbarTheme,dc,SBP_ARROWBTN,style,&rect,NULL);
          }
          else {
            /* ignore */ ::DrawFrameControl(dc,&rect,DFC_SCROLL,DFCS_SCROLLUP | (draw->selected ? DFCS_FLAT : 0));
          }
          break;
        case Display::arrowDownImageIndex:
          if (HasTheming()) {
            if (draw->selected) {
              style=ABS_DOWNPRESSED;
            }
            else if (draw->activated) {
              style=ABS_DOWNHOT;
            }
            else if (draw->disabled) {
              style=ABS_DOWNDISABLED;
            }
            else {
              style=ABS_DOWNNORMAL;
            }

            /* ignore */ MyDrawThemeBackground(scrollbarTheme,dc,SBP_ARROWBTN,style,&rect,NULL);
          }
          else {
            /* ignore */ ::DrawFrameControl(dc,&rect,DFC_SCROLL,DFCS_SCROLLDOWN | (draw->selected ? DFCS_FLAT : 0));
          }
          break;
        case Display::comboImageIndex:
          if (HasTheming()) {
            DrawInfo::Point points[3];

            if (draw->disabled) {
              draw->PushForeground(GetSysColor(COLOR_3DLIGHT));
            }
            else {
              draw->PushForeground(RGB(0,0,0));
            }

            points[0].x=x+w/5;
            points[0].y=y+h/5;
            points[1].x=x+w-1-w/5;
            points[1].y=y+h/5;
            points[2].x=x+w / 2;
            points[2].y=y+h-1-h/5;
            draw->FillPolygon(points,3);
            draw->PopForeground();
          }
          else {
            DrawInfo::Point points[3];

            draw->PushForeground(GetSysColor(COLOR_BTNFACE));
            draw->FillRectangle(x,y,w,h);
            draw->PopForeground();

            if (draw->disabled) {
              draw->PushForeground(GetSysColor(COLOR_3DLIGHT));
            }
            else {
              draw->PushForeground(RGB(0,0,0));
            }

            points[0].x=x+w/5;
            points[0].y=y+h/5;
            points[1].x=x+w-1-w/5;
            points[1].y=y+h/5;
            points[2].x=x+w / 2;
            points[2].y=y+h-1-h/5;
            draw->FillPolygon(points,3);
            draw->PopForeground();
          }
          break;
        case Display::comboEditButtonImageIndex:
          if (HasTheming()) {
            if (draw->disabled) {
              style=CBXS_DISABLED;
            }
            else if (draw->selected) {
              style=CBXS_PRESSED;
            }
            else if (draw->activated) {
              style=CBXS_HOT;
            }
            else {
              style=CBXS_NORMAL;
            }

            MyDrawThemeBackground(comboboxTheme,dc,CP_DROPDOWNBUTTON,style,&rect,NULL);
          }
          else {
            /* ignore */ ::DrawFrameControl(dc,&rect,DFC_SCROLL,DFCS_SCROLLCOMBOBOX | (draw->selected ? DFCS_FLAT : 0));
          }
          break;
        case Display::treeExpanderImageIndex: {
          DrawInfo::Point points[3];

          draw->PushForeground(GetSysColor(COLOR_3DSHADOW));
          if (draw->selected) {
            // arrowDown
            points[0].x=x+w/4;
            points[0].y=y+h/4;
            points[1].x=x+w-w/4;
            points[1].y=y+h/4;
            points[2].x=x+w/2;
            points[2].y=y+h-h/4;
            draw->FillPolygon(points,3);
          }
          else {
            // arrowRight
            points[0].x=x+w-w/4;
            points[0].y=y+h/2;
            points[1].x=x+w/4;
            points[1].y=y+h/4;
            points[2].x=x+w/4;
            points[2].y=y+h-h/4;
            draw->FillPolygon(points,3);
          }
          draw->PopForeground();
          }
          break;
        case Display::hscaleKnobImageIndex:
          if (HasTheming()) {
            if (draw->selected) {
              style=TUS_PRESSED;
            }
            else if (draw->activated) {
              style=TUS_HOT;
            }
            else if (draw->disabled) {
              style=TUS_DISABLED;
            }
            else if (draw->focused) {
              style=TUS_FOCUSED;
            }
            else {
              style=TUS_NORMAL;
            }
            /* ignore */ MyDrawThemeBackground(trackbarTheme,dc,TKP_THUMBTOP,style,&rect,NULL);
          }
          else {
            DrawRectangle(draw,x,y,w,h,false);
            DrawRectangle(draw,x+1,y+1,w-2,h-2,false);

            draw->PushForeground(GetSysColor(COLOR_BTNFACE));
            draw->FillRectangle(x+2,y+2,w-4,h-4);
            draw->PopForeground();

            draw->PushForeground(GetSysColor(COLOR_3DSHADOW));
            draw->DrawLine(x+(w-1)/2,y+2,x+(w-1)/2,y+h-3);
            draw->PopForeground();
            draw->PushForeground(GetSysColor(COLOR_3DHIGHLIGHT));
            draw->DrawLine(x+(w-1)/2+1,y+2,x+(w-1)/2+1,y+h-3);
            draw->PopForeground();
          }
          break;
        case Display::vscaleKnobImageIndex:
          if (HasTheming()) {
            if (draw->selected) {
              style=TUVS_PRESSED;
            }
            else if (draw->activated) {
              style=TUVS_HOT;
            }
            else if (draw->disabled) {
              style=TUVS_DISABLED;
            }
            else if (draw->focused) {
              style=TUVS_FOCUSED;
            }
            else {
              style=TUVS_NORMAL;
            }
            /* ignore */ MyDrawThemeBackground(trackbarTheme,dc,TKP_THUMBLEFT,style,&rect,NULL);
          }
          else {
            DrawRectangle(draw,x,y,w,h,false);
            DrawRectangle(draw,x+1,y+1,w-2,h-2,false);

            draw->PushForeground(GetSysColor(COLOR_BTNFACE));
            draw->FillRectangle(x+2,y+2,w-4,h-4);
            draw->PopForeground();

            draw->PushForeground(GetSysColor(COLOR_3DSHADOW));
            draw->DrawLine(x+2,y+h/2,x+w-3,y+h/2);
            draw->PopForeground();
            draw->PushForeground(GetSysColor(COLOR_3DHIGHLIGHT));
            draw->DrawLine(x+2,y+h/2+1,x+w-3,y+h/2+1);
            draw->PopForeground();
          }
          break;
        case Display::leftSliderImageIndex:
          if (HasTheming()) {
            size_t height=rect.bottom-rect.top;

            rect.top+=height/3;
            rect.bottom-=height/3;
            /* ignore */ MyDrawThemeBackground(trackbarTheme,dc,TKP_TRACK,TRS_NORMAL,&rect,NULL);
          }
          else {
            draw->PushForeground(GetSysColor(COLOR_HIGHLIGHT));
            draw->FillRectangle(x,y,w,h);
            draw->PopForeground();
          }
          break;
        case Display::rightSliderImageIndex:
          if (HasTheming()) {
            size_t height=rect.bottom-rect.top;

            rect.top+=height/3;
            rect.bottom-=height/3;
            /* ignore */ MyDrawThemeBackground(trackbarTheme,dc,TKP_TRACK,TRS_NORMAL,&rect,NULL);
          }
          else {
            draw->PushForeground(GetSysColor(COLOR_BTNFACE));
            draw->FillRectangle(x,y,w,h);
            draw->PopForeground();
          }
          break;
        case Display::topSliderImageIndex:
          if (HasTheming()) {
            size_t width=rect.right-rect.left;

            rect.left+=width/3;
            rect.right-=width/3;
            /* ignore */ MyDrawThemeBackground(trackbarTheme,dc,TKP_TRACKVERT,TRVS_NORMAL,&rect,NULL);
          }
          else {
            draw->PushForeground(GetSysColor(COLOR_BTNFACE));
            draw->FillRectangle(x,y,w,h);
            draw->PopForeground();
          }
          break;
        case Display::bottomSliderImageIndex:
          if (HasTheming()) {
            size_t width=rect.right-rect.left;

            rect.left+=width/3;
            rect.right-=width/3;
            /* ignore */ MyDrawThemeBackground(trackbarTheme,dc,TKP_TRACKVERT,TRVS_NORMAL,&rect,NULL);
          }
          else {
            draw->PushForeground(GetSysColor(COLOR_HIGHLIGHT));
            draw->FillRectangle(x,y,w,h);
            draw->PopForeground();
          }
          break;
        case Display::menuDividerImageIndex:
          if (HasTheming() && menuTheme!=NULL) {
            /* ignore */ MyDrawThemeBackground(menuTheme,dc,MENU_SEPARATOR,1,&rect,NULL);
          }
          else {
            draw->PushForeground(GetSysColor(COLOR_3DSHADOW));
            draw->DrawLine(x,y,x+w-1,y);
            draw->PopForeground();
            draw->PushForeground(GetSysColor(COLOR_3DHIGHLIGHT));
            draw->DrawLine(x,y+1,x+w-1,y+1);
            draw->PopForeground();
          }
          break;
        case Display::menuSubImageIndex:
          if (HasTheming() && menuTheme!=NULL) {
            /* ignore */ MyDrawThemeBackground(menuTheme,dc,MENU_POPUPSUBMENU,1,&rect,NULL);
          }
          else {
            OS::DrawInfo::Point points[3];

            if (draw->disabled) {
              draw->PushForeground(GetSysColor(COLOR_3DSHADOW));
            }
            else {
              draw->PushForeground(RGB(0,0,0));
            }
            points[0].x=x+width-1-width/5;
            points[0].y=y+height / 2;
            points[1].x=x+width/5;
            points[1].y=y+height/5;
            points[2].x=x+width/5;
            points[2].y=y+height-1-height/5;
            draw->FillPolygon(points,3);
            draw->PopForeground();
          }
          break;
        case Display::menuCheckImageIndex:
          if (HasTheming() && menuTheme!=NULL) {
            if (draw->selected) {
              if (draw->disabled) {
                /* ignore */ MyDrawThemeBackground(menuTheme,dc,MENU_POPUPCHECKBACKGROUND,1,&rect,NULL);
                /* ignore */ MyDrawThemeBackground(menuTheme,dc,MENU_POPUPCHECK,2,&rect,NULL);
              }
              else {
                /* ignore */ MyDrawThemeBackground(menuTheme,dc,MENU_POPUPCHECKBACKGROUND,2,&rect,NULL);
                /* ignore */ MyDrawThemeBackground(menuTheme,dc,MENU_POPUPCHECK,1,&rect,NULL);
              }
            }
          }
          else {
           /* ignore */ ::DrawFrameControl(dc,&rect,
                                            DFC_BUTTON,
                                            DFCS_BUTTONCHECK | (draw->selected ? DFCS_CHECKED : 0));
          }
          break;
        case Display::menuRadioImageIndex:
          if (HasTheming() && menuTheme!=NULL) {
            if (draw->selected) {
              if (draw->disabled) {
                /* ignore */ MyDrawThemeBackground(menuTheme,dc,MENU_POPUPCHECKBACKGROUND,1,&rect,NULL);
                /* ignore */ MyDrawThemeBackground(menuTheme,dc,MENU_POPUPCHECK,4,&rect,NULL);
              }
              else {
                /* ignore */ MyDrawThemeBackground(menuTheme,dc,MENU_POPUPCHECKBACKGROUND,2,&rect,NULL);
                /* ignore */ MyDrawThemeBackground(menuTheme,dc,MENU_POPUPCHECK,3,&rect,NULL);
              }
            }
          }
          else {
            /* ignore */ ::DrawFrameControl(dc,&rect,
                                            DFC_BUTTON,
                                            DFCS_BUTTONRADIO | (draw->selected ? DFCS_CHECKED : 0));
          }
          break;
#if 0
        case Display::closeImageIndex:
          if (HasTheming() && menuTheme!=NULL) {
            /* ignore */ MyDrawThemeBackground(menuTheme,dc,MENU_SYSTEMCLOSE,1,&rect,NULL);
          }
          else {
            draw->PushForeground(GetSysColor(COLOR_BTNFACE));
            draw->FillRectangle(x,y,w,h);
            draw->PopForeground();

            draw->PushPen(2,DrawInfo::penNormal);
            draw->PushForeground(RGB(0,0,0));
            draw->DrawLine(x,y,x+w-1,y+h-1);
            draw->DrawLine(x+w-1,y,x,y+h-1);
            draw->PopForeground();
            draw->PopPen();
          }
          break;
#endif
        case Display::radioImageIndex:
          if (HasTheming()) {
            if (draw->selected) {
              if (draw->activated) {
                style=RBS_CHECKEDHOT;
              }
              else if (draw->disabled) {
                style=RBS_CHECKEDDISABLED;
              }
              else {
                style=RBS_CHECKEDNORMAL;
              }
            }
            else if (draw->activated) {
              style=RBS_UNCHECKEDHOT;
            }
            else if (draw->disabled) {
              style=RBS_UNCHECKEDDISABLED;
            }
            else {
              style=RBS_UNCHECKEDNORMAL;
            }

            /* ignore */ MyDrawThemeBackground(buttonTheme,dc,BP_RADIOBUTTON,style,&rect,NULL);
          }
          else {
            /* ignore */ ::DrawFrameControl(dc,&rect,
                                            DFC_BUTTON,
                                            DFCS_BUTTONRADIO | (draw->selected ? DFCS_CHECKED : 0));
          }
          break;
        case Display::checkImageIndex:
          if (HasTheming()) {
            if (draw->selected) {
              if (draw->activated) {
                style=CBS_CHECKEDHOT;
              }
              else if (draw->disabled) {
                style=CBS_CHECKEDDISABLED;
              }
              else {
                style=CBS_CHECKEDNORMAL;
              }
            }
            else if (draw->activated) {
              style=CBS_UNCHECKEDHOT;
            }
              else if (draw->disabled) {
                style=CBS_UNCHECKEDDISABLED;
              }
            else {
              style=CBS_UNCHECKEDNORMAL;
            }

            /* ignore */ MyDrawThemeBackground(buttonTheme,dc,BP_CHECKBOX,style,&rect,NULL);
          }
          else {
           /* ignore */ ::DrawFrameControl(dc,&rect,
                                            DFC_BUTTON,
                                            DFCS_BUTTONCHECK | (draw->selected ? DFCS_CHECKED : 0));
          }
          break;
        case Display::ledImageIndex:
          if (draw->selected) {
            draw->PushForeground(GetSysColor(COLOR_HIGHLIGHT));
            draw->FillArc(x,y,width-1,height-1,0*64,360*64);
            draw->PopForeground();
          }
          else {
            draw->PushForeground(GetSysColor(COLOR_3DSHADOW));
            draw->FillArc(x,y,width-1,height-1,0*64,360*64);
            draw->PopForeground();
          }

          draw->PushForeground(GetSysColor(COLOR_3DHIGHLIGHT));
          draw->DrawArc(x+width / 4,y+height / 4,
                       width / 2,height / 2,60*64,90*64);
          draw->PopForeground();

          draw->PushForeground(GetSysColor(COLOR_3DSHADOW));
          draw->DrawArc(x,y,width-1,height-1,0*64,360*64);
          draw->PopForeground();
          break;
        }

        SetTextAlign(dc,TA_BASELINE+TA_LEFT);

#if defined(LUM_HAVE_LIB_CAIRO)
        if (dynamic_cast<Cairo::DrawInfo*>(draw)!=NULL) {
          cairo_surface_mark_dirty(dynamic_cast<Cairo::DrawInfo*>(draw)->surface);
        }
#endif
      }


      Fill::Fill(Display::FillIndex fill)
        : fill(fill)
      {
        switch (fill) {
        case Display::buttonBackgroundFillIndex:
        case Display::positiveButtonBackgroundFillIndex:
        case Display::negativeButtonBackgroundFillIndex:
        case Display::defaultButtonBackgroundFillIndex:
          if (HasTheming()) {
            RECT content,extent;

            content.left=0;
            content.right=0;
            content.top=0;
            content.bottom=0;
            MyGetThemeBackgroundExtent(buttonTheme,NULL,BP_PUSHBUTTON,PBS_NORMAL,&content,&extent);
            leftBorder=-extent.left;
            rightBorder=extent.right;
            topBorder=-extent.top;
            bottomBorder=extent.bottom;
            transparentBorder=true;
          }
          else {
            leftBorder=GetSystemMetrics(SM_CXEDGE);
            rightBorder=leftBorder;
            topBorder=GetSystemMetrics(SM_CYEDGE);
            bottomBorder=topBorder;
          }

          // 1 pixel for the focus frame and two pixel for the space between frame and content
          leftBorder+=1+2;
          rightBorder+=1+2;
          topBorder+=1+2;
          bottomBorder+=1+2;
          break;
        case Display::toolbarButtonBackgroundFillIndex:
          if (HasTheming()) {
            RECT content,extent;

            content.left=0;
            content.right=0;
            content.top=0;
            content.bottom=0;
            MyGetThemeBackgroundExtent(toolbarTheme,NULL,TP_BUTTON,TS_NORMAL,&content,&extent);
            leftBorder=-extent.left;
            rightBorder=extent.right;
            topBorder=-extent.top;
            bottomBorder=extent.bottom;
            transparentBorder=true;
          }
          else {
            leftBorder=GetSystemMetrics(SM_CXEDGE);
            rightBorder=leftBorder;
            topBorder=GetSystemMetrics(SM_CYEDGE);
            bottomBorder=topBorder;
          }
          break;
        case Display::comboBackgroundFillIndex:
          if (HasTheming()) {
            RECT content,extent;

            content.left=0;
            content.right=0;
            content.top=0;
            content.bottom=0;
            MyGetThemeBackgroundExtent(buttonTheme,NULL,BP_PUSHBUTTON,PBS_NORMAL,&content,&extent);
            leftBorder=-extent.left;
            rightBorder=extent.right;
            topBorder=-extent.top;
            bottomBorder=extent.bottom;
            transparentBorder=true;
          }
          else {
            leftBorder=GetSystemMetrics(SM_CXEDGE);
            rightBorder=leftBorder;
            topBorder=GetSystemMetrics(SM_CYEDGE);
            bottomBorder=topBorder;
          }

          // 1 pixel for the focus frame and one pixel for the space between frame and content
          leftBorder+=1+1;
          rightBorder+=1+1;
          topBorder+=1+1;
          bottomBorder+=1+1;
          break;
        case Display::editComboBackgroundFillIndex:
          if (HasTheming()) {
          }
          else {
            leftBorder=GetSystemMetrics(SM_CXEDGE);
            rightBorder=leftBorder;
            topBorder=GetSystemMetrics(SM_CYEDGE);
            bottomBorder=topBorder;
          }
          break;
        case Display::entryBackgroundFillIndex:
          if (HasTheming()) {
            RECT content,extent;

            content.left=0;
            content.right=0;
            content.top=0;
            content.bottom=0;

            MyGetThemeBackgroundExtent(editTheme,NULL,EP_EDITTEXT,ETS_NORMAL,&content,&extent);
            leftBorder=-extent.left;
            rightBorder=extent.right;
            topBorder=-extent.top;
            bottomBorder=extent.bottom;
            transparentBorder=true;
          }
          else {
            leftBorder=GetSystemMetrics(SM_CXEDGE);
            rightBorder=leftBorder;
            topBorder=GetSystemMetrics(SM_CYEDGE);
            bottomBorder=topBorder;
          }
          // We manually add some space in all directions, since there is no way to
          // get a theme specific value for the space between the actual border and its content.
          leftBorder+=3;
          rightBorder+=3;
          topBorder+=3;
          bottomBorder+=3;
          break;
        case Display::columnBackgroundFillIndex:
        case Display::columnLeftBackgroundFillIndex:
        case Display::columnMiddleBackgroundFillIndex:
        case Display::columnRightBackgroundFillIndex:
        case Display::columnEndBackgroundFillIndex:
          if (HasTheming()) {
            RECT content,extent;

            content.left=0;
            content.right=0;
            content.top=0;
            content.bottom=0;

            MyGetThemeBackgroundExtent(headerTheme,NULL,HP_HEADERITEM,HIS_NORMAL,&content,&extent);
            leftBorder=-extent.left;
            rightBorder=extent.right;
            topBorder=-extent.top;
            bottomBorder=extent.bottom;
          }
          else {
            leftBorder=GetSystemMetrics(SM_CXEDGE);
            rightBorder=leftBorder;
            topBorder=GetSystemMetrics(SM_CYEDGE);
            bottomBorder=topBorder;
          }
          // We manually add some space in all directions, since there is no way to
          // get a theme specific value for the space between the actual border and its content.
          leftBorder+=3;
          rightBorder+=3;
          topBorder+=3;
          bottomBorder+=3;
          break;
        case Display::hprogressBackgroundFillIndex:
          if (HasTheming()) {
            RECT content,extent;

            content.left=0;
            content.right=0;
            content.top=0;
            content.bottom=0;
            MyGetThemeBackgroundExtent(progressTheme,NULL,PP_BAR,1,&content,&extent);
            leftBorder=-extent.left;
            rightBorder=extent.right;
            topBorder=-extent.top;
            bottomBorder=extent.bottom;
          }
          else {
            leftBorder=GetSystemMetrics(SM_CXEDGE);
            rightBorder=leftBorder;
            topBorder=GetSystemMetrics(SM_CYEDGE);
            bottomBorder=topBorder;
          }
          break;
        case Display::vprogressBackgroundFillIndex:
          if (HasTheming()) {
            RECT content,extent;

            content.left=0;
            content.right=0;
            content.top=0;
            content.bottom=0;
            MyGetThemeBackgroundExtent(progressTheme,NULL,PP_BARVERT,1,&content,&extent);
            leftBorder=-extent.left;
            rightBorder=extent.right;
            topBorder=-extent.top;
            bottomBorder=extent.bottom;
          }
          else {
            leftBorder=GetSystemMetrics(SM_CXEDGE);
            rightBorder=leftBorder;
            topBorder=GetSystemMetrics(SM_CYEDGE);
            bottomBorder=topBorder;
          }
          break;
        case Display::menuWindowBackgroundFillIndex:
          leftBorder=2;
          rightBorder=leftBorder;
          topBorder=2;
          bottomBorder=topBorder;
          break;
        case Display::popupWindowBackgroundFillIndex:
          leftBorder=2;
          rightBorder=leftBorder;
          topBorder=2;
          bottomBorder=topBorder;
          break;
        case Display::dialogWindowBackgroundFillIndex:
          leftBorder=0;
          rightBorder=leftBorder;
          topBorder=0;
          bottomBorder=topBorder;
          break;
        case Display::tooltipWindowBackgroundFillIndex:
          leftBorder=1;
          rightBorder=leftBorder;
          topBorder=1;
          bottomBorder=topBorder;
          break;
        case Display::menuPulldownBackgroundFillIndex:
          if (HasTheming() && menuTheme!=NULL) {
            RECT content,extent;

            content.left=0;
            content.right=0;
            content.top=0;
            content.bottom=0;

            MyGetThemeBackgroundExtent(menuTheme,NULL,MENU_BARITEM,1 /* TODO */,&content,&extent);
            leftBorder=-extent.left;
            rightBorder=extent.right;
            topBorder=-extent.top;
            bottomBorder=extent.bottom;
            transparentBorder=true;
          }
          else {
            leftBorder=2;
            rightBorder=2;
            topBorder=2;
            bottomBorder=2;
          }
          break;
        case Display::graphBackgroundFillIndex:
          leftBorder=GetSystemMetrics(SM_CXEDGE);
          rightBorder=leftBorder;
          topBorder=GetSystemMetrics(SM_CYEDGE);
          bottomBorder=topBorder;
          break;
        case Display::scrolledBackgroundFillIndex:
          if (HasTheming()) {
            RECT content,extent;

            content.left=0;
            content.right=0;
            content.top=0;
            content.bottom=0;
            MyGetThemeBackgroundExtent(listboxTheme,NULL,LBCP_BORDER_NOSCROLL,0,&content,&extent);
            leftBorder=-extent.left;
            rightBorder=extent.right;
            topBorder=-extent.top;
            bottomBorder=extent.bottom;
            transparentBorder=true;
          }
          else {
            leftBorder=GetSystemMetrics(SM_CXEDGE);
            rightBorder=leftBorder;
            topBorder=GetSystemMetrics(SM_CYEDGE);
            bottomBorder=topBorder;
          }
          break;
        case Display::boxedBackgroundFillIndex:
          leftBorder=GetSystemMetrics(SM_CXEDGE);
          rightBorder=leftBorder;
          topBorder=GetSystemMetrics(SM_CYEDGE);
          bottomBorder=topBorder;
          break;
        case Display::plateBackgroundFillIndex:
          leftBorder=1;
          rightBorder=1;
          topBorder=1;
          bottomBorder=1;
          break;
        case Display::labelBackgroundFillIndex:
          leftBorder=1;
          rightBorder=leftBorder;
          topBorder=1;
          bottomBorder=topBorder;
          transparentBorder=true;
          break;
        case Display::tabRiderBackgroundFillIndex:
          if (HasTheming()) {
            RECT content,extent;

            content.left=0;
            content.right=0;
            content.top=0;
            content.bottom=0;

            MyGetThemeBackgroundExtent(tabTheme,NULL,TABP_TOPTABITEM,TTIS_NORMAL,&content,&extent);

            leftBorder=-extent.left;
            rightBorder=extent.right;
            topBorder=-extent.top;
            bottomBorder=extent.bottom;
            transparentBorder=true;
          }
          else {
            leftBorder=2;
            rightBorder=leftBorder;
            topBorder=2;
            bottomBorder=0;
            transparentBorder=true;
          }
          break;
        }
      }

      void Fill::Draw(Lum::OS::DrawInfo* draw,
                      int xOff, int yOff, size_t width, size_t height,
                      int x, int y, size_t w, size_t h)
      {
        Bitmap *bitmap;
        HBRUSH brush;
        HDC    dc=0;
        RECT   content,clip;
        int    style;
        bool   fillRect=true;

        bitmap=driver->CreateBitmap(width,height);
        if (bitmap==NULL) {
          std::cerr << "Cannot create bitmap" << std::endl;
          return;
        }

        if (transparentBorder) {
          draw->CopyToBitmap(xOff,yOff,width,height,0,0,bitmap);
        }

#if defined(LUM_HAVE_LIB_CAIRO)
        if (dynamic_cast<Cairo::Win32DrawInfo*>(draw)!=NULL) {
          cairo_surface_flush(dynamic_cast<Cairo::DrawInfo*>(bitmap->GetDrawInfo())->surface);
          dc=cairo_win32_surface_get_dc(dynamic_cast<Cairo::DrawInfo*>(bitmap->GetDrawInfo())->surface);
        }
#endif
        if (dynamic_cast<DrawInfo*>(draw)!=NULL) {
          dc=dynamic_cast<DrawInfo*>(bitmap->GetDrawInfo())->dc;
        }
        else {
          assert(true);
        }

        // Original background area
        clip.left=x-xOff;
        clip.top=y-yOff;
        clip.right=content.left+w;
        clip.bottom=content.top+h;

        // Area the background area should be cliped to
        content.left=0;
        content.right=width;
        content.top=0;
        content.bottom=height;

        switch (fill) {
        case Display::backgroundFillIndex:
          if (HasTheming()) {
            RECT extent;

            MyGetThemeBackgroundExtent(windowTheme,dc,WP_DIALOG,1,&content,&extent);
            MyDrawThemeBackground(windowTheme,dc,WP_DIALOG,1,&extent,&clip);

            return;
          }
          else {
            brush=::GetSysColorBrush(COLOR_3DFACE);
          }
          break;
        case Display::tableBackgroundFillIndex:
        case Display::tableBackground2FillIndex:
          if (HasTheming()) {
            if (draw->selected) {
              if (draw->focused) {
                brush=::GetSysColorBrush(COLOR_HIGHLIGHT);
              }
              else {
                brush=::GetSysColorBrush(COLOR_BTNFACE);
              }
            }
            else {
              brush=::GetSysColorBrush(COLOR_WINDOW);
            }
          }
          else {
            if (draw->selected) {
              if (draw->focused) {
                brush=::GetSysColorBrush(COLOR_HIGHLIGHT);
              }
              else {
                brush=::GetSysColorBrush(COLOR_BTNFACE);
              }
            }
            else {
              brush=::GetSysColorBrush(COLOR_WINDOW);
            }
          }
          break;
        case Display::buttonBackgroundFillIndex:
        case Display::positiveButtonBackgroundFillIndex:
        case Display::negativeButtonBackgroundFillIndex:
        case Display::defaultButtonBackgroundFillIndex:
          if (HasTheming()) {
            if (draw->disabled) {
              style=PBS_DISABLED;
            }
            else if (draw->selected) {
              style=PBS_PRESSED;
            }
            else if (draw->activated) {
              style=PBS_HOT;
            }
            else if (draw->focused) {
              style=PBS_DEFAULTED;
            }
            else {
              style=PBS_NORMAL;
            }

            MyDrawThemeBackground(buttonTheme,dc,BP_PUSHBUTTON,style,&content,&clip);

            if (draw->focused && !draw->disabled) {
              RECT rect2;

              rect2.left=content.left+leftBorder-3;
              rect2.right=content.right-rightBorder+3;
              rect2.top=content.top+topBorder-3;
              rect2.bottom=content.bottom-bottomBorder+3;

              /* ignore */ DrawFocusRect(dc,&rect2);
            }

            fillRect=false;
          }
          else {
            /* ignore */ DrawFrameControl(dc,
                                          &content,
                                          DFC_BUTTON,
                                          DFCS_BUTTONPUSH | (draw->selected ? DFCS_PUSHED : 0));
            fillRect=false;
          }
          break;
        case Display::scrollButtonBackgroundFillIndex:
          if (HasTheming()) {
            RECT extent;

            if (draw->disabled) {
              style=PBS_DISABLED;
            }
            else if (draw->selected) {
              style=PBS_PRESSED;
            }
            else if (draw->activated) {
              style=PBS_HOT;
            }
            else if (draw->focused) {
              style=PBS_DEFAULTED;
            }
            else {
              style=PBS_NORMAL;
            }

            MyGetThemeBackgroundExtent(buttonTheme,dc,BP_PUSHBUTTON,style,&content,&extent);
            MyDrawThemeBackground(buttonTheme,dc,BP_PUSHBUTTON,style,&extent,&clip);

            fillRect=false;
          }
          else {
            brush=::GetSysColorBrush(COLOR_BTNFACE);
          }
          break;
        case Display::comboBackgroundFillIndex:
          if (HasTheming()) {
            if (draw->disabled) {
              style=PBS_DISABLED;
            }
            else if (draw->selected) {
              style=PBS_PRESSED;
            }
            else if (draw->activated) {
              style=PBS_HOT;
            }
            else if (draw->focused) {
              style=PBS_DEFAULTED;
            }
            else {
              style=PBS_NORMAL;
            }

            MyDrawThemeBackground(buttonTheme,dc,BP_PUSHBUTTON,style,&content,&clip);

            if (draw->focused && !draw->disabled) {
              RECT rect2;

              rect2.left=content.left+leftBorder-2;
              rect2.right=content.right-rightBorder+2;
              rect2.top=content.top+topBorder-2;
              rect2.bottom=content.bottom-bottomBorder+2;

              /* ignore */ DrawFocusRect(dc,&rect2);
            }

            fillRect=false;
          }
          else {
            /* ignore */ DrawFrameControl(dc,
                                          &content,
                                          DFC_BUTTON,
                                          DFCS_BUTTONPUSH | (draw->selected ? DFCS_PUSHED : 0));
            fillRect=false;
          }
          break;
        case Display::editComboBackgroundFillIndex:
          if (HasTheming()) {
              return;
          }
          else {
            /* ignore */ DrawEdge(dc,&content,EDGE_SUNKEN,BF_RECT);
            fillRect=false;
          }
          break;
        case Display::toolbarButtonBackgroundFillIndex:
          if (HasTheming()) {
            if (draw->disabled) {
              style=TS_DISABLED;
            }
            else if (draw->selected) {
              style=TS_PRESSED;
            }
            else if (draw->activated) {
              style=TS_HOT;
            }
            else {
              style=TS_NORMAL;
            }

            if (MyIsThemeBackgroundPartiallyTransparent(toolbarTheme,TP_BUTTON,style)) {
              brush=::GetSysColorBrush(COLOR_BTNFACE);

              /* ignore */ FillRect(dc,&clip,brush);
            }

            MyDrawThemeBackground(toolbarTheme,dc,TP_BUTTON,style,&content,&clip);

            fillRect=false;
          }
          else {
            /* ignore */ DrawFrameControl(dc,
                                          &content,
                                          DFC_BUTTON,
                                          DFCS_BUTTONPUSH | (draw->selected ? DFCS_PUSHED : 0));
            fillRect=false;
          }
          break;
        case Display::entryBackgroundFillIndex:
          if (HasTheming()) {
            /* ignore */ MyDrawThemeBackground(editTheme,dc,EP_EDITTEXT,ETS_NORMAL,&content,&clip);
          }
          else {
            /* ignore */ DrawEdge(dc,&content,EDGE_SUNKEN,BF_RECT);
          }

          if (draw->selected) {
            if (draw->focused) {
              brush=::GetSysColorBrush(COLOR_HIGHLIGHT);
            }
            else {
            brush=::GetSysColorBrush(COLOR_BTNFACE);
            }
          }
          else {
            brush=::GetSysColorBrush(COLOR_WINDOW);
          }

          content.left+=leftBorder;
          content.right-=rightBorder;
          content.top+=topBorder;
          content.bottom-=bottomBorder;

          /* ignore */ FillRect(dc,&content,brush);

          fillRect=false;
          break;
        case Display::hprogressBackgroundFillIndex:
          if (HasTheming()) {
            MyDrawThemeBackground(progressTheme,dc,PP_BAR,1,&content,&clip);

            fillRect=false;
          }
          else {
            /* ignore */ FillRect(dc,&content,::GetSysColorBrush(COLOR_BTNFACE));
            /* ignore */ DrawEdge(dc,&content,EDGE_SUNKEN,BF_RECT);
            fillRect=false;
          }
          break;
        case Display::vprogressBackgroundFillIndex:
          if (HasTheming()) {
            MyDrawThemeBackground(progressTheme,dc,PP_BARVERT,1,&content,&clip);

            fillRect=false;
          }
          else {
            /* ignore */ FillRect(dc,&content,::GetSysColorBrush(COLOR_BTNFACE));
            /* ignore */ DrawEdge(dc,&content,EDGE_SUNKEN,BF_RECT);
            fillRect=false;
          }
          break;
        case Display::hprogressFillIndex:
          if (HasTheming()) {
            RECT extent;
            int  chunkSize;
            int  spaceSize;

            MyGetThemeInt(progressTheme,PP_CHUNK,1,TMT_PROGRESSCHUNKSIZE,&chunkSize);
            MyGetThemeInt(progressTheme,PP_CHUNK,1,TMT_PROGRESSSPACESIZE,&spaceSize);

            MyGetThemeBackgroundExtent(progressTheme,dc,PP_BAR,1,&content,&extent);
            MyDrawThemeBackground(progressTheme,dc,PP_BAR,1,&extent,&clip);

            content.left=0;
            content.right=content.left+chunkSize;
            content.top=0;
            content.bottom=height;

            while (content.right<=width) {
              MyDrawThemeBackground(progressTheme,dc,PP_CHUNK,1,&content,NULL);
              content.left+=(chunkSize+spaceSize);
              content.right=content.left+chunkSize;
            }

            //content.right=x+w;
            //MyDrawThemeBackground(progressTheme,dc,PP_CHUNK,1,&content,NULL);

            fillRect=false;
          }
          else {
            brush=::GetSysColorBrush(COLOR_HIGHLIGHT);
          }
          break;
        case Display::vprogressFillIndex:
          if (HasTheming()) {
            RECT extent;
            int  chunkSize;
            int  spaceSize;

            MyGetThemeInt(progressTheme,PP_CHUNKVERT,1,TMT_PROGRESSCHUNKSIZE,&chunkSize);
            MyGetThemeInt(progressTheme,PP_CHUNKVERT,1,TMT_PROGRESSSPACESIZE,&spaceSize);

            MyGetThemeBackgroundExtent(progressTheme,dc,PP_BARVERT,1,&content,&extent);
            MyDrawThemeBackground(progressTheme,dc,PP_BARVERT,1,&extent,&clip);

            content.left=0;
            content.right=width;
            content.bottom=height;
            content.top=content.bottom-chunkSize;

            while (content.top>=0) {
              MyDrawThemeBackground(progressTheme,dc,PP_CHUNKVERT,1,&content,NULL);
              content.bottom-=(chunkSize+spaceSize);
              content.top=content.bottom-chunkSize;
            }

            //content.top=y;
            //MyDrawThemeBackground(progressTheme,dc,PP_CHUNKVERT,1,&content,NULL);

            fillRect=false;
          }
          else {
            brush=::GetSysColorBrush(COLOR_HIGHLIGHT);
          }
          break;
        case Display::hknobBoxFillIndex:
        case Display::vknobBoxFillIndex:
          if (HasTheming()) {
            if (fill==Display::hknobBoxFillIndex) {
              MyDrawThemeBackground(scrollbarTheme,dc,SBP_LOWERTRACKHORZ,SCRBS_NORMAL,&content,&clip);
            }
            else {
              MyDrawThemeBackground(scrollbarTheme,dc,SBP_LOWERTRACKVERT,SCRBS_NORMAL,&content,&clip);
            }

            fillRect=false;
          }
          else {
            HBITMAP  bitmap;
            HBRUSH   brush;
            COLORREF oldTextColor;
            COLORREF oldBgColor;
            WORD     data[]= {0xaa, 0x55};

            bitmap=::CreateBitmap(8,2,1,1,data);
            brush=::CreatePatternBrush(bitmap);
            oldTextColor=::SetTextColor(dc,GetSysColor(COLOR_WINDOW));
            oldBgColor=::SetBkColor(dc,GetSysColor(COLOR_SCROLLBAR));

            /* ignore */ ::FillRect(dc,&content,brush);

            /* ignore */ ::SetTextColor(dc,oldTextColor);
            /* ignore */ ::SetBkColor(dc,oldBgColor);

            ::DeleteObject(brush);
            ::DeleteObject(bitmap);

            fillRect=false;
          }
          break;
        case Display::tabBackgroundFillIndex:
          if (HasTheming()) {
            RECT extent;

            FillRect(dc,&content,::GetSysColorBrush(COLOR_INFOBK));
            MyGetThemeBackgroundExtent(tabTheme,dc,TABP_BODY,1,&content,&extent);
            MyDrawThemeBackground(tabTheme,dc,TABP_BODY,1,&extent,&clip);

            fillRect=false;
          }
          else {
            brush=::GetSysColorBrush(COLOR_BTNFACE);
          }
          break;
        case Display::tabRiderBackgroundFillIndex:
          if (HasTheming()) {
            RECT tc,extent;

            FillRect(dc,&content,::GetSysColorBrush(COLOR_INFOBK));

            // Extent of tab frame
            tc.left=0;
            tc.right=0;
            tc.top=0;
            tc.bottom=0;
            MyGetThemeBackgroundExtent(tabTheme,NULL,TABP_PANE,1,&tc,&extent);

            // Under the theming engine the tab rider contains the gap!
            // So draw the control larger by the amount of the gap
            height+=-extent.top;
            content.bottom+=-extent.top;

            if (draw->selected) {
              style=TTIS_SELECTED;
            }
            else {
              style=TTIS_NORMAL;
            }

            MyGetThemeBackgroundExtent(tabTheme,dc,TABP_TOPTABITEM,style,&content,&extent);
            MyDrawThemeBackground(tabTheme,dc,TABP_TOPTABITEM,style,&extent,&clip);

            fillRect=false;
          }
          else {
            FillRect(dc,&content,::GetSysColorBrush(COLOR_BTNFACE));
            // right
            draw->PushForeground(GetSysColor(COLOR_3DDKSHADOW));

            // TODO:
            draw->DrawLine(xOff+width-1,yOff+3,xOff+width-1,yOff+height-1);
            draw->DrawLine(xOff+width-2,yOff+1,xOff+width-2,yOff+height-1);
            draw->PopForeground();

            draw->PushForeground(GetSysColor(COLOR_3DHIGHLIGHT));

            // top
            draw->DrawLine(xOff+3,yOff  ,xOff+width-4,yOff);
            draw->DrawLine(xOff+1,yOff+1,xOff+width-3,yOff+1);

            // left
            draw->DrawLine(xOff  ,yOff+3,xOff    ,yOff+height-1);
            draw->DrawLine(xOff+1,yOff+1,xOff+1  ,yOff+height-1);

            draw->PopForeground();

            fillRect=false;
          }
          break;
        case Display::columnBackgroundFillIndex:
        case Display::columnLeftBackgroundFillIndex:
        case Display::columnMiddleBackgroundFillIndex:
        case Display::columnRightBackgroundFillIndex:
        case Display::columnEndBackgroundFillIndex:
          if (HasTheming()) {
            if (draw->selected) {
              style=HIS_PRESSED;
            }
            else if (draw->activated) {
              style=HIS_HOT;
            }
            else {
              style=HIS_NORMAL;
            }

            MyDrawThemeBackground(headerTheme,dc,HP_HEADERITEM,style,&content,&clip);

            fillRect=false;
          }
          else {
            /* ignore */ DrawFrameControl(dc,
                                          &content,
                                          DFC_BUTTON,
                                          DFCS_BUTTONPUSH | (draw->selected ? DFCS_PUSHED : 0));
            fillRect=false;
          }
          break;
        case Display::menuStripBackgroundFillIndex:
          if (HasTheming() && menuTheme!=NULL) {
            style=1; // TODO

            MyDrawThemeBackground(menuTheme,dc,MENU_BARBACKGROUND,style,&content,&clip);

            fillRect=false;
          }
          else {
            brush=::GetSysColorBrush(COLOR_BTNFACE);
          }
          break;
        case Display::menuPulldownBackgroundFillIndex:
          if (HasTheming() && menuTheme!=NULL) {
            if (draw->selected) {
              style=3; // TODO
            }
            else if (draw->activated) {
              style=2; // TODO
            }
            else {
              style=1; // TODO
            }

            MyDrawThemeBackground(menuTheme,dc,MENU_BARBACKGROUND,style,&content,&clip);
            MyDrawThemeBackground(menuTheme,dc,MENU_BARITEM,style,&content,&clip);

            fillRect=false;
          }
          else {
            /* ignore */ FillRect(dc,&content,GetSysColorBrush(COLOR_BTNFACE));

            // TODO

            // Outer area is always filled with background color
            draw->PushForeground(GetSysColor(COLOR_BTNFACE));
            draw->DrawRectangle(xOff,yOff,width,height);
            draw->PopForeground();

            if (draw->selected) {
              draw->PushForeground(GetSysColor(COLOR_3DSHADOW));
              draw->DrawLine(xOff+1,yOff+height-2,xOff+1,yOff+2); // Left
              draw->DrawLine(xOff+1,yOff+1,xOff+width-2,yOff+1); // Top
              draw->PopForeground();

              draw->PushForeground(GetSysColor(COLOR_3DHIGHLIGHT));
              draw->DrawLine(xOff+width-2,yOff+2,xOff+width-2,yOff+height-2); // right
              draw->DrawLine(xOff+width-2,yOff+height-2,xOff+1,yOff+height-2); // Bottom
              draw->PopForeground();
            }
            else if (draw->activated) {
              draw->PushForeground(GetSysColor(COLOR_3DHIGHLIGHT));
              draw->DrawLine(xOff+1,yOff+height-2,xOff+1,yOff+2); // Left
              draw->DrawLine(xOff+1,yOff+1,xOff+width-2,yOff+1); // Top
              draw->PopForeground();

              draw->PushForeground(GetSysColor(COLOR_3DSHADOW));
              draw->DrawLine(xOff+width-2,yOff+2,xOff+width-2,yOff+height-2); // right
              draw->DrawLine(xOff+width-2,yOff+height-2,xOff+1,yOff+height-2); // Bottom
              draw->PopForeground();
            }
            else {
              draw->PushForeground(GetSysColor(COLOR_BTNFACE));
              draw->DrawRectangle(xOff+1,yOff+1,width-2,height-2);
              draw->PopForeground();
            }

            fillRect=false;
          }
          break;
        case Display::menuEntryBackgroundFillIndex:
          if (HasTheming() && menuTheme!=NULL) {
            /* ignore */ FillRect(dc,&content,GetSysColorBrush(COLOR_MENU));

            if (draw->selected) {
              style=2; // TODO
            }
            else {
              style=1; // TODO
            }

            MyDrawThemeBackground(menuTheme,dc,MENU_POPUPITEM,style,&content,&clip);

            fillRect=false;
          }
          else {
            if (draw->selected) {
              brush=::GetSysColorBrush(COLOR_HIGHLIGHT);
            }
            else {
              brush=::GetSysColorBrush(COLOR_MENU);
            }
          }
          break;
        case Display::graphBackgroundFillIndex:
          /* ignore */ FillRect(dc,&content,::GetSysColorBrush(COLOR_3DFACE));
          /* ignore */ DrawEdge(dc,&content,EDGE_SUNKEN,BF_RECT);
          fillRect=false;
          break;
        case Display::toolbarBackgroundFillIndex:
          brush=::GetSysColorBrush(COLOR_3DFACE);
          break;
        case Display::menuWindowBackgroundFillIndex:
#if 0
          if (HasTheming()) {
            /* ignore */ FrameRect(dc,&content,GetSysColorBrush(COLOR_3DSHADOW));
          }
          else {
#endif
          /* ignore */ FillRect(dc,&content,::GetSysColorBrush(COLOR_3DFACE));
          /* ignore */ DrawEdge(dc,&content,EDGE_RAISED,BF_RECT);
         //}
          fillRect=false;
          break;
        case Display::popupWindowBackgroundFillIndex:
          /* ignore */ FillRect(dc,&content,::GetSysColorBrush(COLOR_3DFACE));
          /* ignore */ DrawEdge(dc,&content,EDGE_RAISED,BF_RECT);
          fillRect=false;
          break;
        case Display::dialogWindowBackgroundFillIndex:
          /* ignore */ FillRect(dc,&content,::GetSysColorBrush(COLOR_3DFACE));
          fillRect=false;
          break;
        case Display::tooltipWindowBackgroundFillIndex:
          /* ignore */ FillRect(dc,&content,::GetSysColorBrush(COLOR_INFOBK));

          draw->PushForeground(RGB(0,0,0));
          draw->DrawRectangle(content.left,
                              content.top,
                              content.right-content.left,
                              content.bottom-content.top);
          draw->PopForeground();
          fillRect=false;
          break;
        case Display::listboxBackgroundFillIndex:
          /* ignore */ FillRect(dc,&content,::GetSysColorBrush(COLOR_WINDOW));
          fillRect=false;
          break;
        case Display::scrolledBackgroundFillIndex:
          if (HasTheming()) {
            MyDrawThemeBackground(listboxTheme,dc,LBCP_BORDER_NOSCROLL,0,&content,&clip);

            fillRect=false;
          }
          else {
            /* ignore */ FillRect(dc,&content,::GetSysColorBrush(COLOR_3DFACE));
            /* ignore */ DrawEdge(dc,&content,EDGE_SUNKEN,BF_RECT);
            fillRect=false;
          }
          break;
        case Display::hscrollBackgroundFillIndex:
        case Display::vscrollBackgroundFillIndex:
          fillRect=false;
          break;
        case Display::boxedBackgroundFillIndex:
          /* ignore */ FillRect(dc,&content,::GetSysColorBrush(COLOR_3DFACE));
          /* ignore */ DrawEdge(dc,&content,EDGE_SUNKEN,BF_RECT);
          fillRect=false;
          break;
        case Display::plateBackgroundFillIndex:
          /* ignore */ FillRect(dc,&content,::GetSysColorBrush(COLOR_3DFACE));
          draw->PushForeground(RGB(0,0,0));
          draw->DrawRectangle(content.left,
                              content.top,
                              content.right-content.left,
                              content.bottom-content.top);
          draw->PopForeground();
          fillRect=false;
          break;
        case Display::labelBackgroundFillIndex:
            // TODO
            if (draw->focused) {
              /* ignore */ DrawFocusRect(dc,&content);
            }
          fillRect=false;
          break;
        case Display::statusbarBackgroundFillIndex:
          fillRect=false;
          break;
        default:
          std::cerr << "Missing background support for background " << fill << std::endl;
          assert(false);
          break;
        }

        if (fillRect) {
          /* ignore */ FillRect(dc,&content,brush);
        }

#if defined(LUM_HAVE_LIB_CAIRO)
        if (dynamic_cast<Cairo::DrawInfo*>(draw)!=NULL) {
          cairo_surface_mark_dirty(dynamic_cast<Cairo::DrawInfo*>(draw)->surface);
        }
#endif
        draw->CopyFromBitmap(bitmap,x-xOff,y-yOff,w,h,x,y);

        delete bitmap;
      }

      Frame::Frame(Display::FrameIndex frame)
      : frame(frame)
      {
        switch (frame) {
        case Display::tabFrameIndex:
          if (HasTheming()) {
            RECT content,extent;

            content.left=0;
            content.right=0;
            content.top=0;
            content.bottom=0;
            MyGetThemeBackgroundExtent(tabTheme,NULL,TABP_PANE,1,&content,&extent);
            leftBorder=-extent.left;
            rightBorder=extent.right;
            topBorder=-extent.top;
            bottomBorder=extent.bottom;
          }
          else {
            leftBorder=2;
            rightBorder=leftBorder;
            topBorder=2;
            bottomBorder=topBorder;
          }
          break;
        case Display::hscaleFrameIndex:
          if (HasTheming()) {
            leftBorder=0;
            rightBorder=0;
            topBorder=0;
            bottomBorder=0;
          }
          else {
            leftBorder=2;
            rightBorder=leftBorder;
            topBorder=2;
            bottomBorder=topBorder;
          }
          break;
        case Display::vscaleFrameIndex:
          if (HasTheming()) {
            leftBorder=0;
            rightBorder=0;
            topBorder=0;
            bottomBorder=0;
          }
          else {
            leftBorder=2;
            rightBorder=leftBorder;
            topBorder=2;
            bottomBorder=topBorder;
          }
          break;
        case Display::focusFrameIndex:
          leftBorder=1;
          rightBorder=leftBorder;
          topBorder=1;
          bottomBorder=topBorder;
          break;
        case Display::statuscellFrameIndex:
          leftBorder=1;
          rightBorder=leftBorder;
          topBorder=1;
          bottomBorder=topBorder;
          break;
        case Display::valuebarFrameIndex:
          leftBorder=1;
          rightBorder=1;
          topBorder=1;
          bottomBorder=1;
          break;
        case Display::groupFrameIndex:
          leftBorder=2;
          rightBorder=2;
          topBorder=2;
          bottomBorder=2;
          break;
        }

        minWidth=leftBorder+rightBorder;
        minHeight=topBorder+bottomBorder;
      }

      void Frame::Draw(Lum::OS::DrawInfo* draw, int x, int y, size_t w, size_t h)
      {
        HDC  dc;
        RECT rect;

        if (dynamic_cast<DrawInfo*>(draw)!=NULL) {
          dc=dynamic_cast<DrawInfo*>(draw)->dc;
        }
#if defined(LUM_HAVE_LIB_CAIRO)
        else if (dynamic_cast<Cairo::DrawInfo*>(draw)!=NULL) {
          cairo_surface_flush(dynamic_cast<Cairo::DrawInfo*>(draw)->surface);
          dc=cairo_win32_surface_get_dc(dynamic_cast<Cairo::DrawInfo*>(draw)->surface);
        }
#endif
        else {
          assert(false);
        }

        if (minWidth==0 && minHeight==0) {
          return;
        }

        rect.left=x;
        rect.top=y;
        rect.right=x+w;
        rect.bottom=y+h;

        switch (frame) {
        case Display::tabFrameIndex:
          if (HasTheming()) {
            HRGN region=CreateRectRgn(x,y,x+w,y+h);
            HRGN tmpRegion=CreateRectRgn(x+leftBorder,y+topBorder,x+w-minWidth,y+h-minHeight);

            // Push clip for inner region
            /* ignore */ CombineRgn(region,region,tmpRegion,RGN_DIFF);
            /* ignore */ DeleteObject(tmpRegion);
            /* ignore */ SelectClipRgn(dc,region);

            /* ignore */ MyDrawThemeBackground(tabTheme,dc,TABP_PANE,1,&rect,NULL);

            // Pop clip for inner region
            /* ignore */ SelectClipRgn(dc,NULL);
            /* ignore */ DeleteObject(region);

            RECT clip;

            rect.left=x+gx+1;
            rect.right=rect.left+gw-2;
            rect.bottom=y+topBorder;
            rect.top=rect.bottom-10;

            clip.left=x+gx+1;
            clip.right=clip.left+gw-2;
            clip.top=y;
            clip.bottom=clip.top+topBorder;

            /* ignore */ MyDrawThemeBackground(tabTheme,dc,TABP_BODY,1,&rect,&clip);
          }
          else {
            draw->PushForeground(GetSysColor(COLOR_3DDKSHADOW));

            /* right line */
            draw->DrawLine(x+w-2,y,x+w-2,y+h-1-2);
            draw->DrawLine(x+w-1,y,x+w-1,y+h-1-2);

            /* bottom line */
            draw->DrawLine(x+1,y+h-2,x+w-1,y+h-2);
            draw->DrawLine(x,  y+h-1,x+w-1,y+h-1);

            draw->PopForeground();


            draw->PushForeground(GetSysColor(COLOR_3DHIGHLIGHT));

            /* top line */

            /* Gap */
            draw->PushForeground(::GetSysColor(COLOR_BTNFACE));
            draw->DrawLine(x+gx+1,y,  x+gx+gw-3,y);
            draw->DrawLine(x+gx  ,y+1,x+gx+gw-2,y+1);
            draw->PopForeground();

            // left of gap
            if (gx>0) {
              draw->DrawLine(x,y,  x+gx,  y);
              draw->DrawLine(x,y+1,x+gx-1,y+1);
            }

            // right of gap
            if (gx+gw<w) {
              draw->DrawLine(x+gx+gw-2,y,  x+w-3,y);
              draw->DrawLine(x+gx+gw-1,y+1,x+w-3,y+1);
            }

            /* left line */
            draw->DrawLine(x  ,y,x  ,y+h-1);
            draw->DrawLine(x+1,y,x+1,y+h-1-1);

            draw->PopForeground();
          }
          break;
        case Display::hscaleFrameIndex:
          if (HasTheming()) {
          }
          else {
            /* ignore */ DrawEdge(dc,&rect,EDGE_SUNKEN,BF_RECT);
          }
          break;
        case Display::vscaleFrameIndex:
          if (HasTheming()) {
          }
          else {
            /* ignore */ DrawEdge(dc,&rect,EDGE_SUNKEN,BF_RECT);
          }
          break;
        case Display::focusFrameIndex:
          /* ignore */ DrawFocusRect(dc,&rect);
          break;
        case Display::statuscellFrameIndex:
          draw->PushForeground(GetSysColor(COLOR_3DSHADOW));
          draw->DrawLine(x,y+h-1,x,y+1);
          draw->DrawLine(x,y,x+w-1,y);
          draw->PopForeground();

          draw->PushForeground(GetSysColor(COLOR_3DHIGHLIGHT));
          draw->DrawLine(x+w-1,y+1,x+w-1,y+h-1);
          draw->DrawLine(x+w-1,y+h-1,x,y+h-1);
          draw->PopForeground();
          break;
        case Display::valuebarFrameIndex:
          draw->PushForeground(GetSysColor(COLOR_3DHIGHLIGHT));
          draw->DrawLine(x,y+h-1,x,y+1);
          draw->DrawLine(x,y,x+w-1,y);
          draw->PopForeground();

          draw->PushForeground(GetSysColor(COLOR_3DSHADOW));
          draw->DrawLine(x+w-1,y+1,x+w-1,y+h-1);
          draw->DrawLine(x+w-1,y+h-1,x,y+h-1);
          draw->PopForeground();
          break;
        case Display::groupFrameIndex:
          {
            int top;

            if (HasGap()) {
              top=y+gh / 2;
            }
            else {
              top=y;
            }
            draw->PushForeground(GetSysColor(COLOR_3DSHADOW));
            draw->DrawLine(x,y+h-1,x,top);
            draw->DrawLine(x+w-2,top+1,x+w-2,y+h-2);
            draw->DrawLine(x+1,y+h-2,x+w-2,y+h-2);
            if (HasGap()) {
              draw->DrawLine(x,top,x+OS::display->GetSpaceHorizontal(Display::spaceLabelObject)-1,top);
              draw->DrawLine(x+3*OS::display->GetSpaceHorizontal(Display::spaceLabelObject)+gw+1,top,x+w-1,top);
              draw->DrawLine(x+3*OS::display->GetSpaceHorizontal(Display::spaceLabelObject)+gw,top,
                             x+3*OS::display->GetSpaceHorizontal(Display::spaceLabelObject)+gw,top+1);
            }
            else {
              draw->DrawLine(x,top,x+w-1,top);
            }
            draw->PopForeground();

            draw->PushForeground(GetSysColor(COLOR_3DHIGHLIGHT));
            draw->DrawLine(x+1,y+h-2,x+1,top+1);
            draw->DrawLine(x+w-1,top+1,x+w-1,y+h-1);
            draw->DrawLine(x+1,y+h-1,x+w-2,y+h-1);
            if (HasGap()) {
              draw->DrawLine(x+2,top+1,x+OS::display->GetSpaceHorizontal(Display::spaceLabelObject)-1,top+1);
              draw->DrawLine(x+3*OS::display->GetSpaceHorizontal(Display::spaceLabelObject)+gw+1,top+1,x+w-2,top+1);
              draw->DrawLine(x+OS::display->GetSpaceHorizontal(Display::spaceLabelObject),top,x+OS::display->GetSpaceHorizontal(Display::spaceLabelObject),top+1);
            }
            else {
              draw->DrawLine(x+2,top+1,x+w-2,top+1);
            }
            draw->PopForeground();
          }
          break;
        default:
          assert(false);
          break;
        }

#if defined(LUM_HAVE_LIB_CAIRO)
        if (dynamic_cast<Cairo::DrawInfo*>(draw)!=NULL) {
          cairo_surface_mark_dirty(dynamic_cast<Cairo::DrawInfo*>(draw)->surface);
        }
#endif
      }

      Win32Theme::Win32Theme(Lum::OS::Display *display)
      : OS::Base::Theme(display)
      {
        color[Display::backgroundColor]=GetSysColor(COLOR_BTNFACE);
        color[Display::tableTextColor]=GetSysColor(COLOR_WINDOWTEXT);
        color[Display::textColor]=GetSysColor(COLOR_BTNTEXT);
        color[Display::textSelectColor]=GetSysColor(COLOR_BTNTEXT);
        color[Display::textDisabledColor]=GetSysColor(COLOR_GRAYTEXT);
        color[Display::fillColor]=GetSysColor(COLOR_HIGHLIGHT);
        if (HasTheming()) {
          color[Display::fillTextColor]=GetSysColor(COLOR_WINDOWTEXT);
        }
        else {
          color[Display::fillTextColor]=GetSysColor(COLOR_HIGHLIGHTTEXT);
        }
        color[Display::graphScaleColor]=GetSysColor(COLOR_GRAYTEXT);
        color[Display::blackColor]=RGB(0,0,0);
        color[Display::whiteColor]=RGB(0xff,0xff,0xff);
        color[Display::tabTextColor]=GetSysColor(COLOR_BTNTEXT);
        color[Display::tabTextSelectColor]=GetSysColor(COLOR_BTNTEXT);
        color[Display::editTextColor]=GetSysColor(COLOR_WINDOWTEXT);
        if (HasTheming()) {
          color[Display::editTextFillColor]=GetSysColor(COLOR_WINDOWTEXT);
        }
        else {
          color[Display::editTextFillColor]=GetSysColor(COLOR_HIGHLIGHTTEXT);
        }

        fill[Display::backgroundFillIndex]=new Fill(Display::backgroundFillIndex);
        fill[Display::tableBackgroundFillIndex]=new Fill(Display::tableBackgroundFillIndex);
        fill[Display::tableBackground2FillIndex]=new Fill(Display::tableBackground2FillIndex);
        fill[Display::buttonBackgroundFillIndex]=new Fill(Display::buttonBackgroundFillIndex);
        fill[Display::positiveButtonBackgroundFillIndex]=new Fill(Display::positiveButtonBackgroundFillIndex);
        fill[Display::negativeButtonBackgroundFillIndex]=new Fill(Display::negativeButtonBackgroundFillIndex);
        fill[Display::defaultButtonBackgroundFillIndex]=new Fill(Display::defaultButtonBackgroundFillIndex);
        fill[Display::toolbarButtonBackgroundFillIndex]=new Fill(Display::toolbarButtonBackgroundFillIndex);
        fill[Display::scrollButtonBackgroundFillIndex]=new Fill(Display::scrollButtonBackgroundFillIndex);
        fill[Display::comboBackgroundFillIndex]=new Fill(Display::comboBackgroundFillIndex);
        fill[Display::editComboBackgroundFillIndex]=new Fill(Display::editComboBackgroundFillIndex);
        fill[Display::entryBackgroundFillIndex]=new Fill(Display::entryBackgroundFillIndex);
        fill[Display::hprogressBackgroundFillIndex]=new Fill(Display::hprogressBackgroundFillIndex);
        fill[Display::hprogressFillIndex]=new Fill(Display::hprogressFillIndex);
        fill[Display::vprogressBackgroundFillIndex]=new Fill(Display::vprogressBackgroundFillIndex);
        fill[Display::vprogressFillIndex]=new Fill(Display::vprogressFillIndex);
        fill[Display::hknobBoxFillIndex]=new Fill(Display::hknobBoxFillIndex);
        fill[Display::vknobBoxFillIndex]=new Fill(Display::vknobBoxFillIndex);
        fill[Display::tabBackgroundFillIndex]=new Fill(Display::tabBackgroundFillIndex);
        fill[Display::tabRiderBackgroundFillIndex]=new Fill(Display::tabRiderBackgroundFillIndex);
        fill[Display::columnBackgroundFillIndex]=new Fill(Display::columnBackgroundFillIndex);
        fill[Display::columnLeftBackgroundFillIndex]=new Fill(Display::columnLeftBackgroundFillIndex);
        fill[Display::columnMiddleBackgroundFillIndex]=new Fill(Display::columnMiddleBackgroundFillIndex);
        fill[Display::columnRightBackgroundFillIndex]=new Fill(Display::columnRightBackgroundFillIndex);
        fill[Display::columnEndBackgroundFillIndex]=new Fill(Display::columnEndBackgroundFillIndex);
        fill[Display::menuStripBackgroundFillIndex]=new Fill(Display::menuStripBackgroundFillIndex);
        fill[Display::menuPulldownBackgroundFillIndex]=new Fill(Display::menuPulldownBackgroundFillIndex);
        fill[Display::menuEntryBackgroundFillIndex]=new Fill(Display::menuEntryBackgroundFillIndex);
        fill[Display::menuWindowBackgroundFillIndex]=new Fill(Display::menuWindowBackgroundFillIndex);
        fill[Display::popupWindowBackgroundFillIndex]=new Fill(Display::popupWindowBackgroundFillIndex);
        fill[Display::dialogWindowBackgroundFillIndex]=new Fill(Display::dialogWindowBackgroundFillIndex);
        fill[Display::tooltipWindowBackgroundFillIndex]=new Fill(Display::tooltipWindowBackgroundFillIndex);
        fill[Display::graphBackgroundFillIndex]=new Fill(Display::graphBackgroundFillIndex);
        fill[Display::toolbarBackgroundFillIndex]=new Fill(Display::toolbarBackgroundFillIndex);
        fill[Display::labelBackgroundFillIndex]=new Fill(Display::labelBackgroundFillIndex);
        fill[Display::listboxBackgroundFillIndex]=new Fill(Display::listboxBackgroundFillIndex);
        fill[Display::scrolledBackgroundFillIndex]=new Fill(Display::scrolledBackgroundFillIndex);
        fill[Display::hscrollBackgroundFillIndex]=new Fill(Display::hscrollBackgroundFillIndex);
        fill[Display::vscrollBackgroundFillIndex]=new Fill(Display::vscrollBackgroundFillIndex);
        fill[Display::boxedBackgroundFillIndex]=new Fill(Display::boxedBackgroundFillIndex);
        fill[Display::plateBackgroundFillIndex]=new Fill(Display::plateBackgroundFillIndex);
        fill[Display::statusbarBackgroundFillIndex]=NULL;

        frame[Display::tabFrameIndex]=new Frame(Display::tabFrameIndex);
        frame[Display::hscaleFrameIndex]=new Frame(Display::hscaleFrameIndex);
        frame[Display::vscaleFrameIndex]=new Frame(Display::vscaleFrameIndex);
        frame[Display::focusFrameIndex]=new Frame(Display::focusFrameIndex);
        frame[Display::statuscellFrameIndex]=new Frame(Display::statuscellFrameIndex);
        frame[Display::valuebarFrameIndex]=new Frame(Display::valuebarFrameIndex);
        frame[Display::groupFrameIndex]=new Frame(Display::groupFrameIndex);

        image[Display::hknobImageIndex]=new ThemeImage(Display::hknobImageIndex);
        image[Display::vknobImageIndex]=new ThemeImage(Display::vknobImageIndex);
        image[Display::arrowLeftImageIndex]=new ThemeImage(Display::arrowLeftImageIndex);
        image[Display::arrowRightImageIndex]=new ThemeImage(Display::arrowRightImageIndex);
        image[Display::arrowUpImageIndex]=new ThemeImage(Display::arrowUpImageIndex);
        image[Display::arrowDownImageIndex]=new ThemeImage(Display::arrowDownImageIndex);
        image[Display::scrollLeftImageIndex]=image[Display::arrowLeftImageIndex];
        image[Display::scrollRightImageIndex]=image[Display::arrowRightImageIndex];
        image[Display::scrollUpImageIndex]=image[Display::arrowUpImageIndex];
        image[Display::scrollDownImageIndex]=image[Display::arrowDownImageIndex];
        image[Display::comboImageIndex]=new ThemeImage(Display::comboImageIndex);
        image[Display::comboEditButtonImageIndex]=new ThemeImage(Display::comboEditButtonImageIndex);
        image[Display::comboDividerImageIndex]=new ThemeImage(Display::comboDividerImageIndex);
        image[Display::treeExpanderImageIndex]=new ThemeImage(Display::treeExpanderImageIndex);
        image[Display::hscaleKnobImageIndex]=new ThemeImage(Display::hscaleKnobImageIndex);
        image[Display::vscaleKnobImageIndex]=new ThemeImage(Display::vscaleKnobImageIndex);
        image[Display::leftSliderImageIndex]=new ThemeImage(Display::leftSliderImageIndex);
        image[Display::rightSliderImageIndex]=new ThemeImage(Display::rightSliderImageIndex);
        image[Display::topSliderImageIndex]=new ThemeImage(Display::topSliderImageIndex);
        image[Display::bottomSliderImageIndex]=new ThemeImage(Display::bottomSliderImageIndex);
        image[Display::menuDividerImageIndex]=new ThemeImage(Display::menuDividerImageIndex);
        image[Display::menuSubImageIndex]=new ThemeImage(Display::menuSubImageIndex);
        image[Display::menuCheckImageIndex]=new ThemeImage(Display::menuCheckImageIndex);
        image[Display::menuRadioImageIndex]=new ThemeImage(Display::menuRadioImageIndex);
        //image[Display::closeImageIndex]=new ThemeImage(Display::closeImageIndex);
        image[Display::radioImageIndex]=new ThemeImage(Display::radioImageIndex);
        image[Display::checkImageIndex]=new ThemeImage(Display::checkImageIndex);
        image[Display::ledImageIndex]=new ThemeImage(Display::ledImageIndex);
      }

      Win32Theme::~Win32Theme()
      {
        // no code
      }

      bool Win32Theme::HasFontSettings() const
      {
        return false;
      }

      std::wstring Win32Theme::GetProportionalFontName() const
      {
        NONCLIENTMETRICSW metrics;

        metrics.cbSize=sizeof(NONCLIENTMETRICSW);

        ::SystemParametersInfoW(SPI_GETNONCLIENTMETRICS,sizeof(NONCLIENTMETRICSW),&metrics,0);

        return metrics.lfMessageFont.lfFaceName;
      }

      double Win32Theme::GetProportionalFontSize() const
      {
        NONCLIENTMETRICSW metrics;

        metrics.cbSize=sizeof(NONCLIENTMETRICSW);

        ::SystemParametersInfoW(SPI_GETNONCLIENTMETRICS,sizeof(NONCLIENTMETRICSW),&metrics,0);

        return std::abs(metrics.lfMessageFont.lfHeight);
      }

      std::wstring Win32Theme::GetFixedFontName() const
      {
        ::SelectObject(((Display*)display)->hdc,GetStockObject(ANSI_FIXED_FONT));

        wchar_t buffer[1024];

        ::GetTextFaceW(((Display*)display)->hdc,1024,buffer);

        return buffer;
      }

      double Win32Theme::GetFixedFontSize() const
      {
        TEXTMETRIC metric;

        SelectObject(((Display*)display)->hdc,GetStockObject(ANSI_FIXED_FONT));
        GetTextMetrics(((Display*)display)->hdc,&metric);

        return metric.tmHeight;
      }

      size_t Win32Theme::GetSpaceHorizontal(Display::Space space) const
      {
        switch (space) {
        case Display::spaceWindowBorder:
          return 12;
        case Display::spaceInterGroup:
          return 18;
        case Display::spaceGroupIndent:
          return 12;
        case Display::spaceInterObject:
          return 6;
        case Display::spaceObjectBorder:
          return 6;
        case Display::spaceIntraObject:
          return 6;
        case Display::spaceLabelObject:
          return 12;
        case Display::spaceObjectDetail:
          return 16;
        }

        return 1;
      }

      size_t Win32Theme::GetSpaceVertical(Display::Space space) const
      {
        switch (space) {
        case Display::spaceWindowBorder:
          return 12;
        case Display::spaceInterGroup:
          return 18;
        case Display::spaceGroupIndent:
          return 12;
        case Display::spaceInterObject:
          return 6;
        case Display::spaceObjectBorder:
          return 3;
        case Display::spaceIntraObject:
          return 6;
        case Display::spaceLabelObject:
          return 6;
        case Display::spaceObjectDetail:
          return 16;
        }

        return 1;
      }

      size_t Win32Theme::GetIconWidth() const
      {
        return 16;
      }

      size_t Win32Theme::GetIconHeight() const
      {
        return 16;
      }

      size_t Win32Theme::GetDragStartSensitivity() const
      {
        return 6;
      }

      size_t Win32Theme::GetMouseClickHoldSensitivity() const
      {
        return 5;
      }

      size_t Win32Theme::GetMinimalButtonWidth() const
      {
        return Lum::Base::GetSize(Lum::Base::Size::stdCharWidth,8);
      }

      size_t Win32Theme::GetFirstTabOffset() const
      {
        return 0;
      }

      size_t Win32Theme::GetLastTabOffset() const
      {
        return OS::display->GetSpaceHorizontal(Display::spaceInterObject);
      }

      OS::Theme::ScrollKnobMode Win32Theme::GetScrollKnobMode() const
      {
        return OS::Theme::scrollKnobModeSingle;
      }

      OS::Theme::ToolbarPosition Win32Theme::GetToolbarPosition() const
      {
        return OS::Theme::toolbarTop;
      }

      bool Win32Theme::ShowButtonImages() const
      {
        return false;
      }

      OS::Theme::MenuType Win32Theme::GetMenuType() const
      {
        return menuTypeStrip;
      }
    }
  }
}
