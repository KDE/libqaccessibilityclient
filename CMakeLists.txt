project(QAccessibilityClient)

set(QACCESSIBILITYCLIENT_VERSION_MAJOR 0)
set(QACCESSIBILITYCLIENT_VERSION_MINOR 1)
set(QACCESSIBILITYCLIENT_VERSION_PATCH 0)
set(QACCESSIBILITYCLIENT_VERSION ${QACCESSIBILITYCLIENT_VERSION_MAJOR}.${QACCESSIBILITYCLIENT_VERSION_MINOR}.${QACCESSIBILITYCLIENT_VERSION_PATCH})

# minimum required cmake is 2.8.6 for automoc-support
cmake_minimum_required(VERSION 2.8.6)

add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

set(CMAKE_AUTOMOC TRUE)

# override the KDE defaults and build tests unless explicitly disabled
option(KDE4_BUILD_TESTS  "Build the tests" ON)
option(QT4_BUILD "Force building with Qt4 even if Qt5 is found")

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}
)

if (NOT QT4_BUILD)
find_package(Qt5Widgets QUIET)
find_package(Qt5DBus QUIET)
endif()


if (${Qt5Widgets_FOUND} AND ${Qt5DBus_FOUND})
    set(QT5_BUILD True)
endif (${Qt5Widgets_FOUND} AND ${Qt5DBus_FOUND})

if (QT5_BUILD)
    message("Using Qt 5")
    add_definitions(${Qt5Core_DEFINITIONS} ${Qt5Widgets_DEFINITIONS} ${Qt5DBus_DEFINITIONS})
    include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5DBus_INCLUDE_DIRS})
else (QT5_BUILD)
    find_package(Qt4 REQUIRED)
    if (${QT4_FOUND})
        message("Using Qt 4")
        include(${QT_USE_FILE})
        add_definitions(${QT_DEFINITIONS})
        include_directories(${QT4_INCLUDES} ${QT_INCLUDE_DIR} ${QT_QT_INCLUDE_DIR} ${QT_QTDBUS_INCLUDE_DIR})
    endif (${QT4_FOUND})
endif (QT5_BUILD)


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

set(_QAccessibilityClient_CONFIG_DEST "${LIB_INSTALL_DIR}/cmake/${PROJECT_NAME}")

set(INSTALL_TARGETS_DEFAULT_ARGS  RUNTIME DESTINATION "${BIN_INSTALL_DIR}"
                                  LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
                                  ARCHIVE DESTINATION "${LIB_INSTALL_DIR}" COMPONENT Devel )
if(APPLE)
   set(INSTALL_TARGETS_DEFAULT_ARGS  ${INSTALL_TARGETS_DEFAULT_ARGS}
                               BUNDLE DESTINATION "${BUNDLE_INSTALL_DIR}" )
endif(APPLE)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(examples)

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

configure_file( "${QAccessibilityClient_SOURCE_DIR}/QAccessibilityClientConfig.cmake.in" "${QAccessibilityClient_BINARY_DIR}/QAccessibilityClientConfig.cmake" @ONLY )
install( FILES
         "${QAccessibilityClient_BINARY_DIR}/QAccessibilityClientConfig.cmake"
         DESTINATION "${_QAccessibilityClient_CONFIG_DEST}" )
