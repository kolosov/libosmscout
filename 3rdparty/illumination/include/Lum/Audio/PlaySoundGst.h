#ifndef LUM_AUDIO_PLAYSOUNDGST_H
#define LUM_AUDIO_PLAYSOUNDGST_H

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

#include <gst/gst.h>

#include <Lum/Audio/PlaySound.h>

#include <Lum/Model/Action.h>

namespace Lum {
  namespace Audio {

    /**
      Implementation of the Playsound interface using gstreamer.
    */
    class LUMAPI PlaySoundGst : public PlaySound
    {
    private:
      std::wstring            filename;
      Type                    type;
      ::GstElement            *bin;
      ::GSource               *gsource;

    public:
      ::GstState              state;

    public:
      PlaySoundGst();
      virtual ~PlaySoundGst();

      virtual State GetState() const;

      virtual void SetFilename(const std::wstring& filename, Type type=typeAuto);

      virtual void Play();
      virtual void Pause();
      virtual void Stop();
    };
  }
}

#endif
