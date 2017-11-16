FOREACH(size 16)
  FOREACH(name link link_break press release wait)
    INSTALL(FILES ${name}_${size}x${size}x32.png
      DESTINATION share/libgaminggear/icons/hicolor/${size}x${size}/actions
      RENAME gaminggear-${name}.png
    )
  ENDFOREACH()
ENDFOREACH()
