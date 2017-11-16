CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/gaminggear/gaminggear.h.in ${CMAKE_CURRENT_BINARY_DIR}/gaminggear/gaminggear.h)

ADD_CUSTOM_TARGET(generated_headers ALL
  DEPENDS
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
    ${CMAKE_CURRENT_BINARY_DIR}/gaminggear/gaminggear.h
)

ADD_SUBDIRECTORY(gaminggear)
