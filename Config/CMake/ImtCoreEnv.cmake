cmake_minimum_required(VERSION 3.26)


if(NOT DEFINED IACFDIR)
	set(IACFDIR "$ENV{IACFDIR}")
endif()

include(${IACFDIR}/Config/CMake/IAcfEnv.cmake)

if(NOT DEFINED IMTCOREDIR)
	set(IMTCOREDIR "$ENV{IMTCOREDIR}")
endif()

if(NOT DEFINED IMTCOREDIR_BUILD)
	set(IMTCOREDIR_BUILD "$ENV{IMTCOREDIR_BUILD}")
	if(IMTCOREDIR_BUILD STREQUAL "")
		set(IMTCOREDIR_BUILD ${IMTCOREDIR})
	endif()
endif()

if(NOT DEFINED OPENSSLDIR)
	set(OPENSSLDIR "$ENV{OPENSSLDIR}")
	if(OPENSSLDIR STREQUAL "")
		set(OPENSSLDIR "${IMTCOREDIR}/3rdParty/openssl/1.1")
	endif()
endif()

if(NOT DEFINED QUAZIP)
	set(QUAZIP "${IMTCOREDIR}/3rdParty/quazip")
endif()
if(WIN32)
	if(NOT DEFINED PYTHON38DIR)
		set(PYTHON38DIR "$ENV{PYTHON38DIR}")
		if(PYTHON38DIR STREQUAL "")
			message(WARNING "PYTHON38DIR environment variable is not set. Please set it to your Python 3.8 installation directory.")
		endif()
	endif()
endif()

if(NOT DEFINED ACFPUBLICDIR)
	set(ACFPUBLICDIR "$ENV{ACFPUBLICDIR}")
endif()

if(NOT DEFINED ZLIBDIR)
	set(ZLIBDIR "$ENV{ZLIBDIR}")
	if(ZLIBDIR STREQUAL "")
		set(ZLIBDIR "${ACFPUBLICDIR}/3rdParty/zlib")
	endif()
endif()

function(imt_chain_windeploy_target target)
	if(TARGET ${target})
		if(DEFINED IMT_PREV_WINDEPLOY_TARGET AND TARGET ${IMT_PREV_WINDEPLOY_TARGET})
			add_dependencies(${target} ${IMT_PREV_WINDEPLOY_TARGET})
		endif()
		set(IMT_PREV_WINDEPLOY_TARGET ${target} PARENT_SCOPE)
	endif()
endfunction()

include_directories("${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles")
include_directories("${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}")

# The Sdl source tree carries hand-written headers (e.g. imtpay/) that are not part of the
# per-target INCLUDE_DIR/IMPL_DIR exposed by acf_register_library, so it stays unconditional.
include_directories("${IMTCOREDIR}/Sdl")

if(NOT ACF_MODERN_CMAKE)
	# Legacy mode: global include/link dirs for repos that haven't migrated to
	# find_package(ImtCore) + target-based deps yet. Skipped when ACF_MODERN_CMAKE is ON.
	include_directories("${IMTCOREDIR}/Include")
	include_directories("${IMTCOREDIR}/Impl")

	link_directories("${IMTCOREDIR_BUILD}/Lib/${CMAKE_BUILD_TYPE}_${TARGETNAME}")
else()
	# Discover the IAcf package published by its build tree. IAcf transitively pulls in the
	# AcfSln and Acf packages (find_dependency(Acf)/find_dependency(AcfSln) in IAcfConfig.cmake),
	# so the Acf::/AcfSln::/IAcf:: imported targets referenced by the ImtCore dependency graph resolve.
	# Acf_DIR/AcfSln_DIR are set as well so those transitive find_dependency() calls locate the sibling build trees.
	if(NOT DEFINED IACFDIR_BUILD)
		set(IACFDIR_BUILD "${IACFDIR}")
	endif()

	set(IAcf_DIR "${IACFDIR_BUILD}/Lib/${CMAKE_BUILD_TYPE}_${TARGETNAME}/cmake")
	message(VERBOSE "ImtCoreEnv find_package(IAcf) from ${IAcf_DIR}")

	find_package(IAcf QUIET GLOBAL)

	if(NOT IAcf_FOUND)
		message(STATUS "IAcf package not found. Using AcfSln package instead.")
	endif()
endif()
