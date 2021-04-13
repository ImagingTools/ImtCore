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

#QMake
#imtauth.file = ../../Include/imtauth/QMake/imtauth.pro
#imtauthgui.file = ../../Include/imtauthgui/QMake/imtauthgui.pro
#imtbase.file = ../../Include/imtbase/QMake/imtbase.pro
#imtwidgets.file = ../../Include/imtwidgets/QMake/imtwidgets.pro
#imtrest.file = ../../Include/imtrest/QMake/imtrest.pro
#imtrepo.file = ../../Include/imtrepo/QMake/imtrepo.pro
#imtreport.file = ../../Include/imtreport/QMake/imtreport.pro
#imtrepogui.file = ../../Include/imtrepogui/QMake/imtrepogui.pro
#imtreportgui.file = ../../Include/imtreportgui/QMake/imtreportgui.pro
#imt3d.file = ../../Include/imt3d/QMake/imt3d.pro
#imtgui.file = ../../Include/imtgui/QMake/imtgui.pro
#imtloggui.file = ../../Include/imtloggui/QMake/imtloggui.pro
#imtlog.file = ../../Include/imtlog/QMake/imtlog.pro
#imt3dview.file = ../../Include/imt3dview/QMake/imt3dview.pro
#imt3dgui.file = ../../Include/imt3dgui/QMake/imt3dgui.pro
#imtfile.file = ../../Include/imtfile/QMake/imtfile.pro
#imtzip.file = ../../Include/imtzip/QMake/imtzip.pro
#imtpy.file = ../../Include/imtpy/QMake/imtpy.pro
#imtqml.file = ../../Include/imtqml/QMake/imtqml.pro
#imtlic.file = ../../Include/imtlic/QMake/imtlic.pro
#imtlicgui.file = ../../Include/imtlicgui/QMake/imtlicgui.pro

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


#QMake
#ImtCorePck.file = ../../Impl/ImtCorePck/QMake/ImtCorePck.pro
#ImtAuthPck.file = ../../Impl/ImtAuthPck/QMake/ImtAuthPck.pro
#ImtAuthGuiPck.file = ../../Impl/ImtAuthGuiPck/QMake/ImtAuthGuiPck.pro
#ImtLicensePck.file = ../../Impl/ImtLicensePck/QMake/ImtLicensePck.pro
#ImtLicenseGuiPck.file = ../../Impl/ImtLicenseGuiPck/QMake/ImtLicenseGuiPck.pro
#ImtRestPck.file = ../../Impl/ImtRestPck/QMake/ImtRestPck.pro
#Imt3dPck.file = ../../Impl/Imt3dPck/QMake/Imt3dPck.pro
#ImtRepoGuiPck.file = ../../Impl/ImtRepoGuiPck/QMake/ImtRepoGuiPck.pro
#ImtReportGuiPck.file = ../../Impl/ImtReportGuiPck/QMake/ImtReportGuiPck.pro
#ImtRepoPck.file = ../../Impl/ImtRepoPck/QMake/ImtRepoPck.pro
#ImtReportPck.file = ../../Impl/ImtReportPck/QMake/ImtReportPck.pro
#ImtCoreLoc.file = ../../Impl/ImtCoreLoc/QMake/ImtCoreLoc.pro
#ImtGuiPck.file = ../../Impl/ImtGuiPck/QMake/ImtGuiPck.pro
#ImtQmlPck.file = ../../Impl/ImtQmlPck/QMake/ImtQmlPck.pro
#ImtLogPck.file = ../../Impl/ImtLogPck/QMake/ImtLogPck.pro
#ImtLogGuiPck.file = ../../Impl/ImtLogGuiPck/QMake/ImtLogGuiPck.pro
#Imt3dViewPck.file = ../../Impl/Imt3dViewPck/QMake/Imt3dViewPck.pro
#Imt3dGuiPck.file = ../../Impl/Imt3dGuiPck/QMake/Imt3dGuiPck.pro
#ImtZipPck.file = ../../Impl/ImtZipPck/QMake/ImtZipPck.pro
#ImtPyPck.file = ../../Impl/ImtPyPck/QMake/ImtPyPck.pro

# Application
IF(!UNIX)
#	add_subdirectory("${IMPL_DIR}/EagleView/CMake" "${AUX_INCLUDE_DIR}/EagleView")
ENDIF()
#!linux{
	#SUBDIRS += EagleView
	#EagleView.file = ../../Impl/EagleView/QMake/EagleView.pro
	#EagleView.depends = Imt3dViewPck Imt3dGuiPck
#}

# Tutorials
	
#	add_subdirectory("${IMPL_DIR}/imtservletdemo/CMake" "${AUX_INCLUDE_DIR}/imtservletdemo")
	#SUBDIRS += imtservletdemo
	#imtservletdemo.file = ../../Docs/Tutorials/Include/imtservletdemo/QMake/imtservletdemo.pro

#	 add_subdirectory("${IMPL_DIR}/ServletDemoPck/CMake" "${AUX_INCLUDE_DIR}/ServletDemoPck")
#	SUBDIRS += ServletDemoPck
	#ServletDemoPck.file = ../../Docs/Tutorials/Impl/ServletDemoPck/QMake/ServletDemoPck.pro
     #   ServletDemoPck.depends = ImtRestPck ImtCorePck imtservletdemo

#	add_subdirectory("${IMPL_DIR}/ServerDemo/CMake" "${AUX_INCLUDE_DIR}/ServerDemo")
#	SUBDIRS += ServerDemo
#	ServerDemo.file = ../../Docs/Tutorials/Impl/ServerDemo/QMake/ServerDemo.pro
#	ServerDemo.depends = ServletDemoPck

