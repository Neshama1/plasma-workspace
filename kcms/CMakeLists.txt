add_subdirectory(desktoptheme)
add_subdirectory(icons)
add_subdirectory(translations)

if(KUserFeedback_FOUND)
    add_subdirectory(feedback)
endif()

add_subdirectory(style)
add_subdirectory(fonts)
add_subdirectory(krdb)
add_subdirectory(lookandfeel)
add_subdirectory(colors)
if(X11_Xcursor_FOUND)
    add_subdirectory(cursortheme)
endif()

set(libkxftconfig_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/fonts/kxftconfig.cpp )

if(FONTCONFIG_FOUND AND FREETYPE_FOUND)
    add_subdirectory( kfontinst )
endif()
