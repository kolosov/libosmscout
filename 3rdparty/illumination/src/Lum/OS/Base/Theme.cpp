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

#include <Lum/OS/Base/Theme.h>

#include <Lum/Base/Size.h>
#include <Lum/Base/Util.h>

#include <cmath>

#define DEFAULT_PROP_FONT_SIZE 12
#define DEFAULT_FIXED_FONT_SIZE 13

namespace Lum {
  namespace OS {
    namespace Base {

      /**
        Draw background using a plain color.
      */
      class FrameFill : public Fill
      {
      public:
        enum Type {
          none,
          single3DIn,
          single3DOut,
          singleFrame, //! Simple black frame of 1 pixel size
          double3DOut,
          ridge,
          button,      //! graphical button frame
          box,         //! box frame
          tabRider,    //! "normal" tab rider
          label,       //! Draw focus frame only if draw->focused
        };

      private:
        Type  type;
        Color color;

      public:
        FrameFill(Type type, Color color);

        void Draw(DrawInfo* draw,
                  int xOff, int yOff, size_t width, size_t height,
                  int x, int y, size_t w, size_t h);
      };

      FrameFill::FrameFill(Type type, Color color)
      : type(type),
        color(color)
      {
        switch (type) {
        case none:
          topBorder=0;
          bottomBorder=0;
          leftBorder=0;
          rightBorder=0;
          break;
        case single3DIn:
        case single3DOut:
        case singleFrame:
          topBorder=1;
          bottomBorder=1;
          leftBorder=1;
          rightBorder=1;
          break;
        case double3DOut:
        case ridge:
          topBorder=2;
          bottomBorder=2;
          leftBorder=2;
          rightBorder=2;
          break;
        case button:
          topBorder=4;
          bottomBorder=4;
          leftBorder=4;
          rightBorder=4;
          break;
        case box:
          topBorder=2;
          bottomBorder=2;
          leftBorder=2;
          rightBorder=2;
          break;
        case tabRider:
          topBorder=2;
          bottomBorder=0;
          leftBorder=2;
          rightBorder=2;
          transparentBorder=true;
          break;
        case label:
          topBorder=1;
          bottomBorder=1;
          leftBorder=1;
          rightBorder=1;
          transparentBorder=true;
          break;
        }
      }

      void FrameFill::Draw(OS::DrawInfo* draw,
                           int xOff, int yOff, size_t width, size_t height,
                           int x, int y, size_t w, size_t h)
      {
        Color shine,shadow,halfShadow;
        int   type;

        x=xOff;
        y=yOff;
        w=width;
        h=height;

        draw->PushForeground(color);
        draw->FillRectangle(x,y,w,h);
        draw->PopForeground();

        shine=dynamic_cast<DefaultTheme*>(display->GetTheme())->shineColor;
        shadow=dynamic_cast<DefaultTheme*>(display->GetTheme())->shadowColor;
        halfShadow=dynamic_cast<DefaultTheme*>(display->GetTheme())->halfShadowColor;

        type=this->type;

        if (draw->selected) {
          switch (type) {
          case single3DOut:
            type=single3DIn;
            break;
          default:
            break;
          }
        }

        switch (type) {
        case double3DOut:
          draw->PushForeground(shine);
          draw->DrawLine(x,y+h-1,x,y+1);
          draw->DrawLine(x,y,x+w-1,y);
          draw->DrawLine(x+1,y+h-2,x+1,y+2);
          draw->DrawLine(x+1,y+1,x+w-2,y+1);
          draw->PopForeground();

          draw->PushForeground(shadow);
          draw->DrawLine(x+w-1,y+1,x+w-1,y+h-1);
          draw->DrawLine(x+w-1,y+h-1,x,y+h-1);
          draw->DrawLine(x+w-2,y+2,x+w-2,y+h-2);
          draw->DrawLine(x+w-2,y+h-2,x+1,y+h-2);
          draw->PopForeground();
          break;

        case single3DIn:
        case single3DOut:
          if (type==single3DIn) {
            draw->PushForeground(shadow);
          }
          else {
            draw->PushForeground(shine);
          }
          draw->DrawLine(x,y+h-1,x,y+1);
          draw->DrawLine(x,y,x+w-1,y);
          draw->PopForeground();

          if (type==single3DIn) {
            draw->PushForeground(shine);
          }
          else {
            draw->PushForeground(shadow);
          }
          draw->DrawLine(x+w-1,y+1,x+w-1,y+h-1);
          draw->DrawLine(x+w-1,y+h-1,x,y+h-1);
          draw->PopForeground();
          break;

        case singleFrame:
          draw->PushForeground(shadow);
          draw->DrawRectangle(x,y,w,h);
          draw->PopForeground();
          break;

        case ridge:
          draw->PushForeground(shine);
          draw->DrawLine(x,y,x+w-2,y);
          draw->DrawLine(x+2,y+h-2,x+w-2,y+h-2);
          draw->DrawLine(x,y+h-1,x,y);
          draw->DrawLine(x+w-2,y+2,x+w-2,y+h-2);
          draw->PopForeground();

          draw->PushForeground(shadow);
          draw->DrawLine(x+w-1,y,x+w-1,y+h-1);
          draw->DrawLine(x+1,y+h-2,x+1,y+1);
          draw->DrawLine(x+1,y+h-1,x+w-1,y+h-1);
          draw->DrawLine(x+1,y+1,x+w-2,y+1);
          draw->PopForeground();
          break;

        case button:
          if (draw->selected) {
            draw->PushForeground(OS::Display::blackColor);
            draw->DrawRectangle(x,y,w,h);
            draw->PopForeground();
            draw->PushForeground(halfShadow);
            draw->DrawRectangle(x+1,y+1,w-2,h-2);
            draw->PopForeground();
          }
          else {
            draw->PushForeground(shine);
            draw->DrawLine(x,y+h-1,x,y+1);
            draw->DrawLine(x,y,x+w-2,y);
            draw->PopForeground();

            draw->PushForeground(OS::Display::backgroundColor);
            draw->DrawLine(x+1,y+h-2,x+1,y+2);
            draw->DrawLine(x+1,y+1,x+w-3,y+1);
            draw->PopForeground();

            draw->PushForeground(shadow);
            draw->DrawLine(x+w-1,y,x+w-1,y+h-1);
            draw->DrawLine(x+w-1,y+h-1,x,y+h-1);
            draw->PopForeground();

            draw->PushForeground(halfShadow);
            draw->DrawLine(x+w-2,y+1,x+w-2,y+h-2);
            draw->DrawLine(x+w-2,y+h-2,x+1,y+h-2);
            draw->PopForeground();
          }
          break;

        case box:
          draw->PushForeground(halfShadow);
          draw->DrawLine(x,y+h-1,x,y+1);
          draw->DrawLine(x,y,x+w-2,y);
          draw->PopForeground();

          draw->PushForeground(shadow);
          draw->DrawLine(x+1,y+h-2,x+1,y+2);
          draw->DrawLine(x+1,y+1,x+w-3,y+1);
          draw->PopForeground();

          draw->PushForeground(shine);
          draw->DrawLine(x+w-1,y,x+w-1,y+h-1);
          draw->DrawLine(x+w-1,y+h-1,x,y+h-1);
          draw->PopForeground();

          draw->PushForeground(OS::Display::backgroundColor);
          draw->DrawLine(x+w-2,y+1,x+w-2,y+h-2);
          draw->DrawLine(x+w-2,y+h-2,x+1,y+h-2);
          draw->PopForeground();
          break;

        case tabRider:
          // right
          draw->PushForeground(shadow);
          draw->DrawLine(x+w-1,y+3,x+w-1,y+h-1);
          draw->DrawLine(x+w-2,y+1,x+w-2,y+h-1);
          draw->PopForeground();

          draw->PushForeground(shine);

          // top
          draw->DrawLine(x+3,y  ,x+w-4,y);
          draw->DrawLine(x+1,y+1,x+w-3,y+1);

          // left
          draw->DrawLine(x  ,y+3,x    ,y+h-1);
          draw->DrawLine(x+1,y+1,x+1  ,y+h-1);

          draw->PopForeground();
          break;
        case label:
          if (draw->focused) {
            draw->PushForeground(OS::Display::textColor);
            draw->PushDash("\002\002",2,OS::DrawInfo::fDash);
            draw->DrawLine(x          ,y,      x+w-1,y);
            draw->DrawLine(x+w-1,y,            x+w-1,y+h-1);
            draw->DrawLine(x+w-1,y+h-1,x,            y+h-1);
            draw->DrawLine(x          ,y+h-1,x,      y);
            draw->PopDash();
            draw->PopForeground();
          }
          break;
        }
      }

      class SelectFill : public Fill
      {
      private:
        Color colorA;
        Color colorB;

      public:
        SelectFill(Color colorA, Color colorB);

        void Draw(DrawInfo* draw,
                  int xOff, int yOff, size_t width, size_t height,
                  int x, int y, size_t w, size_t h);
      };

      SelectFill::SelectFill(Color colorA, Color colorB)
      : colorA(colorA),
        colorB(colorB)
      {
        // no code
      }

      void SelectFill::Draw(OS::DrawInfo* draw,
                            int xOff, int yOff, size_t width, size_t height,
                            int x, int y, size_t w, size_t h)
      {
        if (draw->selected) {
          draw->PushForeground(colorB);
        }
        else {
          draw->PushForeground(colorA);
        }

        draw->FillRectangle(x,y,w,h);

        draw->PopForeground();
      }

     class EditFill : public Fill
      {
      private:
        Color colorA;
        Color colorB;

      public:
        EditFill(Color colorA, Color colorB);

        void Draw(DrawInfo* draw,
                  int xOff, int yOff, size_t width, size_t height,
                  int x, int y, size_t w, size_t h);
      };

      EditFill::EditFill(Color colorA, Color colorB)
      : colorA(colorA),
        colorB(colorB)
      {
        topBorder=2;
        bottomBorder=2;
        leftBorder=2;
        rightBorder=2;
      }

      void EditFill::Draw(OS::DrawInfo* draw,
                          int xOff, int yOff, size_t width, size_t height,
                          int x, int y, size_t w, size_t h)
      {
        Color shine,shadow, halfShadow, halfShine;

        shine=dynamic_cast<DefaultTheme*>(display->GetTheme())->shineColor;
        shadow=dynamic_cast<DefaultTheme*>(display->GetTheme())->shadowColor;
        halfShadow=dynamic_cast<DefaultTheme*>(display->GetTheme())->halfShadowColor;
        halfShine=dynamic_cast<DefaultTheme*>(display->GetTheme())->halfShineColor;

        if (draw->selected) {
          draw->PushForeground(colorB);
        }
        else {
          draw->PushForeground(colorA);
        }

        draw->FillRectangle(x,y,w,h);
        draw->PopForeground();

        if (!draw->selected) {
          draw->PushForeground(shadow);
          draw->DrawLine(x,y+h-1,x,y+1);
          draw->DrawLine(x,y,x+w-1,y);
          draw->PopForeground();

          draw->PushForeground(halfShadow);
          draw->DrawLine(x+1,y+h-2,x+1,y+2);
          draw->DrawLine(x+1,y+1,x+w-2,y+1);
          draw->PopForeground();

          draw->PushForeground(halfShine);
          draw->DrawLine(x+w-1,y+1,x+w-1,y+h-1);
          draw->DrawLine(x+w-1,y+h-1,x,y+h-1);
          draw->PopForeground();

          draw->PushForeground(shine);
          draw->DrawLine(x+w-2,y+2,x+w-2,y+h-2);
          draw->DrawLine(x+w-2,y+h-2,x+1,y+h-2);
          draw->PopForeground();
        }
      }

      Theme::Theme(OS::Display *display)
      : OS::Theme(display),
        display(display)
      {
        emptyFrame=new OS::EmptyFrame();

        if (display->GetColorMode()==Display::colorModeMonochrome) {
          // TODO
        }
        else if (display->GetColorMode()==Display::colorModeGreyScale) {
          display->AllocateNamedColor("grey95",fillColor[0]);
          display->AllocateNamedColor("grey82",fillColor[1]);
          display->AllocateNamedColor("grey45",fillColor[2]);
        }
        else {
          display->AllocateNamedColor("red",fillColor[0]);
          display->AllocateNamedColor("green",fillColor[1]);
          display->AllocateNamedColor("blue",fillColor[2]);
        }
      }

      Theme::~Theme()
      {
        if (display->GetColorMode()==Display::colorModeMonochrome) {
          // TODO
        }
        else if (display->GetColorMode()==Display::colorModeGreyScale) {
          display->FreeColor(fillColor[0]);
          display->FreeColor(fillColor[1]);
          display->FreeColor(fillColor[2]);
        }
        else {
          display->FreeColor(fillColor[0]);
          display->FreeColor(fillColor[1]);
          display->FreeColor(fillColor[2]);
        }
      }

      Color Theme::GetColor(Display::ColorIndex color) const
      {
        return this->color[color];
      }

      Fill* Theme::GetFill(Display::FillIndex fill) const
      {
        return this->fill[fill].Get();
      }

      Frame* Theme::GetFrame(Display::FrameIndex frame) const
      {
        return this->frame[frame].Get();
      }

      Image* Theme::GetImage(Display::ImageIndex image) const
      {
        return this->image[image].Get();
      }

      Image* Theme::GetStockImage(StockImage image) const
      {
        return stockImage[image].Get();
      }

      void Theme::PushUniqueFill(OS::DrawInfo* draw, size_t index, size_t maximum)
      {
        if (index==maximum && index%3==2) {
          index=1;
        }
        else {
          index=index%3;
        }

        draw->PushForeground(fillColor[index]);
      }

      void Theme::PopUniqueFill(OS::DrawInfo* draw, size_t index, size_t maximum)
      {
        draw->PopForeground();
      }

      bool Theme::IsLightTheme() const
      {
        return true;
      }

      size_t Theme::GetMinimalTableRowHeight() const
      {
        return 0;
      }

      size_t Theme::GetHorizontalFuelBarMinWidth() const
      {
        return 0;
      }

      size_t Theme::GetHorizontalFuelBarMinHeight() const
      {
        return 0;
      }

      size_t Theme::GetVerticalFuelBarMinWidth() const
      {
        return 0;
      }

      size_t Theme::GetVerticalFuelBarMinHeight() const
      {
        return 0;
      }

      size_t Theme::GetCursorWidth(OS::Font* font) const
      {
        size_t result;

        result=(size_t)std::ceil(font->pixelHeight*0.04);

        if (result==0) {
          result=1;
        }

        return result;
      }

      int Theme::GetSubMenuHorizontalOffset() const
      {
        return 0;
      }

      bool Theme::OptimizeMenus() const
      {
        return false;
      }

      size_t Theme::MaxPopupMenuEntries() const
      {
        return 10;
      }

      bool Theme::RequestFingerFriendlyControls() const
      {
        return false;
      }

      Lum::Base::Size Theme::GetFingerFriendlyMinWidth() const
      {
        return Lum::Base::Size(Lum::Base::Size::mm,9);
      }

      Lum::Base::Size Theme::GetFingerFriendlyMinHeight() const
      {
        return Lum::Base::Size(Lum::Base::Size::mm,9);
      }

      bool Theme::ShowKeyShortcuts() const
      {
        return true;
      }

      bool Theme::FocusOnEditableControlsOnly() const
      {
        return false;
      }

      unsigned long Theme::GetCursorBlinkTimeMicroSeconds() const
      {
        return 600000;
      }

      bool Theme::SubdialogsAlwaysFlexHoriz() const
      {
        return false;
      }

      bool Theme::SubdialogsAlwaysFlexVert() const
      {
        return false;
      }

      bool Theme::PopupsAreDialogs() const
      {
        return false;
      }

      Theme::DialogActionPosition Theme::GetDialogActionPosition() const
      {
        return actionPosHorizButtonRowBelow;
      }

      bool Theme::ImplicitCancelAction() const
      {
        return false;
      }

      bool Theme::ImplicitDefaultAction() const
      {
        return false;
      }

      /**
        Class that creates internal default versions of theming images.
      */
      class InternalImage : public OS::Image
      {
      public:
        enum Type {
          radio,
          checkbox,
          hSliderKnob,
          vSliderKnob,
          sliderUnfilled,
          sliderFilled,
          knob,
          arrowUp,
          arrowDown,
          arrowLeft,
          arrowRight,
          scrollArrowUp,
          scrollArrowDown,
          scrollArrowLeft,
          scrollArrowRight,
          hLine,
          vLine,
          combo,
          comboButton,
          led,
          treeCross,
          close
        };

      private:
        size_t        width;
        size_t        height;
        bool          alpha;
        unsigned long drawCap;
        Type          type;

      private:
        void DrawRectangle(OS::DrawInfo* draw, int x, int y, size_t w, size_t h, bool in);

      public:
        InternalImage(Type type);

        size_t GetWidth() const;
        size_t GetHeight() const;
        bool GetAlpha() const;
        unsigned long GetDrawCap() const;

        void Draw(OS::DrawInfo* draw, int x, int y);
        void DrawScaled(OS::DrawInfo* draw, int x, int y, size_t w, size_t h);
        void DrawStretched(OS::DrawInfo* draw, int x, int y, size_t w, size_t h);
      };

      /**
        A class that implements a number of frames. Use the frames
        implemented here everywhere where possible. Do @emph{not} implement
        your own frames, add them here. Add your frames also here, when you
        want to implement another look.
      */
      class LUMAPI InternalFrame : public OS::Frame
      {
      public:
        enum Type {
          single3DIn,
          single3DOut,
          singleFrame, //! Simple black frame of 1 pixel size
          double3DOut,
          group3D,
          focus,       //! A dotted-line focus frame
          box,         //! box frame
          tab,         //! "Normal" frame for complete tab
        };

      public:
        Type type;

      private:
        void SetFrame(Type type);

      public:
        InternalFrame(Type type);

        virtual void SetGap(size_t x, size_t width, size_t height);
        virtual void Draw(OS::DrawInfo* draw, int x, int y, size_t w, size_t h);
      };

      InternalImage::InternalImage(Type type)
      : width(0),height(0),alpha(false),drawCap(0),type(type)
      {
        int            unit;

        this->type=type;

        /* Setting flags depending of image type */

        switch (type) {
        case led:         /* Currently the LED image does not draw differently when disabled.
                             This is just a hack to ease use of VO:State */
        case checkbox:
        case radio:
        case arrowUp:
        case arrowDown:
        case arrowLeft:
        case arrowRight:
        case scrollArrowUp:
        case scrollArrowDown:
        case scrollArrowLeft:
        case scrollArrowRight:
          drawCap|=drawsDisabled;
          break;
        case hSliderKnob:
        case vSliderKnob:
        default:
          break;
        }

        switch (type) {
        case radio:
        case knob:
        case arrowUp:
        case arrowDown:
        case arrowLeft:
        case arrowRight:
        case combo:
        case led:
        case treeCross:
        case close:
          alpha=true;
          break;
        default:
          alpha=false;
        }

        unit=(DEFAULT_PROP_FONT_SIZE/2>0) ? DEFAULT_PROP_FONT_SIZE/2 : 1;

        switch (type) {
        case checkbox:
          width=2*unit;
          height=2*unit;
          break;
        case radio:
          width=Lum::Base::RoundUpEven(7*unit / 4);
          height=Lum::Base::RoundUpEven(7*unit / 4);
          break;
        case arrowLeft:
        case arrowRight:
        case arrowUp:
        case arrowDown:
          width=3*unit/2;
          height=3*unit/2;
          break;
        case scrollArrowLeft:
        case scrollArrowRight:
        case scrollArrowUp:
        case scrollArrowDown:
          width=3*unit/2+4;
          height=3*unit/2+4;
          break;
        case knob:
          width=2*unit;
          height=2*unit;
          break;
        case hSliderKnob:
          width=Lum::Base::RoundUpEven(5*unit);
          height=std::max(4,2*unit);
          break;
        case vSliderKnob:
          width=std::max(4,2*unit);
          height=Lum::Base::RoundUpEven(5*unit);
          break;
        case sliderUnfilled:
        case sliderFilled:
          width=1;
          height=1;
          break;
        case hLine:
          width=4;
          height=2;
          break;
        case vLine:
          width=2;
          height=4;
          break;
        case combo:
          width=unit;
          height=unit;
          break;
        case led:
          width=Lum::Base::RoundUpEven(7*unit / 4);
          height=Lum::Base::RoundUpEven(7*unit / 4);
          break;
        case comboButton:
          width=2*unit;
          height=2*unit;
          break;
        case treeCross:
          width=unit;
          height=unit;
          break;
        case close:
          width=unit;
          height=unit;
          break;
        }
      }

      size_t InternalImage::GetWidth() const
      {
        return width;
      }

      size_t InternalImage::GetHeight() const
      {
        return height;
      }

      bool InternalImage::GetAlpha() const
      {
        return alpha;
      }

      unsigned long InternalImage::GetDrawCap() const
      {
        return drawCap;
      }

      void InternalImage::DrawRectangle(OS::DrawInfo* draw, int x, int y, size_t w, size_t h, bool in)
      {
        if (in) {
          draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->shadowColor);
        }
        else {
          draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->shineColor);
        }

        draw->DrawLine(x,y+h-1,x,y);
        draw->DrawLine(x+1,y,x+w-1,y);
        draw->PopForeground();

        if (in) {
          draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->shineColor);
        }
        else {
          draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->shadowColor);
        }

        draw->DrawLine(x+w-1,y+1,x+w-1,y+h-1);
        draw->DrawLine(x+w-2,y+h-1,x+1,y+h-1);
        draw->PopForeground();
      }

      void InternalImage::Draw(OS::DrawInfo* draw, int x, int y)
      {
        DrawScaled(draw,x,y,GetWidth(),GetHeight());
      }

      void InternalImage::DrawStretched(OS::DrawInfo* draw,
                                        int x, int y,
                                        size_t width, size_t height)
      {
        DrawScaled(draw,x,y,width,height);
      }

      void InternalImage::DrawScaled(OS::DrawInfo* draw,
                                     int x, int y,
                                     size_t width, size_t height)
      {
        OS::DrawInfo::Point points[3];
        OS::FontRef         font=OS::display->GetFont();
        Color               shine,shadow,halfShadow;

        shine=dynamic_cast<DefaultTheme*>(display->GetTheme())->shineColor;
        shadow=dynamic_cast<DefaultTheme*>(display->GetTheme())->shadowColor;
        halfShadow=dynamic_cast<DefaultTheme*>(display->GetTheme())->halfShadowColor;

        switch (type) {
        case hSliderKnob:
        case vSliderKnob:
          DrawRectangle(draw,x,y,width,height,false);
          DrawRectangle(draw,x+1,y+1,width-2,height-2,false);

          draw->PushForeground(OS::Display::backgroundColor);
          draw->FillRectangle(x+2,y+2,width-4,height-4);
          draw->PopForeground();

          if (type==hSliderKnob) {
            draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->shadowColor);
            draw->DrawLine(x+(width-1) / 2,y+2,x+(width-1) / 2,y+height-3);
            draw->PopForeground();
            draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->shineColor);
            draw->DrawLine(x+(width-1) / 2+1,y+2,x+(width-1) / 2+1,y+height-3);
            draw->PopForeground();
          }
          else {
            draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->shadowColor);
            draw->DrawLine(x+2,y+height / 2,x+width-3,y+height / 2);
            draw->PopForeground();
            draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->shineColor);
            draw->DrawLine(x+2,y+height / 2+1,x+width-3,y+height / 2+1);
            draw->PopForeground();
          }
          break;

        case sliderUnfilled:
          draw->PushForeground(OS::Display::backgroundColor);
          draw->FillRectangle(x,y,width,height);
          draw->PopForeground();
          break;

        case sliderFilled:
          draw->PushForeground(OS::Display::fillColor);
          draw->FillRectangle(x,y,width,height);
          draw->PopForeground();
          break;

        case knob:
          draw->PushForeground(OS::Display::backgroundColor); /* TODO: */
          draw->DrawLine(x,  y+height-1,x,          y+1);
          draw->DrawLine(x,  y,           x+width-2,y);
          draw->PopForeground();

          draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->shineColor);
          draw->DrawLine(x+1,y+height-2,x+1,        y+2);
          draw->DrawLine(x+1,y+1,         x+width-3,y+1);
          draw->PopForeground();

          draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->shadowColor);
          draw->DrawLine(x+width-1,y,           x+width-1,y+height-1);
          draw->DrawLine(x+width-1,y+height-1,x,          y+height-1);
          draw->PopForeground();

          draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->halfShadowColor);
          draw->DrawLine(x+width-2,y+1,         x+width-2,y+height-2);
          draw->DrawLine(x+width-2,y+height-2,x+1,        y+height-2);
          draw->PopForeground();
          break;

        case arrowLeft:
          if (draw->disabled) {
            draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->halfShadowColor);
          }
          else {
            draw->PushForeground(OS::Display::blackColor);
          }
          points[0].x=x+width/5;
          points[0].y=y+height / 2;
          points[1].x=x+width-1-width/5;
          points[1].y=y+height/5;
          points[2].x=x+width-1-width/5;
          points[2].y=y+height-1-height/5;
          draw->FillPolygon(points,3);
          draw->PopForeground();
          break;

        case arrowRight:
          if (draw->disabled) {
            draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->halfShadowColor);
          }
          else {
            draw->PushForeground(OS::Display::blackColor);
          }
          points[0].x=x+width-1-width/5;
          points[0].y=y+height / 2;
          points[1].x=x+width/5;
          points[1].y=y+height/5;
          points[2].x=x+width/5;
          points[2].y=y+height-1-height/5;
          draw->FillPolygon(points,3);
          draw->PopForeground();
          break;

        case arrowUp:
          if (draw->disabled) {
            draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->halfShadowColor);
          }
          else {
            draw->PushForeground(OS::Display::blackColor);
          }
          points[0].x=x+width / 2;
          points[0].y=y+height/5;
          points[1].x=x+width/5;
          points[1].y=y+height-1-height/5;
          points[2].x=x+width-1-width/5;
          points[2].y=y+height-1-height/5;
          draw->FillPolygon(points,3);
          draw->PopForeground();
          break;

        case arrowDown:
          if (draw->disabled) {
            draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->halfShadowColor);
          }
          else {
            draw->PushForeground(OS::Display::blackColor);
          }
          points[0].x=x+width/5;
          points[0].y=y+height/5;
          points[1].x=x+width-1-width/5;
          points[1].y=y+height/5;
          points[2].x=x+width / 2;
          points[2].y=y+height-1-height/5;
          draw->FillPolygon(points,3);
          draw->PopForeground();
          break;

        case scrollArrowLeft:
          draw->PushForeground(OS::Display::backgroundColor);
          draw->FillRectangle(x+2,y+2,width-4,height-4);
          draw->PopForeground();

          if (draw->selected) {
            draw->PushForeground(OS::Display::blackColor);
            draw->DrawRectangle(x,y,width,height);
            draw->PopForeground();
            draw->PushForeground(halfShadow);
            draw->DrawRectangle(x+1,y+1,width-2,height-2);
            draw->PopForeground();
          }
          else {
            draw->PushForeground(shine);
            draw->DrawLine(x,y+height-1,x,y+1);
            draw->DrawLine(x,y,x+width-2,y);
            draw->PopForeground();

            draw->PushForeground(OS::Display::backgroundColor);
            draw->DrawLine(x+1,y+height-2,x+1,y+2);
            draw->DrawLine(x+1,y+1,x+width-3,y+1);
            draw->PopForeground();

            draw->PushForeground(shadow);
            draw->DrawLine(x+width-1,y,x+width-1,y+height-1);
            draw->DrawLine(x+width-1,y+height-1,x,y+height-1);
            draw->PopForeground();

            draw->PushForeground(halfShadow);
            draw->DrawLine(x+width-2,y+1,x+width-2,y+height-2);
            draw->DrawLine(x+width-2,y+height-2,x+1,y+height-2);
            draw->PopForeground();
          }

          if (draw->disabled) {
            draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->halfShadowColor);
          }
          else {
            draw->PushForeground(OS::Display::blackColor);
          }
          points[0].x=x+width/5;
          points[0].y=y+height / 2;
          points[1].x=x+width-1-width/5;
          points[1].y=y+height/5;
          points[2].x=x+width-1-width/5;
          points[2].y=y+height-1-height/5;
          draw->FillPolygon(points,3);
          draw->PopForeground();
          break;

        case scrollArrowRight:
          draw->PushForeground(OS::Display::backgroundColor);
          draw->FillRectangle(x+2,y+2,width-4,height-4);
          draw->PopForeground();

          if (draw->selected) {
            draw->PushForeground(OS::Display::blackColor);
            draw->DrawRectangle(x,y,width,height);
            draw->PopForeground();
            draw->PushForeground(halfShadow);
            draw->DrawRectangle(x+1,y+1,width-2,height-2);
            draw->PopForeground();
          }
          else {
            draw->PushForeground(shine);
            draw->DrawLine(x,y+height-1,x,y+1);
            draw->DrawLine(x,y,x+width-2,y);
            draw->PopForeground();

            draw->PushForeground(OS::Display::backgroundColor);
            draw->DrawLine(x+1,y+height-2,x+1,y+2);
            draw->DrawLine(x+1,y+1,x+width-3,y+1);
            draw->PopForeground();

            draw->PushForeground(shadow);
            draw->DrawLine(x+width-1,y,x+width-1,y+height-1);
            draw->DrawLine(x+width-1,y+height-1,x,y+height-1);
            draw->PopForeground();

            draw->PushForeground(halfShadow);
            draw->DrawLine(x+width-2,y+1,x+width-2,y+height-2);
            draw->DrawLine(x+width-2,y+height-2,x+1,y+height-2);
            draw->PopForeground();
          }

          if (draw->disabled) {
            draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->halfShadowColor);
          }
          else {
            draw->PushForeground(OS::Display::blackColor);
          }
          points[0].x=x+width-1-width/5;
          points[0].y=y+height / 2;
          points[1].x=x+width/5;
          points[1].y=y+height/5;
          points[2].x=x+width/5;
          points[2].y=y+height-1-height/5;
          draw->FillPolygon(points,3);
          draw->PopForeground();
          break;

        case scrollArrowUp:
          draw->PushForeground(OS::Display::backgroundColor);
          draw->FillRectangle(x+2,y+2,width-4,height-4);
          draw->PopForeground();

          if (draw->selected) {
            draw->PushForeground(OS::Display::blackColor);
            draw->DrawRectangle(x,y,width,height);
            draw->PopForeground();
            draw->PushForeground(halfShadow);
            draw->DrawRectangle(x+1,y+1,width-2,height-2);
            draw->PopForeground();
          }
          else {
            draw->PushForeground(shine);
            draw->DrawLine(x,y+height-1,x,y+1);
            draw->DrawLine(x,y,x+width-2,y);
            draw->PopForeground();

            draw->PushForeground(OS::Display::backgroundColor);
            draw->DrawLine(x+1,y+height-2,x+1,y+2);
            draw->DrawLine(x+1,y+1,x+width-3,y+1);
            draw->PopForeground();

            draw->PushForeground(shadow);
            draw->DrawLine(x+width-1,y,x+width-1,y+height-1);
            draw->DrawLine(x+width-1,y+height-1,x,y+height-1);
            draw->PopForeground();

            draw->PushForeground(halfShadow);
            draw->DrawLine(x+width-2,y+1,x+width-2,y+height-2);
            draw->DrawLine(x+width-2,y+height-2,x+1,y+height-2);
            draw->PopForeground();
          }

          if (draw->disabled) {
            draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->halfShadowColor);
          }
          else {
            draw->PushForeground(OS::Display::blackColor);
          }
          points[0].x=x+width / 2;
          points[0].y=y+height/5;
          points[1].x=x+width/5;
          points[1].y=y+height-1-height/5;
          points[2].x=x+width-1-width/5;
          points[2].y=y+height-1-height/5;
          draw->FillPolygon(points,3);
          draw->PopForeground();
          break;

        case scrollArrowDown:
          draw->PushForeground(OS::Display::backgroundColor);
          draw->FillRectangle(x+2,y+2,width-4,height-4);
          draw->PopForeground();

          if (draw->selected) {
            draw->PushForeground(OS::Display::blackColor);
            draw->DrawRectangle(x,y,width,height);
            draw->PopForeground();
            draw->PushForeground(halfShadow);
            draw->DrawRectangle(x+1,y+1,width-2,height-2);
            draw->PopForeground();
          }
          else {
            draw->PushForeground(shine);
            draw->DrawLine(x,y+height-1,x,y+1);
            draw->DrawLine(x,y,x+width-2,y);
            draw->PopForeground();

            draw->PushForeground(OS::Display::backgroundColor);
            draw->DrawLine(x+1,y+height-2,x+1,y+2);
            draw->DrawLine(x+1,y+1,x+width-3,y+1);
            draw->PopForeground();

            draw->PushForeground(shadow);
            draw->DrawLine(x+width-1,y,x+width-1,y+height-1);
            draw->DrawLine(x+width-1,y+height-1,x,y+height-1);
            draw->PopForeground();

            draw->PushForeground(halfShadow);
            draw->DrawLine(x+width-2,y+1,x+width-2,y+height-2);
            draw->DrawLine(x+width-2,y+height-2,x+1,y+height-2);
            draw->PopForeground();
          }

          if (draw->disabled) {
            draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->halfShadowColor);
          }
          else {
            draw->PushForeground(OS::Display::blackColor);
          }
          points[0].x=x+width/5;
          points[0].y=y+height/5;
          points[1].x=x+width-1-width/5;
          points[1].y=y+height/5;
          points[2].x=x+width / 2;
          points[2].y=y+height-1-height/5;
          draw->FillPolygon(points,3);
          draw->PopForeground();
          break;
        case hLine:
          DrawRectangle(draw,x,y+(height-2) / 2,width,2,true);
          break;

        case vLine:
          DrawRectangle(draw,x+(width-2) / 2,y,2,height,true);
          break;

        case combo:
          if (draw->disabled) {
            draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->halfShineColor);
          }
          else {
            draw->PushForeground(OS::Display::blackColor);
          }

          points[0].x=x+width/4;
          points[0].y=y+height/4;
          points[1].x=x+width-1-width/4;
          points[1].y=y+height/4;
          points[2].x=x+width / 2;
          points[2].y=y+height-1-height/4;
          draw->FillPolygon(points,3);
          draw->PopForeground();
          break;

        case comboButton:
          // Background

          draw->PushForeground(OS::Display::backgroundColor);
          draw->FillRectangle(x+2,y+2,width-4,height-4);
          draw->PopForeground();

          // Frame

          draw->PushForeground(OS::Display::backgroundColor); /* TODO: */
          draw->DrawLine(x,  y+height-1,x,          y+1);
          draw->DrawLine(x,  y,           x+width-2,y);
          draw->PopForeground();

          draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->shineColor);
          draw->DrawLine(x+1,y+height-2,x+1,        y+2);
          draw->DrawLine(x+1,y+1,         x+width-3,y+1);
          draw->PopForeground();

          draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->shadowColor);
          draw->DrawLine(x+width-1,y,           x+width-1,y+height-1);
          draw->DrawLine(x+width-1,y+height-1,x,          y+height-1);
          draw->PopForeground();

          draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->halfShadowColor);
          draw->DrawLine(x+width-2,y+1,         x+width-2,y+height-2);
          draw->DrawLine(x+width-2,y+height-2,x+1,        y+height-2);
          draw->PopForeground();

          // Image

          if (draw->disabled) {
            draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->halfShineColor);
          }
          else {
            draw->PushForeground(OS::Display::blackColor);
          }

          points[0].x=x+width/4;
          points[0].y=y+height/4;
          points[1].x=x+width-1-width/4;
          points[1].y=y+height/4;
          points[2].x=x+width / 2;
          points[2].y=y+height-1-height/4;
          draw->FillPolygon(points,3);
          draw->PopForeground();
          break;

        case treeCross:
          draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->shadowColor);
          if (draw->selected) {
            // arrowDown
            points[0].x=x+width / 4;
            points[0].y=y+height / 4;
            points[1].x=x+width-width / 4;
            points[1].y=y+height / 4;
            points[2].x=x+width / 2;
            points[2].y=y+height-height / 4;
            draw->FillPolygon(points,3);
          }
          else {
            // arrowRight
            points[0].x=x+width-width / 4;
            points[0].y=y+height / 2;
            points[1].x=x+width / 4;
            points[1].y=y+height / 4;
            points[2].x=x+width / 4;
            points[2].y=y+height-height / 4;
            draw->FillPolygon(points,3);
          }
          draw->PopForeground();
          break;

        case close:
          draw->PushForeground(OS::Display::backgroundColor);
          draw->FillRectangle(x,y,width,height);
          draw->PopForeground();

          draw->PushForeground(OS::Display::blackColor);
          draw->DrawLine(x,y,x+width-1,y+height-1);
          draw->DrawLine(x+width-1,y,x,y+height-1);
          draw->PopForeground();
          break;
        case checkbox:
          if (draw->disabled) {
            draw->PushForeground(OS::Display::backgroundColor);
          }
          else {
            draw->PushForeground(OS::Display::whiteColor);
          }
          draw->FillRectangle(x,y,width,height);
          draw->PopForeground();

          draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->halfShadowColor);
          draw->DrawLine(x,y+height-1,x,y+1);
          draw->DrawLine(x,y,x+width-1,y);
          draw->PopForeground();

          draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->shadowColor);
          draw->DrawLine(x+1,y+height-2,x+1,y+2);
          draw->DrawLine(x+1,y+1,x+width-2,y+1);
          draw->PopForeground();

          draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->shineColor);
          draw->DrawLine(x+width-1,y+1,x+width-1,y+height-1);
          draw->DrawLine(x+width-1,y+height-1,x,y+height-1);
          draw->PopForeground();

          draw->PushForeground(OS::Display::backgroundColor); /* TODO: */
          draw->DrawLine(x+width-2,y+2,x+width-2,y+height-2);
          draw->DrawLine(x+width-2,y+height-2,x+1,y+height-2);
          draw->PopForeground();

          if (draw->selected) {
            if (draw->disabled) {
              draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->halfShadowColor);
            }
            else {
              draw->PushForeground(OS::Display::blackColor);
            }
            draw->PushPen(2,OS::DrawInfo::penNormal);
            draw->DrawLine(x+3,y+3,x+width-4,y+height-4);
            draw->DrawLine(x+width-4,y+3,x+3,y+height-4);
            draw->PopPen();
            draw->PopForeground();
          }
          break;
        case radio:
          if (draw->disabled) {
            draw->PushForeground(OS::Display::backgroundColor); /* TODO: */
          }
          else {
            draw->PushForeground(OS::Display::whiteColor);
          }
          draw->FillArc(x,y,width,height,0,360*64);
          draw->PopForeground();

          draw->PushPen(2,OS::DrawInfo::penNormal);
          draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->halfShadowColor);
          draw->DrawArc(x,y,width,height,45*64,180*64);
          draw->PopForeground();
          draw->PopPen();

          draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->shadowColor);
          draw->DrawArc(x+1,y+1,width-2,height-2,45*64,180*64);
          draw->PopForeground();

          draw->PushPen(2,OS::DrawInfo::penNormal);
          draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->shineColor);
          draw->DrawArc(x,y,width,height,225*64,180*64);
          draw->PopForeground();
          draw->PopPen();

          draw->PushForeground(OS::Display::backgroundColor); /* TODO: */
          draw->DrawArc(x+1,y+1,width-2,height-2,225*64,180*64);
          draw->PopForeground();

          if (draw->selected) {
            if (draw->disabled) {
              draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->halfShadowColor);
            }
            else {
              draw->PushForeground(OS::Display::blackColor);
            }
            draw->FillArc(x+4,y+4,width-8,height-8,0,360*64);
            draw->PopForeground();
          }
          break;
        case led:
          if (draw->selected) {
            if (OS::display->GetColorMode()==OS::Display::colorModeMonochrome) {
              draw->PushForeground(OS::Display::whiteColor);
            }
            else {
              draw->PushForeground(OS::Display::fillColor);
            }
            draw->FillArc(x,y,width-1,height-1,0*64,360*64);
            draw->PopForeground();
          }
          else {
            draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->shadowColor);
            draw->FillArc(x,y,width-1,height-1,0*64,360*64);
            draw->PopForeground();
          }

          draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->shineColor);
          draw->DrawArc(x+width / 4,y+height / 4,
                        width / 2,height / 2,60*64,90*64);
          draw->PopForeground();

          draw->PushForeground(dynamic_cast<DefaultTheme*>(display->GetTheme())->shadowColor);
          draw->DrawArc(x,y,width-1,height-1,0*64,360*64);
          draw->PopForeground();
          break;
        }
      }

      InternalFrame::InternalFrame(Type type)
      {
        SetFrame(type);
      }

      /**
        Set the frame to be used.

        NOTE
        The frame can be changed at runtime.
      */
      void InternalFrame::SetFrame(Type type)
      {
        this->type=type;
        alpha=false;

        switch (type) {
        case single3DIn:
        case single3DOut:
        case singleFrame:
          topBorder=1;
          bottomBorder=1;
          leftBorder=1;
          rightBorder=1;
          break;
        case double3DOut:
        case group3D:
          if (HasGap()) {
            topBorder=std::max((size_t)2,gh);
          }
          else {
            topBorder=2;
          }
          bottomBorder=2;
          leftBorder=2;
          rightBorder=2;
          break;
        case focus:
          topBorder=1;
          bottomBorder=1;
          leftBorder=1;
          rightBorder=1;
          break;
        case box:
          topBorder=2;
          bottomBorder=2;
          leftBorder=2;
          rightBorder=2;
          break;
        case tab:
          topBorder=2;
          bottomBorder=2;
          leftBorder=2;
          rightBorder=2;
          break;
        }

        minWidth=leftBorder+rightBorder;
        minHeight=topBorder+bottomBorder;

        if (HasGap()) {
          alpha=true;
        }
      }

      void InternalFrame::SetGap(size_t x, size_t width, size_t height)
      {
        Frame::SetGap(x,width,height);
        SetFrame(type);
      }

      void InternalFrame::Draw(OS::DrawInfo* draw, int x, int y, size_t w, size_t h)
      {
        Color shine,shadow,halfShadow;
        int   top,type;

        shine=dynamic_cast<DefaultTheme*>(display->GetTheme())->shineColor;
        shadow=dynamic_cast<DefaultTheme*>(display->GetTheme())->shadowColor;
        halfShadow=dynamic_cast<DefaultTheme*>(display->GetTheme())->halfShadowColor;

        type=this->type;

        if (draw->selected) {
          switch (type) {
          case single3DOut:
            type=single3DIn;
            break;
          default:
            break;
          }
        }

        switch (type) {
        case double3DOut:
          draw->PushForeground(shine);
          draw->DrawLine(x,y+h-1,x,y+1);
          draw->DrawLine(x,y,x+w-1,y);
          draw->DrawLine(x+1,y+h-2,x+1,y+2);
          draw->DrawLine(x+1,y+1,x+w-2,y+1);
          draw->PopForeground();

          draw->PushForeground(shadow);
          draw->DrawLine(x+w-1,y+1,x+w-1,y+h-1);
          draw->DrawLine(x+w-1,y+h-1,x,y+h-1);
          draw->DrawLine(x+w-2,y+2,x+w-2,y+h-2);
          draw->DrawLine(x+w-2,y+h-2,x+1,y+h-2);
          draw->PopForeground();
          break;

        case single3DIn:
        case single3DOut:
          if (type==single3DIn) {
            draw->PushForeground(shadow);
          }
          else {
            draw->PushForeground(shine);
          }
          draw->DrawLine(x,y+h-1,x,y+1);
          draw->DrawLine(x,y,x+w-1,y);
          draw->PopForeground();

          if (type==single3DIn) {
            draw->PushForeground(shine);
          }
          else {
            draw->PushForeground(shadow);
          }
          draw->DrawLine(x+w-1,y+1,x+w-1,y+h-1);
          draw->DrawLine(x+w-1,y+h-1,x,y+h-1);
          draw->PopForeground();
          break;

        case singleFrame:
          draw->PushForeground(shadow);
          draw->DrawRectangle(x,y,w,h);
          draw->PopForeground();
          break;

        case group3D:
          if (HasGap()) {
            top=y+gh / 2;
          }
          else {
            top=y;
          }
          draw->PushForeground(shadow);
          draw->DrawLine(x,y+h-1,x,top);
          draw->DrawLine(x+w-2,top+1,x+w-2,y+h-2);
          draw->DrawLine(x+1,y+h-2,x+w-2,y+h-2);
          if (HasGap()) {
            draw->DrawLine(x,top,x+OS::display->GetSpaceHorizontal(OS::Display::spaceLabelObject)-1,top);
            draw->DrawLine(x+3*OS::display->GetSpaceHorizontal(OS::Display::spaceLabelObject)+gw+1,top,x+w-1,top);
            draw->DrawLine(x+3*OS::display->GetSpaceHorizontal(OS::Display::spaceLabelObject)+gw,top,
                           x+3*OS::display->GetSpaceHorizontal(OS::Display::spaceLabelObject)+gw,top+1);
          }
          else {
            draw->DrawLine(x,top,x+w-1,top);
          }
          draw->PopForeground();

          draw->PushForeground(shine);
          draw->DrawLine(x+1,y+h-2,x+1,top+1);
          draw->DrawLine(x+w-1,top+1,x+w-1,y+h-1);
          draw->DrawLine(x+1,y+h-1,x+w-2,y+h-1);
          if (HasGap()) {
            draw->DrawLine(x+2,top+1,x+OS::display->GetSpaceHorizontal(OS::Display::spaceLabelObject)-1,top+1);
            draw->DrawLine(x+3*OS::display->GetSpaceHorizontal(OS::Display::spaceLabelObject)+gw+1,top+1,x+w-2,top+1);
            draw->DrawLine(x+OS::display->GetSpaceHorizontal(OS::Display::spaceLabelObject),top,x+OS::display->GetSpaceHorizontal(OS::Display::spaceLabelObject),top+1);
          }
          else {
            draw->DrawLine(x+2,top+1,x+w-2,top+1);
          }
          draw->PopForeground();
          break;

        case focus:
          draw->PushForeground(OS::Display::textColor);
          draw->PushDash("\002\002",2,OS::DrawInfo::fDash);
          draw->DrawLine(x          ,y,      x+w-1,y);
          draw->DrawLine(x+w-1,y,            x+w-1,y+h-1);
          draw->DrawLine(x+w-1,y+h-1,x,            y+h-1);
          draw->DrawLine(x          ,y+h-1,x,      y);
          draw->PopDash();
          draw->PopForeground();
          break;

        case box:
          draw->PushForeground(halfShadow);
          draw->DrawLine(x,y+h-1,x,y+1);
          draw->DrawLine(x,y,x+w-2,y);
          draw->PopForeground();

          draw->PushForeground(shadow);
          draw->DrawLine(x+1,y+h-2,x+1,y+2);
          draw->DrawLine(x+1,y+1,x+w-3,y+1);
          draw->PopForeground();

          draw->PushForeground(shine);
          draw->DrawLine(x+w-1,y,x+w-1,y+h-1);
          draw->DrawLine(x+w-1,y+h-1,x,y+h-1);
          draw->PopForeground();

          draw->PushForeground(OS::Display::backgroundColor);
          draw->DrawLine(x+w-2,y+1,x+w-2,y+h-2);
          draw->DrawLine(x+w-2,y+h-2,x+1,y+h-2);
          draw->PopForeground();
          break;

        case tab:
          draw->PushForeground(shadow);

          /* right line */
          draw->DrawLine(x+w-2,y,x+w-2,y+h-1-2);
          draw->DrawLine(x+w-1,y,x+w-1,y+h-1-2);

          /* bottom line */
          draw->DrawLine(x+1,y+h-2,x+w-1,y+h-2);
          draw->DrawLine(x,  y+h-1,x+w-1,y+h-1);

          draw->PopForeground();

          /* Gap */
          draw->PushForeground(OS::Display::backgroundColor);
          draw->DrawLine(x+gx+1,y,  x+gx+gw-3,y);
          draw->DrawLine(x+gx  ,y+1,x+gx+gw-2,y+1);
          draw->PopForeground();

          draw->PushForeground(shine);

          /* top line */

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
          break;

        }
      }

      DefaultTheme::DefaultTheme(OS::Display *display)
      : Theme(display)
      {
        const char* fillNames[Display::fillCount];
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
          colorNames[Display::textSmartAColor]="white";
          colorNames[Display::textSmartBColor]="black";

          display->AllocateNamedColor("black",shineColor);
          display->AllocateNamedColor("black",halfShineColor);
          display->AllocateNamedColor("black",halfShadowColor);
          display->AllocateNamedColor("black",shadowColor);
        }
        else if (display->GetColorMode()==Display::colorModeGreyScale) {
          colorNames[Display::backgroundColor]="grey70";
          colorNames[Display::tableTextColor]="black";
          colorNames[Display::textColor]="black";
          colorNames[Display::textSelectColor]="black";
          colorNames[Display::textDisabledColor]="grey45";
          colorNames[Display::fillColor]="grey60";
          colorNames[Display::fillTextColor]="black";
          colorNames[Display::graphScaleColor]="grey45";
          colorNames[Display::blackColor]="black";
          colorNames[Display::whiteColor]="white";
          colorNames[Display::tabTextColor]="black";
          colorNames[Display::tabTextSelectColor]="black";
          colorNames[Display::editTextColor]="black";
          colorNames[Display::editTextFillColor]="white";
          colorNames[Display::textSmartAColor]="black";
          colorNames[Display::textSmartBColor]="white";

          display->AllocateNamedColor("grey95",shineColor);
          display->AllocateNamedColor("grey82",halfShineColor);
          display->AllocateNamedColor("grey45",halfShadowColor);
          display->AllocateNamedColor("grey20",shadowColor);
        }
        else {
          colorNames[Display::backgroundColor]="grey70";
          colorNames[Display::tableTextColor]="black";
          colorNames[Display::textColor]="black";
          colorNames[Display::textSelectColor]="black";
          colorNames[Display::textDisabledColor]="grey45";
          colorNames[Display::fillColor]="royal blue";
          colorNames[Display::fillTextColor]="white";
          colorNames[Display::graphScaleColor]="grey45";
          colorNames[Display::blackColor]="black";
          colorNames[Display::whiteColor]="white";
          colorNames[Display::tabTextColor]="black";
          colorNames[Display::tabTextSelectColor]="black";
          colorNames[Display::editTextColor]="black";
          colorNames[Display::editTextFillColor]="white";
          colorNames[Display::textSmartAColor]="black";
          colorNames[Display::textSmartBColor]="white";

          display->AllocateNamedColor("grey95",shineColor);
          display->AllocateNamedColor("grey82",halfShineColor);
          display->AllocateNamedColor("grey45",halfShadowColor);
          display->AllocateNamedColor("grey20",shadowColor);
        }

        for (size_t i=0; i<Display::colorCount; i++) {
          display->AllocateNamedColor(colorNames[i],color[i]);
        }

        if (display->GetColorMode()==Display::colorModeMonochrome) {
          fillNames[Display::backgroundFillIndex]="white";
          fillNames[Display::tableBackgroundFillIndex]=fillNames[Display::backgroundFillIndex];
          fillNames[Display::tableBackground2FillIndex]=fillNames[Display::backgroundFillIndex];
          fillNames[Display::hprogressFillIndex]="black";
          fillNames[Display::vprogressFillIndex]="black";
          fillNames[Display::tooltipWindowBackgroundFillIndex]=fillNames[Display::backgroundFillIndex];
        }
        else if (display->GetColorMode()==Display::colorModeGreyScale) {
          fillNames[Display::backgroundFillIndex]="grey70";
          fillNames[Display::tableBackgroundFillIndex]="white";
          fillNames[Display::tableBackground2FillIndex]="grey95";
          fillNames[Display::hprogressFillIndex]="grey60";
          fillNames[Display::vprogressFillIndex]="grey60";
          fillNames[Display::tooltipWindowBackgroundFillIndex]=fillNames[Display::backgroundFillIndex];
        }
        else {
          fillNames[Display::backgroundFillIndex]="grey70";
          fillNames[Display::tableBackgroundFillIndex]="white";
          fillNames[Display::tableBackground2FillIndex]="grey95";
          fillNames[Display::hprogressFillIndex]="royal blue";
          fillNames[Display::vprogressFillIndex]="royal blue";
          fillNames[Display::tooltipWindowBackgroundFillIndex]="light yellow";
        }

        fillNames[Display::buttonBackgroundFillIndex]=fillNames[Display::backgroundFillIndex];
        fillNames[Display::positiveButtonBackgroundFillIndex]=fillNames[Display::buttonBackgroundFillIndex];
        fillNames[Display::negativeButtonBackgroundFillIndex]=fillNames[Display::buttonBackgroundFillIndex];
        fillNames[Display::defaultButtonBackgroundFillIndex]=fillNames[Display::buttonBackgroundFillIndex];
        fillNames[Display::toolbarButtonBackgroundFillIndex]=fillNames[Display::buttonBackgroundFillIndex];
        fillNames[Display::scrollButtonBackgroundFillIndex]=fillNames[Display::buttonBackgroundFillIndex];
        fillNames[Display::comboBackgroundFillIndex]=fillNames[Display::buttonBackgroundFillIndex];
        fillNames[Display::editComboBackgroundFillIndex]=fillNames[Display::buttonBackgroundFillIndex];
        fillNames[Display::entryBackgroundFillIndex]=fillNames[Display::tableBackgroundFillIndex];
        fillNames[Display::hprogressBackgroundFillIndex]=fillNames[Display::backgroundFillIndex];
        fillNames[Display::vprogressBackgroundFillIndex]=fillNames[Display::backgroundFillIndex];
        fillNames[Display::hknobBoxFillIndex]=fillNames[Display::backgroundFillIndex];
        fillNames[Display::vknobBoxFillIndex]=fillNames[Display::backgroundFillIndex];
        fillNames[Display::tabBackgroundFillIndex]=fillNames[Display::backgroundFillIndex];
        fillNames[Display::tabRiderBackgroundFillIndex]=fillNames[Display::backgroundFillIndex];
        fillNames[Display::columnBackgroundFillIndex]=fillNames[Display::buttonBackgroundFillIndex];
        fillNames[Display::columnLeftBackgroundFillIndex]=fillNames[Display::columnBackgroundFillIndex];
        fillNames[Display::columnRightBackgroundFillIndex]=fillNames[Display::columnBackgroundFillIndex];
        fillNames[Display::columnMiddleBackgroundFillIndex]=fillNames[Display::columnBackgroundFillIndex];
        fillNames[Display::columnEndBackgroundFillIndex]=fillNames[Display::backgroundFillIndex];
        fillNames[Display::menuStripBackgroundFillIndex]=fillNames[Display::backgroundFillIndex];
        fillNames[Display::menuPulldownBackgroundFillIndex]=fillNames[Display::backgroundFillIndex];
        fillNames[Display::menuEntryBackgroundFillIndex]=fillNames[Display::backgroundFillIndex];
        fillNames[Display::menuWindowBackgroundFillIndex]=fillNames[Display::backgroundFillIndex];
        fillNames[Display::popupWindowBackgroundFillIndex]=fillNames[Display::backgroundFillIndex];
        fillNames[Display::dialogWindowBackgroundFillIndex]=fillNames[Display::backgroundFillIndex];
        fillNames[Display::graphBackgroundFillIndex]="white";
        fillNames[Display::toolbarBackgroundFillIndex]=fillNames[Display::backgroundFillIndex];
        fillNames[Display::labelBackgroundFillIndex]=fillNames[Display::backgroundFillIndex];
        fillNames[Display::listboxBackgroundFillIndex]=fillNames[Display::backgroundFillIndex];
        fillNames[Display::scrolledBackgroundFillIndex]=fillNames[Display::backgroundFillIndex];
        fillNames[Display::hscrollBackgroundFillIndex]=fillNames[Display::backgroundFillIndex];
        fillNames[Display::vscrollBackgroundFillIndex]=fillNames[Display::backgroundFillIndex];
        fillNames[Display::boxedBackgroundFillIndex]=fillNames[Display::backgroundFillIndex];
        fillNames[Display::plateBackgroundFillIndex]=fillNames[Display::backgroundFillIndex];
        fillNames[Display::statusbarBackgroundFillIndex]=fillNames[Display::backgroundFillIndex];

        for (size_t i=0; i<Display::fillCount; i++) {
          display->AllocateNamedColor(fillNames[i],fillColors[i]);
        }

        for (size_t i=0; i<Display::fillCount; i++) {
          fill[i]=new PlainFill(fillColors[i]);
        }

        fill[Display::tableBackgroundFillIndex]=new SelectFill(fillColors[Display::tableBackgroundFillIndex],color[Display::fillColor]);
        fill[Display::tableBackground2FillIndex]=new SelectFill(fillColors[Display::tableBackground2FillIndex],color[Display::fillColor]);
        fill[Display::entryBackgroundFillIndex]=new EditFill(fillColors[Display::entryBackgroundFillIndex],color[Display::fillColor]);

        fill[Display::buttonBackgroundFillIndex]=new FrameFill(FrameFill::button,fillColors[Display::buttonBackgroundFillIndex]);
        fill[Display::positiveButtonBackgroundFillIndex]=new FrameFill(FrameFill::button,fillColors[Display::positiveButtonBackgroundFillIndex]);
        fill[Display::negativeButtonBackgroundFillIndex]=new FrameFill(FrameFill::button,fillColors[Display::negativeButtonBackgroundFillIndex]);
        fill[Display::defaultButtonBackgroundFillIndex]=new FrameFill(FrameFill::button,fillColors[Display::defaultButtonBackgroundFillIndex]);
        fill[Display::toolbarButtonBackgroundFillIndex]=new FrameFill(FrameFill::button,fillColors[Display::toolbarButtonBackgroundFillIndex]);
        fill[Display::hprogressBackgroundFillIndex]=new FrameFill(FrameFill::box,fillColors[Display::hprogressBackgroundFillIndex]);
        fill[Display::vprogressBackgroundFillIndex]=new FrameFill(FrameFill::box,fillColors[Display::vprogressBackgroundFillIndex]);
        fill[Display::comboBackgroundFillIndex]=new FrameFill(FrameFill::button,fillColors[Display::comboBackgroundFillIndex]);
        fill[Display::editComboBackgroundFillIndex]=new FrameFill(FrameFill::box,fillColors[Display::editComboBackgroundFillIndex]);
        fill[Display::tabRiderBackgroundFillIndex]=new FrameFill(FrameFill::tabRider,fillColors[Display::tabRiderBackgroundFillIndex]);
        fill[Display::columnBackgroundFillIndex]=new FrameFill(FrameFill::button,fillColors[Display::columnBackgroundFillIndex]);
        fill[Display::columnLeftBackgroundFillIndex]=new FrameFill(FrameFill::button,fillColors[Display::columnLeftBackgroundFillIndex]);
        fill[Display::columnMiddleBackgroundFillIndex]=new FrameFill(FrameFill::button,fillColors[Display::columnMiddleBackgroundFillIndex]);
        fill[Display::columnRightBackgroundFillIndex]=new FrameFill(FrameFill::button,fillColors[Display::columnRightBackgroundFillIndex]);
        fill[Display::menuWindowBackgroundFillIndex]=new FrameFill(FrameFill::double3DOut,fillColors[Display::menuWindowBackgroundFillIndex]);
        fill[Display::popupWindowBackgroundFillIndex]=new FrameFill(FrameFill::double3DOut,fillColors[Display::popupWindowBackgroundFillIndex]);
        fill[Display::tooltipWindowBackgroundFillIndex]=new FrameFill(FrameFill::singleFrame,fillColors[Display::tooltipWindowBackgroundFillIndex]);
        fill[Display::labelBackgroundFillIndex]=new FrameFill(FrameFill::label,fillColors[Display::labelBackgroundFillIndex]);
        fill[Display::scrolledBackgroundFillIndex]=new FrameFill(FrameFill::box,fillColors[Display::scrolledBackgroundFillIndex]);
        fill[Display::boxedBackgroundFillIndex]=new FrameFill(FrameFill::box,fillColors[Display::boxedBackgroundFillIndex]);
        fill[Display::plateBackgroundFillIndex]=new FrameFill(FrameFill::singleFrame,fillColors[Display::plateBackgroundFillIndex]);

        frame[Display::tabFrameIndex]=new InternalFrame(InternalFrame::tab);
        frame[Display::hscaleFrameIndex]=new InternalFrame(InternalFrame::box);
        frame[Display::vscaleFrameIndex]=new InternalFrame(InternalFrame::box);
        frame[Display::focusFrameIndex]=new InternalFrame(InternalFrame::focus);
        frame[Display::statuscellFrameIndex]=new InternalFrame(InternalFrame::single3DIn);
        frame[Display::valuebarFrameIndex]=new InternalFrame(InternalFrame::single3DOut);
        frame[Display::groupFrameIndex]=new InternalFrame(InternalFrame::group3D);

        image[Display::hknobImageIndex]=new InternalImage(InternalImage::knob);
        image[Display::vknobImageIndex]=new InternalImage(InternalImage::knob);
        image[Display::arrowLeftImageIndex]=new InternalImage(InternalImage::arrowLeft);
        image[Display::arrowRightImageIndex]=new InternalImage(InternalImage::arrowRight);
        image[Display::arrowUpImageIndex]=new InternalImage(InternalImage::arrowUp);
        image[Display::arrowDownImageIndex]=new InternalImage(InternalImage::arrowDown);
        image[Display::scrollLeftImageIndex]=new InternalImage(InternalImage::scrollArrowLeft);
        image[Display::scrollRightImageIndex]=new InternalImage(InternalImage::scrollArrowRight);
        image[Display::scrollUpImageIndex]=new InternalImage(InternalImage::scrollArrowUp);
        image[Display::scrollDownImageIndex]=new InternalImage(InternalImage::scrollArrowDown);
        image[Display::comboImageIndex]=new InternalImage(InternalImage::combo);
        image[Display::comboEditButtonImageIndex]=new InternalImage(InternalImage::comboButton);
        image[Display::comboDividerImageIndex]=new InternalImage(InternalImage::vLine);
        image[Display::treeExpanderImageIndex]=new InternalImage(InternalImage::treeCross);
        image[Display::hscaleKnobImageIndex]=new InternalImage(InternalImage::hSliderKnob);
        image[Display::vscaleKnobImageIndex]=new InternalImage(InternalImage::vSliderKnob);
        image[Display::leftSliderImageIndex]=new InternalImage(InternalImage::sliderFilled);
        image[Display::rightSliderImageIndex]=new InternalImage(InternalImage::sliderUnfilled);
        image[Display::topSliderImageIndex]=new InternalImage(InternalImage::sliderUnfilled);
        image[Display::bottomSliderImageIndex]=new InternalImage(InternalImage::sliderFilled);
        image[Display::menuDividerImageIndex]=new InternalImage(InternalImage::hLine);
        image[Display::menuSubImageIndex]=new InternalImage(InternalImage::arrowRight);
        image[Display::menuCheckImageIndex]=new InternalImage(InternalImage::checkbox);
        image[Display::menuRadioImageIndex]=new InternalImage(InternalImage::radio);
        image[Display::radioImageIndex]=new InternalImage(InternalImage::radio);
        image[Display::checkImageIndex]=new InternalImage(InternalImage::checkbox);
        image[Display::ledImageIndex]=new InternalImage(InternalImage::led);
        // positive
        // default
        // help
        // close
      }

      DefaultTheme::~DefaultTheme()
      {
        display->FreeColor(shineColor);
        display->FreeColor(shadowColor);

        for (size_t i=0; i<Display::fillCount; i++) {
          display->FreeColor(fillColors[i]);
        }

        for (size_t i=0; i<Display::colorCount; i++) {
          display->FreeColor(color[i]);
        }
      }

      bool DefaultTheme::HasFontSettings() const
      {
        return false;
      }

      std::wstring DefaultTheme::GetProportionalFontName() const
      {
        return L"helvetica";
      }

      double DefaultTheme::GetProportionalFontSize() const
      {
        return DEFAULT_PROP_FONT_SIZE;
      }

      std::wstring DefaultTheme::GetFixedFontName() const
      {
        return L"fixed";
      }

      double DefaultTheme::GetFixedFontSize() const
      {
        return DEFAULT_FIXED_FONT_SIZE;
      }

      size_t DefaultTheme::GetSpaceHorizontal(Display::Space space) const
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

      size_t DefaultTheme::GetSpaceVertical(Display::Space space) const
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

      size_t DefaultTheme::GetIconWidth() const
      {
        return 16;
      }

      size_t DefaultTheme::GetIconHeight() const
      {
        return 16;
      }

      size_t DefaultTheme::GetDragStartSensitivity() const
      {
        return 6;
      }

      size_t DefaultTheme::GetMouseClickHoldSensitivity() const
      {
        return 5;
      }

      size_t DefaultTheme::GetMinimalButtonWidth() const
      {
        return Lum::Base::GetSize(Lum::Base::Size::stdCharWidth,8);
      }

      size_t DefaultTheme::GetFirstTabOffset() const
      {
        return 0;
      }

      size_t DefaultTheme::GetLastTabOffset() const
      {
        return OS::display->GetSpaceHorizontal(OS::Display::spaceInterObject);
      }

      OS::Theme::ScrollKnobMode DefaultTheme::GetScrollKnobMode() const
      {
        return OS::Theme::scrollKnobModeDouble;
      }

      OS::Theme::ToolbarPosition DefaultTheme::GetToolbarPosition() const
      {
        return OS::Theme::toolbarTop;
      }

      bool DefaultTheme::ShowButtonImages() const
      {
        return true;
      }

      OS::Theme::MenuType DefaultTheme::GetMenuType() const
      {
        return menuTypeStrip;
      }
    }
  }
}
