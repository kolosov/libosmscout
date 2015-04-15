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

#include <Lum/OS/Curses/Theme.h>

namespace Lum {
  namespace OS {
    namespace Curses {

      TextImage::TextImage(Display::ImageIndex image)
        : image(image)
      {
        switch (image) {
        case Display::hknobImageIndex:
        case Display::vknobImageIndex:
        case Display::arrowLeftImageIndex:
        case Display::arrowRightImageIndex:
        case Display::arrowUpImageIndex:
        case Display::arrowDownImageIndex:
        case Display::scrollLeftImageIndex:
        case Display::scrollRightImageIndex:
        case Display::scrollUpImageIndex:
        case Display::scrollDownImageIndex:
        case Display::comboImageIndex:
        case Display::treeExpanderImageIndex:
        case Display::hscaleKnobImageIndex:
        case Display::vscaleKnobImageIndex:
        case Display::menuSubImageIndex:
        case Display::menuCheckImageIndex:
        case Display::menuRadioImageIndex:
        case Display::hpaneHandleImageIndex:
        case Display::vpaneHandleImageIndex:
        case Display::radioImageIndex:
        case Display::checkImageIndex:
        case Display::ledImageIndex:
          width=1;
          height=1;
          break;
        case Display::comboEditButtonImageIndex:
          width=3;
          height=1;
          break;
        case Display::comboDividerImageIndex:
        case Display::leftSliderImageIndex:
        case Display::rightSliderImageIndex:
        case Display::topSliderImageIndex:
        case Display::bottomSliderImageIndex:
        case Display::menuDividerImageIndex:
          width=0;
          height=0;
          break;
        }
      }

      size_t TextImage::GetWidth() const
      {
        return width;
      }

      size_t TextImage::GetHeight() const
      {
        return height;
      }

      bool TextImage::GetAlpha() const
      {
        return true;
      }

      unsigned long TextImage::GetDrawCap() const
      {
        return 0;//drawsDisabled|drawsFocused|drawsActivated;
      }

      void TextImage::Draw(OS::DrawInfo *draw, int x,int y)
      {
        Draw(draw,x,y,GetWidth(),GetHeight());
      }

      void TextImage::DrawStretched(OS::DrawInfo *draw,
                                         int x,int y,
                                         size_t w, size_t h)
      {
        Draw(draw,x,y,w,h);
      }

      void TextImage::DrawScaled(OS::DrawInfo *draw,
                                      int x,int y,
                                      size_t w, size_t h)
      {
        Draw(draw,x,y,w,h);
      }


      void TextImage::Draw(OS::DrawInfo *draw, int x, int y, size_t w, size_t h)
      {
        draw->PushFont(OS::display->GetFont());
        draw->PushForeground(Display::textColor);

        for (int i=x; i<(int)(x+w); ++i) {
          for (int j=y; j<(int)(y+h); ++j) {

            switch (image) {
            case Display::menuCheckImageIndex:
            case Display::checkImageIndex:
              draw->PushForeground(Display::textColor);
              if (draw->selected) {
                draw->DrawString(i,j,L"X");
              }
              else {
                draw->DrawString(i,j,L"o");
              }
              draw->PopForeground();
              break;
            case Display::menuRadioImageIndex:
            case Display::radioImageIndex:
              draw->PushForeground(Display::textColor);
              if (draw->selected) {
                draw->DrawString(i,j,L"*");
              }
              else {
                draw->DrawString(i,j,L"o");
              }
              draw->PopForeground();
              break;
            case Display::ledImageIndex:
              if (draw->selected) {
                draw->DrawString(i,j,L"*");
              }
              else {
                draw->DrawString(i,j,L"o");
              }
              break;
            case Display::hknobImageIndex:
            case Display::vknobImageIndex:
            case Display::hpaneHandleImageIndex:
            case Display::vpaneHandleImageIndex:
              draw->PushForeground(dynamic_cast<Theme*>(OS::display->GetTheme())->buttonBackgroundColor);
              draw->FillRectangle(i,j,1,1);
              draw->PopForeground();
              draw->PushForeground(Display::textColor);
              draw->DrawString(i,j,L"#");
              draw->PopForeground();
              break;
            case Display::arrowLeftImageIndex:
            case Display::scrollLeftImageIndex:
              draw->PushForeground(Display::textColor);
              draw->DrawString(i,j,L"<");
              draw->PopForeground();
              break;
            case Display::arrowRightImageIndex:
            case Display::scrollRightImageIndex:
            case Display::menuSubImageIndex:
              draw->PushForeground(Display::textColor);
              draw->DrawString(i,j,L">");
              draw->PopForeground();
              break;
            case Display::arrowUpImageIndex:
            case Display::scrollUpImageIndex:
              draw->PushForeground(Display::textColor);
              draw->DrawString(i,j,L"^");
              draw->PopForeground();
              break;
            case Display::arrowDownImageIndex:
            case Display::scrollDownImageIndex:
            case Display::comboImageIndex:
              draw->PushForeground(Display::textColor);
              draw->DrawString(i,j,L"v");
              draw->PopForeground();
              break;
            case Display::comboEditButtonImageIndex:
              if (i==x) {
                draw->DrawFillString(i,j,L"[",OS::display->GetColor(Display::backgroundColor));
              }
              else if (i==(int)(x+w-1)) {
                draw->DrawFillString(i,j,L"]",OS::display->GetColor(Display::backgroundColor));
              }
              else {
                if (draw->selected) {
                  draw->PushForeground(Display::fillTextColor);
                }
                draw->DrawFillString(i,j,L"v",OS::display->GetColor(Display::backgroundColor));
                if (draw->selected) {
                  draw->PopForeground();
                }
              }
              break;
            case Display::treeExpanderImageIndex:
              if (draw->selected) {
                draw->DrawString(i,j,L"-");
              }
              else {
                draw->DrawString(i,j,L"+");
              }
              break;
            case Display::hscaleKnobImageIndex:
            case Display::vscaleKnobImageIndex:
              if (draw->selected) {
                draw->PushForeground(Display::fillTextColor);
              }
              draw->DrawString(i,j,L"*");
              if (draw->selected) {
                draw->PopForeground();
              }
              break;
            case Display::leftSliderImageIndex:
            case Display::bottomSliderImageIndex:
              draw->DrawString(i,j,L"#");
              break;
            case Display::rightSliderImageIndex:
            case Display::topSliderImageIndex:
              draw->DrawString(i,j,L" ");
              break;
            case Display::comboDividerImageIndex:
            case Display::menuDividerImageIndex:
              break;
            }
          }
        }

        draw->PopForeground();
        draw->PopFont();
      }

      TextStockImage::TextStockImage(Theme::StockImage image)
        : image(image)
      {
        switch (image) {
        case Theme::imageClose:
        case Theme::imageAdd:
        case Theme::imageRemove:
        case Theme::imageClear:
        case Theme::imageEdit:
          width=1;
          height=1;
          break;
        case Theme::imagePositive:
        case Theme::imageNegative:
        case Theme::imageDefault:
        case Theme::imageHelp:
        case Theme::imageSave:
        case Theme::imageMediaRecord:
        case Theme::imageMediaPlay:
        case Theme::imageMediaPause:
        case Theme::imageMediaStop:
          width=0;
          height=0;
          break;
        }
      }

      size_t TextStockImage::GetWidth() const
      {
        return width;
      }

      size_t TextStockImage::GetHeight() const
      {
        return height;
      }

      bool TextStockImage::GetAlpha() const
      {
        return true;
      }

      unsigned long TextStockImage::GetDrawCap() const
      {
        return 0;//drawsDisabled|drawsFocused|drawsActivated;
      }

      void TextStockImage::Draw(OS::DrawInfo *draw, int x,int y)
      {
        Draw(draw,x,y,GetWidth(),GetHeight());
      }

      void TextStockImage::DrawStretched(OS::DrawInfo *draw,
                                         int x,int y,
                                         size_t w, size_t h)
      {
        Draw(draw,x,y,w,h);
      }

      void TextStockImage::DrawScaled(OS::DrawInfo *draw,
                                      int x,int y,
                                      size_t w, size_t h)
      {
        Draw(draw,x,y,w,h);
      }

      void TextStockImage::Draw(OS::DrawInfo *draw, int x, int y, size_t w, size_t h)
      {
        draw->PushFont(OS::display->GetFont());
        draw->PushForeground(Display::textColor);

        for (int i=x; i<(int)(x+w); ++i) {
          for (int j=y; j<(int)(y+h); ++j) {

            switch (image) {
            case Theme::imageClose:
              draw->DrawString(i,j,L"X");
              break;
            case Theme::imageAdd:
              draw->DrawString(i,j,L"+");
              break;
            case Theme::imageRemove:
            case Theme::imageClear:
              draw->DrawString(i,j,L"-");
              break;
            case Theme::imageEdit:
              draw->DrawString(i,j,L"v");
              break;
            case Theme::imagePositive:
            case Theme::imageNegative:
            case Theme::imageDefault:
            case Theme::imageHelp:
            case Theme::imageSave:
            case Theme::imageMediaRecord:
            case Theme::imageMediaPlay:
            case Theme::imageMediaPause:
            case Theme::imageMediaStop:
              break;
            }
          }
        }

        draw->PopForeground();
        draw->PopFont();
      }

      TextFill::TextFill(Display::FillIndex fill)
      : fill(fill)
      {
        switch ( fill) {
        case Display::buttonBackgroundFillIndex:
        case Display::defaultButtonBackgroundFillIndex:
        case Display::positiveButtonBackgroundFillIndex:
        case Display::negativeButtonBackgroundFillIndex:
        case Display::toolbarButtonBackgroundFillIndex:
        case Display::entryBackgroundFillIndex:
          leftBorder=1;
          rightBorder=1;
          topBorder=0;
          bottomBorder=0;
          break;
        case Display::hprogressBackgroundFillIndex:
        case Display::vprogressBackgroundFillIndex:
        case Display::scrolledBackgroundFillIndex:
        case Display::boxedBackgroundFillIndex:
        case Display::menuWindowBackgroundFillIndex:
        case Display::popupWindowBackgroundFillIndex:
          leftBorder=1;
          rightBorder=1;
          topBorder=1;
          bottomBorder=1;
          break;
        case Display::statusbarBackgroundFillIndex:
          leftBorder=0;
          rightBorder=0;
          topBorder=1;
          bottomBorder=0;
          break;
        case Display::menuPulldownBackgroundFillIndex:
          leftBorder=0;
          rightBorder=1;
          topBorder=0;
          bottomBorder=0;
          break;
        case Display::tabRiderBackgroundFillIndex:
          leftBorder=1;
          rightBorder=leftBorder;
          topBorder=1;
          bottomBorder=0;
          break;
        default:
          leftBorder=0;
          rightBorder=0;
          topBorder=0;
          bottomBorder=0;
        }
      }

      void TextFill::Draw(OS::DrawInfo* draw,
                         int xOff, int yOff, size_t width, size_t height,
                         int x, int y, size_t w, size_t h)
      {
        Color backgroundColor=OS::display->GetColor(Display::backgroundColor);

        switch (fill) {
        case Display::backgroundFillIndex:
          draw->PushForeground(Display::backgroundColor);
          draw->FillRectangle(x,y,w,h);
          draw->PopForeground();
          break;
        case Display::tableBackgroundFillIndex:
        case Display::tableBackground2FillIndex:
          if (draw->selected) {
            draw->PushForeground(Display::fillColor);
          }
          else {
            draw->PushForeground(dynamic_cast<Theme*>(OS::display->GetTheme())->tableBackgroundColor);
          }
          draw->FillRectangle(x,y,w,h);
          draw->PopForeground();
          break;
        case Display::buttonBackgroundFillIndex:
        case Display::defaultButtonBackgroundFillIndex:
        case Display::toolbarButtonBackgroundFillIndex:
          draw->PushForeground(dynamic_cast<Theme*>(OS::display->GetTheme())->buttonBackgroundColor);
          draw->FillRectangle(x,y,w,h);
          draw->PopForeground();

          draw->PushFont(OS::display->GetFont());
          draw->PushForeground(Display::blackColor);

          for (size_t i=yOff; i<yOff+(size_t)height; ++i) {
            draw->DrawString(xOff,i,L"[");
          }

          for (size_t i=yOff; i<yOff+(size_t)height; ++i) {
            draw->DrawString(xOff+width-1,i,L"]");
          }

          draw->PopForeground();
          draw->PopFont();
          break;
        case Display::positiveButtonBackgroundFillIndex:
          draw->PushForeground(dynamic_cast<Theme*>(OS::display->GetTheme())->positiveButtonBackgroundColor);
          draw->FillRectangle(x,y,w,h);
          draw->PopForeground();

          draw->PushFont(OS::display->GetFont());
          draw->PushForeground(Display::blackColor);

          for (size_t i=yOff; i<yOff+(size_t)height; ++i) {
            draw->DrawString(xOff,i,L"<");
          }

          for (size_t i=yOff; i<yOff+(size_t)height; ++i) {
            draw->DrawString(xOff+width-1,i,L">");
          }

          draw->PopForeground();
          draw->PopFont();
          break;
        case Display::negativeButtonBackgroundFillIndex:
          draw->PushForeground(dynamic_cast<Theme*>(OS::display->GetTheme())->negativeButtonBackgroundColor);
          draw->FillRectangle(x,y,w,h);
          draw->PopForeground();

          draw->PushFont(OS::display->GetFont());
          draw->PushForeground(Display::blackColor);

          for (size_t i=yOff; i<yOff+(size_t)height; ++i) {
            draw->DrawString(xOff,i,L"[");
          }

          for (size_t i=yOff; i<yOff+(size_t)height; ++i) {
            draw->DrawString(xOff+width-1,i,L"]");
          }

          draw->PopForeground();
          draw->PopFont();
          break;
        case Display::scrollButtonBackgroundFillIndex:
          draw->PushForeground(dynamic_cast<Theme*>(OS::display->GetTheme())->buttonBackgroundColor);
          draw->FillRectangle(x,y,w,h);
          draw->PopForeground();
          break;
          /* combo */
          /* editCombo */
        case Display::entryBackgroundFillIndex:
          if (draw->selected) {
            draw->PushForeground(Display::fillColor);
          }
          else {
            draw->PushForeground(dynamic_cast<Theme*>(OS::display->GetTheme())->tableBackgroundColor);
          }
          draw->FillRectangle(x,y,w,h);
          draw->PopForeground();

          draw->PushFont(OS::display->GetFont());
          draw->PushForeground(Display::blackColor);

          for (size_t i=yOff; i<yOff+(size_t)height; ++i) {
            draw->DrawString(xOff,i,L"[");
          }

          for (size_t i=yOff; i<yOff+(size_t)height; ++i) {
            draw->DrawString(xOff+width-1,i,L"]");
          }

          draw->PopForeground();
          draw->PopFont();
          break;
        case Display::menuStripBackgroundFillIndex:
          draw->PushForeground(Display::backgroundColor);
          draw->FillRectangle(x,y,w,h);
          draw->PopForeground();
          break;

        case Display::hprogressBackgroundFillIndex:
        case Display::vprogressBackgroundFillIndex:
          draw->PushForeground(Display::backgroundColor);
          draw->FillRectangle(x,y,w,h);
          draw->PopForeground();

          draw->PushFont(OS::display->GetFont());
          draw->PushForeground(Display::blackColor);

          draw->DrawFillString(xOff,yOff,L"\x250c",backgroundColor);
          draw->DrawFillString(xOff+width-1,yOff,L"\x2510",backgroundColor);
          draw->DrawFillString(xOff,yOff+height-1,L"\x2514",backgroundColor);
          draw->DrawFillString(xOff+width-1,yOff+height-1,L"\x2518",backgroundColor);

          // top
          for (int i=xOff+1; i<=xOff+(int)width-2; i++) {
            draw->DrawFillString(i,yOff,L"\x2500",backgroundColor);
          }

          // left
          for (int i=yOff+1; i<=yOff+(int)height-2; i++) {
            draw->DrawFillString(xOff,i,L"\x2502",backgroundColor);
          }

          // right
          for (int i=yOff+1; i<=yOff+(int)height-2; i++) {
            draw->DrawFillString(xOff+width-1,i,L"\x2502",backgroundColor);
          }

          // bottom
          for (int i=xOff+1; i<=xOff+(int)width-2; i++) {
            draw->DrawFillString(i,yOff+height-1,L"\x2500",backgroundColor);
          }

          draw->PopForeground();
          draw->PopFont();
          break;
        case Display::hprogressFillIndex:
        case Display::vprogressFillIndex:
          draw->PushForeground(Display::fillColor);
          draw->FillRectangle(x,y,w,h);
          draw->PopForeground();
          break;
        case Display::tabRiderBackgroundFillIndex:
          draw->PushForeground(Display::backgroundColor);
          draw->FillRectangle(x,y,w,h);
          draw->PopForeground();

          draw->PushFont(OS::display->GetFont());
          draw->PushForeground(Display::blackColor);

          draw->DrawFillString(xOff,yOff,L"\x250c",backgroundColor);
          draw->DrawFillString(xOff+width-1,yOff,L"\x2510",backgroundColor);

          // top
          for (int i=xOff+1; i<=xOff+(int)width-2; i++) {
            draw->DrawFillString(i,yOff,L"\x2500",backgroundColor);
          }

          // left
          for (int i=yOff+1; i<=yOff+(int)height-1; i++) {
            draw->DrawFillString(xOff,i,L"\x2502",backgroundColor);
          }

          // right
          for (int i=yOff+1; i<=yOff+(int)height-1; i++) {
            draw->DrawFillString(xOff+width-1,i,L"\x2502",backgroundColor);
          }

          draw->PopForeground();
          draw->PopFont();
          break;
        case Display::menuPulldownBackgroundFillIndex:
          if (draw->selected) {
            draw->PushForeground(Display::fillColor);
          }
          else {
            draw->PushForeground(Display::backgroundColor);
          }
          draw->FillRectangle(x,y,w,h);
          draw->PopForeground();
          break;
        case Display::menuEntryBackgroundFillIndex:
          if (draw->selected) {
            draw->PushForeground(Display::fillColor);
          }
          else {
            draw->PushForeground(Display::backgroundColor);
          }
          draw->FillRectangle(x,y,w,h);
          draw->PopForeground();
          break;
        case Display::menuWindowBackgroundFillIndex:
        case Display::popupWindowBackgroundFillIndex:
          draw->PushForeground(Display::backgroundColor);
          draw->FillRectangle(x,y,w,h);
          draw->PopForeground();

          draw->PushFont(OS::display->GetFont());
          draw->PushForeground(Display::blackColor);

          draw->DrawFillString(xOff,yOff,L"\x250c",backgroundColor);
          draw->DrawFillString(xOff+width-1,yOff,L"\x2510",backgroundColor);
          draw->DrawFillString(xOff,yOff+height-1,L"\x2514",backgroundColor);
          draw->DrawFillString(xOff+width-1,yOff+height-1,L"\x2518",backgroundColor);

          // top
          for (int i=xOff+1; i<=xOff+(int)width-2; i++) {
            draw->DrawFillString(i,yOff,L"\x2500",backgroundColor);
          }

          // left
          for (int i=yOff+1; i<=yOff+(int)height-2; i++) {
            draw->DrawFillString(xOff,i,L"\x2502",backgroundColor);
          }

          // right
          for (int i=yOff+1; i<=yOff+(int)height-2; i++) {
            draw->DrawFillString(xOff+width-1,i,L"\x2502",backgroundColor);
          }

          // bottom
          for (int i=xOff+1; i<=xOff+(int)width-2; i++) {
            draw->DrawFillString(i,yOff+height-1,L"\x2500",backgroundColor);
          }

          draw->PopForeground();
          draw->PopFont();
          break;
        case Display::graphBackgroundFillIndex:
          draw->PushForeground(Display::backgroundColor);
          draw->FillRectangle(x,y,w,h);
          draw->PopForeground();
          break;
        case Display::toolbarBackgroundFillIndex:
          draw->PushForeground(Display::backgroundColor);
          draw->FillRectangle(x,y,w,h);
          draw->PopForeground();
          break;
        case Display::listboxBackgroundFillIndex:
          draw->PushForeground(Display::backgroundColor);
          draw->FillRectangle(x,y,w,h);
          draw->PopForeground();
          break;
        case Display::scrolledBackgroundFillIndex:
        case Display::boxedBackgroundFillIndex:
        case Display::plateBackgroundFillIndex:
          draw->PushForeground(Display::backgroundColor);
          draw->FillRectangle(x,y,w,h);
          draw->PopForeground();

          draw->PushFont(OS::display->GetFont());
          draw->PushForeground(Display::blackColor);

          draw->DrawFillString(xOff,yOff,L"\x250c",backgroundColor);
          draw->DrawFillString(xOff+width-1,yOff,L"\x2510",backgroundColor);
          draw->DrawFillString(xOff,yOff+height-1,L"\x2514",backgroundColor);
          draw->DrawFillString(xOff+width-1,yOff+height-1,L"\x2518",backgroundColor);

          // top
          for (int i=xOff+1; i<=xOff+(int)width-2; i++) {
            draw->DrawFillString(i,yOff,L"\x2500",backgroundColor);
          }

          // left
          for (int i=yOff+1; i<=yOff+(int)height-2; i++) {
            draw->DrawFillString(xOff,i,L"\x2502",backgroundColor);
          }

          // right
          for (int i=yOff+1; i<=yOff+(int)height-2; i++) {
            draw->DrawFillString(xOff+width-1,i,L"\x2502",backgroundColor);
          }

          // bottom
          for (int i=xOff+1; i<=xOff+(int)width-2; i++) {
            draw->DrawFillString(i,yOff+height-1,L"\x2500",backgroundColor);
          }

          draw->PopForeground();
          draw->PopFont();
          break;
        case Display::statusbarBackgroundFillIndex:
          draw->PushForeground(Display::backgroundColor);
          draw->FillRectangle(x,y,w,h);
          draw->PopForeground();

          draw->PushFont(OS::display->GetFont());
          draw->PushForeground(Display::blackColor);

          for (int i=xOff; i<=xOff+(int)width-1; i++) {
            draw->DrawFillString(i,yOff,L"\x2500",backgroundColor);
          }

          draw->PopForeground();
          draw->PopFont();
          break;
        default:
          assert(false);
          break;
        }
      }

      TextFrame::TextFrame(Display::FrameIndex frame)
      : frame(frame)
      {
        switch (frame) {
        case Display::tabFrameIndex:
          leftBorder=1;
          rightBorder=leftBorder;
          topBorder=1;
          bottomBorder=topBorder;
          break;
        default:
          leftBorder=0;
          rightBorder=leftBorder;
          topBorder=0;
          bottomBorder=topBorder;
          break;
        }

        minWidth=leftBorder+rightBorder;
        minHeight=topBorder+bottomBorder;
      }

      void TextFrame::Draw(OS::DrawInfo* draw, int x, int y, size_t w, size_t h)
      {
        if (minWidth==0 && minHeight==0) {
          return;
        }

        Color backgroundColor=OS::display->GetColor(Display::backgroundColor);

        switch (frame) {
        case Display::tabFrameIndex:
          draw->PushFont(OS::display->GetFont());
          draw->PushForeground(Display::blackColor);

          draw->DrawFillString(x,y,L"\x250c",backgroundColor);
          draw->DrawFillString(x+w-1,y,L"\x2510",backgroundColor);
          draw->DrawFillString(x,y+h-1,L"\x2514",backgroundColor);
          draw->DrawFillString(x+w-1,y+h-1,L"\x2518",backgroundColor);

          // top
          for (int i=x+1; i<=x+(int)w-2; i++) {
            draw->DrawFillString(i,y,L"\x2500",backgroundColor);
          }

          // left
          for (int i=y+1; i<=y+(int)h-2; i++) {
            draw->DrawFillString(x,i,L"\x2502",backgroundColor);
          }

          // right
          for (int i=y+1; i<=y+(int)h-2; i++) {
            draw->DrawFillString(x+w-1,i,L"\x2502",backgroundColor);
          }

          // bottom
          for (int i=x+1; i<=x+(int)w-2; i++) {
            draw->DrawFillString(i,y+h-1,L"\x2500",backgroundColor);
          }

          draw->PopForeground();
          draw->PopFont();
          break;
        default:
          assert(false);
          break;
        }
      }

      Theme::Theme(OS::Display *display)
      : OS::Base::Theme(display)
      {
        const char* colorNames[Display::colorCount];

        if (display->GetColorMode()==Display::colorModeMonochrome) {
          colorNames[Display::backgroundColor]="white";
          colorNames[Display::tableTextColor]="black";
          colorNames[Display::textColor]="black";
          colorNames[Display::textSelectColor]="black";
          colorNames[Display::textDisabledColor]="black";
          colorNames[Display::fillColor]="black";
          colorNames[Display::fillTextColor]="white";
          colorNames[Display::graphScaleColor]="black";
          colorNames[Display::blackColor]="black";
          colorNames[Display::whiteColor]="white";
          colorNames[Display::tabTextColor]="black";
          colorNames[Display::tabTextSelectColor]="black";
          colorNames[Display::editTextColor]="black";
          colorNames[Display::editTextFillColor]="white";
        }
        else {
          colorNames[Display::backgroundColor]="white";
          colorNames[Display::tableTextColor]="white";
          colorNames[Display::textColor]="black";
          colorNames[Display::textSelectColor]="black";
          colorNames[Display::textDisabledColor]="black";
          colorNames[Display::fillColor]="cyan";
          colorNames[Display::fillTextColor]="black";
          colorNames[Display::graphScaleColor]="black";
          colorNames[Display::blackColor]="black";
          colorNames[Display::whiteColor]="white";
          colorNames[Display::tabTextColor]="black";
          colorNames[Display::tabTextSelectColor]="black";
          colorNames[Display::editTextColor]="black";
          colorNames[Display::editTextFillColor]="white";
        }

        for (size_t i=0; i<Display::colorCount; i++) {
          display->AllocateNamedColor(colorNames[i],color[i]);
        }

        if (display->GetColorMode()==Display::colorModeMonochrome) {
          display->AllocateNamedColor("white",tableBackgroundColor);
          display->AllocateNamedColor("white",buttonBackgroundColor);
          display->AllocateNamedColor("white",positiveButtonBackgroundColor);
          display->AllocateNamedColor("white",negativeButtonBackgroundColor);
        }
        else {
          display->AllocateNamedColor("blue",tableBackgroundColor);
          display->AllocateNamedColor("green",buttonBackgroundColor);
          display->AllocateNamedColor("green",positiveButtonBackgroundColor);
          display->AllocateNamedColor("red",negativeButtonBackgroundColor);
        }

        fill[Display::backgroundFillIndex]=new TextFill(Display::backgroundFillIndex);
        fill[Display::tableBackgroundFillIndex]=new TextFill(Display::tableBackgroundFillIndex);
        fill[Display::tableBackground2FillIndex]=new TextFill(Display::tableBackground2FillIndex);
        fill[Display::buttonBackgroundFillIndex]=new TextFill(Display::buttonBackgroundFillIndex);
        fill[Display::positiveButtonBackgroundFillIndex]=new TextFill(Display::positiveButtonBackgroundFillIndex);
        fill[Display::negativeButtonBackgroundFillIndex]=new TextFill(Display::negativeButtonBackgroundFillIndex);
        fill[Display::defaultButtonBackgroundFillIndex]=new TextFill(Display::defaultButtonBackgroundFillIndex);
        fill[Display::toolbarButtonBackgroundFillIndex]=new TextFill(Display::toolbarButtonBackgroundFillIndex);
        fill[Display::scrollButtonBackgroundFillIndex]=new TextFill(Display::scrollButtonBackgroundFillIndex);
        fill[Display::comboBackgroundFillIndex]=fill[Display::buttonBackgroundFillIndex];
        fill[Display::editComboBackgroundFillIndex]=fill[Display::buttonBackgroundFillIndex];
        fill[Display::entryBackgroundFillIndex]=new TextFill(Display::entryBackgroundFillIndex);
        fill[Display::hprogressBackgroundFillIndex]=new TextFill(Display::hprogressBackgroundFillIndex);
        fill[Display::hprogressFillIndex]=new TextFill(Display::hprogressFillIndex);
        fill[Display::vprogressBackgroundFillIndex]=new TextFill(Display::vprogressBackgroundFillIndex);
        fill[Display::vprogressFillIndex]=new TextFill(Display::vprogressFillIndex);
        fill[Display::hknobBoxFillIndex]=fill[Display::backgroundFillIndex];
        fill[Display::vknobBoxFillIndex]=fill[Display::backgroundFillIndex];
        fill[Display::tabBackgroundFillIndex]=fill[Display::backgroundFillIndex];
        fill[Display::tabRiderBackgroundFillIndex]=new TextFill(Display::tabRiderBackgroundFillIndex);
        fill[Display::columnBackgroundFillIndex]=fill[Display::buttonBackgroundFillIndex];
        fill[Display::columnLeftBackgroundFillIndex]=fill[Display::buttonBackgroundFillIndex];
        fill[Display::columnMiddleBackgroundFillIndex]=fill[Display::buttonBackgroundFillIndex];
        fill[Display::columnRightBackgroundFillIndex]=fill[Display::buttonBackgroundFillIndex];
        fill[Display::columnEndBackgroundFillIndex]=fill[Display::backgroundFillIndex];
        fill[Display::menuStripBackgroundFillIndex]=new TextFill(Display::menuStripBackgroundFillIndex);
        fill[Display::menuPulldownBackgroundFillIndex]=new TextFill(Display::menuPulldownBackgroundFillIndex);
        fill[Display::menuEntryBackgroundFillIndex]=new TextFill(Display::menuEntryBackgroundFillIndex);
        fill[Display::menuWindowBackgroundFillIndex]=new TextFill(Display::menuWindowBackgroundFillIndex);
        fill[Display::popupWindowBackgroundFillIndex]=new TextFill(Display::popupWindowBackgroundFillIndex);
        fill[Display::dialogWindowBackgroundFillIndex]=fill[Display::backgroundFillIndex];
        fill[Display::tooltipWindowBackgroundFillIndex]=fill[Display::backgroundFillIndex];
        fill[Display::graphBackgroundFillIndex]=new TextFill(Display::graphBackgroundFillIndex);
        fill[Display::toolbarBackgroundFillIndex]=fill[Display::backgroundFillIndex];
        fill[Display::labelBackgroundFillIndex]=fill[Display::backgroundFillIndex];
        fill[Display::listboxBackgroundFillIndex]=new TextFill(Display::listboxBackgroundFillIndex);
        fill[Display::scrolledBackgroundFillIndex]=new TextFill(Display::scrolledBackgroundFillIndex);
        fill[Display::hscrollBackgroundFillIndex]=fill[Display::backgroundFillIndex];
        fill[Display::vscrollBackgroundFillIndex]=fill[Display::backgroundFillIndex];
        fill[Display::boxedBackgroundFillIndex]=new TextFill(Display::boxedBackgroundFillIndex);
        fill[Display::plateBackgroundFillIndex]=new TextFill(Display::plateBackgroundFillIndex);
        fill[Display::statusbarBackgroundFillIndex]=new TextFill(Display::statusbarBackgroundFillIndex);

        frame[Display::tabFrameIndex]=new TextFrame(Display::tabFrameIndex);
        /*frame[Display::columnFrameIndex]=new TextFrame(Display::columnFrameIndex);
        frame[Display::columnLeftFrameIndex]=frame[Display::columnFrameIndex];
        frame[Display::columnMiddleFrameIndex]=frame[Display::columnFrameIndex];
        frame[Display::columnRightFrameIndex]=frame[Display::columnFrameIndex];
        frame[Display::columnEndFrameIndex]=emptyFrame;*/
        frame[Display::hscaleFrameIndex]=emptyFrame;
        frame[Display::vscaleFrameIndex]=emptyFrame;
        frame[Display::focusFrameIndex]=emptyFrame;
        frame[Display::statuscellFrameIndex]=emptyFrame;
        //frame[Display::plateFrameIndex]=emptyFrame;
        frame[Display::valuebarFrameIndex]=emptyFrame;
        frame[Display::groupFrameIndex]=emptyFrame; // TODO
        //frame[Display::menuPulldownFrameIndex]=new TextFrame(Display::menuPulldownFrameIndex);

        image[Display::hknobImageIndex]=new TextImage(Display::hknobImageIndex);
        image[Display::vknobImageIndex]=new TextImage(Display::vknobImageIndex);
        image[Display::arrowLeftImageIndex]=new TextImage(Display::arrowLeftImageIndex);
        image[Display::arrowRightImageIndex]=new TextImage(Display::arrowRightImageIndex);
        image[Display::arrowUpImageIndex]=new TextImage(Display::arrowUpImageIndex);
        image[Display::arrowDownImageIndex]=new TextImage(Display::arrowDownImageIndex);
        image[Display::scrollLeftImageIndex]=new TextImage(Display::scrollLeftImageIndex);
        image[Display::scrollRightImageIndex]=new TextImage(Display::scrollRightImageIndex);
        image[Display::scrollUpImageIndex]=new TextImage(Display::scrollUpImageIndex);
        image[Display::scrollDownImageIndex]=new TextImage(Display::scrollDownImageIndex);
        image[Display::comboImageIndex]=new TextImage(Display::comboImageIndex);
        image[Display::comboEditButtonImageIndex]=new TextImage(Display::comboEditButtonImageIndex);
        image[Display::comboDividerImageIndex]=new TextImage(Display::comboDividerImageIndex);
        image[Display::treeExpanderImageIndex]=new TextImage(Display::treeExpanderImageIndex);
        image[Display::hscaleKnobImageIndex]=new TextImage(Display::hscaleKnobImageIndex);
        image[Display::vscaleKnobImageIndex]=new TextImage(Display::vscaleKnobImageIndex);
        image[Display::leftSliderImageIndex]=new TextImage(Display::leftSliderImageIndex);
        image[Display::rightSliderImageIndex]=new TextImage(Display::rightSliderImageIndex);
        image[Display::topSliderImageIndex]=new TextImage(Display::topSliderImageIndex);
        image[Display::bottomSliderImageIndex]=new TextImage(Display::bottomSliderImageIndex);
        image[Display::menuDividerImageIndex]=new TextImage(Display::menuDividerImageIndex);
        image[Display::menuSubImageIndex]=new TextImage(Display::menuSubImageIndex);
        image[Display::menuCheckImageIndex]=new TextImage(Display::menuCheckImageIndex);
        image[Display::menuRadioImageIndex]=new TextImage(Display::menuRadioImageIndex);
        image[Display::hpaneHandleImageIndex]=new TextImage(Display::hpaneHandleImageIndex);
        image[Display::vpaneHandleImageIndex]=new TextImage(Display::vpaneHandleImageIndex);
        image[Display::radioImageIndex]=new TextImage(Display::radioImageIndex);
        image[Display::checkImageIndex]=new TextImage(Display::checkImageIndex);
        image[Display::ledImageIndex]=new TextImage(Display::ledImageIndex);

        stockImage[imageClose]=new TextStockImage(imageClose);

      }

      Theme::~Theme()
      {
        display->FreeColor(tableBackgroundColor);
        display->FreeColor(buttonBackgroundColor);
        display->FreeColor(positiveButtonBackgroundColor);
        display->FreeColor(negativeButtonBackgroundColor);

        for (size_t i=0; i<Display::colorCount; i++) {
          display->FreeColor(color[i]);
        }
      }

      bool Theme::HasFontSettings() const
      {
        return true;
      }

      std::wstring Theme::GetProportionalFontName() const
      {
        return L"";
      }

      double Theme::GetProportionalFontSize() const
      {
        return 1;
      }

      std::wstring Theme::GetFixedFontName() const
      {
        return L"";
      }

      double Theme::GetFixedFontSize() const
      {
        return 1;
      }

      size_t Theme::GetSpaceHorizontal(Display::Space space) const
      {
        switch (space) {
        case Display::spaceWindowBorder:
          return 0;
        case Display::spaceInterGroup:
          return 1;
        case Display::spaceGroupIndent:
          return 1;
        case Display::spaceInterObject:
          return 0;
        case Display::spaceObjectBorder:
          return 0;
        case Display::spaceIntraObject:
          return 1;
        case Display::spaceLabelObject:
          return 1;
        case Display::spaceObjectDetail:
          return 1;
        }

        return 1;
      }

      size_t Theme::GetSpaceVertical(Display::Space space) const
      {
        switch (space) {
        case Display::spaceWindowBorder:
          return 0;
        case Display::spaceInterGroup:
          return 0;
        case Display::spaceGroupIndent:
          return 0;
        case Display::spaceInterObject:
          return 0;
        case Display::spaceObjectBorder:
          return 0;
        case Display::spaceIntraObject:
          return 0;
        case Display::spaceLabelObject:
          return 0;
        case Display::spaceObjectDetail:
          return 0;
        }

        return 1;
      }

      size_t Theme::GetIconWidth() const
      {
        return 1;
      }

      size_t Theme::GetIconHeight() const
      {
        return 1;
      }

      size_t Theme::GetDragStartSensitivity() const
      {
        return 1;
      }

      size_t Theme::GetMouseClickHoldSensitivity() const
      {
        return 1;
      }

      size_t Theme::GetMinimalButtonWidth() const
      {
        return 0;
      }

      size_t Theme::GetFirstTabOffset() const
      {
        return 0;
      }

      size_t Theme::GetLastTabOffset() const
      {
        return 0;
      }

      OS::Theme::ScrollKnobMode Theme::GetScrollKnobMode() const
      {
        return OS::Theme::scrollKnobModeDouble;
      }

      OS::Theme::ToolbarPosition Theme::GetToolbarPosition() const
      {
        return OS::Theme::toolbarTop;
      }

      bool Theme::ShowButtonImages() const
      {
        return false;
      }

      OS::Theme::MenuType Theme::GetMenuType() const
      {
        return menuTypeStrip;
      }
    }
  }
}
