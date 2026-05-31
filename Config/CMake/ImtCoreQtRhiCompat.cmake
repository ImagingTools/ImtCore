# ImtCoreQtRhiCompat.cmake
#
# Adds the versioned QtGui headers directory to the include path so that
# <rhi/qrhi.h> can be found.  Required for Qt >= 6.8.0 where RHI headers
# live in a version-specific subdirectory.
#
# Cross-platform: Windows, Linux, macOS (including framework builds).
# Does NOT link or reference any Qt private libraries.

include_guard(GLOBAL)

if(NOT TARGET Qt6::Gui)
	find_package(Qt6 COMPONENTS Gui QUIET)
endif()
if(NOT TARGET Qt6::Gui OR NOT DEFINED Qt6_VERSION OR Qt6_VERSION VERSION_LESS "6.8.0")
	return()
endif()

# Collect hint directories for find_path().
set(_qrhi_hints "")

# From INTERFACE_INCLUDE_DIRECTORIES of the public Qt6::Gui target.
get_target_property(_gui_incs Qt6::Gui INTERFACE_INCLUDE_DIRECTORIES)
if(_gui_incs)
	foreach(_d IN LISTS _gui_incs)
		if(NOT _d MATCHES "^\\$<")
			list(APPEND _qrhi_hints "${_d}")
		endif()
	endforeach()
endif()

# From the imported library location (adds its parent and Headers/ subdir so
# that both regular and framework install layouts are covered).
get_target_property(_cfgs Qt6::Gui IMPORTED_CONFIGURATIONS)
if(_cfgs)
	list(GET _cfgs 0 _cfg)
	get_target_property(_loc Qt6::Gui "IMPORTED_LOCATION_${_cfg}")
	if(_loc)
		get_filename_component(_lib_dir "${_loc}" DIRECTORY)
		list(APPEND _qrhi_hints "${_lib_dir}" "${_lib_dir}/Headers")
	endif()
endif()

find_path(_qrhi_versioned_dir
	NAMES rhi/qrhi.h
	HINTS ${_qrhi_hints}
	PATH_SUFFIXES "${Qt6_VERSION}/QtGui"
	NO_DEFAULT_PATH
)

if(_qrhi_versioned_dir)
	include_directories("${_qrhi_versioned_dir}")
	message(STATUS "[ImtCore] Added versioned QtGui RHI headers: ${_qrhi_versioned_dir}")
else()
	message(FATAL_ERROR "[ImtCore] Could not find versioned QtGui RHI headers")
endif()

# Clean up local variables to avoid leaking into the caller's scope.
unset(_qrhi_hints)
unset(_gui_incs)
unset(_d)
unset(_cfgs)
unset(_cfg)
unset(_loc)
unset(_lib_dir)
