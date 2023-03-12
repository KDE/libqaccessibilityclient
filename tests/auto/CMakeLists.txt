# The unittest app
add_executable(tst_accessibilityclient)

target_sources(tst_accessibilityclient PRIVATE
    tst_accessibilityclient.cpp
    ${CMAKE_SOURCE_DIR}/src/atspi/dbusconnection.cpp
)

target_link_libraries(tst_accessibilityclient
    QAccessibilityClient
    Qt${QT_MAJOR_VERSION}::Widgets
    Qt${QT_MAJOR_VERSION}::DBus
    Qt${QT_MAJOR_VERSION}::Test
)

add_test(NAME libkdeaccessibilityclient-tst_accessibilityclient COMMAND tst_accessibilityclient)

# A test app that can run in a QProcess
add_executable(simplewidgetapp)

target_sources(simplewidgetapp PRIVATE
    simple/simplewidgetapp.cpp
)

target_link_libraries(simplewidgetapp
    QAccessibilityClient
    Qt${QT_MAJOR_VERSION}::Widgets
    Qt${QT_MAJOR_VERSION}::Test
)
