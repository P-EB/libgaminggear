# Stefan Achatz 2014
#
# Sets install path for cmake modules.
# Following variables are set:
#
# INSTALL_CMAKE_MODULESDIR
#
# Does nothing if INSTALL_CMAKE_MODULESDIR is already set.

IF(NOT INSTALL_CMAKE_MODULESDIR)

  FILE(GLOB POSSIBLE_PATHS /usr/share/cmake*)
  LIST(SORT POSSIBLE_PATHS)
  LIST(REVERSE POSSIBLE_PATHS)

  FIND_PATH(INSTALL_CMAKE_MODULESDIR Modules
    PATHS
      ${POSSIBLE_PATHS}
  )
  
  IF(NOT INSTALL_CMAKE_MODULESDIR)
    MESSAGE(FATAL_ERROR "Don't know where to install cmake modules. Please set INSTALL_CMAKE_MODULESDIR manually.")
  ENDIF()
 
  SET(INSTALL_CMAKE_MODULESDIR "${INSTALL_CMAKE_MODULESDIR}/Modules" CACHE PATH "" FORCE)
  MESSAGE(STATUS "Installing cmake modules in ${INSTALL_CMAKE_MODULESDIR}")
ENDIF()
