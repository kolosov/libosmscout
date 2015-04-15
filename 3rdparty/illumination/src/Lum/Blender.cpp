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

#include <Lum/Blender.h>

#include <Lum/OS/Theme.h>

#include <Lum/Button.h>
#include <Lum/Image.h>

namespace Lum {

  /**
    Hidden class used internally by Blender for display of pseudo dialogs.
  */
  class BlenderEntry : public Group
  {
  private:
    Blender *blender;
    Object  *main;
    Button  *close;
    bool    isTop;

  public:
    BlenderEntry(Blender* blender, Object* main, bool isTop);
    virtual ~BlenderEntry();

    void SetMain(Object* object);

    bool VisitChildren(Visitor &visitor, bool onlyVisible);

    void CalcSize();
    void Layout();
  };


  BlenderEntry::BlenderEntry(Blender* blender, Object* main, bool isTop)
  : blender(blender),main(main),close(new Button()),isTop(isTop)
  {
    assert(main!=NULL);

    close->SetParent(this);

    if (OS::display->GetTheme()->GetStockImage(OS::Theme::imageClose)!=NULL) {
      Image *image;
      image=new Image();
      image->SetImage(OS::display->GetTheme()->GetStockImage(OS::Theme::imageClose));
      close->SetLabel(image);
    }
    else {
      close->SetText(L"X");
    }

    if (isTop) {
      close->SetModel(blender->GetCloseTopAction());
    }
    else {
      close->SetModel(blender->GetCloseBottomAction());
    }

    main->SetParent(this);
    // TODO: Reset background and frame
  }

  BlenderEntry::~BlenderEntry()
  {
    delete close;
    delete main;
  }

  bool BlenderEntry::VisitChildren(Visitor &visitor, bool /*onlyVisible*/)
  {
    if (!visitor.Visit(main)) {
      return false;
    }

    if (!visitor.Visit(close)) {
      return false;
    }

    return true;
  }

  void BlenderEntry::CalcSize()
  {
    close->CalcSize();
    main->CalcSize();

    width=std::max(main->GetOWidth(),close->GetOWidth());
    height=close->GetOHeight()+
           OS::display->GetSpaceVertical(OS::Display::spaceInterObject)+
           main->GetOHeight();

    // We do not want to resize it (yet)
    minWidth=width;
    minHeight=height;

    Group::CalcSize();
  }

  void BlenderEntry::Layout()
  {
    close->Move(x,y);
    close->Layout();
    main->Resize(width,
                 height-close->GetOHeight()-OS::display->GetSpaceVertical(OS::Display::spaceInterObject));
    main->Move(x,y+close->GetOHeight()+OS::display->GetSpaceVertical(OS::Display::spaceInterObject));
    main->Layout();

    Group::Layout();
  }

  Blender::Blender()
  : top(NULL),main(NULL),bottom(NULL),
     closeTopAction(new Model::Action()),closeBottomAction(new Model::Action())
  {
    Observe(closeTopAction);
    Observe(closeBottomAction);
  }

  Blender::~Blender()
  {
    std::map<std::wstring,Object*>::iterator iter;

    top=NULL;
    iter=topMap.begin();
    while (iter!=topMap.end()) {
      delete iter->second;

      ++iter;
    }

    delete main;

    bottom=NULL;
    iter=bottomMap.begin();
    while (iter!=bottomMap.end()) {
      delete iter->second;

      ++iter;
    }
  }

  /**
    Add an object to the list of blend in dialogs displayed at the top of the
    blender control.
  */
  void Blender::AddTop(const std::wstring& name, Object* object)
  {
    //assert(!inited);
    assert(object!=NULL);

    object=new BlenderEntry(this,object,true);
    object->SetParent(this);
    topMap[name]=object;

    // Temporary hack!
    if (inited) {
      object->CalcSize();
    }
  }

  /**
    @return true, if the object with name \name is a registered blend in at top dialog.
  */
  bool Blender::HasTop(const std::wstring& name) const
  {
    return topMap.find(name)!=topMap.end();
  }

  /**
    Open the blend in dialog with the given \p name at the top of the blender control.
  */
  void Blender::SetTop(const std::wstring& name)
  {
    top=topMap[name];
    layouted=false;
    Redraw();
  }

  /**
    Hide a potential open blend in dialog at the top.
  */
  void Blender::HideTop()
  {
    if (top!=NULL) {
      top->Hide();
      top=NULL;
      layouted=false;
      Redraw();
    }
  }

  /**
    Set the main object that might be surrounded by the blend in dialogs at the top and bottom.
  */
  void Blender::SetMain(Object* object)
  {
    assert(object!=NULL);

    delete main;
    object->SetParent(this);
    main=object;
  }

  /**
    Add an object to the list of blend in dialogs displayed at the bottom of the
    blender control.
  */
  void Blender::AddBottom(const std::wstring& name, Object* object)
  {
    //assert(!inited);
    assert(object!=NULL);

    object=new BlenderEntry(this,object,false);
    object->SetParent(this);
    bottomMap[name]=object;

    // Temporary hack!
    if (inited) {
      object->CalcSize();
    }
  }

  /**
    @return true, if the object with name \name is a registered blend in at bottom dialog.
  */
  bool Blender::HasBottom(const std::wstring& name) const
  {
    return bottomMap.find(name)!=bottomMap.end();
  }

  /**
    Open the blend in dialog with the given \p name at the bottom of the blender control.
  */
  void Blender::SetBottom(const std::wstring& name)
  {
    bottom=bottomMap[name];
    layouted=false;
    Redraw();
  }

  /**
    Hide a potential open blend in dialog at the bottom.
  */
  void Blender::HideBottom()
  {
    if (bottom!=NULL) {
      bottom->Hide();
      bottom=NULL;
      layouted=false;
      Redraw();
    }
  }

  void Blender::HideBoth()
  {
    if (top!=NULL || bottom!=NULL) {
      if (top!=NULL) {
        top->Hide();
        top=NULL;
      }

      if (bottom!=NULL) {
        bottom->Hide();
        bottom=NULL;
      }

      layouted=false;
      Redraw();
    }
  }

  Model::Action* Blender::GetCloseTopAction() const
  {
    return closeTopAction.Get();
  }
  Model::Action* Blender::GetCloseBottomAction() const
  {
    return closeBottomAction.Get();
  }

  bool Blender::VisitChildren(Visitor &visitor, bool onlyVisible)
  {
    if (!onlyVisible) {
      for (std::map<std::wstring,Object*>::iterator iter=topMap.begin();
           iter!=topMap.end();
           ++iter) {
        if (!visitor.Visit(iter->second)) {
          return false;
        }
      }

      for (std::map<std::wstring,Object*>::iterator iter=bottomMap.begin();
           iter!=bottomMap.end();
           ++iter) {
        if (!visitor.Visit(iter->second)) {
          return false;
        }
      }
    }
    else {
      if (top!=NULL &&
          !visitor.Visit(top)) {
        return false;
      }

      if (bottom!=NULL &&
          !visitor.Visit(bottom)) {
        return false;
      }
    }

    if (!visitor.Visit(main)) {
      return false;
    }

    return true;
  }

  void Blender::CalcSize()
  {
    std::map<std::wstring,Object*>::iterator iter;
    size_t                                   topHeight,bottomHeight;

    minWidth=0;
    minHeight=0;

    width=0;
    height=0;

    topHeight=0;
    iter=topMap.begin();
    while (iter!=topMap.end()) {
      iter->second->CalcSize();
      minWidth=std::max(minWidth,iter->second->GetOMinWidth());
      width=std::max(width,iter->second->GetOWidth());
      topHeight=std::max(topHeight,iter->second->GetOHeight());

      ++iter;
    }

    bottomHeight=0;
    iter=bottomMap.begin();
    while (iter!=bottomMap.end()) {
      iter->second->CalcSize();
      minWidth=std::max(minWidth,iter->second->GetOWidth());
      width=std::max(width,iter->second->GetOWidth());
      bottomHeight=std::max(bottomHeight,iter->second->GetOHeight());

      ++iter;
    }

    main->CalcSize();
    minWidth=std::max(minWidth,main->GetOMinWidth());
    width=std::max(width,main->GetOWidth());
    minHeight=std::max(main->GetOHeight(),topHeight+main->GetOMinHeight()+bottomHeight);
    height=minHeight;

    Group::CalcSize();
  }

  void Blender::Layout()
  {
    int    yPos;
    size_t h;

    yPos=y;
    h=height;

    if (top!=NULL) {
      yPos+=top->GetOHeight();
      h-=top->GetOHeight();
      top->MoveResize(x,y,width,top->GetOHeight());
      top->Layout();
    }

    if (bottom!=NULL) {
      h-=bottom->GetOHeight();
      bottom->MoveResize(x,y+height-bottom->GetOHeight(),width,bottom->GetOHeight());
      bottom->Layout();
    }

    main->MoveResize(x,yPos,width,h);
    main->Layout();

    Group::Layout();
  }

  void Blender::Resync(Base::Model* model, const Base::ResyncMsg& msg)
  {
    if (model==closeTopAction && closeTopAction->IsFinished()) {
      HideTop();
    }
    else if (model==closeBottomAction && closeBottomAction->IsFinished()) {
      HideBottom();
    }

    Group::Resync(model,msg);
  }

}

