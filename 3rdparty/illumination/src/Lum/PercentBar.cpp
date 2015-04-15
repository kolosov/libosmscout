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
#include <Lum/PercentBar.h>

#include <Lum/Base/String.h>

namespace Lum {

  PercentBar::PercentBar()
  : horizontal(true),
    font(OS::display->GetFont(OS::Display::fontScaleFootnote))
  {
    SetBackground(OS::display->GetFill(OS::Display::graphBackgroundFillIndex));
  }

  void PercentBar::DisplayHorizontal()
  {
    horizontal=true;
  }

  void PercentBar::DisplayVertical()
  {
    horizontal=false;
  }

  bool PercentBar::SetModel(Base::Model* model)
  {
    this->model=dynamic_cast<Model::DoubleDataStream*>(model);

    Control::SetModel(this->model);

    return this->model.Valid();
  }

  void PercentBar::CalcSize()
  {
    OS::FrameRef box=OS::display->GetFrame(OS::Display::valuebarFrameIndex);

    if (horizontal) {
      width=1;
      height=font->height+
             2*OS::display->GetSpaceVertical(OS::Display::spaceObjectBorder)+
             box->minHeight;
    }
    else {
      width=0;
      for (size_t i=0; i<=9; i++) {
        width=std::max(width,font->StringWidth(Base::NumberToWString(i)));
      }

      width=font->StringWidth(L"1")+2*width+font->StringWidth(L"%")+
            2*OS::display->GetSpaceHorizontal(OS::Display::spaceObjectBorder)+
            box->minWidth;
    height=1;
    }

    minWidth=width;
    minHeight=height;

    Object::CalcSize();
  }

  void PercentBar::Draw(OS::DrawInfo* draw,
                        int x, int y, size_t w, size_t h)
  {
    Object::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* --- */

    if (!model.Valid() || model->IsNull()) {
      return;
    }

    double sum;
    size_t entries,count,current,prev;

    sum=0;
    entries=0;
    for (size_t i=0; i<model->GetChannels(); i++) {
      sum+=model->Get(i);
      entries++;
    }

    if (sum<100.0) {
      entries++;
    }

    prev=0;
    count=1;
    for (size_t i=0; i<model->GetChannels(); i++) {
      OS::FrameRef valueBox=OS::display->GetFrame(OS::Display::valuebarFrameIndex);

      if (horizontal) {
        current=(size_t)(width*model->Get(i)+0.5);

        if (current>=valueBox->minWidth) {
          std::wstring   label;
          OS::FontExtent extent;

          draw->PushUniqueFill(count-1,entries);
          draw->FillRectangle(this->x+prev+valueBox->leftBorder,this->y+valueBox->topBorder,
                              current-valueBox->minWidth,height-valueBox->minHeight);
          draw->PopUniqueFill(count,entries);
          valueBox->Draw(draw,this->x+prev,this->y,current,height);

          label=::Lum::Base::NumberToWString((int)(model->Get(i)*100));
          label.append(L"%");

          font->StringExtent(label,extent);

          if (current-valueBox->minWidth>=extent.width && height-valueBox->minHeight>=font->height) {
            draw->PushForeground(OS::Display::textColor);
            draw->PushFont(font,OS::Font::normal);
            draw->DrawString(this->x+prev+(current-extent.width)/2,
                             this->y+font->ascent+(height-font->height)/2,
                             label);
            draw->PopFont();
            draw->PopForeground();
          }

          prev+=current;
        }
      }
      else {
        current=(size_t)(height*model->Get(i)+0.5);

        if (current>=valueBox->minHeight) {
          std::wstring   label;
          OS::FontExtent extent;

          draw->PushUniqueFill(count-1,entries);
          draw->FillRectangle(this->x+valueBox->leftBorder,this->y+height-prev-current+valueBox->topBorder,
                              width-valueBox->minWidth,current-valueBox->minHeight);
          draw->PopUniqueFill(count,entries);
          valueBox->Draw(draw,this->x,this->y+height-prev-current,width,current);

          label=::Lum::Base::NumberToWString((int)(model->Get(i)*100));
          label.append(L"%");

          font->StringExtent(label,extent);

          if (current-valueBox->minHeight>=font->height && width>=width-valueBox->minWidth) {
            draw->PushForeground(OS::Display::textColor);
            draw->PushFont(font,OS::Font::normal);
            draw->DrawString(this->x+valueBox->leftBorder+(width-extent.width-valueBox->minWidth)/2,
                             this->y+height-prev-current+valueBox->topBorder+(current-valueBox->minHeight-font->height)/2+font->ascent,
                             label);
            draw->PopFont();
            draw->PopForeground();
          }

          prev+=current;
        }
      }

      count++;
    }
  }

  void PercentBar::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==this->model) {
      Redraw();
    }
    else {
      Control::Resync(model,msg);
    }
  }
}
