# ---------------------------------------------------------------------------
# Generate and export the "ImtCore" CMake package.
#
# This mirrors the ACF / AcfSln / IAcf package exports (Config/CMake/AcfPackageExport.cmake
# in the Acf module, Config/CMake/AcfSlnPackageExport.cmake in AcfSln and
# Config/CMake/IAcfPackageExport.cmake in IAcf). After this runs, downstream projects can
# discover ImtCore with a single call:
#
#     find_package(ImtCore REQUIRED)
#     target_link_libraries(myTarget PUBLIC ImtCore::imtbase ImtCore::imtgui ...)
#
# and inherit include directories and inter-library dependencies transitively.
# The generated config pulls in the underlying IAcf package (and, through it, the
# AcfSln and Acf packages) via find_dependency(IAcf).
#
# Two flavours of the package are produced:
#  * a build-tree package (no install step required), written next to the
#    compiled libraries so it matches the existing in-tree build layout, and
#  * an install-tree package for a relocatable `cmake --install` deployment.
#
# The ImtCore library targets are collected in the ${ACF_EXPORT_SET} export set,
# which the shared StaticConfig.cmake / acf_register_library() machinery from
# Acf registers each library into (ACF_EXPORT_SET defaults to
# "${ACF_PACKAGE_NAME}Targets", i.e. "ImtCoreTargets").
#
# Included once, centrally, from Build/CMake/CMakeLists.txt after all library
# targets and their dependencies have been declared.
# ---------------------------------------------------------------------------

include(CMakePackageConfigHelpers)

if(NOT DEFINED ACF_EXPORT_SET)
	set(ACF_EXPORT_SET "ImtCoreTargets")
endif()

# Package version. project(ImtCore) does not set one, so fall back to a default
# that still lets consumers request a version and use find_package version
# checks.
if(NOT DEFINED ImtCore_VERSION OR ImtCore_VERSION STREQUAL "")
	if(DEFINED PROJECT_VERSION AND NOT PROJECT_VERSION STREQUAL "")
		set(ImtCore_VERSION "${PROJECT_VERSION}")
	else()
		set(ImtCore_VERSION "1.0.0")
	endif()
endif()

# Build-tree location: alongside the produced libraries, mirroring the existing
# Lib/${CMAKE_BUILD_TYPE}_${TARGETNAME} layout.
set(IMTCORE_LIB_OUTPUT_DIR "${IMTCOREDIR_BUILD}/Lib/${CMAKE_BUILD_TYPE}_${TARGETNAME}")
set(IMTCORE_PACKAGE_BUILD_DIR "${IMTCORE_LIB_OUTPUT_DIR}/cmake")

# Ensure the output directory exists before writing export/config files into it,
# otherwise export()/configure_package_config_file() can fail on a fresh build tree.
file(MAKE_DIRECTORY "${IMTCORE_PACKAGE_BUILD_DIR}")

# --- Build-tree export ------------------------------------------------------
export(EXPORT ${ACF_EXPORT_SET}
	NAMESPACE ImtCore::
	FILE "${IMTCORE_PACKAGE_BUILD_DIR}/ImtCoreTargets.cmake")

# Qt major version this package was built against. Baked into the generated
# ImtCoreConfig.cmake so consumers resolve the matching Qt imported targets.
set(IMTCORE_QT_VERSION_MAJOR "${QT_VERSION_MAJOR}")

configure_package_config_file(
	"${IMTCOREDIR}/Config/CMake/ImtCoreConfig.cmake.in"
	"${IMTCORE_PACKAGE_BUILD_DIR}/ImtCoreConfig.cmake"
	INSTALL_DESTINATION "${IMTCORE_PACKAGE_BUILD_DIR}"
	NO_SET_AND_CHECK_MACRO)

write_basic_package_version_file(
	"${IMTCORE_PACKAGE_BUILD_DIR}/ImtCoreConfigVersion.cmake"
	VERSION "${ImtCore_VERSION}"
	COMPATIBILITY SameMajorVersion)

# Allow find_package(ImtCore) to locate the build-tree package directly (e.g. via
# CMAKE_PREFIX_PATH=<build>/Lib/<config>/cmake or ImtCore_DIR).
message(STATUS "ImtCore: build-tree package written to ${IMTCORE_PACKAGE_BUILD_DIR}")

# --- Install-tree export ----------------------------------------------------
install(EXPORT ${ACF_EXPORT_SET}
	NAMESPACE ImtCore::
	DESTINATION "lib/cmake/ImtCore"
	FILE "ImtCoreTargets.cmake")

install(FILES
	"${IMTCORE_PACKAGE_BUILD_DIR}/ImtCoreConfig.cmake"
	"${IMTCORE_PACKAGE_BUILD_DIR}/ImtCoreConfigVersion.cmake"
	DESTINATION "lib/cmake/ImtCore")
