# Stefan Achatz 2015
#
# Tries to find location of uhid device.
# Header is used to determine if uhid is available for compilation. Build system
# doesn't need to actually load module uhid. If device path is not found a default
# is used which can be overwritten by setting UHID_DIR explicitly.
# Following vairables are set:
#
# UHID_FOUND
# UHID_INCLUDEDIR
# UHID_DIR

FIND_PATH(UHID_INCLUDEDIR linux/uhid.h)

IF(NOT UHID_DIR)
  FIND_PATH(UHID_DIR uhid
    HINTS
      /dev
      /dev/input
  )  
ENDIF()

IF(NOT UHID_DIR)
  SET(UHID_DIR "/dev")
ENDIF()

IF(UHID_INCLUDEDIR)
  SET(UHID_FOUND true)
  MESSAGE(STATUS "Using ${UHID_DIR}/uhid")
ENDIF()

IF(UHID_FIND_REQUIRED AND NOT UHID_FOUND)
  MESSAGE(FATAL_ERROR "Could not find UHID")
ENDIF()
