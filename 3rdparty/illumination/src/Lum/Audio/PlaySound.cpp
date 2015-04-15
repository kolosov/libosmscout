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

#include <Lum/Audio/PlaySound.h>

#include <Lum/Private/Config.h>

#include <Lum/Audio/PlaySoundNull.h>

#if defined(HAVE_LIB_GSTREAMER)
  #include <Lum/Audio/PlaySoundGst.h>
#endif

#if defined(__WIN32__) || defined(WIN32)
  #include <Lum/Audio/PlaySoundWin32.h>
#endif

namespace Lum {
  namespace Audio {
    PlaySound::PlaySound()
    {
      // no code
    }

    PlaySound::~PlaySound()
    {
      // no code
    }

    PlaySound* GetPlaySound()
    {
      PlaySound *result=NULL;
#if defined(HAVE_LIB_GSTREAMER)
      if (result==NULL) {
        result=new PlaySoundGst();
      }
#endif
#if defined(__WIN32__) || defined(WIN32)
      if (result==NULL) {
        result=new PlaySoundWin32();
      }
#endif
      if (result==NULL) {
        result=new PlaySoundNull();
      }

      return result;
    }
  }
}
