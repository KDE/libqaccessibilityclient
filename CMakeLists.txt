cmake_minimum_required(VERSION 3.16.0)

project(QAccessibilityClient VERSION "0.4.1")

include(FeatureSummary)
set(KF5_MIN_VERSION "5.89.0")
set(REQUIRED_QT_VERSION "5.15.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://commits.kde.org/extra-cmake-modules")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(ECMSetupVersion)
include(GenerateExportHeader)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX QACCESSIBILITYCLIENT
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/QAccessibilityClientConfigVersion.cmake"
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/libqaccessibilityclient-version.h"
)
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050f02)
add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x055900)
add_definitions(-DQT_USE_FAST_OPERATOR_PLUS)

if(BUILD_TESTING)
    enable_testing()
    set(QT_OTHER_COMPONENTS Test)
endif()

find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED COMPONENTS
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
