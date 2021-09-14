cmake_minimum_required(VERSION 3.14)

project(pilandok VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Network Concurrent Quick REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network Concurrent Quick REQUIRED)

set(PROJECT_SOURCES
        src/main.cpp
        src/network/kijangclient.h src/network/kijangclient.cpp
        src/network/kijangprotocol.h src/network/kijangprotocol.cpp
        src/output/pilandukaudiooutput.h src/output/pilandukaudiooutput.cpp
        src/output/pilandukvideooutput.h src/output/pilandukvideooutput.cpp
        src/input/kijangmotioninput.h src/input/kijangmotioninput.cpp
        qml.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(pilandok
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET pilandok APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(pilandok SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(pilandok
          ${PROJECT_SOURCES}
        )
    endif()
endif()

target_compile_definitions(pilandok
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(pilandok
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Concurrent)

set_target_properties(pilandok PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER moe.kancil.pilandok
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(pilandok)
    qt_finalize_executable(pilandok)
endif()

# Set version numbering
add_compile_definitions(VERSION_MAJOR=0)
add_compile_definitions(VERSION_MINOR=0)
add_compile_definitions(VERSION_MIC=1)
add_compile_definitions(VERSION_DEBUG=1)
