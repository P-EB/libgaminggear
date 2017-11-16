# Stefan Achatz 2015
#
# Tries to find location of uinput device.
# Header is used to determine if uinput is available for compilation. Build system
# doesn't need to actually load module uinput. If device path is not found a default
# is used which can be overwritten by setting UINPUT_DIR explicitly.
# Following vairables are set:
#
# UINPUT_FOUND
# UINPUT_INCLUDEDIR
# UINPUT_DIR

FIND_PATH(UINPUT_INCLUDEDIR linux/uinput.h)

IF(NOT UINPUT_DIR)
  FIND_PATH(UINPUT_DIR uinput
    HINTS
      /dev
      /dev/input
  )  
ENDIF()

IF(NOT UINPUT_DIR)
  SET(UINPUT_DIR "/dev")
ENDIF()

IF(UINPUT_INCLUDEDIR)
  SET(UINPUT_FOUND true)
  MESSAGE(STATUS "Using ${UINPUT_DIR}/uinput")
ENDIF()

IF(UINPUT_FIND_REQUIRED AND NOT UINPUT_FOUND)
  MESSAGE(FATAL_ERROR "Could not find UINPUT")
ENDIF()
