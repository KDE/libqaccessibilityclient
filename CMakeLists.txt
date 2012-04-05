project(LibKdeAccessibilityClient)

cmake_minimum_required(VERSION 2.6)
add_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII)
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

configure_file( "${LibKdeAccessibilityClient_SOURCE_DIR}/LibKdeAccessibilityClient.cmake.in" "${LibKdeAccessibilityClient_BINARY_DIR}/LibKdeAccessibilityClient.cmake" @ONLY )

find_package(KDE4 REQUIRED)
include(KDE4Defaults)

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

# Require libatspi2-core headers for building:
include(FindPkgConfig)
pkg_check_modules(ATSPI2 REQUIRED atspi-2)
include_directories(${ATSPI2_INCLUDE_DIRS})

set(CMAKE_INSTALL_NAME_DIR ${LIB_INSTALL_DIR})

add_subdirectory(src)
add_subdirectory(tests)

if(KDE4_USE_COMMON_CMAKE_PACKAGE_CONFIG_DIR)
    set(_LibKdeAccessibilityClient_CONFIG_DEST "${LIB_INSTALL_DIR}/cmake/libkdeaccessibilityclient")
else(KDE4_USE_COMMON_CMAKE_PACKAGE_CONFIG_DIR)
    set(_LibKdeAccessibilityClient_CONFIG_DEST "${LIB_INSTALL_DIR}/libkdeaccessibilityclient")
endif(KDE4_USE_COMMON_CMAKE_PACKAGE_CONFIG_DIR)


install( FILES
         "${LibKdeAccessibilityClient_BINARY_DIR}/LibKdeAccessibilityClient.cmake"
         DESTINATION "${_LibKdeAccessibilityClient_CONFIG_DEST}" )

# install( EXPORT LibKdeAccessibilityClientTargets
#          DESTINATION "${_LibKdeAccessibilityClient_CONFIG_DEST}"
#          NAMESPACE LibKdeAccessibilityClient__
#          FILE LibKdeAccessibilityClient.cmake )
