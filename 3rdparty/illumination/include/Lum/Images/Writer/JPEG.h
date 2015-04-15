#ifndef LUM_IMAGES_WRITER_JPEG_H
#define LUM_IMAGES_WRITER_JPEG_H

#include <Lum/Base/Status.h>

#include <Lum/Images/Writer.h>

namespace Lum {
  namespace Images {

    extern LUMAPI ::Lum::Base::Status WriteJPEG(const std::wstring& filename,
                                                const ImageRef& image,
                                                const WriterOptions& options);
  }
}

#endif
