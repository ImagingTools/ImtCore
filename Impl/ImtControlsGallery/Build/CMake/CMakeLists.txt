cmake_minimum_required(VERSION 3.26)

project(ImtControlsGallery LANGUAGES CXX)

set(ARXC_CONFIG "${PROJECT_SOURCE_DIR}/../../../Config/ImtCore.awc")
set(ARXC_FILES "${PROJECT_SOURCE_DIR}/../${PROJECT_NAME}.acc")
set(ARXC_OUTDIR "${AUX_INCLUDE_DIR}")

if(NOT DEFINED WEB_COMPILE)
	set(WEB_COMPILE ON CACHE BOOL "Build whith web compatibility")
endif()

if(NOT DEFINED QT_VERSION_MAJOR)
	find_package(QT NAMES Qt6 Qt5 COMPONENTS Core)
	find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS Core Widgets QuickWidgets Core Gui Xml Network Svg Sql WebSockets Qml Concurrent QuickControls2 REQUIRED)
	if(QT_VERSION_MAJOR EQUAL 6)
		find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS Core5Compat REQUIRED)
	endif()
endif()

if(NOT DEFINED BUILDDIR)
	get_filename_component(BUILDDIR "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
else()
	get_filename_component(BUILDDIR "${BUILDDIR}" ABSOLUTE)
endif()

if(NOT DEFINED IMTCOREDIR)
	set(IMTCOREDIR "$ENV{IMTCOREDIR}")
endif()

if(NOT DEFINED IMTCOREDIR_BUILD)
	set(IMTCOREDIR_BUILD "$ENV{IMTCOREDIR}")
endif()

include(${IMTCOREDIR}/Config/CMake/ImtCoreEnv.cmake)
include(${IMTCOREDIR}/Config/CMake/PartituraUpgrade.cmake)
imt_core_upgrade_partitura("${CMAKE_CURRENT_LIST_DIR}/../..")

get_filename_component(INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../Include" ABSOLUTE)
get_filename_component(IMPL_DIR "${CMAKE_CURRENT_LIST_DIR}/../../Impl" ABSOLUTE)

set(AUX_INCLUDE_DIR "${BUILDDIR}/ImtControlsGallery/AuxInclude/${TARGETNAME}/GeneratedFiles")

include_directories("${AUX_INCLUDE_DIR}/../")
include_directories("${AUX_INCLUDE_DIR}/")

# Include
add_subdirectory("../../Include/controlsgallerygql/CMake" "${AUX_INCLUDE_DIR}/controlsgallerygql")
add_dependencies(controlsgallerygql controlsgallerysdl)

# Impl
add_subdirectory("../../Impl/ControlsGallery/CMake" "${AUX_INCLUDE_DIR}/ControlsGallery")
add_subdirectory("../../Impl/ControlsGalleryGqlPck/CMake" "${AUX_INCLUDE_DIR}/ControlsGalleryGqlPck")
add_dependencies(ControlsGalleryGqlPck controlsgallerygql)

# Qml
add_subdirectory("../../Qml/controlsgalleryqml/CMake" "${AUX_INCLUDE_DIR}/controlsgalleryqml")
add_dependencies(controlsgalleryqml controlsgallerysdl)

# Sdl
add_subdirectory("../../Sdl/controlsgallery/CMake" "${AUX_INCLUDE_DIR}/controlsgallerysdl")

