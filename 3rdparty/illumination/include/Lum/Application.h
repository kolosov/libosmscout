#ifndef LUM_APPLICATION_H
#define LUM_APPLICATION_H

/*
  This source is part of the Illumination library
  Copyright (C) 2007  Tim Teulings

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
#include <Lum/Private/ImportExport.h>

#include <string>
#include <vector>

namespace Lum {
  /**
    Base class for main event loops defining fundamental order
    of program initialisation and deinitialisation.

    General execution order:
    * Initialize() - calls Cleanup() on error and then quits
    * ReadConfiguration() - calls Clanup() on error and then quits
    * Main() - calls Clanup() on error and then quits
    * Cleanup()

    Runtime callbacks:
    * OnDumpStatus()
  */
  class LUMAPI Application
  {
  protected:
    Application();

    virtual bool Initialize() = 0;
    virtual bool Main() = 0;
    virtual void Cleanup() = 0;

  public:
    virtual ~Application();

    virtual void SetApplicationName(const std::wstring& appName) = 0;
    virtual void SetArguments(int argc, char* argv[]) = 0;

    virtual std::wstring GetApplicationName() const = 0;
    virtual std::wstring GetProgramName() const = 0;

    virtual const std::vector<std::wstring>& GetArguments() const = 0;
    virtual size_t GetArgumentCount() const = 0;
    virtual std::wstring GetArgument(size_t index) const = 0;

    virtual bool Run() = 0;

    static Application* Instance();
  };

  class LUMAPI BaseApplication : public Application
  {
  private:
    std::wstring              appName;
    std::wstring              progName;
    std::vector<std::wstring> args;

  protected:
    bool Initialize();
    void Cleanup();

  public:
    void SetApplicationName(const std::wstring& appName);
    void SetArguments(int argc, char* argv[]);

    std::wstring GetApplicationName() const;
    std::wstring GetProgramName() const;

    const std::vector<std::wstring>& GetArguments() const;
    size_t GetArgumentCount() const;
    std::wstring GetArgument(size_t index) const;

    bool Run();
  };

  class LUMAPI IlluminationApplication : public BaseApplication
  {
  protected:
    bool Initialize();
    bool Prepare();
    void Cleanup();

  public:
    IlluminationApplication();
  };

  template<class D>
  class GUIApplication : public IlluminationApplication
  {
  protected:
    bool Main()
    {
      D window;

      if (window.Open()) {
        window.SetExitAction(window.GetClosedAction());
        window.EventLoop();
        window.Close();

        return true;
      }
      else {
        return false;
      }
    }
  };

#define LUM_MAIN(MAIN_CLASS,APP_NAME) \
  int main(int argc, char* argv[])\
  {\
    MAIN_CLASS mainClass;\
    \
    mainClass.SetApplicationName(APP_NAME);\
    mainClass.SetArguments(argc,argv);\
    \
    if (mainClass.Run()) {\
      return 0;\
    }\
    else {\
      return 1;\
    }\
  }
}

#endif
