project(imtlicdb)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

find_package("Qt${QT_DEFAULT_MAJOR_VERSION}" COMPONENTS Sql REQUIRED)
target_link_libraries(${PROJECT_NAME} Qt${QT_DEFAULT_MAJOR_VERSION}::Sql)
target_link_libraries(${PROJECT_NAME} imtbase imtlic)
