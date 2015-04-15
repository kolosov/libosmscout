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

#include <Lum/OS/Font.h>

namespace Lum {
  namespace OS {

    Font::Font()
    : features(0),
      ascent(0),descent(0),
      height(0),pixelHeight(0),pointHeight(0),
      avWidth(0),setWidth(0),
      spacing(spacingDefault)
    {
      // no code
    }

    Font::~Font()
    {
      // no code
    }

      #if 0

      /**
      Returns the named size if available. Creates and registeres it if not.

      This method is for internal use only.
      */
      /*      FontSize FontEncodingDesc::GetOrCreateSize(int size)
      {
        FontSize fontSize;

        fontSize=f.sizes;
        while ((fontSize!=NULL) && (fontSize.size!=size)) {
          fontSize=fontSize.next;
        }
        if (fontSize!=NULL) {
          return fontSize;
        }
        else {

          fontSize=new FontSize;     foundry.families:=NIL;*)
          fontSize.size=size;
          fontSize.next=f.sizes;
          f.sizes=fontSize;
          return fontSize;
        }
      }*/

    /**
      Returns the named family if available. Creates and registeres it if not.

      This method is for internal use only.
    */
    FontEncoding FontFoundryDesc::GetOrCreateEncoding(char name[])
    {
      FontEncoding encoding;

      encoding=f.encodings;
      while ((encoding!=NULL) && (encoding.encoding^!=name)) {
        encoding=encoding.next;
      }
      if (encoding!=NULL) {
        return encoding;
      }
      else {

        encoding=new FontEncoding;
        encoding.sizes=NULL;
        encoding.anySize=false;

        encoding.encoding=new Text(str.Length(name)+1);
        COPY(name,encoding.encoding^);
        encoding.next=f.encodings;
        f.encodings=encoding;
        return encoding;
      }
    }

    /**
      Returns the named foundry if available. Creates and registeres it if not.

      This method is for internal use only.
    */
    FontFoundry FontFamilyDesc::GetOrCreateFoundry(char name[])
    {
      FontFoundry foundry;

      foundry=f.foundries;
      while ((foundry!=NULL) && (foundry.name^!=name)) {
        foundry=foundry.next;
      }
      if (foundry!=NULL) {
        return foundry;
      }
      else {

        foundry=new FontFoundry;
        foundry.encodings=NULL;

        foundry.name=new Text(str.Length(name)+1);
        COPY(name,foundry.name^);
        foundry.next=f.foundries;
        f.foundries=foundry;
        return foundry;
      }
    }

    /**
      Returns the named family if available. Creates and registeres it if not.

      This method is for internal use only.
    */
    FontFamily FontListDesc::GetOrCreateFamily(char name[])
    {
      FontFamily family;

      family=f.families;
      while ((family!=NULL) && (family.name^!=name)) {
        family=family.next;
      }
      if (family!=NULL) {
        return family;
      }
      else {

        family=new FontFamily;

        family.name=new Text(str.Length(name)+1);
        COPY(name,family.name^);
        family.foundries=NULL;
        family.next=f.families;
        f.families=family;
        return family;
      }
    }

#endif

  }
}
