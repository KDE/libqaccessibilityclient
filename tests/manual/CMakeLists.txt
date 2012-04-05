project(accessibleapps)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)


#FIXME
set(LibKdeAccessibilityClient_DIR /usr/local/lib/cmake/)

message(${CMAKE_MODULE_PATH})

set(CMAKE_MODULE_PATH /usr/local/lib/cmake)
find_package(LibKdeAccessibilityClient REQUIRED)


set(accessibleapps_SRCS
    main.cpp
)
kde4_add_executable(accessibleapps ${accessibleapps_SRCS})

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES} ${LIBKDEACCESSIBILITYCLIENT_INCLUDE_DIR})
target_link_libraries(accessibleapps ${QT_QTCORE_LIBRARY} ${LIBKDEACCESSIBILITYCLIENT_LIBS})


