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

#include <Lum/OS/Display.h>

#include <Lum/Base/Util.h>

namespace Lum {
  namespace OS {

/*    const int fontNormal=0;
      const int fontCondensed=1;
      const int fontNarrow=2;
    const int fontDouble=3;

      const int fontFixed=0;
    const int fontProportional=1;

      const int canDrawFocused=0;
    const int canDrawDisabled=1;

      const int copy=0;
    const int invert=1;

      const int none=0;
    const int text=1;

      const int disableWidth=8;
      const int disableHeight=2;
      const int bigChessWidth=16;
    const int bigChessHeight=4;

      const int littleEndian=1;
    const int bigEndian=2;

      signed char systemByteOrder;
      char disablePattern[2];
      char bigChessPattern[8];
      char sPointLine[2];
      char mPointLine[2];
      char lPointLine[2];
      ColorName colorNames[colorCount];

      Bitmap smallChess;
      Bitmap bigChess;
      std::wstring home;
      */

    struct ColorEntry
    {
      Display::ColorIndex index;
      const char*         name;
    };

    static ColorEntry colorEntries[Display::colorCount+1] =
    {
      {Display::backgroundColor              ,"background"},
      {Display::tableTextColor               ,"tableText"},
      {Display::textColor                    ,"text"},
      {Display::textSelectColor              ,"textSelect"},
      {Display::textDisabledColor            ,"textDisabled"},
      {Display::fillColor                    ,"fill"},
      {Display::fillTextColor                ,"fillText"},
      {Display::graphScaleColor              ,"graphScale"},
      {Display::blackColor                   ,"black"},
      {Display::whiteColor                   ,"white"},
      {Display::tabTextColor                 ,"tabText"},
      {Display::tabTextSelectColor           ,"tabTextSelect"},
      {Display::editTextColor                ,"editText"},
      {Display::editTextFillColor            ,"editTextFill"},
      {Display::textSmartAColor              ,"textSmartA"},
      {Display::textSmartBColor              ,"textSmartB"},
      {Display::blackColor                   ,NULL}
    };

    const char* colorNames[Display::colorCount] =
    {
      "background",
      "tableText",
      "text",
      "textSelect",
      "textDisabled",
      "fill",
      "fillText",
      "graphScale",
      "black",
      "white",
      "tabText",
      "tabTextSelect",
      "editText",
      "editTextFill",
      "textSmartA",
      "textSmartB"
    };

    const char* fillNames[Display::fillCount] =
    {
      "background",
      "tableBackground",
      "tableBackground2",
      "buttonBackground",
      "positiveButtonBackground",
      "negativeButtonBackground",
      "defaultButtonBackground",
      "toolbarButtonBackground",
      "scrollButtonBackground",
      "comboBackground",
      "editComboBackground",
      "entryBackground",
      "hprogressBackground",
      "hprogress",
      "vprogressBackground",
      "vprogress",
      "hknobBox",
      "vknobBox",
      "tab",
      "tabRider",
      "columnBackground",
      "columnLeftBackground",
      "columnMiddleBackground",
      "columnRightBackground",
      "columnEndBackground",
      "menuStripBackground",
      "menuPulldownBackground",
      "menuEntryBackground",
      "menuWindowBackground",
      "popupWindowBackground",
      "dialogWindowBackground",
      "tooltipWindowBackground",
      "graphBackground",
      "toolbarBackground",
      "labelBackground",
      "listboxBackground",
      "scrolledBackground",
      "hscrollBackground",
      "vscrollBackground",
      "boxedBackground",
      "plateBackground",
      "statusbarBackground"
    };

    const char* frameNames[Display::frameCount] =
    {
      "tab",
      "hscale",
      "vscale",
      "focus",
      "statuscell",
      "valuebar",
      "group"
    };

    extern "C" {
      Display::ColorIndex GetColorIndexByName(const std::string& name)
      {
        int x=0;
        while (colorEntries[x].name!=NULL) {
          if (name==colorEntries[x].name) {
            return colorEntries[x].index;
          }
          x++;
        }

        return colorEntries[x].index;
      }
    }

#if 0
              bool GetRGB32ByColorName(char name[], int r, int g, int b)
              {
                char color[16];
                int intense;
                int length;
                int x;

                length=0;
                while ((length<LEN(name)) && (name[length]!=0x0)) {
                  name[length]=CAP(name[length]);
                  length++;
                }
                x=0;
                while ((x<LEN(name)) && (name[x]>='A') && (name[x]<='Z')) {
                  color[x]=name[x];
                  x++;
                }
                color[x]=0x0;
                intense=100;
                if (name[0]=='#') {
                  if (length==7) {
                    for (/*TODO*/x=1TO6) {
                      if (~(((name[x]>='0') && (name[x]<='9')) || ((name[x]>='A') && (name[x]<='F')))) {
                        return false;
                      }
                    }
                    r=GetHex(name[1])*16+GetHex(name[2]);
                    g=GetHex(name[3])*16+GetHex(name[4]);
                    b=GetHex(name[5])*16+GetHex(name[6]);
                  }
                  else {
                    return false;
                  }
                }
                else {
                  if ((x<LEN(name)) && (name[x]>='0') && (name[x]<='9')) {
                    intense=0;
                    while ((x<LEN(name)) && (name[x]>='0') && (name[x]<='9')) {
                      intense=intense*10+ORD(name[x])-ORD('0');
                      x++;
                    }
                  }
                  if (color=="RED") {
                    r=0FFH;
                    g=0;
                    b=0;
                  }
                  else if (color=="GREEN") {
                    r=0;
                    g=0FFH;
                    b=0;
                  }
                  else if (color=="BLUE") {
                    r=0;
                    g=0;
                    b=0FFH;
                  }
                  else if (color=="YELLOW") {
                    r=0FFH;
                    g=0FFH;
                    b=0;
                  }
                  else if (color=="MAGENTA") {
                    r=0FFH;
                    g=0;
                    b=0FFH;
                  }
                  else if (color=="CYAN") {
                    r=0;
                    g=0FFH;
                    b=0FFH;
                  }
                  else if (color=="BLACK") {
                    r=0;
                    g=0;
                    b=0;
                  }
                  else if (color=="WHITE") {
                    r=0FFH;
                    g=0FFH;
                    b=0FFH;
                  }
                  else if ((color=="GREY") || (color=="GRAY")) {
                    r=0FFH;
                    g=0FFH;
                    b=0FFH;
                  }
                  else {
                    return false;
                  }
                }Scale *)
r=(r*intense)/100;
                g=(g*intense)/100;
                b=(b*intense)/100;Expand to 32 bit *)
r=r*256*256*256;
                g=g*256*256*256;
                b=b*256*256*256;
                return true;
              }

              bool DoByteCheck(BYTE x[])
              {

                return SYSTEM.VAL(CHAR,x[0])==0x1;
              }

              void CheckByteOrder()
              {
                int i;

                i=1;
                if (DoByteCheck(i)) {
                  systemByteOrder=littleEndian;
                }
                else {
                  systemByteOrder=bigEndian;
                }
              }

#endif

    Display* display;
  }
}
