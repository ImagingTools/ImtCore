project(imtdb)


include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

if(AUTOMATIC_DB_BACKUPS)
	target_compile_definitions(${PROJECT_NAME} PRIVATE AUTOMATIC_DB_BACKUPS)
endif()

