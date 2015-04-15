#ifndef LUM_BASE_PATH_H
#define LUM_BASE_PATH_H

/*
  This source is part of the Illumination library
  Copyright (C) 2005  Tim Teulings

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

#include <vector>
#include <string>

#include <Lum/Private/ImportExport.h>

#include <Lum/Base/DateTime.h>
#include <Lum/Base/Reference.h>
#include <Lum/Base/Status.h>

namespace Lum {
  namespace Base {

    /**
     \brief A class for abstracting pathes to directories and files.

     This class help dealing with pathes to directories and files names. It tries
     to abstract plattform specific syntax for pathes and tries to abstract them
     to make handling of pathes plattform independent.

     Pathes currently have the following features:
     - They can be absokute or relative
     - They consist of a number of directory parts in sum building the complete directory
     - An optional filename, which make the Path a path to a file instead of a directory
     - Adding a directory part
     - Going up in the directory hierachie
     - Some test functions against real filesystem
     - Access to some special pathes
    */
    class LUMAPI Path
    {
    private:
      std::vector<std::wstring> parts;
      std::wstring              root;
      std::wstring              file;

    public:
      Path(const std::wstring& nativePath=L"");
      virtual ~Path();

      // Getter
      bool IsEmpty() const;
      bool IsAbsolute() const;
      bool IsDir() const;
      bool Exists() const;
      bool IsRoot() const;
      std::wstring GetRoot() const;
      std::wstring GetDir(bool trailingSeparator=false) const;
      std::wstring GetSubDir(size_t pos,bool trailingSeparator=false) const;
      std::wstring GetBaseName() const;
      std::wstring GetPath() const;
      size_t GetPartCount() const;
      std::wstring GetPart(size_t pos) const;

      // Setter
      void SetNativePath(const std::wstring& nativePath);
      void SetNativeDir(const std::wstring& dir);
      void SetNativeDirAndFile(const std::wstring& dir, const std::wstring& file);
      void SetRoot(const std::wstring& root);
      void SetDir(const std::wstring& dir);
      void AppendDir(const std::wstring& dir);
      void SetBaseName(const std::wstring& baseName);
      bool GoUp();
      Status CreateDir();
      Status CreateDirRecursive();

      static bool Exists(const std::wstring& nativePath);
      static bool IsDir(const std::wstring& nativePath);
      static Status GetAbsoluteFilename(const std::wstring& fileName, std::wstring& absoluteFilename);

      static std::wstring GetCWD();
      static std::wstring GetApplicationConfigPath();

      static Status CreateDir(const std::wstring& nativePath);
      static Status RemoveDir(const std::wstring& nativePath);
      static Status RemoveFile(const std::wstring& nativePath);

      friend LUMAPI bool operator!=(const Path& a, const Path& b);
    };

    extern LUMAPI bool operator!=(const Path& a, const Path& b);

    /**
      Class for scanning of directory contents.
     */
    class LUMAPI DirScanner : public Referencable
    {
    public:
      /**
        Possisble access rights
      */
      enum Right
      {
        rightRead    = 1 << 0,
        rightWrite   = 1 << 1,
        rightExecute = 1 << 2
      };

      /**
        Values of an found directory entry
      */
      class LUMAPI Entry
      {
      public:
        std::wstring     name;
        bool             isDir;
        bool             isSoftLink;
        bool             isHardLink;
        bool             isHidden;

        unsigned long    size;
        unsigned long    blockSize;
        unsigned long    linkId;
        SystemTime       timeModification;

        unsigned short   user;
        unsigned short   group;
        unsigned short   other;
      };

    protected:
      Path path;

    public:
      /**
        Factory method for creating a scanner for the current
        operating system.
       */
      static DirScanner* Create(const Path& path);

    protected:
      DirScanner(const Path& path);

    public:
      virtual ~DirScanner();

      /**
        Scan for the next (the first) entry. Returns false, if there
        are no more entries. In this case check status, if this was the natural
        end of the directory or if there was some error while scanning.
      */
      virtual bool GetNext(Entry& entry, Status& status) = 0;
    };

    typedef Reference<DirScanner> DirScannerRef;

  }
}

#endif
