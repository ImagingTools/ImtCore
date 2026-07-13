project(imtgeo LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS Positioning REQUIRED)

if(QT_VERSION_MAJOR EQUAL 5)
	find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS Location REQUIRED)
endif()

