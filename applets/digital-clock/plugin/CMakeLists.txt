add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_org.kde.plasma.digitalclock\")

find_package(IsoCodes)
set_package_properties(IsoCodes PROPERTIES DESCRIPTION "ISO language, territory, currency, script codes and their translations"
                       URL "https://salsa.debian.org/iso-codes-team/iso-codes"
                       PURPOSE "Translation of country names in digital clock applet"
                       TYPE RUNTIME
                      )

set(digitalclockplugin_SRCS
    timezonemodel.cpp
    timezonesi18n.cpp
    digitalclockplugin.cpp
    clipboardmenu.cpp
    )


include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMPoQmTools)

#target_include_directories(digitalclockplugin PRIVATE "${GSTREAMER_INCLUDE_DIR}")

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
        Config
        I18n
        CoreAddons
        )


add_library(digitalclockplugin SHARED ${digitalclockplugin_SRCS})
target_link_libraries(digitalclockplugin Qt::Core
                                          Qt::Qml
					Qt::DBus
                                          Qt::Widgets # for QAction...
                                          KF5::CoreAddons
                                          KF5::I18n
					  KF5::ConfigCore)

install(TARGETS digitalclockplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/private/digitalclock)

install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/private/digitalclock)
