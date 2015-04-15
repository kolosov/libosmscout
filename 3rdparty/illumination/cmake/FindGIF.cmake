# - Find GIF
# Find the native GIF includes and library
# This module defines
#  GIF_INCLUDE_DIR, where to find gif_lib.h etc.
#  GIF_LIBRARIES, the libraries needed to use GIF.
#  GIF_FOUND, If false, do not try to use GIF.
# also defined, but not for general use are
#  GIF_LIBRARY, where to find the GIF library.

FIND_PATH(GIF_INCLUDE_DIR gif_lib.h
/usr/local/include
/usr/include
)

SET(GIF_NAMES ${GIF_NAMES} gif)
FIND_LIBRARY(GIF_LIBRARY
  NAMES ${GIF_NAMES}
  PATHS /usr/lib /usr/local/lib
  )

IF (GIF_LIBRARY AND GIF_INCLUDE_DIR)
    SET(GIF_LIBRARIES ${GIF_LIBRARY})
    SET(GIF_FOUND "YES")
ELSE (GIF_LIBRARY AND GIF_INCLUDE_DIR)
  SET(GIF_FOUND "NO")
ENDIF (GIF_LIBRARY AND GIF_INCLUDE_DIR)


IF (GIF_FOUND)
   IF (NOT GIF_FIND_QUIETLY)
      MESSAGE(STATUS "Found GIF: ${GIF_LIBRARIES}")
   ENDIF (NOT GIF_FIND_QUIETLY)
ELSE (GIF_FOUND)
   IF (GIF_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find GIF library")
   ENDIF (GIF_FIND_REQUIRED)
ENDIF (GIF_FOUND)

# Deprecated declarations.
SET (NATIVE_GIF_INCLUDE_PATH ${GIF_INCLUDE_DIR} )
GET_FILENAME_COMPONENT (NATIVE_GIF_LIB_PATH ${GIF_LIBRARY} PATH)

MARK_AS_ADVANCED(
  GIF_LIBRARY
  GIF_INCLUDE_DIR
  )
