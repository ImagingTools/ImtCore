cmake_minimum_required(VERSION 3.1.0)

project(ImtCore)

if (NOT DEFINED IMTCOREDIR)
	set(IMTCOREDIR "$ENV{IMTCOREDIR}")
endif ()

include(${IMTCOREDIR}/Config/CMake/ImtCoreEnv.cmake)

set (INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../Include")
set (IMPL_DIR "${CMAKE_CURRENT_LIST_DIR}/../../Impl")
set (TESTS_DIR "${CMAKE_CURRENT_LIST_DIR}/../../Tests")
set(AUX_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../AuxInclude/${TARGETNAME}/GeneratedFiles")
set(ENABLE_TESTS_IMTCORE True CACHE STRING "Enable tests (True or False), defaults to True")

include_directories(${AUX_INCLUDE_DIR}/..)


#------ DesignToken ------

add_subdirectory("${INCLUDE_DIR}/imtbase/CMake" "${AUX_INCLUDE_DIR}/imtbase")
add_subdirectory("${INCLUDE_DIR}/imtdesign/CMake" "${AUX_INCLUDE_DIR}/imtdesign")

add_subdirectory("${IMPL_DIR}/ImtDesignPck/CMake" "${AUX_INCLUDE_DIR}/ImtDesignPck")
add_dependencies(ImtDesignPck imtdesign)

add_subdirectory("${IMPL_DIR}/DesignTokenCreatorExe/CMake" "${AUX_INCLUDE_DIR}/DesignTokenCreator")
add_dependencies(DesignTokenCreator imtbase imtdesign)

add_subdirectory("${INCLUDE_DIR}/imtstyle/CMake" "${AUX_INCLUDE_DIR}/imtstyle")
add_dependencies(imtstyle DesignTokenCreator)


#-------- Include --------

add_subdirectory("${INCLUDE_DIR}/imtapp/CMake" "${AUX_INCLUDE_DIR}/imtapp")
add_subdirectory("${INCLUDE_DIR}/imtdev/CMake" "${AUX_INCLUDE_DIR}/imtdev")
add_subdirectory("${INCLUDE_DIR}/imtrest/CMake" "${AUX_INCLUDE_DIR}/imtrest")
add_subdirectory("${INCLUDE_DIR}/imtgql/CMake" "${AUX_INCLUDE_DIR}/imtgql")
add_dependencies(imtgql imtrest)
add_subdirectory("${INCLUDE_DIR}/imtauth/CMake" "${AUX_INCLUDE_DIR}/imtauth")
add_subdirectory("${INCLUDE_DIR}/imtauthgui/CMake" "${AUX_INCLUDE_DIR}/imtauthgui")
add_dependencies(imtauthgui DesignTokenCreator)
add_subdirectory("${INCLUDE_DIR}/imtcrypt/CMake" "${AUX_INCLUDE_DIR}/imtcrypt")
add_subdirectory("${INCLUDE_DIR}/imtwidgets/CMake" "${AUX_INCLUDE_DIR}/imtwidgets")
add_subdirectory("${INCLUDE_DIR}/imtrepo/CMake" "${AUX_INCLUDE_DIR}/imtrepo")
add_subdirectory("${INCLUDE_DIR}/imtreport/CMake" "${AUX_INCLUDE_DIR}/imtreport")
add_subdirectory("${INCLUDE_DIR}/imtreportgui/CMake" "${AUX_INCLUDE_DIR}/imtreportgui")
add_subdirectory("${INCLUDE_DIR}/imtgui/CMake" "${AUX_INCLUDE_DIR}/imtgui")
add_dependencies(imtgui imtwidgets)
add_subdirectory("${INCLUDE_DIR}/imtguigql/CMake" "${AUX_INCLUDE_DIR}/imtguigql")
add_dependencies(imtguigql imtgql)
add_subdirectory("${INCLUDE_DIR}/imtloggui/CMake" "${AUX_INCLUDE_DIR}/imtloggui")
add_subdirectory("${INCLUDE_DIR}/imtfile/CMake" "${AUX_INCLUDE_DIR}/imtfile")
add_subdirectory("${INCLUDE_DIR}/imtlog/CMake" "${AUX_INCLUDE_DIR}/imtlog")
add_subdirectory("${INCLUDE_DIR}/imtzip/CMake" "${AUX_INCLUDE_DIR}/imtzip")
add_subdirectory("${INCLUDE_DIR}/imtlic/CMake" "${AUX_INCLUDE_DIR}/imtlic")
add_subdirectory("${INCLUDE_DIR}/imtlicgql/CMake" "${AUX_INCLUDE_DIR}/imtlicgql")
add_dependencies(imtlicgql imtgql)
add_subdirectory("${INCLUDE_DIR}/imtlicgui/CMake" "${AUX_INCLUDE_DIR}/imtlicgui")
add_subdirectory("${INCLUDE_DIR}/imtcom/CMake" "${AUX_INCLUDE_DIR}/imtcom")
add_subdirectory("${INCLUDE_DIR}/imthttp/CMake" "${AUX_INCLUDE_DIR}/imthttp")
add_dependencies(imthttp imtcom)
add_subdirectory("${INCLUDE_DIR}/imtauthgql/CMake" "${AUX_INCLUDE_DIR}/imtauthgql")
add_subdirectory("${INCLUDE_DIR}/imtauthdb/CMake" "${AUX_INCLUDE_DIR}/imtauthdb")
add_subdirectory("${INCLUDE_DIR}/imtgeo/CMake" "${AUX_INCLUDE_DIR}/imtgeo")

if(DEFINED USE_MONGODB AND USE_MONGODB)
	add_subdirectory("${INCLUDE_DIR}/imtmongo/CMake" "${AUX_INCLUDE_DIR}/imtmongo")
endif()

if(NOT DEFINED USE_EJ OR USE_EJ)
	add_subdirectory("${INCLUDE_DIR}/imtej/CMake" "${AUX_INCLUDE_DIR}/imtej")
endif()

if(NOT DEFINED USE_QT3D OR USE_QT3D)
	add_subdirectory("${INCLUDE_DIR}/imt3d/CMake" "${AUX_INCLUDE_DIR}/imt3d")
	add_subdirectory("${INCLUDE_DIR}/imt3dview/CMake" "${AUX_INCLUDE_DIR}/imt3dview")
	add_subdirectory("${INCLUDE_DIR}/imt3dgui/CMake" "${AUX_INCLUDE_DIR}/imt3dgui")
	add_dependencies(imt3dgui DesignTokenCreator)
endif()

#add_subdirectory("${INCLUDE_DIR}/imtpy/CMake" "${AUX_INCLUDE_DIR}/imtpy")

if(NOT DEFINED USE_QTQML OR USE_QTQML)
	add_subdirectory("${INCLUDE_DIR}/imtqml/CMake" "${AUX_INCLUDE_DIR}/imtqml")
	add_subdirectory("${INCLUDE_DIR}/imtstyleqml/CMake" "${AUX_INCLUDE_DIR}/imtstyleqml")
endif()

if(NOT DEFINED USE_QTDB OR USE_QTDB)
	add_subdirectory("${INCLUDE_DIR}/imtdb/CMake" "${AUX_INCLUDE_DIR}/imtdb")
	add_subdirectory("${INCLUDE_DIR}/imtlicdb/CMake" "${AUX_INCLUDE_DIR}/imtlicdb")
endif()


#-------- Impl --------

if(NOT ANDROID)

	add_subdirectory("${IMPL_DIR}/ImtCorePck/CMake" "${AUX_INCLUDE_DIR}/ImtCorePck")
	add_dependencies(ImtCorePck imtbase)

	add_subdirectory("${IMPL_DIR}/ImtAppPck/CMake" "${AUX_INCLUDE_DIR}/ImtAppPck")
	add_dependencies(ImtAppPck imtapp)

	add_subdirectory("${IMPL_DIR}/ImtDevPck/CMake" "${AUX_INCLUDE_DIR}/ImtDevPck")
	add_dependencies(ImtDevPck imtdev)

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

	add_subdirectory("${IMPL_DIR}/ImtReportGuiPck/CMake" "${AUX_INCLUDE_DIR}/ImtReportGuiPck")
	add_dependencies(ImtReportGuiPck imtreport imtreportgui)

	add_subdirectory("${IMPL_DIR}/ImtRepoPck/CMake" "${AUX_INCLUDE_DIR}/ImtRepoPck")
	add_dependencies(ImtRepoPck imtbase)

	add_subdirectory("${IMPL_DIR}/ImtReportPck/CMake" "${AUX_INCLUDE_DIR}/ImtReportPck")
	add_dependencies(ImtReportPck imtreport imtbase)

	add_subdirectory("${IMPL_DIR}/ImtCoreLoc/CMake" "${AUX_INCLUDE_DIR}/ImtCoreLoc")

	add_subdirectory("${IMPL_DIR}/ImtLogPck/CMake" "${AUX_INCLUDE_DIR}/ImtLogPck")
	add_dependencies(ImtLogPck imtbase)

	add_subdirectory("${IMPL_DIR}/ImtZipPck/CMake" "${AUX_INCLUDE_DIR}/ImtZipPck")
	add_dependencies(ImtZipPck imtzip)

	add_subdirectory("${IMPL_DIR}/ImtFilePck/CMake" "${AUX_INCLUDE_DIR}/ImtFilePck")
	add_dependencies(ImtFilePck imtfile)

	add_subdirectory("${IMPL_DIR}/ImtGqlPck/CMake" "${AUX_INCLUDE_DIR}/ImtGqlPck")
	add_dependencies(ImtGqlPck imtgql)

	add_subdirectory("${IMPL_DIR}/ImtComPck/CMake" "${AUX_INCLUDE_DIR}/ImtComPck")
	add_dependencies(ImtComPck imtcom)

	add_subdirectory("${IMPL_DIR}/ImtHttpPck/CMake" "${AUX_INCLUDE_DIR}/ImtHttpPck")
	add_dependencies(ImtHttpPck imthttp)

	add_subdirectory("${IMPL_DIR}/ImtGeoPck/CMake" "${AUX_INCLUDE_DIR}/ImtGeoPck")
	add_dependencies(ImtGeoPck imtgeo)

    if(DEFINED USE_MONGODB AND USE_MONGODB)
		add_subdirectory("${IMPL_DIR}/ImtMongoPck/CMake" "${AUX_INCLUDE_DIR}/ImtMongoPck")
		add_dependencies(ImtMongoPck imtmongo)
	endif()

	if(NOT DEFINED USE_EJ OR USE_EJ)
		add_subdirectory("${IMPL_DIR}/ImtEjPck/CMake" "${AUX_INCLUDE_DIR}/ImtEjPck")
		add_dependencies(ImtEjPck imtej)
	endif()

	if(NOT DEFINED USE_QTDB OR USE_QTDB)
		add_subdirectory("${IMPL_DIR}/ImtAuthDbPck/CMake" "${AUX_INCLUDE_DIR}/ImtAuthDbPck")
		add_dependencies(ImtAuthDbPck imtauthdb imtdb)

		add_subdirectory("${IMPL_DIR}/ImtDatabasePck/CMake" "${AUX_INCLUDE_DIR}/ImtDatabasePck")
		add_dependencies(ImtDatabasePck imtbase imtdb)

		add_subdirectory("${IMPL_DIR}/ImtLicenseDbPck/CMake" "${AUX_INCLUDE_DIR}/ImtLicenseDbPck")
		add_dependencies(ImtLicenseDbPck imtbase imtdb imtlicdb)

		add_subdirectory("${IMPL_DIR}/ImtLicenseGqlPck/CMake" "${AUX_INCLUDE_DIR}/ImtLicenseGqlPck")
		add_dependencies(ImtLicenseGuiPck imtbase imtlic imtlicgql imtdb)

		add_subdirectory("${IMPL_DIR}/ImtAuthGqlPck/CMake" "${AUX_INCLUDE_DIR}/ImtAuthGqlPck")
		add_dependencies(ImtAuthGqlPck imtauthgql imtdb)

		add_subdirectory("${IMPL_DIR}/ImtGuiGqlPck/CMake" "${AUX_INCLUDE_DIR}/ImtGuiGqlPck")
		add_dependencies(ImtGuiGqlPck imtguigql imtdb)
	endif()

	if(NOT DEFINED USE_QT3D OR USE_QT3D)
		add_subdirectory("${IMPL_DIR}/Imt3dPck/CMake" "${AUX_INCLUDE_DIR}/Imt3dPck")
		add_dependencies(Imt3dPck imt3d)
		add_subdirectory("${IMPL_DIR}/ImtGuiPck/CMake" "${AUX_INCLUDE_DIR}/ImtGuiPck")
		add_dependencies(ImtGuiPck imt3d imtgui)

		add_subdirectory("${IMPL_DIR}/ImtLogGuiPck/CMake" "${AUX_INCLUDE_DIR}/ImtLogGuiPck")
		add_dependencies(ImtLogGuiPck imt3d imtgui)

		add_subdirectory("${IMPL_DIR}/Imt3dViewPck/CMake" "${AUX_INCLUDE_DIR}/Imt3dViewPck")
		add_dependencies(Imt3dViewPck imt3dview)

		add_subdirectory("${IMPL_DIR}/Imt3dGuiPck/CMake" "${AUX_INCLUDE_DIR}/Imt3dGuiPck")
		add_dependencies(Imt3dGuiPck imt3dview imt3dgui)
	endif()

	if(NOT DEFINED USE_QTQML OR USE_QTQML)
		add_subdirectory("${IMPL_DIR}/ImtQmlPck/CMake" "${AUX_INCLUDE_DIR}/ImtQmlPck")
		add_dependencies(ImtQmlPck imtqml)
		add_subdirectory("${IMPL_DIR}/ImtControlsGallery/CMake" "${AUX_INCLUDE_DIR}/ImtControlsGallery")
		
	endif()

endif()

#add_subdirectory("${IMPL_DIR}/ImtPyPck/CMake" "${AUX_INCLUDE_DIR}/ImtPyPck")
#add_dependencies(ImtPyPck imtpy)


# Application
if(NOT UNIX)
#	add_subdirectory("${IMPL_DIR}/EagleView/CMake" "${AUX_INCLUDE_DIR}/EagleView")
#	add_dependencies(EagleView Imt3dViewPck Imt3dGuiPck)
endif()


#-------- Tests --------
if (${ENABLE_TESTS_IMTCORE})
    add_subdirectory("${INCLUDE_DIR}/imttest/CMake" "${AUX_INCLUDE_DIR}/imttest")

    if(NOT DEFINED USE_QTDB OR USE_QTDB)
        add_subdirectory("${IMPL_DIR}/ImtTestPck/CMake" "${AUX_INCLUDE_DIR}/ImtTestPck")
        add_dependencies(ImtTestPck imtbase imtdb imttest)
    endif()

    add_subdirectory("${INCLUDE_DIR}/imtbase/Test/CMake" "${AUX_INCLUDE_DIR}/imtbase/Test")

    add_subdirectory("${TESTS_DIR}/ObjectCollectionTest/CMake" "${AUX_INCLUDE_DIR}/ObjectCollectionTest")
    add_subdirectory("${TESTS_DIR}/FileCollectionTest/CMake" "${AUX_INCLUDE_DIR}/FileCollectionTest")
    add_subdirectory("${TESTS_DIR}/RevisionControllerTest/CMake" "${AUX_INCLUDE_DIR}/RevisionControllerTest")
endif()

