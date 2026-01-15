cmake_minimum_required(VERSION 3.1.0)

project(EjDoc LANGUAGES CXX)


if(${MSVC})
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /wd4127 /wd4250 /wd4347 /wd4355 /wd4365 /wd4481 /wd4505 /wd4510 /wd4511 /wd4512 /wd4548 /wd4571 /wd4619 /wd4625 /wd4626 /wd4640 /wd4702 /wd4710 /wd4820 /wd4826")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /MP /fp:fast")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /bigobj")

    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /Qpar /Gy /Gw /FS /Zc:threadSafeInit-")
    set(CMAKE_CXX_FLAGS_RELEASE_INIT "${CMAKE_CXX_FLAGS_RELEASE_INIT} /Ot /Oi /Ob2 /GS-")
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui)

add_compile_definitions(COMMON_LIBRARY)

set(PROJECT_SOURCES
		ejcalculator.cpp
		ejcalculator.h
		ejcommon.cpp
		../include/ejcommon.h
		ejtextcontrol.cpp
		../include/ejtextcontrol.h
		ejstoragehelper.cpp
		../include/ejstoragehelper.h
		ejstyles.cpp
		../include/ejstyles.h
		ejtableblocks.cpp
		../include/ejtableblocks.h
		ejdocument.cpp
		../include/ejdocument.h
		docprops.cpp
		../include/docprops.h
)

include_directories(../include/)

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCES})

add_compile_definitions(COMMON_LIBRARY)

target_compile_definitions(${PROJECT_NAME}
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(${PROJECT_NAME}
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
	MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)



set(COMPILER_NAME "Clang")
if(${MSVC})
	if(${MSVC_TOOLSET_VERSION} STREQUAL 142)
		set(COMPILER_NAME "VC16")
	elseif(${MSVC_TOOLSET_VERSION} STREQUAL 143)
		set(COMPILER_NAME "VC17")
	endif()
elseif(${APPLE})
	if("${CMAKE_OSX_ARCHITECTURES}" STREQUAL "arm64")
		set(COMPILER_CODE "${COMPILER_NAME}OSX_arm64")
		add_compile_definitions(COMPILER_NAME=ClangOSX)
		add_compile_definitions(PLATFORM_CODE=arm64)
	else()
		set(COMPILER_CODE "${COMPILER_NAME}OSX_x64")
		add_compile_definitions(COMPILER_NAME=ClangOSX)
		add_compile_definitions(PLATFORM_CODE=x64)
	endif()
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
	if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		set(COMPILER_CODE "GCCLinux_x64")
		add_compile_definitions(COMPILER_NAME=GCCLinux)
	else()
		set(COMPILER_CODE "ClangLinux_x64")
		add_compile_definitions(COMPILER_NAME=ClangLinux)
	endif()
	add_compile_definitions(PLATFORM_CODE=x64)
endif()


set(outlibdir "${PROJECT_SOURCE_DIR}/../libs/${CMAKE_BUILD_TYPE}_Qt${QT_VERSION_MAJOR}_${COMPILER_NAME}_x64")

set_target_properties( ${PROJECT_NAME}
	PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY ${outlibdir}
	LIBRARY_OUTPUT_DIRECTORY ${outlibdir}
	RUNTIME_OUTPUT_DIRECTORY ${outlibdir}
	POSITION_INDEPENDENT_CODE ON
)

if(QT_VERSION_MAJOR EQUAL 6)
#    qt_import_qml_plugins(${PROJECT_NAME})
#    qt_finalize_executable(${PROJECT_NAME})
endif()
