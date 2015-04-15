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

#include <Lum/Audio/PlaySoundWin32.h>

#include <Lum/OS/Win32/OSAPI.h>

namespace Lum {
  namespace Audio {

    PlaySoundWin32::PlaySoundWin32()
     : state(stateNull)
    {
      // no code
    }

    PlaySound::State PlaySoundWin32::GetState() const
    {
      return state;
    }

    void PlaySoundWin32::SetFilename(const std::wstring& filename, Type type)
    {
      this->filename=filename;
    }

    void PlaySoundWin32::Play()
    {
      if (::PlaySoundW(filename.c_str(),NULL,SND_ASYNC|SND_FILENAME)) {
        state=statePlaying;
      }
      else {
        state=stateNull;
      }
    }

    void PlaySoundWin32::Pause()
    {
      // no code
    }

    void PlaySoundWin32::Stop()
    {
      ::PlaySoundW(NULL,NULL,SND_ASYNC|SND_FILENAME);
      state=stateNull;
    }
  }
}
