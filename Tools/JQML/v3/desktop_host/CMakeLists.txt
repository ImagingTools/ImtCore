cmake_minimum_required(VERSION 3.16)
project(JqmlDesktopHost VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(IMTCOREDIR "$ENV{IMTCOREDIR}" CACHE PATH "Path to ImtCore")
set(ACFDIR "$ENV{ACFDIR}" CACHE PATH "Path to Acf")
set(TARGETNAME "$ENV{TARGETNAME}" CACHE STRING "Build target suffix, e.g. Qt6_VC17_x64")

if(IMTCOREDIR STREQUAL "")
    set(IMTCOREDIR "D:/IT/ImtCore" CACHE PATH "Path to ImtCore" FORCE)
endif()

if(ACFDIR STREQUAL "")
    get_filename_component(_itRoot "${IMTCOREDIR}" DIRECTORY)
    set(ACFDIR "${_itRoot}/Acf" CACHE PATH "Path to Acf" FORCE)
endif()

if(TARGETNAME STREQUAL "")
    set(TARGETNAME "Qt6_VC17_x64" CACHE STRING "Build target suffix" FORCE)
endif()

include_directories(
    "${IMTCOREDIR}/Include"
    "${ACFDIR}/Include"
)

link_directories(
    "${IMTCOREDIR}/Lib/Debug_${TARGETNAME}"
    "${IMTCOREDIR}/Lib/Release_${TARGETNAME}"
    "${ACFDIR}/Lib/Debug_${TARGETNAME}"
    "${ACFDIR}/Lib/Release_${TARGETNAME}"
)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick WebSockets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick WebSockets)

set(PROJECT_SOURCES
    main.cpp
    qml.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(JqmlDesktopHost
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(JqmlDesktopHost ${PROJECT_SOURCES})
endif()

target_link_libraries(JqmlDesktopHost
    PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::WebSockets
    PRIVATE imtqml imtbasesdl imtgui imtgql imtauth imtdb imtserverapp imtcom imtbase imtservergql
    PRIVATE iqt ipackage ifile icmm i3d iprm ilog ibase icomp iattr imath iser imod istd
    PRIVATE Advapi32 User32
)

set_target_properties(JqmlDesktopHost PROPERTIES
    WIN32_EXECUTABLE FALSE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(JqmlDesktopHost)
    qt_finalize_executable(JqmlDesktopHost)
endif()
