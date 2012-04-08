
add_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII)

set(LibKdeAccessibilityClient_SRCS
    kdeaccessibilityclient/accessibleobject.cpp
    kdeaccessibilityclient/registry.cpp
)

set(LibKdeAccessibilityClientPrivate_SRCS
    atspi/dbusconnection.cpp
    atspi/qt-atspi.cpp
)

kde4_add_library(kdeaccessibilityclient SHARED ${LibKdeAccessibilityClient_SRCS} ${LibKdeAccessibilityClientPrivate_SRCS})

target_link_libraries(kdeaccessibilityclient ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY})

set(LIBKDEACCESSIBILITYCLIENT_SOVERSION ${LIBKDEACCESSIBILITYCLIENT_VERSION_MAJOR})
set_target_properties(kdeaccessibilityclient
    PROPERTIES VERSION ${LIBKDEACCESSIBILITYCLIENT_VERSION}
    SOVERSION ${LIBKDEACCESSIBILITYCLIENT_SOVERSION}
)


install(TARGETS kdeaccessibilityclient EXPORT LibKdeAccessibilityClient ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install(FILES
    kdeaccessibilityclient/libkdeaccessibilityclient_export.h
    kdeaccessibilityclient/accessibleobject.h
    kdeaccessibilityclient/registry.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/kdeaccessibilityclient COMPONENT Devel
)
