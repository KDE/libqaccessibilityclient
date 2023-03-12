cmake_minimum_required(VERSION 3.16.0)

project(accessibleapps)

#To find the installed library:
#set(LibKdeAccessibilityClient_DIR /usr/local/lib/cmake/)
#find_package(LibKdeAccessibilityClient REQUIRED)

#Instead we just use the one in the build directory since that's what we want to test.

add_executable(accessibleapps)

qt_wrap_ui(accessibleapps_UI_HDRS
    eventview.ui
)

target_sources(accessibleapps PRIVATE
    main.cpp
    mainwindow.cpp
    accessibletree.cpp
    accessibleproperties.cpp
    clientcachedialog.cpp
    eventview.cpp
    uiview.cpp

    ${accessibleapps_UI_HDRS}
)

target_link_libraries(accessibleapps
    QAccessibilityClient
    Qt${QT_MAJOR_VERSION}::Widgets
)

install(TARGETS accessibleapps DESTINATION bin)
