# ImtCoreQtRhiCompat.cmake
#
# Ensures the versioned QtGui headers directory (containing rhi/qrhi.h) is on the
# include path.  On macOS framework builds the versioned headers live inside
# QtGui.framework/Versions/A/Headers/<version>/QtGui and are NOT exposed by the
# public Qt6::Gui target.  This module resolves that directory and adds it
# globally so that any translation unit including <imt3dgui/QtRhiCompat.h> can
# find <rhi/qrhi.h>.

if(_IMTCORE_QT_RHI_COMPAT_INCLUDED)
    return()
endif()
set(_IMTCORE_QT_RHI_COMPAT_INCLUDED TRUE)

# Need at least Qt 6 Gui to be available.
if(NOT TARGET Qt6::Gui)
    find_package(Qt6 COMPONENTS Gui QUIET)
endif()

if(NOT TARGET Qt6::Gui)
    return()
endif()

# Determine the full Qt version string.
if(NOT DEFINED Qt6Core_VERSION_MAJOR)
    find_package(Qt6 COMPONENTS Core QUIET)
endif()

if(DEFINED Qt6Core_VERSION_MAJOR)
    set(_qrhi_qt_ver "${Qt6Core_VERSION_MAJOR}.${Qt6Core_VERSION_MINOR}.${Qt6Core_VERSION_PATCH}")
else()
    return()
endif()

if(_qrhi_qt_ver VERSION_LESS "6.8.0")
    return()
endif()

# --- Resolve the versioned QtGui headers directory ---
set(_qrhi_headers_dir "")

# Strategy 1: Framework layout (macOS) — derive from IMPORTED_LOCATION
foreach(_cfg RELEASE RELWITHDEBINFO MINSIZEREL DEBUG NOCONFIG "")
    if(_cfg STREQUAL "")
        get_target_property(_qrhi_loc Qt6::Gui IMPORTED_LOCATION)
    else()
        get_target_property(_qrhi_loc Qt6::Gui IMPORTED_LOCATION_${_cfg})
    endif()
    if(_qrhi_loc AND NOT _qrhi_loc MATCHES "-NOTFOUND$")
        break()
    endif()
endforeach()

if(_qrhi_loc AND _qrhi_loc MATCHES "QtGui\\.framework")
    get_filename_component(_qrhi_fw_dir "${_qrhi_loc}" DIRECTORY)
    set(_qrhi_fw_versioned "${_qrhi_fw_dir}/Headers/${_qrhi_qt_ver}/QtGui")
    if(EXISTS "${_qrhi_fw_versioned}/rhi/qrhi.h")
        set(_qrhi_headers_dir "${_qrhi_fw_versioned}")
    endif()
endif()

# Strategy 2: Non-framework layout — look for versioned subdir under QtGui include
if(NOT _qrhi_headers_dir)
    get_target_property(_qrhi_gui_incs Qt6::Gui INTERFACE_INCLUDE_DIRECTORIES)
    foreach(_dir IN LISTS _qrhi_gui_incs)
        # Skip generator expressions
        if(_dir MATCHES "^\\$<")
            continue()
        endif()
        file(TO_CMAKE_PATH "${_dir}" _dir_norm)
        if(_dir_norm MATCHES "/QtGui$")
            set(_versioned "${_dir_norm}/${_qrhi_qt_ver}/QtGui")
            if(EXISTS "${_versioned}/rhi/qrhi.h")
                set(_qrhi_headers_dir "${_versioned}")
            elseif(EXISTS "${_dir_norm}/rhi/qrhi.h")
                set(_qrhi_headers_dir "${_dir_norm}")
            endif()
            if(_qrhi_headers_dir)
                break()
            endif()
        endif()
    endforeach()
endif()

# Strategy 3: Derive from Qt6Gui_DIR (cmake config directory)
if(NOT _qrhi_headers_dir AND DEFINED Qt6Gui_DIR)
    get_filename_component(_qrhi_qt_lib "${Qt6Gui_DIR}/../../.." ABSOLUTE)
    # Framework layout
    set(_qrhi_fw_candidate "${_qrhi_qt_lib}/QtGui.framework/Versions/A/Headers/${_qrhi_qt_ver}/QtGui")
    if(EXISTS "${_qrhi_fw_candidate}/rhi/qrhi.h")
        set(_qrhi_headers_dir "${_qrhi_fw_candidate}")
    else()
        # Non-framework layout
        get_filename_component(_qrhi_qt_prefix "${_qrhi_qt_lib}/.." ABSOLUTE)
        set(_qrhi_inc_candidate "${_qrhi_qt_prefix}/include/QtGui/${_qrhi_qt_ver}/QtGui")
        if(EXISTS "${_qrhi_inc_candidate}/rhi/qrhi.h")
            set(_qrhi_headers_dir "${_qrhi_inc_candidate}")
        endif()
    endif()
endif()

if(_qrhi_headers_dir)
    include_directories("${_qrhi_headers_dir}")
    message(STATUS "[ImtCore] Added versioned QtGui RHI headers: ${_qrhi_headers_dir}")
endif()
