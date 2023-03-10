# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcm_fonts\")

########### next target ###############

include_directories(../kfontinst/lib)

set(kcm_fonts_PART_SRCS ../krdb/krdb.cpp previewrenderengine.cpp previewimageprovider.cpp fonts.cpp fontsaasettings.cpp kxftconfig.cpp ../kfontinst/lib/FcEngine.cpp)

if(X11_FOUND)
    set(kcm_fonts_PART_SRCS ${kcm_fonts_PART_SRCS} ${libkxftconfig_SRCS})
endif()

kcmutils_generate_module_data(
    kcm_fonts_PART_SRCS
    MODULE_DATA_HEADER fontsdata.h
    MODULE_DATA_CLASS_NAME FontsData
    SETTINGS_HEADERS fontssettings.h fontsaasettings.h
    SETTINGS_CLASSES FontsSettings FontsAASettings
)

kconfig_add_kcfg_files(kcm_fonts_PART_SRCS fontssettings.kcfgc fontsaasettingsbase.kcfgc GENERATE_MOC)

add_library(kcm_fonts MODULE ${kcm_fonts_PART_SRCS})

target_link_libraries(kcm_fonts Qt::DBus Qt::Xml KF5::KCMUtils KF5::I18n KF5::KDELibs4Support kfontinst)

if(X11_FOUND)
    target_link_libraries(kcm_fonts
        Qt::X11Extras
        KF5::I18n
        KF5::Plasma
        KF5::PlasmaQuick
        KF5::Declarative
        KF5::QuickAddons
        KF5::WidgetsAddons
        PW::KWorkspace
        XCB::IMAGE
        )

    target_link_libraries(kcm_fonts X11::X11 X11::Xft)
endif()

kcoreaddons_desktop_to_json(kcm_fonts "kcm_fonts.desktop")

########### install files ###############
install(FILES fontssettings.kcfg DESTINATION ${KDE_INSTALL_KCFGDIR})
install(TARGETS kcm_fonts DESTINATION ${KDE_INSTALL_PLUGINDIR}/kcms)
install(FILES kcm_fonts.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
kpackage_install_package(package kcm_fonts kcms)
