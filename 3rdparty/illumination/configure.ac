dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.56)
AC_INIT([illumination1],[0.2],[tim@teulings.org],[illumination])

AC_CONFIG_SRCDIR(src/Lum/OS/Display.cpp)
AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_HEADERS([include/Lum/Private/Config.h  include/Lum/Features.h])
AM_INIT_AUTOMAKE([no-define nostdinc subdir-objects dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AC_GNU_SOURCE
AC_PROG_CXX
AC_LANG(C++)
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL
AC_PROG_INSTALL

dnl Driver selection
AC_ARG_ENABLE(cairo-driver,
              [  --enable-cairo-driver      include cairo driver])
AC_ARG_ENABLE(carbon-driver,
              [  --enable-carbon-driver     include carbon (MAC OS X) driver])
AC_ARG_ENABLE(curses-driver,
              [  --enable-curses-driver     include curses driver])
AC_ARG_ENABLE(x11-driver,
              [  --enable-x11-driver        include x11 driver])
AC_ARG_ENABLE(w32-driver,
              [  --enable-win32-driver      include win32 (Windows native) driver])

AC_ARG_ENABLE(gif-support,
              [  --enable-gif-support       add support for gif images])
AC_ARG_ENABLE(jpeg-support,
              [  --enable-jpeg-support      add support for jpeg images])
AC_ARG_ENABLE(png-support,
              [  --enable-png-support       add support for png images])
AC_ARG_ENABLE(tiff-support,
              [  --enable-tiff-support      add support for tiff images])

AC_ARG_ENABLE(x11session,
              [  --enable-xsession          include support for X11 session management])

AC_ARG_ENABLE(glib-support,
              [  --enable-glib-support      include support for GLib])
AC_ARG_ENABLE(dbus-support,
              [  --enable-dbus-support      include support for DBUS desktop communication protocol])
AC_ARG_ENABLE(gtk-support,
              [  --enable-gtk-support       include support for GTK])
AC_ARG_ENABLE(gstreamer-support,
              [  --enable-gstreamer-support include support for gstreamer])
AC_ARG_ENABLE(hildon-support,
              [  --enable-hildon-support    include support for maemo hildon framework])
AC_ARG_ENABLE(osso-support,
              [  --enable-osso-support      include support for maemo osso framework])
AC_ARG_ENABLE(conic-support,
              [  --enable-conic-support     include support for maemo conic framework])


AC_TYPE_SIZE_T
AC_CHECK_SIZEOF([wchar_t])

AC_CHECK_HEADERS([unistd.h libgen.h pwd.h sys/time.h])

AC_CHECK_FUNCS([select \
                getpid \
                gettimeofday \
                gmtime_r localtime_r asctime_r])


AM_CONDITIONAL(HAVE_FUNC_SELECT,[test "$ac_cv_func_select" != no])

dnl Check for pthread
ACX_PTHREAD([CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
             LIBS="$PTHREAD_LIBS $LIBS"
             CC="$PTHREAD_CC"
             AC_DEFINE(ILLUMINATION_THREAD_PTHREAD,1,[Illumination uses pthread for thread implementation])])

AC_CHECK_HEADER([langinfo.h],
                [AC_DEFINE(HAVE_HEADER_LANGINFO,1,[header langinfo.h found])])

AC_SEARCH_LIBS([sqrt],[m])

dnl For Windows specific functions we cannot use AC_CHECK_FUNCS because
dnl it does not use the necessary DLL epxort markers (__declspec).
dnl We have to write our own little test macro.

AC_MSG_CHECKING([for mbsrtowcs])
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([[#include <wchar.h>]],
                   [[return mbsrtowcs!=NULL;]])],
  [AC_MSG_RESULT(yes)
   AC_DEFINE(HAVE_MBSRTOWCS,1,[String conversion function mbsrtowcs])],
  [AC_MSG_RESULT(no)])

AC_MSG_CHECKING([for wcsrtombs])
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([[#include <wchar.h>]],
                   [[return wcsrtombs!=NULL;]])],
  [AC_MSG_RESULT(yes)
   AC_DEFINE(HAVE_WCSRTOMBS,1,[String conversion function wcsrtombs])],
  [AC_MSG_RESULT(no)])

AC_MSG_CHECKING([for GetSystemTimeAsFileTime])
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([[#include <windows.h>]],
                   [[return GetSystemTimeAsFileTime!=NULL;]])],
  [AC_MSG_RESULT(yes)
   AC_DEFINE(HAVE_GETSYSTEMTIMEASFILETIME,1,[Windows time function GetSystemTimeAsFileTime])],
  [AC_MSG_RESULT(no)])

AC_MSG_CHECKING([for SleepEx])
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([[#include <windows.h>]],
                   [[return SleepEx!=NULL;]])],
  [AC_MSG_RESULT(yes)
   AC_DEFINE(HAVE_SLEEPEX,1,[Windows time function SleepEx])],
  [AC_MSG_RESULT(no)])

AC_MSG_CHECKING([for gmtime_s])
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([[#include <time.h>]],
                   [[return gmtime_s!=NULL;]])],
  [AC_MSG_RESULT(yes)
   AC_DEFINE(HAVE_GMTIME_S,1,[Windows time function gmtime_s])],
  [AC_MSG_RESULT(no)])

AC_MSG_CHECKING([for localtime_s])
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([[#include <time.h>]],
                   [[return localtime_s!=NULL;]])],
  [AC_MSG_RESULT(yes)
   AC_DEFINE(HAVE_LOCALTIME_S,1,[Windows time function localtime_s])],
  [AC_MSG_RESULT(no)])

AC_MSG_CHECKING([for FindFirstFile])
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([[#include <windows.h>]],
                   [[return FindFirstFileW!=NULL;]])],
  [AC_MSG_RESULT(yes)
   AC_DEFINE(HAVE_FINDFIRSTFILEW,1,[Windows directory scanning function FindFirstFileW])],
  [AC_MSG_RESULT(no)])

AC_FUNC_SELECT_ARGTYPES

if test "$GXX" = yes; then
  CPPFLAGS="$CPPFLAGS -Wall -Wpointer-arith -Wsign-compare -Wundef -Wcast-qual -Wcast-align -Wwrite-strings -Wredundant-decls -Wno-long-long"
fi

dnl Selfmade tests

if test "$build_os" != "mingw32"; then
  AC_MSG_CHECKING([for gcc symbol visibility support])
  OLDCXXFLAGS="$CXXFLAGS"
  CXXFLAGS="$CXXFLAGS -fvisibility=hidden -fvisibility-inlines-hidden"
  AC_TRY_COMPILE([],
                 [],
                 [LIB_CXXFLAGS="-fvisibility=hidden -fvisibility-inlines-hidden"
                  AC_SUBST(LIB_CXXFLAGS)
                  AC_MSG_RESULT(yes)
                  AC_DEFINE(ILLUMINATION_EXPORT_SYMBOLS,1,[Illumination uses special gcc compiler features to export symbols])],
                 [AC_MSG_RESULT(no)])
  CXXFLAGS="$OLDCXXFLAGS"
fi

dnl Test for full support for std::wstring
AC_MSG_CHECKING([for std::wstring])
AC_LINK_IFELSE([
  AC_LANG_PROGRAM([[#include <string>]],
                  [[std::wstring bla=L"";]])],
  [AC_MSG_RESULT(yes); HAS_WSTRING="yes"],
  [AC_MSG_RESULT(no); HAS_WSTRING="no"])

dnl If std::wstring does not work out of the box, we just
dnl try to explicitely instanciate std::basic_string<wchar_t>

if test "$HAS_WSTRING" = no; then
  AC_MSG_CHECKING([for working std::basic_string<wchar_t>])
  AC_LINK_IFELSE(
    [AC_LANG_PROGRAM(
      [[#include <string>
template class std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> >;]],
      [[std::wstring bla=L"";]])],
    [AC_MSG_RESULT(yes)
     AC_DEFINE(EXPLICIT_WSTRING,1,[Need explicit instanciation of std::wstring])],
    [AC_MSG_ERROR(No propper support of std::wstring)])
fi

dnl This is the iconv macro from the gettext package,
dnl  doing all the dirty iconv job for us
AM_ICONV
AC_MSG_CHECKING([for iconv supporting WCHAR_T])
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([[#include <iconv.h>]],
                  [[if (iconv_open("UTF-32", "WCHAR_T") == (iconv_t)(-1)) return 1;]])],
  [AC_MSG_RESULT(yes)
   AC_DEFINE(ICONV_HAS_WCHAR_T,1,[iconv supports wchar_t])],
  [AC_MSG_RESULT(no)])

AC_MSG_CHECKING([for iconv supporting conversion to ""])
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([[#include <iconv.h>]],
                  [[if (iconv_open("UTF-32", "") == (iconv_t)(-1)) return 1;]])],
  [AC_MSG_RESULT(yes)
   AC_DEFINE(ICONV_SUPPORTS_EMPTY_STRING,1,[iconv supports empty string])],
  [AC_MSG_RESULT(no)])

dnl Check for libxml2
PKG_CHECK_MODULES(XML2,[libxml-2.0 >= 2.6.0],
                  [HASXML="yes"
                   AC_SUBST(XML2_CFLAGS)
                   AC_SUBST(XML2_LIBS)
                   AC_DEFINE(HAVE_LIB_XML2, 1,[libxml2 found])],
                  [HASXML="no"])

if test "$HASXML" = no; then
  dnl Check for xerces
  OLDLIBS="$LIBS"
  LIBS="-lxerces-c $LIBS"
  AC_MSG_CHECKING([for xerces])
  AC_LINK_IFELSE(
    [AC_LANG_PROGRAM([[#include <xercesc/util/PlatformUtils.hpp>
                      #include <xercesc/parsers/SAXParser.hpp>
                      #include <xercesc/sax2/XMLReaderFactory.hpp>
                      XERCES_CPP_NAMESPACE_USE]],
                     [[XMLPlatformUtils::Initialize();]])],
    [AC_MSG_RESULT(yes)
     AC_DEFINE(HAVE_LIB_XERCES,1,[libxerces found])],
    [AC_MSG_RESULT(no)
     LIBS="$OLDLIBS"
     HASXERCES="no"])
fi

dnl Check for X Window system

if test "$enable_x11_driver" != "no"; then
  AC_PATH_XTRA

  if test "$no_x" != yes; then
    X11_LIBS="-lXext -lX11"
    OLDLDFLAGS="$LDLFAGS"
    LDFLAGS="$LDFLAGS $X_LIBS"

    AC_DEFINE(LUM_HAVE_LIB_X,1,[X11 found])

    if test "$enable_x11session" != "no"; then
      AC_CHECK_LIB(SM,SmcOpenConnection,[],,)
      AC_SUBST(X11_LIBS)
      LDLFAGS="$OLDLDFLAGS"
    fi
  fi
fi
AM_CONDITIONAL(LUM_HAVE_LIB_X,[test "$no_x" != yes])

dnl Check for libXft
PKG_CHECK_MODULES(XFT,[xft],
                  [AC_SUBST(XFT_CFLAGS)
                   AC_SUBST(XFT_LIBS)
                   AC_DEFINE(HAVE_LIB_XFT2,1,[Version 2 of libXft found])],
                  [HASXFT="no"])

PKG_CHECK_MODULES(XRANDR,[xrandr],
                  [AC_SUBST(XRANDR_CFLAGS)
                   AC_SUBST(XRANDR_LIBS)
                   AC_DEFINE(HAVE_LIB_XRANDR,1,[xrandr library found])],
                   [HASXRANDR=no])

dnl Check for libCairo

if test "$enable_cairo_driver" != "no"; then
  PKG_CHECK_MODULES(CAIRO,[cairo],
                    [AC_SUBST(CAIRO_CFLAGS)
                     AC_SUBST(CAIRO_LIBS)
                     AC_DEFINE(LUM_HAVE_LIB_CAIRO,1,[libcairo found])],
                    [HASCAIRO="no"])
fi
AM_CONDITIONAL(LUM_HAVE_LIB_CAIRO,[test -n "$CAIRO_LIBS"])

dnl Checking for curses and panel library

if test "$enable_curses_driver" != "no"; then
  OLDLIBS="$LIBS"
  LIBS="-lpanel -lcurses $LIBS"
  AC_MSG_CHECKING([for curses and panel library])
  AC_LINK_IFELSE(
    [AC_LANG_PROGRAM([[#include <curses.h>
                      #include <panel.h>]],
                     [[initscr();
                      update_panels();]])],
    [AC_MSG_RESULT(yes)
     AC_DEFINE(LUM_HAVE_LIB_CURSES,1,[curses and panel library found])],
    [AC_MSG_RESULT(no)
     LIBS="$OLDLIBS"
     HASCURSES="no"])
else
  HASCURSES="no"
fi
AM_CONDITIONAL(LUM_HAVE_LIB_CURSES,[test "$HASCURSES" != no])

dnl Checking for Win32

if test "$enable_win32_driver" != "no"; then
  AC_MSG_CHECKING([for WIN32])
  AC_LINK_IFELSE([
    AC_LANG_PROGRAM([[#include <windows.h>]],
                    [[GetModuleHandleA(0);]])],
    [AC_MSG_RESULT(yes);
     WIN32_CFLAGS="-D_WIN32_WINNT=0x0501"
     WIN32_LIBS="-lgdi32 -lwinmm"
     LDFLAGS="$LDFLAGS -no-undefined"
     AC_SUBST(WIN32_CFLAGS)
     AC_SUBST(WIN32_LIBS)
     AC_DEFINE(LUM_HAVE_LIB_WIN32,1,[WIN32 found])],
    [AC_MSG_RESULT(no)])
fi
AM_CONDITIONAL(LUM_HAVE_LIB_WIN32,[test -n "$WIN32_LIBS"])

dnl Checking for Carbon

if test "$enable_carbon_driver" != "no"; then
  AC_MSG_CHECKING([for Mac OS X])
  OLDCXXFLAGS="$CXXFLAGS"
  CXXFLAGS="$CXXFLAGS -framework Carbon"
  AC_TRY_COMPILE([],
                 [],
                 [CARBON_LIBS="-framework Carbon"
                  CARBON_CFLAGS=""
                  AC_SUBST(CARBON_LIBS)
                  AC_SUBST(CARBON_CFLAGS)
                  AC_MSG_RESULT(yes)
                  AC_DEFINE(LUM_HAVE_LIB_CARBON,1,[Mac OS X found])],
                 [AC_MSG_RESULT(no)])
  CXXFLAGS="$OLDCXXFLAGS"
fi
AM_CONDITIONAL(LUM_HAVE_LIB_CARBON,[test -n "$CARBON_LIBS"])

dnl Check for optional image libraries

AC_CHECK_LIB(z,gzopen,,,)

dnl libpng

if test "$enable_png_support" != "no"; then
  PKG_CHECK_MODULES(PNG,libpng,
                    [AC_SUBST(PNG_CFLAGS)
                     AC_SUBST(PNG_LIBS)
                     AC_DEFINE(HAVE_LIB_PNG,1,[libpng found])],
                     [HASCAIRO="no"])
fi
AM_CONDITIONAL(HAVE_LIB_PNG,[test -n "$PNG_LIBS"])

dnl libjpeg

if test "$enable_jpeg_support" != "no"; then
  AC_CHECK_LIB(jpeg,jpeg_read_scanlines,[],,)
  if test "$ac_cv_lib_jpeg_jpeg_read_scanlines" = yes; then
    AC_CHECK_HEADER([jpeglib.h],
                    [have_lib_jpeg=true
         AC_DEFINE(HAVE_LIB_JPEG,1,[libjpeg found])])
  fi
fi
AM_CONDITIONAL(HAVE_LIB_JPEG,[test "$have_lib_jpeg" = true])

dnl libtiff

if test "$enable_tiff_support" != "no"; then
  AC_CHECK_LIB(tiff,TIFFOpen,[],,[-lz -lm -ljpeg])
  if test "$ac_cv_lib_tiff_TIFFOpen" = yes; then
    AC_CHECK_HEADER([tiffio.h],
                    [have_lib_tiff=true
                     AC_DEFINE(HAVE_LIB_TIFF,1,[libtiff found])])
  fi
fi
AM_CONDITIONAL(HAVE_LIB_TIFF,[test "$have_lib_tiff" = true])

dnl libgif

if test "$enable_gif_support" != "no"; then
  AC_CHECK_LIB(gif,DGifOpenFileName,[],,)
  if test "$ac_cv_lib_gif_DGifOpenFileName" = yes; then
    AC_CHECK_HEADER([gif_lib.h],
                    [have_lib_gif=true
         AC_DEFINE(HAVE_LIB_GIF,1,[libgif found])])
  fi
fi
AM_CONDITIONAL(HAVE_LIB_GIF,[test "$have_lib_gif" = true])

if test "$enable_glib_support" != "no"; then
  PKG_CHECK_MODULES(GLIB,[glib-2.0],
                    [AC_SUBST(GLIB_CFLAGS)
                     AC_SUBST(GLIB_LIBS)
                     AC_DEFINE(HAVE_LIB_GLIB,1,[GLib libraries found])],
                     [HASGLIB=no])
fi
AM_CONDITIONAL(HAVE_LIB_GLIB,[test -n "$GLIB_LIBS"])

if test "$enable_hildon_support" != "no"; then
  PKG_CHECK_MODULES(HILDON,[hildon-1 gdk-pixbuf-xlib-2.0],
                    [AC_SUBST(HILDON_CFLAGS)
                     AC_SUBST(HILDON_LIBS)
                     AC_DEFINE(LUM_HAVE_LIB_HILDON,1,[hildon libraries found])
                     AC_DEFINE(HAVE_LIB_GTK,1,[GTK libraries found])
                     enable_gtk_support=no],
                    [HASHILDON=no])
fi
AM_CONDITIONAL(LUM_HAVE_LIB_HILDON,[test -n "$HILDON_LIBS"])

if test "$enable_gtk_support" != "no"; then
  PKG_CHECK_MODULES(GTK,[gtk+-2.0 >= 2.6 gdk-pixbuf-xlib-2.0 gthread-2.0],
                    [AC_SUBST(GTK_CFLAGS)
                     AC_SUBST(GTK_LIBS)
                     AC_DEFINE(HAVE_LIB_GTK,1,[GTK libraries found])],
                     [HASGTK=no])
fi
AM_CONDITIONAL(HAVE_LIB_GTK,[test -n "$GTK_LIBS" -o -n "$HILDON_LIBS"])

if test "$enable_dbus_support" != "no"; then
  PKG_CHECK_MODULES(DBUS,[dbus-1 >= 0.61 dbus-glib-1 >= 0.61],
                    [AC_SUBST(DBUS_CFLAGS)
                     AC_SUBST(DBUS_LIBS)
                     AC_DEFINE(HAVE_LIB_DBUS,1,[libdbus found])],
                     [HASDBUS=NO])
fi
AM_CONDITIONAL(HAVE_LIB_DBUS,[test -n "$DBUS_LIBS"])

if test "$enable_gstreamer_support" != "no"; then
  PKG_CHECK_MODULES(GSTREAMER,[gstreamer-0.10],
                    [AC_SUBST(GSTREAMER_CFLAGS)
                     AC_SUBST(GSTREAMER_LIBS)
                     AC_DEFINE(HAVE_LIB_GSTREAMER,1,[libgstreamer found])],
                     [HASGSTREAMER=NO])
fi
AM_CONDITIONAL(HAVE_LIB_GSTREAMER,[test -n "$GSTREAMER_LIBS"])

if test "$enable_gconf_support" != "no"; then
  PKG_CHECK_MODULES(GCONF,[gconf-2.0],
                    [AC_SUBST(GCONF_CFLAGS)
                     AC_SUBST(GCONF_LIBS)
                     AC_DEFINE(HAVE_LIB_GCONF,1,[libgconf found])],
                     [HASGCONF=NO])
fi
AM_CONDITIONAL(HAVE_LIB_GCONF,[test -n "$GCONF_LIBS"])

if test "$enable_osso_support" != "no"; then
  PKG_CHECK_MODULES(OSSO,[libosso],
                    [AC_SUBST(OSSO_CFLAGS)
                     AC_SUBST(OSSO_LIBS)
                     AC_DEFINE(HAVE_LIB_OSSO,1,[libosso found])],
                     [HASOSSO=NO])
fi
AM_CONDITIONAL(HAVE_LIB_OSSO,[test -n "$OSSO_LIBS"])

if test "$enable_conic" != "no"; then
  PKG_CHECK_MODULES(CONIC,[conic],
                    [AC_SUBST(CONIC_CFLAGS)
                     AC_SUBST(CONIC_LIBS)
                     AC_DEFINE(HAVE_LIB_CONIC,1,[libconic found])],
                     [HASCONIC=NO])
fi
AM_CONDITIONAL(HAVE_LIB_CONIC,[test -n "$CONIC_LIBS"])

CPPFLAGS="-DLIB_DATADIR=\\\"$datadir/$PACKAGE_NAME\\\" $CPPFLAGS"

AC_CONFIG_FILES([Makefile src/Makefile include/Makefile tests/Makefile illumination.pc illumination-uninstalled.pc illumination-full.pc illumination-full-uninstalled.pc])
AC_OUTPUT

