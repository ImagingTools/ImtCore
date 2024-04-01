project(imtgqlrepo LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../../Config/CMake/ImtCoreSdl.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../../Config/CMake/DdlCreator.cmake)

find_package("Qt${QT_VERSION_MAJOR}" COMPONENTS Core REQUIRED)
target_link_libraries(${PROJECT_NAME} imtgql imtbase)

GenerateDdlCode(
	"imtgqlrepo"
	"imtgqlrepo"
	"${PROJECT_SOURCE_DIR}/../DDL"
	"${AUX_INCLUDE_DIR}/${PROJECT_NAME}/DDL"
)

GenerateSdl(
	"${CMAKE_CURRENT_LIST_DIR}/../SDL/General.sdl"
	"${AUX_INCLUDE_DIR}/${PROJECT_NAME}/SDL"
	"${PROJECT_NAME}::sdl"
)

