project(ImtDatabaseGqlPck LANGUAGES CXX)


message(VERBOSE "QT_VERSION_MAJOR ${QT_VERSION_MAJOR}")

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

if(NOT DEFINED USE_QTQML OR USE_QTQML)
	set(IMT_DATABASE_GQL_QT_LIBRARIES Qt${QT_VERSION_MAJOR}::QuickWidgets)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
	imtdbgql
	${IMT_DATABASE_GQL_QT_LIBRARIES}
)

