include_directories(
    ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

add_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII)
add_definitions(-DQACCESSIBILITYCLIENT_LIBRARY)

set(QAccessibilityClient_SRCS
    qaccessibilityclient/accessibleobject_p.cpp
    qaccessibilityclient/accessibleobject.cpp
    qaccessibilityclient/registry.cpp
    qaccessibilityclient/registry_p.cpp
)

set(QAccessibilityClientPrivate_SRCS
    atspi/dbusconnection.cpp
    atspi/qt-atspi.cpp
)

add_library(qaccessibilityclient SHARED ${QAccessibilityClient_SRCS} ${QAccessibilityClientPrivate_SRCS})

if (QT5_BUILD)
    target_link_libraries(qaccessibilityclient ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5DBus_LIBRARIES})
else (QT5_BUILD)
    target_link_libraries(qaccessibilityclient ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY})
endif(QT5_BUILD)

set(QACCESSIBILITYCLIENT_SOVERSION ${QACCESSIBILITYCLIENT_VERSION_MAJOR})
set_target_properties(qaccessibilityclient
    PROPERTIES VERSION ${QACCESSIBILITYCLIENT_VERSION}
    SOVERSION ${QACCESSIBILITYCLIENT_SOVERSION}
)


install(TARGETS qaccessibilityclient EXPORT QAccessibilityClient ${INSTALL_TARGETS_DEFAULT_ARGS})
install(EXPORT QAccessibilityClient DESTINATION ${_QAccessibilityClient_CONFIG_DEST})

########### install files ###############

install(FILES
    qaccessibilityclient/qaccessibilityclient_export.h
    qaccessibilityclient/accessibleobject.h
    qaccessibilityclient/registry.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/qaccessibilityclient COMPONENT Devel
)
