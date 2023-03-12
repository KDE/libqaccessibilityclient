set(QACCESSIBILITYCLIENT_LIB_NAME "qaccessibilityclient-qt${QT_MAJOR_VERSION}")

add_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII)
add_definitions(-DQACCESSIBILITYCLIENT_LIBRARY)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX QACCESSIBILITYCLIENT
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/QAccessibilityClientConfigVersion.cmake"
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/libqaccessibilityclient-version.h"
)

add_library(QAccessibilityClient SHARED)

set_target_properties(QAccessibilityClient PROPERTIES
    OUTPUT_NAME ${QACCESSIBILITYCLIENT_LIB_NAME}
    EXPORT_NAME ${QACCESSIBILITYCLIENT_LIB_NAME}
    VERSION   ${QACCESSIBILITYCLIENT_VERSION}
    SOVERSION ${QACCESSIBILITYCLIENT_SOVERSION}
)

target_sources(QAccessibilityClient PRIVATE
    qaccessibilityclient/accessibleobject_p.cpp
    qaccessibilityclient/accessibleobject.cpp
    qaccessibilityclient/registry.cpp
    qaccessibilityclient/registry_p.cpp

    atspi/dbusconnection.cpp
    atspi/qt-atspi.cpp
)

generate_export_header(QAccessibilityClient
    BASE_NAME qaccessibilityclient
)
target_link_libraries(QAccessibilityClient
    PUBLIC
        Qt${QT_MAJOR_VERSION}::Widgets
        Qt${QT_MAJOR_VERSION}::Core
    PRIVATE
        Qt${QT_MAJOR_VERSION}::DBus
)

target_include_directories(QAccessibilityClient
    INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}>"
)


install(TARGETS QAccessibilityClient EXPORT QAccessibilityClient ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/qaccessibilityclient_export.h
    qaccessibilityclient/accessibleobject.h
    qaccessibilityclient/registry.h
    ${CMAKE_CURRENT_BINARY_DIR}/libqaccessibilityclient-version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/qaccessibilityclient COMPONENT Devel
)

set(_QAccessibilityClient_CONFIG_DEST "${KDE_INSTALL_CMAKEPACKAGEDIR}/${PROJECT_NAME}")

install(EXPORT QAccessibilityClient DESTINATION ${_QAccessibilityClient_CONFIG_DEST})

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/QAccessibilityClientConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/QAccessibilityClientConfig.cmake"
    INSTALL_DESTINATION  ${_QAccessibilityClient_CONFIG_DEST}
)

install( FILES
         "${CMAKE_CURRENT_BINARY_DIR}/QAccessibilityClientConfig.cmake"
         "${CMAKE_CURRENT_BINARY_DIR}/QAccessibilityClientConfigVersion.cmake"
         DESTINATION "${_QAccessibilityClient_CONFIG_DEST}" )
