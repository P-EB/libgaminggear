# Stefan Achatz 2014
#
# Tries to find libusb and sets following variables according to found capabilities:
#
# SQLITE3_FOUND
# SQLITE3_INCLUDE_DIRS
# SQLITE3_LIBRARIES
# SQLITE3_VERSION

FIND_PACKAGE(PkgConfig)
PKG_CHECK_MODULES(PKG_SQLITE3 sqlite3)

FIND_PATH(SQLITE3_INCLUDE_DIRS sqlite3.h
  HINTS
    ${PKG_SQLITE3_INCLUDE_DIRS}
    ${PKG_SQLITE3_INCLUDEDIR}
)

FIND_LIBRARY(SQLITE3_LIBRARIES sqlite3
  HINTS ${PKG_SQLITE3_LIBRARY_DIRS}
)

IF(SQLITE3_LIBRARIES AND SQLITE3_INCLUDE_DIRS)
  FILE(READ "${SQLITE3_INCLUDE_DIRS}/sqlite3.h" _CONTENTS)
  IF(_CONTENTS)
    STRING(REGEX REPLACE ".*#define[ \t]+SQLITE_VERSION[ \t]+\"([0-9.]+)\".*" "\\1" SQLITE3_VERSION "${_CONTENTS}")
  ELSE()
    MESSAGE(FATAL_ERROR "Can't read ${SQLITE3_INCLUDE_DIRS}/sqlite3.h")
  ENDIF()

  IF(SQLITE3_FIND_VERSION)
    IF(SQLITE3_FIND_VERSION_EXACT)
      IF(SQLITE3_VERSION VERSION_EQUAL SQLITE3_FIND_VERSION)
        SET(SQLITE3_FOUND true)
      ELSE()
        SET(SQLITE3_FOUND false)
        MESSAGE("sqlite3 version mismatch ${SQLITE3_VERSION} != ${SQLITE3_FIND_VERSION}")
      ENDIF()
    ELSE()
      IF(SQLITE3_VERSION VERSION_EQUAL SQLITE3_FIND_VERSION OR
         SQLITE3_VERSION VERSION_GREATER SQLITE3_FIND_VERSION)
        SET(SQLITE3_FOUND true)
      ELSE()
        SET(SQLITE3_FOUND false)
        MESSAGE("sqlite3 version mismatch ${SQLITE3_VERSION} < ${SQLITE3_FIND_VERSION}")
      ENDIF()
    ENDIF()
  ELSE()
    SET(SQLITE3_FOUND true)
  ENDIF()
ENDIF()

IF(NOT SQLITE3_FOUND)
  UNSET(SQLITE3_INCLUDE_DIRS)
  UNSET(SQLITE3_LIBRARIES)
  UNSET(SQLITE3_VERSION)
ENDIF()

IF(SQLITE3_FIND_REQUIRED AND NOT SQLITE3_FOUND)
  MESSAGE(FATAL_ERROR "Could not find required SQLITE3")
ENDIF()
