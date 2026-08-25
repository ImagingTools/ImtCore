project(ImtGqlPck LANGUAGES CXX)


message(VERBOSE "QT_VERSION_MAJOR ${QT_VERSION_MAJOR}")

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} PRIVATE
	imtgql
)

if(NOT DEFINED USE_QTQML OR USE_QTQML)
	target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::QuickWidgets)
endif()

