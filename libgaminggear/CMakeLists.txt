ADD_CUSTOM_COMMAND(
  OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/g_cclosure_gaminggear_marshaller.h
    ${CMAKE_CURRENT_BINARY_DIR}/g_cclosure_gaminggear_marshaller.c
  COMMAND glib-genmarshal --header --prefix=g_cclosure_gaminggear_marshal ${CMAKE_CURRENT_SOURCE_DIR}/g_gaminggear_marshaller >${CMAKE_CURRENT_BINARY_DIR}/g_cclosure_gaminggear_marshaller.h
  COMMAND glib-genmarshal --body --prefix=g_cclosure_gaminggear_marshal ${CMAKE_CURRENT_SOURCE_DIR}/g_gaminggear_marshaller >${CMAKE_CURRENT_BINARY_DIR}/g_cclosure_gaminggear_marshaller.c
  DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/g_gaminggear_marshaller
)

SET(SOURCES
  gaminggear_audio_notificator_canberra.c
  gaminggear_configuration.c
  gaminggear_device.c
  gaminggear_helper.c
  gaminggear_key_file.c
  gaminggear_key_translations.c
  gaminggear_macro_keystrokes.c
  gaminggear_macro_thread.c
  gaminggear_macro_threads.c
  gaminggear_macro.c
  gaminggear_macros.c
  gaminggear_profile_data.c
  gaminggear_profiles.c
  gaminggear_screen_notificator_notify.c
  macros_converter_roccat_arvo.c
  macros_converter_roccat_kone.c
  macros_converter_roccat_swarm.c
  macros_converter_roccat_valo.c
  macros_converter_roccat.c
  ${CMAKE_CURRENT_BINARY_DIR}/g_cclosure_gaminggear_marshaller.c
)

SET(LIBRARIES
  ${CANBERRA_LIBRARIES}
  ${GLIB2_LIBRARIES}
  ${NOTIFY_LIBRARIES}
)

IF(SQLITE3_FOUND)
  LIST(APPEND SOURCES macros_converter_steelseries_engine3.c)
  LIST(APPEND LIBRARIES ${SQLITE3_LIBRARIES})
ENDIF()

IF(UHID_FOUND)
  LIST(APPEND SOURCES gaminggear_input_events_uhid.c)
ELSEIF(UINPUT_FOUND)
  LIST(APPEND SOURCES gaminggear_input_events_uinput.c)
ENDIF()

ADD_LIBRARY (libgaminggear SHARED ${SOURCES})

ADD_DEPENDENCIES(libgaminggear
  generated_headers
)

TARGET_LINK_LIBRARIES(libgaminggear ${LIBRARIES})

TARGET_LINK_LIBRARIES(libgaminggear LINK_INTERFACE_LIBRARIES)

SET_TARGET_PROPERTIES(libgaminggear
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_MICRO}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME gaminggear
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"GamingGear\\\""
)

INSTALL(TARGETS libgaminggear DESTINATION ${INSTALL_LIBDIR})
