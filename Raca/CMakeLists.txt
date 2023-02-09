cmake_minimum_required(VERSION 3.5)

project(Raca VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Concurrent)


file (GLOB_RECURSE Raca_SOURCES CONFIGURE_DEPENDS "*.cpp")
file (GLOB_RECURSE Raca_HEADERS CONFIGURE_DEPENDS "*.h")
file (GLOB_RECURSE Raca_UI CONFIGURE_DEPENDS "*.ui")
file (GLOB_RECURSE Raca_QRC CONFIGURE_DEPENDS "*.qrc")

set(PROJECT_SOURCES
    ${Raca_SOURCES}
    ${Raca_HEADERS}
    ${Raca_UI}
    ${Raca_QRC}
)
if (WIN32)
    list (APPEND PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/resource/Raca.rc")
endif()


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Raca
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Raca APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Raca SHARED
            ${PROJECT_SOURCES}
        )
    # Define properties for Android with Qt 5 after find_package() calls as:
    #    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
else()
    add_executable(Raca
        ${PROJECT_SOURCES}
    )
endif()
endif()

target_link_libraries(Raca PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Concurrent
)

set_target_properties(Raca PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS Raca
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Raca)
endif()


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../QHotkey ${CMAKE_CURRENT_BINARY_DIR}/QHotkey)
include_directories(../QHotkey)
target_link_libraries(Raca PRIVATE QHotkey)

if (WIN32)
    target_link_libraries(Raca PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/bin/oleacc.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/bin/comsuppw.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/bin/Oleaut32.lib
    )
endif()
