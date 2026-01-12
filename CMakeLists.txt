cmake_minimum_required(VERSION 3.16.0)

project(QAccessibilityClient VERSION "0.6.0")

include(FeatureSummary)
set(KF_MIN_VERSION "6.0.0")
set(REQUIRED_QT_VERSION "6.5.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
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
include(ECMGenerateQDoc)

if(BUILD_TESTING)
    enable_testing()
    set(QT_OTHER_COMPONENTS Test)
endif()

find_package(Qt6 ${REQUIRED_QT_VERSION} CONFIG REQUIRED COMPONENTS
    Widgets
    DBus
    ${QT_OTHER_COMPONENTS}
)

ecm_set_disabled_deprecation_versions(
    QT 6.6
)

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
