cmake_minimum_required(VERSION 3.1.0)

project(EjDoc LANGUAGES CXX)



set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /wd4127 /wd4250 /wd4347 /wd4355 /wd4365 /wd4481 /wd4505 /wd4510 /wd4511 /wd4512 /wd4548 /wd4571 /wd4619 /wd4625 /wd4626 /wd4640 /wd4702 /wd4710 /wd4820 /wd4826")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /MP /fp:fast")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /bigobj")

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /Qpar /Gy /Gw /FS /Zc:threadSafeInit-")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "${CMAKE_CXX_FLAGS_RELEASE_INIT} /Ot /Oi /Ob2 /GS-")

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
set(outlibdir "${PROJECT_SOURCE_DIR}/../libs")

set_target_properties( ${PROJECT_NAME}
	PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY ${outlibdir}
	LIBRARY_OUTPUT_DIRECTORY ${outlibdir}
	RUNTIME_OUTPUT_DIRECTORY ${outlibdir}
	POSITION_INDEPENDENT_CODE ON
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(${PROJECT_NAME})
    qt_finalize_executable(${PROJECT_NAME})
endif()
