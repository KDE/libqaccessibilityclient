
set(LibKdeAccessibilityClient_SRCS

)

kde4_add_library(libkdeaccessibilityclient SHARED ${LibKdeAccessibilityClient_SRCS})

target_link_libraries(libkdeaccessibilityclient ${QT_QTDBUS_LIBRARY})

set(LIBKDEACCESSIBILITYCLIENT_VERSION "0.1.0")
set(LIBKDEACCESSIBILITYCLIENT_SOVERSION "0")
set_target_properties(libkdeaccessibilityclient
    PROPERTIES VERSION ${LIBKDEACCESSIBILITYCLIENT_VERSION}
    SOVERSION ${LIBKDEACCESSIBILITYCLIENT_SOVERSION}
)


install(TARGETS libkdeaccessibilityclient EXPORT LibKdeAccessibilityClient ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install(FILES
    libkdeaccessibilityclient_export.h

    DESTINATION ${INCLUDE_INSTALL_DIR}/libkdeaccessibilityclient COMPONENT Devel
)
