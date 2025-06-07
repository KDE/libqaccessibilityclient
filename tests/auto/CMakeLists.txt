# The unittest app
add_executable(tst_accessibilityclient)

target_sources(tst_accessibilityclient PRIVATE
    tst_accessibilityclient.cpp
    ${CMAKE_SOURCE_DIR}/src/atspi/dbusconnection.cpp
)

target_link_libraries(tst_accessibilityclient
    QAccessibilityClient
    Qt6::Widgets
    Qt6::DBus
    Qt6::Test
)

add_test(NAME libkdeaccessibilityclient-tst_accessibilityclient COMMAND tst_accessibilityclient)

# A test app that can run in a QProcess
add_executable(simplewidgetapp)

target_sources(simplewidgetapp PRIVATE
    simple/simplewidgetapp.cpp
)

target_link_libraries(simplewidgetapp
    QAccessibilityClient
    Qt6::Widgets
    Qt6::Test
)
