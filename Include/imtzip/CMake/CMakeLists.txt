project(imtzip LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)

message(VERBOSE "ENV IMTCOREDIR $ENV{IMTCOREDIR}")
include(${IMTCOREDIR}/Config/CMake/Quazip.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

if(QT_VERSION_MAJOR EQUAL 6)
	target_link_libraries(${PROJECT_NAME} ${ACF_QT_MODULE_LINK_SCOPE} Qt${QT_VERSION_MAJOR}::Core5Compat)
endif()
