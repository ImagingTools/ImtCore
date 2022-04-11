project(imtzip LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include($ENV{IMTCOREDIR}/Config/CMake/Quazip.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

if(QT_VERSION_MAJOR EQUAL 6)
	find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS Core5Compat REQUIRED)
	target_link_libraries(${PROJECT_NAME}  Qt${QT_VERSION_MAJOR}::Core5Compat)
endif()
