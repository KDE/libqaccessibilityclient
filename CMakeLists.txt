cmake_minimum_required(VERSION 3.16.0)

project(QAccessibilityClient VERSION "0.6.0")

include(FeatureSummary)
set(KF5_MIN_VERSION "5.91.0")
set(REQUIRED_QT_VERSION "5.15.2")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://api.kde.org/frameworks/extra-cmake-modules/html/index.html")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(ECMSetupVersion)
include(ECMGenerateExportHeader)
include(ECMDeprecationSettings)
include(ECMQtDeclareLoggingCategory)
include(CMakePackageConfigHelpers)

if(BUILD_TESTING)
    enable_testing()
    set(QT_OTHER_COMPONENTS Test)
endif()

if (QT_MAJOR_VERSION STREQUAL "6")
    set(REQUIRED_QT_VERSION "6.5.0")
endif()

find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} CONFIG REQUIRED COMPONENTS
    Widgets
    DBus
    ${QT_OTHER_COMPONENTS}
)

if (QT_MAJOR_VERSION STREQUAL "6")
    ecm_set_disabled_deprecation_versions(
        QT 6.6
    )
else()
    ecm_set_disabled_deprecation_versions(
        QT 5.15.2
    )
endif()

add_subdirectory(src)

if(BUILD_TESTING)
    add_subdirectory(tests)
    add_subdirectory(examples)
endif()

ecm_qt_install_logging_categories(
        EXPORT LIBQACCESSIBILITYCLIENT
        FILE libqaccessibilityclient.categories
        DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
        )

feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)
