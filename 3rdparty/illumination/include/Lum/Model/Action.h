#ifndef LUM_MODEL_ACTION_H
#define LUM_MODEL_ACTION_H

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

#include <Lum/Base/Model.h>

namespace Lum {
  namespace Model {

    /**
    Special model that represens an action. An Action is not a point in time
    but has different internal states.

    * By default the action is in state inactive.
    * The state can then change from inactive to started.
    * The state started can be followed by finished or canceled.
    * Both states (finished and canceled) are only temporary the state of the model
      will imediately fall back to inactive.

    Every state changes triggers a notification. So if an object waits for
    an action to be finished it has to wait for a resnc event and then check the
    state of the object to be finsihed. You have to check the state while handling
    the notification since imediately afte rthis notifcation state changes back to
    inactive!
    */
    class LUMAPI Action : public Base::Model
    {
    public:
      enum State {
        inactive,
        started,
        canceled,
        finished
      };

    private:
      State state;

    public:
      Action();
      ~Action();

      // Getter
      State GetState() const;
      bool IsInactive() const;
      bool IsStarted() const;
      bool IsCanceled() const;
      bool IsFinished() const;

      // Setter
      void Start();
      void Cancel();
      void Finish();
      void Trigger();
      void Trigger(const Base::ResyncMsg& msg);
    };

#if defined(LUM_INSTANTIATE_TEMPLATES)
    LUM_EXPTEMPL template class LUMAPI Lum::Base::Reference<Action>;
#endif

    typedef Base::Reference<Action> ActionRef;
  }
}

#endif
