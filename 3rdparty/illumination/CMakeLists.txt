CMAKE_MINIMUM_REQUIRED(VERSION 2.4.7 FATAL_ERROR))

INCLUDE_DIRECTORIES(include)
INCLUDE(CheckIncludeFile)
INCLUDE(CheckFunctionExists)

INCLUDE(cmake/CheckTypeSizeCXX.cmake)
INCLUDE(cmake/CheckWin32FunctExists.cmake)

PROJECT(Illumination)

SET(CMAKE_VERBOSE_MAKEFILE TRUE)

SUBDIRS(src)

#AC_TYPE_SIZE_T

CHECK_INCLUDE_FILE(unistd.h HAVE_UNISTD_H)
CHECK_INCLUDE_FILE(pwd.h HAVE_PWD_H)
CHECK_INCLUDE_FILE(libgen.h HAVE_LIBGEN_H)
CHECK_INCLUDE_FILE(sys/time.h HAVE_SYS_TIME_H)

CHECK_FUNCTION_EXISTS(select HAVE_SELECT)
CHECK_FUNCTION_EXISTS(getpid HAVE_GETPID)
CHECK_FUNCTION_EXISTS(gettimeofday HAVE_GETTIMEOFDAY)
CHECK_FUNCTION_EXISTS(gmtime_r HAVE_GMTIME_R)
CHECK_FUNCTION_EXISTS(localtime_r LOCALTIME_R)

CHECK_FUNCTION_EXISTS(mbsrtowcs HAVE_MBSRTOWCS)
CHECK_FUNCTION_EXISTS(wcsrtombs HAVE_WCSRTOMBS)

# Windows stuff
CHECK_WIN32_FUNCTION_EXISTS(GetSystemTimeAsFileTime HAVE_GETSYSTEMTIMEASFILETIME)
CHECK_WIN32_FUNCTION_EXISTS(SleepEx HAVE_SLEEPEX)
CHECK_WIN32_FUNCTION_EXISTS(gmtime_s HAVE_GMTIME_S)
CHECK_WIN32_FUNCTION_EXISTS(localtime_s HAVE_LOCALTIME_S)

CHECK_WIN32_FUNCTION_EXISTS(FindFirstFileW HAVE_FINDFIRSTFILEW)

#AC_FUNC_SELECT_ARGTYPES

IF(CMAKE_COMPILER_IS_GNUCXX)
  ADD_DEFINITIONS("-Wall -Wpointer-arith -Wsign-compare -Wundef -Wcast-qual -Wcast-align -Wwrite-strings -Wredundant-decls -Wno-long-long")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

#AC_MSG_CHECKING([for gcc symbol visibility support])
#dnl Test for full support for std::wstring

SET(CMAKE_EXTRA_INCLUDE_FILES string)
IF (WIN32)
  SET(HAVE_LIB_WIN32 "YES")
ELSE (WIN32)
  CHECK_TYPE_SIZE_CXX("wchar_t" SIZEOF_WCHAR_T)
ENDIF (WIN32)

SET(CMAKE_EXTRA_INCLUDE_FILES)

#dnl This is the iconv macro from the gettext package,
CHECK_INCLUDE_FILE(langinfo.h HAVE_HEADER_LANGINFO)

INCLUDE(FindPkgConfig)

INCLUDE(FindThreads)

INCLUDE(FindX11)
INCLUDE(FindCurses)

pkg_check_modules(XFT xft)
pkg_check_modules(CAIRO cairo>=1.2.0)

#dnl Check for xerces
pkg_check_modules(XML libxml-2.0>=2.6.0)

INCLUDE(FindPNG)
INCLUDE(FindJPEG)
INCLUDE(FindTIFF)
INCLUDE(cmake/FindGIF.cmake)

pkg_check_modules(DBUS dbus-1>=0.61)

pkg_check_modules(HILDON hildon-libs)

pkg_check_modules(GTK gtk+-2.0>=2.6)

pkg_check_modules(GST gstreamer-0.10)

IF(CURSES_LIBRARIES)
  INCLUDE_DIRECTORIES(${CURSES_INCLUDE_DIR})
ENDIF(CURSES_LIBRARIES)

IF(X11_FOUND)
  INCLUDE_DIRECTORIES(${X11_X11_INCLUDE_PATH} ${X11_Xext_INCLUDE_PATH})
ENDIF(X11_FOUND)

IF(XFT_FOUND)
  INCLUDE_DIRECTORIES(${XFT_INCLUDE_DIRS})
  LINK_DIRECTORIES(${XFT_LIBRARY_DIR})
ENDIF(XFT_FOUND)

IF (CAIRO_FOUND)
  INCLUDE_DIRECTORIES(${CAIRO_INCLUDE_DIRS})
  LINK_DIRECTORIES(${CAIRO_LIBRARY_DIRS})
ENDIF(CAIRO_FOUND)

IF (WIN32)
  ADD_DEFINITIONS(-D_WIN32_WINNT=0x0501)
ENDIF(WIN32)

IF(APPLE)
ENDIF(APPLE)

IF(XML_FOUND)
  INCLUDE_DIRECTORIES(${XML_INCLUDE_DIRS})
  LINK_DIRECTORIES(${XML_LIBRARY_DIRS})
ENDIF(XML_FOUND)

IF(PNG_FOUND)
  INCLUDE_DIRECTORIES(${PNG_INCLUDE_DIR})
ENDIF(PNG_FOUND)

IF(JPEG_FOUND)
  INCLUDE_DIRECTORIES(${JPEG_INCLUDE_DIR})
ENDIF(JPEG_FOUND)

IF(TIFF_FOUND)
  INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIR})
ENDIF(TIFF_FOUND)

IF(GIF_FOUND)
  INCLUDE_DIRECTORIES(${GIF_INCLUDE_DIR})
ENDIF(GIF_FOUND)

IF(DBUS_FOUND)
  INCLUDE_DIRECTORIES(${DBUS_INCLUDE_DIRS})
  LINK_DIRECTORIES(${DBUS_LIBRARY_DIRS})
ENDIF(DBUS_FOUND)

IF(HILDON_FOUND)
  INCLUDE_DIRECTORIES(${HILDON_INCLUDE_DIRS})
  LINK_DIRECTORIES(${HILDON_LIBRARY_DIRS})
ENDIF(HILDON_FOUND)

IF(GTK_FOUND)
  INCLUDE_DIRECTORIES(${GTK_INCLUDE_DIRS})
  LINK_DIRECTORIES(${GTK_LIBRARY_DIRS})
ENDIF(GTK_FOUND)

IF(GST_FOUND)
  INCLUDE_DIRECTORIES(${GST_INCLUDE_DIRS})
  LINK_DIRECTORIES(${GST_LIBRARY_DIRS})
ENDIF(GST_FOUND)

CONFIGURE_FILE ("${CMAKE_SOURCE_DIR}/include/Lum/Features.h.template" "${CMAKE_SOURCE_DIR}/include/Lum/Features.h" ESCAPE_QUOTES)
CONFIGURE_FILE ("${CMAKE_SOURCE_DIR}/include/Lum/Private/Config.h.template" "${CMAKE_SOURCE_DIR}/include/Lum/Private/Config.h" ESCAPE_QUOTES)
