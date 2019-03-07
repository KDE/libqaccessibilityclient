include_directories(
    ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

# The unittest app
set(tst_accessibilityclient_SRCS
    tst_accessibilityclient.cpp
    ${CMAKE_SOURCE_DIR}/src/atspi/dbusconnection.cpp
)
add_executable(tst_accessibilityclient ${tst_accessibilityclient_SRCS})

target_link_libraries(tst_accessibilityclient
    Qt5::Widgets
    Qt5::DBus
    Qt5::Test
    ${QACCESSIBILITYCLIENT_LIB_NAME}
)

add_test(NAME libkdeaccessibilityclient-tst_accessibilityclient COMMAND tst_accessibilityclient)

# A test app that can run in a QProcess
set(simplewidgetapp_SRCS
    simple/simplewidgetapp.cpp
)
add_executable(simplewidgetapp ${simplewidgetapp_SRCS})

target_link_libraries(simplewidgetapp
    Qt5::Widgets
    Qt5::Test
    ${QACCESSIBILITYCLIENT_LIB_NAME}
)
