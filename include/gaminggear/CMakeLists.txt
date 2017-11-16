IF(WITH_DOC)
  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

  ADD_CUSTOM_COMMAND(
    OUTPUT
      ${CMAKE_CURRENT_BINARY_DIR}/html/index.html
    COMMAND doxygen
    DEPENDS
      ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
      ${CMAKE_CURRENT_SOURCE_DIR}/*.h
  )

  ADD_CUSTOM_TARGET(documentation ALL
    DEPENDS
      ${CMAKE_CURRENT_BINARY_DIR}/html/index.html
  )
  
  INSTALL(
    DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
    DESTINATION share/doc/libgaminggear
  )
ENDIF()

INSTALL(
  FILES
    audio_notificator.h
    input_events.h
    gaminggear_device.h
    gaminggear_fx_plugin.h
    gaminggear_hid_combo_box.h
    gaminggear_hscale.h
    gaminggear_macro_dialog.h
    gaminggear_macro_editor_dialog.h
    gaminggear_profile_data.h
    gaminggear_profile_page.h
    gaminggear_profile_table.h
    gaminggear_text_dialog.h
    gaminggear_timeout_bar.h
    gaminggear_timeout_dialog.h
    gaminggear_xy_scales.h
	${CMAKE_CURRENT_BINARY_DIR}/gaminggear.h
    gdk_key_translations.h
    gfx.h
    hid_uid.h
    key_translations.h
    macro.h
    macro_threads.h
    profiles.h
    screen_notificator.h
    threads.h
  DESTINATION include/gaminggear-${V_MAJOR}/gaminggear
)
