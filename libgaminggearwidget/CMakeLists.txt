INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/libgaminggear
  ${CMAKE_BINARY_DIR}/libgaminggear
)

ADD_LIBRARY (libgaminggearwidget SHARED
  gaminggear_about_dialog.c
  gaminggear_dialogs.c
  gaminggear_dscale.c
  gaminggear_gdk_key_translations.c
  gaminggear_hid_combo_box.c
  gaminggear_hscale.c
  gaminggear_macro_combo_box.c
  gaminggear_macro_dialog.c
  gaminggear_macro_editor_advanced_list_store.c
  gaminggear_macro_editor_advanced_table.c
  gaminggear_macro_editor_advanced.c
  gaminggear_macro_editor_basic_list_store.c
  gaminggear_macro_editor_basic_tree_view.c
  gaminggear_macro_editor_basic.c
  gaminggear_macro_editor_dialog_menu_bar.c
  gaminggear_macro_editor_dialog.c
  gaminggear_macro_editor_interface.c
  gaminggear_macro_editor_key_sequence_frame.c
  gaminggear_macro_editor_macro_options_frame.c
  gaminggear_macro_editor_macros_frame.c
  gaminggear_macro_editor_record_options_frame.c
  gaminggear_macro_editor.c
  gaminggear_macro_tree_store.c
  gaminggear_macro_tree_view.c
  gaminggear_profile_list_store.c
  gaminggear_profile_page.c
  gaminggear_profile_table.c
  gaminggear_ruler.c
  gaminggear_text_dialog.c
  gaminggear_timeout_bar.c
  gaminggear_timeout_dialog.c
  gaminggear_xy_scales.c
  gaminggearwidget.c
  gaminggearwidget_helper.c
  gtk_gaminggear_helper.c
)

ADD_DEPENDENCIES(libgaminggearwidget
  generated_headers
)

TARGET_LINK_LIBRARIES(libgaminggearwidget
  libgaminggear
  ${M_LIBRARY}
  ${GTK_LIBRARIES}
  ${GDK_LIBRARIES}
  ${RT_LIBRARY}
)

TARGET_LINK_LIBRARIES(libgaminggearwidget LINK_INTERFACE_LIBRARIES)

SET_TARGET_PROPERTIES(libgaminggearwidget
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_MICRO}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME gaminggearwidget
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"GamingGear\\\""
)

INSTALL(TARGETS libgaminggearwidget DESTINATION ${INSTALL_LIBDIR})
