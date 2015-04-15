SET(SOURCES Lum/Base/Background.cpp
            Lum/Base/DateTime.cpp
            Lum/Base/DragDrop.cpp
            Lum/Base/Fill.cpp
            Lum/Base/Frame.cpp
            Lum/Base/Image.cpp
            Lum/Base/Object.cpp
            Lum/Base/Path.cpp
            Lum/Base/Singleton.cpp
            Lum/Base/Size.cpp
            Lum/Base/Status.cpp
            Lum/Base/Util.cpp
            Lum/Config/Config.cpp
            Lum/Dlg/About.cpp
            Lum/Dlg/File.cpp
            Lum/Dlg/Msg.cpp
            Lum/Dlg/Progress.cpp
            Lum/Dlg/Value.cpp
            Lum/Edit/Edit.cpp
            Lum/Edit/Function.cpp
            Lum/Edit/Style.cpp
            Lum/Edit/Syntax.cpp
            Lum/Edit/Text.cpp
            Lum/Images/Image.cpp
            Lum/Images/Loader.cpp
            Lum/Images/Writer.cpp
            Lum/Model/Action.cpp
            Lum/Model/Adjustment.cpp
            Lum/Model/Boolean.cpp
            Lum/Model/Calendar.cpp
            Lum/Model/DataStream.cpp
            Lum/Model/Dir.cpp
            Lum/Model/Header.cpp
            Lum/Model/Path.cpp
            Lum/Model/Selection.cpp
            Lum/Model/Simple.cpp
            Lum/Model/String.cpp
            Lum/Model/Table.cpp
            Lum/Model/Time.cpp
            Lum/Model/Tree.cpp
            Lum/OS/Base/Bitmap.cpp
            Lum/OS/Base/Display.cpp
            Lum/OS/Base/DrawInfo.cpp
            Lum/OS/Base/Event.cpp
            Lum/OS/Base/Font.cpp
            Lum/OS/Base/Window.cpp
            Lum/OS/Bitmap.cpp
            Lum/OS/Display.cpp
            Lum/OS/DrawInfo.cpp
            Lum/OS/Event.cpp
            Lum/OS/Factories.cpp
            Lum/OS/Fill.cpp
            Lum/OS/Font.cpp
            Lum/OS/Frame.cpp
            Lum/OS/Image.cpp
            Lum/OS/Probe.cpp
            Lum/OS/Theme.cpp
            Lum/OS/Thread.cpp
            Lum/OS/Tray.cpp
            Lum/OS/Window.cpp
            Lum/Prefs/Base.cpp
            Lum/Array.cpp
            Lum/Blender.cpp
            Lum/Boolean.cpp
            Lum/Button.cpp
            Lum/ButtonRow.cpp
            Lum/Clock.cpp
            Lum/ColorArea.cpp
            Lum/Combo.cpp
            Lum/DatePicker.cpp
            Lum/DateSelect.cpp
            Lum/DateView.cpp
            Lum/Dialog.cpp
            Lum/DirNameSelect.cpp
            Lum/DirSelect.cpp
            Lum/FileNameSelect.cpp
            Lum/FrameGroup.cpp
            Lum/FuelGauge.cpp
            Lum/Grid.cpp
            Lum/Header.cpp
            Lum/Image.cpp
            Lum/Knob.cpp
            Lum/Label.cpp
            Lum/LED.cpp
            Lum/Menu.cpp
            Lum/Multi.cpp
            Lum/Object.cpp
            Lum/Panel.cpp
            Lum/PercentBar.cpp
            Lum/PopupControl.cpp
            Lum/Radio.cpp
            Lum/Scale.cpp
            Lum/Scroller.cpp
            Lum/Segment7.cpp
            Lum/Slider.cpp
            Lum/Space.cpp
            Lum/State.cpp
            Lum/StatusLine.cpp
            Lum/String.cpp
            Lum/Tab.cpp
            Lum/Table.cpp
            Lum/TableView.cpp
            Lum/Text.cpp
            Lum/TextValue.cpp
            Lum/Time.cpp
            Lum/TimeLine.cpp
            Lum/TimeSelect.cpp
            Lum/TimeView.cpp
            Lum/Toolbar.cpp
            Lum/Tree.cpp
            Lum/View.cpp
            Lum/WindowGroup.cpp)

IF(X11_FOUND)
  SET (SOURCES "${SOURCES}" Lum/OS/X11/Bitmap.cpp
                            Lum/OS/X11/Display.cpp
                            Lum/OS/X11/DrawInfo.cpp
                            Lum/OS/X11/Event.cpp
                            Lum/OS/X11/Factories.cpp
                            Lum/OS/X11/Font.cpp
                            Lum/OS/X11/Image.cpp
                            Lum/OS/X11/Tray.cpp
                            Lum/OS/X11/Window.cpp)
ENDIF(X11_FOUND)

IF(CAIRO_FOUND)
  SET(SOURCES "${SOURCES}" Lum/OS/Cairo/Display.cpp
                           Lum/OS/Cairo/Image.cpp)
ENDIF(CAIRO_FOUND)

IF(CURSES_LIBRARIES)
  SET(SOURCES "${SOURCES}" Lum/OS/Curses/Bitmap.cpp
                           Lum/OS/Curses/Display.cpp
                           Lum/OS/Curses/DrawInfo.cpp
                           Lum/OS/Curses/Event.cpp
                           Lum/OS/Curses/Factories.cpp
                           Lum/OS/Curses/Font.cpp
                           Lum/OS/Curses/Tray.cpp
                           Lum/OS/Curses/Window.cpp)
ENDIF(CURSES_LIBRARIES)

IF(WIN32)
  SET(SOURCES "${SOURCES}" Lum/OS/Win32/Bitmap.cpp
                           Lum/OS/Win32/Event.cpp
                           Lum/OS/Win32/Display.cpp
                           Lum/OS/Win32/DrawInfo.cpp
                           Lum/OS/Win32/Factories.cpp
                           Lum/OS/Win32/Font.cpp
                           Lum/OS/Win32/Image.cpp
                           Lum/OS/Win32/Tray.cpp
                           Lum/OS/Win32/Win32Theme.cpp
                           Lum/OS/Win32/Window.cpp)
ENDIF(WIN32)

IF(APPLE)
  SET(SOURCES "${SOURCES}" Lum/OS/Carbon/Display.cpp
                           Lum/OS/Carbon/Factories.cpp
                           Lum/OS/Carbon/Font.cpp
                           Lum/OS/Carbon/Window.cpp)
ENDIF(APPLE)

IF(PNG_FOUND)
  SET(SOURCES "${SOURCES}" Lum/Images/Loader/PNG.cpp
                           Lum/Images/Writer/PNG.cpp)
ENDIF(PNG_FOUND)

IF(JPEG_FOUND)
  SET(SOURCES "${SOURCES}" Lum/Images/Loader/JPEG.cpp
                           Lum/Images/Writer/JPEG.cpp)
ENDIF(JPEG_FOUND)

IF(TIFF_FOUND)
  SET(SOURCES "${SOURCES}" Lum/Images/Loader/TIFF.cpp)
ENDIF(TIFF_FOUND)

IF(GIF_FOUND)
  SET(SOURCES "${SOURCES}" Lum/Images/Loader/GIF.cpp)
ENDIF(GIF_FOUND)

ADD_LIBRARY(Illumination MODULE ${SOURCES})

SET_TARGET_PROPERTIES(Illumination PROPERTIES DEFINE_SYMBOL LUMDLL)

TARGET_LINK_LIBRARIES(Illumination ${CMAKE_THREAD_LIBS_INIT})

IF(CURSES_LIBRARIES)
  TARGET_LINK_LIBRARIES(Illumination ${CURSES_LIBRARY} panel)
ENDIF(CURSES_LIBRARIES)

IF(X11_FOUND)
  TARGET_LINK_LIBRARIES(Illumination ${X11_X11_LIB} ${X11_Xext_LIB})
ENDIF(X11_FOUND)

IF(XFT_FOUND)
  SET_TARGET_PROPERTIES(Illumination PROPERTIES COMPILE_FLAGS "${XFT_CFLAGS_OTHERS}" LINKFLAGS "${XFT_LDFLAGS_OTHERS}")
  TARGET_LINK_LIBRARIES(Illumination ${XFT_LIBRARIES})
ENDIF(XFT_FOUND)

IF (CAIRO_FOUND)
  SET_TARGET_PROPERTIES(Illumination PROPERTIES COMPILE_FLAGS "${CAIRO_CFLAGS_OTHERS}" LINKFLAGS "${CAIRO_LDFLAGS_OTHERS}")
  TARGET_LINK_LIBRARIES(Illumination ${CAIRO_LIBRARIES})
ENDIF(CAIRO_FOUND)

IF (WIN32)
ENDIF(WIN32)

IF(APPLE)
ENDIF(APPLE)

IF(XML_FOUND)
  SET_TARGET_PROPERTIES(Illumination PROPERTIES COMPILE_FLAGS "${XML_CFLAGS_OTHERS}" LINKFLAGS "${XML_LDFLAGS_OTHERS}")
  TARGET_LINK_LIBRARIES(Illumination ${XML_LIBS})
ENDIF(XML_FOUND)

IF(PNG_FOUND)
  TARGET_LINK_LIBRARIES(Illumination ${PNG_LIBRARIES})
ENDIF(PNG_FOUND)

IF(JPEG_FOUND)
  TARGET_LINK_LIBRARIES(Illumination ${JPEG_LIBRARIES})
ENDIF(JPEG_FOUND)

IF(TIFF_FOUND)
  TARGET_LINK_LIBRARIES(Illumination ${TIFF_LIBRARIES})
ENDIF(TIFF_FOUND)

IF(GIF_FOUND)
  TARGET_LINK_LIBRARIES(Illumination ${GIF_LIBRARIES})
ENDIF(GIF_FOUND)

IF(DBUS_FOUND)
  SET_TARGET_PROPERTIES(Illumination PROPERTIES COMPILE_FLAGS "${DBUS_CFLAGS_OTHERS}" LINKFLAGS "${DBUS_LDFLAGS_OTHERS}")
  TARGET_LINK_LIBRARIES(Illumination ${DBUS_LIBRARIES})
ENDIF(DBUS_FOUND)

IF(HILDON_FOUND)
  SET_TARGET_PROPERTIES(Illumination PROPERTIES COMPILE_FLAGS "${HILDON_CFLAGS_OTHERS}" LINKFLAGS "${HILDON_LDFLAGS_OTHERS}")
  TARGET_LINK_LIBRARIES(Illumination ${HILDON_LIBRARIES})
ENDIF(HILDON_FOUND)

IF(GTK_FOUND)
  SET_TARGET_PROPERTIES(Illumination PROPERTIES COMPILE_FLAGS "${GTK_CFLAGS_OTHERS}" LINKFLAGS "${GTK_LDFLAGS_OTHERS}")
  TARGET_LINK_LIBRARIES(Illumination ${GTK_LIBRARIES})
ENDIF(GTK_FOUND)

IF(GST_FOUND)
  SET_TARGET_PROPERTIES(Illumination PROPERTIES COMPILE_FLAGS "${GST_CFLAGS_OTHERS}" LINKFLAGS "${GST_LDFLAGS_OTHERS}")
  TARGET_LINK_LIBRARIES(Illumination ${GST_LIBRARIES})
ENDIF(GST_FOUND)
