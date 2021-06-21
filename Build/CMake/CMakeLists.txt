cmake_minimum_required(VERSION 3.1.0)

project(ImtCore)

if(NOT DEFINED IMTCOREDIR)
	set(IMTCOREDIR "$ENV{IMTCOREDIR}")
endif()

include(${IMTCOREDIR}/Config/CMake/ImtCoreEnv.cmake)

set (INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../Include")
set (IMPL_DIR "${CMAKE_CURRENT_LIST_DIR}/../../Impl")
set(AUX_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../AuxInclude/Qt${QT_DEFAULT_MAJOR_VERSION}_${TARGETNAME}/GeneratedFiles")

include_directories(${AUX_INCLUDE_DIR}/..)

#Include

add_subdirectory("${INCLUDE_DIR}/imtauth/CMake" "${AUX_INCLUDE_DIR}/imtauth")
add_subdirectory("${INCLUDE_DIR}/imtauthgui/CMake" "${AUX_INCLUDE_DIR}/imtauthgui")
add_subdirectory("${INCLUDE_DIR}/imtbase/CMake" "${AUX_INCLUDE_DIR}/imtbase")
add_subdirectory("${INCLUDE_DIR}/imtcrypt/CMake" "${AUX_INCLUDE_DIR}/imtcrypt")
add_subdirectory("${INCLUDE_DIR}/imtwidgets/CMake" "${AUX_INCLUDE_DIR}/imtwidgets")
add_subdirectory("${INCLUDE_DIR}/imtstyle/CMake" "${AUX_INCLUDE_DIR}/imtstyle")
add_subdirectory("${INCLUDE_DIR}/imtrest/CMake" "${AUX_INCLUDE_DIR}/imtrest")
add_subdirectory("${INCLUDE_DIR}/imtrepo/CMake" "${AUX_INCLUDE_DIR}/imtrepo")
add_subdirectory("${INCLUDE_DIR}/imtreport/CMake" "${AUX_INCLUDE_DIR}/imtreport")
add_subdirectory("${INCLUDE_DIR}/imtreportgui/CMake" "${AUX_INCLUDE_DIR}/imtreportgui")
add_subdirectory("${INCLUDE_DIR}/imt3d/CMake" "${AUX_INCLUDE_DIR}/imt3d")
add_subdirectory("${INCLUDE_DIR}/imtgui/CMake" "${AUX_INCLUDE_DIR}/imtgui")
add_dependencies(imtgui imtwidgets)
add_subdirectory("${INCLUDE_DIR}/imtloggui/CMake" "${AUX_INCLUDE_DIR}/imtloggui")
add_subdirectory("${INCLUDE_DIR}/imtfile/CMake" "${AUX_INCLUDE_DIR}/imtfile")
add_subdirectory("${INCLUDE_DIR}/imtlog/CMake" "${AUX_INCLUDE_DIR}/imtlog")
add_subdirectory("${INCLUDE_DIR}/imt3dview/CMake" "${AUX_INCLUDE_DIR}/imt3dview")
add_subdirectory("${INCLUDE_DIR}/imt3dgui/CMake" "${AUX_INCLUDE_DIR}/imt3dgui")
add_subdirectory("${INCLUDE_DIR}/imtzip/CMake" "${AUX_INCLUDE_DIR}/imtzip")
add_subdirectory("${INCLUDE_DIR}/imtpy/CMake" "${AUX_INCLUDE_DIR}/imtpy")
add_subdirectory("${INCLUDE_DIR}/imtqml/CMake" "${AUX_INCLUDE_DIR}/imtqml")
add_subdirectory("${INCLUDE_DIR}/imtlic/CMake" "${AUX_INCLUDE_DIR}/imtlic")
add_subdirectory("${INCLUDE_DIR}/imtlicgui/CMake" "${AUX_INCLUDE_DIR}/imtlicgui")
add_subdirectory("${INCLUDE_DIR}/imtdb/CMake" "${AUX_INCLUDE_DIR}/imtdb")

add_subdirectory("${INCLUDE_DIR}/imtgql/CMake" "${AUX_INCLUDE_DIR}/imtgql")

#Impl

add_subdirectory("${IMPL_DIR}/ImtCorePck/CMake" "${AUX_INCLUDE_DIR}/ImtCorePck")
add_dependencies(ImtCorePck imtbase)

add_subdirectory("${IMPL_DIR}/ImtStylePck/CMake" "${AUX_INCLUDE_DIR}/ImtStylePck")
add_dependencies(ImtStylePck imtstyle)

add_subdirectory("${IMPL_DIR}/ImtAuthPck/CMake" "${AUX_INCLUDE_DIR}/ImtAuthPck")
add_dependencies(ImtAuthPck imtbase imtauth)

add_subdirectory("${IMPL_DIR}/ImtAuthGuiPck/CMake" "${AUX_INCLUDE_DIR}/ImtAuthGuiPck")
add_dependencies(ImtAuthGuiPck imtbase imtauth imtauthgui)

add_subdirectory("${IMPL_DIR}/ImtLicensePck/CMake" "${AUX_INCLUDE_DIR}/ImtLicensePck")
add_dependencies(ImtLicensePck imtbase)

add_subdirectory("${IMPL_DIR}/ImtLicenseGuiPck/CMake" "${AUX_INCLUDE_DIR}/ImtLicenseGuiPck")
add_dependencies(ImtLicenseGuiPck imtbase imtlic imtlicgui)

add_subdirectory("${IMPL_DIR}/ImtCryptPck/CMake" "${AUX_INCLUDE_DIR}/ImtCryptPck")
add_dependencies(ImtCryptPck imtbase imtcrypt)

add_subdirectory("${IMPL_DIR}/ImtRestPck/CMake" "${AUX_INCLUDE_DIR}/ImtRestPck")
add_dependencies(ImtRestPck imtbase imtrest)

add_subdirectory("${IMPL_DIR}/Imt3dPck/CMake" "${AUX_INCLUDE_DIR}/Imt3dPck")
add_dependencies(Imt3dPck imt3d)

add_subdirectory("${IMPL_DIR}/ImtReportGuiPck/CMake" "${AUX_INCLUDE_DIR}/ImtReportGuiPck")
add_dependencies(ImtReportGuiPck imtreport imtreportgui)

add_subdirectory("${IMPL_DIR}/ImtRepoPck/CMake" "${AUX_INCLUDE_DIR}/ImtRepoPck")
add_dependencies(ImtRepoPck imtbase)

add_subdirectory("${IMPL_DIR}/ImtReportPck/CMake" "${AUX_INCLUDE_DIR}/ImtReportPck")
add_dependencies(ImtReportPck imtreport imtbase)

add_subdirectory("${IMPL_DIR}/ImtCoreLoc/CMake" "${AUX_INCLUDE_DIR}/ImtCoreLoc")

add_subdirectory("${IMPL_DIR}/ImtGuiPck/CMake" "${AUX_INCLUDE_DIR}/ImtGuiPck")
add_dependencies(ImtGuiPck imt3d imtgui)

add_subdirectory("${IMPL_DIR}/ImtQmlPck/CMake" "${AUX_INCLUDE_DIR}/ImtQmlPck")

add_subdirectory("${IMPL_DIR}/ImtLogPck/CMake" "${AUX_INCLUDE_DIR}/ImtLogPck")
add_dependencies(ImtLogPck imtbase)

add_subdirectory("${IMPL_DIR}/ImtLogGuiPck/CMake" "${AUX_INCLUDE_DIR}/ImtLogGuiPck")
add_dependencies(ImtLogGuiPck imt3d imtgui)

add_subdirectory("${IMPL_DIR}/Imt3dViewPck/CMake" "${AUX_INCLUDE_DIR}/Imt3dViewPck")
add_dependencies(Imt3dViewPck imt3dview)

add_subdirectory("${IMPL_DIR}/Imt3dGuiPck/CMake" "${AUX_INCLUDE_DIR}/Imt3dGuiPck")
add_dependencies(Imt3dGuiPck imt3dview imt3dgui)

add_subdirectory("${IMPL_DIR}/ImtZipPck/CMake" "${AUX_INCLUDE_DIR}/ImtZipPck")
add_dependencies(ImtZipPck imtzip)

add_subdirectory("${IMPL_DIR}/ImtPyPck/CMake" "${AUX_INCLUDE_DIR}/ImtPyPck")
add_dependencies(ImtPyPck imtpy)

add_subdirectory("${IMPL_DIR}/ImtGqlPck/CMake" "${AUX_INCLUDE_DIR}/ImtGqlPck")
add_dependencies(ImtGqlPck imtgql)

# Application
if(NOT UNIX)
#	add_subdirectory("${IMPL_DIR}/EagleView/CMake" "${AUX_INCLUDE_DIR}/EagleView")
#	add_dependencies(EagleView Imt3dViewPck Imt3dGuiPck)
endif()
