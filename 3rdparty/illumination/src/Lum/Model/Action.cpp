#include <Lum/Model/Action.h>

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

namespace Lum {
  namespace Model {

    Action::Action()
    : state(inactive)
    {
      // no code
    }

    Action::~Action()
    {
      if (state!=inactive) {
        state=inactive;
        Notify();
      }
    }

    /**
      Returns the current state of the action object
    */
    Action::State Action::GetState() const
    {
      return state;
    }

    /**
      Returns true, if the action object is currently inactive
    */
    bool Action::IsInactive() const
    {
      return state==inactive;
    }

    /**
      Returns true, if the action is started
    */
    bool Action::IsStarted() const
    {
      return state==started;
    }

    /**
      Returns true, if the action was canceled
    */
    bool Action::IsCanceled() const
    {
      return state==canceled;
    }

    /**
      Returns true, if the action is done
    */
    bool Action::IsFinished() const
    {
      return state==finished;
    }

    /**
      Starts the action
    */
    void Action::Start()
    {
      assert(state==inactive);

      state=started;
      Notify();
    }

    /**
      Cancels the current action
    */
    void Action::Cancel()
    {
      assert(state==started);

      state=canceled;
      Notify();
      state=inactive;
      Notify();
    }

    /**
      Finishes the current action
    */
    void Action::Finish()
    {
      assert(state==started);

      state=finished;
      Notify();
      state=inactive;
      Notify();
    }

    /**
      Starts the action and then imediately finishes it.
    */
    void Action::Trigger()
    {
      if (state==inactive) {
        Start();
        Finish();
      }
      else if (state==started) {
        state=finished;
        Notify();
        state=started;
        Notify();
      }
    }

    /**
      Starts the action and then imediately finishes it.
    */
    void Action::Trigger(const Base::ResyncMsg& msg)
    {
      if (state==inactive) {
        state=started;
        Notify(msg);
	state=finished;
        Notify(msg);
        state=inactive;
        Notify(msg);
      }
      else if (state==started) {
        state=finished;
        Notify(msg);
        state=started;
        Notify(msg);
      }
    }

  }
}
