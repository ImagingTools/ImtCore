project(ControlsGallery LANGUAGES CXX)

set(ARXC_CONFIG "${PROJECT_SOURCE_DIR}/../../../Config/ControlsGallery.awc")
set(ARXC_FILES "${PROJECT_SOURCE_DIR}/../${PROJECT_NAME}.acc")
set(ARXC_OUTDIR "${AUX_INCLUDE_DIR}")

if(WIN32)
#	set(CMAKE_WIN32_EXECUTABLE "on")
	set(ACF_CONVERT_FILES "${PROJECT_SOURCE_DIR}/../VC/ControlsGallery.rc.xtracf")
	set(ACF_CONVERT_REGISTRY "${PROJECT_SOURCE_DIR}/../VC/FileSubstitCopyApp.acc")
	set(ACF_CONVERT_CONFIG "${PROJECT_SOURCE_DIR}/../../../Config/BaseOnly.awc")
	set(RC_FILE "${AUX_INCLUDE_DIR}/${PROJECT_NAME}/ControlsGallery.rc")
endif()

if(APPLE)
	set(CMAKE_MACOSX_BUNDLE "on")
endif()

include(${ACFDIR}/Config/CMake/CustomBuild.cmake)
include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${IMTCOREDIR}/Config/CMake/Quazip.cmake)
include(${ACFDIR}/Config/CMake/DeployQt.cmake)

if(DEFINED WEB_COMPILE AND WEB_COMPILE)
	add_compile_definitions(WEB_COMPILE)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
	ImtCoreLoc
	controlsgallerysdl controlsgallerygql controlsgalleryqml
	imtappsdl imtauthsdl imtlicsdl imtcolorsdl imt2dsdl
	imtmail imtgui imtauth imtauthgui imtlicdb imtlic imtlicgui imtwidgets imtzip imt3dgui imtrepo imtstyle imtqml imtdb imtfile
	imtlicgql imtguigql imtclientgql imtgql imtauthgql imtauthdb imtcom imtservice imtlog imtservergql imtcol
	imtcontrolsqml imtstylecontrolsqml imtguiqml imtcolguiqml imtdocguiqml imtguigqlqml imtauthguiqml imtlicguiqml
	imtbase imtbasesdl imtserverapp imtapp imtrest imtcrypt imtchatdb imtdeskdb imtchat
	AcfSlnLoc
	AcfLoc iauth iservice imtdoc
	Qt${QT_VERSION_MAJOR}::QuickWidgets Qt${QT_VERSION_MAJOR}::WebSockets Qt${QT_VERSION_MAJOR}::Sql
)

include(${ACFDIR}/Config/CMake/AcfStdGui.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)

set(DEPLOY_QT_QML_DIRS"${IMTCOREDIR}/Qml")
deploy_qt(TARGET ${PROJECT_NAME})


if(WIN32)
	file(COPY ${IMTCOREDIR}/3rdParty/openssl/1.1/lib/x64/libcrypto-1_1-x64.dll DESTINATION ${IMTCOREDIR}/Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME})
	file(COPY ${IMTCOREDIR}/3rdParty/openssl/1.1/lib/x64/libssl-1_1-x64.dll DESTINATION ${IMTCOREDIR}/Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME})
endif()

include(${IMTCOREDIR}/Config/CMake/OpenSSL.cmake)
include(${IACFDIR}/Config/CMake/zlib.cmake)

