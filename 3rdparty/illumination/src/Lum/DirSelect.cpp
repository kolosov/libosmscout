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

#include <Lum/DirSelect.h>

#include <Lum/Base/Path.h>
#include <Lum/Base/Util.h>

#include <Lum/Button.h>

namespace Lum {

  DirSelect::DirSelect()
  : model(NULL),templ(NULL)
  {
    SetBackground(OS::display->GetFill(OS::Display::boxedBackgroundFillIndex));
  }

  DirSelect::~DirSelect()
  {
    for (size_t x=0; x<objects.size(); x++) {
      delete objects[x];
    }

    delete templ;
  }

  Object* DirSelect::CreateButton()
  {
    Button *button;

    button=new Button();
    button->SetFlex(false,true);
    button->SetParent(this);
    button->SetWindow(GetWindow());

    return button;
  }

  void DirSelect::GenerateItemList()
  {
    std::wstring itemPath;

    if (!model.Valid()) {
      return;
    }

    items.clear();

    items.resize(model->Get().GetPartCount()+1);

    for (size_t x=0; x<model->Get().GetPartCount(); x++) {
      items[model->Get().GetPartCount()-x-1].label=model->Get().GetPart(x);
    }

    items[model->Get().GetPartCount()].label=model->Get().GetRoot();

    // Resize arrays if required
    if (items.size()>models.size()) {
      models.resize(items.size());
    }

    if (items.size()>objects.size()) {
      objects.resize(items.size(),NULL);
    }

    // Create models and button for display if required
    for (size_t i=0; i<items.size(); i++) {
      if (!models[i].Valid()) {
        models[i]=new Model::Action();
        Observe(models[i]);
      }

      if (objects[i]==NULL) {
        objects[i]=CreateButton();
        dynamic_cast<Button*>(objects[i])->SetModel(models[i]);
      }

      objects[i]->SetId(i);
      dynamic_cast<Button*>(objects[i])->SetText(items[i].label);
      objects[i]->CalcSize();
    }

    SetRelayout();
  }

  bool DirSelect::SetModel(Base::Model* model)
  {
    this->model=dynamic_cast<Model::Path*>(model);

    Control::SetModel(this->model);

    return this->model.Valid();
  }

  bool DirSelect::VisitChildren(Visitor &visitor, bool /*onlyVisible*/)
  {
    for (size_t x=0; x<items.size(); x++) {
      if (!visitor.Visit(objects[x])) {
        return false;
      }
    }

    return true;
  }

  void DirSelect::CalcSize()
  {
    templ=CreateButton();
    dynamic_cast<Button*>(templ)->SetText(L"?");
    templ->CalcSize();

    width=1;
    height=templ->GetOHeight();

    minWidth=width;
    minHeight=height;

    Control::CalcSize();
  }

  bool DirSelect::HandleMouseEvent(const OS::MouseEvent& event)
  {
    class MouseEventVisitor : public Visitor
    {
    public:
      const OS::MouseEvent& event;
      Object                *grab;

    public:
      MouseEventVisitor(const OS::MouseEvent& event) : event(event) {}
      bool Visit(Object* object)
      {
        return !object->HandleMouseEvent(event);
      }
    };

    MouseEventVisitor visitor(event);
    bool              res;

    if (!visible) {
      return false;
    }

    //visitor.grab=grab;

    res=!VisitChildren(visitor,true);

    //grab=visitor.grab;

    return res;
  }

  void DirSelect::Layout()
  {
    size_t width;
    int    pos;

    width=0;
    for (size_t i=0; i<items.size(); i++) {
      objects[i]->ResizeHeight(this->height);
      width+=objects[i]->GetOWidth();
    }

    if (width>this->width) {
      pos=this->x+this->width;
    }
    else {
      pos=this->x+width;
    }

    for (size_t i=0; i<items.size(); i++) {
      objects[i]->Move(pos-objects[i]->GetOWidth(),this->y);

      pos-=objects[i]->GetOWidth();
    }

    Control::Layout();
  }

  void DirSelect::PreDrawChilds(OS::DrawInfo* draw)
  {
    draw->PushClip(GetX(),GetY(),GetWidth(),GetHeight());
  }

  void DirSelect::PostDrawChilds(OS::DrawInfo* draw)
  {
    draw->PopClip();
  }

  void DirSelect::Resync(Base::Model* model, const Base::ResyncMsg& /*msg*/)
  {
    if (model==this->model) {
      GenerateItemList();
      Redraw();
    }
    else { // Buttons
      size_t x=0;
      while (x<models.size()) {
        if (models[x].Get()==model && models[x]->IsFinished()) {
          if (this->model.Valid()) {
            Lum::Base::Path newPath;

            if (x==this->model->Get().GetPartCount()) {
              newPath.SetNativeDir(this->model->Get().GetRoot());
            }
            else {
              newPath.SetNativeDir(this->model->Get().GetSubDir(this->model->Get().GetPartCount()-x-1));
            }
            this->model->Set(newPath);
          }
          break;
        }
        x++;
      }
    }
  }
}
