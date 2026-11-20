project(imtfile LANGUAGES CXX)


include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

# The owner and the access list of the private files (CPrivateFilePersistenceComp)
if(WIN32)
	target_link_libraries(${PROJECT_NAME} PUBLIC advapi32)
endif()

