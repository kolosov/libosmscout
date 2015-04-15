#ifndef LUM_IMAGES_LOADER_PNG_H
#define LUM_IMAGES_LOADER_PNG_H

#include <Lum/Images/Loader.h>

namespace Lum {
  namespace Images {

    class LUMAPI LoaderPNG : public Loader
    {
    public:
      bool CanLoad(const std::wstring& filename) const;

      bool Load(const std::wstring& filename,
                Image* image) const;
    };
  }
}

#endif
