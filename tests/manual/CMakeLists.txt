project(accessibleapps)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)

#To find the installed library:
#set(LibKdeAccessibilityClient_DIR /usr/local/lib/cmake/)
#find_package(LibKdeAccessibilityClient REQUIRED)

#Instead we just use the one in the build directory since that's what we want to test.

set(accessibleapps_SRCS
    main.cpp
    mainwindow.cpp
)

kde4_add_ui_files(accessibleapps_SRCS
    mainwindow.ui
)

kde4_add_executable(accessibleapps ${accessibleapps_SRCS})

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES} ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(accessibleapps ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${KDE4_KDEUI_LIBRARY} kdeaccessibilityclient)

