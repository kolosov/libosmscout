#ifndef LUM_AUDIO_PLAYSOUND_H
#define LUM_AUDIO_PLAYSOUND_H

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

namespace Lum {
  namespace Audio {
    /**
      Class to play sounds.
    */
    class LUMAPI PlaySound : public Base::MsgObject
    {
    public:
      enum Type {
        typeAuto,
        typeWav,
        typeMp3
      };

      enum State {
        stateNull,
        stateReady,
        statePlaying,
        statePaused
      };

    public:
      PlaySound();
      virtual ~PlaySound();

      virtual State GetState() const = 0;

      virtual void SetFilename(const std::wstring& filename, Type type=typeAuto) = 0;

      virtual void Play() = 0;
      virtual void Pause() = 0;
      virtual void Stop() = 0;
    };

    extern LUMAPI PlaySound* GetPlaySound();
  }
}

#endif
