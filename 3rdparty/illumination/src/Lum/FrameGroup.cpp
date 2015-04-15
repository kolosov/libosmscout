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

#include <Lum/FrameGroup.h>

#include <Lum/Text.h>

namespace Lum {

  FrameGroup::FrameGroup()
    : frame(OS::display->GetFrame(OS::Display::groupFrameIndex)),
      object(NULL),
      text(NULL),
      horizSpace(0),
      vertSpace(0),
      space(true)
  {
    // no code
  }

  FrameGroup::~FrameGroup()
  {
    delete object;
    delete text;
  }

  bool FrameGroup::VisitChildren(Visitor &visitor, bool onlyVisible)
  {
    if (object!=NULL) {
      if (!visitor.Visit(object)) {
        return false;
      }
    }

    if (text!=NULL) {
      if (!visitor.Visit(text)) {
        return false;
      }
    }

    return true;
  }

  void FrameGroup::SetFrame(OS::Frame* frame)
  {
    this->frame=frame;
  }

  void FrameGroup::SetObject(Object* object)
  {
    delete this->object;

    this->object=object;
    this->object->SetParent(this);
  }

  /**
    If parameter space is 'true' some space is left between the frame
    and its content.
  */
  void FrameGroup::SetSpace(bool space)
  {
    this->space=space;
  }

  /**
    Some frames allows an aditional (textual) label drawn as part of the
    frm. Call this method to set the object to be displayed.
  */
  void FrameGroup::SetLabel(Object* label)
  {
    delete this->text;

    this->text=label;
    this->text->SetParent(this);
  }

  void FrameGroup::SetLabel(const std::wstring& label)
  {
    SetLabel(new Text(label));
  }

  void FrameGroup::CalcSize()
  {
    if (space) {
      horizSpace=OS::display->GetSpaceHorizontal(OS::Display::spaceGroupIndent);
      vertSpace=OS::display->GetSpaceVertical(OS::Display::spaceGroupIndent);
    }
    else {
      horizSpace=0;
      vertSpace=0;
    }

    if (text!=NULL) {
      text->CalcSize();
      frame->SetGap(OS::display->GetSpaceHorizontal(OS::Display::spaceGroupIndent),
                    text->GetOWidth(),text->GetOHeight());
    }

    width=horizSpace+frame->leftBorder+frame->rightBorder;
    height=vertSpace+frame->topBorder+frame->bottomBorder;

    minWidth=width;
    minHeight=height;

    if (object!=NULL) {
      object->CalcSize();
      width+=object->GetOWidth();
      height+=object->GetOHeight();
      minWidth+=object->GetOMinWidth();
      minHeight+=object->GetOMinHeight();
    }

    Group::CalcSize();
  }

  void FrameGroup::Layout()
  {
    if (object!=NULL) {
      object->Resize(width-horizSpace-frame->leftBorder-frame->rightBorder,
                    height-vertSpace-frame->topBorder-frame->bottomBorder);
      object->Move(x+frame->leftBorder+(width-frame->leftBorder-frame->rightBorder-object->GetOWidth()) / 2,
                   y+frame->topBorder+(height-frame->topBorder-frame->bottomBorder-object->GetOHeight()) / 2);
    }

    if (text!=NULL) {
      text->Move(x+frame->gx+OS::display->GetSpaceHorizontal(OS::Display::spaceLabelObject),y);
    }

    Group::Layout();
  }

  void FrameGroup::Draw(OS::DrawInfo* draw,
                        int x, int y, size_t w, size_t h)
  {
    Group::Draw(draw,x,y,w,h); /* We must call Draw of our superclass */

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* --- */

    frame->Draw(draw,this->x,this->y,this->width,this->height);
  }

}
