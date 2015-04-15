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

#include <Lum/Application.h>

#include <cassert>

#include <Lum/Base/String.h>

#include <Lum/OS/Probe.h>

namespace Lum {
  static Application* instance=NULL;

  Application::Application()
  {
    assert(instance==NULL);

    instance=this;
  }

  Application::~Application()
  {
    if (instance==this) {
      instance=NULL;
    }
  }

  Application* Application::Instance()
  {
    return instance;
  }

  void BaseApplication::SetApplicationName(const std::wstring& appName)
  {
    this->appName=appName;
  }

  void BaseApplication::SetArguments(int argc, char* argv[])
  {
    this->progName=Lum::Base::StringToWString(argv[0]);

    if (argc>1) {
      args.resize(argc-1);

      for (int i=1; i<argc; i++) {
        args[i-1]=Lum::Base::StringToWString(argv[i]);
      }
    }
  }

  std::wstring BaseApplication::GetApplicationName() const
  {
    return appName;
  }

  std::wstring BaseApplication::GetProgramName() const
  {
    return progName;
  }

  const std::vector<std::wstring>& BaseApplication::GetArguments() const
  {
    return args;
  }

  size_t BaseApplication::GetArgumentCount() const
  {
    return args.size();
  }

  std::wstring BaseApplication::GetArgument(size_t index) const
  {
    assert(index<args.size());

    return args[index];
  }

  bool BaseApplication::Initialize()
  {
    return true;
  }

  bool BaseApplication::Run()
  {
    if (!Initialize()) {
      Cleanup();
      return false;
    }

    if (!Main()) {
      Cleanup();
      return false;
    }

    Cleanup();

    return true;
  }


  void BaseApplication::Cleanup()
  {
    // no code
  }

  IlluminationApplication::IlluminationApplication()
  {
    Lum::OS::prober.Instance();
  }

  bool IlluminationApplication::Initialize()
  {
    if (!BaseApplication::Initialize()) {
      return false;
    }

    return Lum::OS::prober->Open(GetApplicationName(),
                                 GetProgramName(),
                                 GetArguments());
  }

  void IlluminationApplication::Cleanup()
  {
    Lum::OS::prober->Close();

    BaseApplication::Cleanup();
  }
}
