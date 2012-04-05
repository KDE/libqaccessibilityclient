set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )


kde4_add_unit_test(libkdeaccessibilityclienttest libkdeaccessibilityclienttest.cpp)
target_link_libraries(libkdeaccessibilityclienttest libkdeaccessibilityclient
    ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY} ${QT_QTDBUS_LIBRARY})


include_directories( .. )


