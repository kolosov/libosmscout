AM_CPPFLAGS = -I$(top_srcdir)/include -I../include $(X_CFLAGS) $(XFTFLAGS)
AM_LDFLAGS  = $(X_LIBS) $(X_PRE_LIBS) -lXext -lX11 $(X_EXTRA_LIBS) $(XFTLIBS) $(LIBICONV) ../src/libIllumination.la

check_PROGRAMS = Path \
                 TextConversion

TESTS = $(check_PROGRAMS)

Path_SOURCES = Path.cpp
Path_DEPENDENCIES = $(top_srcdir)/src/libIllumination.la

TextConversion_SOURCES = TextConversion.cpp
TextConversion_DEPENDENCIES = $(top_srcdir)/src/libIllumination.la


