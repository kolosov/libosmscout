#ifndef LUM_SCALE_H
#define LUM_SCALE_H

#include <Lum/Base/Size.h>

#include <Lum/OS/Display.h>

#include <Lum/Object.h>

namespace Lum {

  class LUMAPI Scale : public Object
  {
  protected:
    int            from;
    int            to;
    std::wstring   fromText;
    std::wstring   toText;
    OS::FontRef    font;
    OS::FontExtent fromExt;
    OS::FontExtent toExt;

  public:
    Scale();

    void SetInterval(int from, int to);
  };

  class LUMAPI HScale : public Scale
  {
  public:
    void CalcSize();
    void Draw(OS::DrawInfo* draw,
              int x, int y, size_t w, size_t h);
  };

  class LUMAPI VScale : public Scale
  {
    void CalcSize();
    void Draw(OS::DrawInfo* draw,
              int x, int y, size_t w, size_t h);
  };
}

#endif
