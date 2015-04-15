AM_CPPFLAGS = $(LIB_CXXFLAGS) -DLUMDLL -I$(top_srcdir)/include \
              $(X_CFLAGS) $(XFT_CFLAGS) $(XRANDR_CFLAGS)\
              $(XML2_CFLAGS) \
              $(PNG_CFLAGS) \
              $(WIN32_CFLAGS) \
              $(CAIRO_CFLAGS) \
              $(GLIB_CFLAGS) $(DBUS_CFLAGS) $(GTK_CFLAGS) $(GSTREAMER_CFLAGS) $(GCONF_CFLAGS) \
              $(HILDON_CFLAGS) $(OSSO_CFLAGS) $(CONIC_CFLAGS)


lib_LTLIBRARIES = libIllumination.la

# We disable $(X_PRE_LIBS) until we need them somewhere
libIllumination_la_LDFLAGS = -release 0.2 -version-info 1:0:0 \
                            $(X_LIBS) $(X11_LIBS) $(X_EXTRA_LIBS) $(XFT_LIBS) $(XRANDR_LIBS)\
                            $(LIBICONV) \
                            $(WIN32_LIBS) \
                            $(CARBON_LIBS) \
                            $(XML2_LIBS) \
                            $(PNG_LIBS) \
                            $(CAIRO_LIBS) \
                            $(GLIB_FLAGS) $(DBUS_LIBS) $(GTK_LIBS) $(GSTREAMER_LIBS) $(GCONF_LIBS) \
                            $(HILDON_LIBS) $(OSSO_LIBS) $(CONIC_LIBS)

libIllumination_la_SOURCES= Lum/Audio/PlaySound.cpp \
                            Lum/Audio/PlaySoundNull.cpp \
                            Lum/Base/DateTime.cpp \
                            Lum/Base/DragDrop.cpp \
                            Lum/Base/L10N.cpp \
                            Lum/Base/Model.cpp \
                            Lum/Base/Object.cpp \
                            Lum/Base/Path.cpp \
                            Lum/Base/Reference.cpp \
                            Lum/Base/ResourcePool.cpp \
                            Lum/Base/Singleton.cpp \
                            Lum/Base/Size.cpp \
                            Lum/Base/Status.cpp \
                            Lum/Base/String.cpp \
                            Lum/Base/Util.cpp \
                            Lum/Config/Config.cpp \
                            Lum/Def/Action.cpp \
                            Lum/Def/AppInfo.cpp \
                            Lum/Def/Boolean.cpp \
                            Lum/Def/Desc.cpp \
                            Lum/Def/Interval.cpp \
                            Lum/Def/Menu.cpp \
                            Lum/Def/MultiView.cpp \
                            Lum/Def/Number.cpp \
                            Lum/Def/OneOfMany.cpp \
                            Lum/Def/OneOfN.cpp \
                            Lum/Def/Property.cpp \
                            Lum/Def/Props.cpp \
                            Lum/Dlg/About.cpp \
                            Lum/Dlg/ActionDialog.cpp \
                            Lum/Dlg/ColorChooser.cpp \
                            Lum/Dlg/Properties.cpp \
                            Lum/Dlg/File.cpp \
                            Lum/Dlg/Help.cpp \
                            Lum/Dlg/Msg.cpp \
                            Lum/Dlg/Password.cpp \
                            Lum/Dlg/Progress.cpp \
                            Lum/Dlg/TimeSpanChooser.cpp \
                            Lum/Dlg/Validator.cpp \
                            Lum/Dlg/Value.cpp \
                            Lum/Dlg/ValuePopup.cpp \
                            Lum/Dlg/Values.cpp \
                            Lum/Edit/Edit.cpp \
                            Lum/Edit/Function.cpp \
                            Lum/Edit/Style.cpp \
                            Lum/Edit/Syntax.cpp\
                            Lum/Edit/Text.cpp\
                            Lum/Images/Image.cpp \
                            Lum/Images/Loader.cpp \
                            Lum/Images/StatefulImage.cpp \
                            Lum/Images/Writer.cpp \
                            Lum/Manager/Behaviour.cpp \
                            Lum/Manager/Display.cpp \
                            Lum/Manager/FileSystem.cpp \
                            Lum/Manager/Keyboard.cpp \
                            Lum/Manager/Network.cpp \
                            Lum/Model/Action.cpp \
                            Lum/Model/Adjustment.cpp \
                            Lum/Model/Boolean.cpp \
                            Lum/Model/Calendar.cpp \
                            Lum/Model/Color.cpp \
                            Lum/Model/Converter.cpp \
                            Lum/Model/DataStream.cpp \
                            Lum/Model/Dir.cpp \
                            Lum/Model/Header.cpp \
                            Lum/Model/Number.cpp \
                            Lum/Model/Path.cpp \
                            Lum/Model/Selection.cpp \
                            Lum/Model/Simple.cpp \
                            Lum/Model/String.cpp \
                            Lum/Model/Table.cpp \
                            Lum/Model/Time.cpp \
                            Lum/Model/Tree.cpp \
                            Lum/OS/Base/Behaviour.cpp \
                            Lum/OS/Base/Bitmap.cpp \
                            Lum/OS/Base/Display.cpp \
                            Lum/OS/Base/DrawInfo.cpp \
                            Lum/OS/Base/Event.cpp \
                            Lum/OS/Base/Font.cpp \
                            Lum/OS/Base/Theme.cpp \
                            Lum/OS/Base/Window.cpp \
                            Lum/OS/Bitmap.cpp \
                            Lum/OS/Color.cpp \
                            Lum/OS/Display.cpp \
                            Lum/OS/DrawInfo.cpp \
                            Lum/OS/Driver.cpp \
                            Lum/OS/Event.cpp \
                            Lum/OS/EventLoop.cpp \
                            Lum/OS/Fill.cpp \
                            Lum/OS/Font.cpp \
                            Lum/OS/Frame.cpp \
                            Lum/OS/Image.cpp \
                            Lum/OS/Paragraph.cpp \
                            Lum/OS/Probe.cpp \
                            Lum/OS/Theme.cpp \
                            Lum/OS/Thread.cpp \
                            Lum/OS/Tray.cpp \
                            Lum/OS/Window.cpp \
                            Lum/About.cpp \
                            Lum/Application.cpp \
                            Lum/Array.cpp \
                            Lum/AspectRatio.cpp \
                            Lum/Blender.cpp \
                            Lum/Boolean.cpp \
                            Lum/BooleanButton.cpp \
                            Lum/Button.cpp \
                            Lum/ButtonRow.cpp \
                            Lum/ColorView.cpp \
                            Lum/Combo.cpp \
                            Lum/DatePicker.cpp \
                            Lum/DateSelect.cpp \
                            Lum/DateView.cpp \
                            Lum/Dialog.cpp \
                            Lum/DirNameSelect.cpp \
                            Lum/DirSelect.cpp \
                            Lum/FileNameSelect.cpp \
                            Lum/Float.cpp \
                            Lum/FrameGroup.cpp \
                            Lum/FuelGauge.cpp \
                            Lum/Grid.cpp \
                            Lum/Header.cpp \
                            Lum/Image.cpp \
                            Lum/KineticScroller.cpp \
                            Lum/Knob.cpp \
                            Lum/Label.cpp \
                            Lum/LED.cpp \
                            Lum/Menu.cpp \
                            Lum/Multi.cpp \
                            Lum/Object.cpp \
                            Lum/Pane.cpp \
                            Lum/Panel.cpp \
                            Lum/PercentBar.cpp \
                            Lum/PercentPie.cpp \
                            Lum/Placeholder.cpp \
                            Lum/Popup.cpp \
                            Lum/PopupControl.cpp \
                            Lum/PopupGroup.cpp \
                            Lum/PosMarker.cpp \
                            Lum/Radio.cpp \
                            Lum/RGBControl.cpp \
                            Lum/Scale.cpp \
                            Lum/ScrolledObject.cpp \
                            Lum/Scroller.cpp \
                            Lum/Segment7.cpp \
                            Lum/Slider.cpp \
                            Lum/Space.cpp \
                            Lum/State.cpp \
                            Lum/StatusLine.cpp \
                            Lum/String.cpp \
                            Lum/Tab.cpp \
                            Lum/Table.cpp \
                            Lum/TableView.cpp \
                            Lum/Text.cpp \
                            Lum/TextLabel.cpp \
                            Lum/TextValue.cpp \
                            Lum/Time.cpp \
                            Lum/TimeLine.cpp \
                            Lum/TimeSelect.cpp \
                            Lum/TimeSpan.cpp \
                            Lum/TimeView.cpp \
                            Lum/Toggle.cpp \
                            Lum/Toolbar.cpp \
                            Lum/Tree.cpp \
                            Lum/ValueButton.cpp \
                            Lum/ValuePainter.cpp \
                            Lum/View.cpp \
                            Lum/Window.cpp \
                            Lum/WindowGroup.cpp \
                            Lum/Wizard.cpp

if HAVE_FUNC_SELECT
libIllumination_la_SOURCES += Lum/OS/EventLoopUnix.cpp
endif

if HAVE_LIB_GSTREAMER
libIllumination_la_SOURCES += Lum/Audio/PlaySoundGst.cpp
endif

if LUM_HAVE_LIB_X
libIllumination_la_SOURCES += Lum/OS/X11/Display.cpp \
                              Lum/OS/X11/Event.cpp \
                              Lum/OS/X11/SubWindow.cpp \
                              Lum/OS/X11/Tray.cpp \
                              Lum/OS/X11/Window.cpp

endif

if LUM_HAVE_LIB_HILDON
libIllumination_la_SOURCES += Lum/OS/Hildon/Behaviour.cpp \
                              Lum/OS/Hildon/KeyboardManager.cpp \
                              Lum/OS/Hildon/DisplayManager.cpp
endif

if HAVE_LIB_CONIC
libIllumination_la_SOURCES += Lum/OS/Hildon/NetworkManager.cpp
endif


if LUM_HAVE_LIB_CAIRO
libIllumination_la_SOURCES += Lum/OS/Cairo/Bitmap.cpp \
                              Lum/OS/Cairo/Driver.cpp \
                              Lum/OS/Cairo/DrawInfo.cpp \
                              Lum/OS/Cairo/Font.cpp \
                              Lum/OS/Cairo/Image.cpp
endif

if LUM_HAVE_LIB_CURSES
libIllumination_la_SOURCES += Lum/OS/Curses/Bitmap.cpp \
                              Lum/OS/Curses/Display.cpp \
                              Lum/OS/Curses/DrawInfo.cpp \
                              Lum/OS/Curses/Event.cpp \
                              Lum/OS/Curses/Driver.cpp \
                              Lum/OS/Curses/Font.cpp \
                              Lum/OS/Curses/Theme.cpp \
                              Lum/OS/Curses/Tray.cpp \
                              Lum/OS/Curses/Window.cpp
endif

if LUM_HAVE_LIB_WIN32
libIllumination_la_SOURCES += Lum/Audio/PlaySoundWin32.cpp \
                              Lum/OS/Win32/Bitmap.cpp \
                              Lum/OS/Win32/Display.cpp \
                              Lum/OS/Win32/DrawInfo.cpp \
                              Lum/OS/Win32/Driver.cpp \
                              Lum/OS/Win32/Event.cpp \
                              Lum/OS/Win32/EventLoop.cpp \
                              Lum/OS/Win32/Font.cpp \
                              Lum/OS/Win32/Image.cpp \
                              Lum/OS/Win32/Tray.cpp \
                              Lum/OS/Win32/Win32Theme.cpp \
                              Lum/OS/Win32/Window.cpp
endif

if LUM_HAVE_LIB_CARBON
libIllumination_la_SOURCES += Lum/OS/Carbon/Display.cpp \
                              Lum/OS/Carbon/DrawInfo.cpp \
                              Lum/OS/Carbon/Event.cpp \
                              Lum/OS/Carbon/Factories.cpp \
                              Lum/OS/Carbon/Font.cpp \
                              Lum/OS/Carbon/Window.cpp
endif

if HAVE_LIB_PNG
libIllumination_la_SOURCES += Lum/Images/Loader/PNG.cpp \
                              Lum/Images/Writer/PNG.cpp
endif

if HAVE_LIB_JPEG
libIllumination_la_SOURCES += Lum/Images/Loader/JPEG.cpp \
                              Lum/Images/Writer/JPEG.cpp
endif

if HAVE_LIB_TIFF
libIllumination_la_SOURCES += Lum/Images/Loader/TIFF.cpp
endif

if HAVE_LIB_GIF
libIllumination_la_SOURCES += Lum/Images/Loader/GIF.cpp
endif

if HAVE_LIB_GTK
libIllumination_la_SOURCES += Lum/OS/EventLoopGMainLoop.cpp\
                              Lum/OS/Gtk/GtkTheme.cpp
endif
