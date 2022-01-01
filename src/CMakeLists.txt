add_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII)
add_definitions(-DQACCESSIBILITYCLIENT_LIBRARY)

add_library(${QACCESSIBILITYCLIENT_LIB_NAME} SHARED)

target_sources(${QACCESSIBILITYCLIENT_LIB_NAME} PRIVATE
    qaccessibilityclient/accessibleobject_p.cpp
    qaccessibilityclient/accessibleobject.cpp
    qaccessibilityclient/registry.cpp
    qaccessibilityclient/registry_p.cpp

    atspi/dbusconnection.cpp
    atspi/qt-atspi.cpp
)

generate_export_header(${QACCESSIBILITYCLIENT_LIB_NAME}
    BASE_NAME qaccessibilityclient
)
target_link_libraries(${QACCESSIBILITYCLIENT_LIB_NAME}
    PUBLIC
        Qt${QT_MAJOR_VERSION}::Widgets
        Qt${QT_MAJOR_VERSION}::Core
    PRIVATE
        Qt${QT_MAJOR_VERSION}::DBus
)

set_target_properties(${QACCESSIBILITYCLIENT_LIB_NAME}
    PROPERTIES VERSION ${PROJECT_VERSION}
    SOVERSION ${QACCESSIBILITYCLIENT_SOVERSION}
)
target_include_directories(${QACCESSIBILITYCLIENT_LIB_NAME}
    INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}>"
)


install(TARGETS ${QACCESSIBILITYCLIENT_LIB_NAME} EXPORT QAccessibilityClient ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(EXPORT QAccessibilityClient DESTINATION ${_QAccessibilityClient_CONFIG_DEST})

########### install files ###############

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/qaccessibilityclient_export.h
    qaccessibilityclient/accessibleobject.h
    qaccessibilityclient/registry.h
    ${CMAKE_CURRENT_BINARY_DIR}/libqaccessibilityclient-version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/qaccessibilityclient COMPONENT Devel
)
