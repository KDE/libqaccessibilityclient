project(accessibleapps)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++0x")

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

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
    eventview.cpp
    uiview.cpp
    tests_auto_modeltest_modeltest.cpp
)

set(accessibleapps_UIS
    eventview.ui
)

qt4_wrap_ui(accessibleapps_UI_HDRS ${accessibleapps_UIS})

add_executable(accessibleapps ${accessibleapps_SRCS} ${accessibleapps_MOC} ${accessibleapps_UI_HDRS})
target_link_libraries(accessibleapps ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY} ${KDE4_KDEUI_LIBRARY} qaccessibilityclient)
