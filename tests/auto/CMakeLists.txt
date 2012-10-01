include_directories(
    ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

if (QT5_BUILD)
    find_package(Qt5Test QUIET)
    add_definitions(${Qt5Core_DEFINITIONS} ${Qt5DBus_DEFINITIONS} ${Qt5Widgets_DEFINITIONS} ${Qt5Test_DEFINITIONS})
    include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5Test_INCLUDE_DIRS})
    add_definitions(${Qt5Core_EXECUTABLE_COMPILE_FLAGS})
    set(CMAKE_CXX_FLAGS ${Qt5Core_EXECUTABLE_COMPILE_FLAGS})
else (QT5_BUILD)
    include_directories(${QT_QTTEST_INCLUDE_DIR})
endif (QT5_BUILD)

# The unittest app
set(tst_accessibilityclient_SRCS
    tst_accessibilityclient.cpp
    ${CMAKE_SOURCE_DIR}/src/atspi/dbusconnection.cpp
)
add_executable(tst_accessibilityclient ${tst_accessibilityclient_SRCS})

if (QT5_BUILD)
    target_link_libraries(tst_accessibilityclient ${Qt5Widgets_LIBRARIES} ${Qt5Test_LIBRARIES} qaccessibilityclient)
else (QT5_BUILD)
    target_link_libraries(tst_accessibilityclient ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY} ${QT_QTDBUS_LIBRARY} qaccessibilityclient)
endif (QT5_BUILD)

add_test(libkdeaccessibilityclient-tst_accessibilityclient tst_accessibilityclient)

# A test app that can run in a QProcess
set(simplewidgetapp_SRCS
    simple/simplewidgetapp.cpp
)
add_executable(simplewidgetapp ${simplewidgetapp_SRCS})


if (QT5_BUILD)
    target_link_libraries(simplewidgetapp ${Qt5Widgets_LIBRARIES} ${Qt5Test_LIBRARIES} qaccessibilityclient)
else (QT5_BUILD)
    target_link_libraries(simplewidgetapp ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY} ${QT_QTDBUS_LIBRARY} qaccessibilityclient)
endif (QT5_BUILD)
