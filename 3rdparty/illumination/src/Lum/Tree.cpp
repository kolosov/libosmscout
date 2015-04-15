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

#include <Lum/Tree.h>

#include <Lum/Dialog.h>

namespace Lum {

  Tree::Tree()
  : font(OS::display->GetFont()),
    selected(NULL),colHeight(0)
  {
    SetBackground(OS::display->GetFill(OS::Display::listboxBackgroundFillIndex));

    SetCanFocus(true);
    RequestFocus();
  }

  Tree::~Tree()
  {
    // no code
  }

  bool Tree::SetModel(Base::Model* model)
  {
    this->model=dynamic_cast<Model::Tree*>(model);

    Scrollable::SetModel(this->model);

    return this->model.Valid();
  }

  void Tree::SetSelectionChangedAction(Lum::Model::Action* action)
  {
    selectionChangedAction=action;
    if (action!=NULL) {
      selectionChangedAction->Trigger();
    }
  }

  Model::Action* Tree::GetSelectionChangedAction() const
  {
    return selectionChangedAction.Get();
  }

  void Tree::SetDoubleClickAction(Lum::Model::Action* action)
  {
    doubleClickAction=action;
  }

  Model::Action* Tree::GetDoubleClickAction() const
  {
    return doubleClickAction.Get();
  }

  void Tree::SetSelection(Model::Tree::Item* item)
  {
    assert(model.Valid());

    if (selected!=item) {
      if (selected!=NULL && selected->IsVisible() && visible) {
        Model::Tree::Item *tmp;
        size_t            pos,offset;

        tmp=selected;
        selected=NULL;

        tmp->GetPosAndOffset(pos,offset);

        if (pos>=vAdjustment->GetTop() && pos<=vAdjustment->GetTop()+vAdjustment->GetVisible()-1) {
          Redraw(this->x,this->y+colHeight*(pos-vAdjustment->GetTop()),width,colHeight);
        }
      }

      selected=item;

      if (selected!=NULL && selected->IsVisible() && visible) {
        size_t pos,offset;

        selected->GetPosAndOffset(pos,offset);

        if (pos>=vAdjustment->GetTop() && pos<=vAdjustment->GetTop()+vAdjustment->GetVisible()-1) {
          Redraw(this->x,this->y+colHeight*(pos-vAdjustment->GetTop()),width,colHeight);
        }
      }

      if (selectionChangedAction.Valid()) {
        selectionChangedAction->Trigger();
      }
    }
  }

  Model::Tree::Item* Tree::GetSelection() const
  {
    return selected;
  }

  void Tree::CalcSize()
  {
    colHeight=font->height+2*OS::display->GetSpaceVertical(OS::Display::spaceObjectBorder);

    width=1;
    height=1;

    /* Our minimal size is equal to the normal size */
    minWidth=width;
    minHeight=height;

    /* We *must* call CalcSize of our superclass! */
    Scrollable::CalcSize();
  }

  void Tree::GetDimension(size_t& width, size_t& height)
  {
    width=this->width;

    if (model.Valid()) {
      height=model->GetVisible();
    }
    else {
      height=colHeight*this->height;
    }
  }

  void Tree::Layout()
  {
    if (!model.Valid() || model->GetTop()==NULL) {
      hAdjustment->SetInvalid();
      vAdjustment->SetInvalid();
    }
    else {
      if (vAdjustment->GetTop()+height/colHeight-1>model->GetVisible()) {
        vAdjustment->SetDimension(model->GetVisible()-vAdjustment->GetTop()+1,model->GetVisible());
      }
      else {
        vAdjustment->SetDimension(std::min(height/colHeight,model->GetVisible()),model->GetVisible());
      }

      //TODO: Set hAdjustment
      hAdjustment->SetDimension(width,width);
    }

    Scrollable::Layout();
  }

  bool Tree::GetClickedEntryPos(int y, size_t& pos)
  {
    assert(model.Valid());

    pos=(y-this->y) / colHeight;
    if ((y-this->y) % colHeight>0) {
      ++pos;
    }

    pos+=vAdjustment->GetTop()-1;

    return pos>=1 && pos<=model->GetVisible();
  }

  bool Tree::HandleMouseEvent(const OS::MouseEvent& event)
  {
    /* It makes no sense to get the focus if we are currently not visible */
    if (!visible || !model.Valid() || !model->IsEnabled()) {
      return false;
    }

    if (event.type==OS::MouseEvent::down && PointIsIn(event) && (event.button==OS::MouseEvent::button1 || event.button==OS::MouseEvent::button3)) {
      size_t pos;

      if (GetClickedEntryPos(event.y,pos)) {
        Model::Tree::Item *item;
        size_t            offset;

        /* ignore */ model->GetVisibleItem(pos,item,offset);

        if (event.x>=x+(int)(offset*OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject)) &&
            event.x<=x+(int)(offset*OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject))+(int)colHeight) {
          if (item->HasChildren()) {
            if (item->IsExpanded()) {
              item->Collaps();
            }
            else {
              item->Expand();
            }
          }
        }
        else if (event.x>x+(int)(offset*OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject))+(int)colHeight &&
                 event.x<(int)(x+width)) {
          if (item!=selected) {
            SetSelection(item);
          }
          else if (GetWindow()->IsDoubleClicked() &&
                   item!=NULL) {
            if (doubleClickAction.Valid()) {
              doubleClickAction->Trigger();
            }
          }
        }
      }

      /*
        Since we want the focus for waiting for buttonup we return
        a pointer to ourself.
      */
      return true;
    }

    return false;
  }

  bool Tree::HandleKeyEvent(const OS::KeyEvent& event)
  {
    if (!model.Valid()) {
      return false;
    }

    if (event.type==OS::KeyEvent::down) {
      switch (event.key) {
      case OS::keyLeft:
        if (selected!=NULL) {
          if (selected->HasChildren() &&
              selected->IsExpanded()) {
            selected->Collaps();
          }
        }
        break;
      case OS::keyRight:
        if (selected!=NULL) {
          if (selected->HasChildren() &&
              !selected->IsExpanded()) {
            selected->Expand();
          }
        }
        break;
      case OS::keyUp:
        if (selected!=NULL) {
          if (selected->GetPrevious()!=NULL &&
              selected->GetPrevious()->HasChildren() &&
              selected->GetPrevious()->IsExpanded()) {
            SetSelection(selected->GetPrevious()->GetLastChild());
          }
          else if (selected->GetPrevious()!=NULL) {
            SetSelection(selected->GetPrevious());
          }
          else if (selected->GetParent()!=NULL) {
            SetSelection(selected->GetParent());
          }
        }
        break;
      case OS::keyDown:
        if (selected!=NULL) {
          if (selected->HasChildren() &&
              selected->IsExpanded()) {
            SetSelection(selected->GetFirstChild());
          }
          else if (selected->GetNext()!=NULL) {
            SetSelection(selected->GetNext());
          }
          else if (selected->GetParent()!=NULL &&
                   selected->GetParent()->GetNext()!=NULL) {
            SetSelection(selected->GetParent()->GetNext());
          }
        }
        else {
          SetSelection(model->GetTop());
        }
        break;
      default:
        return false;
        break;
      }

      return true;
    }

    return false;
  }

  void Tree::DrawItem(OS::DrawInfo* draw, Model::Tree::Item* item, size_t y, size_t offset)
  {
    if (item->GetObject()!=NULL) {
      // TODO
    }
    else {
      OS::FontExtent extent;
      int            x;
      OS::ImageRef   cross=OS::display->GetImage(OS::Display::treeExpanderImageIndex);

      font->StringExtent(item->GetText(),extent);
      x=this->x+offset*OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject);

      if (item->HasChildren()) {
        if (item->IsExpanded()) {
          draw->selected=true;
        }
        cross->Draw(draw,
                    x,this->y+y+(colHeight-cross->GetHeight())/2);
        draw->selected=false;
      }

      if (item==selected) {
        OS::FillRef back=OS::display->GetFill(OS::Display::tableBackgroundFillIndex);

        draw->selected=true;

        back->Draw(draw,
                   x+colHeight+OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject),
                   this->y+y+(colHeight-font->height)/2,
                   extent.width,font->height,
                   x+colHeight+OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject),
                   this->y+y+(colHeight-font->height)/2,
                   extent.width,font->height);
        draw->selected=false;
      }

      if (item==selected) {
        if (HasFocus()) {
          OS::FrameRef focus=OS::display->GetFrame(OS::Display::focusFrameIndex);

          focus->Draw(draw,
                      x+colHeight+OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject),
                      this->y+y+(colHeight-font->height)/2,
                      extent.width,font->height);
        }
        draw->PushForeground(OS::Display::fillTextColor);
      }
      else {
        draw->PushForeground(OS::Display::tableTextColor);
      }
      draw->PushFont(font,0);
      draw->DrawString(x-extent.left+colHeight+OS::display->GetSpaceHorizontal(OS::Display::spaceIntraObject),
                       this->y+y+font->ascent+(colHeight-font->height) / 2,
                       item->GetText());
      draw->PopFont();
      draw->PopForeground();
    }
  }

  void Tree::Draw(OS::DrawInfo* draw,
                  int x, int y, size_t w, size_t h)
  {
    Scrollable::Draw(draw,x,y,w,h);

    if (!OIntersect(x,y,w,h)) {
      return;
    }

    /* --- */

    if (!model.Valid() || model->GetTop()==NULL) {
      return;
    }

    size_t            offset;
    Model::Tree::Item *item;

    draw->PushClip(x,y,width,height);
    if (model->GetVisibleItem(vAdjustment->GetTop(),item,offset)) {
      size_t yPos=0;

      while (item!=NULL && yPos<height) {
        DrawItem(draw,item,yPos,offset);

        if (item->IsExpanded() && item->HasChildren()) {
          offset++;
          item=item->GetFirstChild();
        }
        else {
          while (item!=NULL && item->GetNext()==NULL) {
            item=item->GetParent();
            --offset;
          }
          if (item!=NULL) {
            item=item->GetNext();
          }
        }

        yPos+=colHeight;
      }
    }
    draw->PopClip();
  }

  void Tree::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==this->model && visible) {
      if (dynamic_cast<const Model::Tree::ItemCollapsedMsg*>(&msg)!=NULL) {
        Redraw();
        if (selected!=NULL) {
          SetSelection(selected->GetVisible());
        }
      }
      else if (dynamic_cast<const Model::Tree::ItemExpandedMsg*>(&msg)!=NULL) {
        Redraw();
      }
      else {
        Redraw();
      }
    }
    else if (model==vAdjustment->GetTopModel() && visible) {
      Redraw();
    }
    else {
      Scrollable::Resync(model,msg);

    }
  }
}

