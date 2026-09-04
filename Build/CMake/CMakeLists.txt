cmake_minimum_required(VERSION 3.26)

project(ImtCore)


if(NOT DEFINED QT_VERSION_MAJOR)
	find_package(QT NAMES Qt6 Qt5 COMPONENTS Core)
	find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS Core Concurrent Gui Network PrintSupport Sql Svg Xml WebSockets Widgets REQUIRED)

	if(QT_VERSION_MAJOR EQUAL 6)
		find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS Core5Compat REQUIRED)
	endif()
endif()


if(NOT DEFINED USE_GEO OR USE_GEO)
	find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS Positioning REQUIRED)

	if(QT_VERSION_MAJOR EQUAL 5)
		find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS Location REQUIRED)
	endif()
endif()

if(NOT DEFINED USE_QTQML OR USE_QTQML)
	find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS Qml QuickControls2 QuickWidgets REQUIRED)
endif()

if(NOT DEFINED USE_QT3D OR USE_QT3D)
	find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS OpenGL OpenGLWidgets REQUIRED)
endif()

option(BUILD_TESTING "Configures projects for testing" OFF)
if(BUILD_TESTING)
	find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS Test REQUIRED)
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

# Enable modern target-based architecture:
# find_package(IAcf) provides Acf::/AcfSln::/IAcf:: imported targets with transitive include dirs and link deps.
# Legacy global include_directories()/link_directories() in the Env chain are skipped.
option(ACF_MODERN_CMAKE "Enable modern target-based architecture" ON)

option(AUTOMATIC_DB_BACKUPS "Enable automatic database backups" ON)
option(USE_MDBX "Use 3rdParty/libmdbx" OFF)

# Collect ImtCore libraries in their own export set so they are published as the
# ImtCore:: package. Read by StaticConfig.cmake / acf_register_library().
set(ACF_PACKAGE_NAME "ImtCore")

include(${IMTCOREDIR}/Config/CMake/ImtCoreEnv.cmake)

message(VERBOSE "ImtCore ACFDIR_BUILD ${ACFDIR_BUILD}")

set(INCLUDE_DIR "${IMTCOREDIR}/Include")
set(IMPL_DIR	"${IMTCOREDIR}/Impl")
set(QML_DIR		"${IMTCOREDIR}/Qml")
set(SDL_DIR		"${IMTCOREDIR}/Sdl")
set(TESTS_DIR	"${IMTCOREDIR}/Tests")
set(AUX_INCLUDE_DIR "${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles")

include_directories(${AUX_INCLUDE_DIR}/..)
include_directories(${AUX_INCLUDE_DIR})


#------ Required libs ------
add_subdirectory("${INCLUDE_DIR}/imtcore/CMake" "${AUX_INCLUDE_DIR}/imtcore")
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
add_dependencies(DesignTokenCreator imtdesign ImtDesignPck)
imt_chain_windeploy_target(DesignTokenCreator)

#------ DDLCodeCreator ------
add_subdirectory("${IMPL_DIR}/DdlCodeCreatorExe/CMake" "${AUX_INCLUDE_DIR}/DdlCodeCreator")
add_dependencies(DdlCodeCreator imtddl ImtDdlPck)
imt_chain_windeploy_target(DdlCodeCreator)

#------ SDL tool ------
add_subdirectory("${INCLUDE_DIR}/imtsdl/CMake" "${AUX_INCLUDE_DIR}/imtsdl")
add_subdirectory("${IMPL_DIR}/ImtSdlPck/CMake" "${AUX_INCLUDE_DIR}/ImtSdlPck")
add_dependencies(ImtSdlPck imtsdl imtfile imtbase)

add_subdirectory("${INCLUDE_DIR}/imtsdlgenqml/CMake" "${AUX_INCLUDE_DIR}/imtsdlgenqml")
add_subdirectory("${IMPL_DIR}/ImtSdlGenQmlPck/CMake" "${AUX_INCLUDE_DIR}/ImtSdlGenQmlPck")
add_dependencies(ImtSdlGenQmlPck imtsdlgenqml imtsdl imtfile imtbase)

add_subdirectory("${INCLUDE_DIR}/imtsdlgencpp/CMake" "${AUX_INCLUDE_DIR}/imtsdlgencpp")
add_subdirectory("${IMPL_DIR}/ImtSdlGenCppPck/CMake" "${AUX_INCLUDE_DIR}/ImtSdlGenCppPck")
add_dependencies(ImtSdlGenCppPck imtsdl imtsdlgencpp imtsdlgenqml imtfile imtbase)

add_subdirectory("${IMPL_DIR}/ImtCorePck/CMake" "${AUX_INCLUDE_DIR}/ImtCorePck")
add_dependencies(ImtCorePck imtbase imtfile)

add_subdirectory("${IMPL_DIR}/SdlCodeGeneratorExe/CMake" "${AUX_INCLUDE_DIR}/SdlCodeGenerator")
add_dependencies(SdlCodeGenerator ImtCorePck ImtSdlPck ImtSdlGenCppPck ImtSdlGenQmlPck)
imt_chain_windeploy_target(SdlCodeGenerator)

#------ SDL tool tests ------
if (BUILD_TESTING)
	add_subdirectory("${INCLUDE_DIR}/imtsdlgencpp/Test/CMake" "${AUX_INCLUDE_DIR}/ImtSdlGenCppTest")
	imt_chain_windeploy_target(ImtSdlGenTest)
	add_subdirectory("${INCLUDE_DIR}/imtsdlgenqml/Test/CMake" "${AUX_INCLUDE_DIR}/ImtSdlGenQmlTest")
	imt_chain_windeploy_target(ImtSdlGenQmlTest)
endif()

#------ OpenAPI ------
add_subdirectory("${INCLUDE_DIR}/imtoas/CMake" "${AUX_INCLUDE_DIR}/imtoas")
add_subdirectory("${IMPL_DIR}/ImtOasPck/CMake" "${AUX_INCLUDE_DIR}/ImtOasPck")
add_subdirectory("${IMPL_DIR}/OasCodeGeneratorExe/CMake" "${AUX_INCLUDE_DIR}/OasCodeGeneratorExe")
imt_chain_windeploy_target(OasCodeGeneratorExe)

if(NOT IMT_BUILD_TOOLS_ONLY)
	#-------- SDL generated libraries --------
	add_subdirectory("${SDL_DIR}/imtbase/CMake" "${AUX_INCLUDE_DIR}/imtbasesdl")
	add_dependencies(imtbasesdl SdlCodeGenerator)

	add_subdirectory("${SDL_DIR}/imt2d/CMake" "${AUX_INCLUDE_DIR}/imt2dsdl")
	add_dependencies(imt2dsdl imtcolorsdl)

	add_subdirectory("${SDL_DIR}/imtcolor/CMake" "${AUX_INCLUDE_DIR}/imtcolorsdl")
	add_dependencies(imtcolorsdl imtbasesdl)

	add_subdirectory("${SDL_DIR}/imtauth/CMake" "${AUX_INCLUDE_DIR}/imtauthsdl")
	add_dependencies(imtauthsdl imtbasesdl)

	add_subdirectory("${SDL_DIR}/imtlic/CMake" "${AUX_INCLUDE_DIR}/imtlicsdl")
	add_dependencies(imtlicsdl imtbasesdl)

	add_subdirectory("${SDL_DIR}/imtapp/CMake" "${AUX_INCLUDE_DIR}/imtappsdl")
	add_dependencies(imtappsdl imtbasesdl)

	add_subdirectory("${SDL_DIR}/imtpay/CMake" "${AUX_INCLUDE_DIR}/imtpaysdl")
	add_dependencies(imtpaysdl imtbasesdl)

	add_subdirectory("${SDL_DIR}/imtchat/CMake" "${AUX_INCLUDE_DIR}/imtchatsdl")
	add_dependencies(imtchatsdl imtbasesdl)

	if(NOT DEFINED USE_QTDB OR USE_QTDB)
		add_subdirectory("${SDL_DIR}/imtdesk/CMake" "${AUX_INCLUDE_DIR}/imtdesksdl")
		add_dependencies(imtdesksdl imtbasesdl)
	endif()

	#-------- Libraries --------
	add_subdirectory("${INCLUDE_DIR}/imtauth/CMake"			"${AUX_INCLUDE_DIR}/imtauth")
	add_subdirectory("${INCLUDE_DIR}/imtapp/CMake"			"${AUX_INCLUDE_DIR}/imtapp")
	add_subdirectory("${INCLUDE_DIR}/imtcrypt/CMake"		"${AUX_INCLUDE_DIR}/imtcrypt")
	add_subdirectory("${INCLUDE_DIR}/imtdesk/CMake"			"${AUX_INCLUDE_DIR}/imtdesk")
	add_subdirectory("${INCLUDE_DIR}/imtdev/CMake"			"${AUX_INCLUDE_DIR}/imtdev")
	add_subdirectory("${INCLUDE_DIR}/imtdevgui/CMake"		"${AUX_INCLUDE_DIR}/imtdevgui")
	add_subdirectory("${INCLUDE_DIR}/imtdoc/CMake"			"${AUX_INCLUDE_DIR}/imtdoc")
	add_subdirectory("${INCLUDE_DIR}/imthype/CMake"			"${AUX_INCLUDE_DIR}/imthype")
	add_subdirectory("${INCLUDE_DIR}/imthypegui/CMake"		"${AUX_INCLUDE_DIR}/imthypegui")
	add_subdirectory("${INCLUDE_DIR}/imtlic/CMake"			"${AUX_INCLUDE_DIR}/imtlic")
	add_subdirectory("${INCLUDE_DIR}/imtlog/CMake"			"${AUX_INCLUDE_DIR}/imtlog")
	add_subdirectory("${INCLUDE_DIR}/imtloggui/CMake"		"${AUX_INCLUDE_DIR}/imtloggui")
	add_subdirectory("${INCLUDE_DIR}/imtmail/CMake"			"${AUX_INCLUDE_DIR}/imtmail")
	add_subdirectory("${INCLUDE_DIR}/imtrepo/CMake"			"${AUX_INCLUDE_DIR}/imtrepo")
	add_subdirectory("${INCLUDE_DIR}/imtreport/CMake"		"${AUX_INCLUDE_DIR}/imtreport")
	add_subdirectory("${INCLUDE_DIR}/imtreportgui/CMake"	"${AUX_INCLUDE_DIR}/imtreportgui")
	add_subdirectory("${INCLUDE_DIR}/imtwidgets/CMake"		"${AUX_INCLUDE_DIR}/imtwidgets")
	add_subdirectory("${INCLUDE_DIR}/imtzip/CMake"			"${AUX_INCLUDE_DIR}/imtzip")

	add_subdirectory("${INCLUDE_DIR}/imtclientgql/CMake" "${AUX_INCLUDE_DIR}/imtclientgql")
	add_dependencies(imtclientgql imtbasesdl)

	add_subdirectory("${INCLUDE_DIR}/imtcol/CMake" "${AUX_INCLUDE_DIR}/imtcol")
	add_dependencies(imtcol imtbasesdl)

	add_subdirectory("${INCLUDE_DIR}/imtgui/CMake" "${AUX_INCLUDE_DIR}/imtgui")
	add_dependencies(imtgui DesignTokenCreator)

	add_subdirectory("${INCLUDE_DIR}/imtlicgui/CMake" "${AUX_INCLUDE_DIR}/imtlicgui")
	add_dependencies(imtlicgui DesignTokenCreator)

	add_subdirectory("${INCLUDE_DIR}/imtlicgql/CMake" "${AUX_INCLUDE_DIR}/imtlicgql")
	add_dependencies(imtlicgql imtlicsdl)

	add_subdirectory("${INCLUDE_DIR}/imtstyle/CMake" "${AUX_INCLUDE_DIR}/imtstyle")
	add_dependencies(imtstyle DesignTokenCreator)

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

		if(BUILD_TESTING)
			add_subdirectory("${INCLUDE_DIR}/imt3d/Test/CMake" "${AUX_INCLUDE_DIR}/imt3d/Test")
		endif()
	endif()

	if(NOT DEFINED USE_QTDB OR USE_QTDB)
		add_subdirectory("${INCLUDE_DIR}/imtauthdb/CMake"	"${AUX_INCLUDE_DIR}/imtauthdb")
		add_subdirectory("${INCLUDE_DIR}/imtchat/CMake"		"${AUX_INCLUDE_DIR}/imtchat")
		add_subdirectory("${INCLUDE_DIR}/imtchatdb/CMake"	"${AUX_INCLUDE_DIR}/imtchatdb")
		add_subdirectory("${INCLUDE_DIR}/imtcom/CMake"		"${AUX_INCLUDE_DIR}/imtcom")
		add_subdirectory("${INCLUDE_DIR}/imtdb/CMake"		"${AUX_INCLUDE_DIR}/imtdb")
		add_subdirectory("${INCLUDE_DIR}/imtdbgql/CMake"	"${AUX_INCLUDE_DIR}/imtdbgql")
		add_subdirectory("${INCLUDE_DIR}/imtdeskdb/CMake"	"${AUX_INCLUDE_DIR}/imtdeskdb")
		add_subdirectory("${INCLUDE_DIR}/imthttp/CMake"		"${AUX_INCLUDE_DIR}/imthttp")
		add_subdirectory("${INCLUDE_DIR}/imtgql/CMake"		"${AUX_INCLUDE_DIR}/imtgql")
		add_subdirectory("${INCLUDE_DIR}/imthypedb/CMake"	"${AUX_INCLUDE_DIR}/imthypedb")
		add_subdirectory("${INCLUDE_DIR}/imtlicdb/CMake"	"${AUX_INCLUDE_DIR}/imtlicdb")
		add_subdirectory("${INCLUDE_DIR}/imtrest/CMake"		"${AUX_INCLUDE_DIR}/imtrest")

		add_subdirectory("${INCLUDE_DIR}/imtauthgql/CMake" "${AUX_INCLUDE_DIR}/imtauthgql")
		add_dependencies(imtauthgql imtauthsdl)

		add_subdirectory("${INCLUDE_DIR}/imtdbgui/CMake" "${AUX_INCLUDE_DIR}/imtdbgui")
		add_dependencies(imtdbgui DesignTokenCreator)

		add_subdirectory("${INCLUDE_DIR}/imtguigql/CMake" "${AUX_INCLUDE_DIR}/imtguigql")
		add_dependencies(imtguigql DesignTokenCreator)

		add_subdirectory("${INCLUDE_DIR}/imtserverapp/CMake" "${AUX_INCLUDE_DIR}/imtserverapp")
		add_dependencies(imtserverapp imtbasesdl)

		add_subdirectory("${INCLUDE_DIR}/imtservergql/CMake" "${AUX_INCLUDE_DIR}/imtservergql")
		add_dependencies(imtservergql imtappsdl)

		if(NOT DEFINED USE_GEO OR USE_GEO)
			add_subdirectory("${INCLUDE_DIR}/imtgeo/CMake" "${AUX_INCLUDE_DIR}/imtgeo")
		endif()

	endif()

	add_subdirectory("${INCLUDE_DIR}/imt2d/CMake" "${AUX_INCLUDE_DIR}/imt2d")
	add_dependencies(imt2d imt2dsdl)

	if(USE_MDBX)
		add_subdirectory("${INCLUDE_DIR}/imtmdbx/CMake" "${AUX_INCLUDE_DIR}/imtmdbx")
	endif()

	if(BUILD_TESTING)
		add_subdirectory("${INCLUDE_DIR}/imt2d/Test/CMake" "${AUX_INCLUDE_DIR}/imt2d/Test")
	endif()

	if(NOT DEFINED USE_QTDB OR USE_QTDB)
		add_subdirectory("${INCLUDE_DIR}/imtservice/CMake" "${AUX_INCLUDE_DIR}/imtservice")
	endif()

	#-------- Qml --------
	if(NOT DEFINED USE_QTQML OR USE_QTQML)
		# TODO return it when new wrappers will be finished
		# add_subdirectory("${INCLUDE_DIR}/imtqml2d/CMake" "${AUX_INCLUDE_DIR}/imtqml2d")

		if(NOT DEFINED USE_QTDB OR USE_QTDB)
			add_subdirectory("${INCLUDE_DIR}/imtauthgui/CMake" "${AUX_INCLUDE_DIR}/imtauthgui")
			add_dependencies(imtauthgui DesignTokenCreator)
		endif()

		add_subdirectory("${INCLUDE_DIR}/imtchatgql/CMake" "${AUX_INCLUDE_DIR}/imtchatgql")
		add_dependencies(imtchatgql imtchatsdl)

		if(NOT DEFINED USE_QTDB OR USE_QTDB)
			add_subdirectory("${INCLUDE_DIR}/imtdeskgql/CMake" "${AUX_INCLUDE_DIR}/imtdeskgql")
			add_dependencies(imtdeskgql imtdesksdl)
		endif()

		add_subdirectory("${INCLUDE_DIR}/imtqml/CMake" "${AUX_INCLUDE_DIR}/imtqml")
		add_dependencies(imtqml imtbasesdl)

		add_subdirectory("${INCLUDE_DIR}/imtstylecontrolsqml/CMake" "${AUX_INCLUDE_DIR}/imtstylecontrolsqml")

		# QML libraries
		add_subdirectory("${QML_DIR}/imtauthgui/CMake"		"${AUX_INCLUDE_DIR}/imtauthguiqml")
		add_subdirectory("${QML_DIR}/imtchatgui/CMake"		"${AUX_INCLUDE_DIR}/imtchatguiqml")
		add_subdirectory("${QML_DIR}/imtcolgui/CMake"		"${AUX_INCLUDE_DIR}/imtcolguiqml")
		add_subdirectory("${QML_DIR}/imtcontrols/CMake"		"${AUX_INCLUDE_DIR}/imtcontrolsqml")
		add_subdirectory("${QML_DIR}/imtdeskgui/CMake"		"${AUX_INCLUDE_DIR}/imtdeskguiqml")
		add_subdirectory("${QML_DIR}/imtdocgui/CMake"		"${AUX_INCLUDE_DIR}/imtdocguiqml")
		add_subdirectory("${QML_DIR}/imtgeogui/CMake"		"${AUX_INCLUDE_DIR}/imtgeoguiqml")
		add_subdirectory("${QML_DIR}/imtgui/CMake"			"${AUX_INCLUDE_DIR}/imtguiqml")
		add_subdirectory("${QML_DIR}/imtguigql/CMake"		"${AUX_INCLUDE_DIR}/imtguigqlqml")
		add_subdirectory("${QML_DIR}/imtlicgui/CMake"		"${AUX_INCLUDE_DIR}/imtlicguiqml")
	endif()

	#-------- Packages --------
	if(NOT ANDROID)


		add_subdirectory("${IMPL_DIR}/ImtAppPck/CMake" "${AUX_INCLUDE_DIR}/ImtAppPck")
		add_dependencies(ImtAppPck imtapp)

		add_subdirectory("${IMPL_DIR}/ImtMailPck/CMake" "${AUX_INCLUDE_DIR}/ImtMailPck")
		add_dependencies(ImtMailPck imtmail)

		add_subdirectory("${IMPL_DIR}/ImtDevPck/CMake" "${AUX_INCLUDE_DIR}/ImtDevPck")
		add_dependencies(ImtDevPck imtbase imtdev)

		add_subdirectory("${IMPL_DIR}/ImtDevGuiPck/CMake" "${AUX_INCLUDE_DIR}/ImtDevGuiPck")
		add_dependencies(ImtDevGuiPck imtbase imtdevgui)

		add_subdirectory("${IMPL_DIR}/ImtDocPck/CMake" "${AUX_INCLUDE_DIR}/ImtDocPck")
		add_dependencies(ImtDocPck imtdoc)

		add_subdirectory("${IMPL_DIR}/ImtStylePck/CMake" "${AUX_INCLUDE_DIR}/ImtStylePck")
		add_dependencies(ImtStylePck imtstyle DesignTokenCreator)

		add_subdirectory("${IMPL_DIR}/ImtAuthPck/CMake" "${AUX_INCLUDE_DIR}/ImtAuthPck")
		add_dependencies(ImtAuthPck imtbase imtauth)

		add_subdirectory("${IMPL_DIR}/ImtDeskPck/CMake" "${AUX_INCLUDE_DIR}/ImtDeskPck")
		add_dependencies(ImtDeskPck imtbase imtdesk)

		if((NOT DEFINED USE_QTQML OR USE_QTQML) AND (NOT DEFINED USE_QTDB OR USE_QTDB))
			add_subdirectory("${IMPL_DIR}/ImtAuthGuiPck/CMake" "${AUX_INCLUDE_DIR}/ImtAuthGuiPck")
			add_dependencies(ImtAuthGuiPck imtbase imtauth imtauthgui imtgui)
		endif()

		if(USE_MDBX)
			add_subdirectory("${IMPL_DIR}/ImtMdbxPck/CMake" "${AUX_INCLUDE_DIR}/ImtMdbxPck")
			add_dependencies(ImtMdbxPck imtmdbx)
		endif()

		add_subdirectory("${IMPL_DIR}/ImtLicensePck/CMake" "${AUX_INCLUDE_DIR}/ImtLicensePck")
		add_dependencies(ImtLicensePck imtbase imtlic)

		add_subdirectory("${IMPL_DIR}/ImtLicenseGuiPck/CMake" "${AUX_INCLUDE_DIR}/ImtLicenseGuiPck")
		add_dependencies(ImtLicenseGuiPck imtbase imtlic imtlicgui)

		add_subdirectory("${IMPL_DIR}/ImtCryptPck/CMake" "${AUX_INCLUDE_DIR}/ImtCryptPck")
		add_dependencies(ImtCryptPck imtbase imtcrypt)

		add_subdirectory("${IMPL_DIR}/ImtReportGuiPck/CMake" "${AUX_INCLUDE_DIR}/ImtReportGuiPck")
		add_dependencies(ImtReportGuiPck imtreport imtreportgui)

		add_subdirectory("${IMPL_DIR}/ImtRepoPck/CMake" "${AUX_INCLUDE_DIR}/ImtRepoPck")
		add_dependencies(ImtRepoPck imtbase imtrepo)

		add_subdirectory("${IMPL_DIR}/ImtReportPck/CMake" "${AUX_INCLUDE_DIR}/ImtReportPck")
		add_dependencies(ImtReportPck imtbase imtreport)

		add_subdirectory("${IMPL_DIR}/ImtCoreLoc/CMake" "${AUX_INCLUDE_DIR}/ImtCoreLoc")

		add_subdirectory("${IMPL_DIR}/ImtLogPck/CMake" "${AUX_INCLUDE_DIR}/ImtLogPck")
		add_dependencies(ImtLogPck imtbase imtlog)

		add_subdirectory("${IMPL_DIR}/ImtZipPck/CMake" "${AUX_INCLUDE_DIR}/ImtZipPck")
		add_dependencies(ImtZipPck imtzip)
		imt_chain_windeploy_target(ImtZipPck)

		add_subdirectory("${IMPL_DIR}/ImtFilePck/CMake" "${AUX_INCLUDE_DIR}/ImtFilePck")
		add_dependencies(ImtFilePck imtbase imtfile)

		add_subdirectory("${IMPL_DIR}/ImtColPck/CMake" "${AUX_INCLUDE_DIR}/ImtColPck")
		add_dependencies(ImtColPck imtbase imtcol)

		add_subdirectory("${IMPL_DIR}/ImtHypePck/CMake" "${AUX_INCLUDE_DIR}/ImtHypePck")
		add_dependencies(ImtHypePck imtbase imthype)

		add_subdirectory("${IMPL_DIR}/ImtHypeGuiPck/CMake" "${AUX_INCLUDE_DIR}/ImtHypeGuiPck")
		add_dependencies(ImtHypeGuiPck imtwidgets imtgui imthype imthypegui)

		if((DEFINED USE_MONGODB AND USE_MONGODB) AND (NOT DEFINED USE_QTDB OR USE_QTDB))
			add_subdirectory("${IMPL_DIR}/ImtMongoPck/CMake" "${AUX_INCLUDE_DIR}/ImtMongoPck")
			add_dependencies(ImtMongoPck imtmongo imtdb)
			imt_chain_windeploy_target(ImtMongoPck)
		endif()

		if(NOT DEFINED USE_EJ OR USE_EJ)
			add_subdirectory("${IMPL_DIR}/ImtEjPck/CMake" "${AUX_INCLUDE_DIR}/ImtEjPck")
			add_dependencies(ImtEjPck imtej)
		endif()

		if(NOT DEFINED USE_QTDB OR USE_QTDB)
			add_subdirectory("${IMPL_DIR}/ImtAuthDbPck/CMake" "${AUX_INCLUDE_DIR}/ImtAuthDbPck")
			add_dependencies(ImtAuthDbPck imtbase imtdb imtauth imtauthdb)

			add_subdirectory("${IMPL_DIR}/ImtChatPck/CMake" "${AUX_INCLUDE_DIR}/ImtChatPck")
			add_dependencies(ImtChatPck imtbase imtchat)

			add_subdirectory("${IMPL_DIR}/ImtDatabasePck/CMake" "${AUX_INCLUDE_DIR}/ImtDatabasePck")
			add_dependencies(ImtDatabasePck imtbase imtauth imtcol imtdb)
			imt_chain_windeploy_target(ImtDatabasePck)

			add_subdirectory("${IMPL_DIR}/ImtClientGqlPck/CMake" "${AUX_INCLUDE_DIR}/ImtClientGqlPck")
			add_dependencies(ImtClientGqlPck imtbasesdl imtrest imtcom imtservergql imtclientgql)

			add_subdirectory("${IMPL_DIR}/ImtComPck/CMake" "${AUX_INCLUDE_DIR}/ImtComPck")
			add_dependencies(ImtComPck imtbase imtcom)
			imt_chain_windeploy_target(ImtComPck)

			add_subdirectory("${IMPL_DIR}/ImtHttpPck/CMake" "${AUX_INCLUDE_DIR}/ImtHttpPck")
			add_dependencies(ImtHttpPck imthttp)

			add_subdirectory("${IMPL_DIR}/ImtGqlPck/CMake" "${AUX_INCLUDE_DIR}/ImtGqlPck")
			add_dependencies(ImtGqlPck imtbase imtrest imtgql)

			add_subdirectory("${IMPL_DIR}/ImtLicenseDbPck/CMake" "${AUX_INCLUDE_DIR}/ImtLicenseDbPck")
			add_dependencies(ImtLicenseDbPck imtbase imtdb imtlicdb)

			add_subdirectory("${IMPL_DIR}/ImtLicenseGqlPck/CMake" "${AUX_INCLUDE_DIR}/ImtLicenseGqlPck")
			add_dependencies(ImtLicenseGqlPck imtbase imtlic imtdb imtlicgql)

			add_subdirectory("${IMPL_DIR}/ImtDatabaseGqlPck/CMake" "${AUX_INCLUDE_DIR}/ImtDatabaseGqlPck")
			add_dependencies(ImtDatabaseGqlPck imtbase imtdb imtgql imtservergql imtdbgql)

			add_subdirectory("${IMPL_DIR}/ImtHypeDbPck/CMake" "${AUX_INCLUDE_DIR}/ImtHypeDbPck")
			add_dependencies(ImtHypeDbPck imtbase imtdb imthype imthypedb)

			add_subdirectory("${IMPL_DIR}/ImtChatDbPck/CMake" "${AUX_INCLUDE_DIR}/ImtChatDbPck")
			add_dependencies(ImtChatDbPck imtbase imtdb imtchat imtchatdb)

			add_subdirectory("${IMPL_DIR}/ImtDeskDbPck/CMake" "${AUX_INCLUDE_DIR}/ImtDeskDbPck")
			add_dependencies(ImtDeskDbPck imtbase imtdb imtdesk imtdeskdb)

			add_subdirectory("${IMPL_DIR}/ImtGuiGqlPck/CMake" "${AUX_INCLUDE_DIR}/ImtGuiGqlPck")
			add_dependencies(ImtGuiGqlPck imtgui imtgql imtdb imtservergql imtguigql)

			add_subdirectory("${IMPL_DIR}/ImtRestPck/CMake" "${AUX_INCLUDE_DIR}/ImtRestPck")
			add_dependencies(ImtRestPck imtbase imtrest)
			imt_chain_windeploy_target(ImtRestPck)

			add_subdirectory("${IMPL_DIR}/ImtServerAppPck/CMake" "${AUX_INCLUDE_DIR}/ImtServerAppPck")
			add_dependencies(ImtServerAppPck imtdb imtgql imtserverapp)

			add_subdirectory("${IMPL_DIR}/ImtServicePck/CMake" "${AUX_INCLUDE_DIR}/ImtServicePck")
			add_dependencies(ImtServicePck imtbase imtcom imtservice)

			add_subdirectory("${IMPL_DIR}/ImtServerGqlPck/CMake" "${AUX_INCLUDE_DIR}/ImtServerGqlPck")
			add_dependencies(ImtServerGqlPck imtbasesdl imtcom imtauth imtrest imtserverapp imtdoc imthype imtservergql)

			if((NOT DEFINED USE_GEO OR USE_GEO) AND (NOT DEFINED USE_QTDB OR USE_QTDB))
				add_subdirectory("${IMPL_DIR}/ImtGeoPck/CMake" "${AUX_INCLUDE_DIR}/ImtGeoPck")
				add_dependencies(ImtGeoPck imtdb imtservergql imtgeo)
				imt_chain_windeploy_target(ImtGeoPck)
			endif()

			if(WIN32 AND (NOT DEFINED USE_JQML_HOST OR USE_JQML_HOST))
				add_subdirectory("${IMPL_DIR}/JQMLHost/CMake" "${AUX_INCLUDE_DIR}/JQMLHost")
				add_dependencies(JQMLHost
					imtbasesdl imtappsdl imtauthsdl imtlicsdl imtcolorsdl imt2dsdl imtchatsdl imtdesksdl imtpaysdl
					imtcontrolsqml imtstylecontrolsqml imtguiqml imtguigqlqml imtcolguiqml imtdocguiqml imtauthguiqml imtlicguiqml imtchatguiqml imtdeskguiqml
					imtqml imtgui imtguigql imtauth imtauthgui imtstyle imtbase
				)
				imt_chain_windeploy_target(JQMLHost)
			endif()
		endif()

		if(NOT DEFINED USE_QT3D OR USE_QT3D)
			add_subdirectory("${IMPL_DIR}/Imt3dPck/CMake" "${AUX_INCLUDE_DIR}/Imt3dPck")
			add_dependencies(Imt3dPck imt3d)

			add_subdirectory("${IMPL_DIR}/ImtGuiPck/CMake" "${AUX_INCLUDE_DIR}/ImtGuiPck")
			add_dependencies(ImtGuiPck imt3d imtgui)

			add_subdirectory("${IMPL_DIR}/ImtLogGuiPck/CMake" "${AUX_INCLUDE_DIR}/ImtLogGuiPck")
			add_dependencies(ImtLogGuiPck imtgui imtlog imtloggui)

			add_subdirectory("${IMPL_DIR}/Imt3dViewPck/CMake" "${AUX_INCLUDE_DIR}/Imt3dViewPck")
			add_dependencies(Imt3dViewPck imt3d imt3dview)

			add_subdirectory("${IMPL_DIR}/Imt3dGuiPck/CMake" "${AUX_INCLUDE_DIR}/Imt3dGuiPck")
			add_dependencies(Imt3dGuiPck imt3d imt3dview imt3dgui)
			imt_chain_windeploy_target(Imt3dGuiPck)

			add_subdirectory("${IMPL_DIR}/Imt3dDemo/CMake" "${AUX_INCLUDE_DIR}/Imt3dDemo")
			add_dependencies(Imt3dDemo imt3dgui imt3dview imt3d)
			imt_chain_windeploy_target(Imt3dDemo)
		endif()

		if(NOT DEFINED USE_QTQML OR USE_QTQML)
			if(NOT DEFINED USE_QTDB OR USE_QTDB)
				add_subdirectory("${IMPL_DIR}/ImtQmlPck/CMake" "${AUX_INCLUDE_DIR}/ImtQmlPck")
				add_dependencies(ImtQmlPck imtgui imtgql imtcom imtqml)
				imt_chain_windeploy_target(ImtQmlPck)

				add_subdirectory("${IMPL_DIR}/ImtAuthGqlPck/CMake" "${AUX_INCLUDE_DIR}/ImtAuthGqlPck")
				add_dependencies(ImtAuthGqlPck imtqml imtlic imtservergql imtserverapp imtclientgql imtdb imtdoc imtauthgql)

				add_subdirectory("${IMPL_DIR}/ImtChatGqlPck/CMake" "${AUX_INCLUDE_DIR}/ImtChatGqlPck")
				add_dependencies(ImtChatGqlPck imtchat imtchatsdl imtchatgql)

				add_subdirectory("${IMPL_DIR}/ImtDeskGqlPck/CMake" "${AUX_INCLUDE_DIR}/ImtDeskGqlPck")
				add_dependencies(ImtDeskGqlPck imtdb imtdesk imtdesksdl imtservergql imtdoc imtdeskgql)
				if (IMT_BUILD_CONTROLS_GALLERY)
					add_subdirectory("${IMPL_DIR}/ImtControlsGallery/Build/CMake" "${AUX_INCLUDE_DIR}/ImtControlsGallery")
					add_dependencies(ControlsGallery imtgui imtauthsdl imtlicsdl)
				endif() # IMT_BUILD_CONTROLS_GALLERY
			endif()
		endif()
	endif()

	#-------- Tests --------
	if (BUILD_TESTING)
		add_subdirectory("${INCLUDE_DIR}/imtbase/Test/CMake" "${AUX_INCLUDE_DIR}/imtbase/Test")
		imt_chain_windeploy_target(imtbaseTest)
		add_subdirectory("${INCLUDE_DIR}/imtfile/Test/CMake" "${AUX_INCLUDE_DIR}/imtfile/Test")
		imt_chain_windeploy_target(imtfileTest)
		add_subdirectory("${INCLUDE_DIR}/imtservergql/Test/CMake" "${AUX_INCLUDE_DIR}/imtservergql/Test")
		add_dependencies(imtservergqlTest imtservergql imtfile imtbasesdl)
		imt_chain_windeploy_target(imtservergqlTest)
		add_subdirectory("${INCLUDE_DIR}/imttest/CMake" "${AUX_INCLUDE_DIR}/imttest")

		if(NOT DEFINED USE_QTDB OR USE_QTDB)
			add_subdirectory("${IMPL_DIR}/ImtTestPck/CMake" "${AUX_INCLUDE_DIR}/ImtTestPck")
			add_dependencies(ImtTestPck imtbase imtdb imttest)

			add_subdirectory("${TESTS_DIR}/ObjectCollectionTest/CMake" "${AUX_INCLUDE_DIR}/ObjectCollectionTest")
			add_dependencies(ImtCoreObjectCollectionTest imtbase imtauth imtdb imttest)

			add_subdirectory("${TESTS_DIR}/StructureTest/CMake" "${AUX_INCLUDE_DIR}/StructureTest")
			add_dependencies(StructureTest imtbase imtauth imtdb imttest)
		endif()

		if(USE_MDBX)
			add_subdirectory("${INCLUDE_DIR}/imtmdbx/Test/CMake" "${AUX_INCLUDE_DIR}/imtmdbx/Test")
			imt_chain_windeploy_target(imtmdbxTest)
		endif()

		add_subdirectory("${INCLUDE_DIR}/imtgql/Test/CMake" "${AUX_INCLUDE_DIR}/imtgql/Test")
		add_dependencies(imtGqlTest imtbase imtgql imtbasesdl)
		imt_chain_windeploy_target(imtGqlTest)

		add_subdirectory("${INCLUDE_DIR}/imtgqltest/CMake" "${AUX_INCLUDE_DIR}/imtgqltest")
		add_dependencies(imtgqltest imtcom imtrest imtbase)

		add_subdirectory("${TESTS_DIR}/FileCollectionTest/CMake" "${AUX_INCLUDE_DIR}/FileCollectionTest")
		add_dependencies(ImtCoreFileCollectionTest imtbase imtauth imtrepo)

		add_subdirectory("${TESTS_DIR}/RevisionControllerTest/CMake" "${AUX_INCLUDE_DIR}/RevisionControllerTest")
		add_dependencies(ImtCoreRevisionControllerTest imtauth imtbase)

		add_subdirectory("${TESTS_DIR}/PersonalAccessTokenTest/CMake" "${AUX_INCLUDE_DIR}/PersonalAccessTokenTest")
		add_dependencies(PersonalAccessTokenTest imtbase imtauth)

		add_subdirectory("${TESTS_DIR}/CollectionDocumentServiceTest/CMake" "${AUX_INCLUDE_DIR}/CollectionDocumentServiceTest")
		add_dependencies(CollectionDocumentServiceTest imtbase imtdoc)

		file(COPY ${TESTS_DIR}/Migrations DESTINATION ${TESTS_DIR}/../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME})
	endif() # BUILD_TESTING
endif() # NOT IMT_BUILD_TOOLS_ONLY

# Declare target-based deps so include paths/link order propagate transitively across ImtCore
# libraries and onto the underlying Acf::/AcfSln::/IAcf:: libraries. Guarded on the modern
# packaging machinery (acf_register_library from Acf) so the file no-ops under the legacy shim.
include("${IMTCOREDIR}/Config/CMake/ImtCoreLibraryDependencies.cmake")

# Export ImtCore libraries as a find_package(ImtCore) package (build + install tree) so
# downstream modules consume them via ImtCore::<lib> targets instead of env vars/manual paths.
include("${IMTCOREDIR}/Config/CMake/ImtCorePackageExport.cmake")

