cmake_policy(SET CMP0048 NEW)
project(accessibleapps VERSION "")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++0x")

include_directories(
    ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

#To find the installed library:
#set(LibKdeAccessibilityClient_DIR /usr/local/lib/cmake/)
#find_package(LibKdeAccessibilityClient REQUIRED)

#Instead we just use the one in the build directory since that's what we want to test.

set(accessibleapps_SRCS
    main.cpp
    mainwindow.cpp
    accessibletree.cpp
    accessibleproperties.cpp
    clientcachedialog.cpp
    eventview.cpp
    uiview.cpp
)

set(accessibleapps_UIS
    eventview.ui
)

qt5_wrap_ui(accessibleapps_UI_HDRS ${accessibleapps_UIS})

add_executable(accessibleapps ${accessibleapps_SRCS} ${accessibleapps_MOC} ${accessibleapps_UI_HDRS})

target_link_libraries(accessibleapps
    Qt5::Widgets
    ${QACCESSIBILITYCLIENT_LIB_NAME}
)

install(TARGETS accessibleapps DESTINATION bin)
