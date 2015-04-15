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

#include <Lum/Base/Path.h>

#include <Lum/Private/Config.h>

#include <sys/stat.h>
#if defined(__WIN32__) || defined(WIN32)
  #include <windows.h>
  #include <winnt.h>
  #include <direct.h>
  #include <shlobj.h>
  #include <Lum/OS/Win32/OSAPI.h>
#else
  #include <dirent.h>
#endif

#if defined(HAVE_UNISTD_H)
  #include <unistd.h>
#endif
#if defined(HAVE_LIBGEN_H)
  #include <libgen.h>
#endif
#if defined(HAVE_SYS_TIME_H)
  #include <sys/types.h>
#endif
#include <cstdio>

#include <cctype>
#include <cstdlib>
#include <cstring>

#include <Lum/Base/L10N.h>
#include <Lum/Base/String.h>

#include <Lum/Manager/FileSystem.h>

#include <Lum/OS/Display.h>

namespace Lum {
  namespace Base {

#if defined(__WIN32__) || defined(WIN32)
    static wchar_t      nativeSeparator=L'\\';
    static std::wstring nativeSeparatorList=L"/\\";
#else
    static wchar_t      nativeSeparator=L'/';
    static std::wstring nativeSeparatorList=L"/";
#endif

    /**
      The constructor of the path
      @param nativePath An optional initial path in host native syntax.
       Internally calls SetNativePath.
    */
    Path::Path(const std::wstring& nativePath)
    {
      SetNativePath(nativePath);
    }

    Path::~Path()
    {
      // no code
    }

    /**
      Is the path empty.
      @return true, if path has no parts and no file name.
    */
    bool Path::IsEmpty() const
    {
      return parts.size()==0 && file.empty();
    }

    /**
      Is the path absolute to the root of the file sytsem or relative to some other directory.
      @return true, if the path is an absolute path.
    */
    bool Path::IsAbsolute() const
    {
      return !root.empty();
    }

    /**
      Tells if the path is a directory or a file by trying to find the path in the
      file system of the host machine.
      @return true, if path is a directory, false if it is a file or does not exists
    */
    bool Path::IsDir() const
    {
      return IsDir(GetPath());
    }

    /**
      Tell if the path does exists in the file system of the host machine. If you want to know
      if PAth is a file or a directory call IsDir.
      @return true, if the path exists
    */
    bool Path::Exists() const
    {
      return Exists(GetPath());
    }

    /**
      Tells if the path points to the root of the file system. This requires that it is
      absolute and has an empty path list.
    */
    bool Path::IsRoot() const
    {
      return !root.empty() && parts.size()==0;
    }

    /**
      Returns the root part of the path. May be empty.
    */
    std::wstring Path::GetRoot() const
    {
      return root;
    }

    /**
      Returns the directory part of the path. May be empty.
    */
    std::wstring Path::GetDir(bool trailingSeparator) const
    {
      std::wstring path;

      path.assign(root);

      for (size_t x=0; x<parts.size(); x++) {
        if (x>0) {
          path.append(1,nativeSeparator);
        }
        path.append(parts[x]);
      }

      if (parts.size()>0 && trailingSeparator) {
        path.append(1,nativeSeparator);
      }

      return path;
    }

    /**
      Returns the sub path form the 0th element upto the element at position pos.
    */
    std::wstring Path::GetSubDir(size_t pos, bool trailingSeparator) const
    {
      std::wstring path;

      path.assign(root);

      for (size_t x=0; x<=pos; x++) {
        if (x>0) {
          path.append(1,nativeSeparator);
        }
        path.append(parts[x]);
      }

      if (parts.size()>0 && trailingSeparator) {
        path.append(1,nativeSeparator);
      }

      return path;
    }

    /**
      Return the base name of the path.
    */
    std::wstring Path::GetBaseName() const
    {
      return file;
    }

    /**
      Return the directory part of the path.
    */
    std::wstring Path::GetPath() const
    {
      std::wstring path;

      path=GetDir();
      if (!file.empty()) {
        path.append(1,nativeSeparator);
        path.append(file);
      }

      return path;
    }

    /**
      Return the number of parts in the current path.
    */
    size_t Path::GetPartCount() const
    {
      return parts.size();

    }

    /**
      Return the path element at position pos. The ppath element at position
      0 is the root element.
    */
    std::wstring Path::GetPart(size_t pos) const
    {
      assert(pos<parts.size());

      return parts[pos];
    }

    /**
      Assign a new path to the current path object. This method will check
      - If the path is absolute or relative
      - If the path points to a directory or a file. If the path ends in a directory
        separator is is a directory, else it is asumed that it points to a file.
    */
    void Path::SetNativePath(const std::wstring& nativePath)
    {
      bool         isFile;
      std::wstring path;

      parts.clear();
      root.erase();
      file.erase();

      path=nativePath;

      // Empty file names are easy
      if (path.empty()) {
        return;
      }

      // If file does end in a separator, it is a directory
      if (nativeSeparatorList.find(path[path.length()-1])!=std::wstring::npos) {
        isFile=false;
      }
      else {
        // if it does not end with a separator it is a file
        isFile=true;
      }

      // Cut off file part
      if (isFile) {
        size_t end=path.length()-1;

        while (end>0 && nativeSeparatorList.find(path[end])==std::wstring::npos) {
          end--;
        }

        if (end==0 && nativeSeparatorList.find(path[0])==std::wstring::npos) {
          file=path;
          return;
        }
        else {
          file=path.substr(end+1);
          path.erase(end+1);
        }
      }

      size_t start=0;

      // if it starts with an separator it is absolute
#if defined(__WIN32__) || defined(WIN32)
      if (path.size()>=2 && path[1]==L':') {
        if (path.size()>=3 && path[2]==nativeSeparator) {
          root=path.substr(0,3);
          start=3;
        }
        else {
          start=2;
          root=path.substr(0,2);
        }
      }
#else
      if (nativeSeparatorList.find(path[start])!=std::wstring::npos) {
        root.assign(1,nativeSeparator);
        start++;
      }
#endif

      // Skip multiple path separators
      while (start<path.length() && nativeSeparatorList.find(path[start])!=std::wstring::npos) {
        start++;
      }

      // collect all directory parts
      while (start<path.length()) {
        size_t end;

        // find end of current part
        end=start+1;
        while (end<path.length() && nativeSeparatorList.find(path[end])==std::wstring::npos) {
          end++;
        }

        parts.push_back(path.substr(start,end-start));

        start=end+1;
        // Skip multiple path separators
        while (start<path.length() && nativeSeparatorList.find(path[start])!=std::wstring::npos) {
          start++;
        }
      }
    }

    void Path::SetRoot(const std::wstring& root)
    {
      this->root=root;
    }

    /**
      Set the initial directory part.
    */
    void Path::SetDir(const std::wstring& dir)
    {
      parts.clear();
      parts.push_back(dir);
    }

    /**
      Append an additional directory part to the existing path.
    */
    void Path::AppendDir(const std::wstring& dir)
    {
      parts.push_back(dir);
    }

    /**
      Assign the given dir.
    */
    void Path::SetNativeDir(const std::wstring& dir)
    {
      if (!dir.empty()) {
        if (nativeSeparatorList.find(dir[dir.length()-1])==std::wstring::npos) {
          SetNativePath(dir+nativeSeparator);
        }
        else {
          SetNativePath(dir);
        }
      }
    }

    /**
      Assign the given file in the given dir.
    */
    void Path::SetNativeDirAndFile(const std::wstring& dir, const std::wstring& file)
    {
      if (!dir.empty()) {
        if (nativeSeparatorList.find(dir[dir.length()-1])==std::wstring::npos) {
          SetNativePath(dir+nativeSeparator+file);
        }
        else {
          SetNativePath(dir+file);
        }
      }
    }

    /**
      Set a (new) filename for the path.
    */
    void Path::SetBaseName(const std::wstring& baseName)
    {
      file=baseName;
    }

    /**
      Go up one step in the directory hierachie. Automatically deletes an possibly existing
      filename part.
      @return false, if we are already at the top of the directory hierachie.
    */
    bool Path::GoUp()
    {
      if (parts.size()==0) {
        return false;
      }

      parts.pop_back();
      file.erase();

      return true;
    }

    Status Path::CreateDir()
    {
      return CreateDir(GetDir());
    }

    Status Path::CreateDirRecursive()
    {
      Status status;

      for (size_t x=0; x<GetPartCount(); x++) {
        if (!Exists(GetSubDir(x))) {
          status=CreateDir(GetSubDir(x));

          if (!status) {
            return status;
          }
        }
      }

      return status;
    }

    /**
      Returns true, if the given nativePath exists
    */
    bool Path::Exists(const std::wstring& nativePath)
    {
      struct stat info;

      return stat(WStringToString(nativePath).c_str(),&info)==0;
    }

    /**
      Returns true, if the given nativePath is a directory.
    */
    bool Path::IsDir(const std::wstring& nativePath)
    {
      struct stat info;

      if (stat(WStringToString(nativePath).c_str(),&info)==0) {
        return (info.st_mode & S_IFDIR)!=0;
        //S_ISDIR(info.st_mode);
      }
      else {
        return false;
      }
    }

    Status Path::GetAbsoluteFilename(const std::wstring& fileName,
                                     std::wstring& absoluteFilename)
    {
      Status status;

#if defined(__WIN32__) || defined(WIN32)
      wchar_t     *buffer=NULL;
      DWORD       bufSize;
      DWORD       res;

      bufSize=GetFullPathNameW(fileName.c_str(),0,NULL,NULL);

      if (bufSize==0) {
        status.SetToCurrentW32Error();
        return status;
      }

      bufSize++;

      buffer=new wchar_t[bufSize];

      res=GetFullPathNameW(fileName.c_str(),bufSize,buffer,NULL);

      if (res>0 && res<=bufSize) {
        absoluteFilename=std::wstring(buffer,res);
        delete [] buffer;
      }
      else {
        status.SetToCurrentW32Error();
        delete [] buffer;
      }

      return status;
#else
      std::string tmp,res;
      char        orgDir[10*1024];
      char        newDir[10*1024];
      char        dir[10*1024];
      char        file[10*1024];

      if (getcwd(orgDir,sizeof(orgDir))==NULL) {
        status.SetToCurrentErrno();
        return status;
      }

      tmp=WStringToString(fileName);

      strncpy(file,tmp.c_str(),sizeof(file)-1);
      strncpy(file,basename(file),sizeof(file)-1);

      strncpy(dir,tmp.c_str(),sizeof(dir)-1);
      strncpy(dir,dirname(dir),sizeof(dir)-1);

      if (chdir(dir)!=0) {
        status.SetToCurrentErrno();
        return status;
      }

      if (getcwd(newDir,sizeof(newDir))==NULL) {
        chdir(orgDir);
        status.SetToCurrentErrno();
        return status;
      }

      res=std::string(newDir)+"/"+file;

      chdir(orgDir);

      absoluteFilename=StringToWString(res);

      return status;
#endif
    }

    /**
      Returns the current working directory (which is the base for all realtive paths).
    */
    std::wstring Path::GetCWD()
    {
      char buffer[4096];

      if (getcwd(buffer,sizeof(buffer))!=NULL) {
        std::wstring res;

        res=StringToWString(buffer);
        if (!res.empty() && nativeSeparatorList.find(res[res.length()-1])==std::wstring::npos) {
          res.append(1,nativeSeparator);
        }

        return res;
      }
      else {
        return L"";
      }
    }

    std::wstring Path::GetApplicationConfigPath()
    {
      std::wstring path;

      Manager::FileSystem::Instance()->GetEntry(Manager::FileSystem::appConfigFile,path);

      return path;
    }

    Status Path::CreateDir(const std::wstring& nativePath)
    {
      Status status;

#if defined(__WIN32__) || defined(WIN32)
      std::wstring tmp(nativePath);

      tmp.insert(0,L"\\\\?\\");

      if (!CreateDirectoryW(tmp.c_str(),NULL)) {
        status.SetToCurrentW32Error();
      }

      return status;
#else
      std::string path;

      path=WStringToString(nativePath);

      if (mkdir(path.c_str(),S_IRWXU|S_IRWXG|S_IRWXO)!=0) {
        status.SetToCurrentErrno();
      }

      return status;
#endif
    }

    Status Path::RemoveDir(const std::wstring& nativePath)
    {
      Status status;

#if defined(__WIN32__) || defined(WIN32)
      assert(false);
#else
      std::string path;

      path=WStringToString(nativePath);

      if (remove(path.c_str())!=0) {
        status.SetToCurrentErrno();
      }
#endif

      return status;
    }

    Status Path::RemoveFile(const std::wstring& nativePath)
    {
      Status status;

#if defined(__WIN32__) || defined(WIN32)
      assert(false);
#else
      std::string path;

      path=WStringToString(nativePath);

      if (remove(path.c_str())!=0) {
        status.SetToCurrentErrno();
      }
#endif

      return status;
    }

    bool operator!=(const Path& a, const Path& b)
    {
      // TODO: Under Windows we have to compare case-insensitive
      return a.GetPath()!=b.GetPath();
    }

    DirScanner::DirScanner(const Path& path)
      : path(path)
    {
      // no code
    }

    DirScanner::~DirScanner()
    {
    }

#if defined(HAVE_FINDFIRSTFILEW)
    class Win32DirScanner : public DirScanner
    {
    private:
      bool   initialized;
      HANDLE handle;
      Status status;

    public:
      Win32DirScanner(const Path& path)
       : DirScanner(path),
         initialized(false),
         handle(INVALID_HANDLE_VALUE)
      {
        // no code
      }

      bool GetNext(Entry& entry, Status& status)
      {
        WIN32_FIND_DATAW data;

        if (initialized && handle==INVALID_HANDLE_VALUE) {
          status=this->status;
          return false;
        }
        else if (!initialized) {
          std::wstring tmp(path.GetPath());

          initialized=true;

          if (tmp.length()>0 && tmp[tmp.length()-1]!=L'\\'&& tmp[tmp.length()-1]!=L'/') {
            tmp.append(1,L'\\');
          }
          tmp.append(1,L'*');

          handle=FindFirstFileW(tmp.c_str(),&data);
          if (handle==INVALID_HANDLE_VALUE) {
            this->status.SetToCurrentW32Error();
            return false;
          }
        }
        else {
          if (!FindNextFileW(handle,&data)) {
            if (GetLastError()!=ERROR_NO_MORE_FILES) {
              this->status.SetToCurrentW32Error();
            }

            FindClose(handle);

            return false;
          }
        }

        entry.name=data.cFileName;
        entry.isDir=data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
        entry.isSoftLink=false;
        entry.isHardLink=false;
        entry.isHidden=data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN;

        entry.size=data.nFileSizeLow;
        entry.blockSize=0;
        entry.linkId=0;
        entry.timeModification.SetTime(data.ftLastWriteTime);

        entry.user=0;
        entry.user|=rightRead;
        if (!(data.dwFileAttributes & FILE_ATTRIBUTE_READONLY)) {
          entry.user|=rightWrite;
        }

        entry.group=0;
        entry.other=0;

        return true;
      }
    };

#else
    class UnixDirScanner : public DirScanner
    {
    private:
      bool        initialized;
      DIR         *dir;
      Status      status;
      std::string frontPath;

    public:
      UnixDirScanner(const Path& path)
       : DirScanner(path),
         initialized(false),
         dir(NULL),
         frontPath(WStringToString(path.GetPath()))
      {
        // no code
      }

      bool GetNext(Entry& entry, Status& status)
      {
        if (initialized && dir==NULL) {
          status=this->status;
          return false;
        }
        else if (!initialized) {
          initialized=true;
          dir=opendir(WStringToString(path.GetPath()).c_str());

          if (dir==NULL) {
            this->status.SetToCurrentErrno();
            status=this->status;
            return false;
          }
        }

        while (true) {
          struct dirent *entr;
          struct stat   info;
          std::string   subPath(frontPath);

          entr=readdir(dir);

          if (entr==NULL) {
            // TODO: Error case?
            closedir(dir);
            dir=NULL;
            return false;
          }

          subPath.append(1,'/');
          subPath.append(entr->d_name);

          if (lstat(subPath.c_str(),&info)==-1) {
            // We skip this entry
            continue;
          }

          entry.name=Base::StringToWString(entr->d_name);
          entry.isSoftLink=S_ISLNK(info.st_mode);
          entry.isHardLink=info.st_nlink!=1;

          if (entry.isHardLink) {
            entry.linkId=info.st_ino;
          }
          else {
            entry.linkId=0;
          }

          if (entry.isSoftLink) {
            if (stat(entr->d_name,&info)==-1) {
              // We skip this entry
              continue;
            }
          }

          entry.isDir=S_ISDIR(info.st_mode);
          entry.isHidden=entry.name.length()>0 && entry.name[0]==L'.';

          entry.size=info.st_size;
          entry.blockSize=info.st_blocks*512;
          entry.timeModification.SetTime(info.st_mtime);

          entry.user=0;
          if (info.st_mode & S_IRUSR) {
            entry.user|=rightRead;
          }
          if (info.st_mode & S_IWUSR) {
            entry.user|=rightWrite;
          }
          if (info.st_mode & S_IXUSR) {
            entry.user|=rightExecute;
          }

          entry.group=0;
          if (info.st_mode & S_IRGRP) {
            entry.group|=rightRead;
          }
          if (info.st_mode & S_IWGRP) {
            entry.group|=rightWrite;
          }
          if (info.st_mode & S_IXGRP) {
            entry.group|=rightExecute;
          }

          entry.other=0;
          if (info.st_mode & S_IROTH) {
            entry.other|=rightRead;
          }
          if (info.st_mode & S_IWOTH) {
            entry.other|=rightWrite;
          }
          if (info.st_mode & S_IXOTH) {
            entry.other|=rightExecute;
          }

          return true;
        }
      }
    };
#endif

    DirScanner* DirScanner::Create(const Path& path)
    {
#if defined(HAVE_FINDFIRSTFILEW)
      return new Win32DirScanner(path);
#else
      return new UnixDirScanner(path);
#endif
    }
  }
}
