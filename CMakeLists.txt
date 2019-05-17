cmake_minimum_required(VERSION 3.0.0)
project(QAccessibilityClient VERSION "0.4.1")

include(FeatureSummary)
find_package(ECM "5.18.0" REQUIRED NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://projects.kde.org/projects/kdesupport/extra-cmake-modules")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(ECMSetupVersion)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX QACCESSIBILITYCLIENT
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/QAccessibilityClientConfigVersion.cmake"
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/libqaccessibilityclient-version.h"
)

add_definitions(-DQT_USE_FAST_OPERATOR_PLUS)

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

set(_QAccessibilityClient_CONFIG_DEST "${KDE_INSTALL_CMAKEPACKAGEDIR}/${PROJECT_NAME}")

add_subdirectory(src)

if(BUILD_TESTING)
    add_subdirectory(tests)
    add_subdirectory(examples)
endif()

include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/QAccessibilityClientConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/QAccessibilityClientConfig.cmake"
    INSTALL_DESTINATION  ${_QAccessibilityClient_CONFIG_DEST}
)

install( FILES
         "${QAccessibilityClient_BINARY_DIR}/QAccessibilityClientConfig.cmake"
         "${QAccessibilityClient_BINARY_DIR}/QAccessibilityClientConfigVersion.cmake"
         DESTINATION "${_QAccessibilityClient_CONFIG_DEST}" )

feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)
