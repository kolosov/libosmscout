#ifndef LUM_IMAGES_WRITER_H
#define LUM_IMAGES_WRITER_H

/*
  This source is part of the Illumination library
  Copyright (C) 2006  Tim Teulings

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

#include <list>
#include <string>

#include <Lum/Private/ImportExport.h>

#include <Lum/Base/Singleton.h>

#include <Lum/Images/Image.h>

namespace Lum {
  namespace Images {

    /**
      Common options, that can be handed over to a Writer
      implementation.

      Note, that the values of Options are always proposals.
      The image writing function is always free to ignore the
      options because limitations of the Writer itself or the format.
    */
    class LUMAPI WriterOptions
    {
    private:
      size_t quality; /** quality, between 0 (min) and 100 (max and default). */

    public:
      WriterOptions();

      size_t GetQuality() const;
      void SetQuality(size_t quality);
    };
  }
}

#endif
