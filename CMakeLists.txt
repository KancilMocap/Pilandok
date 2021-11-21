cmake_minimum_required(VERSION 3.14)

project(pilandok VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Network Concurrent Quick Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network Concurrent Quick Widgets REQUIRED)

set(PROJECT_SOURCES
        src/main.cpp
        src/application/pilandok.h src/application/pilandok.cpp
        src/application/pilandoklogger.h src/application/pilandoklogger.cpp
        src/network/kijangclient.h src/network/kijangclient.cpp
        src/network/kijangclientmodule.h src/network/kijangclientmodule.cpp
        src/network/kijangservercontrolmodule.h src/network/kijangservercontrolmodule.cpp
        src/network/kijangmainmodule.h src/network/kijangmainmodule.cpp
        src/network/kijangprotocol.h src/network/kijangprotocol.cpp
        src/output/pilandokoutputmanager.h src/output/pilandokoutputmanager.cpp
        src/output/pilandokaudiooutput.h src/output/pilandokaudiooutput.cpp
        src/output/pilandokvideooutput.h src/output/pilandokvideooutput.cpp
        src/input/kijangmotioninput.h src/input/kijangmotioninput.cpp
        qml.qrc
)

qt_add_executable(pilandok
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

add_library(cryptopp STATIC IMPORTED)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(cryptopp PROPERTIES
        IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/cryptlib-debug.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/inc/cryptopp")
else()
    set_target_properties(cryptopp PROPERTIES
        IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/cryptlib-release.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/inc/cryptopp")
endif()

target_compile_definitions(pilandok
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(pilandok
  PRIVATE cryptopp Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Concurrent Qt${QT_VERSION_MAJOR}::Widgets)

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
