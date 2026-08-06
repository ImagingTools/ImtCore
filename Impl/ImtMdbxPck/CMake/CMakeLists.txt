project(ImtMdbxPck LANGUAGES CXX)


include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/Mdbx.cmake)

target_link_libraries(${PROJECT_NAME} ${ACF_PACKAGE_LINK_SCOPE}
	imtmdbx
)

if(WIN32)
	file(COPY ${IMTCOREDIR}/3rdParty/libmdbx/lib/${CMAKE_BUILD_TYPE}_${TARGETNAME}/mdbx.dll DESTINATION ${PROJECT_SOURCE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME})
elseif(APPLE)
	if("${CMAKE_OSX_ARCHITECTURES}" STREQUAL "arm64")
		file(COPY ${IMTCOREDIR}/3rdParty/libmdbx/lib/ClangOSX_arm64/libmdbx.a DESTINATION ${PROJECT_SOURCE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME})
	else()
		file(COPY ${IMTCOREDIR}/3rdParty/libmdbx/lib/ClangOSX_x64/libmdbx.a DESTINATION ${PROJECT_SOURCE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME})
	endif()
elseif(UNIX)
	file(COPY ${IMTCOREDIR}/3rdParty/libmdbx/lib/${COMPILER_CODE}/libmdbx.so DESTINATION ${PROJECT_SOURCE_DIR}/../../../Bin/${CMAKE_BUILD_TYPE}_${TARGETNAME})
endif()

