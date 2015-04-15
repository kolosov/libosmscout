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

#include <Lum/Base/Model.h>

#include <functional>
#include <set>

namespace Lum {
  namespace Base {

    ResyncMsg::~ResyncMsg()
    {
      // no code
    }

    /**
      MsgObject inherits the Init-function from Object.
    */
    MsgObject::MsgObject()
    : id(0),name(L"")
    {
      // no code
    }

    MsgObject::~MsgObject()
    {
      while (models.size()>0) {
        models.begin()->Get()->RemoveObject(this);
        models.erase(models.begin());
      }
    }

    /**
      We must have the ability to set the id of an object
    */
    void MsgObject::SetId(int id)
    {
      this->id=id;
    }

    /**
      You can give an object a name. This can be usefull for automatically
      matching models (which also can have a name) with objects.
    */
    void MsgObject::SetName(std::wstring name)
    {
      this->name=name;
    }

    /**
      Return the previously assigned id using SetId or "0" if no id was assigned.
    */
    int MsgObject::GetId() const
    {
      return id;
    }

    /**
      Return the previously assigned name using SetName or the empty string if no name
      was assigned.
    */
    std::wstring MsgObject::GetName() const
    {
      return name;
    }

    /**
      Use this function to attach an MsgObject to the given \p model.

      Normaly you should not call this method directly, the
      object should offer special methods for this.
    */
    void MsgObject::Observe(Model* model)
    {
      assert(model!=NULL);

      model->AddObject(this);

      models.push_back(model);
    }

    /**
      Use this function to attach an MsgObject to the given \p model.

      Normaly you should not call this method directly, the
      object should offer special methods for this.
    */
    void MsgObject::ObservePrior(Model* model)
    {
      assert(model!=NULL);

      model->PrependObject(this);

      models.push_front(model);
    }

    /**
      Deattach the MsgObject from the given \p model.
    */
    void MsgObject::Forget(Model* model)
    {
      assert(model!=NULL);

      model->RemoveObject(this);

      for (std::list<ModelRef>::iterator iter=models.begin(); iter!=models.end(); ++iter) {
        if (iter->Get()==model) {
          models.erase(iter);
          return;
        }
      }

      // We must not come this way, since Attach/Unattach must occure in pairs!
      assert(false);
    }

    /**
      This method gets called when a model wants you to resync yourself with
      its contents.
    */
    void MsgObject::Resync(Model* /*model*/, const ResyncMsg& /*msg*/)
    {
      // no code
    }

    /**
      Initializes an freshly allocated Model. You must call this method
      right after allocation and before any other method call.

      The model is by default switched on.
    */
    Model::Model()
    : depth(0),
      off(0),
      enabled(true)
    {
      // no code
    }

    Model::~Model()
    {
      // There is something wrong in your ModelRef handling if a models is freed
      // but still has watchers!
      assert(watchers.size()==0);
    }

    /**
      Signal that changes of the model are allowed. This will trigger
      a notify.
     */
    void Model::Enable()
    {
      if (!enabled) {
        enabled=true;

        Notify();
      }
    }

    /**
      Signal that changes of the model are not allowed. The model
      is still changeable by direct calls but GUI elements should react
      on this state change and forbit changes. This will trigger a notify.
     */
    void Model::Disable()
    {
      if (enabled) {
        enabled=false;

        Notify();
      }
    }

    /**
      Return if model is enabled (true) or disabled (false).
     */
    bool Model::IsEnabled() const
    {
      return enabled;
    }

    /**
      Return if model is disabled (true) or enabled (false).
     */
    bool Model::IsDisabled() const
    {
      return !enabled;
    }

    /**
      Certain models may support the notion of having a null value, which has to be
      understand as "no value set". The default implementation always returns
      false.
    */
    bool Model::IsNull() const
    {
      return false;
    }

    /**
      Make a new working copy of the current value. The original value can be made
      available again by calling Model::Undo. It can be overwritten by
      calling Model::Save and the current copy can we thrown away by calling
      Model::Pop.

      Using these method syou can implement backup/restore semantics
      (normally by using some value stack).

      A Model does not need to implement these methods, for some models
      this would be difficult or just would not make sense (like lists and
      table models), however since some code relies on it, it should clearly
      mark this fact.
    */
    void Model::Push()
    {
      depth++;
    }

    /**
      See Push.
    */
    void Model::Undo()
    {
      // no code
    }

    /**
      See Push.
    */
    void Model::Save()
    {
      // no code
    }

    /**
      See Push.
    */
    void Model::Pop()
    {
      assert(depth>0);

      depth--;
    }

    /**
      Add an object (viewer) to a model
    */
    void Model::AddObject(MsgObject* object)
    {
      assert(object!=NULL);

      WatcherEntry entry;

      entry.object=object;
      entry.depth=depth;

      watchers.push_back(entry);

      if (IsOn()) {
        ResyncMsg msg;

        object->Resync(this,msg);
      }
    }

    /**
      Add an object (viewer) to a model
    */
    void Model::PrependObject(MsgObject* object)
    {
      assert(object!=NULL);

      WatcherEntry entry;

      entry.object=object;
      entry.depth=depth;

      watchers.push_front(entry);

      if (IsOn()) {
        ResyncMsg msg;

        object->Resync(this,msg);
      }
    }

    /**
      Remove an object from an model.
    */
    void Model::RemoveObject(MsgObject* object)
    {
      assert(object!=NULL);

      for (std::list<WatcherEntry>::iterator iter=watchers.begin();
           iter!=watchers.end();
           ++iter) {
        if (iter->object==object) {
          watchers.erase(iter);
          return;
        }
      }

      // We must not come this way, since Add/Remove must occure in pairs!
      assert(false);
    }

    /**
      A model should call this method with an optional resynmessage
      when you want your viewers to resync themselfs.
    */
    void Model::Notify(const ResyncMsg& msg)
    {
      std::list<WatcherEntry>::iterator iter;
      std::set<MsgObject*>              signaled;

      if (off>0) {
        return;
      }

      iter=watchers.begin();
      while (iter!=watchers.end()) {
        if (iter->depth==depth && signaled.find(iter->object)==signaled.end()) {
          signaled.insert(iter->object);
          iter->object->Resync(this,msg);

          // Because triggering the model might result in watchers deregistering themself
          // from the model thus destroying the iterator, we start itertaing again over
          // the list of watcher every time a watcher has been triggered.
          // We use signaled set to skip all watchers that have already been triggered.
          iter=watchers.begin();
        }
        else {
          ++iter;
        }
      }
    }

    void Model::Notify()
    {
      ResyncMsg msg;

      Notify(msg);
    }

    /**
      Switch on communication between the Model and its
      listeners/viewers (derived from MsgObject.

      If the model was switched of before Model::Notify() will be
      called with an instance of ResyncMsg signaling a major change to the
      listeners.
      
      Calls to On() and Off() are paired. So if you call Off() two times, you
      must call On() two times, too, to get notification reactivated again.
      
      Calling On() on an already active model will trigger an assertion.
    */
    void Model::On(bool notify)
    {
      assert(off>0);
      
      off--;
      
      if (IsOn() && notify) {
        Notify();
      }
    }

    /**
      Switch off communication between the Model and its
      listeners/viewers (derived from MsgObject.

      Use this method before starting some major change to a model and do
      not want the listeners to refresh for every single action. Call
      Model::On() to turn on communication.
    */
    void Model::Off()
    {
      off++;
    }

    /**
      Tell if the Model is currently switch on or off.
    */
    bool Model::IsOn()
    {
      return off==0;
    }

    bool Model::IsObserved() const
    {
      return watchers.size()>0;
    }
  }
}

