#include <Lum/OS/Win32/Image.h>

#include <Lum/OS/Win32/Display.h>
#include <Lum/OS/Win32/DrawInfo.h>

#include <iostream>

namespace Lum {
  namespace OS {
    namespace Win32 {

      Image::Image()
      : dc(0),bitmap(0),data(NULL)
      {
        // no code
      }

      Image::~Image()
      {
        FreeOSStructures();
      }

      void Image::GetBitmapHeader(BITMAPINFO &info)
      {
        info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth=width;
        info.bmiHeader.biHeight=-height; /* Top-down bitmap */
        info.bmiHeader.biPlanes=1;
        info.bmiHeader.biBitCount=32;
        info.bmiHeader.biCompression=BI_RGB;
        info.bmiHeader.biSizeImage=0;
        info.bmiHeader.biXPelsPerMeter=2835; /* 72 DPI */
        info.bmiHeader.biYPelsPerMeter=835; /* 72 DPI */
        info.bmiHeader.biClrUsed=0;
        info.bmiHeader.biClrImportant=0;
      }

      struct WPixel
      {
        unsigned char b;
        unsigned char g;
        unsigned char r;
        unsigned char a;
      };

      void Image::AlphaBlend(OS::DrawInfo* draw, int x, int y, size_t w, size_t h, int dx, int dy)
      {
        HDC        tmpDC;
        HBITMAP    tmpBitmap;
        HGDIOBJ    oldBitmap;
        BITMAPINFO bi;
        LPVOID     data;

        if (w>width) {
          w=width;
        }

        if (h>height) {
          h=height;
        }

        //
        // 1. Step: Create DC and Bitmap for temporary buffer
        //

        /* Create a bitmap compatible with the output device */
        tmpDC=::CreateCompatibleDC(dynamic_cast<DrawInfo*>(draw)->dc);
        if (tmpDC==0) {
          std::cerr << "Image: Cannot CreateCompatibleDC" << std::endl;
          return;
        }

        tmpBitmap=::CreateCompatibleBitmap(dynamic_cast<DrawInfo*>(draw)->dc,w,h);
        if (tmpBitmap==0) {
          std::cerr << "Image: Cannot CreateCompatibleBitmap" << std::endl;
          /* ignore */ ::DeleteDC(tmpDC);
          return;
        }

        oldBitmap=::SelectObject(tmpDC,tmpBitmap);

        //
        // 2. Step: Copy content of window/bitmap to temporary buffer
        //

        /* Copy pixels from output device to temporary bitmap */
        /* ignore */ ::BitBlt(tmpDC,0,0,w,h,dynamic_cast<DrawInfo*>(draw)->dc,dx,dy,SRCCOPY);

        //
        // 3. Step: Get the pixel data of the temporary buffer
        //

        /* Get bitmap size */
        GetBitmapHeader(bi);
        bi.bmiHeader.biWidth=w;
        bi.bmiHeader.biHeight=-h;

        /* ignore */ ::GetDIBits(tmpDC,bitmap,0,h,NULL,&bi,DIB_RGB_COLORS);

        /* Alllocate memory */
        data=::GlobalAlloc(GMEM_FIXED,bi.bmiHeader.biSizeImage);
        if (data==NULL) {
          std::cerr << "Cannot allocate temporary image memory using GlobalAlloc" << std::endl;
          /* ignore */ ::DeleteObject(tmpBitmap);
          /* ignore */ ::DeleteDC(tmpDC);
          return;
        }

        /* Fetch pixels */
        if (::GetDIBits(tmpDC,tmpBitmap,0,h,data,&bi,DIB_RGB_COLORS)!=(int)h) {
          std::cerr << "Image:AlphaBlend: GetDIBits" << std::endl;
          /* ignore */ ::DeleteObject(tmpBitmap);
          /* ignore */ ::DeleteDC(tmpDC);
          /* ignore */ ::GlobalFree(data);
          return;
        }

        /* ignore */ SelectObject(tmpDC,oldBitmap);

        //
        // 4. Step: Now apply image with alpha channel to temporary buffer
        //

        WPixel *dest=(WPixel*)data;

        /* Perform alpha-blend of image and device bitmap */

        for (size_t k=0; k<h; k++) {  // rows
          size_t srcOff,destOff;

          srcOff=(y+k)*width+x;
          destOff=k*w;

          for (size_t j=0; j<w; j++) { // column
            int sr,sg,sb,dr,dg,db,a,a2;

            /* source: our image data */
            sr=current[srcOff].r;
            sg=current[srcOff].g;
            sb=current[srcOff].b;
            a=current[srcOff].a;
            a2=(255-a);

            /* destination: output device bitmap data */
            dr=dest[destOff].r;
            dg=dest[destOff].g;
            db=dest[destOff].b;

            /* blend */
            dest[destOff].r=(sr*a+dr*a2)/255;
            dest[destOff].g=(sg*a+dg*a2)/255;
            dest[destOff].b=(sb*a+db*a2)/255;

            ++srcOff;
            ++destOff;
          }
        }

        /* Write back pixels */
        /* ignore */ ::SetDIBits(tmpDC,tmpBitmap,0,h,data,&bi,DIB_RGB_COLORS);

        //
        // 5. Step: Copy content of temporary buffer back towindow/bitmap.
        //

        oldBitmap=::SelectObject(tmpDC,tmpBitmap);
        /* ignore */ ::BitBlt(dynamic_cast<DrawInfo*>(draw)->dc,dx,dy,w,h,tmpDC,0,0,SRCCOPY);
        /* ignore */ SelectObject(tmpDC,oldBitmap);

        //
        // 6. Step: Free everything
        //

        /* ignore */ ::DeleteObject(tmpBitmap);
        /* ignore */ ::DeleteDC(tmpDC);
        /* ignore */ ::GlobalFree(data);
      }

      void Image::CreateOSStructures()
      {
        BITMAPINFO info;
        Display    *d;
        LPVOID     dataPtr;

        if (source==NULL) {
          return;
        }

        d=dynamic_cast<Display*>(display);
        dc=CreateCompatibleDC(d->hdc);

        if (dc==0) {
          std::cerr << "Image: Cannot CreateCompatibleDC" << std::endl;
          return;
        }

        GetBitmapHeader(info);

        bitmap=CreateDIBSection(dc,&info,DIB_RGB_COLORS,&dataPtr,0,0);
        if (bitmap==0) {
          std::cerr << "Image: Failed to CreateDIBSection" << std::endl;
          /* ignore */ ::DeleteDC(dc);
          dc=0;
          return;
        }

        data=(unsigned char*)dataPtr;
        if (data==NULL) {
          std::cerr << "Image: No pixel data" << std::endl;
          /* ignore */ ::DeleteDC(dc);
          /* ignore */ ::DeleteObject(bitmap);
          bitmap=0;
          dc=0;
          return;
        }

        size_t off=0;

        for (size_t x=0; x<width*height; x++) {
          data[off]=current[x].b;
          ++off;
          data[off]=current[x].g;
          ++off;
          data[off]=current[x].r;
          ++off;
          data[off]=0x0;
          ++off;
        }

        /* ignore */ SelectObject(dc,bitmap);
      }

      void Image::FreeOSStructures()
      {
        if (bitmap!=0) {
          /* ignore */ DeleteObject(bitmap);
          bitmap=0;
        }

        if (dc!=0) {
          /* ignore */ DeleteDC(dc);
          dc=0;
        }
      }

      void Image::Draw(OS::DrawInfo* draw, int x, int y)
      {
        if (bitmap==0) {
          CreateOSStructures();
        }

        if (bitmap==0) {
          return;
        }

        if (alpha) {
          AlphaBlend(draw,0,0,width,height,x,y);
        }
        else {
          /* ignore */ BitBlt(dynamic_cast<DrawInfo*>(draw)->dc,x,y,width,height,dc,0,0,SRCCOPY);
        }
      }

      void Image::DrawSub(OS::DrawInfo* draw, int x, int y, size_t w, size_t h, int dx, int dy)
      {
        if (w==0 || h==0 || dynamic_cast<DrawInfo*>(draw)==NULL) {
          return;
        }

        if (bitmap==0) {
          CreateOSStructures();
        }

        if (bitmap==0) {
          return;
        }

        if (alpha) {
          AlphaBlend(draw,x,y,width,height,dx,dy);
        }
        else {
          /* ignore */ BitBlt(dynamic_cast<DrawInfo*>(draw)->dc,dx,dy,width,height,dc,x,y,SRCCOPY);
        }
      }

      void Image::DrawSubCliped(OS::DrawInfo* draw, int x, int y, size_t w, size_t h, int dx, int dy)
      {
        DrawSub(draw,x,y,w,h,dx,dy);
      }

      void Image::DrawTiled(OS::DrawInfo* draw, int x, int y, size_t w, size_t h, int /*dx*/, int /*dy*/)
      {
        if (bitmap==0) {
          CreateOSStructures();
        }

        if (bitmap==0) {
          return;
        }

        HBRUSH hbrush;
        RECT   rect;

        hbrush=CreatePatternBrush(bitmap);

        rect.left=x;
        rect.top=y;
        rect.right=x+w;
        rect.bottom=y+h;
        /* ignore */ FillRect(dynamic_cast<DrawInfo*>(draw)->dc,&rect,hbrush);

        /* ignore */ DeleteObject(hbrush);
      }

      ImageFactory::ImageFactory(OS::Display* d)
      : Factory(d)
      {
        // no code
      }

      void ImageFactory::Deinit(OS::Display* /*display*/)
      {
        // no code
      }

      Images::Image* ImageFactory::CreateImage() const
      {
        return new Image();
      }
    }
  }
}
