set(tst_accessibilityclient_SRCS
    tst_accessibilityclient.cpp
    ${CMAKE_SOURCE_DIR}/src/atspi/dbusconnection.cpp
)

kde4_add_unit_test(tst_accessibilityclient ${tst_accessibilityclient_SRCS})
target_link_libraries(tst_accessibilityclient ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY} ${QT_QTDBUS_LIBRARY} kdeaccessibilityclient)

# A test app that can run in a QProcess
set(simplewidgetapp_SRCS
    simple/simplewidgetapp.cpp
)
kde4_add_executable(simplewidgetapp ${simplewidgetapp_SRCS})
target_link_libraries(simplewidgetapp ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/src)
