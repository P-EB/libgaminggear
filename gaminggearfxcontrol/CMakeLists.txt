ADD_EXECUTABLE(gaminggearfxcontrol
  gaminggearfxcontrol.c
)

SET_TARGET_PROPERTIES(gaminggearfxcontrol
  PROPERTIES
    COMPILE_FLAGS "-fvisibility=hidden"
)

TARGET_LINK_LIBRARIES(gaminggearfxcontrol
  libgaminggearfx
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS gaminggearfxcontrol
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)
  
