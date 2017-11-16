# Stefan Achatz 2014
#
# Tries to find out if the target is a 64bit machine and sets paths and naming
# conventions according to target.
# following variables are set:
#
# INSTALL_LIBDIR
#
# Does nothing if INSTALL_LIBDIR is already set.

IF(NOT INSTALL_LIBDIR)
  FIND_PACKAGE(PkgConfig)
  PKG_CHECK_MODULES(PKG_TEST glib-2.0)
  
  IF(PKG_TEST_LIBDIR)
    SET(INSTALL_LIBDIR ${PKG_TEST_LIBDIR})
  ENDIF()

  SET(INSTALL_LIBDIR "${INSTALL_LIBDIR}" CACHE PATH "" FORCE)
  MESSAGE(STATUS "Installing libraries in ${INSTALL_LIBDIR}")
ENDIF()

IF(NOT INSTALL_LIBDIR)
  EXECUTE_PROCESS(COMMAND ${CMAKE_C_COMPILER} -dumpmachine
    OUTPUT_VARIABLE MACHINE
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  MESSAGE(STATUS "Building for target ${MACHINE}")

  STRING(REGEX MATCH
    "(x86_64-*)|(X86_64-*)|(AMD64-*)|(amd64-*)"
    _machine_x86_64
    "${MACHINE}"
  )

  IF(_machine_x86_64)
    SET(INSTALL_LIBDIR /lib64)
  ELSE()
    SET(INSTALL_LIBDIR /lib)
  ENDIF()

  SET(INSTALL_LIBDIR "${INSTALL_LIBDIR}" CACHE PATH "" FORCE)
  MESSAGE(STATUS "Installing libraries in ${INSTALL_LIBDIR}")
ENDIF()
