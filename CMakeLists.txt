project(LibKdeAccessibilityClient)

set(LIBKDEACCESSIBILITYCLIENT_VERSION_MAJOR 0)
set(LIBKDEACCESSIBILITYCLIENT_VERSION_MINOR 1)
set(LIBKDEACCESSIBILITYCLIENT_VERSION_PATCH 0)
set(LIBKDEACCESSIBILITYCLIENT_VERSION ${LIBKDEACCESSIBILITYCLIENT_VERSION_MAJOR}.${LIBKDEACCESSIBILITYCLIENT_VERSION_MINOR}.${LIBKDEACCESSIBILITYCLIENT_VERSION_PATCH})

cmake_minimum_required(VERSION 2.6)
add_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII)
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

# override the KDE defaults and build tests unless explicitly disabled
option(KDE4_BUILD_TESTS  "Build the tests" ON)

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

# we need the absolute directories where stuff will be installed too
# but since the variables which contain the destinations can be relative
# or absolute paths, we need this macro to make them all absoulte, Alex
macro(MAKE_INSTALL_PATH_ABSOLUTE out in)
   if (IS_ABSOLUTE "${in}")    # IS_ABSOLUTE is new since cmake 2.4.8
      set(${out} "${in}")
   else()
      set(${out} "\${KDEPIMLIBS_INSTALL_DIR}/${in}")
   endif()
endmacro(MAKE_INSTALL_PATH_ABSOLUTE out in)

make_install_path_absolute(LIBKDEACCESSIBILITYCLIENT_INCLUDE_DIR ${INCLUDE_INSTALL_DIR})
make_install_path_absolute(LIBKDEACCESSIBILITYCLIENT_LIB_DIR     ${LIB_INSTALL_DIR})

configure_file( "${LibKdeAccessibilityClient_SOURCE_DIR}/LibKdeAccessibilityClientConfig.cmake.in" "${LibKdeAccessibilityClient_BINARY_DIR}/LibKdeAccessibilityClientConfig.cmake" @ONLY )

if(KDE4_USE_COMMON_CMAKE_PACKAGE_CONFIG_DIR)
    set(_LibKdeAccessibilityClient_CONFIG_DEST "${LIB_INSTALL_DIR}/cmake/")
else(KDE4_USE_COMMON_CMAKE_PACKAGE_CONFIG_DIR)
    set(_LibKdeAccessibilityClient_CONFIG_DEST "${LIB_INSTALL_DIR}/libkdeaccessibilityclient")
endif(KDE4_USE_COMMON_CMAKE_PACKAGE_CONFIG_DIR)


install( FILES
         "${LibKdeAccessibilityClient_BINARY_DIR}/LibKdeAccessibilityClientConfig.cmake"
         DESTINATION "${_LibKdeAccessibilityClient_CONFIG_DEST}" )
