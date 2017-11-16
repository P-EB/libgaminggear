# Stefan Achatz 2014
#
# Sets install path for pkg-config modules.
# Following variables are set:
#
# INSTALL_PKGCONFIGDIR
#
# Does nothing if INSTALL_PKGCONFIGDIR is already set.

IF(NOT INSTALL_PKGCONFIGDIR)
  INCLUDE(INSTALL_LIBDIR)
  
  FIND_PATH(INSTALL_PKGCONFIGDIR pkgconfig
    HINTS
      /usr/share
      ${INSTALL_LIBDIR}
  )
  
  IF(NOT INSTALL_PKGCONFIGDIR)
    MESSAGE(FATAL_ERROR "Don't know where to install pkg-config files. Please set INSTALL_PKGCONFIGDIR manually.")
  ENDIF()
  
  SET(INSTALL_PKGCONFIGDIR "${INSTALL_PKGCONFIGDIR}/pkgconfig" CACHE PATH "" FORCE)
  MESSAGE(STATUS "Installing pkg-config module in ${INSTALL_PKGCONFIGDIR}")
ENDIF()

