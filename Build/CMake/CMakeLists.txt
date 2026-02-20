cmake_minimum_required(VERSION 3.26)

project(ImtCore)

if(NOT DEFINED QT_VERSION_MAJOR)
	find_package(QT NAMES Qt6 Qt5 COMPONENTS Core)
	find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS Core Widgets Core Gui Xml Network Svg Sql WebSockets Concurrent PrintSupport REQUIRED)
	if(QT_VERSION_MAJOR EQUAL 6)
		find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS Core5Compat REQUIRED)
	endif()
	if(NOT DEFINED USE_QTQML OR USE_QTQML)
		find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS QuickWidgets QuickControls2 Qml REQUIRED)
	endif()
endif()

if(UNIX AND NOT APPLE)
	if (NOT CMAKE_SHARED_LINKER_FLAGS)
		set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined")
	endif()
endif()

get_filename_component(IMTCOREDIR "../.." ABSOLUTE BASE_DIR ${CMAKE_CURRENT_LIST_DIR})

if(NOT DEFINED IMTCOREDIR_BUILD)
	set(IMTCOREDIR_BUILD "${IMTCOREDIR}")
endif()

include(${IMTCOREDIR}/Config/CMake/ImtCoreEnv.cmake)
include(${IMTCOREDIR}/Config/CMake/PartituraUpgrade.cmake)
if (NOT ANDROID)
	imt_core_upgrade_partitura("${CMAKE_CURRENT_LIST_DIR}/../..")
endif()

message(VERBOSE "ImtCore ACFDIR_BUILD ${ACFDIR_BUILD}")

set (INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../Include")
set (IMPL_DIR "${CMAKE_CURRENT_LIST_DIR}/../../Impl")
set (QML_DIR "${CMAKE_CURRENT_LIST_DIR}/../../Qml")
set (SDL_DIR "${CMAKE_CURRENT_LIST_DIR}/../../Sdl")
set (TESTS_DIR "${CMAKE_CURRENT_LIST_DIR}/../../Tests")

set(AUX_INCLUDE_DIR "${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles")

set(BUILD_TESTING OFF CACHE BOOL "Configures projects for testing")

include_directories(${AUX_INCLUDE_DIR}/..)
include_directories(${AUX_INCLUDE_DIR})


#------ Required libs ------
add_subdirectory("${INCLUDE_DIR}/imtbase/CMake" "${AUX_INCLUDE_DIR}/imtbase")

add_subdirectory("${INCLUDE_DIR}/imtdesign/CMake" "${AUX_INCLUDE_DIR}/imtdesign")
add_subdirectory("${IMPL_DIR}/ImtDesignPck/CMake" "${AUX_INCLUDE_DIR}/ImtDesignPck")
add_dependencies(ImtDesignPck imtdesign)

add_subdirectory("${INCLUDE_DIR}/imtfile/CMake" "${AUX_INCLUDE_DIR}/imtfile")

add_subdirectory("${INCLUDE_DIR}/imtddl/CMake" "${AUX_INCLUDE_DIR}/imtddl")
add_subdirectory("${IMPL_DIR}/ImtDdlPck/CMake" "${AUX_INCLUDE_DIR}/ImtDdlPck")
add_dependencies(ImtDdlPck imtddl)

#------ DesignToken ------
add_subdirectory("${IMPL_DIR}/DesignTokenCreatorExe/CMake" "${AUX_INCLUDE_DIR}/DesignTokenCreator")
add_dependencies(DesignTokenCreator imtbase imtdesign)

#------ DDLCodeCreator ------
add_subdirectory("${IMPL_DIR}/DdlCodeCreatorExe/CMake" "${AUX_INCLUDE_DIR}/DdlCodeCreator")
add_dependencies(DdlCodeCreator imtddl)

#------ SDL ------
add_subdirectory("${INCLUDE_DIR}/imtsdl/CMake" "${AUX_INCLUDE_DIR}/imtsdl")
add_subdirectory("${IMPL_DIR}/ImtSdlPck/CMake" "${AUX_INCLUDE_DIR}/ImtSdlPck")
add_dependencies(ImtSdlPck imtsdl imtfile imtbase)

add_subdirectory("${INCLUDE_DIR}/imtsdlgenqml/CMake" "${AUX_INCLUDE_DIR}/imtsdlgenqml")
add_subdirectory("${IMPL_DIR}/ImtSdlGenQmlPck/CMake" "${AUX_INCLUDE_DIR}/ImtSdlGenQmlPck")
add_dependencies(ImtSdlGenQmlPck imtsdlgenqml imtsdl imtfile imtbase)

add_subdirectory("${INCLUDE_DIR}/imtsdlgencpp/CMake" "${AUX_INCLUDE_DIR}/imtsdlgencpp")
add_subdirectory("${IMPL_DIR}/ImtSdlGenCppPck/CMake" "${AUX_INCLUDE_DIR}/ImtSdlGenCppPck")
add_dependencies(ImtSdlGenCppPck imtsdlgencpp imtsdlgenqml imtsdl imtfile imtbase)

add_subdirectory("${IMPL_DIR}/SdlCodeGeneratorExe/CMake" "${AUX_INCLUDE_DIR}/SdlCodeGenerator")
add_dependencies(SdlCodeGenerator ImtSdlGenCppPck ImtSdlGenQmlPck ImtSdlPck)

#------ OpenAPI ------
add_subdirectory("${INCLUDE_DIR}/imtoas/CMake" "${AUX_INCLUDE_DIR}/imtoas")
add_subdirectory("${IMPL_DIR}/ImtOasPck/CMake" "${AUX_INCLUDE_DIR}/ImtOasPck")
add_subdirectory("${IMPL_DIR}/OasCodeGeneratorExe/CMake" "${AUX_INCLUDE_DIR}/OasCodeGeneratorExe")

#------ BaseTests ------
if (NOT DEFINED BUILD_TESTING OR BUILD_TESTING)
	add_subdirectory("${INCLUDE_DIR}/imtbase/Test/CMake" "${AUX_INCLUDE_DIR}/imtbase/Test")
	add_subdirectory("${INCLUDE_DIR}/imtsdlgencpp/Test/CMake" "${AUX_INCLUDE_DIR}/ImtSdlGenCppTest")
	add_subdirectory("${INCLUDE_DIR}/imtsdlgenqml/Test/CMake" "${AUX_INCLUDE_DIR}/ImtSdlGenQmlTest")
endif()


if(NOT DEFINED IMT_BUILD_ONLYTOOLS OR NOT IMT_BUILD_ONLYTOOLS)
	#-------- Sdl --------
	if(NOT DEFINED USE_SDL OR USE_SDL)
		add_subdirectory("${SDL_DIR}/imtbase/CMake" "${AUX_INCLUDE_DIR}/imtbasesdl")
		add_subdirectory("${SDL_DIR}/imt2d/CMake" "${AUX_INCLUDE_DIR}/imt2dsdl")
		add_subdirectory("${SDL_DIR}/imtcolor/CMake" "${AUX_INCLUDE_DIR}/imtcolorsdl")
		add_dependencies(imt2dsdl imtbasesdl imtcolorsdl)
		add_subdirectory("${SDL_DIR}/imtauth/CMake" "${AUX_INCLUDE_DIR}/imtauthsdl")
		add_subdirectory("${SDL_DIR}/imtlic/CMake" "${AUX_INCLUDE_DIR}/imtlicsdl")
		add_subdirectory("${SDL_DIR}/imtapp/CMake" "${AUX_INCLUDE_DIR}/imtappsdl")
		add_subdirectory("${SDL_DIR}/imtpay/CMake" "${AUX_INCLUDE_DIR}/imtpaysdl")
		add_dependencies(imtpaysdl imtbasesdl)
	endif()

	#-------- Libraries --------
	add_subdirectory("${INCLUDE_DIR}/imtapp/CMake" "${AUX_INCLUDE_DIR}/imtapp")
	add_subdirectory("${INCLUDE_DIR}/imtmail/CMake" "${AUX_INCLUDE_DIR}/imtmail")
	add_subdirectory("${INCLUDE_DIR}/imtdev/CMake" "${AUX_INCLUDE_DIR}/imtdev")
	add_subdirectory("${INCLUDE_DIR}/imtdevgui/CMake" "${AUX_INCLUDE_DIR}/imtdevgui")
	add_dependencies(imtdevgui imtdev)
	add_subdirectory("${INCLUDE_DIR}/imtdoc/CMake" "${AUX_INCLUDE_DIR}/imtdoc")
	add_dependencies(imtdoc imtbase)
	add_subdirectory("${INCLUDE_DIR}/imtrest/CMake" "${AUX_INCLUDE_DIR}/imtrest")
	add_dependencies(imtrest imtbasesdl)
	add_subdirectory("${INCLUDE_DIR}/imtstyle/CMake" "${AUX_INCLUDE_DIR}/imtstyle")
	add_dependencies(imtstyle DesignTokenCreator)
	add_subdirectory("${INCLUDE_DIR}/imtcol/CMake" "${AUX_INCLUDE_DIR}/imtcol")
	add_dependencies(imtcol imtbase imtbasesdl)
	add_subdirectory("${INCLUDE_DIR}/imtgql/CMake" "${AUX_INCLUDE_DIR}/imtgql")
	add_subdirectory("${INCLUDE_DIR}/imtservergql/CMake" "${AUX_INCLUDE_DIR}/imtservergql")
	add_dependencies(imtservergql imtgql imtrest imtcol)
	add_subdirectory("${INCLUDE_DIR}/imtclientgql/CMake" "${AUX_INCLUDE_DIR}/imtclientgql")
	add_subdirectory("${INCLUDE_DIR}/imtauth/CMake" "${AUX_INCLUDE_DIR}/imtauth")
	add_subdirectory("${INCLUDE_DIR}/imtcrypt/CMake" "${AUX_INCLUDE_DIR}/imtcrypt")
	add_subdirectory("${INCLUDE_DIR}/imtwidgets/CMake" "${AUX_INCLUDE_DIR}/imtwidgets")
	add_subdirectory("${INCLUDE_DIR}/imtrepo/CMake" "${AUX_INCLUDE_DIR}/imtrepo")
	add_subdirectory("${INCLUDE_DIR}/imtreport/CMake" "${AUX_INCLUDE_DIR}/imtreport")
	add_subdirectory("${INCLUDE_DIR}/imtreportgui/CMake" "${AUX_INCLUDE_DIR}/imtreportgui")
	add_subdirectory("${INCLUDE_DIR}/imtgui/CMake" "${AUX_INCLUDE_DIR}/imtgui")
	add_dependencies(imtgui imtwidgets imtcol)
	add_subdirectory("${INCLUDE_DIR}/imtguigql/CMake" "${AUX_INCLUDE_DIR}/imtguigql")
	add_dependencies(imtguigql imtgql imtservergql)
	add_subdirectory("${INCLUDE_DIR}/imtloggui/CMake" "${AUX_INCLUDE_DIR}/imtloggui")
	add_subdirectory("${INCLUDE_DIR}/imtlog/CMake" "${AUX_INCLUDE_DIR}/imtlog")
	add_subdirectory("${INCLUDE_DIR}/imtzip/CMake" "${AUX_INCLUDE_DIR}/imtzip")
	add_subdirectory("${INCLUDE_DIR}/imtlic/CMake" "${AUX_INCLUDE_DIR}/imtlic")
	add_subdirectory("${INCLUDE_DIR}/imtlicgql/CMake" "${AUX_INCLUDE_DIR}/imtlicgql")
	add_dependencies(imtlicgql imtgql imtservergql)
	add_subdirectory("${INCLUDE_DIR}/imtlicgui/CMake" "${AUX_INCLUDE_DIR}/imtlicgui")
	add_subdirectory("${INCLUDE_DIR}/imtcom/CMake" "${AUX_INCLUDE_DIR}/imtcom")
	add_subdirectory("${INCLUDE_DIR}/imthttp/CMake" "${AUX_INCLUDE_DIR}/imthttp")
	add_dependencies(imthttp imtcom)
	add_subdirectory("${INCLUDE_DIR}/imtauthdb/CMake" "${AUX_INCLUDE_DIR}/imtauthdb")
	add_subdirectory("${INCLUDE_DIR}/imtservice/CMake" "${AUX_INCLUDE_DIR}/imtservice")
	add_subdirectory("${INCLUDE_DIR}/imtserverapp/CMake" "${AUX_INCLUDE_DIR}/imtserverapp")
	add_dependencies(imtserverapp imtbasesdl)
	add_subdirectory("${INCLUDE_DIR}/imthype/CMake" "${AUX_INCLUDE_DIR}/imthype")
	add_subdirectory("${INCLUDE_DIR}/imthypegui/CMake" "${AUX_INCLUDE_DIR}/imthypegui")

	if(NOT DEFINED USE_GEO OR USE_GEO)
		add_subdirectory("${INCLUDE_DIR}/imtgeo/CMake" "${AUX_INCLUDE_DIR}/imtgeo")
		add_dependencies(imtgeo imtbasesdl)
	endif()

	if(DEFINED USE_MONGODB AND USE_MONGODB)
		add_subdirectory("${INCLUDE_DIR}/imtmongo/CMake" "${AUX_INCLUDE_DIR}/imtmongo")
	endif()

	if(NOT DEFINED USE_EJ OR USE_EJ)
		add_subdirectory("${INCLUDE_DIR}/imtej/CMake" "${AUX_INCLUDE_DIR}/imtej")
	endif()

	if(NOT DEFINED USE_QT3D OR USE_QT3D)
		find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS OpenGLWidgets REQUIRED)

		add_subdirectory("${INCLUDE_DIR}/imt3d/CMake" "${AUX_INCLUDE_DIR}/imt3d")
		add_subdirectory("${INCLUDE_DIR}/imt3dview/CMake" "${AUX_INCLUDE_DIR}/imt3dview")
		add_subdirectory("${INCLUDE_DIR}/imt3dgui/CMake" "${AUX_INCLUDE_DIR}/imt3dgui")
		add_dependencies(imt3dgui DesignTokenCreator)
	endif()

	if(NOT DEFINED USE_QTDB OR USE_QTDB)
		add_subdirectory("${INCLUDE_DIR}/imtdb/CMake" "${AUX_INCLUDE_DIR}/imtdb")
		add_subdirectory("${INCLUDE_DIR}/imtdbgui/CMake" "${AUX_INCLUDE_DIR}/imtdbgui")
		add_subdirectory("${INCLUDE_DIR}/imtlicdb/CMake" "${AUX_INCLUDE_DIR}/imtlicdb")
		add_subdirectory("${INCLUDE_DIR}/imtdbgql/CMake" "${AUX_INCLUDE_DIR}/imtdbgql")
		add_subdirectory("${INCLUDE_DIR}/imthypedb/CMake" "${AUX_INCLUDE_DIR}/imthypedb")
	endif()

	if(DEFINED USE_MDBX AND USE_MDBX)
		add_subdirectory("${INCLUDE_DIR}/imtmdbx/CMake" "${AUX_INCLUDE_DIR}/imtmdbx")
	endif()

	#-------- Qml --------
	if(NOT DEFINED USE_QTQML OR USE_QTQML)
		# TODO return it when new wrappers will be finished
		# add_subdirectory("${INCLUDE_DIR}/imtqml2d/CMake" "${AUX_INCLUDE_DIR}/imtqml2d")
		add_subdirectory("${INCLUDE_DIR}/imtauthgui/CMake" "${AUX_INCLUDE_DIR}/imtauthgui")
		add_dependencies(imtauthgui DesignTokenCreator)
		add_subdirectory("${INCLUDE_DIR}/imtauthgql/CMake" "${AUX_INCLUDE_DIR}/imtauthgql")
		add_subdirectory("${QML_DIR}/imtdocgui/CMake" "${AUX_INCLUDE_DIR}/imtdocguiqml")
		add_subdirectory("${QML_DIR}/imtauthgui/CMake" "${AUX_INCLUDE_DIR}/imtauthguiqml")
		add_subdirectory("${QML_DIR}/imtcolgui/CMake" "${AUX_INCLUDE_DIR}/imtcolguiqml")
		add_subdirectory("${QML_DIR}/imtgui/CMake" "${AUX_INCLUDE_DIR}/imtguiqml")
		add_subdirectory("${QML_DIR}/imtguigql/CMake" "${AUX_INCLUDE_DIR}/imtguigqlqml")
		add_subdirectory("${QML_DIR}/imtlicgui/CMake" "${AUX_INCLUDE_DIR}/imtlicguiqml")
		add_subdirectory("${QML_DIR}/imtcontrols/CMake" "${AUX_INCLUDE_DIR}/imtcontrolsqml")
		add_subdirectory("${INCLUDE_DIR}/imtqml/CMake" "${AUX_INCLUDE_DIR}/imtqml")
		add_dependencies(imtqml imtbasesdl)
		add_subdirectory("${INCLUDE_DIR}/imtstylecontrolsqml/CMake" "${AUX_INCLUDE_DIR}/imtstylecontrolsqml")
	endif()

	#-------- Packages --------
	if(NOT ANDROID)
		add_subdirectory("${IMPL_DIR}/ImtCorePck/CMake" "${AUX_INCLUDE_DIR}/ImtCorePck")
		add_dependencies(ImtCorePck imtbase)

		add_subdirectory("${IMPL_DIR}/ImtAppPck/CMake" "${AUX_INCLUDE_DIR}/ImtAppPck")
		add_dependencies(ImtAppPck imtapp)

		add_subdirectory("${IMPL_DIR}/ImtMailPck/CMake" "${AUX_INCLUDE_DIR}/ImtMailPck")
		add_dependencies(ImtMailPck imtmail)

		add_subdirectory("${IMPL_DIR}/ImtDevPck/CMake" "${AUX_INCLUDE_DIR}/ImtDevPck")
		add_dependencies(ImtDevPck imtdev)

		add_subdirectory("${IMPL_DIR}/ImtDevGuiPck/CMake" "${AUX_INCLUDE_DIR}/ImtDevGuiPck")
		add_dependencies(ImtDevGuiPck imtdevgui)

		add_subdirectory("${IMPL_DIR}/ImtDocPck/CMake" "${AUX_INCLUDE_DIR}/ImtDocPck")
		add_dependencies(ImtDocPck imtdoc)

		add_subdirectory("${IMPL_DIR}/ImtStylePck/CMake" "${AUX_INCLUDE_DIR}/ImtStylePck")
		add_dependencies(ImtStylePck imtstyle)

		add_subdirectory("${IMPL_DIR}/ImtAuthPck/CMake" "${AUX_INCLUDE_DIR}/ImtAuthPck")
		add_dependencies(ImtAuthPck imtbase imtauth)

	if(NOT DEFINED USE_QTQML OR USE_QTQML)
		add_subdirectory("${IMPL_DIR}/ImtAuthGuiPck/CMake" "${AUX_INCLUDE_DIR}/ImtAuthGuiPck")
		add_dependencies(ImtAuthGuiPck imtbase imtauth imtauthgui)
	endif()

	if(DEFINED USE_MDBX AND USE_MDBX)
		add_subdirectory("${IMPL_DIR}/ImtMdbxPck/CMake" "${AUX_INCLUDE_DIR}/ImtMdbxPck")
		add_dependencies(ImtMdbxPck imtmdbx)
	endif()

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

		add_subdirectory("${IMPL_DIR}/ImtServerGqlPck/CMake" "${AUX_INCLUDE_DIR}/ImtServerGqlPck")
		add_dependencies(ImtServerGqlPck imtservergql)

		add_subdirectory("${IMPL_DIR}/ImtClientGqlPck/CMake" "${AUX_INCLUDE_DIR}/ImtClientGqlPck")
		add_dependencies(ImtClientGqlPck ImtGqlPck imtclientgql)

		add_subdirectory("${IMPL_DIR}/ImtComPck/CMake" "${AUX_INCLUDE_DIR}/ImtComPck")
		add_dependencies(ImtComPck imtcom)

		add_subdirectory("${IMPL_DIR}/ImtServicePck/CMake" "${AUX_INCLUDE_DIR}/ImtServicePck")
		add_dependencies(ImtServicePck imtservice)

		add_subdirectory("${IMPL_DIR}/ImtHttpPck/CMake" "${AUX_INCLUDE_DIR}/ImtHttpPck")
		add_dependencies(ImtHttpPck imthttp)

		add_subdirectory("${IMPL_DIR}/ImtColPck/CMake" "${AUX_INCLUDE_DIR}/ImtColPck")
		add_dependencies(ImtColPck imtcol)

		add_subdirectory("${IMPL_DIR}/ImtServerAppPck/CMake" "${AUX_INCLUDE_DIR}/ImtServerAppPck")
		add_dependencies(ImtServerAppPck imtserverapp)

		add_subdirectory("${IMPL_DIR}/ImtHypePck/CMake" "${AUX_INCLUDE_DIR}/ImtHypePck")
		add_dependencies(ImtHypePck imthype)

		add_subdirectory("${IMPL_DIR}/ImtHypeGuiPck/CMake" "${AUX_INCLUDE_DIR}/ImtHypeGuiPck")
		add_dependencies(ImtHypeGuiPck imthypegui imthype)

		if(NOT DEFINED USE_GEO OR USE_GEO)
			add_subdirectory("${IMPL_DIR}/ImtGeoPck/CMake" "${AUX_INCLUDE_DIR}/ImtGeoPck")
			add_dependencies(ImtGeoPck imtgeo)
		endif()

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

			add_subdirectory("${IMPL_DIR}/ImtDatabaseGqlPck/CMake" "${AUX_INCLUDE_DIR}/ImtDatabaseGqlPck")
			add_dependencies(ImtDatabaseGqlPck imtdbgql)

			add_subdirectory("${IMPL_DIR}/ImtGuiGqlPck/CMake" "${AUX_INCLUDE_DIR}/ImtGuiGqlPck")
			add_dependencies(ImtGuiGqlPck imtguigql imtdb)

			add_subdirectory("${IMPL_DIR}/ImtHypeDbPck/CMake" "${AUX_INCLUDE_DIR}/ImtHypeDbPck")
			add_dependencies(ImtHypeDbPck imthypedb imtdb imthype)
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
			if(NOT DEFINED USE_QTDB OR USE_QTDB)
				add_subdirectory("${IMPL_DIR}/ImtAuthGqlPck/CMake" "${AUX_INCLUDE_DIR}/ImtAuthGqlPck")
				add_dependencies(ImtAuthGqlPck imtauthgql imtdb)
			endif()
		endif()
	endif()

	#-------- Tests --------
	if (NOT DEFINED BUILD_TESTING OR BUILD_TESTING)
		add_subdirectory("${INCLUDE_DIR}/imttest/CMake" "${AUX_INCLUDE_DIR}/imttest")

		if(NOT DEFINED USE_QTDB OR USE_QTDB)
			add_subdirectory("${IMPL_DIR}/ImtTestPck/CMake" "${AUX_INCLUDE_DIR}/ImtTestPck")
			add_dependencies(ImtTestPck imtbase imtdb imttest)
		endif()

		if(DEFINED USE_MDBX AND USE_MDBX)
			add_subdirectory("${INCLUDE_DIR}/imtmdbx/Test/CMake" "${AUX_INCLUDE_DIR}/imtmdbx/Test")
		endif()

		add_subdirectory("${INCLUDE_DIR}/imtgql/Test/CMake" "${AUX_INCLUDE_DIR}/imtgql/Test")
		add_subdirectory("${INCLUDE_DIR}/imtgqltest/CMake" "${AUX_INCLUDE_DIR}/imtgqltest")
		add_subdirectory("${INCLUDE_DIR}/imt3d/Test/CMake" "${AUX_INCLUDE_DIR}/imt3d/Test")

		add_subdirectory("${TESTS_DIR}/ObjectCollectionTest/CMake" "${AUX_INCLUDE_DIR}/ObjectCollectionTest")
		add_subdirectory("${TESTS_DIR}/FileCollectionTest/CMake" "${AUX_INCLUDE_DIR}/FileCollectionTest")
		add_subdirectory("${TESTS_DIR}/RevisionControllerTest/CMake" "${AUX_INCLUDE_DIR}/RevisionControllerTest")
		add_subdirectory("${TESTS_DIR}/StructureTest/CMake" "${AUX_INCLUDE_DIR}/StructureTest")
		add_subdirectory("${TESTS_DIR}/PersonalAccessTokenTest/CMake" "${AUX_INCLUDE_DIR}/PersonalAccessTokenTest")
		file(COPY ${TESTS_DIR}/Migrations DESTINATION ${TESTS_DIR}/../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME})
	endif()

endif() # NOT ONLYTOOLS

