cmake_minimum_required(VERSION 3.1.0)

project(ImtCore)

set (INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../Include")
set (IMPL_DIR "${CMAKE_CURRENT_LIST_DIR}/../../Impl")

subdirlist(INCLUDE_DIRS ${INCLUDE_DIR})
subdirlist(IMPL_DIRS ${IMPL_DIR})

get_target_name(TARGETNAME)

set(AUX_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../AuxInclude/Qt${QT_DEFAULT_MAJOR_VERSION}_${TARGETNAME}/GeneratedFiles")
set(IMT_BIN_DIR "${CMAKE_CURRENT_LIST_DIR}/../../Bin/${TARGETNAME}")

include_directories(${AUX_INCLUDE_DIR}/..)

#Include

add_subdirectory("${INCLUDE_DIR}/imtauth/CMake" "${AUX_INCLUDE_DIR}/imtauth")
add_subdirectory("${INCLUDE_DIR}/imtauthgui/CMake" "${AUX_INCLUDE_DIR}/imtauthgui")
add_subdirectory("${INCLUDE_DIR}/imtbase/CMake" "${AUX_INCLUDE_DIR}/imtbase")
add_subdirectory("${INCLUDE_DIR}/imtwidgets/CMake" "${AUX_INCLUDE_DIR}/imtwidgets")
add_subdirectory("${INCLUDE_DIR}/imtrest/CMake" "${AUX_INCLUDE_DIR}/imtrest")
add_subdirectory("${INCLUDE_DIR}/imtrepo/CMake" "${AUX_INCLUDE_DIR}/imtrepo")
add_subdirectory("${INCLUDE_DIR}/imtreport/CMake" "${AUX_INCLUDE_DIR}/imtreport")
add_subdirectory("${INCLUDE_DIR}/imtrepogui/CMake" "${AUX_INCLUDE_DIR}/imtrepogui")
add_subdirectory("${INCLUDE_DIR}/imtreportgui/CMake" "${AUX_INCLUDE_DIR}/imtreportgui")
add_subdirectory("${INCLUDE_DIR}/imt3d/CMake" "${AUX_INCLUDE_DIR}/imt3d")
add_subdirectory("${INCLUDE_DIR}/imtgui/CMake" "${AUX_INCLUDE_DIR}/imtgui")
add_subdirectory("${INCLUDE_DIR}/imtloggui/CMake" "${AUX_INCLUDE_DIR}/imtloggui")
add_subdirectory("${INCLUDE_DIR}/imtfile/CMake" "${AUX_INCLUDE_DIR}/imtfile")
add_subdirectory("${INCLUDE_DIR}/imtlog/CMake" "${AUX_INCLUDE_DIR}/imtlog")
add_subdirectory("${INCLUDE_DIR}/imt3dview/CMake" "${AUX_INCLUDE_DIR}/imt3dview")
add_subdirectory("${INCLUDE_DIR}/imt3dgui/CMake" "${AUX_INCLUDE_DIR}/imt3dgui")
#add_subdirectory("${INCLUDE_DIR}/imtzip/CMake" "${AUX_INCLUDE_DIR}/imtzip")
add_subdirectory("${INCLUDE_DIR}/imtpy/CMake" "${AUX_INCLUDE_DIR}/imtpy")
add_subdirectory("${INCLUDE_DIR}/imtqml/CMake" "${AUX_INCLUDE_DIR}/imtqml")
add_subdirectory("${INCLUDE_DIR}/imtlic/CMake" "${AUX_INCLUDE_DIR}/imtlic")
add_subdirectory("${INCLUDE_DIR}/imtlicgui/CMake" "${AUX_INCLUDE_DIR}/imtlicgui")

#Impl

add_subdirectory("${IMPL_DIR}/ImtCorePck/CMake" "${AUX_INCLUDE_DIR}/ImtCorePck")
add_subdirectory("${IMPL_DIR}/ImtAuthPck/CMake" "${AUX_INCLUDE_DIR}/ImtAuthPck")
add_subdirectory("${IMPL_DIR}/ImtAuthGuiPck/CMake" "${AUX_INCLUDE_DIR}/ImtAuthGuiPck")
add_subdirectory("${IMPL_DIR}/ImtLicensePck/CMake" "${AUX_INCLUDE_DIR}/ImtLicensePck")
add_subdirectory("${IMPL_DIR}/ImtLicenseGuiPck/CMake" "${AUX_INCLUDE_DIR}/ImtLicenseGuiPck")
add_subdirectory("${IMPL_DIR}/ImtRestPck/CMake" "${AUX_INCLUDE_DIR}/ImtRestPck")
add_subdirectory("${IMPL_DIR}/Imt3dPck/CMake" "${AUX_INCLUDE_DIR}/Imt3dPck")
add_subdirectory("${IMPL_DIR}/ImtRepoGuiPck/CMake" "${AUX_INCLUDE_DIR}/ImtRepoGuiPck")
add_subdirectory("${IMPL_DIR}/ImtReportGuiPck/CMake" "${AUX_INCLUDE_DIR}/ImtReportGuiPck")
add_subdirectory("${IMPL_DIR}/ImtRepoPck/CMake" "${AUX_INCLUDE_DIR}/ImtRepoPck")
add_subdirectory("${IMPL_DIR}/ImtReportPck/CMake" "${AUX_INCLUDE_DIR}/ImtReportPck")
add_subdirectory("${IMPL_DIR}/ImtCoreLoc/CMake" "${AUX_INCLUDE_DIR}/ImtCoreLoc")
add_subdirectory("${IMPL_DIR}/ImtGuiPck/CMake" "${AUX_INCLUDE_DIR}/ImtGuiPck")
add_subdirectory("${IMPL_DIR}/ImtQmlPck/CMake" "${AUX_INCLUDE_DIR}/ImtQmlPck")
add_subdirectory("${IMPL_DIR}/ImtLogPck/CMake" "${AUX_INCLUDE_DIR}/ImtLogPck")
add_subdirectory("${IMPL_DIR}/ImtLogGuiPck/CMake" "${AUX_INCLUDE_DIR}/ImtLogGuiPck")
add_subdirectory("${IMPL_DIR}/Imt3dGuiPck/CMake" "${AUX_INCLUDE_DIR}/Imt3dGuiPck")
add_subdirectory("${IMPL_DIR}/Imt3dViewPck/CMake" "${AUX_INCLUDE_DIR}/Imt3dViewPck")
#add_subdirectory("${IMPL_DIR}/ImtZipPck/CMake" "${AUX_INCLUDE_DIR}/ImtZipPck")
add_subdirectory("${IMPL_DIR}/ImtPyPck/CMake" "${AUX_INCLUDE_DIR}/ImtPyPck")

# Application
IF(!UNIX)
#	add_subdirectory("${IMPL_DIR}/EagleView/CMake" "${AUX_INCLUDE_DIR}/EagleView")
ENDIF()