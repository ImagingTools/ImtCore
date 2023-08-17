project(ImtGqlPck LANGUAGES CXX)

message("QT_VERSION_MAJOR " ${QT_VERSION_MAJOR})


include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/AcfStdGui.cmake)

if(NOT DEFINED USE_QTQML OR USE_QTQML)
	find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS QuickWidgets REQUIRED)
	target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::QuickWidgets)
endif()

target_link_libraries(${PROJECT_NAME} imtbase imtgql imtrest imtcom)


