cmake_minimum_required(VERSION 3.16.0)

project(dumper)

#To find the installed library:
#set(LibKdeAccessibilityClient_DIR /usr/local/lib/cmake/)
#find_package(LibKdeAccessibilityClient REQUIRED)

#Instead we just use the one in the build directory since that's what we want to test.

add_executable(dumper)

target_sources(dumper PRIVATE
    main.cpp
    dumper.cpp
    dumper.h
)

target_link_libraries(dumper
    QAccessibilityClient
    Qt${QT_MAJOR_VERSION}::Widgets
)

install(TARGETS dumper DESTINATION bin)
