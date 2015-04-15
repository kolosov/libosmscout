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

#include <Lum/Base/Util.h>

#include <Lum/Private/Config.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#if defined(HAVE_UNISTD_H)
  #include <unistd.h>
#endif

#include <cwchar>

#include <sstream>
#include <fstream>

#include <Lum/Base/String.h>

namespace Lum {
  namespace Base {

    int RoundRange(int x, int a, int b)
    {
      if (x<a) {
        return a;
      }
      else if (x>b) {
        return b;
      }
      else {
        return x;
      }
    }

    /**
      Rounds to the nearest integer number.
    */

    int RoundDiv(int a,int b)
    {
      int c;

      c=a/b;
      if (a%b>=b/2) {
        c++;
      }

      return c;
    }

    /**
      Rounds the number so that is is even. Currently only works correctly for
      numbers greater zero.
    */
    int RoundUpEven(int a)
    {
      if ((a / 2)>0) {
        return a+1;
      }
      else {
        return a;
      }
    }

    int UpDiv(int a,int b)
    {
      if (a / b==0) {
        if (a<0) {
          return -1;
        }
        else if (a>0) {
          return 1;
        }
        else {
          return 0;
        }
      }
      else {
        return a / b;
      }
    }

    bool CopyFile(const std::wstring& source, const std::wstring& dest)
    {
      std::ifstream *in=new std::ifstream;
      std::ofstream *out=new std::ofstream;
      size_t        size;
      char          buffer[1024];
      bool          res;

      in->open(WStringToString(source).c_str(),std::ios::in|std::ios::binary);
      if (!*in) {
        return false;
      }

      out->open(WStringToString(dest).c_str(),std::ios::out|std::ios::binary|std::ios::trunc);
      if (!*out) {
        return false;
      }

      in->seekg(0,std::ios::end);
      size=in->tellg();

      if (*in && size>0) {
        in->seekg(0,std::ios::beg);

        while (size>=1024) {
          in->read(buffer,1024);
          out->write(buffer,1024);
         size-=1024;
        }

        if (size>0) {
          in->read(buffer,size);
          out->write(buffer,size);
        }
      }

      res= *in && *out;

      in->close();
      out->close();

      delete in;
      delete out;

      return res;
    }
  }
}
