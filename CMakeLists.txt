cmake_minimum_required(VERSION 3.0.0)

include(FeatureSummary)
find_package(ECM "5.18.0" REQUIRED NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://projects.kde.org/projects/kdesupport/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(ECMSetupVersion)

cmake_policy(SET CMP0048 NEW)
project(QAccessibilityClient VERSION "0.3.0")

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX QACCESSIBILITYCLIENT
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/libqaccessibilityclient-version.h"
)

add_definitions(-DQT_USE_FAST_OPERATOR_PLUS)

set(CMAKE_AUTOMOC TRUE)

option(BUILD_TESTING "Build the testing tree." ON)
if(BUILD_TESTING)
    enable_testing()
    set(QT_OTHER_COMPONENTS Test)
endif()

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}
)

set(QT_MIN_VERSION "5.6.0")
find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Widgets
    DBus
    ${QT_OTHER_COMPONENTS}
)

set(QACCESSIBILITYCLIENT_LIB_NAME "qaccessibilityclient-qt5")

set(CMAKE_INSTALL_NAME_DIR ${LIB_INSTALL_DIR})

if (WIN32)
   set(EXEC_INSTALL_PREFIX  "")
   set(LIB_INSTALL_DIR "lib${LIB_SUFFIX}" CACHE STRING "Directory where lib will install")
   set(BIN_INSTALL_DIR "bin")
   set(INCLUDE_INSTALL_DIR  "include")
else (WIN32)
   if(APPLE)
      set(BUNDLE_INSTALL_DIR "/Applications/KDE4" CACHE PATH "Directory where application bundles will be installed to on OSX" )
   endif(APPLE)
   set(EXEC_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
   set(BIN_INSTALL_DIR "${EXEC_INSTALL_PREFIX}/bin")
   set(LIB_INSTALL_DIR "${EXEC_INSTALL_PREFIX}/lib${LIB_SUFFIX}" CACHE STRING
       "Directory where lib will install")
   set(INCLUDE_INSTALL_DIR  "${CMAKE_INSTALL_PREFIX}/include")
endif (WIN32)

set(_QAccessibilityClient_CONFIG_DEST "${LIB_INSTALL_DIR}/cmake/${PROJECT_NAME}")

set(INSTALL_TARGETS_DEFAULT_ARGS  RUNTIME DESTINATION "${BIN_INSTALL_DIR}"
                                  LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
                                  ARCHIVE DESTINATION "${LIB_INSTALL_DIR}" COMPONENT Devel )
if(APPLE)
   set(INSTALL_TARGETS_DEFAULT_ARGS  ${INSTALL_TARGETS_DEFAULT_ARGS}
                               BUNDLE DESTINATION "${BUNDLE_INSTALL_DIR}" )
endif(APPLE)

add_subdirectory(src)

if(BUILD_TESTING)
    add_subdirectory(tests)
    add_subdirectory(examples)
endif()

# we need the absolute directories where stuff will be installed too
# but since the variables which contain the destinations can be relative
# or absolute paths, we need this macro to make them all absoulte, Alex
macro(MAKE_INSTALL_PATH_ABSOLUTE out in)
   if (IS_ABSOLUTE "${in}")    # IS_ABSOLUTE is new since cmake 2.4.8
      set(${out} "${in}")
   else()
      set(${out} "\${CMAKE_INSTALL_PREFIX}/${in}")
   endif()
endmacro(MAKE_INSTALL_PATH_ABSOLUTE out in)

make_install_path_absolute(QACCESSIBILITYCLIENT_INCLUDE_DIR ${INCLUDE_INSTALL_DIR})
make_install_path_absolute(QACCESSIBILITYCLIENT_LIB_DIR     ${LIB_INSTALL_DIR})

include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/QAccessibilityClientConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/QAccessibilityClientConfig.cmake"
    INSTALL_DESTINATION  ${_QAccessibilityClient_CONFIG_DEST}
)

install( FILES
         "${QAccessibilityClient_BINARY_DIR}/QAccessibilityClientConfig.cmake"
         DESTINATION "${_QAccessibilityClient_CONFIG_DEST}" )
