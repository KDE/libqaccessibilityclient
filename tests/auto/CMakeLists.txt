set(tst_accessibilityclient_SRCS
    tst_accessibilityclient.cpp
    ${CMAKE_SOURCE_DIR}/src/atspi/dbusconnection.cpp
)

kde4_add_unit_test(tst_accessibilityclient ${tst_accessibilityclient_SRCS})

target_link_libraries(tst_accessibilityclient ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY} ${QT_QTDBUS_LIBRARY} kdeaccessibilityclient)

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/src)

