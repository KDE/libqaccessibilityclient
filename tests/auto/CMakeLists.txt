include_directories(
    ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

find_package(Qt5Test QUIET)
add_definitions(${Qt5Core_DEFINITIONS} ${Qt5DBus_DEFINITIONS} ${Qt5Widgets_DEFINITIONS} ${Qt5Test_DEFINITIONS})
include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5Test_INCLUDE_DIRS})
add_definitions(${Qt5Core_EXECUTABLE_COMPILE_FLAGS})
set(CMAKE_CXX_FLAGS ${Qt5Core_EXECUTABLE_COMPILE_FLAGS})

# The unittest app
set(tst_accessibilityclient_SRCS
    tst_accessibilityclient.cpp
    ${CMAKE_SOURCE_DIR}/src/atspi/dbusconnection.cpp
)
add_executable(tst_accessibilityclient ${tst_accessibilityclient_SRCS})

target_link_libraries(tst_accessibilityclient ${Qt5Widgets_LIBRARIES} ${Qt5Test_LIBRARIES} ${QACCESSIBILITYCLIENT_LIB_NAME})

add_test(NAME libkdeaccessibilityclient-tst_accessibilityclient COMMAND tst_accessibilityclient)

# A test app that can run in a QProcess
set(simplewidgetapp_SRCS
    simple/simplewidgetapp.cpp
)
add_executable(simplewidgetapp ${simplewidgetapp_SRCS})

target_link_libraries(simplewidgetapp ${Qt5Widgets_LIBRARIES} ${Qt5Test_LIBRARIES} ${QACCESSIBILITYCLIENT_LIB_NAME})
