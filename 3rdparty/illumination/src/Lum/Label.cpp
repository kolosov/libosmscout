#include <Lum/Label.h>

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

#include <Lum/Base/Util.h>

#include <Lum/OS/Display.h>

#include <Lum/Text.h>
#include <Lum/TextLabel.h>

namespace Lum {

  void Label::Entry::CalcSize()
  {
    label->CalcSize();
    object->CalcSize();

    size_t height=std::max(label->GetOHeight(),object->GetOHeight());

    label->ResizeHeight(height);
    object->ResizeHeight(height);
  }

  size_t Label::Entry::GetMinHeight() const
  {
    if (label->HasBaseline() && object->HasBaseline()) {
      return std::max(label->GetBaseline(),
                      object->GetBaseline())+
             std::max(label->GetOMinHeight()-label->GetBaseline(),
                      object->GetOMinHeight()-object->GetBaseline());
    }
    else {
      size_t height=0;

      height=std::max(height,label->GetOMinHeight());
      height=std::max(height,object->GetOMinHeight());

      return height;
    }
  }

  size_t Label::Entry::GetHeight() const
  {
    if (label->HasBaseline() && object->HasBaseline()) {
      return std::max(label->GetBaseline(),
                      object->GetBaseline())+
             std::max(label->GetOHeight()-label->GetBaseline(),
                      object->GetOHeight()-object->GetBaseline());
    }
    else {
      size_t height=0;

      height=std::max(height,label->GetOHeight());
      height=std::max(height,object->GetOHeight());

      return height;
    }
  }

  size_t Label::Entry::CanResize(bool grow) const
  {
    return label->CanResize(grow,false) || object->CanResize(grow,false);
  }

  void Label::Entry::Resize(size_t height)
  {
    if (label->HasBaseline() && object->HasBaseline()) {
      height-=std::max(label->GetBaseline(),object->GetBaseline())-
              std::min(label->GetBaseline(),object->GetBaseline());
    }

    label->ResizeHeight(height);
    object->ResizeHeight(height);
  }

  bool Label::Entry::HasOffset() const
  {
    return label->HasBaseline() && object->HasBaseline();
  }

  size_t Label::Entry::GetLabelOffset() const
  {
    if (label->HasBaseline() && object->HasBaseline()) {
      if (object->GetBaseline()>label->GetBaseline()) {
        return object->GetBaseline()-label->GetBaseline();
      }
    }

    return 0;
  }

  size_t Label::Entry::GetObjectOffset() const
  {
    if (label->HasBaseline() && object->HasBaseline()) {
      if (label->GetBaseline()>object->GetBaseline()) {
        return label->GetBaseline()-object->GetBaseline();
      }
    }

    return 0;
  }

  Label::Label()
  : labelWidth(0),
    objectWidth(0),
    landscapeMode(OS::display->IsLandscape())
  {
    // no code
  }

  Label::~Label()
  {
    for (std::list<Entry>::iterator iter=list.begin();
         iter!=list.end();
         ++iter) {
      delete iter->label;
      delete iter->object;
    }
  }

  bool Label::VisitChildren(Visitor &visitor, bool /*onlyVisible*/)
  {
    for (std::list<Entry>::iterator iter=list.begin();
         iter!=list.end();
         ++iter) {
      if (!iter->object->HasImplicitLabel()) {
        if (!visitor.Visit(iter->label)) {
          return false;
        }
      }

      if (!visitor.Visit(iter->object)) {
        return false;
      }
    }

    return true;
  }

  /**
    Add a new entry to the label. A label entry consists of two objects.
    \p text should be a textual label, while \p object is the
    object \p text is the label for.
  */
  Label* Label::AddLabel(Object* label, Object* object)
  {
    assert(label!=NULL);
    assert(object!=NULL);

    Entry entry;

    label->SetParent(this);
    object->SetParent(this);

    object->SetLabelObject(label);

    entry.indent=0;
    entry.label=label;
    entry.object=object;

    list.push_back(entry);

    return this;
  }

  Label* Label::AddLabel(const std::wstring& label, Object* object)
  {
    return AddLabel(new TextLabel(label),object);
  }

  Label* Label::AddLabel(const std::wstring& label, const std::wstring& object)
  {
    return AddLabel(label,new Text(object));
  }

  void Label::CalcSize()
  {
    landscapeMode=true;

    if (GetWindow()!=NULL &&
        OS::display->IsLandscape()) {
      landscapeMode=GetWindow()->GetScreenOrientationHint()==OS::Window::screenOrientationBothSupported ||
                    GetWindow()->GetScreenOrientationHint()==OS::Window::screenOrientationLandscapeSupported ||
                    GetWindow()->GetScreenOrientationHint()==OS::Window::screenOrientationAgnostic;
    }
    else {
      landscapeMode=!(GetWindow()->GetScreenOrientationHint()==OS::Window::screenOrientationBothSupported ||
                      GetWindow()->GetScreenOrientationHint()==OS::Window::screenOrientationPortraitSupported);
    }

    if (!landscapeMode) {
      for (std::list<Entry>::iterator iter=list.begin();
           iter!=list.end();
           ++iter) {
        if (iter->label!=NULL &&
            dynamic_cast<TextLabel*>(iter->label)!=NULL) {
          dynamic_cast<TextLabel*>(iter->label)->SetStyle(OS::Font::bold);
        }
      }
    }

    height=0;
    minHeight=0;
    labelWidth=0;

    if (landscapeMode) {
      size_t objectMinWidth=0;
      // Calculate bounds of labels and objects
      for (std::list<Entry>::iterator iter=list.begin();
           iter!=list.end();
           ++iter) {
        iter->CalcSize();

        if (!iter->object->HasImplicitLabel()) {
          labelWidth=std::max(labelWidth,iter->label->GetOWidth());
        }

        objectWidth=std::max(objectWidth,iter->object->GetOWidth());
        objectMinWidth=std::max(objectMinWidth,iter->object->GetOMinWidth());

        minHeight+=iter->GetMinHeight();
        height+=iter->GetHeight();
      }

      // Add inter line space to height
      if (list.size()>1) {
        height+=(list.size()-1)*OS::display->GetSpaceVertical(OS::Display::spaceInterObject);
        minHeight+=(list.size()-1)*OS::display->GetSpaceVertical(OS::Display::spaceInterObject);
      }

      width=labelWidth+objectWidth;
      minWidth=labelWidth+objectMinWidth;

      objectWidth=width-labelWidth;

      if (labelWidth>0) {
        width+=OS::display->GetSpaceHorizontal(OS::Display::spaceLabelObject);
        minWidth+=OS::display->GetSpaceHorizontal(OS::Display::spaceLabelObject);
      }
    }
    else {
      for (std::list<Entry>::iterator iter=list.begin();
           iter!=list.end();
           ++iter) {
        iter->CalcSize();

        // gap to next label
        if (iter!=list.begin()) {
          minHeight+=OS::display->GetSpaceVertical(OS::Display::spaceInterObject);
          height+=OS::display->GetSpaceVertical(OS::Display::spaceInterObject);
        }

        // label dimension
        if (!iter->object->HasImplicitLabel()) {
          labelWidth=std::max(labelWidth,iter->label->GetOWidth());
        }

        // object dimension
        objectWidth=std::max(objectWidth,iter->object->GetOWidth());

        // calculate width and height
        if (!iter->object->HasImplicitLabel()) {
          minHeight+=iter->label->GetOMinHeight();
          height+=iter->label->GetOHeight();

          minHeight+=OS::display->GetSpaceVertical(OS::Display::spaceInterObject);
          height+=OS::display->GetSpaceVertical(OS::Display::spaceInterObject);

          minWidth=std::max(minWidth,iter->label->GetOMinWidth());
          width=std::max(width,iter->label->GetOWidth());
        }

        minWidth=std::max(minWidth,iter->object->GetOMinWidth());
        width=std::max(width,iter->object->GetOWidth());
        minHeight+=iter->object->GetOMinHeight();
        height+=iter->object->GetOHeight();
      }
    }

    /*
    IF (l.parent#NIL) & (l.parent IS P.Panel) THEN (* if we are in a panel*)
      (*
        Now we use a visitor to walk over all children of our parent (all
        objects in the same parent panel). If CalcSize was already called for
        them, we resize them to the smae labelWidth and objectWidth values and relayout them.

        See Label.ReLayout And local visitor implementation.
      *)
      visitor.me:=l;
      IF l.parent.VisitChildren(visitor,TRUE) THEN
      END;
    END;*/
    Group::CalcSize();
  }

  void Label::Layout()
  {
    size_t curHeight;
    int    pos;
    size_t space=0;

    if (landscapeMode) {
      if (labelWidth>0) {
        space=OS::display->GetSpaceHorizontal(OS::Display::spaceLabelObject);
      }

      objectWidth=width-labelWidth-space;

      /*
        Calculate current height of labels and objects.
        If label is smaller than object in same row try to make it fit.
        Also do this if object is smaller than label.
      */
      curHeight=0;
      for (std::list<Entry>::iterator iter=list.begin();
           iter!=list.end();
           ++iter) {
        curHeight+=iter->GetHeight();

        if (!iter->object->HasImplicitLabel()) {
          iter->label->ResizeWidth(labelWidth);
        }

        iter->object->ResizeWidth(objectWidth);
      }

      // Now add inter row spacing
      if (list.size()>1) {
        curHeight+=(list.size()-1)*OS::display->GetSpaceVertical(OS::Display::spaceInterObject);
      }

      /*
        If current height is not equal to exspected height, resize until equal
         or until no object can be resized anymore.
      */
      while (curHeight!=height) {
        int count=0;

        // Count number of resizable objects
        for (std::list<Entry>::iterator iter=list.begin();
             iter!=list.end();
             ++iter) {
          if (iter->object->CanResize(height>curHeight,false)) {
            count++;
          }
        }

        // Quit if we cannot resize an object anymore.
        if (count==0) {
          break;
        }

        for (std::list<Entry>::iterator iter=list.begin();
             iter!=list.end();
             ++iter) {
          if (iter->CanResize(height>curHeight)) {
            int old;

            old=iter->GetHeight();
            iter->Resize(iter->GetHeight()+Base::UpDiv(height-curHeight,count));
            curHeight+=iter->GetHeight()-old;
            count--;
          }
        }
      }

      // Reposition all objects

      pos=y;
      for (std::list<Entry>::iterator iter=list.begin();
           iter!=list.end();
           ++iter) {
        if (OS::display->GetDefaultTextDirection()==OS::Display::textDirectionRightToLeft) {
          iter->object->Move(x,pos+iter->GetObjectOffset());

          iter->label->Move(x+objectWidth+space,
                            pos+iter->GetLabelOffset());
        }
        else {
          if (iter->HasOffset()) {
            iter->label->Move(x,pos+iter->GetLabelOffset());
            iter->object->Move(x+labelWidth+space,
                               pos+iter->GetObjectOffset());
          }
          else if (iter->object->GetOHeight()>3*iter->label->GetOHeight()) {
            iter->label->Move(x,pos);
          }
          else {
            iter->label->Move(x,pos+(iter->GetHeight()-iter->label->GetOHeight())/2);
          }

          if (iter->object->GetOHeight()<=iter->label->GetOHeight()) {
            iter->object->Move(x+labelWidth+space,
            pos+(iter->label->GetOHeight()-iter->object->GetOHeight())/2);
          }
          else {
            iter->object->Move(x+labelWidth+space,pos);
          }
        }

        pos+=iter->GetHeight()+OS::display->GetSpaceVertical(OS::Display::spaceInterObject);
      }
    }
    else {
      curHeight=0;
      for (std::list<Entry>::iterator iter=list.begin();
           iter!=list.end();
           ++iter) {
        if (iter!=list.begin()) {
          curHeight+=OS::display->GetSpaceVertical(OS::Display::spaceInterObject);
        }

        if (!iter->object->HasImplicitLabel()) {
          iter->label->ResizeWidth(width);

          curHeight+=iter->label->GetOHeight();
          curHeight+=OS::display->GetSpaceVertical(OS::Display::spaceInterObject);
        }

        iter->object->ResizeWidth(width);

        curHeight+=iter->object->GetOHeight();
      }

      while (curHeight!=height) {
        int count=0;

        // Count number of resizable objects
        for (std::list<Entry>::iterator iter=list.begin();
             iter!=list.end();
             ++iter) {
          if (iter->object->CanResize(height>curHeight,false)) {
            count++;
          }
        }

        // Quit if we cannot resize an object anymore.
        if (count==0) {
          break;
        }

        for (std::list<Entry>::iterator iter=list.begin();
             iter!=list.end();
             ++iter) {
          if (iter->object->CanResize(height>curHeight,false)) {
            int old;

            old=iter->GetHeight();
            iter->object->Resize(iter->object->GetOWidth(),
                                 iter->object->GetOHeight()+
                                 Base::UpDiv(height-curHeight,count));
            curHeight+=iter->object->GetOHeight()-old;
            count--;
          }
        }
      }

      // Reposition all objects

      pos=y;
      for (std::list<Entry>::iterator iter=list.begin();
           iter!=list.end();
           ++iter) {

        if (!iter->object->HasImplicitLabel()) {
          iter->label->Move(x,pos);
          pos+=iter->label->GetOHeight()+
               OS::display->GetSpaceVertical(OS::Display::spaceInterObject);
        }

        iter->object->Move(x,pos);
        pos+=iter->object->GetOHeight()+
             OS::display->GetSpaceVertical(OS::Display::spaceInterObject);
      }
    }

    Group::Layout();
  }

  Label* Label::Create(bool horizontalFlex, bool verticalFlex)
  {
    Label *l;

    l=new Label();
    l->SetFlex(horizontalFlex,verticalFlex);

    return l;
  }

}

