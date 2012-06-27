project(LibKdeAccessibilityClient)

set(LIBKDEACCESSIBILITYCLIENT_VERSION_MAJOR 0)
set(LIBKDEACCESSIBILITYCLIENT_VERSION_MINOR 1)
set(LIBKDEACCESSIBILITYCLIENT_VERSION_PATCH 0)
set(LIBKDEACCESSIBILITYCLIENT_VERSION ${LIBKDEACCESSIBILITYCLIENT_VERSION_MAJOR}.${LIBKDEACCESSIBILITYCLIENT_VERSION_MINOR}.${LIBKDEACCESSIBILITYCLIENT_VERSION_PATCH})

# minimum required cmake is 2.8.6 for automoc-support
cmake_minimum_required(VERSION 2.8.6)

add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

set(CMAKE_AUTOMOC TRUE)

# override the KDE defaults and build tests unless explicitly disabled
#option(KDE4_BUILD_TESTS  "Build the tests" ON)

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}
    ${QT4_INCLUDES} ${QT_INCLUDE_DIR} ${QT_QT_INCLUDE_DIR} ${QT_QTDBUS_INCLUDE_DIR}
)

# Require libatspi2-core headers for building:
include(FindPkgConfig)
pkg_check_modules(ATSPI2 REQUIRED atspi-2)
include_directories(${ATSPI2_INCLUDE_DIRS})

set(CMAKE_INSTALL_NAME_DIR ${LIB_INSTALL_DIR})

if (WIN32)
   set(EXEC_INSTALL_PREFIX  "")
   set(LIB_INSTALL_DIR "lib${LIB_SUFFIX}")
   set(BIN_INSTALL_DIR "bin")
   set(INCLUDE_INSTALL_DIR  "include")
else (WIN32)
   if(APPLE)
      set(BUNDLE_INSTALL_DIR "/Applications/KDE4" CACHE PATH "Directory where application bundles will be installed to on OSX" )
   endif(APPLE)
   set(EXEC_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
   set(BIN_INSTALL_DIR "${EXEC_INSTALL_PREFIX}/bin")
   set(LIB_INSTALL_DIR "${EXEC_INSTALL_PREFIX}/lib${LIB_SUFFIX}")
   set(INCLUDE_INSTALL_DIR  "${CMAKE_INSTALL_PREFIX}/include")
endif (WIN32)

set(INSTALL_TARGETS_DEFAULT_ARGS  RUNTIME DESTINATION "${BIN_INSTALL_DIR}"
                                  LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
                                  ARCHIVE DESTINATION "${LIB_INSTALL_DIR}" COMPONENT Devel )
if(APPLE)
   set(INSTALL_TARGETS_DEFAULT_ARGS  ${INSTALL_TARGETS_DEFAULT_ARGS}
                               BUNDLE DESTINATION "${BUNDLE_INSTALL_DIR}" )
endif(APPLE)

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

if(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} VERSION_GREATER 2.6.2)
    set(_LibKdeAccessibilityClient_CONFIG_DEST "${LIB_INSTALL_DIR}/cmake/${PROJECT_NAME}")
else(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} VERSION_GREATER 2.6.2)
    set(_LibKdeAccessibilityClient_CONFIG_DEST "${LIB_INSTALL_DIR}/libkdeaccessibilityclient")
endif(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} VERSION_GREATER 2.6.2)

install( FILES
         "${LibKdeAccessibilityClient_BINARY_DIR}/LibKdeAccessibilityClientConfig.cmake"
         DESTINATION "${_LibKdeAccessibilityClient_CONFIG_DEST}" )
